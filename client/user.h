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
    string uuid;
    string user_name;
    vector<byte> pubkey;
    vector<byte> symkey;

public:
    User(string uuid, string user_name);
    vector<byte> get_pubkey() const;
    void set_pubkey(const vector<byte> new_pubkey);
    vector<byte> get_symkey() const;
    void set_symkey(const vector<byte> new_symkey);
};

#endif // USER_H_
