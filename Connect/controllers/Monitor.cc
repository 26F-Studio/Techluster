//
// Created by Particle_G on 2021/8/19.
//

#include <controllers/Monitor.h>
#include <plugins/Authorizer.h>

using namespace drogon;
using namespace std;
using namespace tech::api::v2;
using namespace tech::plugins;
using namespace tech::utils;

void Monitor::selfInfo(const HttpRequestPtr &req, function<void(const HttpResponsePtr &)> &&callback) {
    HttpStatusCode code = HttpStatusCode::k200OK;
    Json::Value response;
    auto credential = req->getHeader("x-credential");
    if (credential.empty() || !app().getPlugin<Authorizer>()->checkCredential(credential)) {
        code = drogon::k400BadRequest;
        response["type"] = "Error";
        response["reason"] = "Invalid x-credential header";
        http::fromJson(code, response, callback);
        return;
    }
    http::fromJson(code, _service.selfInfo(code), "*", callback);
}

void Monitor::othersInfo(
        const HttpRequestPtr &req,
        function<void(const HttpResponsePtr &)> &&callback,
        const string &nodeType
) {
    HttpStatusCode code = HttpStatusCode::k200OK;
    Json::Value response;
    auto credential = req->getHeader("x-credential");
    if (credential.empty() || !app().getPlugin<Authorizer>()->checkCredential(credential)) {
        code = drogon::k400BadRequest;
        response["type"] = "Error";
        response["reason"] = "Invalid x-credential header";
        http::fromJson(code, response, callback);
        return;
    }
    http::fromJson(code, _service.othersInfo(code, nodeType), "*", callback);
}
