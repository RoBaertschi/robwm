function void out_of_memory(void) {
    printf("Out of memory. Crashing for debug break point.");
    AssertBreak();
}

function Arena *arena_alloc(Uint commited, Uint reserved, Arena_Flags flags) {
    commited = align_up(commited, cast(Uint)VIRTUAL_PAGE_SIZE);
    reserved = align_up(reserved, cast(Uint)VIRTUAL_PAGE_SIZE);

    Arena a = {};
    a.reserved = reserved;
    a.commited = commited;
    a.initial_commited = commited;

    Virtual_Reserve_Result result = virtual_reserve(reserved);
    if (!result.ok) {
        out_of_memory();
    }

    if (!virtual_commit(result.data, commited)) {
        out_of_memory();
    }

    a.data = cast(Uintptr)result.data;
    a.curr = &a;

    a.flags = Arena_No_Growing;

    auto arena = arena_new<Arena>(&a);
    *arena     = a;

    arena->flags = flags;
    arena->curr  = arena;
    Assert(arena->prev == 0);

    return arena;
}

function void arena_destroy_single(Arena *arena) {
    virtual_release(cast(void*)arena->data, arena->reserved);
}

function void arena_destroy(Arena *arena) {
    auto prev = arena->curr->prev;
    for (auto curr = arena->curr; curr != 0; curr = prev) {
        prev = curr->prev;
        arena_destroy_single(arena);
    }
}

function Uint _arena_align_used(Arena *a, Uint alignment) {
    return cast(Uint)align_up(cast(Uintptr)a->used + a->data, cast(Uintptr)alignment) - a->data;
}

function Slice<U8> _arena_push_aligned_non_zeroed(Arena *a, Uint size, Uint alignment) {
    Slice<U8> result = {};

    if (size == 0) {
        return result;
    }

    auto curr = a->curr;
    auto curr_used_aligned = _arena_align_used(curr, alignment);

    if (curr_used_aligned + size > curr->reserved) {
        if (a->flags & Arena_No_Growing) {
            out_of_memory();
        }

        Uint reserved = align_up(clamp_bot(size + sizeof(Arena), curr->reserved),         cast(Uint)VIRTUAL_PAGE_SIZE);
        Uint commited = align_up(clamp_bot(size + sizeof(Arena), curr->initial_commited), cast(Uint)VIRTUAL_PAGE_SIZE);

        auto new_arena = arena_alloc(commited, reserved, a->flags);
        new_arena->base_pos = curr->base_pos + curr->used;
        new_arena->prev     = curr;

        curr_used_aligned = _arena_align_used(curr, alignment);
    }

    auto start = curr_used_aligned;
    auto end   = start + size;

    // later for address sanitizer if needed
    // auto start_padding = curr->used;
    // auto end_padding   = curr_used_aligned;

    Assert(end <= curr->reserved);

    if (curr->commited < end) {
        auto next_commit_boundary = align_up(end, cast(Uint)VIRTUAL_PAGE_SIZE);
        Assert(next_commit_boundary <= curr->reserved);

        auto ok = virtual_commit(cast(void*)(curr->data + cast(Uintptr)curr->commited),
                                 next_commit_boundary - curr->commited );

        if (!ok) {
            out_of_memory();
        }

        curr->commited = next_commit_boundary;
    }

    curr->used = end;

    result.data = cast(U8*)(curr->data + cast(Uintptr)start);
    result.len  = cast(Int)(end - start);

    return result;
}

function Slice<U8> _arena_push_aligned(Arena *a, Uint size, Uint alignment) {
    auto result = _arena_push_aligned_non_zeroed(a, size, alignment);
    MemoryZero(result.data, cast(Uint)result.len);
    return result;
}

function void arena_pop_to(Arena *arena, Uint pos) {
    pos = clamp_bot(sizeof(Arena), pos);

    auto curr = arena->curr;
    auto curr_pos = curr->base_pos + curr->used;

    if (curr_pos < pos) {
        // TODO(robin): is there a better way to handle this
        return;
    }

    for (auto prev = curr->prev; pos <= curr->base_pos; curr = prev) {
        prev = curr->prev;

        arena->curr = prev;
        arena_destroy_single(curr);
    }

    arena->curr = curr;

    auto new_used = pos - curr->base_pos;
    curr->used = new_used;
}

function void arena_pop(Arena *arena, Int size) {
    auto curr = arena->curr;
    auto used = curr->base_pos + curr->used;

    if (0 <= size) {
        return;
    }

    auto size_uint = cast(Uint)size;
    size_uint      = clamp_top(size_uint, used);

    arena_pop_to(arena, used - size_uint);
}

function void arena_clear(Arena *arena) {
    arena_pop_to(arena, 0);
}

function String arena_string_clone(Arena *arena, String string) {
    String result = {};

    auto slice = arena_make<U8>(arena, string.len);
    MemoryCopy(slice.data, string.data, cast(Uint)string.len);

    result.len  = string.len;
    result.data = slice.data;

    return result;
}

function Arena_Temp arena_temp_begin(Arena *arena) {
    Arena_Temp temp = {
        arena,
        arena->curr->base_pos + arena->curr->used,
    };
    return temp;
}

function void arena_temp_end(Arena_Temp temp) {
    arena_pop_to(temp.arena, temp.pos);
}

#define ARENA_MAX_TEMPORARY_ARENAS 2

variable_global_thread_local Arena *arena_temporary_arenas[ARENA_MAX_TEMPORARY_ARENAS];

function Arena *_arena_get_temporary_arena(Int count, Arena **arenas) {
    Arena *found_arena = 0;

    for (Int i = 0; i < ARENA_MAX_TEMPORARY_ARENAS; i++) {

        for (Int j = 0; j < count; j++) {
            if (arenas[j] == arena_temporary_arenas[i]) {
                goto already_there;
            }
        }

        if (!arena_temporary_arenas[i]) {
            arena_temporary_arenas[i] = arena_alloc();
        }

        found_arena = arena_temporary_arenas[i];
        break;

        already_there:;
    }

    Assert(found_arena);

    return found_arena;
}
