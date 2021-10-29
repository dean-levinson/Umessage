#ifndef ENDIANESS_INL_
#define ENDIANESS_INL_

#include <stdint.h>

/**
 * Determines weather the machine's processor is big endian.
 * 
 * @return bool 
 */
static bool is_big_endian(void)
{
    union {
        uint32_t i;
        char c[4];
    } test_int = {0x01020304};

    return test_int.c[0] == 1; 
}

/**
 * Static boolian that indicates weather the machine's processor is big endian.
 * Used instead of calling is_big_endian every time.
 */
static bool is_be = is_big_endian();

/**
 * Template function that receives a variable and turns it into a 
 * little endian version of the same type (i.e., swap the direction of
 * the bytes in the variable).
 * 
 * @tparam T - The type of the variable.
 * @param u - The variable.
 * @return T - The little endian version of the variable.
 */
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