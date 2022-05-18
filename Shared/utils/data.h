//
// Created by ParticleG on 2022/2/3.
//

#pragma once

#include <drogon/drogon.h>

namespace tech::internal {
    template<typename T>
    concept JsonTypes =
    std::convertible_to<T, bool> ||
    std::convertible_to<T, Json::Int> ||
    std::convertible_to<T, Json::Int64> ||
    std::convertible_to<T, Json::UInt> ||
    std::convertible_to<T, Json::UInt64> ||
    std::convertible_to<T, double> ||
    std::convertible_to<T, Json::String>;
};

namespace tech::utils::data {
    std::string randomString(uint64_t length);
}



