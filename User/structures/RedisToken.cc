//
// Created by particleg on 2021/9/29.
//

#include <structures/RedisToken.h>

using namespace std;
using namespace tech::structures;

RedisToken::RedisToken(
        string refresh,
        string access
) : _refreshToken(move(refresh)),
    _accessToken(move(access)) {}

RedisToken::RedisToken(
        RedisToken &&redisToken
) noexcept: _refreshToken(move(redisToken._refreshToken)),
            _accessToken(move(redisToken._accessToken)) {}

string &RedisToken::refresh() { return _refreshToken; }

string &RedisToken::access() { return _accessToken; }

Json::Value RedisToken::parse() const {
    Json::Value result;
    result["refreshToken"] = _refreshToken;
    result["accessToken"] = _accessToken;
    return result;
}

