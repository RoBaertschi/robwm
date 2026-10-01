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
            **bucket = new_binding;
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
