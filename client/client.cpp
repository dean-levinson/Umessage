#include <iostream>
#include <algorithm>

#include "cryptopp_wrapper/RSAWrapper.h"
#include "cryptopp_wrapper/AESWrapper.h"
#include "client.h"
#include "requests.h"
#include "responses.h"
#include "encryptor.h"
#include "decryptor.h"
#include "message.h"

#define CLIENT_VERSION (2)

ServerError::ServerError(): err() {}

ServerError::ServerError(std::string err): err(err) {}

ServerError::ServerError(const char* err): err(err) {}

const char * ServerError::what() const throw() {
    if (err.length() != 0) {
        return err.c_str();
    }

    return "Server error"; 
}

NoSuchUser::NoSuchUser(std::string err_msg): internal_message(err_msg) {
    internal_message += "\nConsider calling 'Get clients list'...";
}

NoSuchUser::NoSuchUser(const char * err_msg): NoSuchUser(std::string(err_msg)) {}

const char * NoSuchUser::what() const throw() {
    return internal_message.c_str();  
}

NoPublicKey::NoPublicKey(const std::string& client_name): client_name(client_name) {
    err = string("User ") + "'" + client_name + "''s public key not found.\n"
     "Consider calling 'Request for public key'...";
}

const char * NoPublicKey::what() const throw() {
    return err.c_str();
}

NoSymmeticKey::NoSymmeticKey(const std::string& client_name): client_name(client_name) {
    err = string("User ") + "'" + client_name + "''s symmetric key not found.\n"
     "Consider calling 'Send a request for symmetric key'...";
}

const char * NoSymmeticKey::what() const throw() {
    return err.c_str();
}

Client::Client(tcp::endpoint endpoint): client_version(CLIENT_VERSION), comm(endpoint) {
    // todo - read details from info file
    PublicDecryptor dec;
    pubkey = dec.get_public_key();
    privkey = dec.get_private_key();
}

Client::Client(address_v4 server_address, unsigned short server_port): Client(tcp::endpoint(server_address, server_port)) {}

void Client::connect() {
    comm.connect();
}

string Client::get_client_id_by_name(const string& target_client_name) const {
    if (users.count(target_client_name)) {
        return users.at(target_client_name).get_client_id();
    }

    std::string err = "There is no user with client name '" + target_client_name + "'";
    throw(NoSuchUser(err));
}


User& Client::get_user_by_client_name(const string& target_client_name) {
    if (users.count(target_client_name)) {
        return users.at(target_client_name);
    }

    std::string err = "There is no user with client name '" + target_client_name + "'";
    throw(NoSuchUser(err));
}

User& Client::get_user_by_client_id(const string& target_client_id) {
    for (std::pair<const string, User>& p: users) {
        if (p.second.get_client_id() == target_client_id) {
            return p.second;
        }
    }

    std::string err = "There is no user with client id '" + target_client_id + "'";
    throw(NoSuchUser(err));
}

void Client::build_and_send_request(RequestCode& request) {
    vector<byte> request_payload = request.build();
    RequestHeaders request_headers = RequestHeaders(client_id, client_version,
                                                    request.get_code(),
                                                    request_payload.size());

    comm.send_bytes(request_headers.build());
    comm.send_bytes(request_payload);
}

void Client::fetch_and_parse_response(ResponseCode& response) {
    ResponseHeaders response_headers; 
    response_headers.parse(comm.receive_bytes(response_headers.size()));

    if (response_headers.code == 9000) {
        throw ServerError();    
    }

    response.parse(comm.receive_bytes(response_headers.payload_size));
}

void Client::register_client(string client_name) {
    // Todo - if client_id member is set, throw excpetion ClientAlreadyRegistered
    Request1000 request_1000 = Request1000(client_name, pubkey);
    build_and_send_request(request_1000);

    Response2000 response;
    try {
        fetch_and_parse_response(response);
    } catch(const ServerError& e) {
        throw(ServerError("Registration Failed. Maybe the user already exists..."));
    }

    // Add myself to the users list
    User myself = User(response.client_id, client_name);
    myself.set_pubkey(pubkey);
    add_user(std::move(myself));

    client_id = response.client_id;
}

