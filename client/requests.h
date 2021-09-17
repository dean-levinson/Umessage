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
    std::string client_id;
    uint8_t version;
    uint16_t code;
    uint32_t payload_size;

public:
    RequestHeaders(std::string client_id, uint8_t version, uint16_t code, uint32_t payload_size);
    vector<byte> build();
};

#endif // REQUESTS_H_
