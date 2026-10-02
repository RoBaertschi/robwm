struct WM_Seat {
    WM_Seat       *next, *prev;
    Wl_Seat       *seat; // Optional, be carefull
    river_seat_v1 *river_seat;
};

typedef List<WM_Seat> WM_Seat_List;

struct WM_State {
    Arena  *arena;
    Logger logger;

    WM_Seat_List seats;
    WM_Seat_List unnamed_seats;
    WM_Seat_List free_seats;
};

function void wm_init(void);
