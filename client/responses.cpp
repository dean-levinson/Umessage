#include <algorithm>
#include <stdexcept>

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

string pop_string(vector<byte>& response, size_t size) {
    if (size > response.size()) {
        throw std::length_error("The string's size is bigger than the response");
    }    
    string output_str;
    output_str.resize(size, 0);
    for (char& c: output_str) {
        c = static_cast<char>(response.front());
        response.erase(response.begin());
    }
    return output_str;
}

template<typename T>
vector<T> pop_vector(vector<byte>& response, size_t size) {
    if (size > response.size()) {
        throw std::length_error("The vector's size is bigger than the response");
    }    
    vector<T> output_vector;
    output_vector.resize(size, byte(0));
    for (T &x: output_vector) {
        memcpy(&x, &response[0], sizeof(T));
        response.erase(response.begin(),
                       response.begin() + sizeof(T));
    }
    return output_vector;
}

size_t ResponseHeaders::size() {
    return sizeof(version) + sizeof(code) + sizeof(payload_size);
}

void ResponseHeaders::parse(vector<byte> received_bytes) {
    version = pop<uint8_t>(received_bytes);
    code = pop<uint16_t>(received_bytes);
    payload_size = pop<uint32_t>(received_bytes);
}

void Response2000::parse(vector<byte> received_bytes) {
    client_id = pop_string(received_bytes, 16);
}

void Response2001::parse(vector<byte> received_bytes) {
    unsigned int num_of_clients = received_bytes.size() / (16 + 255); // Todo - replace with const
    for (unsigned int i = 0; i < num_of_clients; i++) {
        string client_id = pop_string(received_bytes, 16);
        string client_name = pop_string(received_bytes, 255);
        // Remove all the characters from null terminated to the end. 
        client_name.erase(std::find(client_name.begin(), client_name.end(), '\0'), client_name.end());
        client_list.push_back(User(client_id, client_name));
    }
}

void Response2002::parse(vector<byte> received_bytes) {
    client_id = pop_string(received_bytes, 16);
    public_key = pop_string(received_bytes, 160);
}

void Response2003::parse(vector<byte> received_bytes) {
    client_id = pop_string(received_bytes, 16);
    message_id = pop<uint32_t>(received_bytes);
}

void Response2004::parse(vector<byte> received_bytes) {
    client_id = pop_string(received_bytes, 16);
    message_id = pop<uint32_t>(received_bytes);
    message_type = pop<uint8_t>(received_bytes);
    message_size = pop<uint32_t>(received_bytes);
    content = pop_string(received_bytes, message_size);
}
