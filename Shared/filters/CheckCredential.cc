//
// Created by particleg on 2021/9/27.
//

#include <filters/CheckCredential.h>
#include <helpers/ResponseJson.h>
#include <plugins/Authorizer.h>

using namespace drogon;
using namespace std;
using namespace tech::filters;
using namespace tech::helpers;
using namespace tech::plugins;
using namespace tech::types;

void CheckCredential::doFilter(
        const HttpRequestPtr &req,
        FilterCallback &&failedCb,
        FilterChainCallback &&nextCb
) {
    auto credential = req->getHeader("x-credential");
    if (credential.empty() || !app().getPlugin<Authorizer>()->checkCredential(credential)) {
        ResponseJson response;
        response.setStatusCode(k400BadRequest);
        response.setResultCode(ResultCode::InvalidArguments);
        response.setMessage(i18n("invalidArguments"));
        response.httpCallback(failedCb);
        return;
    }
    nextCb();
}
