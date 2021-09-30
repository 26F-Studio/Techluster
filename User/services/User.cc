//
// Created by Particle_G on 2021/8/19.
//

#include <services/User.h>
#include <utils/crypto.h>

using namespace drogon;
using namespace std;
using namespace tech::plugins;
using namespace tech::services;
using namespace tech::utils;

User::User() : _dataManager(app().getPlugin<DataManager>()) {}

Json::Value User::getInfo(HttpStatusCode &code, const Json::Value &data) {
    Json::Value response;
    try {
        auto info = _dataManager->getUserInfo(
                data["accessToken"].asString(),
                data["id"].asInt()
        );
        response["type"] = "Success";
        response["data"] = info;
    } catch (const orm::UnexpectedRows &e) {
        code = k500InternalServerError;
        response["type"] = "Error";
        response["reason"] = "No user's id = " + to_string(data["id"].asInt());
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

Json::Value User::updateInfo(HttpStatusCode &code, const Json::Value &data) {
    Json::Value response;
    try {
        _dataManager->updateUserInfo(
                data["accessToken"].asString(),
                data
        );
        response["type"] = "Success";
    } catch (const orm::DrogonDbException &e) {
        LOG_WARN << e.base().what();
        code = k500InternalServerError;
        response["type"] = "Error";
        response["reason"] = "ORM error";
    } catch (const out_of_range &e) {
        LOG_DEBUG << e.what();
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

Json::Value User::getAvatar(HttpStatusCode &code, const Json::Value &data) {
    Json::Value response;
    try {
        auto avatar = _dataManager->getUserAvatar(
                data["accessToken"].asString(),
                data["id"].asInt()
        );
        response["type"] = "Success";
        response["data"] = avatar;
    } catch (const orm::UnexpectedRows &e) {
        code = k500InternalServerError;
        response["type"] = "Error";
        response["reason"] = "No user's id = " + to_string(data["id"].asInt());
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