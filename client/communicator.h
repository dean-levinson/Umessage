#ifndef COMMUNICATOR_H_
#define COMMUNICATOR_H_

#include <vector>
#include <cstddef>
#include <boost/asio.hpp>

using namespace boost::asio::ip;
using boost::asio::ip::tcp;
using std::vector;
using std::byte;

class Communicator {
private:
    boost::asio::io_context context;
    tcp::socket socket;
    tcp::endpoint endpoint;

public:
    Communicator(tcp::endpoint endpoint);
    void connect();
    void send_bytes(vector<byte> bytes);
    // void send_encrypted_bytes(Encryptor enc, vector<byte> bytes);
    vector<byte> receive_bytes(size_t size);
    // vector<byte>& receive_encrypted_bytes(Decryptor dec, size_t size);
    // ~Communicator();
};

#endif // COMMUNICATOR_H_
