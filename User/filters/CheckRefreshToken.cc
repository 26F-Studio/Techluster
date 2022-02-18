//
// Created by particleg on 2021/9/27.
//

#include <filters/CheckRefreshToken.h>
#include <helpers/ResponseJson.h>
#include <types/ResultCode.h>
#include <utils/http.h>

using namespace drogon;
using namespace std;
using namespace tech::filters;
using namespace tech::helpers;
using namespace tech::types;
using namespace tech::utils;

CheckRefreshToken::CheckRefreshToken() : I18nHelper(CMAKE_PROJECT_NAME) {}

void CheckRefreshToken::doFilter(
        const HttpRequestPtr &req,
        FilterCallback &&failedCb,
        FilterChainCallback &&nextCb
) {
    auto refreshToken = req->getHeader("x-refresh-token");
    if (refreshToken.empty()) {
        ResponseJson response;
        response.setResult(ResultCode::invalidArguments);
        response.setMessage(i18n("invalidArguments"));
        http::fromJson(k400BadRequest, response.ref(), failedCb);
        return;
    }
    req->attributes()->insert("refreshToken", refreshToken);
    nextCb();
}


