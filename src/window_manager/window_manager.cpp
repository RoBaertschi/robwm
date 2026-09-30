variable_global Wm_State *wm_state;

function void wm_river_window_manager_listener_unavailable(void *data,
               river_window_manager_v1 *river_window_manager)
{
    Unused(data);
    Unused(river_window_manager);

    logger_debugf(wm_state->logger, "Window manager is unavailable!");
}

function void wm_river_window_manager_listener_finished(void *data,
               river_window_manager_v1 *river_window_manager)
{
    Unused(data);
    Unused(river_window_manager);

    logger_debugf(wm_state->logger, "Window manager is finished!\n");
}

function void wm_river_window_manager_listener_manage_start(void *data,
               river_window_manager_v1 *river_window_manager)
{
    Unused(data);
    logger_debugf(wm_state->logger, "Window manager manage start.\n");
    river_window_manager_v1_manage_finish(river_window_manager);
}

function void wm_river_window_manager_listener_render_start(void *data,
               river_window_manager_v1 *river_window_manager)
{
    Unused(data);
    logger_debugf(wm_state->logger, "Window manager render start.\n");
    river_window_manager_v1_render_finish(river_window_manager);
}

function void wm_river_window_manager_listener_session_locked(void *data,
               river_window_manager_v1 *river_window_manager)
{
    Unused(data);
    Unused(river_window_manager);
}

function void wm_river_window_manager_listener_session_unlocked(void *data,
               river_window_manager_v1 *river_window_manager)
{
    Unused(data);
    Unused(river_window_manager);
}

function void wm_river_window_manager_listener_window(void *data,
               river_window_manager_v1 *river_window_manager,
               river_window_v1 *river_window)
{
    Unused(data);
    Unused(river_window_manager);
    Unused(river_window);
}

function void wm_river_window_manager_listener_output(void *data,
               river_window_manager_v1 *river_window_manager,
               river_output_v1 *river_output)
{
    Unused(data);
    Unused(river_window_manager);
    Unused(river_output);
}

function void wm_river_window_manager_listener_seat(void *data,
               river_window_manager_v1 *river_window_manager,
               river_seat_v1 *river_seat)
{
    Unused(data);
    Unused(river_window_manager);
    Unused(river_seat);
}

variable_global river_window_manager_v1_listener wm_river_window_manager_listener = {
    wm_river_window_manager_listener_unavailable,
    wm_river_window_manager_listener_finished,
    wm_river_window_manager_listener_manage_start,
    wm_river_window_manager_listener_render_start,
    wm_river_window_manager_listener_session_locked,
    wm_river_window_manager_listener_session_unlocked,
    wm_river_window_manager_listener_window,
    wm_river_window_manager_listener_output,
    wm_river_window_manager_listener_seat,
};

function void wm_init(void) {
    Arena *arena = arena_alloc();
    wm_state = arena_new<Wm_State>(arena);
    wm_state->arena = arena;
    wm_state->logger = { STR("wm") };

    river_window_manager_v1_add_listener(wl_get_window_manager(), &wm_river_window_manager_listener, 0);
}
