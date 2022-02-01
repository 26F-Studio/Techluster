//
// Created by particleg on 2021/9/24.
//

#include <controllers/User.h>

using namespace drogon;
using namespace std;
using namespace tech::api::v2;
using namespace tech::structures;
using namespace tech::utils;

void User::getInfo(const HttpRequestPtr &req, function<void(const HttpResponsePtr &)> &&callback) {
    HttpStatusCode code = HttpStatusCode::k200OK;
    Json::Value response;
    auto accessToken = req->getHeader("x-access-token");
    if (accessToken.empty()) {
        code = drogon::k400BadRequest;
        response["type"] = "Error";
        response["reason"] = "Invalid x-access-token header";
        http::fromJson(code, response, callback);
        return;
    }
    auto id = req->getParameter("id");
    http::fromJson(code, _service.getInfo(
            code,
            accessToken,
            id.empty() ? -1 : stoll(id)
    ), callback);
}

void User::updateInfo(const HttpRequestPtr &req, function<void(const HttpResponsePtr &)> &&callback) {
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
    auto accessToken = req->getHeader("x-access-token");
    if (accessToken.empty()) {
        code = drogon::k400BadRequest;
        response["type"] = "Error";
        response["reason"] = "Invalid x-access-token header";
        http::fromJson(code, response, callback);
        return;
    }
    http::fromJson(code, _service.updateInfo(
            code,
            accessToken,
            request
    ), callback);
}

void User::getAvatar(const HttpRequestPtr &req, function<void(const HttpResponsePtr &)> &&callback) {
    HttpStatusCode code = HttpStatusCode::k200OK;
    Json::Value response;
    auto accessToken = req->getHeader("x-access-token");
    if (accessToken.empty()) {
        code = drogon::k400BadRequest;
        response["type"] = "Error";
        response["reason"] = "Invalid x-access-token header";
        http::fromJson(code, response, callback);
        return;
    }
    auto id = req->getParameter("id");
    http::fromJson(code, _service.getAvatar(
            code,
            accessToken,
            id.empty() ? -1 : stoll(id)
    ), callback);
}

void User::getData(const HttpRequestPtr &req, function<void(const drogon::HttpResponsePtr &)> &&callback) {
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
    auto accessToken = req->getHeader("x-access-token");
    if (accessToken.empty()) {
        code = drogon::k400BadRequest;
        response["type"] = "Error";
        response["reason"] = "Invalid x-access-token header";
        http::fromJson(code, response, callback);
        return;
    }
    auto id = req->getParameter("id");
    auto field = req->attributes()->get<DataField>("field");
    http::fromJson(code, _service.getData(
            code,
            accessToken,
            id.empty() ? -1 : stoll(id),
            field,
            request
    ), callback);
}

void User::updateData(const HttpRequestPtr &req, function<void(const drogon::HttpResponsePtr &)> &&callback) {
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
    auto accessToken = req->getHeader("x-access-token");
    if (accessToken.empty()) {
        code = drogon::k400BadRequest;
        response["type"] = "Error";
        response["reason"] = "Invalid x-access-token header";
        http::fromJson(code, response, callback);
        return;
    }
    auto field = req->attributes()->get<DataField>("field");
    http::fromJson(code, _service.updateData(
            code,
            accessToken,
            field,
            request
    ), callback);
}
