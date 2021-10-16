//
// Created by Particle_G on 2021/8/19.
//

#pragma once

#include <plugins/NodeManager.h>
#include <utils/http.h>

namespace tech::services {
    class Allocator {
    public:
        Allocator();

        Json::Value message(drogon::HttpStatusCode &code);

        Json::Value gaming(drogon::HttpStatusCode &code);

        Json::Value transfers(drogon::HttpStatusCode &code);

        Json::Value transfer(drogon::HttpStatusCode &code, const Json::Value &request);

        Json::Value user(drogon::HttpStatusCode &code);

    private:
        tech::plugins::NodeManager *_nodeManager;

        bool _getNode(std::string &host, const tech::structures::NodeServer::Type &type);
    };
}
