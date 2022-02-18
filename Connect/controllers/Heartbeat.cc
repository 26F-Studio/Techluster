//
// Created by Particle_G on 2021/8/19.
//

#include <controllers/Heartbeat.h>
#include <helpers/RequestJson.h>
#include <helpers/ResponseJson.h>
#include <plugins/Authorizer.h>
#include <utils/http.h>

using namespace drogon;
using namespace std;
using namespace tech::api::v2;
using namespace tech::helpers;
using namespace tech::plugins;
using namespace tech::structures;
using namespace tech::types;
using namespace tech::utils;

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
        http::fromJson(k200OK, ResponseJson().ref(), callback);
    } catch (const exception &e) {
        ResponseJson response;
        response.setResult(ResultCode::invalidArguments);
        response.setMessage(e.what());
        http::fromJson(k406NotAcceptable, response.ref(), callback);
    }
}
