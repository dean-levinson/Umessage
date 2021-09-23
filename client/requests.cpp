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
        push<T>(request, c);
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

Request1000::Request1000(std::string name, std::string pubkey):
    name(name),
    pubkey(pubkey) {};

uint16_t Request1000::get_code() const {
    return 1000;
}

vector<byte> Request1000::build() const {
    vector<byte> request;
    push_string(request, name, 255);
    push_string(request, pubkey, 160);
    return request;
}

Request1002::Request1002(string client_id):
    client_id(client_id) {};

uint16_t Request1002::get_code() const {
    return 1002;
}

vector<byte> Request1002::build() const {
    vector<byte> request;
    push_string(request, client_id, 16);
    return request;
}