#include <iostream>
#include <algorithm>
#include <fstream>
#include <filesystem>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <stdio.h>
#include <cstdlib>
#include <iterator>
#include <boost/filesystem.hpp>

#include "cryptopp_wrapper/Base64Wrapper.h"
#include "client.h"
#include "requests.h"
#include "responses.h"
#include "encryptor.h"
#include "decryptor.h"
#include "message.h"
#include "exceptions.h"

#define CLIENT_VERSION (2)

static const char * INFO_FILE_PATH = "me.info";

static string ascii_to_hex(const string& ascii_str) {
    string result;
    for (auto it = ascii_str.begin(); it != ascii_str.end(); std::advance(it, 2)) {
        string ascii_var;
        ascii_var += *it;
        ascii_var += *std::next(it);
        result += static_cast<char>(std::stoi(ascii_var, 0, 16));
    }
    return result;
}

static string hex_to_ascii(const string& hex_str) {
    std::stringstream ss;
    for (auto it = hex_str.begin(); it != hex_str.end(); ++it) {
        ss << std::setw(2) << std::setfill('0') << std::hex << ((int)*it & 0xff);
    }
    return ss.str();
}

/**
 * Return string that contains the file data.
 */
static string get_file_content(const string& file_path) {
    if (!boost::filesystem::is_regular_file(file_path)) {
        throw(std::invalid_argument("Given file doesn't exist"));
    }
    
    std::ifstream file_obj(file_path, std::ios::binary);
    if (!file_obj.is_open()) {
        throw(std::runtime_error("Cannot open file"));
    }

    std::string content(std::istreambuf_iterator<char>{file_obj},
                        {});

    return content;
}

Client::Client(tcp::endpoint endpoint): client_version(CLIENT_VERSION), comm(endpoint), client_id(), client_name() {
    if (!get_register_info_from_file()) {
        // Generates new pair of keys because there is no register file.
        PublicDecryptor dec;
        pubkey = dec.get_public_key();
        privkey = dec.get_private_key();
    }
}

Client::Client(address_v4 server_address, unsigned short server_port): Client(tcp::endpoint(server_address, server_port)) {}

void Client::connect() {
    comm.connect();
}

string Client::get_client_name() {
    return client_name;
}

bool Client::get_register_info_from_file() {
    if (!std::filesystem::is_regular_file(INFO_FILE_PATH)) {
        return false;        
    }

    std::ifstream infofile(INFO_FILE_PATH);
    if (!infofile.is_open()) {
        throw(std::runtime_error("Info file exists but cannot be opened"));
    }

    string client_id_in_ascii;
    string privkey_in_base64;

    // Read Client name
    std::getline(infofile, this->client_name);

    // Read Client id in ascii
    std::getline(infofile, client_id_in_ascii);

    this->client_id = ascii_to_hex(client_id_in_ascii);

    // Read private key in base64. We cannot read one line because Base64Wrapper separates output into lines.
    // So we read until the end of the file with rdbuf.
    std::stringstream buffer;
    buffer << infofile.rdbuf();
    privkey_in_base64 = buffer.str();

    this->privkey = Base64Wrapper::decode(privkey_in_base64);

    infofile.close();

    return true;
}

void Client::update_register_info_into_file() {
    if (std::filesystem::is_regular_file(INFO_FILE_PATH)) {
        throw(std::runtime_error("Info file already exist"));
    }

    std::ofstream infofile(INFO_FILE_PATH);

    if (!infofile.is_open()) {
        throw(std::runtime_error("Info file exists but cannot be opened"));
    }

    // Write client_name
    infofile << client_name << std::endl;

    // Write client id in ascii
    infofile << hex_to_ascii(client_id) << std::endl;

    // Write private key in base64
    infofile << Base64Wrapper::encode(privkey) << std::endl;

    infofile.close();
}

string Client::get_client_id_by_name(const string& target_client_name) const {
    if (users.count(target_client_name)) {
        return users.at(target_client_name).get_client_id();
    }

    std::string err = "There is no other user with client name '" + target_client_name + "'";
    throw(NoSuchUser(err));
}


User& Client::get_user_by_client_name(const string& target_client_name) {
    if (users.count(target_client_name)) {
        return users.at(target_client_name);
    }

    std::string err = "There is no other user with client name '" + target_client_name + "'";
    throw(NoSuchUser(err));
}

