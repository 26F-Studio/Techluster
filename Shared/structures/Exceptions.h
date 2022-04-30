//
// Created by particleg on 2021/10/2.
//

#pragma once

#include <drogon/drogon.h>
#include <helpers/MessageJson.h>
#include <types/JsonValue.h>
#include <types/MessageType.h>
#include <types/ResultCode.h>

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

    class ResponseException : public internal::BaseException {
    public:
        explicit ResponseException(
                std::string message,
                const types::ResultCode &code = types::ResultCode::internalError,
                const drogon::HttpStatusCode &statusCode = drogon::HttpStatusCode::k500InternalServerError
        );

        explicit ResponseException(
                std::string message,
                const std::exception &e,
                const types::ResultCode &code = types::ResultCode::internalError,
                const drogon::HttpStatusCode &statusCode = drogon::HttpStatusCode::k500InternalServerError
        );

        [[nodiscard]] const types::ResultCode &code() const noexcept;

        [[nodiscard]] const drogon::HttpStatusCode &statusCode() const noexcept;

        [[nodiscard]] Json::Value toJson() const noexcept;

    private:
        const std::string _reason;
        const types::ResultCode _code;
        const drogon::HttpStatusCode _statusCode;
    };

    class MessageException : public internal::BaseException {
    public:
        explicit MessageException(std::string message, bool error = false);

        std::atomic<bool> error;
    };

    namespace json_exception {
        class InvalidFormat : public internal::BaseException {
        public:
            explicit InvalidFormat(std::string message);
        };

        class WrongType : public internal::BaseException {
        public:
            explicit WrongType(const types::JsonValue &valueType);
        };
    }

    namespace sql_exception {
        class EmptyValue : public internal::BaseException {
        public:
            explicit EmptyValue(std::string message);
        };

        class NotEqual : public internal::BaseException {
        public:
            explicit NotEqual(std::string message);
        };
    }

    namespace redis_exception {
        class KeyNotFound : public internal::BaseException {
        public:
            explicit KeyNotFound(std::string message);
        };

        class FieldNotFound : public internal::BaseException {
        public:
            explicit FieldNotFound(std::string message);
        };
    }

    namespace room_exception {
        class PlayerOverFlow : public internal::BaseException {
        public:
            explicit PlayerOverFlow(std::string message);
        };

        class PlayerNotFound : public internal::BaseException {
        public:
            explicit PlayerNotFound(std::string message);
        };

        class RoomOverFlow : public internal::BaseException {
        public:
            explicit RoomOverFlow(std::string message);
        };

        class RoomNotFound : public internal::BaseException {
        public:
            explicit RoomNotFound(std::string message);
        };

        class InvalidPassword : public internal::BaseException {
        public:
            explicit InvalidPassword(std::string message);
        };
    }

    namespace action_exception {
        class ActionNotFound : public internal::BaseException {
        public:
            explicit ActionNotFound(std::string message);
        };

        class Unauthorized : public internal::BaseException {
        public:
            explicit Unauthorized(std::string message);
        };

        class InvalidArgument : public internal::BaseException {
        public:
            explicit InvalidArgument(std::string message);
        };
    }
}
