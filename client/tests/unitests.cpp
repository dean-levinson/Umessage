#include <iostream>
#include <cstddef>
#include <vector>
#include <boost/asio.hpp>

#include "user_interactor.h"
#include "requests.h"
#include "responses.h"

using namespace boost::asio::ip;

void test_request_headers_build() {
    // build target
    uint8_t target[] = {72, 101, 108, 108, 111, 32, 119, 111,
                    114, 108, 100, 0, 0, 0, 0, 0,
                    1, 2, 0, 3, 0, 0, 0};

    // build request
    std::string str = std::string("Hello world");
    RequestHeaders request = RequestHeaders(str, 1, 2, 3);
    std::vector<std::byte> bytes = request.build();
    
    // compare
    for(int i = 0; i < bytes.size(); ++i) {
        assert(target[i] == static_cast<uint8_t>(bytes[i]));
    }

    std::cout << "[V] test_request_headers_build passed" <<std::endl;
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

void test_response_headers_parse() {
    // build response
    uint8_t response_array[7] = {1, 232, 3, 14, 0, 0, 0};
    std::vector<std::byte> response_bytes;
    response_bytes.resize(sizeof(response_array));
    memcpy(&response_bytes[0], response_array, response_bytes.size());
    ResponseHeaders response;
    response.parse(std::move(response_bytes));

    // compare
    assert(response.version == 1);
    assert(response.code == 1000);
    assert(response.payload_size == 14);

    std::cout << "[V] test_response_headers_parse passed" <<std::endl;
}

void test_response_2000_parse() {
    // build response
    std::string response_str(300, '0');
    std::string tmp = string("Hello world");
    response_str.replace(0, tmp.size(), tmp);
    std::vector<std::byte> response_bytes;
    response_bytes.resize(response_str.size());
    memcpy(&response_bytes[0], &response_str[0], response_bytes.size());
    Response2000 response;
    response.parse(std::move(response_bytes));

    // build target
    std::string target = response_str;
    target.resize(255);

    // compare
    assert(response.client_id == target);

    std::cout << "[V] test_response_2000_parse passed" <<std::endl;
}

int main(int argc, char* argv[]) {
    test_request_headers_build();
    test_request_1000_build();
    test_response_headers_parse();
    test_response_2000_parse();

    std::cout << "Press any key to continue . . .";
    std::cin.get();
    return 0;
}
