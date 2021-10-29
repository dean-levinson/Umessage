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
    tcp::endpoint server_endpoint;

public:
    /**
     * Construct a new Communicator object.
     * 
     * @param server_endpoint - The server's endpoint.
     */
    Communicator(tcp::endpoint server_endpoint);

    /**
     * Creates TCP connection to the server. 
     */
    void connect();

    /**
     * Sends the bytes to the server by the given vector's order.
     * 
     * @param bytes - The bytes that should be sent.
     */
    void send_bytes(vector<byte> bytes);

    /**
     * Receives bytes by the given amount. 
     * 
     * @param size - The amount of bytes that should be received.
     * @return vector<byte> - Vector that contains the received bytes.
     */
    vector<byte> receive_bytes(size_t size);
    
    /**
     * Destroy the Communicator object - closes the communication socket.
     * 
     */
    ~Communicator();
};

#endif // COMMUNICATOR_H_
