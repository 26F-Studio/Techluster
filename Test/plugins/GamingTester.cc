//
// Created by Parti on 2021/2/4.
//

#include <drogon/drogon.h>
#include <format>
#include <magic_enum.hpp>
#include <plugins/GamingTester.h>
#include <structures/Exceptions.h>

using namespace drogon;
using namespace magic_enum;
using namespace std;
using namespace tech::plugins;
using namespace tech::structures;
using namespace tech::types;

void GamingTester::processConfig(const Json::Value &config) {
    _userNode = config["userNode"].asString();
    _password = config["password"].asString();
    transform(
            config["emails"].begin(),
            config["emails"].end(),
            back_inserter(_emails),
            [](const Json::Value &item) { return item.asString(); }
    );
}

void GamingTester::connect() {
    atomic<uint64_t> connected;
    for (const auto &email: _emails) {
        Json::Value loginBody;
        loginBody["email"] = email;
        loginBody["password"] = _password;
        const auto tokens = _httpRequest(
                _userNode,
                Post,
                "/auth/login/email",
                {},
                {},
                loginBody
        )["data"];
        const auto accessToken = tokens["accessToken"].asString();
        const auto refreshToken = tokens["refreshToken"].asString();
        const auto id = _httpRequest(
                _userNode,
                Get,
                "/auth/check",
                {},
                {make_pair("x-access-token", accessToken)}
        )["data"].asInt64();
        const auto info = _httpRequest(
                _userNode,
                Get,
                "/user/info",
                {make_pair("userId", to_string(id))},
                {make_pair("x-access-token", accessToken)}
        )["data"];

        _wsMap.emplace(id, _websocketClient(
                _host,
                "/gaming",
                {make_pair("x-access-token", accessToken)},
                [=, &connected](ReqResult reqResult,
                                const HttpResponsePtr &responsePtr,
                                const WebSocketClientPtr &wsPtr) {
                    if (reqResult != ReqResult::Ok) {
                        wsPtr->stop();
                        colorOut(format("Player {} connect failed!", id), WebHelper::Color::red);
                        throw NetworkException("WebSocket connection failed", reqResult);
                    }
                    wsPtr->getConnection()->setContext(make_shared<Player>(Player{
                            id,
                            info["username"].asString(),
                            info["motto"].asString(),
                            enum_cast<Permission>(info["permission"].asString()).value(),
                            email,
                            accessToken,
                            refreshToken
                    }));
                    connected++;
                    colorOut(format("Player {} connected", id), WebHelper::Color::white);
                },
                [](const std::string &message,
                   const WebSocketClientPtr &,
                   const WebSocketMessageType &type) {
                    LOG_INFO << enum_name(type) << ": " << message;
                }
        ));
    }
    while (connected < _emails.size()) {
        this_thread::sleep_for(chrono::milliseconds(100));
    }
}

void GamingTester::disconnect() {
    for (const auto &[_, wsClient]: _wsMap) {
        const auto &connection = wsClient->getConnection();
        if (connection && connection->connected()) {
            connection->forceClose();
        }
    }
    _wsMap.clear();
}
