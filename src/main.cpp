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
#include <spawn.h>

function void spawn_alacritty_action(void *) {
    int _pid;
    extern char **environ;
    char program[] = "alacritty";
    char *const args[] = { program, 0 };
    posix_spawnp(&_pid, "alacritty", 0, 0, args, environ);
}

function_global int main(void) {
    wl_init();
    bm_init();
    wm_init();

    BM_Shortcut shortcut;
    shortcut.keysym = XKB_KEY_Return;
    shortcut.modifiers = BM_Modifier_Ctrl;
    auto binding = bm_binding_from_shortcut(shortcut);
    binding->action.callback = spawn_alacritty_action;
    bm_binding_enable(binding);

    wl_enter_loop();
}
