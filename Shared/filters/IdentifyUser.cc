//
// Created by particleg on 2021/9/27.
//

#include <filters/IdentifyUser.h>
#include <helpers/ResponseJson.h>
#include <plugins/AuthMaintainer.h>
#include <structures/Exceptions.h>
#include <types/ResultCode.h>
#include <utils/http.h>

using namespace drogon;
using namespace std;
using namespace tech::filters;
using namespace tech::helpers;
using namespace tech::plugins;
using namespace tech::structures;
using namespace tech::types;
using namespace tech::utils;

IdentifyUser::IdentifyUser() : I18nHelper(CMAKE_PROJECT_NAME) {}

void IdentifyUser::doFilter(
        const HttpRequestPtr &req,
        FilterCallback &&failedCb,
        FilterChainCallback &&nextCb
) {
    auto accessToken = req->getHeader("x-access-token");
    if (accessToken.empty()) {
        ResponseJson response;
        response.setResult(ResultCode::invalidArguments);
        response.setMessage(i18n("invalidArguments"));
        http::fromJson(k400BadRequest, response.ref(), failedCb);
        return;
    }
    try {
        int64_t id{};
        if (app().getPlugin<AuthMaintainer>()->checkAccessToken(accessToken, id) != k200OK) {
            ResponseJson response;
            response.setResult(ResultCode::notAcceptable);
            response.setMessage(i18n("invalidAccessToken"));
            http::fromJson(k401Unauthorized, response.ref(), failedCb);
            return;
        }
        req->attributes()->insert("id", id);
    } catch (const NetworkException &e) {
        ResponseJson response;
        response.setResult(ResultCode::notAvailable);
        response.setMessage(i18n("notAvailable"));
        response.setReason(e);
        http::fromJson(k503ServiceUnavailable, response.ref(), failedCb);
        return;
    }
    nextCb();
}


