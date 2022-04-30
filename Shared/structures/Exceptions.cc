//
// Created by particleg on 2021/10/2.
//

#include <helpers/ResponseJson.h>
#include <magic_enum.hpp>
#include <structures/Exceptions.h>

using namespace drogon;
using namespace magic_enum;
using namespace std;
using namespace tech::helpers;
using namespace tech::internal;
using namespace tech::structures;
using namespace tech::types;

BaseException::BaseException(string message) : _message(move(message)) {}

char const *BaseException::what() const noexcept { return _message.c_str(); }

CodeException::CodeException(string message, const int &code) :
        BaseException(move(message)), _code(code) {}

const int &CodeException::code() const noexcept { return _code; }

NetworkException::NetworkException(
        string message,
        const ReqResult &result
) : CodeException(move(message), enum_integer(TypePrefix::request) + enum_integer(result)) {}

ResponseException::ResponseException(
        string message,
        const ResultCode &code,
        const drogon::HttpStatusCode &statusCode
) : BaseException(move(message)), _code(code), _statusCode(statusCode) {}

ResponseException::ResponseException(
        string message,
        const exception &e,
        const ResultCode &code,
        const HttpStatusCode &statusCode
) : BaseException(move(message)), _code(code), _statusCode(statusCode), _reason(e.what()) {}

const ResultCode &ResponseException::code() const noexcept { return _code; }

const drogon::HttpStatusCode &ResponseException::statusCode() const noexcept { return _statusCode; }

Json::Value ResponseException::toJson() const noexcept {
    ResponseJson result;
    result.setResultCode(_code);
    result.setMessage(_message);
    if (!_reason.empty()) {
        result.setReason(_reason);
    }
    return result.ref();
}

MessageException::MessageException(
        string message,
        bool error
) : BaseException(move(message)), error(error) {}

json_exception::InvalidFormat::InvalidFormat(std::string message) :
        BaseException(move(message)) {}

json_exception::WrongType::WrongType(const JsonValue &valueType) :
        BaseException(string(enum_name(valueType))) {}

sql_exception::EmptyValue::EmptyValue(string message) :
        BaseException(move(message)) {}

sql_exception::NotEqual::NotEqual(string message) :
        BaseException(move(message)) {}

redis_exception::KeyNotFound::KeyNotFound(string message) :
        BaseException(move(message)) {}

redis_exception::FieldNotFound::FieldNotFound(string message) :
        BaseException(move(message)) {}

room_exception::PlayerOverFlow::PlayerOverFlow(string message) :
        BaseException(move(message)) {}

room_exception::PlayerNotFound::PlayerNotFound(string message) :
        BaseException(move(message)) {}

room_exception::RoomOverFlow::RoomOverFlow(string message) :
        BaseException(move(message)) {}

room_exception::RoomNotFound::RoomNotFound(string message) :
        BaseException(move(message)) {}

room_exception::InvalidPassword::InvalidPassword(string message) :
        BaseException(move(message)) {}

action_exception::ActionNotFound::ActionNotFound(string message) :
        BaseException(move(message)) {}

action_exception::Unauthorized::Unauthorized(string message) :
        BaseException(move(message)) {}

action_exception::InvalidArgument::InvalidArgument(string message) :
        BaseException(move(message)) {}
