//
// Created by particleg on 2021/9/27.
//

#include <filters/IdentifyUser.h>
#include <helpers/ResponseJson.h>
#include <plugins/AuthMaintainer.h>
#include <structures/Exceptions.h>
#include <types/ResultCode.h>

using namespace drogon;
using namespace std;
using namespace tech::filters;
using namespace tech::helpers;
using namespace tech::plugins;
using namespace tech::structures;
using namespace tech::types;

IdentifyUser::IdentifyUser() : I18nHelper(CMAKE_PROJECT_NAME) {}

void IdentifyUser::doFilter(
        const HttpRequestPtr &req,
        FilterCallback &&failedCb,
        FilterChainCallback &&nextCb
) {
    auto accessToken = req->getHeader("x-access-token");
    if (accessToken.empty()) {
        ResponseJson response;
        response.setStatusCode(k400BadRequest);
        response.setResultCode(ResultCode::invalidArguments);
        response.setMessage(i18n("invalidArguments"));
        response.httpCallback(failedCb);
        return;
    }
    try {
        int64_t id{};
        if (app().getPlugin<AuthMaintainer>()->checkAccessToken(accessToken, id) != k200OK) {
            ResponseJson response;
            response.setStatusCode(k401Unauthorized);
            response.setResultCode(ResultCode::notAcceptable);
            response.setMessage(i18n("invalidAccessToken"));
            response.httpCallback(failedCb);
            return;
        }
        req->attributes()->insert("id", id);
    } catch (const NetworkException &e) {
        ResponseJson response;
        response.setStatusCode(k503ServiceUnavailable);
        response.setResultCode(ResultCode::notAvailable);
        response.setMessage(i18n("notAvailable"));
        response.setReason(e);
        response.httpCallback(failedCb);
        return;
    }
    nextCb();
}


