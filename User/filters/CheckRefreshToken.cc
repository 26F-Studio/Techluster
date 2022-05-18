//
// Created by particleg on 2021/9/27.
//

#include <filters/CheckRefreshToken.h>
#include <helpers/ResponseJson.h>
#include <types/ResultCode.h>

using namespace drogon;
using namespace std;
using namespace tech::filters;
using namespace tech::helpers;
using namespace tech::types;

void CheckRefreshToken::doFilter(
        const HttpRequestPtr &req,
        FilterCallback &&failedCb,
        FilterChainCallback &&nextCb
) {
    auto refreshToken = req->getHeader("x-refresh-token");
    if (refreshToken.empty()) {
        ResponseJson response;
        response.setStatusCode(k400BadRequest);
        response.setResultCode(ResultCode::InvalidArguments);
        response.setMessage(i18n("invalidArguments"));
        response.httpCallback(failedCb);
        return;
    }
    req->attributes()->insert("refreshToken", refreshToken);
    nextCb();
}


