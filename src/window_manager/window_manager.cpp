variable_global WM_State *wm_state;

function WM_String_Part *wm_new_string_part(void) {
    auto string_part = list_pop_front(&wm_state->free_string_parts);
    if (!string_part) {
        string_part = arena_new<WM_String_Part>(wm_state->arena);
    }
    return string_part;
}

function WM_String_Parts wm_string_parts_from_cstring(char const *cstring) {
    return wm_string_parts_from_string(string_from_cstring(cstring));
}

function WM_String_Parts wm_string_parts_from_string(String string) {
    Int part_count = string.len / WM_STRING_PART_SIZE;
    if (string.len % WM_STRING_PART_SIZE != 0) {
        part_count += 1;
    }

    WM_String_Parts parts = {};

    for (Int i = 0; i < part_count; i++) {
        Int index = i * WM_STRING_PART_SIZE;
        auto new_string_part = wm_new_string_part();

        Int copy_len = clamp_top(
                        clamp_bot(0_int, string.len - index),
                        cast(Int)WM_STRING_PART_SIZE);

        MemoryCopy(new_string_part->buffer, string.data + index, cast(Uint)copy_len);

        new_string_part->len = copy_len;
        list_push(&parts, new_string_part);
    }

    return parts;
}

function void wm_release_string_parts(WM_String_Parts parts) {
    DLLForEachSafe(parts.first, part) {
        list_remove(&parts, part);
        *part = {};
        list_push(&wm_state->free_string_parts, part);
    }
}

function String wm_string_from_string_parts(Arena *arena, WM_String_Parts parts) {
    auto string_data = arena_make_array<U8>(arena, parts.len * WM_STRING_PART_SIZE);

    Int i = 0;
    DLLForEach(parts.first, part) {
        Uint len_to_copy = cast(Uint)clamp_bot(cast(Int)WM_STRING_PART_SIZE, part->len);
        MemoryCopy(string_data + i, part->buffer, len_to_copy);
        i += cast(Int)len_to_copy;
    }

    arena_pop(arena, (parts.len * WM_STRING_PART_SIZE) - i);
    return {
        string_data,
        i,
    };
}

function void wm_push_command(WM_Command command) {
    auto command_node = list_pop_front(&wm_state->free_commands);
    if (!command_node) {
        command_node = arena_new<WM_Command_Node>(wm_state->arena);
    }

    command_node->command = command;

    list_push(&wm_state->commands, command_node);
}

function void wm_river_window_listener_closed(
    void *data,
    river_window_v1 *river_window)
{
    Unused(river_window);
    auto window = cast(WM_Window *)data;
    wm_push_command({ WM_Command_Manage_Window_Closed, window });
}

function void wm_river_window_listener_dimensions_hint(
    void *data,
    river_window_v1 *river_window,
    I32 min_width,
    I32 min_height,
    I32 max_width,
    I32 max_height)
{
    Unused(data);
    Unused(river_window);
    Unused(min_width);
    Unused(min_height);
    Unused(max_width);
    Unused(max_height);
}

function void wm_river_window_listener_dimensions(
    void *data,
    river_window_v1 *river_window,
    I32 width,
    I32 height)
{
    Unused(data);
    Unused(river_window);
    Unused(width);
    Unused(height);
}

function void wm_river_window_listener_app_id(
    void *data,
    river_window_v1 *river_window,
    char const *app_id)
{
    Unused(river_window);

    auto window = cast(WM_Window *)data;
    wm_release_string_parts(window->app_id);
    window->app_id = wm_string_parts_from_cstring(app_id);
}

function void wm_river_window_listener_title(
    void *data,
    river_window_v1 *river_window,
    char const *title)
{
    Unused(river_window);

    auto window = cast(WM_Window *)data;
    wm_release_string_parts(window->title);
    window->title = wm_string_parts_from_cstring(title);

    auto temp = TEMP_ARENA_GUARD();
    logger_debugf(wm_state->logger,
        "Got window title " FMT_STR ".",
        FMT_STR_ARG(wm_string_from_string_parts(temp.arena, window->title)));
}

function void wm_river_window_listener_parent(
    void *data,
    river_window_v1 *river_window,
    river_window_v1 *parent)
{
    Unused(river_window);
    Unused(parent);

    auto window = cast(WM_Window *)data;

    DLLForEach(wm_state->windows.first, current_window) {
        if (current_window->window == parent) {
            window->parent = current_window;
            break;
        }
    }

    if (!window->parent) {
        logger_errorf(wm_state->logger, "Could not find parent for window.");
    }
}

