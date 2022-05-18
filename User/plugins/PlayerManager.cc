//
// Created by Parti on 2021/2/4.
//

#include <drogon/drogon.h>
#include <helpers/DataJson.h>
#include <plugins/PlayerManager.h>
#include <structures/ExceptionHandlers.h>
#include <structures/Exceptions.h>
#include <utils/crypto.h>
#include <utils/data.h>
#include <utils/datetime.h>
#include <utils/io.h>

using namespace drogon;
using namespace drogon_model;
using namespace std;
using namespace tech::helpers;
using namespace tech::plugins;
using namespace tech::structures;
using namespace tech::types;
using namespace tech::utils;

void PlayerManager::initAndStart(const Json::Value &config) {
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
            config["redis"]["db"].isInt() &&
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
                config["redis"]["db"].asInt(),
                config["redis"]["timeout"].asUInt()
        );
    } catch (const cpp_redis::redis_error &e) {
        LOG_ERROR << e.what();
        abort();
    }

    if (!(
            config["sql"]["expirations"]["removed"].isUInt64()
    )) {
        LOG_ERROR << R"("Invalid sql config")";
        abort();
    }
    _removedInterval = chrono::minutes(config["sql"]["expirations"]["removed"].asUInt64());

    _dataMapper = make_unique<orm::Mapper<techluster::Data>>(app().getDbClient());
    _playerMapper = make_unique<orm::Mapper<techluster::Player>>(app().getDbClient());
    _removedMapper = make_unique<orm::Mapper<techluster::Removed>>(app().getDbClient());

    LOG_INFO << "DataManager loaded.";
}

void PlayerManager::shutdown() {
    _userRedis->disconnect();
    LOG_INFO << "DataManager shutdown.";
}

int64_t PlayerManager::getUserId(const string &accessToken) {
    try {
        return _userRedis->getIdByAccessToken(accessToken);
    } catch (const redis_exception::KeyNotFound &e) {
        LOG_DEBUG << "Key not found:" << e.what();
        throw ResponseException(
                i18n("invalidAccessToken"),
                ResultCode::NotAcceptable,
                k401Unauthorized
        );
    }
}

RedisToken PlayerManager::refresh(const string &refreshToken) {
    try {
        return move(_userRedis->refresh(refreshToken));
    } catch (const redis_exception::KeyNotFound &e) {
        LOG_DEBUG << "Key not found:" << e.what();
        throw ResponseException(
                i18n("invalidRefreshToken"),
                ResultCode::NotAcceptable,
                k401Unauthorized
        );
    }
}

void PlayerManager::verifyEmail(const string &email) {
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

tuple<RedisToken, bool> PlayerManager::loginEmailCode(
        const string &email,
        const string &code
) {
    _checkEmailCode(email, code);

    techluster::Player player;
    if (_playerMapper->count(orm::Criteria(
            techluster::Player::Cols::_email,
            orm::CompareOperator::EQ,
            email
    )) == 0) {
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
            player.getValueOfPassword().empty()
    };
}

RedisToken PlayerManager::loginEmailPassword(
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
                    ResultCode::NullValue,
                    k403Forbidden
            );
        }

        return _userRedis->generateTokens(to_string(player.getValueOfId()));
    } catch (const orm::UnexpectedRows &e) {
        LOG_DEBUG << "Unexpected rows: " << e.what();
        throw ResponseException(
                i18n("invalidEmailPass"),
                ResultCode::NotAcceptable,
                k403Forbidden
        );
    }
}

void PlayerManager::resetEmail(
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
        _playerMapper->update(player);
    } catch (const orm::UnexpectedRows &e) {
        LOG_DEBUG << "Unexpected rows: " << e.what();
        throw ResponseException(
                i18n("userNotFound"),
                ResultCode::NotFound,
                k404NotFound
        );
    }
}

