function V2I32 r_hash_rects_dimensions_from_dimensions(V2I32 dimensions) {
    V2I32 result = {};
    for (U8 axis = Axis_X; axis < Axis__MAX; axis++) {
        // NOTE(robin): make sure to over allocate, so ceil, not floor
        result.v[axis] = (dimensions.v[axis] + (R_HASH_RECT_SIZE - 1)) / R_HASH_RECT_SIZE;
    }
    return result;
}

function Int r_framebuffer_byte_size_from_dimensions(V2I32 dimensions) {
    return cast(Int)dimensions.x * cast(Int)dimensions.y * cast(Int)sizeof(Color);
}

function Int r_hash_rects_byte_size_from_rect_dimensions(V2I32 dimensions) {
    return cast(Int)dimensions.x * cast(Int)dimensions.y * cast(Int)sizeof(U64);
}

function Int r_hash_rects_byte_size_from_dimensions(V2I32 dimensions) {
    return r_hash_rects_byte_size_from_rect_dimensions(r_hash_rects_dimensions_from_dimensions(dimensions));
}

function R_Framebuffer_Layout_Info r_framebuffer_layout_info_from_dimensions(V2I32 dimensions) {
    R_Framebuffer_Layout_Info result = {};

    result.dimensions = dimensions;
    result.hash_rects_byte_size  = r_hash_rects_byte_size_from_dimensions(dimensions);
    result.framebuffer_byte_size = r_framebuffer_byte_size_from_dimensions(dimensions);
    result.byte_size = align_up(result.hash_rects_byte_size * 2 + result.framebuffer_byte_size, cast(Int)VIRTUAL_PAGE_SIZE);

    return result;
}


variable_global R_State *r_state = 0;

function R_State *r_new_state(V2I32 dimensions) {
    auto arena   = arena_alloc();
    auto state   = arena_new<R_State>(arena);
    state->arena = arena;

    r_init_state(state, dimensions);

    return state;
}

function void r_init_state(R_State *state, V2I32 dimensions) {
    state->framebuffer_dimensions = dimensions;
    state->framebuffer_dimensions.x = clamp_bot(1_i32, state->framebuffer_dimensions.x);
    state->framebuffer_dimensions.y = clamp_bot(1_i32, state->framebuffer_dimensions.y);

    auto info = r_framebuffer_layout_info_from_dimensions(state->framebuffer_dimensions);
    Virtual_Reserve_Result result = virtual_reserve_and_commit(cast(Uint)info.byte_size * 2);
    if (!result.ok) {
        out_of_memory();
    }

    r_init_state_with_framebuffer_memory(state, cast(U8 *)result.data, info.byte_size * 2, info);
}

function void r_init_state_with_framebuffer_memory(
    R_State *state,
    U8 *framebuffer_memory,
    Int framebuffer_memory_capacity,
    R_Framebuffer_Layout_Info info)
{
    state->framebuffer_memory_capacity = framebuffer_memory_capacity;
    state->framebuffer_memory = framebuffer_memory;

    state->framebuffer_dimensions = info.dimensions;
    state->framebuffer = cast(Color *)state->framebuffer_memory;
    state->hash_rects[0] = cast(U64*)(state->framebuffer_memory + info.framebuffer_byte_size);
    state->hash_rects[1] = cast(U64*)(state->framebuffer_memory + info.framebuffer_byte_size + info.hash_rects_byte_size);

    // NOTE(robin): Create a stable initial state

    MemoryZero(state->framebuffer_memory, cast(Uint)info.framebuffer_byte_size);

    // TODO(robin): simdfy this
    for (U64 *hash_rects = cast(U64 *)(state->framebuffer_memory + info.framebuffer_byte_size);
        hash_rects < cast(U64 *)(state->framebuffer_memory + info.framebuffer_byte_size + info.hash_rects_byte_size * 2);
        hash_rects++)
    {
        *hash_rects = R_HASH_RECT_INITIAL_HASH;
    }
}

function void r_select_state(R_State *state) {
    r_state = state;
}

function void r_resize(V2I32 dimensions) {
    auto info = r_framebuffer_layout_info_from_dimensions(dimensions);
    if (info.byte_size <= r_state->framebuffer_memory_capacity) {
        // NOTE(robin): already enough capacity
        r_init_state_with_framebuffer_memory(r_state, r_state->framebuffer_memory, r_state->framebuffer_memory_capacity, info);
    } else {
        Virtual_Reserve_Result result = virtual_reserve_and_commit(cast(Uint)info.byte_size * 2);
        if (!result.ok) {
            log_errorf("Could not allocate bigger framebuffer, not enough memory.");
            return;
        }

        r_init_state_with_framebuffer_memory(r_state, cast(U8 *)result.data, info.byte_size * 2, info);
    }
}


function void r_frame(void) {

}
