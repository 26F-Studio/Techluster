//
// Created by Parti on 2021/2/4.
//

#include <drogon/drogon.h>
#include <helpers/DataJson.h>
#include <plugins/DataManager.h>
#include <structures/Exceptions.h>
#include <utils/crypto.h>
#include <utils/data.h>
#include <utils/io.h>

using namespace drogon;
using namespace drogon_model;
using namespace std;
using namespace tech::helpers;
using namespace tech::plugins;
using namespace tech::structures;
using namespace tech::types;
using namespace tech::utils;

DataManager::DataManager() : I18nHelper(CMAKE_PROJECT_NAME) {}

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
            config["smtp"]["server"].isString() &&
            config["smtp"]["account"].isString() &&
            config["smtp"]["password"].isString() &&
            config["smtp"]["senderEmail"].isString() &&
            config["smtp"]["senderName"].isString()
    )) {
        LOG_ERROR << R"(Invalid smtp config)";
        abort();
    }
    _emailHelper = make_unique<EmailHelper>(
            config["smtp"]["server"].asString(),
            config["smtp"]["account"].asString(),
            config["smtp"]["password"].asString(),
            config["smtp"]["senderEmail"].asString(),
            config["smtp"]["senderName"].asString()
    );

    if (!(
            config["redis"]["host"].isString() &&
            config["redis"]["port"].isUInt() &&
            config["redis"]["timeout"].isUInt() &&
            config["redis"]["expirations"]["refresh"].isInt64() &&
            config["redis"]["expirations"]["access"].isInt64() &&
            config["redis"]["expirations"]["email"].isInt64()
    )) {
        LOG_ERROR << R"("Invalid redis config")";
        abort();
    }

    _userRedis = make_unique<UserRedis>(move(UserRedis(
            {
                    chrono::minutes(config["redis"]["expirations"]["refresh"].asInt64()),
                    chrono::minutes(config["redis"]["expirations"]["access"].asInt64()),
                    chrono::minutes(config["redis"]["expirations"]["email"].asInt64())
            }
    )));

    try {
        _userRedis->connect(
                config["redis"]["host"].asString(),
                config["redis"]["port"].asUInt(),
                config["redis"]["timeout"].asUInt()
        );
    } catch (const cpp_redis::redis_error &e) {
        LOG_ERROR << e.what();
        abort();
    }

    _dataMapper = make_unique<orm::Mapper<techluster::Data>>(app().getDbClient());
    _playerMapper = make_unique<orm::Mapper<techluster::Player>>(app().getDbClient());
    _removedMapper = make_unique<orm::Mapper<techluster::Removed>>(app().getDbClient());

    LOG_INFO << "DataManager loaded.";
}

void DataManager::shutdown() {
    _userRedis->disconnect();
    LOG_INFO << "DataManager shutdown.";
}

int64_t DataManager::getUserId(const string &accessToken) {
    try {
        return _userRedis->getIdByAccessToken(accessToken);
    } catch (const redis_exception::KeyNotFound &e) {
        LOG_DEBUG << "Key not found:" << e.what();
        throw ResponseException(
                i18n("invalidAccessToken"),
                ResultCode::notAcceptable,
                k401Unauthorized
        );
    }
}

RedisToken DataManager::refresh(const string &refreshToken) {
    try {
        return move(_userRedis->refresh(refreshToken));
    } catch (const redis_exception::KeyNotFound &e) {
        LOG_DEBUG << "Key not found:" << e.what();
        throw ResponseException(
                i18n("invalidRefreshToken"),
                ResultCode::notAcceptable,
                k401Unauthorized
        );
    }
}

void DataManager::verifyEmail(const string &email) {
    auto code = data::randomString(8);
    _userRedis->setEmailCode(email, code);
    auto mailContent = io::getFileContent("./verifyEmail.html");
    drogon::utils::replaceAll(
            mailContent,
            "{{VERIFY_CODE}}",
            code
    );
    // TODO: Replace with async method
    _emailHelper->smtp(
            email,
            "[Techmino] Verify Code 验证码",
            mailContent
    );
}

