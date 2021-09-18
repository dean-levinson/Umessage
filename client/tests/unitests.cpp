#include <iostream>
#include <cstddef>
#include <vector>
#include <boost/asio.hpp>

#include "user_interactor.h"
#include "requests.h"

using namespace boost::asio::ip;

void test_request_headers_build() {
    //build target
    uint8_t target[] = {72, 101, 108, 108, 111, 32, 119, 111,
                    114, 108, 100, 0, 0, 0, 0, 0,
                    1, 2, 0, 3, 0, 0, 0};

    //build request
    std::string str = std::string("Hello world");
    RequestHeaders request = RequestHeaders(str, 1, 2, 3);
    std::vector<std::byte> bytes = request.build();
    
    //compare
    for(int i = 0; i < bytes.size(); ++i) {
        assert(target[i] == static_cast<uint8_t>(bytes[i]));
    }

    std::cout << "[V] test_request_header_build passed" <<std::endl;
}

void test_request_1000_build() {
    std::string str = std::string("Hello world");
    std::vector<std::byte> key;
    key.push_back(std::byte(11));
    key.push_back(std::byte(12));
    key.push_back(std::byte(13));

    // build target
    uint8_t target[415] = { 0 };
    memcpy(&target[0], &str[0], str.size());
    memcpy(&target[255], &key[0], key.size());

    // build request
    Request1000 request = Request1000(str, key);
    std::vector<std::byte> bytes = request.build();
    
    // compare
    for(int i = 0; i < bytes.size(); ++i) {
        assert(target[i] == static_cast<uint8_t>(bytes[i]));
    }
    
    std::cout << "[V] test_request_1000_build passed" <<std::endl;
}

int main(int argc, char* argv[]) {
    test_request_headers_build();
    test_request_1000_build();

    std::cout << "Press any key to continue . . .";
    std::cin.get();
    return 0;
}
