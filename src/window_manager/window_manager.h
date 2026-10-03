#define WM_STRING_PART_SIZE 64

struct WM_String_Part {
    WM_String_Part *next, *prev;

    Int len; // actually used part of the string
    U8  buffer[WM_STRING_PART_SIZE];
};

typedef List<WM_String_Part> WM_String_Parts;

function WM_String_Part *wm_new_string_part(void);
function WM_String_Parts wm_string_parts_from_cstring(char const *cstring);
function WM_String_Parts wm_string_parts_from_string(String string);
function void wm_release_string_parts(WM_String_Parts parts);
function String wm_string_from_string_parts(Arena *arena, WM_String_Parts parts);

struct WM_Output {
    WM_Output *next, *prev;

    river_output_v1  *output;
    struct WM_Layout *layout;
};

struct WM_Window {
    WM_Window *next, *prev;

    WM_String_Parts title;
    WM_String_Parts app_id;

    WM_Window       *parent;
    river_window_v1 *window;

    V2I32 dimensions;
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
#define WM_COMMANDS             \
    X(Manage_Window_Added)      \
    X(Manage_Window_Closed)     \
    X(Manage_Window_Dimensions) \

enum WM_Command_Kind {
    #define X(name) Glue(WM_Command_, name),
    WM_COMMANDS
    #undef X
    WM_Command__MAX,
};

struct WM_Command {
    WM_Command_Kind kind;
    WM_Window       *window;
    V2I32           new_dimensions;
};

struct WM_Command_Node {
    WM_Command_Node *next, *prev;
    WM_Command      command;
};

typedef List<WM_Command_Node> WM_Command_List;

// Layout

struct WM_Layout_Node {
    WM_Layout_Node *parent;
    WM_Layout_Node *siblings_next,  *siblings_prev;
    WM_Layout_Node *children_first, *children_last;

    Axis      direction;
    WM_Window *window; // if 0, then empty
    V2I32     size; // calculated node size, not the actual window size, the window can do whatever it wants sadly
};

typedef List<WM_Layout_Node, &WM_Layout_Node::siblings_next, &WM_Layout_Node::siblings_prev> WM_Layout_Node_List;

struct WM_Layout {
    WM_Layout *next, *prev;

    WM_Layout_Node *root;
    WM_Layout_Node *focused;
    WM_Output      *output;
};

typedef List<WM_Layout> WM_Layout_List;

function WM_Layout_Node *wm_new_layout_node(void);
function WM_Layout *wm_new_layout(void);

struct WM_State {
    Arena  *arena;
    Logger logger;

    WM_Layout_List      free_layouts;
    WM_Layout_Node_List free_layout_nodes;

    WM_String_Parts free_string_parts;

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
