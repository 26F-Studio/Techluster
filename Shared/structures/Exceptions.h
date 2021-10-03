//
// Created by particleg on 2021/10/2.
//

#pragma once

#include <drogon/drogon.h>

namespace tech::internal {
    class BaseException : public std::exception {
    public:
        explicit BaseException(std::string message);

        ~BaseException() noexcept override = default;

        [[nodiscard]] const char *what() const noexcept override;

    protected:
        const std::string _message;
    };

    class CodeException : public BaseException {
    public:
        explicit CodeException(std::string message, const int &code = 0);

        [[nodiscard]] const int &code() const noexcept;

    protected:
        const int _code;
    };
}

namespace tech::structures {
    class NetworkException : public internal::CodeException {
    public:
        enum class TypePrefix {
            request = 600,
            websocket = 700,
        };

        explicit NetworkException(
                std::string message,
                const drogon::ReqResult &result = drogon::ReqResult::NetworkFailure
        );
    };

    static int operator+(const NetworkException::TypePrefix &prefix, const drogon::ReqResult &result) {
        return static_cast<int>(prefix) + static_cast<int>(result);
    }

    namespace RedisException {
        class KeyNotFound : public internal::BaseException {
        public:
            explicit KeyNotFound(std::string message);
        };

        class NotEqual : public internal::BaseException {
        public:
            explicit NotEqual(std::string message);
        };
    }
}
