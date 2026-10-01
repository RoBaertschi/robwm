struct WM_Seat {
    WM_Seat       *next, *prev;
    Wl_Seat       *seat;
    river_seat_v1 *river_seat;
};

struct WM_Seat_List {
    WM_Seat *first, *last;
    Int     len;
};

function void wm_seat_list_push(WM_Seat_List *list, WM_Seat *seat);
function void wm_seat_list_remove(WM_Seat_List *list, WM_Seat *seat);

typedef void (*WM_On_Seat_Added_Callback)(WM_Seat *seat);
typedef void (*WM_On_Seat_Removed_Callback)(WM_Seat *seat);

struct WM_State {
    Arena  *arena;
    Logger logger;

    WM_On_Seat_Added_Callback   added_callback;
    WM_On_Seat_Removed_Callback removed_callback;

    WM_Seat_List seats;
    WM_Seat_List unnamed_seats;
};

function void wm_init(void);
function void wm_on_seat_added(WM_On_Seat_Added_Callback added_callback);
function void wm_on_seat_removed(WM_On_Seat_Removed_Callback removed_callback);
