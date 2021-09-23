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
    virtual vector<byte> build() const = 0;   
    virtual uint16_t get_code() const = 0; 
};

class Request1000: public RequestCode {
private:
    string name;
    string pubkey;
public:
    Request1000(string name, string pubkey);
    vector<byte> build() const;
    uint16_t get_code() const;
    };

class Request1002: public RequestCode {
private:
    string client_id;
public:
    Request1002(string client_id);
    vector<byte> build() const;
    uint16_t get_code() const;
    };

class Request1003: public RequestCode {
private:
    string client_id;
    uint8_t message_type;
    uint32_t content_size;
    string enc_text;
public:
    Request1003(string client_id, uint8_t message_type, uint32_t content_size, string enc_text);
    vector<byte> build() const;
    uint16_t get_code() const;
    };

#endif // REQUESTS_H_
