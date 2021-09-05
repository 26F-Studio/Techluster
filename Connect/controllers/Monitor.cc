//
// Created by Particle_G on 2021/8/19.
//

#include <controllers/Monitor.h>

using namespace drogon;
using namespace std;
using namespace tech::api::v2;
using namespace tech::utils;

void Monitor::selfInfo(const HttpRequestPtr &req, function<void(const HttpResponsePtr &)> &&callback) {
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
    http::fromJson(code, _service.selfInfo(code, request), callback);
}

void Monitor::othersInfo(const HttpRequestPtr &req, function<void(const HttpResponsePtr &)> &&callback) {
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
    http::fromJson(code, _service.othersInfo(code, request), callback);
}
