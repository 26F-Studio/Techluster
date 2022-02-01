//
// Created by Parti on 2021/2/4.
//

#include <drogon/drogon.h>
#include <structures/Exceptions.h>
#include <plugins/DataManager.h>
#include <structures/JsonHelper.h>
#include <utils/crypto.h>

using namespace drogon;
using namespace drogon_model;
using namespace std;
using namespace tech::plugins;
using namespace tech::structures;
using namespace tech::utils;

void DataManager::initAndStart(const Json::Value &config) {
    if (!(
            config["tokenBucket"]["ip"]["interval"].isUInt64() &&
            config["tokenBucket"]["ip"]["maxCount"].isUInt64() &&
            config["tokenBucket"]["email"]["interval"].isUInt64() &&
            config["tokenBucket"]["email"]["maxCount"].isUInt64()
    )) {
        LOG_ERROR << R"(Invalid tokenBucket config)";
        abort();
    }
    _ipInterval = chrono::seconds(config["tokenBucket"]["ip"]["interval"].asUInt64());
    _ipMaxCount = config["tokenBucket"]["ip"]["maxCount"].asUInt64();
    _emailInterval = chrono::seconds(config["tokenBucket"]["email"]["interval"].asUInt64());
    _emailMaxCount = config["tokenBucket"]["email"]["maxCount"].asUInt64();

    if (!(
            config["expirations"]["refresh"].isInt64() &&
            config["expirations"]["access"].isInt64() &&
            config["expirations"]["email"].isInt64()
    )) {
        LOG_ERROR << R"("Invalid expirations config")";
        abort();
    }
    _redisHelper = make_unique<RedisHelper>(move(RedisHelper(
            {
                    chrono::minutes(config["expirations"]["refresh"].asInt64()),
                    chrono::minutes(config["expirations"]["access"].asInt64()),
                    chrono::minutes(config["expirations"]["email"].asInt64())
            }
    )));

    try {
        _redisHelper->connect(
                config["redis"]["host"].asString(),
                config["redis"]["port"].asUInt(),
                config["redis"]["timeout"].asUInt()
        );
    } catch (const cpp_redis::redis_error &e) {
        LOG_ERROR << e.what();
        abort();
    }

    _pgClient = app().getDbClient();
    _dataMapper = make_unique<orm::Mapper<techluster::Data>>(app().getDbClient());
    _playerMapper = make_unique<orm::Mapper<techluster::Player>>(app().getDbClient());

    LOG_INFO << "DataManager loaded.";
}

void DataManager::shutdown() {
    _redisHelper->disconnect();
    LOG_INFO << "DataManager shutdown.";
}

