#ifndef CLIENT_H_
#define CLIENT_H_

#include <string>
#include <vector>
#include <map>
#include <list>
#include <map>
#include <exception>

#include "communicator.h"
#include "user.h"

using std::map;
using std::vector;
using std::string;
using std::byte;
using std::map;

class ServerError : public std::exception {  
    public:  
        const char * what() const throw(); 
};  

class Client {
private:
    uint8_t client_version;
    map<string, User> users;
    vector<byte> privkey;
    vector<byte> pubkey;
    std::string client_id;
    Communicator comm;
    
public:
    Client(tcp::endpoint endpoint);
    Client(address_v4 address, unsigned short port);
    void connect();
    string get_pubkey();
    // list<Message> get_messages();
    void connect(address_v4 address, unsigned short port);
    void register_client(string username);
    void send_message();
    void get_symmetric_key();
    void send_symmetric_key();

};

#endif // CLIENT_H_
