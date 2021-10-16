#include "exceptions.h"

using std::string;

ServerError::ServerError(): err() {}

ServerError::ServerError(std::string err): err(err) {}

ServerError::ServerError(const char* err): err(err) {}

const char * ServerError::what() const throw() {
    if (err.length() != 0) {
        return err.c_str();
    }

    return "Server error"; 
}

NoSuchUser::NoSuchUser(std::string err_msg): internal_message(err_msg) {
    internal_message += "\nConsider calling 'Get clients list'...";
}

NoSuchUser::NoSuchUser(const char * err_msg): NoSuchUser(std::string(err_msg)) {}

const char * NoSuchUser::what() const throw() {
    return internal_message.c_str();  
}

NoPublicKey::NoPublicKey(const std::string& client_name): client_name(client_name) {
    err = string("User ") + "'" + client_name + "''s public key not found.\n"
     "Consider calling 'Request for public key'...";
}

const char * NoPublicKey::what() const throw() {
    return err.c_str();
}

NoSymmeticKey::NoSymmeticKey(const std::string& client_name): client_name(client_name) {
    err = string("User ") + "'" + client_name + "''s symmetric key not found.\n"
     "Consider calling 'Send a request for symmetric key'...";
}

const char * NoSymmeticKey::what() const throw() {
    return err.c_str();
}

const char* AlreadyRegistered::what() const throw() {
    return "User is already registered";
}