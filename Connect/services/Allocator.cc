//
// Created by Particle_G on 2021/8/19.
//

#include <plugins/Configurator.h>
#include <services/Allocator.h>

using namespace drogon;
using namespace std;
using namespace tech::plugins;
using namespace tech::services;
using namespace tech::structures;

Allocator::Allocator() : _nodeManager(app().getPlugin<NodeManager>()) {}

Json::Value Allocator::message(drogon::HttpStatusCode &code) {
    Json::Value response;
    string host;
    if (getNode(host, NodeServer::Type::message)) {
        response["type"] = "Success";
        response["data"]["host"] = host;
    } else {
        code = HttpStatusCode::k503ServiceUnavailable;
        response["type"] = "Failure";
        response["reason"] = "No node server available now";
    }
    return response;
}

Json::Value Allocator::gaming(drogon::HttpStatusCode &code) {
    Json::Value response;
    string host;
    if (getNode(host, NodeServer::Type::gaming)) {
        response["type"] = "Success";
        response["data"]["host"] = host;
    } else {
        code = HttpStatusCode::k503ServiceUnavailable;
        response["type"] = "Failure";
        response["reason"] = "No node server available now";
    }
    return response;
}

Json::Value Allocator::user(drogon::HttpStatusCode &code) {
    Json::Value response;
    string host;
    if (getNode(host, NodeServer::Type::user)) {
        response["type"] = "Success";
        response["data"]["host"] = host;
    } else {
        code = HttpStatusCode::k503ServiceUnavailable;
        response["type"] = "Failure";
        response["reason"] = "No node server available now";
    }
    return response;
}

bool Allocator::getNode(string &host, const NodeServer::Type &type) {
    try {
        host = _nodeManager->getBestNode(type);
        return true;
    } catch (exception &e) {
        return false;
    }
}
