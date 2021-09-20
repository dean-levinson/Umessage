#include "requests.h"
#include "endianness.inl"

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

void push_string(vector<byte>& request, string str, size_t size) {
    str.resize(size, 0);
    push_string(request, str);
}

template<typename T>
void push_vector(vector<byte>& request, vector<T> vector) {
    for (const T c: vector) {
        push<T>(request, static_cast<byte>(c));
    }
}

template<typename T>
void push_vector(vector<byte>& request, vector<T> vector, size_t size) {
    vector.resize(size, std::byte(0));
    push_vector<T>(request, vector);
}

RequestHeaders::RequestHeaders(std::string client_id, uint8_t version,
                               uint16_t code, uint32_t payload_size):
    client_id(client_id),
    version(version),
    code(code),
    payload_size(payload_size) {}

vector<byte> RequestHeaders::build() {
    vector<byte> request;
    push_string(request, client_id, 16);
    push<uint8_t>(request, version);
    push<uint16_t>(request, code);
    push<uint32_t>(request, payload_size);

    return request;
}

uint8_t RequestCode::get_code() {
    return code;
}

Request1000::Request1000(std::string name, vector<byte> pubkey):
    code(1000),
    name(name),
    pubkey(pubkey) {};

vector<byte> Request1000::build() {
    vector<byte> request;
    push_string(request, name, 255);
    push_vector<byte>(request, pubkey, 160);
    return request;
}
