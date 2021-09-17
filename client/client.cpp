#include "client.h"

Client::Client(tcp::endpoint endpoint): comm(endpoint) {}

Client::Client(address_v4 address, unsigned short port): Client(tcp::endpoint(address, port)) {}

void Client::connect() {
    comm.connect();
}