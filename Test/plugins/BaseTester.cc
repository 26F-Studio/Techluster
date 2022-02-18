//
// Created by ParticleG on 2022/2/4.
//

#include <drogon/drogon.h>
#include <helpers/BasicJson.h>
#include <plugins/BaseTester.h>
#include <structures/Exceptions.h>
#include <utils/data.h>
#include <utils/http.h>

using namespace drogon;
using namespace std;
using namespace tech::helpers;
using namespace tech::plugins;
using namespace tech::structures;
using namespace tech::utils;
using namespace tech::utils::data;

void WebHelper::colorOut(const std::string &msg, const Color &color) {
    cout << "\x1b[" << +color << "m"
         << string(16, '-')
         << msg
         << string(48 - msg.length(), '-')
         << "\x1b[" << +Color::none << "m"
         << endl;
}

Json::Value WebHelper::_httpRequest(
        const HttpMethod &method,
        const string &path,
        const unordered_map<string, string> &parameters,
        const unordered_map<string, string> &headers,
        const Json::Value &body
) {
    auto client = HttpClient::newHttpClient("http://"s.append(_host));
    auto req = HttpRequest::newHttpRequest();
    if (!body.isNull()) {
        req = toRequest(body);
    }
    req->setMethod(method);
    req->setPath(_httpBase + path);
    if (!parameters.empty()) {
        for (const auto &[key, value]: parameters) {
            req->setParameter(key, value);
        }
    }
    if (!headers.empty()) {
        for (const auto &[key, value]: headers) {
            req->addHeader(key, value);
        }
    }
    auto[result, responsePtr] = client->sendRequest(req, 180);
    if (result != ReqResult::Ok) {
        throw NetworkException("Node is down", result);
    }
    Json::Value response;
    string parseError = http::toJson(responsePtr, response);
    if (!parseError.empty()) {
        throw NetworkException("Invalid Json: "s.append(parseError), ReqResult::BadResponse);
    }
    if (!response["code"].isUInt()) {
        throw NetworkException("Request failed: "s.append(BasicJson(response).stringify()), ReqResult::BadResponse);
    }
    LOG_DEBUG << "Response Json: " << BasicJson(response).stringify("  ");
    return response;
}
