//
// Created by Particle_G on 2021/8/19.
//

#include <controllers/Allocator.h>
#include <plugins/Authorizer.h>

using namespace drogon;
using namespace std;
using namespace tech::api::v2;
using namespace tech::plugins;
using namespace tech::utils;

void Allocator::message(const HttpRequestPtr &req, function<void(const HttpResponsePtr &)> &&callback) {
    HttpStatusCode code = HttpStatusCode::k200OK;
    http::fromJson(code, _service.message(code), callback);
}

void Allocator::gaming(const HttpRequestPtr &req, function<void(const HttpResponsePtr &)> &&callback) {
    HttpStatusCode code = HttpStatusCode::k200OK;
    http::fromJson(code, _service.gaming(code), callback);
}

void Allocator::transfers(const HttpRequestPtr &req, function<void(const HttpResponsePtr &)> &&callback) {
    HttpStatusCode code = HttpStatusCode::k200OK;
    http::fromJson(code, _service.transfers(code), callback);
}

void Allocator::transfer(const HttpRequestPtr &req, function<void(const drogon::HttpResponsePtr &)> &&callback) {
    HttpStatusCode code = HttpStatusCode::k200OK;
    Json::Value request, response;
    auto credential = req->getHeader("x-credential");
    if (credential.empty() || !app().getPlugin<Authorizer>()->checkCredential(credential)) {
        code = drogon::k400BadRequest;
        response["type"] = "Error";
        response["reason"] = "Invalid x-credential header";
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
    http::fromJson(code, _service.transfer(code, request), callback);
}

void Allocator::user(const HttpRequestPtr &req, function<void(const HttpResponsePtr &)> &&callback) {
    HttpStatusCode code = HttpStatusCode::k200OK;
    http::fromJson(code, _service.user(code), callback);
}