void PlayerManager::migrateEmail(
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
                    ResultCode::Conflict,
                    k409Conflict
            );
        }
        player.setEmail(newEmail);
        _playerMapper->update(player);
    } catch (const redis_exception::KeyNotFound &e) {
        LOG_DEBUG << "Key not found:" << e.what();
        throw ResponseException(
                i18n("invalidAccessToken"),
                ResultCode::NotAcceptable,
                k401Unauthorized
        );
    } catch (const orm::UnexpectedRows &e) {
        LOG_DEBUG << "Unexpected rows: " << e.what();
        throw ResponseException(
                i18n("userNotFound"),
                ResultCode::NotFound,
                k404NotFound
        );
    }
}

void PlayerManager::deactivateEmail(
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
        removed.setTimestamp(trantor::Date::date());

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
                ResultCode::NotAcceptable,
                k401Unauthorized
        );
    } catch (const orm::UnexpectedRows &e) {
        LOG_DEBUG << "Unexpected rows: " << e.what();
        throw ResponseException(
                i18n("userNotFound"),
                ResultCode::NotFound,
                k404NotFound
        );
    }
}

Json::Value PlayerManager::searchRemoved(const RequestJson &request) {
    using namespace chrono;
    orm::Criteria criteria;
    if (request.check("id", JsonValue::Int64)) {
        criteria = criteria || orm::Criteria(
                techluster::Removed::Cols::_id,
                orm::CompareOperator::EQ,
                request["id"].asInt64()
        );
    }
    if (request.check("email", JsonValue::String)) {
        criteria = criteria || orm::Criteria(
                techluster::Removed::Cols::_email,
                orm::CompareOperator::EQ,
                request["email"].asString()
        );
    }
    if (request.check("username", JsonValue::String)) {
        criteria = criteria || orm::Criteria(
                techluster::Removed::Cols::_username,
                orm::CompareOperator::EQ,
                request["username"].asString()
        );
    }
    if (request.check("clan", JsonValue::String)) {
        criteria = criteria || orm::Criteria(
                techluster::Removed::Cols::_clan,
                orm::CompareOperator::EQ,
                request["clan"].asString()
        );
    }
    try {
        auto rows = _removedMapper->findBy(criteria);
        Json::Value result(Json::arrayValue);
        for (auto &removed: rows) {
            Json::Value row;
            row["id"] = removed.getValueOfId();
            row["username"] = removed.getValueOfUsername();
            row["motto"] = removed.getValueOfMotto();
            row["clan"] = removed.getValueOfClan();

            auto email = removed.getValueOfEmail();
            auto prefixLength = email.find('@');
            email.replace(3, prefixLength - 4, "***");
            row["email"] = email;

            auto removeTime = removed.getValueOfTimestamp();
            if (removed.getValueOfRecoverable()) {
                if (removeTime.after(
                        static_cast<double>(_removedInterval.count())
                ) < trantor::Date::date()) {
                    removed.setRegionToNull();
                    removed.setAvatarToNull();
                    removed.setAvatarHashToNull();
                    removed.setAvatarFrameToNull();
                    removed.setData(R"({"public":{},"protected":{},"private":{}})");
                    removed.setRecoverable(false);
                    _removedMapper->update(removed);
                    row["region"];
                    row["avatar"];
                    row["avatarFrame"];
                } else {
                    row["region"] = removed.getValueOfRegion();
                    row["avatar"] = removed.getValueOfAvatar();
                    row["avatarFrame"] = removed.getValueOfAvatarFrame();
                }
            } else {
                row["region"];
                row["avatar"];
                row["avatarFrame"];
            }
            result.append(row);
        }
        return result;
    } catch (const orm::UnexpectedRows &e) {
        LOG_DEBUG << "Unexpected rows: " << e.what();
        throw ResponseException(
                i18n("userNotFound"),
                ResultCode::NotFound,
                k404NotFound
        );
    }
}

void PlayerManager::restoreRemoved(
        const string &email,
        const string &code
) {
    _checkEmailCode(email, code);
    try {
        auto removedJson = _removedMapper->findOne(orm::Criteria(
                techluster::Removed::Cols::_email,
                orm::CompareOperator::EQ,
                email
        )).toJson();

        techluster::Player player;
        player.updateByJson(removedJson);
        _playerMapper->insert(player);

        techluster::Data data;
        data.setId(player.getValueOfId());
        data.updateByJson(removedJson["data"]);
        _dataMapper->insert(data);
    } catch (const orm::UnexpectedRows &e) {
        LOG_DEBUG << "Unexpected rows: " << e.what();
        throw ResponseException(
                i18n("userNotFound"),
                ResultCode::NotFound,
                k404NotFound
        );
    }
}

