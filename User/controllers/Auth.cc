//
// Created by particleg on 2021/9/24.
//

#include <controllers/Auth.h>
#include <helpers/RequestJson.h>
#include <helpers/ResponseJson.h>
#include <structures/Exceptions.h>

using namespace drogon;
using namespace std;
using namespace tech::api::v2;
using namespace tech::helpers;
using namespace tech::plugins;
using namespace tech::structures;
using namespace tech::types;

Auth::Auth() :
        ResponseJsonHandler(
                [](const ResponseException &e, ResponseJson &response) {
                    response.setStatusCode(e.statusCode());
                    // TODO: Check if this causes too much copying
                    response(e.toJson());
                },
                [this](const orm::DrogonDbException &e, ResponseJson &response) {
                    LOG_ERROR << e.base().what();
                    response.setStatusCode(k500InternalServerError);
                    response.setResultCode(ResultCode::DatabaseError);
                    response.setMessage(i18n("databaseError"));
                },
                [this](const exception &e, ResponseJson &response) {
                    LOG_ERROR << e.what();
                    response.setStatusCode(k500InternalServerError);
                    response.setResultCode(ResultCode::InternalError);
                    response.setMessage(i18n("internalError"));
                    response.setReason(e);
                }
        ),
        _dataManager(app().getPlugin<PlayerManager>()) {}

void Auth::check(const HttpRequestPtr &req, function<void(const HttpResponsePtr &)> &&callback) {
    ResponseJson response;
    handleExceptions([&]() {
        response.setData(_dataManager->getUserId(
                req->attributes()->get<string>("accessToken")
        ));
    }, response);
    response.httpCallback(callback);
}

void Auth::refresh(const HttpRequestPtr &req, function<void(const HttpResponsePtr &)> &&callback) {
    ResponseJson response;
    handleExceptions([&]() {
        response.setData(move(_dataManager->refresh(
                req->attributes()->get<string>("refreshToken")
        ).parse()));
    }, response);
    response.httpCallback(callback);
}

void Auth::verifyEmail(const HttpRequestPtr &req, function<void(const HttpResponsePtr &)> &&callback) {
    ResponseJson response;
    handleExceptions([&]() {
        _dataManager->verifyEmail(
                req->attributes()->get<RequestJson>("requestJson")["email"].asString()
        );
    }, response);
    response.httpCallback(callback);
}

void Auth::loginEmail(const HttpRequestPtr &req, function<void(const HttpResponsePtr &)> &&callback) {
    ResponseJson response;
    handleExceptions([&]() {
        auto request = req->attributes()->get<RequestJson>("requestJson");
        if (request.check("code", JsonValue::String)) {
            const auto &[tokens, isNew] = _dataManager->loginEmailCode(
                    request["email"].asString(),
                    request["code"].asString()
            );
            if (isNew) { response.setResultCode(ResultCode::Continued); }
            response.setData(move(tokens.parse()));
        } else {
            const auto &tokens = _dataManager->loginEmailPassword(
                    request["email"].asString(),
                    request["password"].asString()
            );
            response.setData(move(tokens.parse()));
        }
    }, response);
    response.httpCallback(callback);
}

void Auth::resetEmail(const HttpRequestPtr &req, function<void(const HttpResponsePtr &)> &&callback) {
    ResponseJson response;
    handleExceptions([&]() {
        auto request = req->attributes()->get<RequestJson>("requestJson");
        _dataManager->resetEmail(
                request["email"].asString(),
                request["code"].asString(),
                request["newPassword"].asString()
        );
    }, response);
    response.httpCallback(callback);
}

void Auth::migrateEmail(const HttpRequestPtr &req, function<void(const HttpResponsePtr &)> &&callback) {
    ResponseJson response;
    handleExceptions([&]() {
        auto request = req->attributes()->get<RequestJson>("requestJson");
        _dataManager->migrateEmail(
                req->attributes()->get<string>("accessToken"),
                request["newEmail"].asString(),
                request["code"].asString()
        );
    }, response);
    response.httpCallback(callback);
}

void Auth::deactivateEmail(const HttpRequestPtr &req, function<void(const HttpResponsePtr &)> &&callback) {
    ResponseJson response;
    handleExceptions([&]() {
        auto request = req->attributes()->get<RequestJson>("requestJson");
        _dataManager->deactivateEmail(
                req->attributes()->get<string>("accessToken"),
                request["code"].asString()
        );
    }, response);
    response.httpCallback(callback);
}

void Auth::searchRemoved(const HttpRequestPtr &req, function<void(const HttpResponsePtr &)> &&callback) {
    ResponseJson response;
    handleExceptions([&]() {
        auto request = req->attributes()->get<RequestJson>("requestJson");
        response.setData(_dataManager->searchRemoved(request));
    }, response);
    response.httpCallback(callback);
}

void Auth::restoreRemoved(const HttpRequestPtr &req, function<void(const HttpResponsePtr &)> &&callback) {
    ResponseJson response;
    handleExceptions([&]() {
        auto request = req->attributes()->get<RequestJson>("requestJson");
        _dataManager->restoreRemoved(
                request["email"].asString(),
                request["code"].asString()
        );
    }, response);
    response.httpCallback(callback);
}