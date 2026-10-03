// when I am really scarred about something
// only use this when 100% that I will forget to check for null and there is no better
// way to do it

template <typename T>
struct Maybe_Ptr {
    T *value;
};

template <typename T>
function T *maybe_get(Maybe_Ptr<T> ptr) {
    Assert(ptr.value);
    return ptr.value;
}

template <typename T>
function T *maybe_get_or_null(Maybe_Ptr<T> ptr) {
    return ptr.value;
}

template <typename T>
function Bool maybe_is_set(Maybe_Ptr<T> ptr) {
    return ptr.value != 0;
}
