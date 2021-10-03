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

RedisException::KeyNotFound::KeyNotFound(std::string message) :
        BaseException(move(message)) {}

RedisException::NotEqual::NotEqual(std::string message) :
        BaseException(move(message)) {}
