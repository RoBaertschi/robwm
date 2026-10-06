#include "render_inc.h"
variable_global R_State *r_state = 0;

function R_State *r_new_state(V2I32 initial_dimensions) {
    Arena *arena = arena_alloc();
    auto state = arena_new<R_State>(arena);
    state->arena = arena;

    state->framebuffer_dimensions = initial_dimensions;
    state->framebuffer_dimensions.x = clamp_bot(1_i32, state->framebuffer_dimensions.x);
    state->framebuffer_dimensions.y = clamp_bot(1_i32, state->framebuffer_dimensions.y);

    state->framebuffer = arena_make_array<Color>(arena, cast(Int)(initial_dimensions.x * initial_dimensions.y));

    auto rect_dimensions = r_hash_rects_dimensions();
    auto hashes_size     = rect_dimensions.x * rect_dimensions.y;
    state->curr_hashes = arena_make_array<U64>(arena, hashes_size);
    state->prev_hashes = arena_make_array<U64>(arena, hashes_size);

    return state;
}

function void r_select_state(R_State *state) {
    r_state = state;
}

function V2I32 r_hash_rects_dimensions(void) {
    V2I32 result = {};
    for (U8 axis = Axis_X; axis < Axis__MAX; axis++) {
        result.v[axis] = (r_state->framebuffer_dimensions.v[axis] + (R_HASH_RECT_SIZE - 1)) / R_HASH_RECT_SIZE;
    }
    return result;
}