Json::Value PlayerManager::getUserInfo(const string &accessToken, int64_t userId) {
    int64_t targetId = userId;
    NO_EXCEPTION(
            targetId = _userRedis->getIdByAccessToken(accessToken);
    )
    try {
        auto result = _playerMapper->findOne(orm::Criteria(
                techluster::Player::Cols::_id,
                orm::CompareOperator::EQ,
                targetId
        )).toJson();
        result.removeMember("password");
        result.removeMember("avatar");
        if (userId > 0) {
            result.removeMember("email");
        }
        return result;
    } catch (const orm::UnexpectedRows &e) {
        LOG_DEBUG << "Unexpected rows: " << e.what();
        throw ResponseException(
                i18n("userNotFound"),
                ResultCode::NotFound,
                k404NotFound
        );
    }
}

void PlayerManager::updateUserInfo(
        const string &accessToken,
        RequestJson request
) {
    try {
        auto player = _playerMapper->findOne(orm::Criteria(
                techluster::Player::Cols::_id,
                orm::CompareOperator::EQ,
                _userRedis->getIdByAccessToken(accessToken)
        ));
        if (player.getValueOfPassword().empty()) {
            if (!request.check("password", JsonValue::String)) {
                throw ResponseException(
                        i18n("noPassword"),
                        ResultCode::NullValue,
                        k403Forbidden
                );
            }
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
                ResultCode::NotFound,
                k404NotFound
        );
    }
}

string PlayerManager::getAvatar(const string &accessToken, int64_t userId) {
    int64_t targetId = userId;
    NO_EXCEPTION(
            targetId = _userRedis->getIdByAccessToken(accessToken);
    )
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
                ResultCode::NotFound,
                k404NotFound
        );
    }
}

Json::Value PlayerManager::getUserData(
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
                    ResultCode::NoPermission,
                    k403Forbidden
            );
        }
        targetId = userId < 0 ? tempUserId : userId;
    } catch (const redis_exception::KeyNotFound &e) {
        LOG_DEBUG << "Key not found:" << e.what();
        throw ResponseException(
                i18n("invalidAccessToken"),
                ResultCode::NotAcceptable,
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
        for (const auto &path: request["paths"]) {
            output.append(source.retrieveByPath(path.asString()));
        }
        return output;
    } catch (const orm::UnexpectedRows &e) {
        LOG_DEBUG << "Unexpected rows: " << e.what();
        throw ResponseException(
                i18n("userNotFound"),
                ResultCode::NotFound,
                k404NotFound
        );
    }
}

void PlayerManager::updateUserData(
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
                ResultCode::NotAcceptable,
                k401Unauthorized
        );
    } catch (const orm::UnexpectedRows &e) {
        LOG_DEBUG << "Unexpected rows: " << e.what();
        throw ResponseException(
                i18n("userNotFound"),
                ResultCode::NotFound,
                k404NotFound
        );
    }
}

bool PlayerManager::ipLimit(const string &ip) const {
    return _userRedis->tokenBucket(
            "ip:" + ip,
            _ipInterval,
            _ipMaxCount
    );
}

bool PlayerManager::emailLimit(const string &email) const {
    return _userRedis->tokenBucket(
            "email:" + email,
            _emailInterval,
            _emailMaxCount
    );
}

void PlayerManager::_checkEmailCode(
        const string &email,
        const string &code
) {
    try {
        if (!_userRedis->checkEmailCode(email, code)) {
            throw ResponseException(
                    i18n("invalidCode"),
                    ResultCode::NotAcceptable,
                    k401Unauthorized
            );
        }
        _userRedis->deleteEmailCode(email);
    } catch (const redis_exception::KeyNotFound &e) {
        LOG_DEBUG << "Key not found: " << e.what();
        throw ResponseException(
                i18n("invalidEmail"),
                ResultCode::NotFound,
                k404NotFound
        );
    }
}
