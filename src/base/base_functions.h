function_noreturn void fail(char const *format, ...);

template <typename T>
function T abs(T value) {
    return 0 <= value ? value : -value;
}

function U64 mix_u64(U64 value);
function U64 unmix_u64(U64 x);

function V2I32 v2i32(I32 x, I32 y);
