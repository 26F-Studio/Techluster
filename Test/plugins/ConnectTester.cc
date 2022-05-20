//
// Created by Parti on 2021/2/4.
//

#include <magic_enum.hpp>
#include <plugins/ConnectTester.h>
#include <plugins/Perfmon.h>
#include <types/ResultCode.h>

using namespace drogon;
using namespace magic_enum;
using namespace std;
using namespace tech::helpers;
using namespace tech::plugins;
using namespace tech::types;

void ConnectTester::processConfig(const Json::Value &config) {
    _heartbeatBody = config["report"];
    _credentialHeaders["x-credential"] = config["credential"].asString();
}

Json::Value ConnectTester::allocate(const string &type) {
    WebHelper::colorOut("Retrieve node type: " + type, WebHelper::Color::cyan);
    return _httpRequest(
            Get,
            "/node/allocate",
            {make_pair("nodeType", type)}
    );
}

bool ConnectTester::report(const string &type, uint32_t port) {
    WebHelper::colorOut("Simulate " + type + " report at " + to_string(port), WebHelper::Color::cyan);
    Json::Value body(_heartbeatBody);
    body["port"] = port;
    body["info"] = app().getPlugin<Perfmon>()->parseInfo();
    return _httpRequest(
            Post,
            "/node/report",
            {make_pair("nodeType", type)},
            _credentialHeaders,
            body
    )["code"].asUInt() == enum_integer(ResultCode::Completed);
}

std::string ConnectTester::toHost(uint32_t port) {
    return _heartbeatBody["ip"].asString().append(":").append(to_string(port));
}

DataJson ConnectTester::monitor(const string &type) {
    WebHelper::colorOut("Monitor node type: " + type, WebHelper::Color::cyan);
    return DataJson(_httpRequest(
            Get,
            "/monitor",
            {make_pair("nodeType", type)}
    ));
}
