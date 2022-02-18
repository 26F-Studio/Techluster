//
// Created by particleg on 2021/9/24.
//

#include <controllers/User.h>
#include <helpers/ResponseJson.h>
#include <utils/http.h>

using namespace drogon;
using namespace std;
using namespace tech::api::v2;
using namespace tech::helpers;
using namespace tech::plugins;
using namespace tech::structures;
using namespace tech::types;
using namespace tech::utils;

User::User() :
        ResponseJsonHandler(
                [](const ResponseException &e, ResponseJson &response, HttpStatusCode &code) {
                    code = e.statusCode();
                    // TODO: Check if this causes too much copying
                    response(e.toJson());
                },
                [this](const orm::DrogonDbException &e, ResponseJson &response, HttpStatusCode &code) {
                    LOG_ERROR << e.base().what();
                    code = k500InternalServerError;
                    response.setResult(ResultCode::databaseError);
                    response.setMessage(i18n("databaseError"));
                },
                [this](const exception &e, ResponseJson &response, HttpStatusCode &code) {
                    LOG_ERROR << e.what();
                    code = drogon::k500InternalServerError;
                    response.setResult(ResultCode::internalError);
                    response.setMessage(i18n("internalError"));
                    response.setReason(e);
                }
        ),
        I18nHelper(CMAKE_PROJECT_NAME),
        _dataManager(app().getPlugin<DataManager>()) {}

void User::getInfo(const HttpRequestPtr &req, function<void(const HttpResponsePtr &)> &&callback) {
    HttpStatusCode code = k200OK;
    ResponseJson response;
    handleExceptions([&]() {
        response.setData(_dataManager->getUserInfo(
                req->attributes()->get<string>("accessToken"),
                req->attributes()->get<int64_t>("userId")
        ));
    }, response, code);
    http::fromJson(code, response.ref(), callback);
}

void User::updateInfo(const HttpRequestPtr &req, function<void(const HttpResponsePtr &)> &&callback) {
    HttpStatusCode code = k200OK;
    ResponseJson response;
    handleExceptions([&]() {
        _dataManager->updateUserInfo(
                req->attributes()->get<string>("accessToken"),
                req->attributes()->get<RequestJson>("requestJson")
        );
    }, response, code);
    http::fromJson(code, response.ref(), callback);
}

void User::getAvatar(const HttpRequestPtr &req, function<void(const HttpResponsePtr &)> &&callback) {
    HttpStatusCode code = HttpStatusCode::k200OK;
    ResponseJson response;
    handleExceptions([&]() {
        response.setData(_dataManager->getAvatar(
                req->attributes()->get<string>("accessToken"),
                req->attributes()->get<int64_t>("userId")
        ));
    }, response, code);
    http::fromJson(code, response.ref(), callback);
}

void User::getData(const HttpRequestPtr &req, function<void(const drogon::HttpResponsePtr &)> &&callback) {
    HttpStatusCode code = HttpStatusCode::k200OK;
    ResponseJson response;
    handleExceptions([&]() {
        response.setData(_dataManager->getUserData(
                req->attributes()->get<string>("accessToken"),
                req->attributes()->get<int64_t>("userId"),
                req->attributes()->get<DataField>("dataField"),
                req->attributes()->get<RequestJson>("requestJson")
        ));
    }, response, code);
    http::fromJson(code, response.ref(), callback);
}

void User::updateData(const HttpRequestPtr &req, function<void(const drogon::HttpResponsePtr &)> &&callback) {
    HttpStatusCode code = HttpStatusCode::k200OK;
    ResponseJson response;
    handleExceptions([&]() {
        _dataManager->updateUserData(
                req->attributes()->get<string>("accessToken"),
                req->attributes()->get<DataField>("dataField"),
                req->attributes()->get<RequestJson>("requestJson")
        );
    }, response, code);
    http::fromJson(code, response.ref(), callback);
}