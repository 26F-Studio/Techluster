//
// Created by particleg on 2021/9/27.
//

#include <filters/EmailThreshold.h>
#include <helpers/RequestJson.h>
#include <helpers/ResponseJson.h>
#include <plugins/PlayerManager.h>
#include <structures/Exceptions.h>

using namespace drogon;
using namespace std;
using namespace tech::filters;
using namespace tech::helpers;
using namespace tech::plugins;
using namespace tech::structures;
using namespace tech::types;

void tech::filters::EmailThreshold::doFilter(
        const HttpRequestPtr &req,
        FilterCallback &&failedCb,
        FilterChainCallback &&nextCb
) {
    auto requestJson = req->attributes()->get<RequestJson>("requestJson");
    try {
        if (!app().getPlugin<PlayerManager>()->emailLimit(requestJson["email"].asString())) {
            ResponseJson response;
            response.setStatusCode(k429TooManyRequests);
            response.setResultCode(ResultCode::TooFrequent);
            response.setMessage(i18n("tooFrequent"));
            response.httpCallback(failedCb);
            return;
        }
    } catch (const exception &e) {
        LOG_ERROR << e.what();
        ResponseJson response;
        response.setStatusCode(k500InternalServerError);
        response.setResultCode(ResultCode::InternalError);
        response.setMessage(i18n("internalError"));
        response.httpCallback(failedCb);
        return;
    }
    nextCb();
}
