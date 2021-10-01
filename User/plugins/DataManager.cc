//
// Created by Parti on 2021/2/4.
//

#include <drogon/drogon.h>
#include <plugins/DataManager.h>
#include <utils/crypto.h>

using namespace drogon;
using namespace drogon_model;
using namespace drogon::nosql;
using namespace std;
using namespace sw::redis;
using namespace tech::plugins;
using namespace tech::structures;
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
    }
    ConnectionOptions options;
    options.host = config["redis"]["host"].asString();
    options.port = config["redis"]["port"].asInt();
    options.password = config["redis"]["password"].asString();
    options.db = config["redis"]["db"].asInt();

    if (config["redis"]["connections"].asUInt() == 0) {
        ConnectionPoolOptions poolOptions;
        poolOptions.size = thread::hardware_concurrency();
        _redisHelper = make_unique<RedisHelper>(move(RedisHelper(
                options,
                poolOptions,
                {
                        config["expirations"]["refresh"].asInt64(),
                        config["expirations"]["access"].asInt64(),
                        config["expirations"]["email"].asInt64()
                }
        )));
    } else if (config["redis"]["connections"].asUInt() > 1) {
        ConnectionPoolOptions poolOptions;
        poolOptions.size = config["redis"]["connections"].asUInt();
        _redisHelper = make_unique<RedisHelper>(move(RedisHelper(
                options,
                poolOptions,
                {
                        config["expirations"]["refresh"].asInt64(),
                        config["expirations"]["access"].asInt64(),
                        config["expirations"]["email"].asInt64()
                }
        )));
    } else {
        _redisHelper = make_unique<RedisHelper>(move(RedisHelper(
                options,
                {
                        config["expirations"]["refresh"].asInt64(),
                        config["expirations"]["access"].asInt64(),
                        config["expirations"]["email"].asInt64()
                }
        )));
    }

    _pgClient = app().getDbClient();
    _dataMapper = make_unique<
            Mapper<Techluster::Data>
    >(app().getDbClient());
    _playerMapper = make_unique<
            Mapper<Techluster::Player>
    >(app().getDbClient());

    LOG_INFO << "DataManager loaded.";
}

void DataManager::shutdown() { LOG_INFO << "DataManager shutdown."; }

RedisToken DataManager::refresh(const string &refreshToken) {
    return move(_redisHelper->refresh(refreshToken));
}

std::string DataManager::verifyEmail(const string &email) {
    auto code = drogon::utils::genRandomString(8);
    transform(
            code.begin(),
            code.end(),
            code.begin(),
            ::toupper
    );
    _redisHelper->setEmailCode(email, code);
    return code;
}

RedisToken DataManager::loginEmailCode(const string &email, const string &code) {
    _redisHelper->checkEmailCode(email, code);
    if (_playerMapper->count(Criteria(
            Techluster::Player::Cols::_email,
            CompareOperator::EQ,
            email
    )) == 0) {
        Techluster::Player newPlayer;
        newPlayer.setEmail(email);
        newPlayer.setPassword("Undefined");
        newPlayer.setUsername("email_" + email);
        _playerMapper->insert(newPlayer);
    }
    auto player = _playerMapper->findOne(Criteria(
            Techluster::Player::Cols::_email,
            CompareOperator::EQ,
            email
    ));
    _redisHelper->deleteEmailCode(email);
    return move(_redisHelper->generateTokens(
            to_string(player.getValueOfId())
    ));
}

RedisToken DataManager::loginEmailPassword(
        const string &email,
        const string &password
) {
    auto matchedUsers = _pgClient->execSqlSync(
            "select * from player "
            "where email = $1 "
            "and crypt($2, password) = password",
            email, password
    );
    if (matchedUsers.empty()) {
        throw range_error("No users found");
    }
    return move(_redisHelper->generateTokens(
            to_string(matchedUsers[0]["id"].as<int32_t>())
    ));
}

void DataManager::resetEmail(
        const string &email,
        const string &code,
        const string &newPassword
) {
    _redisHelper->checkEmailCode(email, code);
    if (_playerMapper->count(Criteria(
            Techluster::Player::Cols::_email,
            CompareOperator::EQ,
            email
    )) == 0) {
        throw out_of_range("No user found");
    } else {
        _pgClient->execSqlSync(
                "update player set "
                "password = crypt($1, gen_salt('bf', 10)) "
                "where email = $2",
                newPassword, email
        );
    }
    _redisHelper->deleteEmailCode(email);
}

void DataManager::migrateEmail(
        const string &accessToken,
        const string &newEmail,
        const string &code
) {
    auto player = _playerMapper->findOne(Criteria(
            Techluster::Player::Cols::_email,
            CompareOperator::EQ,
            _redisHelper->getUserId(accessToken)
    ));
    _redisHelper->checkEmailCode(newEmail, code);
    player.setEmail(newEmail);
    _playerMapper->update(player);
    _redisHelper->deleteEmailCode(newEmail);
}

