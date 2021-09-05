//
// Created by Particle_G on 2021/8/19.
//

#include <plugins/Configurator.h>
#include <services/HeartBeat.h>

using namespace drogon;
using namespace std;
using namespace tech::plugins;
using namespace tech::services;
using namespace tech::structures;
using namespace tech::utils;

HeartBeat::HeartBeat() : _nodeManager(app().getPlugin<NodeManager>()) {}

Json::Value HeartBeat::logon(drogon::HttpStatusCode &code, const Json::Value &request) {
    Json::Value response;
    if (!(
            request.isMember("ip") && request["ip"].isString() &&
            request.isMember("port") && request["port"].isUInt() &&
            request.isMember("type") && request["type"].isString() &&
            request.isMember("taskInterval") && request["taskInterval"].isDouble() &&
            request.isMember("description") && request["description"].isString() &&
            request.isMember("credential") && request["credential"].isString() &&
            app().getPlugin<Configurator>()->checkCredential(request["credential"].asString())
    )) {
        response["type"] = "Error";
        response["reason"] = "Invalid request";
        code = HttpStatusCode::k400BadRequest;
    } else {
        try {
            auto nodeServer = NodeServer(
                    request["ip"].asString(),
                    request["port"].asUInt(),
                    _nodeManager->toType(request["type"].asString()),
                    request["taskInterval"].asDouble(),
                    request["description"].asString()
            );
            if (request.isMember("info") && request["info"].isObject()) {
                nodeServer.setInfo(request["info"]);
            }
            _nodeManager->updateNode(move(nodeServer));
        } catch (runtime_error &error) {
            response["type"] = "Error";
            response["reason"] = error.what();
            code = HttpStatusCode::k406NotAcceptable;
        }
    }
    return response;
}
