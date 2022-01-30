//
// Created by particleg on 2021/10/2.
//

#include <structures/Exceptions.h>

#include <utility>

using namespace drogon;
using namespace std;
using namespace tech::internal;
using namespace tech::structures;

BaseException::BaseException(string message) : _message(move(message)) {}

char const *BaseException::what() const

noexcept {
return _message.

c_str();

}

CodeException::CodeException(string message, const int &code) :
        BaseException(move(message)), _code(code) {}

const int &CodeException::code() const

noexcept {
return
_code;
}

NetworkException::NetworkException(
        string message,
        const ReqResult &result
) : CodeException(move(message), TypePrefix::request + result) {}

sql_exception::EmptyValue::EmptyValue(string message) :
        BaseException(move(message)) {}

sql_exception::NotEqual::NotEqual(string message) :
        BaseException(move(message)) {}

redis_exception::KeyNotFound::KeyNotFound(string message) :
        BaseException(move(message)) {}

redis_exception::FieldNotFound::FieldNotFound(string message) :
        BaseException(move(message)) {}

redis_exception::NotEqual::NotEqual(string message) :
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
