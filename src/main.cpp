#include "base/base_inc.h"
#include "base/base_inc.cpp"

#include <stdio.h>

function_global int main(void) {
    U8 test = 0xffffffffffffffff_u8;

    printf(FMT_U8 "\n", test);

    auto result = virtual_reserve(10);
    if (result.ok) {
        printf("virtual_reserve: %p\n", result.data);

        auto commit_ok = virtual_commit(result.data, 10);
        if (commit_ok) {
            printf("virtual_commit: ok\n");
        } else {
            printf("virtual_commit: failed\n");
        }
        virtual_release(result.data, 10);
    } else {
        printf("virtual_reserve: failed");
    }
}
