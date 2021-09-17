#ifndef CLIENT_H_
#define CLIENT_H_

#include <string>
#include <vector>
#include <map>
#include <list>

#include "communicator.h"

using std::map;
using std::vector;
using std::string;
using std::byte;

class Client {
private:
    // map<string, User> users;
    vector<byte> privkey;
    Communicator comm;
    
public:
    Client(tcp::endpoint endpoint);
    Client(address_v4 address, unsigned short port);
    ~Client();
    string get_pubkey();
    // list<Message> get_messages();
    void connect(address_v4 address, unsigned short port);
    void send_message();
    void get_symmetric_key();
    void send_symmetric_key();

};

#endif // CLIENT_H_
