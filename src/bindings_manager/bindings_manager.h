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
    struct {
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

struct BM_River_Binding {
    BM_River_Binding     *bind_next, *bind_prev; // Per BM_Binding
    BM_River_Binding     *seat_next, *seat_prev; // Per BM_Seat
    river_xkb_binding_v1 *river_binding;
};

struct BM_River_Binding_List {
    BM_River_Binding *first, *last;
    Int              len;
};

function void bm_river_binding_list_push(BM_River_Binding_List *list, BM_River_Binding *binding);
function void bm_river_binding_list_remove(BM_River_Binding_List *list, BM_River_Binding *binding);

struct BM_Binding {
    BM_Shortcut    shortcut;
    BM_Binding_Key key;
    BM_Action      action;

    Bool enabled;

    BM_River_Binding_List river_bindings;

    BM_Binding *hash_prev, *hash_next;
};

struct BM_River_Seat {
    BM_River_Seat    *next, *prev;
    river_seat_v1    *seat;
    BM_River_Binding *first, *last;
};

struct BM_River_Seat_List {
    BM_River_Seat *first, *last;
};

function void bm_river_seat_list_push(BM_River_Seat_List *list, BM_River_Seat *seat);
function void bm_river_seat_list_remove(BM_River_Seat_List *list, BM_River_Seat *seat);

struct BM_State {
    Arena  *arena;
    Logger logger;

    BM_River_Seat_List  seats;
    Slice<BM_Binding *> bindings;
};

function void bm_init(void);
function void bm_add_seat(river_seat_v1 *seat);
function BM_Binding_Key bm_binding_key_from_shortcut(BM_Shortcut shortcut);
function BM_Binding *bm_binding_from_key(BM_Binding_Key key);
function BM_Binding *bm_binding_from_shortcut(BM_Shortcut shortcut);
function void bm_binding_enable(BM_Binding *binding);
