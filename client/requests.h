#ifndef REQUESTS_H_
#define REQUESTS_H_

#include <vector>
#include <string>
#include <cstddef>

using std::string;
using std::vector;
using std::byte;

#include <climits>

class RequestHeaders {
private:
    string client_id;
    uint8_t version;
    uint16_t code;
    uint32_t payload_size;

public:
    RequestHeaders(string client_id, uint8_t version, uint16_t code, uint32_t payload_size);
    vector<byte> build();
};

class RequestCode {
protected:
    uint16_t code;
public:
    virtual uint16_t get_code() const = 0;    
};

class Request1000: public RequestCode {
private:
    string name;
    vector<byte> pubkey;
public:
    Request1000(string name, vector<byte> pubkey);
    vector<byte> build();
    uint16_t get_code() const;
    };

#endif // REQUESTS_H_
