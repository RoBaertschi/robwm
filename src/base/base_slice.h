template <typename T>
struct Slice {
    T   *data;
    Int len;

    T& operator[](Int index) {
        Assert(0 <= index && index < len);
        return data[index];
    }

    T const& operator[](Int index) const {
        Assert(0 <= index && index < len);
        return data[index];
    }
};

template <typename T>
function T* begin(Slice<T> const& slice) {
    return slice.data;
}

template <typename T>
function T* end(Slice<T> const& slice) {
    return slice.data + slice.len;
}