function void wm_river_window_listener_decoration_hint(
    void *data,
    river_window_v1 *river_window,
    U32 hint)
{
    Unused(data);
    Unused(river_window);
    Unused(hint);
}

function void wm_river_window_listener_pointer_move_requested(
    void *data,
    river_window_v1 *river_window,
    river_seat_v1 *seat)
{
    Unused(data);
    Unused(river_window);
    Unused(seat);
}

function void wm_river_window_listener_pointer_resize_requested(
    void *data,
    river_window_v1 *river_window,
    river_seat_v1 *seat,
    U32 edges)
{
    Unused(data);
    Unused(river_window);
    Unused(seat);
    Unused(edges);
}

function void wm_river_window_listener_show_window_menu_requested(
    void *data,
    river_window_v1 *river_window,
    I32 x,
    I32 y)
{
    Unused(data);
    Unused(river_window);
    Unused(x);
    Unused(y);
}

function void wm_river_window_listener_maximize_requested(
    void *data,
    river_window_v1 *river_window)
{
    Unused(data);
    Unused(river_window);
}

function void wm_river_window_listener_unmaximize_requested(
    void *data,
    river_window_v1 *river_window)
{
    Unused(data);
    Unused(river_window);
}

function void wm_river_window_listener_fullscreen_requested(
    void *data,
    river_window_v1 *river_window,
    river_output_v1 *river_output)
{
    Unused(data);
    Unused(river_window);
    Unused(river_output);
}

function void wm_river_window_listener_exit_fullscreen_requested(
    void *data,
    river_window_v1 *river_window)
{
    Unused(data);
    Unused(river_window);
}

function void wm_river_window_listener_minimize_requested(
    void *data,
    river_window_v1 *river_window)
{
    Unused(data);
    Unused(river_window);
}

function void wm_river_window_listener_unreliable_pid(
    void *data,
    river_window_v1 *river_window,
    I32 pid)
{
    Unused(data);
    Unused(river_window);
    Unused(pid);
}

function void wm_river_window_listener_presentation_hint(
    void *data,
    river_window_v1 *river_window,
    U32 hint)
{
    Unused(data);
    Unused(river_window);
    Unused(hint);
}

function void wm_river_window_listener_identifier(
    void *data,
    river_window_v1 *river_window,
    char const *identifier)
{
    Unused(data);
    Unused(river_window);
    Unused(identifier);
}

function void wm_river_window_listener_capture_sessions(
    void *data,
    river_window_v1 *river_window,
    U32 count)
{
    Unused(data);
    Unused(river_window);
    Unused(count);
}

variable_global_readonly river_window_v1_listener wm_river_window_listener = {
    wm_river_window_listener_closed,
    wm_river_window_listener_dimensions_hint,
    wm_river_window_listener_dimensions,
    wm_river_window_listener_app_id,
    wm_river_window_listener_title,
    wm_river_window_listener_parent,
    wm_river_window_listener_decoration_hint,
    wm_river_window_listener_pointer_move_requested,
    wm_river_window_listener_pointer_resize_requested,
    wm_river_window_listener_show_window_menu_requested,
    wm_river_window_listener_maximize_requested,
    wm_river_window_listener_unmaximize_requested,
    wm_river_window_listener_fullscreen_requested,
    wm_river_window_listener_exit_fullscreen_requested,
    wm_river_window_listener_minimize_requested,
    wm_river_window_listener_unreliable_pid,
    wm_river_window_listener_presentation_hint,
    wm_river_window_listener_identifier,
    wm_river_window_listener_capture_sessions,
};

function void wm_river_seat_listener_removed(void *data,
               river_seat_v1 *river_seat)
{
    Unused(river_seat);

    auto seat = cast(WM_Seat *)data;
    bm_remove_seat(seat->river_seat);

    river_seat_v1_destroy(seat->river_seat);

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
        auto window  = command.window;

        Bool handled = false;

        switch (command.kind) {
        case WM_Command_Manage_Window_Added: {
            river_window_v1_propose_dimensions(window->window, 0, 0);
            handled = true;
            break;
        case WM_Command_Manage_Window_Closed:
            logger_debugf(wm_state->logger, "Window closed.");

            list_remove(&wm_state->windows, window);

            wm_release_string_parts(window->title);
            wm_release_string_parts(window->app_id);

            river_window_v1_destroy(window->window);
            *window = {};
            list_push(&wm_state->free_windows, window);
            handled = true;
            break;
        }
        case WM_Command__MAX: break;
        }

        if (handled) {
            list_remove(&wm_state->commands, command_node);
            list_push(&wm_state->free_commands, command_node);
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

    river_window_v1_add_listener(river_window, &wm_river_window_listener, window);

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
