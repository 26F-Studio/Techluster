//
// Created by Particle_G on 2021/8/19.
//

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
    if (_getNode(host, NodeServer::Type::message)) {
        response["type"] = "Success";
        response["data"] = host;
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
    if (_getNode(host, NodeServer::Type::gaming)) {
        response["type"] = "Success";
        response["data"] = host;
    } else {
        code = HttpStatusCode::k503ServiceUnavailable;
        response["type"] = "Failure";
        response["reason"] = "No node server available now";
    }
    return response;
}

Json::Value Allocator::transfers(HttpStatusCode &code) {
    Json::Value response;
    response["type"] = "Success";
    response["data"] = _nodeManager->getAllNodes(NodeServer::Type::transfer);
    return response;
}

Json::Value Allocator::transfer(HttpStatusCode &code, const Json::Value &request) {
    // TODO: Implement delay calculation
    Json::Value response;
    string host;
    if (_getNode(host, NodeServer::Type::transfer)) {
        response["type"] = "Success";
        response["data"] = host;
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
    if (_getNode(host, NodeServer::Type::user)) {
        response["type"] = "Success";
        response["data"] = host;
    } else {
        code = HttpStatusCode::k503ServiceUnavailable;
        response["type"] = "Failure";
        response["reason"] = "No node server available now";
    }
    return response;
}

bool Allocator::_getNode(string &host, const NodeServer::Type &type) {
    try {
        host = _nodeManager->getBestNode(type);
        return true;
    } catch (exception &e) {
        return false;
    }
}
