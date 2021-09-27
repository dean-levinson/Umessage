#include "message.h"

Message::Message(uint32_t message_id, string from_client_id, uint8_t message_type, string content):
    message_id(message_id),
    from_client_id(from_client_id),
    message_type(message_type),
    content(content) {};
