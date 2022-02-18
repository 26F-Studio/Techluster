//
// Created by particleg on 2021/9/27.
//

#include <filters/CheckCredential.h>
#include <helpers/ResponseJson.h>
#include <plugins/Authorizer.h>
#include <utils/http.h>

using namespace drogon;
using namespace std;
using namespace tech::filters;
using namespace tech::helpers;
using namespace tech::plugins;
using namespace tech::types;
using namespace tech::utils;

CheckCredential::CheckCredential() : I18nHelper(CMAKE_PROJECT_NAME) {}

void CheckCredential::doFilter(
        const HttpRequestPtr &req,
        FilterCallback &&failedCb,
        FilterChainCallback &&nextCb
) {
    auto credential = req->getHeader("x-credential");
    if (credential.empty() || !app().getPlugin<Authorizer>()->checkCredential(credential)) {
        ResponseJson response;
        response.setResult(ResultCode::invalidArguments);
        response.setMessage(i18n("invalidArguments"));
        http::fromJson(drogon::k400BadRequest, response.ref(), failedCb);
        return;
    }
    nextCb();
}
