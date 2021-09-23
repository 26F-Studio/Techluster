//
// Created by Particle_G on 2021/8/19.
//

#include <services/Monitor.h>

using namespace drogon;
using namespace std;
using namespace tech::plugins;
using namespace tech::services;
using namespace tech::structures;

Monitor::Monitor() :
        _nodeManager(app().getPlugin<NodeManager>()),
        _perfmon(app().getPlugin<Perfmon>()) {}

Json::Value Monitor::selfInfo(HttpStatusCode &code) {
    Json::Value response;
    response["type"] = "Success";
    response["data"] = _perfmon->parseInfo();
    return response;
}

Json::Value Monitor::othersInfo(HttpStatusCode &code, const string &nodeType) {
    Json::Value response;
    try {
        auto type = _nodeManager->toType(nodeType);
        response["type"] = "Success";
        response["data"] = _nodeManager->parseInfo(type);
    } catch (exception &e) {
        response["type"] = "Error";
        response["reason"] = "Invalid nodeType";
        code = HttpStatusCode::k406NotAcceptable;
    }
    return response;
}
