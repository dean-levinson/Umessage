#include "responses.h"
#include "endianness.inl"

/**
 * Pops out and returns value of size T, from the vector's frontend.
 * (The opposite of the traditional std::vector::pop_back method). 
 * 
 * @tparam T - The size which should pop.
 * @param response - vector of bytes that represents the response.
 * @return T - Value of size T from the start of the vector.
 */
template<typename T>
T pop(vector<byte>& response) {
    T var;
    byte array[sizeof(T)];
    for (byte& b: array) {
        b = response.front();
        response.erase(response.begin());
    }    
    memcpy(&var, &array[0], sizeof(T));
    return little_endian<T>(var);
}

ResponseHeaders::ResponseHeaders(vector<byte> received_bytes) {
    version = pop<uint8_t>(received_bytes);
    code = pop<uint16_t>(received_bytes);
    payload_size = pop<uint32_t>(received_bytes);
}
