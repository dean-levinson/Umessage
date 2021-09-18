#include <iostream>
#include <cstddef>
#include <vector>
#include <boost/asio.hpp>

#include "user_interactor.h"
#include "requests.h"

using namespace boost::asio::ip;

void test_requests_build() {
    std::string str = std::string("Hello world");
    uint8_t target[] = {72, 101, 108, 108, 111, 32, 119, 111,
                    114, 108, 100, 1, 2, 0, 3, 0, 0, 0};
    RequestHeaders request = RequestHeaders(str, 1, 2, 3);
    std::vector<std::byte> bytes = request.build();
    
    for(int i = 0; i < bytes.size(); ++i) {
        assert(target[i] == static_cast<uint8_t>(bytes[i]));
    }
    std::cout << "[V] test_requests_build passed" <<std::endl;
}

int main(int argc, char* argv[]) {
    test_requests_build();
    return 0;
}
