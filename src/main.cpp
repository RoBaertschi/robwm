#include "base/base_inc.h"
#include "wayland/wayland_inc.h"

#include "base/base_inc.cpp"
#include "wayland/wayland_inc.cpp"

#include <stdio.h>

function_global int main(void) {
    wl_init();
    wl_enter_loop();
}
