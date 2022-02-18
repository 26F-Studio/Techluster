//
// Created by ParticleG on 2022/2/3.
//

#pragma once

#include <drogon/drogon.h>

namespace tech::internal {
    template<typename T>
    concept Enumerable = std::is_enum_v<T>;

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
    std::string randomString(const uint64_t &length);

    template<internal::Enumerable T>
    constexpr auto operator+(const T &e) noexcept {
        return static_cast<std::underlying_type_t<T>>(e);
    }

    template<internal::Enumerable T1, internal::Enumerable T2>
    constexpr auto operator+(const T1 &t1, const T2 &t2) noexcept {
        return (+t1) + (+t2);
    }
}



