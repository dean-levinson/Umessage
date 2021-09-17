#include "user.h"

User::User(string uuid, string user_name): uuid(uuid), user_name(user_name) {}

vector<byte> User::get_pubkey() const {
    return pubkey;
}

void User::set_pubkey(const vector<byte> new_pubkey) {
    pubkey = new_pubkey;
}

vector<byte> User::get_symkey() const {
     return symkey;
}

void User::set_symkey(const vector<byte> new_symkey) {
    symkey = new_symkey;
}