// X(name, string name)
#define WL_GLOBAL_KINDS \
    X(Unknown, "unknown") \
    X(Wl_Seat, "wl_seat") \
    X(River_Window_Manager, "river_window_manager_v1") \
    X(River_Xkb_Bindings, "river_xkb_bindings_v1")


enum Wl_Global_Kind {
    #define X(name, _string_name) Glue(Wl_Global_Kind_, name),
    WL_GLOBAL_KINDS
    #undef X

    Wl_Global_Kind__MAX,
};

struct Wl_Global {
    Wl_Global *next, *prev;

    Wl_Global_Kind kind;

    U32    name;
    String interface;
    U32    version;

    struct Wl_Seat *seat; // if kind == Seat
};

struct Wl_Global_List {
    Wl_Global *first, *last;
    Int       len;
};

function void wl_global_list_push(Wl_Global_List *list, Wl_Global *global);
function void wl_global_list_remove(Wl_Global_List *list, Wl_Global *global);

struct Wl_Seat {
    Wl_Seat   *next, *prev;
    wl_seat   *seat;
    Wl_Global *global;
};

struct Wl_Seat_List {
    Wl_Seat *first, *last;
    Int     len;
};

function void wl_seat_list_push(Wl_Seat_List *list, Wl_Seat *seat);
function void wl_seat_list_remove(Wl_Seat_List *list, Wl_Seat *seat);

// Hooks

// X(name, sc_name, T)
#define WL_HOOKS \
    X(Wl_Seat_Added, wl_seat_added, Wl_Seat *) \
    X(Wl_Seat_Removed, wl_seat_removed, Wl_Seat *)

// Hook types
#define X(name, _sc_name, T) typedef void (*Glue(Wl_Hook_, name))(void *data, T hook_data);
WL_HOOKS
#undef X

// Hook helper functions
#define X(name, sc_name, _T) function void Glue(wl_set_hook_, sc_name)(Glue(Wl_Hook_, name) hook, void *data);
WL_HOOKS
#undef X

struct Wl_Hooks {
    #define X(name, sc_name, _T) Glue(Wl_Hook_, name) sc_name; void * Glue(sc_name, data);
    WL_HOOKS
    #undef X
};

struct Wl_State {
    Arena       *arena;
    Logger      logger;
    wl_display  *display;
    wl_registry *registry;

    river_window_manager_v1 *window_manager;
    river_xkb_bindings_v1   *xkb_bindings;

    Wl_Hooks hooks;

    Wl_Global_List globals;
    Wl_Seat_List   seats;

    Wl_Global_List free_globals;
    Wl_Seat_List   free_seats;
};

variable_global Wl_State *wl_state;

function void wl_init(void);
function void wl_enter_loop(void);
function Wl_Seat_List *wl_get_seats(void);
function river_window_manager_v1 *wl_get_window_manager(void);
function river_xkb_bindings_v1 *wl_get_xkb_bindings(void);
