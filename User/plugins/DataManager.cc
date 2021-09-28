//
// Created by Parti on 2021/2/4.
//

#include <drogon/drogon.h>
#include <plugins/DataManager.h>
#include <utils/crypto.h>
#include <utils/datetime.h>

using namespace drogon;
using namespace drogon_model;
using namespace drogon::nosql;
using namespace std;
using namespace sw::redis;
using namespace tech::plugins;
using namespace tech::utils;

void DataManager::initAndStart(const Json::Value &config) {
    if (!(
            config.isMember("tokenBucket") && config["tokenBucket"].isObject() &&
            config["tokenBucket"].isMember("ip") && config["tokenBucket"]["ip"].isObject() &&
            config["tokenBucket"].isMember("email") && config["tokenBucket"]["email"].isObject() &&

            config["tokenBucket"]["ip"].isMember("interval") && config["tokenBucket"]["ip"]["interval"].isUInt64() &&
            config["tokenBucket"]["ip"].isMember("maxCount") && config["tokenBucket"]["ip"]["maxCount"].isUInt64() &&
            config["tokenBucket"]["email"].isMember("interval") && config["tokenBucket"]["email"]["interval"].isUInt64() &&
            config["tokenBucket"]["email"].isMember("maxCount") && config["tokenBucket"]["email"]["maxCount"].isUInt64()
    )) {
        LOG_ERROR << R"(Invalid tokenBucket config)";
        abort();
    } else {
        _ipInterval = chrono::seconds(config["tokenBucket"]["ip"]["interval"].asUInt64());
        _ipMaxCount = config["tokenBucket"]["ip"]["maxCount"].asUInt64();
        _emailInterval = chrono::seconds(config["tokenBucket"]["email"]["interval"].asUInt64());
        _emailMaxCount = config["tokenBucket"]["email"]["maxCount"].asUInt64();
    }

    if (!(
            config.isMember("expirations") && config["expirations"].isObject() &&
            config["expirations"].isMember("refresh") && config["expirations"]["refresh"].isInt64() &&
            config["expirations"].isMember("access") && config["expirations"]["access"].isInt64() &&
            config["expirations"].isMember("email") && config["expirations"]["email"].isInt64()
    )) {
        LOG_ERROR << R"("Invalid expirations config")";
        abort();
    } else {
        refreshExpiration = config["expirations"]["refresh"].asInt64();
        accessExpiration = config["expirations"]["access"].asInt64();
        emailExpiration = config["expirations"]["email"].asInt64();
    }

    if (!(
            config.isMember("redis") && config["redis"].isObject() &&
            config["redis"].isMember("host") && config["redis"]["host"].isString() &&
            config["redis"].isMember("port") && config["redis"]["port"].isInt() &&
            config["redis"].isMember("password") && config["redis"]["password"].isString() &&
            config["redis"].isMember("db") && config["redis"]["db"].isInt() &&
            config["redis"].isMember("connections") && config["redis"]["connections"].isUInt()
    )) {
        LOG_ERROR << R"("Invalid redis config")";
        abort();
    } else {
        ConnectionOptions options;
        options.host = config["redis"]["host"].asString();
        options.port = config["redis"]["port"].asInt();
        options.password = config["redis"]["password"].asString();
        options.db = config["redis"]["db"].asInt();

        if (config["redis"]["connections"].asUInt() == 0) {
            ConnectionPoolOptions poolOptions;
            poolOptions.size = thread::hardware_concurrency();
            _redisClient = make_unique<Redis>(options, poolOptions);
        } else if (config["redis"]["connections"].asUInt() > 1) {
            ConnectionPoolOptions poolOptions;
            poolOptions.size = config["redis"]["connections"].asUInt();
            _redisClient = make_unique<Redis>(options, poolOptions);
        } else {
            _redisClient = make_unique<Redis>(options);
        }
    }

    _pgClient = app().getDbClient();
    _playerMapper = make_unique<Mapper<Techluster::Player>>(app().getDbClient());

    LOG_INFO << "DataManager loaded.";
}

void DataManager::shutdown() { LOG_INFO << "DataManager shutdown."; }

DataManager::Token DataManager::refresh(const string &refreshToken) {
    auto result = _redisClient->get("player:refresh:" + refreshToken);
    if (!result) {
        throw out_of_range("Invalid refreshToken");
    }
    _redisClient->expire(
            "player:refresh:" + refreshToken,
            chrono::minutes(refreshExpiration)
    );
    return {refreshToken, _generateAccessToken(result.value())};
}

std::string DataManager::verifyEmail(const string &email) {
    auto verifyCode = drogon::utils::genRandomString(8);
    transform(
            verifyCode.begin(),
            verifyCode.end(),
            verifyCode.begin(),
            ::toupper
    );
    if (!_redisClient->set(
            "player:code:email_" + email,
            verifyCode,
            chrono::minutes(emailExpiration)
    )) {
        throw range_error("Set player:code:email_" + email + " Failed");
    }
    return verifyCode;
}

