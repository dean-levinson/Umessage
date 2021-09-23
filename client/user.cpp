#include "user.h"

User::User(string client_id, string client_name): client_id(client_id), client_name(client_name) {}

string User::get_client_id() const {
    return client_id;
}

string User::get_client_name() const {
    return client_name;
}

string User::get_pubkey() const {
    return pubkey;
}

void User::set_pubkey(const string new_pubkey) {
    pubkey = new_pubkey;
}

string User::get_symkey() const {
     return symkey;
}

void User::set_symkey(const string new_symkey) {
    symkey = new_symkey;
}