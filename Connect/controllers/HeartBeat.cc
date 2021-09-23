//
// Created by Particle_G on 2021/8/19.
//

#include <controllers/HeartBeat.h>

using namespace drogon;
using namespace std;
using namespace tech::api::v2;
using namespace tech::utils;

void HeartBeat::report(const HttpRequestPtr &req, function<void(const HttpResponsePtr &)> &&callback) {
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
    http::fromJson(code, _service.report(code, request), callback);
}
