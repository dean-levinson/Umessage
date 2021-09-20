#ifndef ENDIANESS_INL_
#define ENDIANESS_INL_

#include <stdint.h>

static bool is_big_endian(void)
{
    union {
        uint32_t i;
        char c[4];
    } test_int = {0x01020304};

    return test_int.c[0] == 1; 
}

static bool is_be = is_big_endian();

template <typename T>
T little_endian(T u)
{
    if (!is_be) {
        return u;
    }

    static_assert (CHAR_BIT == 8, "CHAR_BIT != 8");

    union
    {
        T u;
        unsigned char u8[sizeof(T)];
    } source, dest;

    source.u = u;

    for (size_t k = 0; k < sizeof(T); k++)
        dest.u8[k] = source.u8[sizeof(T) - k - 1];

    return dest.u;
}

#endif // ENDIANESS_INL_