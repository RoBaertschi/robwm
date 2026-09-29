#include "base/base_inc.h"
#include "wayland/wayland_inc.h"

#include "base/base_inc.cpp"
#include "wayland/wayland_inc.cpp"

#include <stdio.h>

function_global int main(void) {
    auto result = virtual_reserve(10);
    defer(virtual_release(result.data, 10));
    if (result.ok) {
        printf("virtual_reserve: %p\n", result.data);

        auto commit_ok = virtual_commit(result.data, 10);
        if (commit_ok) {
            printf("virtual_commit: ok\n");
        } else {
            printf("virtual_commit: failed\n");
        }
    } else {
        printf("virtual_reserve: failed\n");
    }

    auto arena = arena_alloc();
    defer(arena_destroy(arena));

    auto floats = arena_make<F32>(arena, 2);

    F32 test = 2.0;
    for (auto& f : floats) {
        f     = test;
        test *= 4;
    }

    for (Int i = 0; i < floats.len; i++) {
        printf("%f\n", floats[i]);
    }
}
