struct Wl_Global {
    Wl_Global *next, *prev;

    U32    name;
    String interface;
    U32    version;
};

struct Wl_Global_List {
    Wl_Global *first, *last;
    Int       len;
};

function void wl_global_list_push(Wl_Global_List *list, Wl_Global *global);
function void wl_global_list_remove(Wl_Global_List *list, Wl_Global *global);

struct Wl_State {
    Arena       *arena;
    Logger      logger;
    wl_display  *display;
    wl_registry *registry;

    river_window_manager_v1 *window_manager;
    river_xkb_bindings_v1   *xkb_bindings;

    Wl_Global_List globals;
};

variable_global Wl_State *wl_state;

function void wl_init(void);
function void wl_enter_loop(void);
function river_window_manager_v1 *wl_get_window_manager(void);
function river_xkb_bindings_v1 *wl_get_xkb_bindings(void);