User& Client::get_user_by_client_id(const string& target_client_id) {
    for (std::pair<const string, User>& p: users) {
        if (p.second.get_client_id() == target_client_id) {
            return p.second;
        }
    }

    std::string err = "There is no user with client id '" + target_client_id + "'";
    throw(NoSuchUser(err));
}

void Client::build_and_send_request(RequestCode& request) {
    vector<byte> request_payload = request.build();
    RequestHeaders request_headers = RequestHeaders(client_id, client_version,
                                                    request.get_code(),
                                                    request_payload.size());

    comm.send_bytes(request_headers.build());
    comm.send_bytes(request_payload);
}

void Client::fetch_and_parse_response(ResponseCode& response) {
    ResponseHeaders response_headers; 
    response_headers.parse(comm.receive_bytes(response_headers.size()));

    if (response_headers.code == 9000) {
        throw ServerError();    
    }

    response.parse(comm.receive_bytes(response_headers.payload_size));
}

void Client::register_client(string new_client_name) {
    if (client_id.size()) {
        throw(AlreadyRegistered());
    };

    Request1000 request_1000 = Request1000(new_client_name, pubkey);
    build_and_send_request(request_1000);

    Response2000 response;
    try {
        fetch_and_parse_response(response);
    } catch(const ServerError& e) {
        throw(ServerError("Registration Failed. Maybe the user already exists..."));
    }

    // // Add myself to the users list
    // User myself = User(response.client_id, new_client_name);
    // myself.set_pubkey(pubkey);
    // add_user(std::move(myself));

    client_name = new_client_name;
    client_id = response.client_id;

    // Update new register params into info file.
    update_register_info_into_file();
}

list<User> Client::get_client_list() {
    RequestHeaders request_headers = RequestHeaders(client_id, client_version,
                                                    1001,
                                                    0);
    comm.send_bytes(request_headers.build());

    Response2001 response;
    fetch_and_parse_response(response);

    std::for_each(response.client_list.begin(), response.client_list.end(),
        [this] (const User& user) {add_user(user); });
    return response.client_list;
}

string Client::get_public_key(std::string target_client_id) {
    Request1002 request = Request1002(target_client_id);
    build_and_send_request(request);

    Response2002 response;
    fetch_and_parse_response(response);
    
    User& target_user = get_user_by_client_id(target_client_id);
    target_user.set_pubkey(response.public_key);

    return response.public_key;
}

void Client::add_user(User user) {
    // don't add myself to list
    if (user.get_client_id() != this->client_id) {
        users.insert(std::pair<string, User>(user.get_client_name(), user));
    }
}

void Client::send_text_message(string target_client_name, string text) {
    User& target_user = get_user_by_client_name(target_client_name);

    if (target_user.get_symkey().size() == 0) {
        throw(NoSymmeticKey(target_user.get_client_name()));
    };

    if (target_user.get_client_id() == client_id) {
        throw(std::invalid_argument("Can't send yourself text messages"));
    };

    SymmetricEncryptor encryptor(target_user.get_symkey());
    Request1003 request(target_user.get_client_id(), 3, encryptor.encrypt(text));

    build_and_send_request(request);

    Response2003 response;
    fetch_and_parse_response(response);

    if (!(response.client_id == target_user.get_client_id())) {
        throw ServerError("Got wrong client_id in response 2003");
    }
}

void Client::get_symmetric_key(const string& target_client_name) {
    User& target_user = get_user_by_client_name(target_client_name);
    Request1003 request = Request1003(target_user.get_client_id(), 1, std::string());

    build_and_send_request(request);

    Response2003 response;
    fetch_and_parse_response(response);

    if (!(response.client_id == target_user.get_client_id())) {
        throw ServerError("Got wrong client_id in response 2003");
    }
}

void Client::send_symmetric_key(const string& target_client_name) {
    User& target_user = get_user_by_client_name(target_client_name);

    if (target_user.get_pubkey().size() == 0) {
        throw(NoPublicKey(target_user.get_client_name()));
    };

    if (!target_user.get_symkey().length()) {
        SymmetricEncryptor symenc = SymmetricEncryptor();
        string generated_key = symenc.get_sym_key();
        target_user.set_symkey(generated_key); // Set the new symmetric key to the target user
    }
    PublicEncryptor pubenc = PublicEncryptor(target_user.get_pubkey());
    string encrypted_payload = pubenc.encrypt(target_user.get_symkey());
    Request1003 request = Request1003(target_user.get_client_id(), 2, encrypted_payload);
    build_and_send_request(request);

    Response2003 response;
    fetch_and_parse_response(response);

    if (!(response.client_id == target_user.get_client_id())) {
        throw ServerError("Got wrong client_id in response 2003");
    }
}