tuple<RedisToken, bool> DataManager::loginEmailCode(
        const string &email,
        const string &code
) {
    _checkEmailCode(email, code);

    techluster::Player player;
    bool isNew = false;
    if (_playerMapper->count(orm::Criteria(
            techluster::Player::Cols::_email,
            orm::CompareOperator::EQ,
            email
    )) == 0) {
        isNew = true;
        player.setEmail(email);
        _playerMapper->insert(player);
        techluster::Data data;
        _dataMapper->insert(data);
    } else {
        player = _playerMapper->findOne(orm::Criteria(
                techluster::Player::Cols::_email,
                orm::CompareOperator::EQ,
                email
        ));
    }

    return {
            _userRedis->generateTokens(to_string(player.getValueOfId())),
            isNew
    };
}

RedisToken DataManager::loginEmailPassword(
        const string &email,
        const string &password
) {
    try {
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
            throw ResponseException(
                    i18n("noPassword"),
                    ResultCode::nullValue,
                    k403Forbidden
            );
        }

        return _userRedis->generateTokens(to_string(player.getValueOfId()));
    } catch (const orm::UnexpectedRows &e) {
        LOG_DEBUG << "Unexpected rows: " << e.what();
        throw ResponseException(
                i18n("invalidEmailPass"),
                ResultCode::notAcceptable,
                k403Forbidden
        );
    }
}

void DataManager::resetEmail(
        const string &email,
        const string &code,
        const string &newPassword
) {
    _checkEmailCode(email, code);

    try {
        auto player = _playerMapper->findOne(orm::Criteria(
                techluster::Player::Cols::_email,
                orm::CompareOperator::EQ,
                email
        ));
        player.setPassword(newPassword);
        if (player.getValueOfIsNew()) {
            player.setIsNew(false);
        }
        _playerMapper->update(player);
    } catch (const orm::UnexpectedRows &e) {
        LOG_DEBUG << "Unexpected rows: " << e.what();
        throw ResponseException(
                i18n("userNotFound"),
                ResultCode::notFound,
                k404NotFound
        );
    }
}

void DataManager::migrateEmail(
        const string &accessToken,
        const string &newEmail,
        const string &code
) {
    _checkEmailCode(newEmail, code);

    try {
        auto player = _playerMapper->findOne(orm::Criteria(
                techluster::Player::Cols::_id,
                orm::CompareOperator::EQ,
                _userRedis->getIdByAccessToken(accessToken)
        ));
        if (player.getValueOfEmail() == newEmail) {
            return;
        }
        if (_playerMapper->count(orm::Criteria(
                techluster::Player::Cols::_email,
                orm::CompareOperator::EQ,
                newEmail
        ))) {
            throw ResponseException(
                    i18n("emailExists"),
                    ResultCode::conflict,
                    k409Conflict
            );
        }
        player.setEmail(newEmail);
        _playerMapper->update(player);
    } catch (const redis_exception::KeyNotFound &e) {
        LOG_DEBUG << "Key not found:" << e.what();
        throw ResponseException(
                i18n("invalidAccessToken"),
                ResultCode::notAcceptable,
                k401Unauthorized
        );
    } catch (const orm::UnexpectedRows &e) {
        LOG_DEBUG << "Unexpected rows: " << e.what();
        throw ResponseException(
                i18n("userNotFound"),
                ResultCode::notFound,
                k404NotFound
        );
    }
}

void DataManager::deactivateEmail(
        const string &accessToken,
        const string &code
) {
    try {
        auto player = _playerMapper->findOne(orm::Criteria(
                techluster::Player::Cols::_id,
                orm::CompareOperator::EQ,
                _userRedis->getIdByAccessToken(accessToken)
        ));
        _checkEmailCode(player.getValueOfEmail(), code);
        auto data = _dataMapper->findOne(orm::Criteria(
                techluster::Data::Cols::_id,
                orm::CompareOperator::EQ,
                player.getValueOfId()
        ));

        techluster::Removed removed;
        removed.setId(player.getValueOfId());
        removed.setEmail(player.getValueOfEmail());
        removed.setUsername(player.getValueOfUsername());
        removed.setMotto(player.getValueOfMotto());
        removed.setRegion(player.getValueOfRegion());
        removed.setAvatar(player.getValueOfAvatar());
        removed.setAvatarHash(player.getValueOfAvatarHash());
        removed.setAvatarFrame(player.getValueOfAvatarFrame());
        // TODO: Determine whether we should keep the following field
        removed.setClan(player.getValueOfClan());

        Json::Value dataJson;
        dataJson["public"] = data.getValueOfPublic();
        dataJson["protected"] = data.getValueOfProtected();
        dataJson["private"] = data.getValueOfPrivate();
        removed.setData(BasicJson(dataJson).stringify());

        _removedMapper->insert(removed);
        _playerMapper->deleteOne(player);
    } catch (const redis_exception::KeyNotFound &e) {
        LOG_DEBUG << "Key not found:" << e.what();
        throw ResponseException(
                i18n("invalidAccessToken"),
                ResultCode::notAcceptable,
                k401Unauthorized
        );
    } catch (const orm::UnexpectedRows &e) {
        LOG_DEBUG << "Unexpected rows: " << e.what();
        throw ResponseException(
                i18n("userNotFound"),
                ResultCode::notFound,
                k404NotFound
        );
    }
}

