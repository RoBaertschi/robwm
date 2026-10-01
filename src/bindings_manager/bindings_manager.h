// X(name, bit pos zero indexed, lower-case name)
#define BM_MODIFIERS   \
    X(Shift, 0, shift) \
    X(Ctrl, 2, ctrl)   \
    X(Mod1, 3, mod1)   \
    X(Mod3, 5, mod3)   \
    X(Mod4, 6, mod4)   \
    X(Mod5, 7, mod5)

enum BM_Modifier : U32 {
    #define X(name, bitpos, _lcname) Glue(BM_Modifier_, name) = 1 << bitpos,
    BM_MODIFIERS
    #undef X

    BM_Modifier_Alt   = BM_Modifier_Mod1,
    BM_Modifier_Super = BM_Modifier_Mod3,
};

static_assert(cast(U32)BM_Modifier_Shift == cast(U32)RIVER_SEAT_V1_MODIFIERS_SHIFT, "BM_Modifier_Shift matches river");
static_assert(cast(U32)BM_Modifier_Ctrl  == cast(U32)RIVER_SEAT_V1_MODIFIERS_CTRL,  "BM_Modifier_Ctrl matches river");
static_assert(cast(U32)BM_Modifier_Mod1  == cast(U32)RIVER_SEAT_V1_MODIFIERS_MOD1,  "BM_Modifier_Mod1 matches river");
static_assert(cast(U32)BM_Modifier_Mod3  == cast(U32)RIVER_SEAT_V1_MODIFIERS_MOD3,  "BM_Modifier_Mod3 matches river");
static_assert(cast(U32)BM_Modifier_Mod4  == cast(U32)RIVER_SEAT_V1_MODIFIERS_MOD4,  "BM_Modifier_Mod4 matches river");
static_assert(cast(U32)BM_Modifier_Mod5  == cast(U32)RIVER_SEAT_V1_MODIFIERS_MOD5,  "BM_Modifier_Mod5 matches river");

typedef U32 BM_Modifiers;
typedef U32 BM_Keysym; // equal to xkb_keysym_t

union BM_Shortcut {
    struct BM_Binding_Key_Pair {
        BM_Keysym    keysym;
        BM_Modifiers modifiers;
    };
    U64 v;
};

#define BM_SHORTCUT_INVALID (BM_Shortcut {})

typedef U64 BM_Binding_Key;

typedef void (*BM_Action_Callback)(void *data);

struct BM_Action {
    void               *data;
    BM_Action_Callback callback;
};

function BM_Action bm_nil_action(void);

struct BM_Binding {
    BM_Shortcut    shortcut;
    BM_Binding_Key key;
    BM_Action      action;

    river_xkb_binding_v1 *river_binding;

    BM_Binding *hash_prev, *hash_next;
};

struct BM_State {
    Arena  *arena;
    Logger logger;

    Slice<BM_Binding *> bindings;
};

function void bm_init(void);
function BM_Binding_Key bm_binding_key_from_shortcut(BM_Shortcut shortcut);
function BM_Binding *bm_binding_from_key(BM_Binding_Key key);
function BM_Binding *bm_binding_from_shortcut(BM_Shortcut shortcut);
