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

struct Wl_State {
    Arena       *arena;
    Logger      logger;
    wl_display  *display;
    wl_registry *registry;

    river_window_manager_v1 *window_manager;
    river_xkb_bindings_v1   *xkb_bindings;

    Wl_Global_List globals;
    Wl_Seat_List   seats;
};

variable_global Wl_State *wl_state;

function void wl_init(void);
function void wl_enter_loop(void);
function Wl_Seat_List *wl_get_seats(void);
function river_window_manager_v1 *wl_get_window_manager(void);
function river_xkb_bindings_v1 *wl_get_xkb_bindings(void);
