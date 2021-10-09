#include <cassert>

#include "encryptor.h"

PublicEncryptor::PublicEncryptor(const string& key): rsapub(key) {
}

std::string PublicEncryptor::encrypt(const string& plain) {
    return rsapub.encrypt(plain);
}

SymmetricEncryptor::SymmetricEncryptor(const string& key_v) {
    memcpy(key, &key_v[0], AESWrapper::DEFAULT_KEYLENGTH);
    aes = AESWrapper(key, AESWrapper::DEFAULT_KEYLENGTH);
}

SymmetricEncryptor::SymmetricEncryptor():
    aes(AESWrapper::GenerateKey(key, AESWrapper::DEFAULT_KEYLENGTH), AESWrapper::DEFAULT_KEYLENGTH)
    {}

std::string SymmetricEncryptor::encrypt(const string& plain) {
    return aes.encrypt(plain.c_str(), plain.size());
}

std::string SymmetricEncryptor::get_sym_key() const {
    // Because std::string doesn't get const unsigned char * type,
    // we use reinterpret_cast.
    return std::string(reinterpret_cast<const char *>(aes.getKey()));
}
