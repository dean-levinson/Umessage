#include <iostream>
#include <algorithm>

#include "cryptopp_wrapper/RSAWrapper.h"
#include "cryptopp_wrapper/AESWrapper.h"
#include "client.h"
#include "requests.h"
#include "responses.h"

#define CLIENT_VERSION (2)

const char * ServerError::what() const throw() {
    return "Got server error";  
}

Client::Client(tcp::endpoint endpoint): client_version(CLIENT_VERSION), comm(endpoint) {
    RSAPrivateWrapper rsapriv;
    pubkey = rsapriv.getPublicKey();
    privkey = rsapriv.getPrivateKey();
}

Client::Client(address_v4 server_address, unsigned short server_port): Client(tcp::endpoint(server_address, server_port)) {}

void Client::connect() {
    comm.connect();
}

string Client::get_client_id_by_name(const string& target_client_name) const {
    return users.at(target_client_name).get_client_id();
}


User& Client::get_user_by_client_name(const string& target_client_name) {
    if (users.count(target_client_name)) {
        return users[target_client_name];
    }

    std::string err = "There is no user with client name - ";
    err += target_client_name;
    throw(std::out_of_range(err.c_str()));
}

User& Client::get_user_by_client_id(const string& target_client_id) {
    for (std::pair<const string, User>& p: users) {
        if (p.second.get_client_id() == target_client_id) {
            return p.second;
        }
    }

    std::string err = "There is no user with client id - ";
    err += target_client_id;
    throw(std::out_of_range(err.c_str()));
}

void Client::fetch_and_parse_response(ResponseCode& response) {
    ResponseHeaders response_headers; 
    response_headers.parse(comm.receive_bytes(response_headers.size()));

    if (response_headers.code == 9000) {
        throw ServerError();    
    }

    response.parse(comm.receive_bytes(response_headers.payload_size));
}

void Client::build_and_send_request(RequestCode& request) {
    vector<byte> request_payload = request.build();
    RequestHeaders request_headers = RequestHeaders(client_id, client_version,
                                                    request.get_code(),
                                                    request_payload.size());

    comm.send_bytes(request_headers.build());
    comm.send_bytes(request_payload);
}

void Client::register_client(string client_name) {
    Request1000 request_1000 = Request1000(client_name, pubkey);
    build_and_send_request(request_1000);

    Response2000 response;
    fetch_and_parse_response(response);

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
        [this] (const User& user) {add_user(user);});
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
    users.insert(std::pair<string, User>(user.get_client_name(), user));
}

void Client::send_text_message(string target_client_name, string text) {
    User& target_user = get_user_by_client_name(target_client_name);

    // Todo - function that wrappes this shit.
    AESWrapper aes(target_user.get_symkey(), AESWrapper::DEFAULT_KEYLENGTH);
    Request1003 request = Request1003(target_client_name, 3, aes.encrypt(text.c_str(), text.length());

    build_and_send_request(request);

    Response
}