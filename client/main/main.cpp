#include <iostream>
#include <cstddef>
#include <vector>
#include <boost/asio.hpp>

#include "user_interactor.h"
#include "requests.h"

using namespace boost::asio::ip;

int main(int argc, char* argv[]) {
    // Todo - resolve from file
    address_v4 server_address = make_address_v4("127.0.0.1");
    unsigned short server_port = 5442;
    Client client = Client(server_address, server_port);
    UserInteractor user_interactor(client);
    user_interactor.start_loop();
    return 0;
}
