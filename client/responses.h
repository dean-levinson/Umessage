#ifndef RESPONSES_H_
#define RESPONSES_H_

#include <vector>
#include <string>
#include <cstddef>
#include <list>

#include "user.h"

using std::vector;
using std::string;
using std::byte;
using std::list;

class ResponseHeaders
{
public:
    uint8_t version;
    uint16_t code;
    uint32_t payload_size;
    
    size_t size();

    // Thanks to move semantics implementation of std::vector,
    // if received_bytes is going to be a rvalue, It be will moved
    // instead of copied. This is good because usually the user of this 
    // class (the Client), will have no need in the Input vector
    // after this call. Therefore, there is no need to implement also
    // a const lvalue overload of this method.
    void parse(vector<byte> received_bytes);
};

class ResponseCode
{
public:
    virtual void parse(vector<byte> received_bytes) = 0;
};

class Response2000: public ResponseCode
{
public:
    std::string client_id;
    void parse(vector<byte> received_bytes);
};

class Response2001: public ResponseCode
{
public:
    list<User> client_list;
    void parse(vector<byte> received_bytes);
};

class Response2002: public ResponseCode
{
public:
    string client_id;
    string public_key;
    void parse(vector<byte> received_bytes);
};

#endif // RESPONSES_H_
