//
// Created by Particle_G on 2021/9/10.
//

#include <drogon/drogon.h>
#include <magic_enum.hpp>
#include <helpers/RequestJson.h>
#include <plugins/NodeMaintainer.h>
#include <structures/Exceptions.h>
#include <types/JsonValue.h>

using namespace drogon;
using namespace magic_enum;
using namespace std;
using namespace trantor;
using namespace tech::helpers;
using namespace tech::plugins;
using namespace tech::structures;
using namespace tech::types;

void NodeMaintainer::initAndStart(const Json::Value &config) {
    if (!(
            config["maintainer"]["ip"].isString() &&
            config["maintainer"]["port"].isUInt() &&
            config["maintainer"]["taskMinutes"].isUInt64() &&
            config["maintainer"]["taskMinutes"].asUInt64() >= 10
    )) {
        LOG_ERROR << R"("Invalid NodeMaintainer config")";
        abort();
    } else {
        connectAddress = InetAddress(
                config["maintainer"]["ip"].asString(),
                config["maintainer"]["port"].asUInt()
        );
        _taskMinutes = chrono::minutes(config["maintainer"]["taskMinutes"].asUInt64());
    }

    _updateUserAddress();

    app().getLoop()->runEvery(_taskMinutes, [this]() {
        _updateUserAddress();
    });

    LOG_INFO << "NodeMaintainer loaded.";
}

void NodeMaintainer::shutdown() { LOG_INFO << "NodeMaintainer shutdown."; }

bool NodeMaintainer::checkNode(NodeType type, const string &address) const {
    auto client = HttpClient::newHttpClient("http://" + connectAddress.load().toIpPort());
    auto req = HttpRequest::newHttpRequest();
    req->setPath("/tech/api/v2/node/check");
    req->setParameter("nodeType", string(enum_name(type)));
    auto [result, responsePtr] = client->sendRequest(req, 3);

    if (result != ReqResult::Ok) {
        throw NetworkException("Connect node is down", result);
    }
    return responsePtr->getStatusCode() == k200OK;
}

HttpStatusCode NodeMaintainer::checkAccessToken(const string &accessToken, int64_t &id) {
    auto client = HttpClient::newHttpClient("http://" + userAddress.load().toIpPort());
    auto req = HttpRequest::newHttpRequest();
    req->setPath("/tech/api/v2/auth/check");
    req->addHeader("x-access-token", accessToken);
    auto [result, responsePtr] = client->sendRequest(req, 3);
    if (result != ReqResult::Ok) {
        _updateUserAddress();
        throw NetworkException("User node is down", result);
    }
    try {
        RequestJson response(responsePtr);
        response.require("data", JsonValue::Int64);
        id = response["data"].asInt64();
        return responsePtr->statusCode();
    } catch (const json_exception::InvalidFormat &e) {
        throw NetworkException("Invalid Json: "s + e.what(), ReqResult::BadResponse);
    }
}

tuple<string, string> NodeMaintainer::getWorkshopItem(const std::string &itemId) {
    auto client = HttpClient::newHttpClient("http://" + workshopAddress.load().toIpPort());
    auto req = HttpRequest::newHttpRequest();
    req->setPath("/tech/api/v2/item/download");
    req->setParameter("itemId", itemId);
    auto [result, responsePtr] = client->sendRequest(req, 3);
    if (result != ReqResult::Ok) {
        _updateWorkshopAddress();
        throw NetworkException("Workshop node is down", result);
    }
    try {
        RequestJson response(responsePtr);
        response.require("data", JsonValue::Object);

        RequestJson data(move(response.ref()["data"]));
        data.require("filename", JsonValue::String);
        data.require("data", JsonValue::String);

        return {
                drogon::utils::base64Decode(data["data"].asString()),
                data["filename"].asString()
        };
    } catch (const json_exception::InvalidFormat &e) {
        throw NetworkException("Invalid Json: "s + e.what(), ReqResult::BadResponse);
    };
}

void NodeMaintainer::_updateUserAddress() {
    auto client = HttpClient::newHttpClient("http://" + connectAddress.load().toIpPort());
    auto req = HttpRequest::newHttpRequest();
    req->setPath("/tech/api/v2/node/allocate");
    req->setParameter("nodeType", "user");
    client->sendRequest(req, [this](ReqResult result, const HttpResponsePtr &responsePtr) {
        if (result != ReqResult::Ok) {
            LOG_ERROR << "Request failed (" << static_cast<int>(result) << ")";
            return;
        }
        try {
            RequestJson response(responsePtr);
            if (responsePtr->getStatusCode() != k200OK) {
                LOG_WARN << "Request failed (" << responsePtr->getStatusCode() << "): \n"
                         << response.stringify();
                return;
            }
            auto parts = drogon::utils::splitString(response["data"].asString(), ":");
            if (parts.size() == 2) {
                userAddress = InetAddress(parts[0], stoi(parts[1]));
                LOG_INFO << "Retrieved user node: " << userAddress.load().toIpPort();
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

void NodeMaintainer::_updateWorkshopAddress() {
    auto client = HttpClient::newHttpClient("http://" + connectAddress.load().toIpPort());
    auto req = HttpRequest::newHttpRequest();
    req->setPath("/tech/api/v2/node/allocate");
    req->setParameter("nodeType", "workshop");
    client->sendRequest(req, [this](ReqResult result, const HttpResponsePtr &responsePtr) {
        if (result != ReqResult::Ok) {
            LOG_ERROR << "Request failed (" << static_cast<int>(result) << ")";
            return;
        }
        try {
            RequestJson response(responsePtr);
            if (responsePtr->getStatusCode() != k200OK) {
                LOG_WARN << "Request failed (" << responsePtr->getStatusCode() << "): \n"
                         << response.stringify();
                return;
            }
            auto parts = drogon::utils::splitString(response["data"].asString(), ":");
            if (parts.size() == 2) {
                workshopAddress = InetAddress(parts[0], stoi(parts[1]));
                LOG_INFO << "Retrieved workshop node: " << workshopAddress.load().toIpPort();
            } else {
                // TODO: Send an email if failed too many times.
                LOG_WARN << "No workshop node available right now!";
            }
        } catch (const json_exception::InvalidFormat &e) {
            LOG_WARN << "Invalid response body (" << responsePtr->getStatusCode() << "): \n"
                     << e.what();
        }
    }, 3);
}
