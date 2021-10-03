//
// Created by particleg on 2021/9/27.
//

#include <filters/IpCoolDown.h>
#include <plugins/DataManager.h>
#include <structures/Exceptions.h>
#include <utils/http.h>

using namespace drogon;
using namespace std;
using namespace tech::filters;
using namespace tech::plugins;
using namespace tech::structures;
using namespace tech::utils;

void IpCoolDown::doFilter(
        const HttpRequestPtr &req,
        FilterCallback &&filterCallback,
        FilterChainCallback &&filterChainCallback
) {
    Json::Value response;
    auto dataManager = app().getPlugin<DataManager>();
    try {
        if (!dataManager->ipLimit(req->getPeerAddr().toIp())) {
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
    filterChainCallback();
}
