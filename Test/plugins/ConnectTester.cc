//
// Created by Parti on 2021/2/4.
//

#include <plugins/ConnectTester.h>
#include <plugins/Perfmon.h>
#include <types/ResultCode.h>
#include <utils/data.h>

using namespace drogon;
using namespace std;
using namespace tech::plugins;
using namespace tech::types;
using namespace tech::utils::data;

void ConnectTester::processConfig(const Json::Value &config) {
    _heartbeatBody = config["heartbeat"];
    _credentialHeaders["x-credential"] = config["credential"].asString();
}

Json::Value ConnectTester::allocator(const string &type) {
    WebHelper::colorOut("Retrieve node type: " + type, WebHelper::Color::cyan);
    return _httpRequest(
            Get,
            "/allocator",
            {make_pair("nodeType", type)}
    );
}

bool ConnectTester::heartbeat(const string &type, const uint32_t &port) {
    WebHelper::colorOut("Simulate " + type + " heartbeat at " + to_string(port), WebHelper::Color::cyan);
    Json::Value body(_heartbeatBody);
    body["port"] = port;
    body["info"] = app().getPlugin<Perfmon>()->parseInfo();
    return _httpRequest(
            Post,
            "/heartbeat/report",
            {make_pair("nodeType", type)},
            _credentialHeaders,
            body
    )["code"].asUInt() == +ResultCode::completed;
}

std::string ConnectTester::toHost(const uint32_t &port) {
    return _heartbeatBody["ip"].asString().append(":").append(to_string(port));
}

Json::Value ConnectTester::monitor(const string &type) {
    WebHelper::colorOut("Monitor node type: " + type, WebHelper::Color::cyan);
    return _httpRequest(
            Get,
            "/monitor",
            {make_pair("nodeType", type)}
    );
}
