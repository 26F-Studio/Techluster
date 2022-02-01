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

void Allocator::transfer(const HttpRequestPtr &req, function<void(const HttpResponsePtr &)> &&callback) {
    HttpStatusCode code = HttpStatusCode::k200OK;
    http::fromJson(code, _service.transfer(code), callback);
}

void Allocator::user(const HttpRequestPtr &req, function<void(const HttpResponsePtr &)> &&callback) {
    HttpStatusCode code = HttpStatusCode::k200OK;
    http::fromJson(code, _service.user(code), callback);
}