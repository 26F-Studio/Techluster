//
// Created by particleg on 2021/9/24.
//

#include <controllers/Auth.h>

using namespace drogon;
using namespace std;
using namespace tech::api::v2;
using namespace tech::utils;

void Auth::refresh(const HttpRequestPtr &req, function<void(const HttpResponsePtr &)> &&callback) {
    HttpStatusCode code = HttpStatusCode::k200OK;
    Json::Value data, response;
    auto refreshToken = req->getHeader("x-refresh-token");
    if (refreshToken.empty()) {
        code = drogon::k400BadRequest;
        response["type"] = "Error";
        response["reason"] = "Invalid x-refresh-token header";
        http::fromJson(code, response, callback);
        return;
    }
    data["refreshToken"] = refreshToken;
    http::fromJson(code, _service.refresh(code, data), callback);
}

void Auth::verifyEmail(const HttpRequestPtr &req, function<void(const drogon::HttpResponsePtr &)> &&callback) {
    HttpStatusCode code = HttpStatusCode::k200OK;
    Json::Value response;
    http::fromJson(code, _service.verifyEmail(
            code,
            req->attributes()->get<Json::Value>("data")
    ), callback);
}

void Auth::loginEmail(const HttpRequestPtr &req, function<void(const HttpResponsePtr &)> &&callback) {
    HttpStatusCode code = HttpStatusCode::k200OK;
    Json::Value request, response;
    string parseError = http::toJson(req, request);
    if (!parseError.empty()) {
        code = drogon::k400BadRequest;
        response["type"] = "Error";
        response["reason"] = "Wrong format: " + parseError;
        http::fromJson(code, response, callback);
        return;
    }
    if (!(
            request.isMember("email") && request["email"].isString() &&
            (
                    (request.isMember("code") && request["code"].isString()) ||
                    (request.isMember("password") && request["password"].isString())
            )
    )) {
        code = drogon::k400BadRequest;
        response["type"] = "Error";
        response["reason"] = "Invalid parameters";
        http::fromJson(code, response, callback);
        return;
    }
    http::fromJson(code, _service.loginMail(code, request), callback);
}

void Auth::resetEmail(const HttpRequestPtr &req, function<void(const HttpResponsePtr &)> &&callback) {
    HttpStatusCode code = HttpStatusCode::k200OK;
    Json::Value request, response;
    string parseError = http::toJson(req, request);
    if (!parseError.empty()) {
        code = drogon::k400BadRequest;
        response["type"] = "Error";
        response["reason"] = "Wrong format: " + parseError;
        http::fromJson(code, response, callback);
        return;
    }
    if (!(
            request.isMember("email") && request["email"].isString() &&
            request.isMember("code") && request["code"].isString() &&
            request.isMember("newPassword") && request["newPassword"].isString()
    )) {
        code = drogon::k400BadRequest;
        response["type"] = "Error";
        response["reason"] = "Invalid parameters";
        http::fromJson(code, response, callback);
        return;
    }
    http::fromJson(code, _service.resetEmail(code, request), callback);
}

void Auth::migrateEmail(
        const HttpRequestPtr &req,
        function<void(const drogon::HttpResponsePtr &)> &&callback
) {
    HttpStatusCode code = HttpStatusCode::k200OK;
    Json::Value request, response;
    auto accessToken = req->getHeader("x-access-token");
    if (accessToken.empty()) {
        code = drogon::k400BadRequest;
        response["type"] = "Error";
        response["reason"] = "Invalid x-access-token header";
        http::fromJson(code, response, callback);
        return;
    }
    string parseError = http::toJson(req, request);
    if (!parseError.empty()) {
        code = drogon::k400BadRequest;
        response["type"] = "Error";
        response["reason"] = "Wrong format: " + parseError;
        http::fromJson(code, response, callback);
        return;
    }
    if (!(
            request.isMember("newEmail") && request["newEmail"].isString() &&
            request.isMember("code") && request["code"].isString()
    )) {
        code = drogon::k400BadRequest;
        response["type"] = "Error";
        response["reason"] = "Invalid parameters";
        http::fromJson(code, response, callback);
        return;
    }
    request["accessToken"] = accessToken;
    http::fromJson(code, _service.migrateEmail(code, request), callback);
}
