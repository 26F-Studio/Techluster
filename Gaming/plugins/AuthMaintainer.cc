//
// Created by Particle_G on 2021/9/10.
//

#include <drogon/drogon.h>
#include <plugins/AuthMaintainer.h>
#include <ranges>
#include <utils/http.h>
#include <utils/serializer.h>

using namespace drogon;
using namespace std;
using namespace trantor;
using namespace tech::plugins;
using namespace tech::utils;

void AuthMaintainer::initAndStart(const Json::Value &config) {
    if (!(
            config.isMember("maintainer") && config["maintainer"].isObject() &&
            config["maintainer"].isMember("ip") && config["maintainer"]["ip"].isString() &&
            config["maintainer"].isMember("port") && config["maintainer"]["port"].isUInt() &&
            config["maintainer"].isMember("taskMinutes") && config["maintainer"]["taskMinutes"].isUInt64() &&
            config["maintainer"]["taskMinutes"].asUInt64() >= 10
    )) {
        LOG_ERROR << R"("Invalid perfmon config")";
        abort();
    } else {
        _connectAddress = InetAddress(
                config["maintainer"]["ip"].asString(),
                config["maintainer"]["port"].asUInt()
        );
        _taskMinutes = chrono::minutes(config["maintainer"]["taskMinutes"].asUInt64());
    }

    app().getLoop()->runEvery(_taskMinutes, [this]() {
        updateNodeAddress();
    });

    LOG_INFO << "AuthMaintainer loaded.";
}

void AuthMaintainer::shutdown() { LOG_INFO << "AuthMaintainer shutdown."; }

void AuthMaintainer::updateNodeAddress() {
    auto client = HttpClient::newHttpClient("http://" + _connectAddress.load().toIpPort());
    auto req = HttpRequest::newHttpRequest();
    req->setPath("/tech/api/v2/allocator/user");
    client->sendRequest(req, [this](ReqResult result, const HttpResponsePtr &responsePtr) {
        if (result == ReqResult::Ok) {
            Json::Value response;
            string parseError = http::toJson(responsePtr, response);
            if (!parseError.empty()) {
                LOG_WARN << "Invalid response body (" << responsePtr->getStatusCode() << "): \n"
                         << parseError;
            } else if (responsePtr->getStatusCode() != k200OK) {
                LOG_WARN << "Request failed (" << responsePtr->getStatusCode() << "): \n"
                         << serializer::json::stringify(response);
            } else {
                auto host = response["data"]["host"].asString();
                auto splits = views::split(host, ":") |
                              views::transform([](auto v) {
                                  auto c = v | views::common;
                                  return string(c.begin(), c.end());
                              });
                _authAddress = InetAddress(
                        *(splits.begin()),
                        stoi(*(++splits.begin()))
                );
            }
        } else {
            LOG_WARN << "Request failed (" << static_cast<int>(result) << ")";
        }
    }, 3);
}
