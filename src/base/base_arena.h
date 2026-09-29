// X(Name, bit pos(0 based))
#define ARENA_FLAGS \
    X(No_Growing, 0)

enum Arena_Flag : U8 {
    #define X(name, pos) Glue(Arena_, name) = 1 << pos,
    ARENA_FLAGS
    #undef X
};

typedef U8 Arena_Flags;

struct Arena {
    Arena   *prev;
    Arena   *curr;
    Uint    base_pos;
    Uintptr data;
    Uint    used;
    Uint    reserved;
    Uint    commited;
    Uint    initial_commited;

    Arena_Flags flags;
};

function void out_of_memory(void);

function Arena *arena_alloc(Uint commited = Megabyte * 2, Uint reserved = Gigabyte, Arena_Flags flags = 0);
function void arena_destroy_single(Arena *arena);
function void arena_destroy(Arena *arena);
function Slice<U8> _arena_push_aligned_non_zeroed(Arena *a, Uint size, Uint alignment);
function Slice<U8> _arena_push_aligned(Arena *a, Uint size, Uint alignment);
function void arena_pop_to(Arena *arena, Uint pos);
function void arena_pop(Arena *arena, Int size);
function void arena_clear(Arena *arena);

template <typename T>
function T *arena_new(Arena *arena) {
    auto result = _arena_push_aligned(arena, sizeof(T), alignof(T));
    return cast(T*)result.data;
}

template <typename T>
function Slice<T> arena_make(Arena *arena, Int count) {
    Slice<T> result = {};

    auto result_data = _arena_push_aligned(arena, sizeof(T) * count, alignof(T));

    result.data = cast(T*)result_data.data;
    result.len  = count;

    return result;
}
