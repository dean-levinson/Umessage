#ifndef CLIENT_H_
#define CLIENT_H_

#include <string>
#include <exception>
#include <vector>
#include <map>
#include <list>
#include <map>
#include <filesystem>

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

class Client {
private:
    uint8_t client_version;
    map<string, User> users; // [Client_name, User]
    string privkey;
    string pubkey;
    string client_id;
    string client_name;
    Communicator comm;

    /**
     * Builds the request payload and the request headers,
     * sends the header's bytes to the communicator, then
     * sends the payload's bytes to the communicator. 
     * 
     * @param request[in] - The RequestCode object that should be send.
     */
    void build_and_send_request(RequestCode& request);

    /**
     * Fetch the response header, parses it and according to the payload length,
     * Fetch the response payload and parse it into the given response.
     * 
     * @param response[out] - The ResponseCode object that should filled. 
     */
    void fetch_and_parse_response(ResponseCode& response);
    
    /**
     * Open "me.info" file from current directory.
     * If exists - read and update register info accordingly, and return true.
     * Else - return false. 
     * @return bool - Indicates weather the reading was successful.
     */
    bool get_register_info_from_file();

    /**
     * Creates "me.info" file with the register parameters, in the following format:
     * Line 1 - Full name.
     * Line 2 - Client id in ascii.
     * Line 3 - Private key in base64.
     */
    void update_register_info_into_file();

public:
    /**
     * Construct a new Client object.
     * 
     * @param endpoint - Server's endpoint details 
     */
    Client(tcp::endpoint endpoint);

    /**
     * Construct a new Client object.
     * 
     * @param server_address - Server's ipv4 address
     * @param server_port - Server's port
     */
    Client(address_v4 server_address, unsigned short server_port);
    
    /**
     * Get the client name of the client. If the client is not registered yet, returns empty string.
     * 
     * @return string 
     */
    string get_client_name();

    /**
     * Get the client id by the client name, from known users (resolved by get_client_list).
     * 
     * @param client_name - The client name
     * @return The client id
     * 
     * @throws NoSuchUser Thrown if there is no user from known users with such client id
     */
    string get_client_id_by_name(const string& client_name) const; 

    /**
     * Get the user object by the client id, from known users (resolved by get_client_list).
     * 
     * @param client_id - The client id
     * @return The user
     * 
     * @throws NoSuchUser Thrown if there is no user from known users with such client id 
     */
    User& get_user_by_client_id(const string& client_id);

    /**
     * Get the user object by the client name, from known users (resolved by get_client_list).
     * 
     * @param client_name - The client name
     * @return The user
     * 
     * @throws NoSuchUser Thrown if there is no user from known users with such client name 
     */
    User& get_user_by_client_name(const string& client_name);

    /**
     * Add the given user to the known users list.
     * 
     * @param user - The user that should be added.
     */
    void add_user(User user);

    /**
     * Connects the client to the server.
     * 
     */
    void connect();

    /**
     * Send register request to the server, with the given client_name and the client's public key (Which
     * has been determine in the initialization)
     * 
     * @param client_name - the client name to register with.
     * 
     * @throws ServerError thrown if the server didn't accept the registration, probably because there is already 
     * a user with the same client_name 
     */
    void register_client(string client_name);

    /**
     * Send get_client_list request to the server, and add the received users to users list.
     * 
     * @return list<User> - The users which have been received.
     */
    list<User> get_client_list();

    /**
     * Send get_public_key request to the server, and saves the received one in the user's object
     * in the users list.
     * 
     * @param client_id - The target client id for the request.  
     * @return The public key.
     */
    string get_public_key(string client_id);

    /**
     * Send text message to the given client_name, with the given text message.
     * 
     * @param target_client_name - The target of the message.
     * @param text - The text of the message.
     */
    void send_text_message(string target_client_name, string text);

    /**
     * Send pull_message_request from the server, handle every message with Client::handle_message
     * method, and returns the messages.
     * 
     * @return list<Message> - The messages, after handling with handle_message.
     */
    list<Message> pull_messages();

    /**
     * Send get_symmetric_key_request to the given target client (by name).
     * 
     * @param target_client_name - The name of the target client
     */
    void get_symmetric_key(const string& target_client_name);

    /**
     * Generates symmetric key for the communcation between the client and the target_client (by name),
     * Encrypts it using the target_client public key, and send it to the target_client.
     * 
     * @param target_client_name - The name of the target client.
     */
    void send_symmetric_key(const string& target_client_name);

    /**
     * Send file to target client.
     *
     * @param target_client_name - The target client to send to. 
     * @param file_path - The path of the file that should be sent.
     */
    void send_file(const string& target_client_name, const string& file_path);
    
    /**
     * Save the given content in temporary path under %TMP%.
     * Returns the full path of the new file. 
     * 
     * @param file_content - The content of the file
     * @return std::string - The path of the temporary file.
     */
    std::string save_temp_file(const string& file_content);

    /**
     * Handle a message from the client, and change it inplace.
     * 
     * case (send_symmetric_key) - Sends symmetric key to the client using Client::send_symmetric_key.
     * case (got_symmetric_key)  - Decrypts the given symmetric key with private key, and saves it in 
     *                             the users object.
     * case (got_text_message)   - Tries to decrypt it using the symmetric key. If it fails, change the messages content
     *                             to "Can't decrypt message".
     * case (got_file)           - Saves the file in temporary path.                          
     *  
     * @param message[in, out] - The message that should be handled
     * 
     * @throws NoSymmetricKey Throws if got message from client that we don't have his symmetric key. 
     */
    void handle_message(Message& message);
};

#endif // CLIENT_H_
