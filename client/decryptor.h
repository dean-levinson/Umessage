#ifndef DECRYPTOR_H_
#define DECRYPTOR_H_

#include <string>

#include "cryptopp_wrapper/AESWrapper.h"
#include "cryptopp_wrapper/RSAWrapper.h"

using std::string;

class Decryptor {
public:
    /**
     * Construct a new Decryptor object
     */
    Decryptor();

    /**
     * Abstract Method.
     * Decrypts the given cipher string.
     * 
     * @param cipher - The cipher to decrypt
     * @return string - String contains the plain chars.
     */
    virtual string decrypt(const string& cipher) = 0; 
};

class PublicDecryptor: public Decryptor
{
private:
    RSAPrivateWrapper rsapriv;
public:
    /**
     * Construct a new Public Decryptor object
     * 
     * @param key - The private key.
     */
    PublicDecryptor(const string& key);

    /**
     * Construct a new Public Decryptor object.
     * Generates a pair of public and private key. 
     */
    PublicDecryptor();

    /**
     * * Overrides abstract Decryptor::decrypt
     */
    string decrypt(const string& cipher);

    /**
     * Returns a copy of the public key
     */
    string get_public_key() const;

    /**
     * Returns a copy of the private key
     */
    string get_private_key() const;
};

class SymmetricDecryptor: public Decryptor
{
private:
    unsigned char key[AESWrapper::DEFAULT_KEYLENGTH];
    AESWrapper aes;
public:
    /**
     * Construct a new Symmetric Decryptor object -
     * Generates a symmetric key.
     * 
     */
    SymmetricDecryptor();

    /**
     * Construct a new Symmetric Decryptor object -
     * Using the given symmetric key.
     * 
     * @param key - The symmetric key to use.
     */
    SymmetricDecryptor(const string& key);

    /**
     * Overrides abstract Decryptor::decrypt
     */
    string decrypt(const string& cipher);

    /**
     * Returns a copy of the symmetric key.
     */
    string get_sym_key() const;
};


#endif // DECRYPTOR_H_
