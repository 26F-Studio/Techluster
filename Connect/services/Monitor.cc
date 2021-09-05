//
// Created by Particle_G on 2021/8/19.
//

#include <plugins/Configurator.h>
#include <services/Monitor.h>

using namespace drogon;
using namespace std;
using namespace tech::plugins;
using namespace tech::services;
using namespace tech::structures;

Monitor::Monitor() :
        _nodeManager(app().getPlugin<NodeManager>()),
        _perfmon(app().getPlugin<Perfmon>()) {}

Json::Value Monitor::selfInfo(drogon::HttpStatusCode &code, const Json::Value &request) {
    Json::Value response;
    if (!(
            request.isMember("credential") && request["credential"].isString() &&
            app().getPlugin<Configurator>()->checkCredential(request["credential"].asString())
    )) {
        response["type"] = "Error";
        response["reason"] = "Invalid request";
        code = HttpStatusCode::k400BadRequest;
    } else {
        response["type"] = "Success";
        response["data"] = _perfmon->parseInfo();
    }
    return response;
}

Json::Value Monitor::othersInfo(drogon::HttpStatusCode &code, const Json::Value &request) {
    Json::Value response;
    if (!(
            request.isMember("credential") && request["credential"].isString() &&
            app().getPlugin<Configurator>()->checkCredential(request["credential"].asString())
    )) {
        response["type"] = "Error";
        response["reason"] = "Invalid request";
        code = HttpStatusCode::k400BadRequest;
    } else {
        response["type"] = "Success";
        response["data"] = _nodeManager->parseInfo();
    }
    return response;
}
