#ifndef ENCRYPTOR_H_
#define ENCRYPTOR_H_

#include <string>

#include "cryptopp_wrapper/AESWrapper.h"
#include "cryptopp_wrapper/RSAWrapper.h"

using std::string;

class Encryptor
{
public:
    /**
     * Construct a new Encryptor object
     */
    Encryptor() {};

    /**
     * Abstract method.
     * Encrypts the given plain.
     * 
     * @param plain - The plain string to encrypt.
     * @return string - That contains the encrypted chars.
     */
    virtual string encrypt(const string& plain) = 0;
};

class PublicEncryptor: public Encryptor
{
private:
    RSAPublicWrapper rsapub;
public:
    /**
     * Construct a new Public Encryptor object using the given key.
     */
    PublicEncryptor(const string& key);

    /**
     * Overrides the abstract Encryptor::encrypt
     */
    string encrypt(const string& plain);
};


class SymmetricEncryptor: public Encryptor
{
private:
    unsigned char key[AESWrapper::DEFAULT_KEYLENGTH];
    AESWrapper aes;
public:
    /**
     * Construct a new Symmetric Encryptor object by generating a symmetric key.
     */
    SymmetricEncryptor();

    /**
     * Construct a new Symmetric Encryptor object by using the given symmetric key.
     */
    SymmetricEncryptor(const string& key);

    /**
     * Overrides the abstract Encryptor::encrypt
     */
    string encrypt(const string& plain); 

    /**
     * Returns a copy of the symmetric key.
     */
    string get_sym_key() const;
};

#endif // ENCRYPTOR_H_
