struct String {
    U8 const *data;
    Int      len;

    U8 operator[](Int index) const {
        Assert(0 <= index && index < len);
        return data[index];
    }

    Bool operator==(String const& other) const {
        if (len != other.len) {
            return false;
        }

        return MemoryMatch(data, other.data, cast(Uint)len);
    }
};

function String string_from_cstring(char const *cstring);

#define STR(literal) String { cast(U8 const *)literal, cast(Int)(sizeof(literal)-1) }
#define FMT_STR "%.*s"
#define FMT_STR_ARG(s) cast(int)(s).len, cast(char const *)(s).data
