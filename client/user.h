#ifndef USER_H_
#define USER_H_

#include <string>
#include <vector>
#include <cstddef>

using std::string;
using std::vector;
using std::byte;

class User
{
private:
    string client_id;
    string client_name;
    vector<byte> pubkey;
    vector<byte> symkey;

public:
    User(string client_id, string client_name);
    string get_client_id() const;
    string get_client_name() const;
    vector<byte> get_pubkey() const;
    void set_pubkey(const vector<byte> new_pubkey);
    vector<byte> get_symkey() const;
    void set_symkey(const vector<byte> new_symkey);
};

#endif // USER_H_
