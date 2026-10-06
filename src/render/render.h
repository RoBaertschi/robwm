#define R_HASH_RECT_SIZE 64 // In pixels

struct R_Framebuffer_Layout_Info {
    V2I32 dimensions;
    Int   framebuffer_byte_size;
    Int   hash_rects_byte_size;
    Int   byte_size;
};

struct R_State {
    Int framebuffer_memory_capacity;
    U8  *framebuffer_memory;

    V2I32 framebuffer_dimensions;
    Color *framebuffer;

    U64 *hash_rects[2];
};

// Allocate the new state on `arena`.
// This only allocates the R_State, the framebuffer memory is allocated via virtual memory directly.
function R_State *r_new_state(Arena *arena, V2I32 initial_dimensions);
// Same as r_new_state but without allocating the state on the arena
function void r_init_state(R_State *state, V2I32 initial_dimensions);
function void r_init_state_with_framebuffer_memory(
    R_State *state,
    U8 *framebuffer_memory,
    Int framebuffer_memory_capacity,
    R_Framebuffer_Layout_Info info);
function void r_select_state(R_State *state);
function void r_resize(V2I32 dimensions);

function V2I32 r_hash_rects_dimensions_from_dimensions(V2I32 dimensions);