Json::Value DataManager::getUserInfo(
        const string &accessToken,
        const int64_t &userId
) {
    auto id = userId;
    if (userId < 0) {
        id = _redisHelper->getUserId(accessToken);
    } else {
        _redisHelper->checkAccessToken(accessToken);
    }
    auto player = _playerMapper->findOne(Criteria(
            Techluster::Player::Cols::_id,
            CompareOperator::EQ,
            id
    ));
    Json::Value result;
    result["id"] = player.getValueOfId();
    result["avatar_frame"] = player.getValueOfAvatarFrame();
    result["avatar_hash"] = player.getValueOfAvatarHash();
    result["clan"] = player.getValueOfClan();
    result["motto"] = player.getValueOfMotto();
    result["region"] = player.getValueOfRegion();
    result["username"] = player.getValueOfUsername();
    return result;
}

void DataManager::updateUserInfo(
        const string &accessToken,
        const Json::Value &info
) {
    auto player = _playerMapper->findOne(Criteria(
            Techluster::Player::Cols::_id,
            CompareOperator::EQ,
            _redisHelper->getUserId(accessToken)
    ));
    bool updated = false;
    if (info.isMember("username") &&
        info["username"].isString() &&
        info["username"].asString() != player.getValueOfUsername()) {
        updated = true;
        player.setEmail(info["username"].asString());
    }
    if (info.isMember("motto") &&
        info["motto"].isString() &&
        info["motto"].asString() != player.getValueOfMotto()) {
        updated = true;
        player.setMotto(info["motto"].asString());
    }
    if (info.isMember("region") &&
        info["region"].isInt() &&
        info["region"].asInt() != player.getValueOfRegion()) {
        updated = true;
        player.setRegion(static_cast<short>(info["region"].asInt()));
    }
    if (info.isMember("avatar") &&
        info["avatar"].isString() &&
        info["avatar"].asString() != player.getValueOfAvatar()) {
        updated = true;
        player.setAvatar(info["avatar"].asString());
        player.setAvatarHash(crypto::blake2b(info["avatar"].asString(), 4));
    }
    if (info.isMember("avatar_frame") &&
        info["avatar_frame"].isInt() &&
        info["avatar_frame"].asInt() != player.getValueOfAvatarFrame()) {
        updated = true;
        player.setAvatarFrame(static_cast<short>(info["avatar_frame"].asInt()));
    }
    if (info.isMember("clan") &&
        info["clan"].isString() &&
        info["clan"].asString() != player.getValueOfClan()) {
        updated = true;
        player.setClan(info["clan"].asString());
    }
    if (!updated) {
        throw invalid_argument("Nothing changed");
    }
    _playerMapper->update(player);
}

string DataManager::getUserAvatar(
        const string &accessToken,
        const int64_t &userId
) {
    auto id = userId;
    if (userId < 0) {
        id = _redisHelper->getUserId(accessToken);
    } else {
        _redisHelper->checkAccessToken(accessToken);
    }
    auto player = _playerMapper->findOne(Criteria(
            Techluster::Player::Cols::_id,
            CompareOperator::EQ,
            id
    ));
    return player.getValueOfAvatar();
}

bool DataManager::ipLimit(const string &ip) const {
    return _redisHelper->tokenBucket(
            "ip:" + ip,
            _ipInterval,
            _ipMaxCount
    );
}

bool DataManager::emailLimit(const string &email) {
    return _redisHelper->tokenBucket(
            "email:" + email,
            _emailInterval,
            _emailMaxCount
    );
}

string DataManager::getUserData(
        const string &accessToken,
        const int64_t &userId,
        const DataField &field
) {
    auto id = userId;
    if (userId < 0) {
        id = _redisHelper->getUserId(accessToken);
    } else {
        _redisHelper->checkAccessToken(accessToken);
    }
    auto data = _dataMapper->findOne(Criteria(
            Techluster::Player::Cols::_id,
            CompareOperator::EQ,
            id
    ));
    string result;
    switch (field) {
        case DataField::kPublic:
            result = data.getValueOfPublic();
            break;
        case DataField::kProtected:
            result = data.getValueOfProtected();
            break;
        case DataField::kPrivate:
            result = data.getValueOfPrivate();
            break;
    }
    return result;
}

void DataManager::updateUserData(
        const string &accessToken,
        const int64_t &userId,
        const DataField &field,
        const string &info
) {
    auto id = userId;
    if (userId < 0) {
        id = _redisHelper->getUserId(accessToken);
    } else {
        _redisHelper->checkAccessToken(accessToken);
    }
    auto data = _dataMapper->findOne(Criteria(
            Techluster::Player::Cols::_id,
            CompareOperator::EQ,
            id
    ));
    switch (field) {
        case DataField::kPublic:
            data.setPublic(info);
            break;
        case DataField::kProtected:
            data.setProtected(info);
            break;
        case DataField::kPrivate:
            data.setPrivate(info);
            break;
    }
}
