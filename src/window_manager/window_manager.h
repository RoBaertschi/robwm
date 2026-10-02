struct WM_Seat {
    WM_Seat       *next, *prev;
    Wl_Seat       *seat; // Optional, be carefull
    river_seat_v1 *river_seat;
};

struct WM_Seat_List {
    WM_Seat *first, *last;
    Int     len;
};

function void wm_seat_list_push(WM_Seat_List *list, WM_Seat *seat);
function void wm_seat_list_remove(WM_Seat_List *list, WM_Seat *seat);

struct WM_State {
    Arena  *arena;
    Logger logger;

    WM_Seat_List seats;
    WM_Seat_List unnamed_seats;
    WM_Seat_List free_seats;
};

function void wm_init(void);
