#ifndef USER_H_
#define USER_H_

#include <string>
#include <vector>
#include <cstddef>

using std::string;

class User
{
private:
    string client_id;
    string client_name;
    string pubkey;
    string symkey;

public:
    User(string client_id, string client_name);
    string get_client_id() const;
    string get_client_name() const;
    string get_pubkey() const;
    void set_pubkey(const string new_pubkey);
    string get_symkey() const;
    void set_symkey(const string new_symkey);
};

#endif // USER_H_
