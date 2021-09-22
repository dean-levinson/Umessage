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
using std::list;
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
    Client(address_v4 server_address, unsigned short server_port);
    void connect();
    void register_client(string client_name);
    list<User> get_client_list();
    string get_public_key() const;
    void send_message() const;
    // list<Message> get_messages();
    void get_symmetric_key() const;
    void send_symmetric_key();

};

#endif // CLIENT_H_
