#include <iostream>
#include <cstddef>
#include <vector>
#include <fstream>
#include <stdexcept>
#include <sstream>
#include <boost/asio.hpp>

#include "user_interactor.h"
#include "requests.h"

using namespace boost::asio::ip;

/**
 * Parse the server info from 'server.info' file which is located in cwd.
 * 'server.info' file format should be:
 * <server_ip>:<server_port>
 * 
 * @param server_address[out]
 * @param server_port[out]
 */
static void resolve_server_info_from_file(address_v4& server_address, unsigned short* server_port) {
    std::ifstream server_info_file("server.info");

    if (!server_info_file.is_open()) {
        throw std::runtime_error("Couldn't open server.info file");
    }

    // Read all file to server_info_data string
    std::stringstream ss;
    ss << server_info_file.rdbuf();
    string server_info_data = ss.str();
    server_info_file.close();

    server_address = make_address_v4(server_info_data.substr(0, server_info_data.find(':')));

    string server_port_str = server_info_data.substr(server_info_data.find(':') + 1, server_info_data.length());
    *server_port = std::stoi(server_port_str);
}

int main(int argc, char* argv[]) {
    address_v4 server_address;
    unsigned short server_port;
    resolve_server_info_from_file(server_address, &server_port);
    
    Client client = Client(server_address, server_port);
    UserInteractor user_interactor(client);
    user_interactor.start_loop();
    return 0;
}
