#include <iostream>

#include "client.h"
#include "requests.h"
#include "responses.h"

#define CLIENT_VERSION (2)

const char * ServerError::what() const throw() {
    return "Got server error";  
}

Client::Client(tcp::endpoint endpoint): client_version(CLIENT_VERSION), comm(endpoint) {}

Client::Client(address_v4 server_address, unsigned short server_port): Client(tcp::endpoint(server_address, server_port)) {}

void Client::connect() {
    comm.connect();
}

string Client::get_client_id(string client_name) const {
    return users.at(client_id).get_client_id();
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

    client_id = response.client_id;
}

list<User> Client::get_client_list() {
    RequestHeaders request_headers = RequestHeaders(client_id, client_version,
                                                    1001,
                                                    0);
    comm.send_bytes(request_headers.build());

    Response2001 response;
    fetch_and_parse_response(response);

    return response.client_list;
}

vector<byte> Client::get_public_key(std::string target_client_id) {
    Request1002 request = Request1002(target_client_id);
    build_and_send_request(request);

    Response2002 response;
    fetch_and_parse_response(response);

    return response.public_key;
}