list<User> Client::get_client_list() {
    RequestHeaders request_headers = RequestHeaders(client_id, client_version,
                                                    1001,
                                                    0);
    comm.send_bytes(request_headers.build());

    Response2001 response;
    fetch_and_parse_response(response);

    std::for_each(response.client_list.begin(), response.client_list.end(),
        [this] (const User& user) {add_user(user); });
    return response.client_list;
}

string Client::get_public_key(std::string target_client_id) {
    Request1002 request = Request1002(target_client_id);
    build_and_send_request(request);

    Response2002 response;
    fetch_and_parse_response(response);
    
    User& target_user = get_user_by_client_id(target_client_id);
    target_user.set_pubkey(response.public_key);

    return response.public_key;
}

void Client::add_user(User user) {
    // don't add myself to list
    if (user.get_client_id() != this->client_id) {
        users.insert(std::pair<string, User>(user.get_client_name(), user));
    }
}

void Client::send_text_message(string target_client_name, string text) {
    User& target_user = get_user_by_client_name(target_client_name);

    if (target_user.get_symkey().size() == 0) {
        throw(NoSymmeticKey(target_user.get_client_name()));
    };

    SymmetricEncryptor encryptor(target_user.get_symkey());
    Request1003 request(target_user.get_client_id(), 3, encryptor.encrypt(text));

    build_and_send_request(request);

    Response2003 response;
    fetch_and_parse_response(response);

    if (!(response.client_id == target_user.get_client_id())) {
        throw ServerError("Got wrong client_id in response 2003");
    }
}

void Client::get_symmetric_key(string target_client_name) {
    User& target_user = get_user_by_client_name(target_client_name);
    Request1003 request = Request1003(target_user.get_client_id(), 1, std::string());

    build_and_send_request(request);

    Response2003 response;
    fetch_and_parse_response(response);

    if (!(response.client_id == target_user.get_client_id())) {
        throw ServerError("Got wrong client_id in response 2003");
    }
}

void Client::send_symmetric_key(string target_client_name) {
    User& target_user = get_user_by_client_name(target_client_name);

    if (target_user.get_pubkey().size() == 0) {
        throw(NoPublicKey(target_user.get_client_name()));
    };

    SymmetricEncryptor symenc = SymmetricEncryptor();
    string generated_key = symenc.get_sym_key();
    target_user.set_symkey(generated_key); // Set the new symmetric key to the target user
    PublicEncryptor pubenc = PublicEncryptor(target_user.get_pubkey());
    string encrypted_payload = pubenc.encrypt(generated_key);
    Request1003 request = Request1003(target_user.get_client_id(), 2, encrypted_payload);
    build_and_send_request(request);

    Response2003 response;
    fetch_and_parse_response(response);

    if (!(response.client_id == target_user.get_client_id())) {
        throw ServerError("Got wrong client_id in response 2003");
    }
}

list<Message> Client::pull_messages() {
    RequestHeaders request_headers = RequestHeaders(client_id, client_version,
                                                    1004, 0);
    comm.send_bytes(request_headers.build());
    
    Response2004 response;
    fetch_and_parse_response(response);

    for (Message& message: response.messages) {
        try {
            handle_message(message);
        } catch (const std::exception& e) {
            std::cout << "DEBUG: passing message handling because of error: " << e.what() << std::endl;
        }
    }

    return response.messages;
}

void Client::handle_message(Message& message) {
    User& other_user = get_user_by_client_id(message.from_client_id);
    // todo - make const vars
    switch (message.message_type) {
        case 1:
        {
            // send symmetric key to other.
            send_symmetric_key(other_user.get_client_name());
            break;
        }
        case 2:
        {   
            // Decrypt and save the new symmetric key.
            if (other_user.get_pubkey().size() == 0) {
                throw(NoPublicKey(other_user.get_client_name()));
            };

            PublicDecryptor decryptor(privkey);
            other_user.set_symkey(decryptor.decrypt(message.content));
            break;
        }
        case 3:
        {
            // Decrypt the message inplace. Probably will change dua refactor.

            try {
                string other_symkey = other_user.get_symkey();
                if (!other_symkey.length()) {
                    throw(NoSymmeticKey(other_user.get_client_name()));
                }
                SymmetricDecryptor decryptor(other_symkey);
                message.content = decryptor.decrypt(message.content);

            } catch (const std::exception& e) {
                // Change the content of the message in case this exception will be caught.
                message.content = std::string("Can't decrypt message");
                throw;
            }
            break;
        }

        case 4:
            // file - TODO
            break;
    }
}