void Client::send_file(const string& target_client_name, const string& file_path) {
    User& target_user = get_user_by_client_name(target_client_name);

    if (target_user.get_symkey().size() == 0) {
        throw(NoSymmeticKey(target_user.get_client_name()));
    };

    if (target_user.get_client_id() == client_id) {
        throw(std::invalid_argument("Can't send yourself a file"));
    };

    SymmetricEncryptor encryptor(target_user.get_symkey());

    std::string file_content = get_file_content(file_path); // todo - delete
    Request1003 request(target_user.get_client_id(), 4, encryptor.encrypt(get_file_content(file_path)));

    build_and_send_request(request);

    Response2003 response;
    fetch_and_parse_response(response);

    if (!(response.client_id == target_user.get_client_id())) {
        throw ServerError("Got wrong client_id in response 2003");
    }
}

list<Message> Client::pull_messages() {
    RequestHeaders request_headers = RequestHeaders(client_id, client_version,
                                                    1004, 0);
    comm.send_bytes(request_headers.build());
    
    Response2004 response;
    fetch_and_parse_response(response);

    for (Message& message: response.messages) {
        try {
            handle_message(message);
        } catch (const std::exception& e) {
            continue;
        }
    }

    return response.messages;
}

std::string Client::save_temp_file(const std::string& file_content) {
    boost::filesystem::path dir_path = getenv("TMP");
    boost::filesystem::path full_path;
    bool valid_path = false;
    while (!valid_path) {
        boost::filesystem::path file_name = tmpnam(NULL);
        full_path = dir_path / file_name;

        if (!boost::filesystem::exists(full_path)) {
            valid_path = true;
        }
    }

    std::ofstream tmp_file(full_path, std::ios::binary);
    if (!tmp_file.is_open()) {
        throw(std::runtime_error("Cannot open tmp file"));
    }

    tmp_file.write(file_content.c_str(), file_content.length());
    tmp_file.close();
    
    return full_path.string();
}

void Client::handle_message(Message& message) {
    try {
        get_user_by_client_id(message.from_client_id); // todo - fix reference problem
    } catch (const std::exception& e) {
        // Change the content of the message in case this exception will be caught.
        message.content = std::string("Failed resolving sender - Got error: ") + e.what();
        throw;
    }

    User& other_user = get_user_by_client_id(message.from_client_id);

    // todo - make const vars
    switch (message.message_type) {
        case 1:
        {   
            try {
                // send symmetric key to other.
                send_symmetric_key(other_user.get_client_name());
                message.content = std::string("Request for symmetric key. Creating and sending symmetric key...");
            } catch (const std::exception& e) {
                // Change the content of the message in case this exception will be caught.
                message.content = std::string("Failed sending symmetric key - Got error: ") + e.what();
                throw;
            }
            break;
        }
        case 2:
        {   
            try {
                PublicDecryptor decryptor(privkey);
                other_user.set_symkey(decryptor.decrypt(message.content));
                message.content = std::string("Symmetric key received. Accepting...");
            } catch (const std::exception& e) {
                // Change the content of the message in case this exception will be caught.
                message.content = std::string("Failed receiving symmetric key - Got error: ") + e.what();
                throw;
            }
            break;
        }
        case 3:
        {
            // Decrypt the message inplace.

            try {
                string other_symkey = other_user.get_symkey();
                if (!other_symkey.length()) {
                    throw(NoSymmeticKey(other_user.get_client_name()));
                }
                SymmetricDecryptor decryptor(other_symkey);
                message.content = decryptor.decrypt(message.content);

            } catch (const std::exception& e) {
                // Change the content of the message in case this exception will be caught.
                message.content = std::string("Couldn't decrypt message - Got error: ") + e.what();
                throw;
            }
            break;
        }

        case 4:
        {
            // Decrypt the file, and change the message content implace accordingly.
            try {
                string other_symkey = other_user.get_symkey();
                if (!other_symkey.length()) {
                    throw(NoSymmeticKey(other_user.get_client_name()));
                }
                SymmetricDecryptor decryptor(other_symkey);

                // Decrypt the file, and save it path in the message content.
                message.content = string("Got new file! ") + save_temp_file(decryptor.decrypt(message.content));

            } catch (const std::exception& e) {
                // Change the content of the message in case this exception will be caught.
                message.content = std::string("Couldn't decrypt file - Got error: ") + e.what();
                throw;
            }
            break;
        }
    }
}