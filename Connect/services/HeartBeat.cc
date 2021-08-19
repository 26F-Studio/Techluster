//
// Created by Particle_G on 2021/8/19.
//

#include <services/HeartBeat.h>

using namespace drogon;
using namespace std;
using namespace tech::plugins;
using namespace tech::services;
using namespace tech::structures;

HeartBeat::HeartBeat() : _nodeManager(app().getPlugin<NodeManager>()) {}

Json::Value HeartBeat::logon(drogon::HttpStatusCode &code, const Json::Value &request) {
    Json::Value response;
    if (!(
            request.isMember("host") && request["host"].isString() &&
            request.isMember("type") && request["type"].isString() &&
            request.isMember("description") && request["description"].isString()
    )) {
        response["type"] = "Error";
        response["reason"] = "Invalid request";
        code = HttpStatusCode::k400BadRequest;
    } else {
        try {
            auto nodeServer = NodeServer(request["host"].asString(),
                                         _nodeManager->toType(request["type"].asString()),
                                         request["description"].asString());
            if (request.isMember("info") && request["info"].isObject()) {
                nodeServer.setInfo(request["info"]);
            }
            _nodeManager->registerNode(move(nodeServer));
        } catch (runtime_error &error) {
            response["type"] = "Error";
            response["reason"] = error.what();
            code = HttpStatusCode::k406NotAcceptable;
        }
    }
    return response;
}
