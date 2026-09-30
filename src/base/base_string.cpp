function String string_from_cstring(char const *cstring) {
    String result = {};

    if (cstring) {
        result.data = cast(U8 const*)cstring;
        result.len  = cast(Int)strlen(cstring);
    }

    return result;
}
