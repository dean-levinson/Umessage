#include <boost/asio.hpp>
#include <iostream>
#include <vector>

#include "communicator.h"

using namespace boost::asio::ip;
using boost::asio::ip::tcp;

Communicator::Communicator(tcp::endpoint endpoint): context(), socket(context) {
    socket.connect(endpoint);
    // socket.connect(tcp::endpoint(address, port));
}

void Communicator::send_bytes(vector<byte> bytes) {
    socket.send(boost::asio::buffer(bytes));
}

vector<byte> Communicator::receive_bytes(size_t size) {
    auto received_bytes = vector<byte>(size);
    size_t read_bytes = read(socket, boost::asio::buffer(received_bytes));

    // Not sure if I need this assertion. socket.read_some / socket.recv is like python socket.recv - 
    // It doesn't guarantee to read all the bytes. read supposed to read all the bytes. 
    assert(size == received_bytes.size());
    return received_bytes;
}
