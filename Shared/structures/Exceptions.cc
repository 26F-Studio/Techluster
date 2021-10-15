//
// Created by particleg on 2021/10/2.
//

#include <structures/Exceptions.h>

using namespace drogon;
using namespace std;
using namespace tech::internal;
using namespace tech::structures;

BaseException::BaseException(std::string message) : _message(move(message)) {}

char const *BaseException::what() const noexcept { return _message.c_str(); }

CodeException::CodeException(std::string message, const int &code) :
        BaseException(move(message)), _code(code) {}

const int &CodeException::code() const noexcept { return _code; }

NetworkException::NetworkException(
        string message,
        const ReqResult &result
) : CodeException(move(message), TypePrefix::request + result) {}

redis_exception::KeyNotFound::KeyNotFound(std::string message) :
        BaseException(move(message)) {}

redis_exception::FieldNotFound::FieldNotFound(std::string message) :
        BaseException(move(message)) {}

redis_exception::NotEqual::NotEqual(std::string message) :
        BaseException(move(message)) {}

room_exception::PlayerOverFlow::PlayerOverFlow(std::string message) :
        BaseException(move(message)) {}

room_exception::PlayerNotFound::PlayerNotFound(std::string message) :
        BaseException(move(message)) {}

room_exception::RoomOverFlow::RoomOverFlow(std::string message) :
        BaseException(move(message)) {}

room_exception::RoomNotFound::RoomNotFound(std::string message) :
        BaseException(move(message)) {}

room_exception::InvalidPassword::InvalidPassword(std::string message) :
        BaseException(move(message)) {}

action_exception::ActionNotFound::ActionNotFound(std::string message) :
        BaseException(move(message)) {}

action_exception::Unauthorized::Unauthorized(std::string message) :
        BaseException(move(message)) {}

action_exception::InvalidArgument::InvalidArgument(std::string message) :
        BaseException(move(message)) {}
