struct WM_Output {
    WM_Output *next, *prev;

    river_output_v1 *output;
};

struct WM_Window {
    WM_Window *next, *prev;

    river_window_v1 *window;
};

struct WM_Seat {
    WM_Seat            *next, *prev;
    Maybe_Ptr<Wl_Seat> seat; // Optional, be careful
    river_seat_v1      *river_seat;
};

typedef List<WM_Output> WM_Output_List;
typedef List<WM_Window> WM_Window_List;
typedef List<WM_Seat> WM_Seat_List;

// X(name)
#define WM_COMMANDS \
    X(Manage_Window_Added)

enum WM_Command_Kind {
    #define X(name) Glue(WM_Command_, name),
    WM_COMMANDS
    #undef X
    WM_Command__MAX,
};

struct WM_Command {
    WM_Command_Kind kind;
    WM_Window       *window;
};

struct WM_Command_Node {
    WM_Command_Node *next, *prev;
    WM_Command      command;
};

typedef List<WM_Command_Node> WM_Command_List;

struct WM_State {
    Arena  *arena;
    Logger logger;

    WM_Command_List commands;
    WM_Command_List free_commands;

    WM_Output_List outputs;
    WM_Output_List free_outputs;

    WM_Window_List windows;
    WM_Window_List free_windows;

    WM_Seat_List seats;
    WM_Seat_List unnamed_seats;
    WM_Seat_List free_seats;
};

function void wm_init(void);