int64_t DataManager::getUserId(const string &accessToken) {
    return _redisHelper->getIdByAccessToken(accessToken);
}

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
    if (_playerMapper->count(orm::Criteria(
            techluster::Player::Cols::_email,
            orm::CompareOperator::EQ,
            email
    )) == 0) {
        techluster::Player newPlayer;
        newPlayer.setEmail(email);
        _playerMapper->insert(newPlayer);
        techluster::Data newData;
        _dataMapper->insert(newData);
    }
    auto player = _playerMapper->findOne(orm::Criteria(
            techluster::Player::Cols::_email,
            orm::CompareOperator::EQ,
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
    auto player = _playerMapper->findOne(orm::Criteria(
            techluster::Player::Cols::_email,
            orm::CompareOperator::EQ,
            email
    ) && orm::Criteria(
            techluster::Player::Cols::_password,
            orm::CompareOperator::EQ,
            password
    ));

    if (player.getValueOfPassword().empty()) {
        throw sql_exception::EmptyValue("password is empty");
    }
    return _redisHelper->generateTokens(to_string(player.getValueOfId()));
}

void DataManager::resetEmail(
        const string &email,
        const string &code,
        const string &newPassword
) {
    _redisHelper->checkEmailCode(email, code);
    auto player = _playerMapper->findOne(orm::Criteria(
            techluster::Player::Cols::_email,
            orm::CompareOperator::EQ,
            email
    ));
    player.setPassword(newPassword);
    _playerMapper->update(player);
    _redisHelper->deleteEmailCode(email);
}

void DataManager::migrateEmail(
        const string &accessToken,
        const string &newEmail,
        const string &code
) {
    auto player = _playerMapper->findOne(orm::Criteria(
            techluster::Player::Cols::_id,
            orm::CompareOperator::EQ,
            _redisHelper->getIdByAccessToken(accessToken)
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
        id = _redisHelper->getIdByAccessToken(accessToken);
    } else {
        _redisHelper->checkAccessToken(accessToken);
    }
    auto player = _playerMapper->findOne(orm::Criteria(
            techluster::Player::Cols::_id,
            orm::CompareOperator::EQ,
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
    auto player = _playerMapper->findOne(orm::Criteria(
            techluster::Player::Cols::_id,
            orm::CompareOperator::EQ,
            _redisHelper->getIdByAccessToken(accessToken)
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
        player.setAvatarHash(crypto::blake2B(info["avatar"].asString(), 4));
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
        id = _redisHelper->getIdByAccessToken(accessToken);
    } else {
        _redisHelper->checkAccessToken(accessToken);
    }

    auto player = _playerMapper->findOne(orm::Criteria(
            techluster::Player::Cols::_id,
            orm::CompareOperator::EQ,
            id
    ));
    return player.getValueOfAvatar();
}

Json::Value DataManager::getUserData(
        const string &accessToken,
        const int64_t &userId,
        const DataField &field,
        const Json::Value &request
) {
    if (!request["paths"].isArray()) {
        throw invalid_argument("'paths' must be an array");
    }
    // TODO: Only allow access to other user's protected data if they are friends
    auto id = userId;
    if (userId < 0) {
        id = _redisHelper->getIdByAccessToken(accessToken);
    } else {
        if (field == DataField::kProtected || field == DataField::kPrivate) {
            throw invalid_argument("Cannot access other's protected data");
        }
        _redisHelper->checkAccessToken(accessToken);
    }
    auto data = _dataMapper->findOne(orm::Criteria(
            techluster::Player::Cols::_id,
            orm::CompareOperator::EQ,
            id
    ));
    string rawString;
    switch (field) {
        case DataField::kPublic:
            rawString = data.getValueOfPublic();
            break;
        case DataField::kProtected:
            rawString = data.getValueOfProtected();
            break;
        case DataField::kPrivate:
            rawString = data.getValueOfPrivate();
            break;
    }
    Json::Value output;
    auto source = JsonHelper(rawString);
    for (const auto &item: request["paths"]) {
        output.append(source.retrieveByPath(item.asString()));
    }
    return output;
}

void DataManager::updateUserData(
        const string &accessToken,
        const DataField &field,
        const Json::Value &request
) {
    if (!request["data"].isArray()) {
        throw invalid_argument("'data' must be an array");
    }
    auto id = _redisHelper->getIdByAccessToken(accessToken);
    auto data = _dataMapper->findOne(orm::Criteria(
            techluster::Player::Cols::_id,
            orm::CompareOperator::EQ,
            id
    ));
    string rawString;
    switch (field) {
        case DataField::kPublic:
            rawString = data.getValueOfPublic();
            break;
        case DataField::kProtected:
            rawString = data.getValueOfProtected();
            break;
        case DataField::kPrivate:
            rawString = data.getValueOfPrivate();
            break;
    }
    auto target = JsonHelper(rawString);
    if (request["options"]["overwrite"].isBool()) {
        target.canOverwrite(request["options"]["overwrite"].asBool());
    }
    if (request["options"]["skip"].isBool()) {
        target.canSkip(request["options"]["skip"].asBool());
    }
    for (const auto &item: request["data"]) {
        target.modifyByPath(item["path"].asString(), item["value"]);
        LOG_DEBUG << target.stringify("  ");
    }
    switch (field) {
        case DataField::kPublic:
            data.setPublic(target.stringify());
            break;
        case DataField::kProtected:
            data.setProtected(target.stringify());
            break;
        case DataField::kPrivate:
            data.setPrivate(target.stringify());
            break;
    }
    _dataMapper->update(data);
}

bool DataManager::ipLimit(const string &ip) const {
    return _redisHelper->tokenBucket(
            "ip:" + ip,
            _ipInterval,
            _ipMaxCount
    );
}

bool DataManager::emailLimit(const string &email) const {
    return _redisHelper->tokenBucket(
            "email:" + email,
            _emailInterval,
            _emailMaxCount
    );
}