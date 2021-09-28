//
// Created by Particle_G on 2021/9/10.
//

#include <drogon/drogon.h>
#include <plugins/AuthMaintainer.h>
#include <regex>
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
        LOG_ERROR << R"("Invalid AuthMaintainer config")";
        abort();
    } else {
        _connectAddress = InetAddress(
                config["maintainer"]["ip"].asString(),
                config["maintainer"]["port"].asUInt()
        );
        _taskMinutes = chrono::minutes(config["maintainer"]["taskMinutes"].asUInt64());
    }

    updateAuthAddress();

    app().getLoop()->runEvery(_taskMinutes, [this]() {
        updateAuthAddress();
    });

    LOG_INFO << "AuthMaintainer loaded.";
}

void AuthMaintainer::shutdown() { LOG_INFO << "AuthMaintainer shutdown."; }

void AuthMaintainer::updateAuthAddress() {
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
                auto parts = drogon::utils::splitString(
                        response["data"]["host"].asString(),
                        ":"
                );
                if (parts.size() == 2) {
                    _authAddress = InetAddress(
                            parts[0],
                            stoi(parts[1])
                    );
                    LOG_INFO << _authAddress.load().toIpPort();
                } else {
                    LOG_WARN << "No auth node available right now!";
                    _authNodeDown = true;
                }
            }
        } else {
            LOG_WARN << "Request failed (" << static_cast<int>(result) << ")";
        }
    }, 3);
}

trantor::InetAddress AuthMaintainer::getAuthAddress() const {
    return _authAddress;
}
