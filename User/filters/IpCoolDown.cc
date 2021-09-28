//
// Created by particleg on 2021/9/27.
//

#include <filters/IpCoolDown.h>
#include <plugins/DataManager.h>
#include <utils/http.h>

using namespace drogon;
using namespace std;
using namespace tech::filters;
using namespace tech::plugins;
using namespace tech::utils;

void IpCoolDown::doFilter(
        const HttpRequestPtr &req,
        FilterCallback &&filterCallback,
        FilterChainCallback &&filterChainCallback
) {
    auto dataManager = app().getPlugin<DataManager>();
    if (!dataManager->ipLimit(req->getPeerAddr().toIp())) {
        Json::Value response;
        response["type"] = "Failed";
        response["reason"] = "Too many requests";
        http::fromJson(k429TooManyRequests, response, filterCallback);
        return;
    }
    filterChainCallback();
}
