#include "base/base_inc.h"
#include "wayland/wayland_inc.h"
#include "bindings_manager/bindings_manager_inc.h"
#include "window_manager/window_manager_inc.h"

#include <xkbcommon/xkbcommon-keysyms.h>

#include "base/base_inc.cpp"
#include "wayland/wayland_inc.cpp"
#include "bindings_manager/bindings_manager_inc.cpp"
#include "window_manager/window_manager_inc.cpp"

#include <stdio.h>

function_global int main(void) {
    wl_init();
    bm_init();
    wm_init();

    BM_Shortcut shortcut;
    shortcut.keysym = XKB_KEY_Return;
    shortcut.modifiers = BM_Modifier_Ctrl;
    auto binding = bm_binding_from_shortcut(shortcut);
    bm_binding_enable(binding);

    wl_enter_loop();
}
