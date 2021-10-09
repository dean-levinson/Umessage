#ifndef ENCRYPTOR_H_
#define ENCRYPTOR_H_

#include <string>

#include "cryptopp_wrapper/AESWrapper.h"
#include "cryptopp_wrapper/RSAWrapper.h"

using std::string;

class Encryptor
{
public:
    Encryptor() {};
    virtual string encrypt(const string& plain) = 0; // Abstract class
};

class PublicEncryptor: public Encryptor
{
private:
    RSAPublicWrapper rsapub;
public:
    PublicEncryptor(const string& key);
    string encrypt(const string& plain);
};


class SymmetricEncryptor: public Encryptor
{
private:
    unsigned char key[AESWrapper::DEFAULT_KEYLENGTH];
    AESWrapper aes;
public:
    SymmetricEncryptor();
    SymmetricEncryptor(const string& key);
    string encrypt(const string& plain); 
    string get_sym_key() const;
};

#endif // ENCRYPTOR_H_
