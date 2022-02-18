//
// Created by particleg on 2021/9/24.
//

#include <controllers/Auth.h>
#include <helpers/RequestJson.h>
#include <helpers/ResponseJson.h>
#include <structures/Exceptions.h>
#include <utils/http.h>

using namespace drogon;
using namespace std;
using namespace tech::api::v2;
using namespace tech::helpers;
using namespace tech::plugins;
using namespace tech::structures;
using namespace tech::types;
using namespace tech::utils;

Auth::Auth() :
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

void Auth::check(const HttpRequestPtr &req, function<void(const HttpResponsePtr &)> &&callback) {
    HttpStatusCode code = k200OK;
    ResponseJson response;
    handleExceptions([&]() {
        response.setData(_dataManager->getUserId(
                req->attributes()->get<string>("accessToken")
        ));
    }, response, code);
    http::fromJson(code, response.ref(), callback);
}

void Auth::refresh(const HttpRequestPtr &req, function<void(const HttpResponsePtr &)> &&callback) {
    HttpStatusCode code = k200OK;
    ResponseJson response;
    handleExceptions([&]() {
        response.setData(move(_dataManager->refresh(
                req->attributes()->get<string>("refreshToken")
        ).parse()));
    }, response, code);
    http::fromJson(code, response.ref(), callback);
}

void Auth::verifyEmail(const HttpRequestPtr &req, function<void(const HttpResponsePtr &)> &&callback) {
    HttpStatusCode code = k200OK;
    ResponseJson response;
    handleExceptions([&]() {
        _dataManager->verifyEmail(
                req->attributes()->get<RequestJson>("requestJson")["email"].asString()
        );
    }, response, code);
    http::fromJson(code, response.ref(), callback);
}

void Auth::loginEmail(const HttpRequestPtr &req, function<void(const HttpResponsePtr &)> &&callback) {
    HttpStatusCode code = k200OK;
    ResponseJson response;
    handleExceptions([&]() {
        auto request = req->attributes()->get<RequestJson>("requestJson");
        if (request.check("code", JsonValue::String)) {
            const auto &[tokens, isNew] = _dataManager->loginEmailCode(
                    request["email"].asString(),
                    request["code"].asString()
            );
            if (isNew) { response.setResult(ResultCode::continued); }
            response.setData(move(tokens.parse()));
        } else {
            const auto &tokens = _dataManager->loginEmailPassword(
                    request["email"].asString(),
                    request["password"].asString()
            );
            response.setData(move(tokens.parse()));
        }
    }, response, code);
    http::fromJson(code, response.ref(), callback);
}

void Auth::resetEmail(const HttpRequestPtr &req, function<void(const HttpResponsePtr &)> &&callback) {
    HttpStatusCode code = k200OK;
    ResponseJson response;
    handleExceptions([&]() {
        auto request = req->attributes()->get<RequestJson>("requestJson");
        _dataManager->resetEmail(
                request["email"].asString(),
                request["code"].asString(),
                request["newPassword"].asString()
        );
    }, response, code);
    http::fromJson(code, response.ref(), callback);
}

void Auth::migrateEmail(const HttpRequestPtr &req, function<void(const HttpResponsePtr &)> &&callback) {
    HttpStatusCode code = k200OK;
    ResponseJson response;
    handleExceptions([&]() {
        auto request = req->attributes()->get<RequestJson>("requestJson");
        _dataManager->migrateEmail(
                req->attributes()->get<string>("accessToken"),
                request["newEmail"].asString(),
                request["code"].asString()
        );
    }, response, code);
    http::fromJson(code, response.ref(), callback);
}