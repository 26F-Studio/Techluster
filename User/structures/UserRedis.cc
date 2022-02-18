//
// Created by particleg on 2021/9/29.
//

#include <structures/Exceptions.h>
#include <structures/UserRedis.h>
#include <utils/crypto.h>

using namespace drogon;
using namespace std;
using namespace cpp_redis;
using namespace tech::structures;
using namespace tech::utils;

// TODO: Make some Redis actions run in pipeline

UserRedis::UserRedis(Expiration expiration) : RedisHelper("user"), _expiration(expiration) {}

UserRedis::UserRedis(UserRedis &&redis) noexcept: RedisHelper("user"), _expiration(redis._expiration) {}

RedisToken UserRedis::refresh(const string &refreshToken) {
    _extendRefreshToken(refreshToken);
    return {
            refreshToken,
            _generateAccessToken(
                    get("user:auth:refresh:" + refreshToken)
            )
    };
}

RedisToken UserRedis::generateTokens(const string &userId) {
    return {
            _generateRefreshToken(userId),
            _generateAccessToken(userId)
    };
}

void UserRedis::checkEmailCode(
        const string &email,
        const string &code
) {
    compare("user:auth:code:email:" + email, code);
}

void UserRedis::deleteEmailCode(const string &email) {
    del("user:auth:code:email:" + email);
}

void UserRedis::setEmailCode(
        const string &email,
        const string &code
) {
    setEx(
            "user:auth:code:email:" + email,
            _expiration.getEmailSeconds(),
            code
    );
}

int64_t UserRedis::getIdByAccessToken(const string &accessToken) {
    return stoll(get("user:auth:access:" + accessToken));
}

void UserRedis::_extendRefreshToken(const string &refreshToken) {
    expire(
            "user:auth:refresh:" + refreshToken,
            _expiration.refresh
    );
}

string UserRedis::_generateRefreshToken(const string &userId) {
    auto refreshToken = crypto::keccak(drogon::utils::getUuid());
    setEx(
            "user:auth:refresh:" + refreshToken,
            _expiration.getRefreshSeconds(),
            userId
    );
    return refreshToken;
}

string UserRedis::_generateAccessToken(const string &userId) {
    auto accessToken = crypto::blake2B(drogon::utils::getUuid());
    setEx(
            "user:auth:id:" + userId,
            _expiration.getAccessSeconds(),
            accessToken
    );
    setEx(
            "user:auth:access:" + accessToken,
            _expiration.getAccessSeconds(),
            userId
    );
    return accessToken;
}