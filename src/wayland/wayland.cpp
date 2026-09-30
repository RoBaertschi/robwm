function void wl_global_list_push(Wl_Global_List *list, Wl_Global *global) {
    DLLPushBack(list->first, list->last, global);
    list->len += 1;
}

function void wl_global_list_remove(Wl_Global_List *list, Wl_Global *global) {
    DLLRemove(list->first, list->last, global);
    list->len -= 1;
}

function void wl_registry_listener_global(void *data,
		       wl_registry *wl_registry,
		       U32 name,
		       char const *interface,
		       U32 version)
{
    Unused(data);
    Unused(wl_registry);

    auto global = arena_new<Wl_Global>(wl_state->arena);
    global->name = name;
    global->interface = arena_string_clone(wl_state->arena, string_from_cstring(interface));
    global->version = version;

    wl_global_list_push(&wl_state->globals, global);
}

function void wl_registry_listener_global_remove(void *data,
		       wl_registry *wl_registry,
		       U32 name)
{
    Unused(data);
    Unused(wl_registry);
    Unused(name);
}

variable_global wl_registry_listener wl_registry_listener = {
    wl_registry_listener_global,
    wl_registry_listener_global_remove,
};

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

    DLLForEach(wl_state->globals.first, global) {
        logger_debugf(wl_state->logger, "Global " FMT_STR ": name=" FMT_U32 ", version=" FMT_U32, FMT_STR_ARG(global->interface), global->name, global->version);

        if (global->interface == string_from_cstring(river_window_manager_v1_interface.name)) {
            if (global->version < 5) {
                fail("Unsupported river_window_manager_v1 version " FMT_U32 ", expected at least version 5.", global->version);
            }

            wl_state->window_manager = cast(river_window_manager_v1*)wl_registry_bind(wl_state->registry, global->name, &river_window_manager_v1_interface, 5);
            logger_debugf(wl_state->logger, "-> Found window manager.");
        } else if (global->interface == string_from_cstring(river_xkb_bindings_v1_interface.name)) {
            if (global->version < 3) {
                fail("Unsupported river_xkb_bindings_v1 version " FMT_U32 ", expected at least version 3.", global->version);
            }

            wl_state->xkb_bindings = cast(river_xkb_bindings_v1*)wl_registry_bind(wl_state->registry, global->name, &river_xkb_bindings_v1_interface, 3);
            logger_debugf(wl_state->logger, "-> Found window manager.");
        }
    }

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

function river_window_manager_v1 *wl_get_window_manager(void) {
    return wl_state->window_manager;
}

function river_xkb_bindings_v1 *wl_get_xkb_bindings(void) {
    return wl_state->xkb_bindings;
}
