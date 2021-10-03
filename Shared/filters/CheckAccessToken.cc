//
// Created by particleg on 2021/9/27.
//

#include <filters/CheckAccessToken.h>
#include <plugins/AuthMaintainer.h>
#include <structures/Exceptions.h>
#include <utils/http.h>

using namespace drogon;
using namespace std;
using namespace tech::filters;
using namespace tech::plugins;
using namespace tech::structures;
using namespace tech::utils;

void CheckAccessToken::doFilter(
        const HttpRequestPtr &req,
        FilterCallback &&filterCallback,
        FilterChainCallback &&filterChainCallback
) {
    Json::Value response;
    auto accessToken = req->getHeader("x-access-token");
    if (accessToken.empty()) {
        response["type"] = "Error";
        response["reason"] = "Invalid x-access-token header";
        http::fromJson(k400BadRequest, response, filterCallback);
        return;
    }
    try {
        if (app().getPlugin<AuthMaintainer>()->checkAccessToken(accessToken) != k200OK) {
            response["type"] = "Failed";
            response["reason"] = "Invalid accessToken";
            http::fromJson(k401Unauthorized, response, filterCallback);
            return;
        }
    } catch (const NetworkException &e) {
        response["type"] = "Error";
        response["reason"] = "Invalid accessToken";
        http::fromJson(k401Unauthorized, response, filterCallback);
        return;
    }
    filterChainCallback();
}
