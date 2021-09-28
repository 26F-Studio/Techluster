//
// Created by Parti on 2020/12/7.
//

#pragma once

#include <string>

namespace tech::utils::crypto {
    namespace panama {
        std::string generateKey();

        std::string generateIV();

        std::string encrypt(const std::string &source, std::string key, std::string iv);

        std::string decrypt(std::string source, std::string key, std::string iv);
    }

    std::string blake2b(const std::string &source, const unsigned int &divider = 2);

    std::string keccak(const std::string &source, const unsigned int &divider = 2);
}

