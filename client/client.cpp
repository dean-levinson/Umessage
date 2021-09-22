#include "client.h"
#include "requests.h"
#include "responses.h"

#define CLIENT_VERSION (2)

Client::Client(tcp::endpoint endpoint): client_version(CLIENT_VERSION), comm(endpoint) {}

Client::Client(address_v4 address, unsigned short port): Client(tcp::endpoint(address, port)) {}

void Client::connect() {
    comm.connect();
}

void Client::register_client(string username) {
    Request1000 request_1000 = Request1000(username, pubkey);
    vector<byte> request_payload = request_1000.build();
    RequestHeaders request_headers = RequestHeaders(string(), client_version,
                                                    request_1000.get_code(),
                                                    request_payload.size());
    comm.send_bytes(request_headers.build());
    comm.send_bytes(request_payload);

    ResponseHeaders response_headers;
    response_headers.parse(comm.receive_bytes(response_headers.size()));
    Response2000 response;
    response.parse(comm.receive_bytes(response_headers.payload_size));
    client_id = response.client_id;
}
