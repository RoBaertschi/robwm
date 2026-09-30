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

function void wl_river_window_manager_listener_unavailable(void *data,
               river_window_manager_v1 *river_window_manager)
{
    Unused(data);
    Unused(river_window_manager);

    logger_debugf(wl_state->logger, "Window manager is unavailable!");
}

function void wl_river_window_manager_listener_finished(void *data,
               river_window_manager_v1 *river_window_manager)
{
    Unused(data);
    Unused(river_window_manager);

    logger_debugf(wl_state->logger, "Window manager is finished!\n");
}

function void wl_river_window_manager_listener_manage_start(void *data,
               river_window_manager_v1 *river_window_manager)
{
    Unused(data);
    Unused(river_window_manager);

    logger_debugf(wl_state->logger, "Window manager manage start.\n");
}

function void wl_river_window_manager_listener_render_start(void *data,
               river_window_manager_v1 *river_window_manager)
{
    Unused(data);
    Unused(river_window_manager);
    logger_debugf(wl_state->logger, "Window manager render start.\n");
}

function void wl_river_window_manager_listener_session_locked(void *data,
               river_window_manager_v1 *river_window_manager)
{
    Unused(data);
    Unused(river_window_manager);
}

function void wl_river_window_manager_listener_session_unlocked(void *data,
               river_window_manager_v1 *river_window_manager)
{
    Unused(data);
    Unused(river_window_manager);
}

function void wl_river_window_manager_listener_window(void *data,
               river_window_manager_v1 *river_window_manager,
               river_window_v1 *river_window)
{
    Unused(data);
    Unused(river_window_manager);
    Unused(river_window);
}

function void wl_river_window_manager_listener_output(void *data,
               river_window_manager_v1 *river_window_manager,
               river_output_v1 *river_output)
{
    Unused(data);
    Unused(river_window_manager);
    Unused(river_output);
}

function void wl_river_window_manager_listener_seat(void *data,
               river_window_manager_v1 *river_window_manager,
               river_seat_v1 *river_seat)
{
    Unused(data);
    Unused(river_window_manager);
    Unused(river_seat);
}

variable_global river_window_manager_v1_listener wl_river_window_manager_listener = {
    wl_river_window_manager_listener_unavailable,
    wl_river_window_manager_listener_finished,
    wl_river_window_manager_listener_manage_start,
    wl_river_window_manager_listener_render_start,
    wl_river_window_manager_listener_session_locked,
    wl_river_window_manager_listener_session_unlocked,
    wl_river_window_manager_listener_window,
    wl_river_window_manager_listener_output,
    wl_river_window_manager_listener_seat,
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
            river_window_manager_v1_add_listener(wl_state->window_manager, &wl_river_window_manager_listener, 0);
            logger_debugf(wl_state->logger, "-> Found and bound window manager.");
        }
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
