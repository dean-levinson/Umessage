#include "requests.h"

bool is_big_endian(void)
{
    union {
        uint32_t i;
        char c[4];
    } test_int = {0x01020304};

    return test_int.c[0] == 1; 
}

bool is_be = is_big_endian();

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

template<typename T>
void push(vector<byte>& request, T var) {
    byte array[sizeof(T)];
    T le_var = little_endian<T>(var);
    memcpy(&array[0], &le_var, sizeof(T));
    for (const byte b: array) {
        request.push_back(b);
    }
}

void push_string(vector<byte>& request, string str) {
    for (const char c: str) {
        request.push_back(static_cast<byte>(c));
    }
}

RequestHeaders::RequestHeaders(std::string client_id, uint8_t version,
                               uint16_t code, uint32_t payload_size):
    client_id(client_id),
    version(version),
    code(code),
    payload_size(payload_size) {}

vector<byte> RequestHeaders::build() {
    vector<byte> request;
    push<uint8_t>(request, version);
    push<uint16_t>(request, code);
    push<uint32_t>(request, payload_size);

    return request;
}