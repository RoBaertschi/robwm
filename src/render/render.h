#define R_HASH_RECT_SIZE 64 // In pixels

struct R_State {
    Arena *arena;

    Slice<U8> framebuffer_memory;

    V2I32 framebuffer_dimensions;
    Color *framebuffer;

    U64 *curr_hashes;
    U64 *prev_hashes;
};

function R_State *r_new_state(V2I32 initial_dimensions);
function void r_select_state(R_State *state);
function void r_resize(V2I32 dimensions);

function V2I32 r_hash_rects_dimensions(void);
