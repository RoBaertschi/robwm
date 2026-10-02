// Globals
variable_global String wl_global_kind_string_name[Wl_Global_Kind__MAX] = {
    #define X(_name, string_name) STR(string_name),
    #undef X
};

// Hooks

#define X(name, sc_name, _T)\
function void Glue(wl_set_hook_, sc_name)(Glue(Wl_Hook_, name) hook, void *data) {\
    Assert(!wl_state->hooks.sc_name);/* To catch early double sets of hooks. */\
    wl_state->hooks.sc_name = hook;\
    wl_state->hooks.Glue(sc_name, data) = data;\
}

WL_HOOKS
#undef X

#define X(_name, sc_name, T) \
function void Glue(wl_call_hook_, sc_name)(T hook_data) {\
    if (wl_state->hooks.sc_name) {\
        wl_state->hooks.sc_name(wl_state->hooks.Glue(sc_name, data), hook_data);\
    }\
}

WL_HOOKS
#undef X

// wl_registry listener

function void wl_registry_listener_global(void *data,
		       wl_registry *wl_registry,
		       U32 name,
		       char const *interface,
		       U32 version)
{
    Unused(data);
    Unused(wl_registry);

    Wl_Global *global = 0;
    if (wl_state->free_globals.first) {
        global = wl_state->free_globals.first;
        list_remove(&wl_state->free_globals, global);
        *global = {};
    } else {
        global = arena_new<Wl_Global>(wl_state->arena);
    }

    global->name = name;
    global->interface = arena_string_clone(wl_state->arena, string_from_cstring(interface));
    global->version = version;

    list_push(&wl_state->globals, global);

    #define IF_MATCHES_INTERFACE(interface_name) if (global->interface == string_from_cstring(Glue(interface_name, _interface).name))
    #define GLOBAL_CHECK_VERSION(interface_name, expected_version) \
        Stmt(if (global->version < expected_version) { \
            fail("Unsupported " #interface_name " version " FMT_U32 ", expected at least version " #expected_version ".", global->version); \
        })

    IF_MATCHES_INTERFACE(river_window_manager_v1) {
        GLOBAL_CHECK_VERSION(river_window_manager_v1, 5);
        global->kind = Wl_Global_Kind_River_Window_Manager;

        wl_state->window_manager = cast(river_window_manager_v1*)wl_registry_bind(wl_state->registry, global->name, &river_window_manager_v1_interface, 5);
        logger_debugf(wl_state->logger, "-> Found window manager.");
    } else IF_MATCHES_INTERFACE(river_xkb_bindings_v1) {
        GLOBAL_CHECK_VERSION(river_xkb_bindings_v1, 3);
        global->kind = Wl_Global_Kind_River_Xkb_Bindings;

        wl_state->xkb_bindings = cast(river_xkb_bindings_v1*)wl_registry_bind(wl_state->registry, global->name, &river_xkb_bindings_v1_interface, 3);
        logger_debugf(wl_state->logger, "-> Found xkb bindings.");
    } else IF_MATCHES_INTERFACE(wl_seat) {
        GLOBAL_CHECK_VERSION(wl_seat, 9);
        global->kind = Wl_Global_Kind_Wl_Seat;

        Wl_Seat *seat = 0;
        if (wl_state->free_seats.first) {
            seat = wl_state->free_seats.first;
            list_remove(&wl_state->free_seats, seat);
            *seat = {};
        } else {
           seat = arena_new<Wl_Seat>(wl_state->arena);
        }
        seat->seat   = cast(wl_seat *)wl_registry_bind(wl_state->registry, global->name, &wl_seat_interface, 9);
        seat->global = global;

        list_push(&wl_state->seats, seat);
        logger_debugf(wl_state->logger, "-> Found wl seat " FMT_U32, global->name);

        wl_call_hook_wl_seat_added(seat);
    }

    #undef IF_MATCHES_INTERFACE
    #undef GLOBAL_CHECK_VERSION
}

function void wl_registry_listener_global_remove(void *data,
		       wl_registry *wl_registry,
		       U32 name)
{
    Unused(data);
    Unused(wl_registry);

    Wl_Global *global = 0;

    DLLForEach(wl_state->globals.first, current_global) {
        if (current_global->name == name) {
            global = current_global;
            break;
        }
    }

    if (global) {
        switch (global->kind) {
        case Wl_Global_Kind_Wl_Seat:
            log_infof("Seat " FMT_U32 " got removed.", name);
            Assert(global->seat);
            wl_call_hook_wl_seat_removed(global->seat);
            wl_seat_release(global->seat->seat);
            break;
        case Wl_Global_Kind_River_Window_Manager:
        case Wl_Global_Kind_River_Xkb_Bindings:
            fail("Compositor removed essential global " FMT_STR "(name=" FMT_U32 ") and don't know what to do, killing myself.",
                FMT_STR_ARG(wl_global_kind_string_name[global->kind]),
                name);
            break;
        case Wl_Global_Kind__MAX:
        case Wl_Global_Kind_Unknown: // Note any global here is also not bound, so no release needed
            list_remove(&wl_state->globals, global);
            list_push(&wl_state->free_globals, global);
            logger_warnf(wl_state->logger, "Compositor removed unknown global " FMT_U32 ". Removing it too.", name);
            break;
        }
    } else {
        logger_errorf(wl_state->logger, "Could not find global " FMT_U32 " to remove it.", name);
    }
}

variable_global wl_registry_listener wl_registry_listener = {
    wl_registry_listener_global,
    wl_registry_listener_global_remove,
};

// General functions

function void wl_init(void) {
    auto arena = arena_alloc();
    wl_state = arena_new<Wl_State>(arena);
    wl_state->arena = arena;
    wl_state->logger = { STR("wayland") };

    wl_state->display = wl_display_connect(0);
    if (wl_state->display == 0) {
        fail("Could not connect to wayland display.");
    }

    wl_state->registry = wl_display_get_registry(wl_state->display);
    wl_registry_add_listener(wl_state->registry, &wl_registry_listener, 0);

    wl_display_roundtrip(wl_state->display);

    if (!wl_state->window_manager) {
        fail("Missing river_window_manager_v1 global.");
    }

    if (!wl_state->xkb_bindings) {
        fail("Missing river_xkb_bindings_v1 global.");
    }
}

function void wl_enter_loop(void) {
    log_infof("Entering infinite loop.");
    river_window_manager_v1_manage_dirty(wl_state->window_manager);
    while (wl_display_dispatch(wl_state->display) != -1) {}
}

function Wl_Seat_List *wl_get_seats(void) {
    return &wl_state->seats;
}

function river_window_manager_v1 *wl_get_window_manager(void) {
    return wl_state->window_manager;
}

function river_xkb_bindings_v1 *wl_get_xkb_bindings(void) {
    return wl_state->xkb_bindings;
}
