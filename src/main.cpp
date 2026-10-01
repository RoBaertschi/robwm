#include "base/base_inc.h"
#include "wayland/wayland_inc.h"
#include "bindings_manager/bindings_manager_inc.h"
#include "window_manager/window_manager_inc.h"

#include "base/base_inc.cpp"
#include "wayland/wayland_inc.cpp"
#include "bindings_manager/bindings_manager_inc.cpp"
#include "window_manager/window_manager_inc.cpp"

#include <stdio.h>

function_global int main(void) {
    wl_init();
    bm_init();
    wm_init();
    wl_enter_loop();
}
