//
// Created by particleg on 2021/9/27.
//

#include <filters/EmailCoolDown.h>
#include <plugins/DataManager.h>
#include <structures/Exceptions.h>
#include <utils/http.h>

using namespace drogon;
using namespace std;
using namespace tech::filters;
using namespace tech::plugins;
using namespace tech::structures;
using namespace tech::utils;

void tech::filters::EmailCoolDown::doFilter(
        const HttpRequestPtr &req,
        FilterCallback &&filterCallback,
        FilterChainCallback &&filterChainCallback
) {
    Json::Value request, response;
    string parseError = http::toJson(req, request);
    if (!parseError.empty()) {
        response["type"] = "Error";
        response["reason"] = "Invalid JSON: " + parseError;
        http::fromJson(k400BadRequest, response, filterCallback);
        return;
    }
    if (!(
            request.isMember("email") && request["email"].isString()
    )) {
        response["type"] = "Error";
        response["reason"] = "Invalid parameters";
        http::fromJson(k400BadRequest, response, filterCallback);
        return;
    }
    auto dataManager = app().getPlugin<DataManager>();
    try {
        if (!dataManager->emailLimit(request["email"].asString())) {
            response["type"] = "Failed";
            response["reason"] = "Too many requests";
            http::fromJson(k429TooManyRequests, response, filterCallback);
            return;
        }
    } catch (const internal::BaseException &e) {
        response["type"] = "Error";
        response["reason"] = e.what();
        http::fromJson(k429TooManyRequests, response, filterCallback);
        return;
    }
    req->attributes()->insert("data", request);
    filterChainCallback();
}
