//
// Created by Parti on 2020/12/7.
//

#include <cryptopp/blake2.h>
#include <cryptopp/files.h>
#include <cryptopp/hex.h>
#include <cryptopp/keccak.h>
#include <cryptopp/osrng.h>
#include <cryptopp/panama.h>
#include <utils/crypto.h>

using namespace tech::utils;
using namespace CryptoPP;
using namespace std;

[[maybe_unused]] string crypto::panama::generateKey() {
    string encodedKey;
    SecByteBlock key(32);
    AutoSeededRandomPool().GenerateBlock(key, key.size());

    HexEncoder encoder;
    encoder.Attach(new StringSink(encodedKey));
    encoder.Put(reinterpret_cast<const CryptoPP::byte *> (key.data()), key.size());
    encoder.MessageEnd();
    return encodedKey;
}

[[maybe_unused]] std::string crypto::panama::generateIv() {
    string encodedIv;
    SecByteBlock iv(32);
    AutoSeededRandomPool().GenerateBlock(iv, iv.size());

    HexEncoder encoder;
    encoder.Attach(new StringSink(encodedIv));
    encoder.Put(reinterpret_cast<const CryptoPP::byte *> (iv.data()), iv.size());
    encoder.MessageEnd();
    return encodedIv;
}

[[maybe_unused]] string crypto::panama::encrypt(const std::string &source, std::string key, std::string iv) {
    string decodedKey, decodedIv, cipher, encodedCipher;

    HexDecoder decoder;
    decoder.Attach(new StringSink(decodedKey));
    decoder.Put(reinterpret_cast<CryptoPP::byte *> (key.data()), key.size());
    decoder.MessageEnd();

    decoder.Attach(new StringSink(decodedIv));
    decoder.Put(reinterpret_cast<CryptoPP::byte *> (iv.data()), iv.size());
    decoder.MessageEnd();

    PanamaCipher<LittleEndian>::Encryption enc;
    enc.SetKeyWithIV(reinterpret_cast<const CryptoPP::byte *> (decodedKey.data()),
                     decodedKey.size(),
                     reinterpret_cast<const CryptoPP::byte *> (decodedIv.data()),
                     decodedIv.size());
    cipher.resize(source.size());
    enc.ProcessData(reinterpret_cast<CryptoPP::byte *> (&cipher[0]),
                    reinterpret_cast<const CryptoPP::byte *> (source.data()),
                    source.size());

    HexEncoder encoder;
    encoder.Attach(new StringSink(encodedCipher));
    encoder.Put(reinterpret_cast<const CryptoPP::byte *> (cipher.data()), cipher.size());
    encoder.MessageEnd();
    return encodedCipher;
}

[[maybe_unused]] string crypto::panama::decrypt(std::string source, std::string key, std::string iv) {
    string decodedCipher, decodedKey, decodedIv, recover;

    HexDecoder decoder;
    decoder.Attach(new StringSink(decodedCipher));
    decoder.Put(reinterpret_cast<CryptoPP::byte *> (source.data()), source.size());
    decoder.MessageEnd();

    decoder.Attach(new StringSink(decodedKey));
    decoder.Put(reinterpret_cast<CryptoPP::byte *> (key.data()), key.size());
    decoder.MessageEnd();

    decoder.Attach(new StringSink(decodedIv));
    decoder.Put(reinterpret_cast<CryptoPP::byte *> (iv.data()), iv.size());
    decoder.MessageEnd();

    PanamaCipher<LittleEndian>::Decryption dec;
    dec.SetKeyWithIV(reinterpret_cast<const CryptoPP::byte *> (decodedKey.data()),
                     decodedKey.size(),
                     reinterpret_cast<const CryptoPP::byte *> (decodedIv.data()),
                     decodedIv.size());
    recover.resize(source.size());
    dec.ProcessData(reinterpret_cast<CryptoPP::byte *> (&recover[0]),
                    reinterpret_cast<const CryptoPP::byte *> (decodedCipher.data()),
                    decodedCipher.size());
    return recover;
}


string crypto::blake2B(const string &source, const unsigned int &divider) {
    stringstream tempStringStream;
    HexEncoder encoder(new FileSink(tempStringStream));
    string digest;
    BLAKE2b hash;
    hash.Update(reinterpret_cast<const CryptoPP::byte *>(source.data()), source.size());
    digest.resize(hash.DigestSize() / divider);
    hash.TruncatedFinal(reinterpret_cast<CryptoPP::byte *>(&digest[0]), digest.size());
    [[maybe_unused]] StringSource tempSource(digest, true, new Redirector(encoder));
    return tempStringStream.str();
}

string crypto::keccak(const string &source, const unsigned int &divider) {
    stringstream tempStringStream;
    HexEncoder encoder(new FileSink(tempStringStream));
    string digest;
    Keccak_512 hash;
    hash.Update(reinterpret_cast<const CryptoPP::byte *>(source.data()), source.size());
    digest.resize(hash.DigestSize() / divider);
    hash.TruncatedFinal(reinterpret_cast<CryptoPP::byte *>(&digest[0]), digest.size());
    [[maybe_unused]] StringSource tempSource(digest, true, new Redirector(encoder));
    return tempStringStream.str();
}