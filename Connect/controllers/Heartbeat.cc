//
// Created by Particle_G on 2021/8/19.
//

#include <controllers/Heartbeat.h>
#include <helpers/RequestJson.h>
#include <helpers/ResponseJson.h>
#include <plugins/Authorizer.h>

using namespace drogon;
using namespace std;
using namespace tech::api::v2;
using namespace tech::helpers;
using namespace tech::plugins;
using namespace tech::structures;
using namespace tech::types;

Heartbeat::Heartbeat() :
        I18nHelper(CMAKE_PROJECT_NAME),
        _nodeManager(app().getPlugin<NodeManager>()) {}

void Heartbeat::report(const HttpRequestPtr &req, function<void(const HttpResponsePtr &)> &&callback) {
    auto nodeType = req->attributes()->get<NodeType>("nodeType");
    auto request = req->attributes()->get<RequestJson>("requestJson");
    try {
        auto nodeServer = NodeServer(
                nodeType,
                request["ip"].asString(),
                request["port"].asUInt(),
                request["taskInterval"].asDouble(),
                request["description"].asString(),
                request["info"]
        );
        _nodeManager->updateNode(move(nodeServer));
        ResponseJson().httpCallback(callback);
    } catch (const exception &e) {
        ResponseJson response;
        response.setStatusCode(k406NotAcceptable);
        response.setResultCode(ResultCode::invalidArguments);
        response.setMessage(e.what());
        response.httpCallback(callback);
    }
}
