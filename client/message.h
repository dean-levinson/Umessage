#ifndef MESSAGE_H_
#define MESSAGE_H_

#include <string>
using std::string;

class Message
{
public:
    uint32_t message_id;
    string from_client_id;
    uint8_t message_type;
    string content;
    Message(uint32_t message_id, string from_client_id, uint8_t message_type, string content);
};

#endif // MESSAGE_H_
