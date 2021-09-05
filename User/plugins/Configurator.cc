//
// Created by Parti on 2021/2/4.
//

#include <drogon/drogon.h>
#include <plugins/Configurator.h>
#include <plugins/Perfmon.h>
#include <utils/http.h>
#include <utils/serializer.h>

using namespace drogon;
using namespace std;
using namespace tech::plugins;
using namespace tech::utils;

void Configurator::initAndStart(const Json::Value &config) {
    if (config.isMember("superusers") && config["superusers"].isArray()) {
        for (const auto &superuser: config["superusers"]) {
            _superusers.insert(superuser.asInt64());
        }
    }

    bool isLocal = false;
    string description;
    if (config.isMember("heartBeat") && config["heartBeat"].isObject()) {
        if (config["heartBeat"].isMember("isLocal") && config["heartBeat"]["isLocal"].isBool()) {
            isLocal = config["heartBeat"]["isLocal"].asBool();
        }
        if (config["heartBeat"].isMember("description") && config["heartBeat"]["description"].isString()) {
            description = config["heartBeat"]["description"].asString();
        }
    }

    if (config.isMember("credential") && config["credential"].isString()) {
        _credential = config["credential"].asString();
    } else {
        LOG_ERROR << R"("Invalid credential config")";
        abort();
    }

    if (config.isMember("connectGateway") && config["connectGateway"].isString()) {
        _connectGateway = config["connectGateway"].asString();
    } else {
        LOG_ERROR << R"("Invalid connectGateway config")";
        abort();
    }

    _heartbeatBody["ip"] = isLocal ? "127.0.0.1" :
                           string(HttpClient::newHttpClient("https://api.ipify.org/")->sendRequest(
                                   HttpRequest::newHttpRequest()
                           ).second->body());
    _heartbeatBody["port"] = app().getListeners()[0].toPort();
    _heartbeatBody["type"] = "user";
    _heartbeatBody["taskInterval"] = app().getPlugin<Perfmon>()->getTaskInterval();
    _heartbeatBody["description"] = description;
    _heartbeatBody["credential"] = _credential;

    app().getLoop()->runEvery(
            _heartbeatBody["taskInterval"].asDouble(),
            [this]() {
                _heartbeatBody["info"] = app().getPlugin<Perfmon>()->parseInfo();
                auto client = HttpClient::newHttpClient("http://" + _connectGateway);
                auto req = HttpRequest::newHttpJsonRequest(_heartbeatBody);
                req->setMethod(Post);
                req->setPath("/tech/api/v2/heartbeat/report");
                client->sendRequest(req, [](ReqResult result, const HttpResponsePtr &responsePtr) {
                    if (result == ReqResult::Ok) {
                        Json::Value response;
                        string parseError = http::toJson(responsePtr, response);
                        if (!parseError.empty()) {
                            LOG_WARN << "Invalid response body (" << responsePtr->getStatusCode() << "): \n"
                                     << parseError;
                        } else if (responsePtr->getStatusCode() != k200OK) {
                            LOG_WARN << "Request failed (" << responsePtr->getStatusCode() << "): \n"
                                     << serializer::json::stringify(response);
                        }
                    } else {
                        LOG_WARN << "Request failed (" << static_cast<int>(result) << ")";
                    }
                }, 3);
            }
    );

    LOG_INFO << "Configurator loaded.";
}

void Configurator::shutdown() { LOG_INFO << "Configurator shutdown."; }

bool Configurator::isSuperusers(const int64_t &uid) const { return _superusers.contains(uid); }

bool Configurator::checkCredential(const string &credential) const { return credential == _credential; }