Json::Value DataManager::getUserInfo(
        const string &accessToken,
        const int64_t &userId
) {
    int64_t targetId;
    try {
        auto tempUserId = _userRedis->getIdByAccessToken(accessToken);
        targetId = userId < 0 ? tempUserId : userId;
    } catch (const redis_exception::KeyNotFound &e) {
        LOG_DEBUG << "Key not found:" << e.what();
        throw ResponseException(
                i18n("invalidAccessToken"),
                ResultCode::notAcceptable,
                k401Unauthorized
        );
    }
    try {
        auto result = _playerMapper->findOne(orm::Criteria(
                techluster::Player::Cols::_id,
                orm::CompareOperator::EQ,
                targetId
        )).toJson();
        result.removeMember("password");
        result.removeMember("avatar");
        result.removeMember("is_new");
        return result;
    } catch (const orm::UnexpectedRows &e) {
        LOG_DEBUG << "Unexpected rows: " << e.what();
        throw ResponseException(
                i18n("userNotFound"),
                ResultCode::notFound,
                k404NotFound
        );
    }
}

void DataManager::updateUserInfo(
        const string &accessToken,
        RequestJson request
) {
    try {
        auto player = _playerMapper->findOne(orm::Criteria(
                techluster::Player::Cols::_id,
                orm::CompareOperator::EQ,
                _userRedis->getIdByAccessToken(accessToken)
        ));
        if (player.getValueOfIsNew()) {
            if (!request.check("password", JsonValue::String)) {
                throw ResponseException(
                        i18n("noPassword"),
                        ResultCode::nullValue,
                        k403Forbidden
                );
            }
            player.setIsNew(false);
        } else {
            request.remove("password");
        }
        if (request.check("avatar", JsonValue::String)) {
            player.setAvatarHash(crypto::blake2B(request["avatar"].asString()));
        }
        player.updateByJson(request.ref());
        _playerMapper->update(player);
    } catch (const orm::UnexpectedRows &e) {
        LOG_DEBUG << "Unexpected rows: " << e.what();
        throw ResponseException(
                i18n("userNotFound"),
                ResultCode::notFound,
                k404NotFound
        );
    }
}

string DataManager::getAvatar(
        const string &accessToken,
        const int64_t &userId
) {
    int64_t targetId;
    try {
        auto tempUserId = _userRedis->getIdByAccessToken(accessToken);
        targetId = userId < 0 ? tempUserId : userId;
    } catch (const redis_exception::KeyNotFound &e) {
        LOG_DEBUG << "Key not found:" << e.what();
        throw ResponseException(
                i18n("invalidAccessToken"),
                ResultCode::notAcceptable,
                k401Unauthorized
        );
    }
    try {
        auto player = _playerMapper->findOne(orm::Criteria(
                techluster::Player::Cols::_id,
                orm::CompareOperator::EQ,
                targetId
        ));
        return player.getValueOfAvatar();
    } catch (const orm::UnexpectedRows &e) {
        LOG_DEBUG << "Unexpected rows: " << e.what();
        throw ResponseException(
                i18n("userNotFound"),
                ResultCode::notFound,
                k404NotFound
        );
    }
}

