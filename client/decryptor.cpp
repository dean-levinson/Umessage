#include "decryptor.h"

Decryptor::Decryptor() {}

PublicDecryptor::PublicDecryptor(const string& key): rsapriv(key) {}

PublicDecryptor::PublicDecryptor(): rsapriv() {}

string PublicDecryptor::decrypt(const string& cipher) {
   return rsapriv.decrypt(cipher);
}

string PublicDecryptor::get_public_key() const {
    return rsapriv.getPublicKey();
}

string PublicDecryptor::get_private_key() const {
    return rsapriv.getPrivateKey();
}

SymmetricDecryptor::SymmetricDecryptor(const string& key_v) {
    memcpy(key, &key_v[0], AESWrapper::DEFAULT_KEYLENGTH);
    aes = AESWrapper(key, AESWrapper::DEFAULT_KEYLENGTH);
}

SymmetricDecryptor::SymmetricDecryptor():
    aes(AESWrapper::GenerateKey(key, AESWrapper::DEFAULT_KEYLENGTH), AESWrapper::DEFAULT_KEYLENGTH)
    {}

string SymmetricDecryptor::decrypt(const string& cipher) {
    return aes.decrypt(cipher.c_str(), cipher.length());
}

std::string SymmetricDecryptor::get_sym_key() const {
    // Because std::string doesn't get const unsigned char * type,
    // we use reinterpret_cast.
    return std::string(reinterpret_cast<const char *>(aes.getKey()));
}
