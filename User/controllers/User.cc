//
// Created by particleg on 2021/9/24.
//

#include <controllers/User.h>

using namespace drogon;
using namespace std;
using namespace tech::api::v2;
using namespace tech::utils;

void User::getInfo(
        const HttpRequestPtr &req,
        function<void(const HttpResponsePtr &)> &&callback
) {
    HttpStatusCode code = HttpStatusCode::k200OK;
    Json::Value data, response;
    auto accessToken = req->getHeader("x-access-token");
    if (accessToken.empty()) {
        code = drogon::k400BadRequest;
        response["type"] = "Error";
        response["reason"] = "Invalid x-access-token header";
        http::fromJson(code, response, callback);
        return;
    }
    auto id = req->getParameter("id");
    data["accessToken"] = accessToken;
    data["id"] = id.empty() ? -1 : stoi(id);
    http::fromJson(code, _service.getInfo(code, data), callback);
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
    request["accessToken"] = accessToken;
    http::fromJson(code, _service.updateInfo(code, request), callback);
}

void User::getAvatar(
        const HttpRequestPtr &req,
        function<void(const HttpResponsePtr &)> &&callback
) {
    HttpStatusCode code = HttpStatusCode::k200OK;
    Json::Value data, response;
    auto accessToken = req->getHeader("x-access-token");
    if (accessToken.empty()) {
        code = drogon::k400BadRequest;
        response["type"] = "Error";
        response["reason"] = "Invalid x-access-token header";
        http::fromJson(code, response, callback);
        return;
    }
    auto id = req->getParameter("id");
    data["accessToken"] = accessToken;
    data["id"] = id.empty() ? -1 : stoi(id);
    http::fromJson(code, _service.getAvatar(code, data), callback);
}