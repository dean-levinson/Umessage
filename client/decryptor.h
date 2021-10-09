#ifndef DECRYPTOR_H_
#define DECRYPTOR_H_

#include <string>

#include "cryptopp_wrapper/AESWrapper.h"
#include "cryptopp_wrapper/RSAWrapper.h"

using std::string;

class Decryptor {
private:
    /* data */
public:
    Decryptor();
    virtual string decrypt(const string& cipher) = 0; // abstract class
};

class PublicDecryptor: public Decryptor
{
private:
    RSAPrivateWrapper rsapriv;
public:
    PublicDecryptor(const string& key);
    PublicDecryptor();
    string decrypt(const string& cipher);
    string get_public_key() const;
    string get_private_key() const;
};

class SymmetricDecryptor: public Decryptor
{
private:
    unsigned char key[AESWrapper::DEFAULT_KEYLENGTH];
    AESWrapper aes;
public:
    SymmetricDecryptor();
    SymmetricDecryptor(const string& key);
    string decrypt(const string& cipher);
    string get_sym_key() const;
};


#endif // DECRYPTOR_H_
