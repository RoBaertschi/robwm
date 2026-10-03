function void bm_nil_action_callback(void *data) {
    Unused(data);
}

function BM_Action bm_nil_action(void) {
    BM_Action result = {};
    result.callback = bm_nil_action_callback;
    return result;
}

variable_global BM_State *bm_state;

function void bm_init(void) {
    auto arena = arena_alloc();
    bm_state = arena_new<BM_State>(arena);
    bm_state->arena = arena;
    bm_state->logger = { STR("bm") };

    bm_state->bindings = arena_make<BM_Binding *>(arena, 4096);
}

function void bm_river_xkb_binding_listener_pressed(
    void *data,
    river_xkb_binding_v1 *river_xkb_binding)
{
    logger_debugf(bm_state->logger, "Binding pressed");
    Unused(river_xkb_binding);
    auto binding = cast(BM_Binding *)data;
    binding->action.callback(binding->action.data);
}

function void bm_river_xkb_binding_listener_released(
    void *data,
    river_xkb_binding_v1 *river_xkb_binding)
{
    // TODO(robin): support on release stuff
    Unused(data);
    Unused(river_xkb_binding);
}

function void bm_river_xkb_binding_listener_stop_repeat(
    void *data,
    river_xkb_binding_v1 *river_xkb_binding)
{
    // TODO(robin): support repeat?
    Unused(data);
    Unused(river_xkb_binding);
}

variable_global_readonly river_xkb_binding_v1_listener bm_river_xkb_binding_listener = {
    bm_river_xkb_binding_listener_pressed,
    bm_river_xkb_binding_listener_released,
    bm_river_xkb_binding_listener_stop_repeat,
};

function void bm_add_binding_to_seat(BM_River_Seat *seat, BM_Binding *binding) {
    BM_River_Binding *river_binding = 0;
    if (bm_state->free_river_bindings.first) {
        river_binding = bm_state->free_river_bindings.first;
        list_remove(&bm_state->free_river_bindings, river_binding);
        *river_binding = {};
    } else {
        river_binding = arena_new<BM_River_Binding>(bm_state->arena);
    }

    river_binding->binding = binding;

    auto bindings = wl_get_xkb_bindings();
    river_binding->river_binding = river_xkb_bindings_v1_get_xkb_binding(
        bindings,
        seat->seat,
        binding->shortcut.keysym,
        binding->shortcut.modifiers);

    river_xkb_binding_v1_add_listener(
        river_binding->river_binding,
        &bm_river_xkb_binding_listener,
        binding);

    if (binding->enabled) {
        river_xkb_binding_v1_enable(river_binding->river_binding);
    }

    DLLPushBack_NP(seat->first, seat->last, river_binding, seat_next, seat_prev);
    list_push(&binding->river_bindings, river_binding);
}

function void bm_add_seat(river_seat_v1 *river_seat) {
    BM_River_Seat *seat = 0;
    if (bm_state->free_seats.first) {
        seat = bm_state->free_seats.first;
        list_remove(&bm_state->free_seats, seat);
        *seat = {};
    } else {
        seat = arena_new<BM_River_Seat>(bm_state->arena);
    }
    seat->seat = river_seat;

    for (auto binding_bucket : bm_state->bindings) {
        for (auto binding = binding_bucket; binding; binding = binding->hash_next) {
            bm_add_binding_to_seat(seat, binding);
        }
    }

    list_push(&bm_state->seats, seat);
}

function void bm_remove_seat(river_seat_v1 *river_seat) {
    BM_River_Seat *seat = 0;
    DLLForEach(bm_state->seats.first, bm_river_seat) {
        if (bm_river_seat->seat == river_seat) {
            seat = bm_river_seat;
            break;
        }
    }

    if (seat) {
        BM_River_Binding *next = 0;
        for (auto river_binding = seat->first; river_binding; river_binding = next) {
            next = river_binding->seat_next;

            river_xkb_binding_v1_destroy(river_binding->river_binding);
            river_binding->river_binding = 0;

            list_remove(&river_binding->binding->river_bindings, river_binding);
            DLLRemove_NP(seat->first, seat->last, river_binding, seat_next, seat_prev);

            *river_binding = {};
            list_push(&bm_state->free_river_bindings, river_binding);
        }

        list_remove(&bm_state->seats, seat);
        list_push(&bm_state->free_seats, seat);
    } else {
        logger_errorf(bm_state->logger, "Could not find river seat to remove: %p", river_seat);
    }
}

function BM_Binding_Key bm_binding_key_from_shortcut(BM_Shortcut shortcut) {
    return mix_u64(shortcut.v);
}

function BM_Binding *bm_binding_from_key(BM_Binding_Key key) {
    auto bucket = &bm_state->bindings[cast(Int)(key % cast(BM_Binding_Key)bm_state->bindings.len)];
    BM_Binding *prev = 0;

    while (true) {
        if (!*bucket) {
            BM_Binding new_binding = {};
            new_binding.key = key;
            new_binding.hash_prev = prev;
            new_binding.action = bm_nil_action();

            *bucket = arena_new<BM_Binding>(bm_state->arena);
            **bucket = new_binding; // I just really don't wanna write (*bucket)->... all the time

            DLLForEach(bm_state->seats.first, seat) {
                bm_add_binding_to_seat(seat, *bucket);
            }
            break;
        }

        if ((*bucket)->key == key) {
            break;
        }

        prev   = *bucket;
        bucket = &(*bucket)->hash_next;
    }

    return *bucket;
}

function BM_Binding *bm_binding_from_shortcut(BM_Shortcut shortcut) {
    auto key = bm_binding_key_from_shortcut(shortcut);
    auto binding = bm_binding_from_key(key);

    if (binding->shortcut.v == BM_SHORTCUT_INVALID.v) {
        binding->shortcut = shortcut;
    }

    Assert(binding->shortcut.v == shortcut.v);
    return binding;
}

function void bm_binding_enable(BM_Binding *binding) {
    if (!binding->enabled) {
        binding->enabled = true;
        DLLForEach_N(binding->river_bindings.first, river_binding, bind_next) {
            river_xkb_binding_v1_enable(river_binding->river_binding);
        }
    }
}

function void bm_binding_disable(BM_Binding *binding) {
    if (binding->enabled) {
        binding->enabled = false;
        DLLForEach_N(binding->river_bindings.first, river_binding, bind_next) {
            river_xkb_binding_v1_disable(river_binding->river_binding);
        }
    }
}
