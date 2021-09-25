#include "encryptor.h"

PublicEncryptor::PublicEncryptor(const string& key): rsapub(key) {}

std::string PublicEncryptor::encrypt(const string& plain) {
    return rsapub.encrypt(plain);
}

symmetricEncryptor::symmetricEncryptor(const string& key_v) {
    memcpy(key, &key_v[0], AESWrapper::DEFAULT_KEYLENGTH);
    AESWrapper aes(key, AESWrapper::DEFAULT_KEYLENGTH);
}

symmetricEncryptor::symmetricEncryptor() {
    AESWrapper aes(AESWrapper::GenerateKey(key, AESWrapper::DEFAULT_KEYLENGTH), AESWrapper::DEFAULT_KEYLENGTH);
}

std::string symmetricEncryptor::encrypt(const string& plain) {
    return aes.encrypt(plain.c_str(), plain.size());
}

std::string symmetricEncryptor::get_sym_key() {
    return std::string(reinterpret_cast<const char *>(aes.getKey()));
}