DataManager::Token DataManager::loginEmailCode(const string &email, const string &code) {
    auto result = _redisClient->get("player:code:email_" + email);
    if (!result) {
        throw out_of_range("Invalid email");
    }
    if (result.value() != code) {
        throw range_error("Invalid code");
    }
    if (_playerMapper->count(Criteria(
            Techluster::Player::Cols::_email,
            CompareOperator::EQ,
            email
    )) == 0) {
        Techluster::Player newPlayer;
        newPlayer.setEmail(email);
        newPlayer.setUsername("email_" + email);
        _playerMapper->insert(newPlayer);
    }
    auto player = _playerMapper->findOne(Criteria(
            Techluster::Player::Cols::_email,
            CompareOperator::EQ,
            email
    ));
    _redisClient->del("player:code:email_" + email);
    return generateTokens(to_string(player.getValueOfId()));
}

DataManager::Token DataManager::loginEmailPassword(const string &email, const string &password) {
    auto matchedUsers = _pgClient->execSqlSync(
            "select * from player "
            "where email = $1 "
            "and crypt($2, password) = password",
            email, password
    );
    if (matchedUsers.empty()) {
        throw orm::RangeError("No users found");
    }
    return generateTokens(to_string(matchedUsers[0]["_id"].as<int32_t>()));
}

DataManager::Token DataManager::loginWeChat(const string &code) {
    // TODO: Implement logic
    return {};
}

bool DataManager::coolDown(const string &key, const chrono::seconds &interval) {
    if (_redisClient->exists(key)) {
        _redisClient->expire(key, interval);
        return false;
    }
    if (!_redisClient->set(key, "", interval)) {
        throw range_error("Set " + key + " Failed");
    }
    return true;
}

string DataManager::_generateRefreshToken(const string &userId) {
    auto refreshToken = crypto::keccak(drogon::utils::getUuid());
    if (!_redisClient->set(
            "player:refresh:" + refreshToken,
            userId,
            chrono::minutes(refreshExpiration)
    )) {
        throw range_error("Set player:refresh:" + refreshToken + " Failed");
    }
    return refreshToken;
}

string DataManager::_generateAccessToken(const string &userId) {
    auto accessToken = crypto::blake2b(drogon::utils::getUuid());
    if (!_redisClient->set(
            "player:access:" + userId,
            accessToken,
            chrono::minutes(accessExpiration)
    )) {
        throw range_error("Set player:access:" + userId + " Failed");
    }
    return accessToken;
}

DataManager::Token DataManager::generateTokens(const std::string &userId) {
    return {_generateRefreshToken(userId), _generateAccessToken(userId)};
}

bool DataManager::tokenBucket(
        const string &key,
        const chrono::microseconds &restoreInterval,
        const uint64_t &maxCount
) const {
    auto maxTTL = chrono::duration_cast<chrono::seconds>(restoreInterval * maxCount);

    auto setCount = [this, &key](const uint64_t &count) {
        if (!_redisClient->set(
                "tokenBucketCount:" + key,
                to_string(count)
        )) {
            throw range_error("Set tokenBucketCount:" + key + " Failed");
        }
    };

    auto setDate = [this, &key](const string &dateStr) {
        if (!_redisClient->set(
                "tokenBucketUpdated:" + key,
                dateStr
        )) {
            throw range_error("Set tokenBucketUpdated:" + key + " Failed");
        }
    };

    auto checkCount = [this, &key](const uint64_t &count) -> bool {
        if (count > 0) {
            _redisClient->decr("tokenBucketCount:" + key);
            return true;
        }
        return false;
    };

    auto bucketCount = _redisClient->get("tokenBucketCount:" + key);

    uint64_t countValue;

    if (!bucketCount) {
        setCount(maxCount - 1);
        countValue = maxCount;
    } else {
        countValue = stoull(bucketCount.value());
    }

    bool hasToken = true;
    auto lastUpdated = _redisClient->get("tokenBucketUpdated:" + key);
    if (!lastUpdated) {
        setDate(datetime::toString());
        setCount(maxCount - 1);
    } else {
        auto nowMicroseconds = datetime::toDate().microSecondsSinceEpoch();
        auto generatedCount =
                (nowMicroseconds -
                 datetime::toDate(lastUpdated.value()).microSecondsSinceEpoch()
                ) / restoreInterval.count() - 1;

        if (generatedCount >= 1) {
            setDate(datetime::toString(nowMicroseconds));
            _redisClient->incrby("tokenBucketCount:" + key, generatedCount - 1);
            hasToken = true;
        } else {
            hasToken = checkCount(countValue);
        }
    }
    _redisClient->expire("tokenBucketCount:" + key, maxTTL);
    _redisClient->expire("tokenBucketUpdated:" + key, maxTTL);
    return hasToken;
}
