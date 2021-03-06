//
// Created by Particle_G on 2021/8/19.
//

#include <controllers/Node.h>
#include <helpers/ResponseJson.h>
#include <magic_enum.hpp>

using namespace drogon;
using namespace magic_enum;
using namespace std;
using namespace tech::api::v2;
using namespace tech::helpers;
using namespace tech::plugins;
using namespace tech::structures;
using namespace tech::types;
using namespace trantor;

Node::Node() :
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

void Node::allocate(const HttpRequestPtr &req, function<void(const HttpResponsePtr &)> &&callback) {
    ResponseJson response;
    handleExceptions([&]() {
        auto nodeType = req->attributes()->get<NodeType>("nodeType");
        if (nodeType == NodeType::Forwarding) {
            response.setData(_nodeManager->getAllNodes(nodeType));
        } else {
            response.setData(_nodeManager->getBestNode(nodeType));
        }
    }, response);
    response.httpCallback(callback);
}

void Node::check(const HttpRequestPtr &req, function<void(const HttpResponsePtr &)> &&callback) {
    ResponseJson response;
    handleExceptions([&]() {
        _nodeManager->checkNode(
                req->attributes()->get<NodeType>("nodeType"),
                req->attributes()->get<InetAddress>("address")
        );
    }, response);
    response.httpCallback(callback);
}

void Node::report(const HttpRequestPtr &req, function<void(const HttpResponsePtr &)> &&callback) {
    ResponseJson response;
    handleExceptions([&]() {
        const auto &requestJson = req->attributes()->get<RequestJson>("requestJson");
        _nodeManager->updateNode(NodeServer{
                req->attributes()->get<NodeType>("nodeType"),
                requestJson["ip"].asString(),
                static_cast<uint16_t>(requestJson["port"].asUInt()),
                requestJson["taskInterval"].asDouble(),
                requestJson["description"].asString(),
                requestJson["info"]
        });
    }, response);
    response.httpCallback(callback);
}