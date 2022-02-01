//
// Created by Particle_G on 2021/8/19.
//

#include <services/User.h>
#include <structures/Exceptions.h>

using namespace drogon;
using namespace std;
using namespace tech::plugins;
using namespace tech::services;
using namespace tech::structures;
using namespace tech::utils;

User::User() : _dataManager(app().getPlugin<DataManager>()) {}

Json::Value User::getInfo(
        HttpStatusCode &code,
        const string &accessToken,
        const int64_t &id
) {
    Json::Value response;
    try {
        auto info = _dataManager->getUserInfo(accessToken, id);
        response["type"] = "Success";
        response["data"] = info;
    } catch (const orm::UnexpectedRows &e) {
        code = k500InternalServerError;
        response["type"] = "Error";
        response["reason"] = "No user's id = " + to_string(id);
    } catch (const redis_exception::KeyNotFound &e) {
        code = k401Unauthorized;
        response["type"] = "Failed";
        response["reason"] = "Invalid access token";
    } catch (const orm::DrogonDbException &e) {
        LOG_ERROR << "error:" << e.base().what();
        code = k500InternalServerError;
        response["type"] = "Error";
        response["reason"] = "ORM error";
    } catch (const exception &e) {
        code = k401Unauthorized;
        response["type"] = "Failed";
        response["reason"] = e.what();
    }
    return response;
}

Json::Value User::updateInfo(
        HttpStatusCode &code,
        const string &accessToken,
        const Json::Value &data
) {
    Json::Value response;
    try {
        _dataManager->updateUserInfo(accessToken, data);
        response["type"] = "Success";
    } catch (const orm::DrogonDbException &e) {
        LOG_WARN << e.base().what();
        code = k500InternalServerError;
        response["type"] = "Error";
        response["reason"] = "ORM error";
    } catch (const redis_exception::KeyNotFound &e) {
        code = k401Unauthorized;
        response["type"] = "Failed";
        response["reason"] = "Invalid access token";
    } catch (const exception &e) {
        LOG_DEBUG << e.what();
        code = k401Unauthorized;
        response["type"] = "Failed";
        response["reason"] = e.what();
    }
    return response;
}

Json::Value User::getAvatar(
        HttpStatusCode &code,
        const string &accessToken,
        const int64_t &id
) {
    Json::Value response;
    try {
        auto avatar = _dataManager->getUserAvatar(accessToken, id);
        response["type"] = "Success";
        response["data"] = avatar;
    } catch (const orm::UnexpectedRows &e) {
        code = k500InternalServerError;
        response["type"] = "Error";
        response["reason"] = "No user's id = " + to_string(id);
    } catch (const redis_exception::KeyNotFound &e) {
        code = k401Unauthorized;
        response["type"] = "Failed";
        response["reason"] = "Invalid access token";
    } catch (const orm::DrogonDbException &e) {
        LOG_ERROR << "error:" << e.base().what();
        code = k500InternalServerError;
        response["type"] = "Error";
        response["reason"] = "ORM error";
    } catch (const exception &e) {
        code = k401Unauthorized;
        response["type"] = "Failed";
        response["reason"] = e.what();
    }
    return response;
}

Json::Value User::getData(
        HttpStatusCode &code,
        const string &accessToken,
        const int64_t &id,
        const DataField &field,
        const Json::Value &data
) {
    Json::Value response;
    try {
        response["type"] = "Success";
        response["data"] = _dataManager->getUserData(
                accessToken,
                id,
                field,
                data
        );
    } catch (const orm::UnexpectedRows &e) {
        code = k500InternalServerError;
        response["type"] = "Error";
        response["reason"] = "No user's id = " + to_string(id);
    } catch (const redis_exception::KeyNotFound &e) {
        code = k401Unauthorized;
        response["type"] = "Failed";
        response["reason"] = "Invalid access token";
    } catch (const orm::DrogonDbException &e) {
        LOG_ERROR << "error:" << e.base().what();
        code = k500InternalServerError;
        response["type"] = "Error";
        response["reason"] = "ORM error";
    } catch (const exception &e) {
        code = k401Unauthorized;
        response["type"] = "Failed";
        response["reason"] = e.what();
    }
    return response;
}

Json::Value User::updateData(
        HttpStatusCode &code,
        const string &accessToken,
        const DataField &field,
        const Json::Value &data
) {
    Json::Value response;
    try {
        _dataManager->updateUserData(
                accessToken,
                field,
                data
        );
        response["type"] = "Success";
    } catch (const orm::UnexpectedRows &e) {
        code = k500InternalServerError;
        response["type"] = "Error";
        response["reason"] = e.what();
    } catch (const redis_exception::KeyNotFound &e) {
        code = k401Unauthorized;
        response["type"] = "Failed";
        response["reason"] = "Invalid access token";
    } catch (const orm::DrogonDbException &e) {
        LOG_ERROR << "error:" << e.base().what();
        code = k500InternalServerError;
        response["type"] = "Error";
        response["reason"] = "ORM error";
    } catch (const exception &e) {
        code = k401Unauthorized;
        response["type"] = "Failed";
        response["reason"] = e.what();
    }
    return response;
}
