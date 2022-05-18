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
        ResponseJsonHandler(
                [](const ResponseException &e, ResponseJson &response) {
                    response.setStatusCode(e.statusCode());
                    // TODO: Check if this causes too much copying
                    response(e.toJson());
                },
                [this](const orm::DrogonDbException &e, ResponseJson &response) {
                    LOG_ERROR << e.base().what();
                    response.setStatusCode(k500InternalServerError);
                    response.setResultCode(ResultCode::DatabaseError);
                    response.setMessage(i18n("databaseError"));
                },
                [this](const exception &e, ResponseJson &response) {
                    LOG_ERROR << e.what();
                    response.setStatusCode(k500InternalServerError);
                    response.setResultCode(ResultCode::InternalError);
                    response.setMessage(i18n("internalError"));
                    response.setReason(e);
                }
        ),
        _nodeManager(app().getPlugin<NodeManager>()) {}

void Heartbeat::report(const HttpRequestPtr &req, function<void(const HttpResponsePtr &)> &&callback) {
    ResponseJson response;
    handleExceptions([&]() {
        auto nodeType = req->attributes()->get<NodeType>("nodeType");
        auto request = req->attributes()->get<RequestJson>("requestJson");
        auto nodeServer = NodeServer(
                nodeType,
                request["ip"].asString(),
                request["port"].asUInt(),
                request["taskInterval"].asDouble(),
                request["description"].asString(),
                request["info"]
        );
        _nodeManager->updateNode(move(nodeServer));
    }, response);
    response.httpCallback(callback);
}
