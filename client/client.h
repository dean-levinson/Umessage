#ifndef CLIENT_H_
#define CLIENT_H_

#include <string>
#include <vector>
#include <map>
#include <list>
#include <map>
#include <exception>

#include "responses.h"
#include "requests.h"
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
    map<string, User> users; // [Client_name, User]
    string privkey;
    string pubkey;
    string client_id;
    Communicator comm;
    void fetch_and_parse_response(ResponseCode& response);
    void build_and_send_request(RequestCode& request);
    
public:
    Client(tcp::endpoint endpoint);
    Client(address_v4 server_address, unsigned short server_port);
    string get_client_id(const string& client_name) const; 
    User& get_user_by_client_id(const string& client_id);
    void add_user(User user);
    void connect();
    void register_client(string client_name);
    list<User> get_client_list();
    string get_public_key(string client_id);
    void send_message();
    // list<Message> get_messages();
    void get_symmetric_key();
    void send_symmetric_key();
};

#endif // CLIENT_H_
