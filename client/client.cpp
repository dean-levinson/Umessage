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

void Client::register_client(string client_name) {
    Request1000 request_1000 = Request1000(client_name, pubkey);
    vector<byte> request_payload = request_1000.build();
    RequestHeaders request_headers = RequestHeaders(string(), client_version,
                                                    request_1000.get_code(),
                                                    request_payload.size());
    comm.send_bytes(request_headers.build());
    comm.send_bytes(request_payload);

    ResponseHeaders response_headers;
    response_headers.parse(comm.receive_bytes(response_headers.size()));

    // todo - add consts file
    if (response_headers.code == 9000) {
        throw ServerError();    
    }

    Response2000 response;
    response.parse(comm.receive_bytes(response_headers.payload_size));
    client_id = response.client_id;
}

list<User> Client::get_client_list() {
    RequestHeaders request_headers = RequestHeaders(string(), client_version,
                                                    1001,
                                                    0);
    comm.send_bytes(request_headers.build());

    ResponseHeaders response_headers;
    response_headers.parse(comm.receive_bytes(response_headers.size()));

    if (response_headers.code == 9000) {
        throw ServerError();    
    }

    Response2001 response;
    response.parse(comm.receive_bytes(response_headers.payload_size));
    return response.client_list;
}