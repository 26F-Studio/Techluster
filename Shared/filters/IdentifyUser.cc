//
// Created by particleg on 2021/9/27.
//

#include <filters/IdentifyUser.h>
#include <helpers/ResponseJson.h>
#include <plugins/NodeMaintainer.h>
#include <structures/Exceptions.h>
#include <types/ResultCode.h>

using namespace drogon;
using namespace std;
using namespace tech::filters;
using namespace tech::helpers;
using namespace tech::plugins;
using namespace tech::structures;
using namespace tech::types;

IdentifyUser::IdentifyUser() {}

void IdentifyUser::doFilter(
        const HttpRequestPtr &req,
        FilterCallback &&failedCb,
        FilterChainCallback &&nextCb
) {
    auto accessToken = req->getHeader("x-access-token");
    if (accessToken.empty()) {
        ResponseJson response;
        response.setStatusCode(k400BadRequest);
        response.setResultCode(ResultCode::InvalidArguments);
        response.setMessage(i18n("invalidArguments"));
        response.httpCallback(failedCb);
        return;
    }
    try {
        int64_t id{};
        if (app().getPlugin<NodeMaintainer>()->checkAccessToken(accessToken, id) != k200OK) {
            ResponseJson response;
            response.setStatusCode(k401Unauthorized);
            response.setResultCode(ResultCode::NotAcceptable);
            response.setMessage(i18n("invalidAccessToken"));
            response.httpCallback(failedCb);
            return;
        }
        req->attributes()->insert("id", id);
    } catch (const NetworkException &e) {
        ResponseJson response;
        response.setStatusCode(k503ServiceUnavailable);
        response.setResultCode(ResultCode::NotAvailable);
        response.setMessage(i18n("notAvailable"));
        response.setReason(e);
        response.httpCallback(failedCb);
        return;
    }
    nextCb();
}


