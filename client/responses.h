#ifndef RESPONSES_H_
#define RESPONSES_H_

#include <vector>
#include <string>
#include <cstddef>

using std::vector;
using std::string;
using std::byte;

class ResponseHeaders
{
public:
    uint8_t version;
    uint16_t code;
    uint32_t payload_size;

    // Thanks to move semantics implementation of std::vector,
    // if received_bytes is going to be a rvalue, It be will moved
    // instead of copied. This is good because usually the user of this 
    // class (the Client), will have no need in the Input vector
    // after this call. Therefore, there is no need to implement also
    // a const lvalue overload of this constructor.
    ResponseHeaders(vector<byte> received_bytes);
};

#endif // RESPONSES_H_
