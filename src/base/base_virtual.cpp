#include <sys/mman.h>

// Probably posix compliant implementation, who knows xD

function Virtual_Reserve_Result virtual_reserve(Uint size) {
    size = align_up(size, cast(Uint)VIRTUAL_PAGE_SIZE);

    Virtual_Reserve_Result result = {};

    result.data = mmap(0, size, PROT_NONE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    result.ok   = result.data != MAP_FAILED;

    return result;
}

function Bool virtual_commit(void *data, Uint size) {
    Assert(align_up(cast(Uintptr)data, cast(Uintptr)VIRTUAL_PAGE_SIZE) == cast(Uintptr)data);
    size = align_up(size, cast(Uint)VIRTUAL_PAGE_SIZE);

    Bool ok = mprotect(data, size, PROT_READ | PROT_WRITE) == 0;
    return ok;
}

function void virtual_release(void *data, Uint size) {
    Assert(align_up(cast(Uintptr)data, cast(Uintptr)VIRTUAL_PAGE_SIZE) == cast(Uintptr)data);
    size = align_up(size, cast(Uint)VIRTUAL_PAGE_SIZE);

    munmap(data, size);
}
