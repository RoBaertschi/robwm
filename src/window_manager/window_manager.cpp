variable_global WM_State *wm_state;

function void wm_push_command(WM_Command command) {
    auto command_node = list_pop_front(&wm_state->free_commands);
    if (!command_node) {
        command_node = arena_new<WM_Command_Node>(wm_state->arena);
    }

    command_node->command = command;

    list_push(&wm_state->commands, command_node);
}

function void wm_river_seat_listener_removed(void *data,
               river_seat_v1 *river_seat)
{
    Unused(river_seat);

    auto seat = cast(WM_Seat *)data;
    bm_remove_seat(seat->river_seat);

    list_remove(&wm_state->seats, seat);
    (*seat) = {}; // clear it
    list_push(&wm_state->free_seats, seat);
}

function void wm_river_seat_listener_wl_seat(void *data,
               river_seat_v1 *river_seat,
               U32 name)
{
    Unused(river_seat);
    auto seat = cast(WM_Seat *)data;

    auto seats = wl_get_seats();
    DLLForEach(seats->first, wl_seat) {
        if (wl_seat->global->name == name) {
            seat->seat = { wl_seat };
            logger_debugf(wm_state->logger, "Found wl_seat " FMT_U32 " for river_seat.", name);
            break;
        }
    }

    if (maybe_is_set(seat->seat)) {
        list_remove(&wm_state->unnamed_seats, seat);
        list_push(&wm_state->seats, seat);

        // TODO(robin): if more are needed, maybe add a hook to the system instead
        bm_add_seat(seat->river_seat);
    } else {
        logger_errorf(wm_state->logger, "Could not find matching wl_seat " FMT_U32 " for a river_seat.", name);
    }
}

function void wm_river_seat_listener_pointer_enter(void *data,
               river_seat_v1 *river_seat,
               river_window_v1 *window)
{
    Unused(data);
    Unused(river_seat);
    Unused(window);
}

function void wm_river_seat_listener_pointer_leave(void *data,
               river_seat_v1 *river_seat)
{
    Unused(data);
    Unused(river_seat);
}

function void wm_river_seat_listener_window_interaction(void *data,
               river_seat_v1 *river_seat,
               river_window_v1 *river_window)
{
    Unused(data);
    Unused(river_seat);
    Unused(river_window);
}

function void wm_river_seat_listener_shell_surface_interaction(void *data,
               river_seat_v1 *river_seat,
               river_shell_surface_v1 *shell_surface)
{
    Unused(data);
    Unused(river_seat);
    Unused(shell_surface);
}

function void wm_river_seat_listener_op_delta(void *data,
               river_seat_v1 *river_seat,
               I32 dx,
               I32 dy)
{
    Unused(data);
    Unused(river_seat);
    Unused(dx);
    Unused(dy);
}

function void wm_river_seat_listener_op_release(void *data,
               river_seat_v1 *river_seat)
{
    Unused(data);
    Unused(river_seat);
}

function void wm_river_seat_listener_pointer_position(void *data,
               river_seat_v1 *river_seat,
               I32 x,
               I32 y)
{
    Unused(data);
    Unused(river_seat);
    Unused(x);
    Unused(y);
}

variable_global_readonly river_seat_v1_listener wm_river_seat_listener = {
    wm_river_seat_listener_removed,
    wm_river_seat_listener_wl_seat,
    wm_river_seat_listener_pointer_enter,
    wm_river_seat_listener_pointer_leave,
    wm_river_seat_listener_window_interaction,
    wm_river_seat_listener_shell_surface_interaction,
    wm_river_seat_listener_op_delta,
    wm_river_seat_listener_op_release,
    wm_river_seat_listener_pointer_position,
};

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

    logger_debugf(wm_state->logger, "Window manager is finished!");
}

function void wm_river_window_manager_listener_manage_start(void *data,
               river_window_manager_v1 *river_window_manager)
{
    Unused(data);
    logger_debugf(wm_state->logger, "Window manager manage start.");

    WM_Command_Node *next = 0;
    for(auto command_node = wm_state->commands.first; command_node; command_node = next) {
        next = command_node->next;

        auto command = command_node->command;

        Bool handled = false;

        switch (command.kind) {
        case WM_Command_Manage_Window_Added: {
            river_window_v1_propose_dimensions(command.window->window, 0, 0);
            handled = true;
            break;
        }
        case WM_Command__MAX: break;
        }

        if (handled) {
            list_remove(&wm_state->commands, command_node);
        }
    }

    logger_debugf(wm_state->logger, "Window manager manage finished.");
    river_window_manager_v1_manage_finish(river_window_manager);
}

function void wm_river_window_manager_listener_render_start(void *data,
               river_window_manager_v1 *river_window_manager)
{
    Unused(data);
    logger_debugf(wm_state->logger, "Window manager render start.");
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

    auto window = list_pop_front(&wm_state->free_windows);
    if (!window) {
        window = arena_new<WM_Window>(wm_state->arena);
    }

    window->window = river_window;

    list_push(&wm_state->windows, window);
    logger_debugf(wm_state->logger, "New window.");

    wm_push_command({ WM_Command_Manage_Window_Added, window });
}

function void wm_river_window_manager_listener_output(void *data,
               river_window_manager_v1 *river_window_manager,
               river_output_v1 *river_output)
{
    Unused(data);
    Unused(river_window_manager);

    auto output = list_pop_front(&wm_state->free_outputs);
    if (!output) {
        output = arena_new<WM_Output>(wm_state->arena);
    }

    output->output = river_output;

    list_push(&wm_state->outputs, output);
}

function void wm_river_window_manager_listener_seat(void *data,
               river_window_manager_v1 *river_window_manager,
               river_seat_v1 *river_seat)
{
    Unused(data);
    Unused(river_window_manager);

    WM_Seat *seat = list_pop_front(&wm_state->free_seats);
    if (!seat) {
        seat = arena_new<WM_Seat>(wm_state->arena);
    }
    seat->river_seat = river_seat;
    list_push(&wm_state->unnamed_seats, seat);

    river_seat_v1_add_listener(seat->river_seat, &wm_river_seat_listener, seat);

    logger_debugf(wm_state->logger, "Got new river seat");
}

variable_global_readonly river_window_manager_v1_listener wm_river_window_manager_listener = {
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
    wm_state = arena_new<WM_State>(arena);
    wm_state->arena = arena;
    wm_state->logger = { STR("wm") };

    wl_set_hook_wl_seat_removed([](void *data, Wl_Seat *wl_seat) {
        Unused(data);

        WM_Seat *seat = 0;
        DLLForEach(wm_state->seats.first, current_seat) {
            if (maybe_get_or_null(current_seat->seat) == wl_seat) {
                seat = current_seat;
                break;
            }
        }

        seat->seat = {};
        logger_debugf(wm_state->logger, "Removed wl seat.");
    }, 0);

    river_window_manager_v1_add_listener(wl_get_window_manager(), &wm_river_window_manager_listener, 0);
}
