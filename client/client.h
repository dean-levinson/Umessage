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
#include "message.h"

using std::map;
using std::vector;
using std::string;
using std::list;
using std::byte;
using std::map;

class ServerError : public std::exception {  
    private:
    std::string err;

    public:  
        ServerError();    
        ServerError(std::string err);
        ServerError(const char* err);
        const char * what() const throw(); 
};  

class NoSuchUser : public std::exception {  
    private:
        std::string internal_message;
    public:  
        NoSuchUser(const char * err_message);
        NoSuchUser(std::string err_message);
        const char * what() const throw(); 
};  

class NoPublicKey : public std::exception {  
    private:
        std::string err;
        std::string client_name;
    public:  
        NoPublicKey(const std::string& client_id);
        const char * what() const throw(); 
};  

class NoSymmeticKey : public std::exception {  
    private:
        std::string err;
        std::string client_name;
    public:  
        NoSymmeticKey(const std::string& client_id);
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
    string get_client_id_by_name(const string& client_name) const; 
    User& get_user_by_client_id(const string& client_id);
    User& get_user_by_client_name(const string& client_name);
    void add_user(User user);
    void connect();
    void register_client(string client_name);
    list<User> get_client_list();
    string get_public_key(string client_id);
    void send_text_message(string target_client_name, string text);
    list<Message> pull_messages();
    void get_symmetric_key(string target_client_name);
    void send_symmetric_key(string target_client_name);
    void handle_message(Message& message);
};

#endif // CLIENT_H_