Json::Value DataManager::getUserData(
        const string &accessToken,
        const int64_t &userId,
        const DataField &field,
        const RequestJson &request
) {
    // TODO: Only allow access to other user's protected data if they are friends
    int64_t targetId;
    try {
        auto tempUserId = _userRedis->getIdByAccessToken(accessToken);
        if (userId != tempUserId && field != DataField::Public) {
            throw ResponseException(
                    i18n("noPermission"),
                    ResultCode::noPermission,
                    k403Forbidden
            );
        }
        targetId = userId < 0 ? tempUserId : userId;
    } catch (const redis_exception::KeyNotFound &e) {
        LOG_DEBUG << "Key not found:" << e.what();
        throw ResponseException(
                i18n("invalidAccessToken"),
                ResultCode::notAcceptable,
                k401Unauthorized
        );
    }
    try {
        auto data = _dataMapper->findOne(orm::Criteria(
                techluster::Player::Cols::_id,
                orm::CompareOperator::EQ,
                targetId
        ));
        string rawString;
        switch (field) {
            case DataField::Public:
                rawString = data.getValueOfPublic();
                break;
            case DataField::Protected:
                rawString = data.getValueOfProtected();
                break;
            case DataField::Private:
                rawString = data.getValueOfPrivate();
                break;
        }
        Json::Value output;
        auto source = DataJson(rawString);
        for (const auto &item: request["paths"]) {
            output.append(source.retrieveByPath(item.asString()));
        }
        return output;
    } catch (const orm::UnexpectedRows &e) {
        LOG_DEBUG << "Unexpected rows: " << e.what();
        throw ResponseException(
                i18n("userNotFound"),
                ResultCode::notFound,
                k404NotFound
        );
    }
}

void DataManager::updateUserData(
        const string &accessToken,
        const DataField &field,
        const RequestJson &request
) {
    try {
        auto data = _dataMapper->findOne(orm::Criteria(
                techluster::Player::Cols::_id,
                orm::CompareOperator::EQ,
                _userRedis->getIdByAccessToken(accessToken)
        ));
        string rawString;
        switch (field) {
            case DataField::Public:
                rawString = data.getValueOfPublic();
                break;
            case DataField::Protected:
                rawString = data.getValueOfProtected();
                break;
            case DataField::Private:
                rawString = data.getValueOfPrivate();
                break;
        }
        auto target = DataJson(rawString);
        if (request.check("options.overwrite", JsonValue::Bool)) {
            target.canOverwrite(request["options"]["overwrite"].asBool());
        }
        if (request.check("options.skip", JsonValue::Bool)) {
            target.canSkip(request["options"]["skip"].asBool());
        }
        for (const auto &item: request["data"]) {
            target.modifyByPath(item["path"].asString(), item["value"]);
            LOG_DEBUG << target.stringify("  ");
        }
        switch (field) {
            case DataField::Public:
                data.setPublic(target.stringify());
                break;
            case DataField::Protected:
                data.setProtected(target.stringify());
                break;
            case DataField::Private:
                data.setPrivate(target.stringify());
                break;
        }
        _dataMapper->update(data);
    } catch (const redis_exception::KeyNotFound &e) {
        LOG_DEBUG << "Key not found:" << e.what();
        throw ResponseException(
                i18n("invalidAccessToken"),
                ResultCode::notAcceptable,
                k401Unauthorized
        );
    } catch (const orm::UnexpectedRows &e) {
        LOG_DEBUG << "Unexpected rows: " << e.what();
        throw ResponseException(
                i18n("userNotFound"),
                ResultCode::notFound,
                k404NotFound
        );
    }
}

bool DataManager::ipLimit(const string &ip) const {
    return _userRedis->tokenBucket(
            "ip:" + ip,
            _ipInterval,
            _ipMaxCount
    );
}

bool DataManager::emailLimit(const string &email) const {
    return _userRedis->tokenBucket(
            "email:" + email,
            _emailInterval,
            _emailMaxCount
    );
}

void DataManager::_checkEmailCode(
        const string &email,
        const string &code
) {
    try {
        _userRedis->checkEmailCode(email, code);
        _userRedis->deleteEmailCode(email);
    } catch (const redis_exception::KeyNotFound &e) {
        LOG_DEBUG << "Key not found: " << e.what();
        throw ResponseException(
                i18n("invalidVerifyEmail"),
                ResultCode::notFound,
                k404NotFound
        );
    } catch (const redis_exception::NotEqual &e) {
        LOG_DEBUG << "Value not equal at: " << e.what();
        throw ResponseException(
                i18n("invalidVerifyCode"),
                ResultCode::notAcceptable,
                k403Forbidden
        );
    }
}
