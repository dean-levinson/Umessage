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
    User();
    /**
     * Returns the client id.
     */
    string get_client_id() const;

    /**
     * Returns the client name.
     */
    string get_client_name() const;

    /**
     * Returns the public key.
     */
    string get_pubkey() const;

    /**
     * Sets the given public key.
     */
    void set_pubkey(const string new_pubkey);

    /**
     * Returns the symmetric key.
     */
    string get_symkey() const;

    /**
     * Sets the given symmetric key.
     */
    void set_symkey(const string new_symkey);
};

#endif // USER_H_
