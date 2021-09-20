#include "client.h"
#include "requests.h"

#define CLIENT_VERSION (2)

Client::Client(tcp::endpoint endpoint): client_version(CLIENT_VERSION), comm(endpoint) {}

Client::Client(address_v4 address, unsigned short port): Client(tcp::endpoint(address, port)) {}

void Client::connect() {
    comm.connect();
}

void Client::register_client(string username) {
    Request1000 request_1000 = Request1000(username, pubkey);
    vector<byte> payload = request_1000.build();
    RequestHeaders request_headers = RequestHeaders(string(), client_version,
                                                    request_1000.get_code(), payload.size());
    comm.send_bytes(request_headers.build());
    comm.send_bytes(payload);
}
