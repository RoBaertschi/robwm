#define VIRTUAL_PAGE_SIZE 4096

struct Virtual_Reserve_Result {
    Bool ok;
    void *data;
};

function Virtual_Reserve_Result virtual_reserve(Uint size);
function Bool virtual_commit(void *data, Uint size);
function Bool virtual_decommit(void *data, Uint size);
function void virtual_release(void *data, Uint size);
