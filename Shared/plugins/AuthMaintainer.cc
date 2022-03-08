//
// Created by Particle_G on 2021/9/10.
//

#include <charconv>
#include <drogon/drogon.h>
#include <helpers/ResponseJson.h>
#include <plugins/AuthMaintainer.h>
#include <structures/Exceptions.h>

using namespace drogon;
using namespace std;
using namespace trantor;
using namespace tech::helpers;
using namespace tech::plugins;
using namespace tech::structures;

void AuthMaintainer::initAndStart(const Json::Value &config) {
    if (!(
            config["maintainer"]["ip"].isString() &&
            config["maintainer"]["port"].isUInt() &&
            config["maintainer"]["taskMinutes"].isUInt64() &&
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

    _updateAuthAddress();

    app().getLoop()->runEvery(_taskMinutes, [this]() {
        _updateAuthAddress();
    });

    LOG_INFO << "AuthMaintainer loaded.";
}

void AuthMaintainer::shutdown() { LOG_INFO << "AuthMaintainer shutdown."; }

string AuthMaintainer::getReportAddress() const { return _connectAddress.load().toIpPort(); }

void AuthMaintainer::_updateAuthAddress() {
    auto client = HttpClient::newHttpClient("http://" + _connectAddress.load().toIpPort());
    auto req = HttpRequest::newHttpRequest();
    req->setPath("/tech/api/v2/allocator/user");
    client->sendRequest(req, [this](ReqResult result, const HttpResponsePtr &responsePtr) {
        if (result != ReqResult::Ok) {
            LOG_ERROR << "Request failed (" << static_cast<int>(result) << ")";
            return;
        }
        try {
            ResponseJson response(responsePtr);
            if (responsePtr->getStatusCode() != k200OK) {
                LOG_WARN << "Request failed (" << responsePtr->getStatusCode() << "): \n"
                         << response.stringify();
                return;
            }
            auto parts = drogon::utils::splitString(response["data"].asString(), ":");
            if (parts.size() == 2) {
                _authAddress = InetAddress(parts[0], stoi(parts[1]));
                LOG_INFO << "Retrieved user node: " << _authAddress.load().toIpPort();
            } else {
                // TODO: Send an email if failed too many times.
                LOG_WARN << "No user node available right now!";
            }
        } catch (const json_exception::InvalidFormat &e) {
            LOG_WARN << "Invalid response body (" << responsePtr->getStatusCode() << "): \n"
                     << e.what();
        }
    }, 3);
}

HttpStatusCode AuthMaintainer::checkAccessToken(const string &accessToken, int64_t &id) {
    auto client = HttpClient::newHttpClient("http://" + _authAddress.load().toIpPort());
    auto req = HttpRequest::newHttpRequest();
    req->setPath("/tech/api/v2/auth/check");
    req->addHeader("x-access-token", accessToken);
    auto[result, responsePtr] = client->sendRequest(req, 3);
    if (result != ReqResult::Ok) {
        _updateAuthAddress();
        throw NetworkException("User node is down", result);
    }
    try {
        ResponseJson response(responsePtr);
        if (!response["data"].isInt64()) {
            throw NetworkException("Invalid Response: " + response.stringify(), ReqResult::BadResponse);
        }
        id = response["data"].asInt64();
        return responsePtr->statusCode();
    } catch (const json_exception::InvalidFormat &e) {
        throw NetworkException("Invalid Json: "s + e.what(), ReqResult::BadResponse);
    }
}
