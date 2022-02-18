//
// Created by particleg on 2021/9/27.
//

#include <filters/EmailThreshold.h>
#include <helpers/RequestJson.h>
#include <helpers/ResponseJson.h>
#include <plugins/DataManager.h>
#include <structures/Exceptions.h>
#include <utils/http.h>

using namespace drogon;
using namespace std;
using namespace tech::filters;
using namespace tech::helpers;
using namespace tech::plugins;
using namespace tech::structures;
using namespace tech::types;
using namespace tech::utils;

EmailThreshold::EmailThreshold() : I18nHelper(CMAKE_PROJECT_NAME) {}

void tech::filters::EmailThreshold::doFilter(
        const HttpRequestPtr &req,
        FilterCallback &&failedCb,
        FilterChainCallback &&nextCb
) {
    auto requestJson = req->attributes()->get<RequestJson>("requestJson");
    try {
        if (!app().getPlugin<DataManager>()->emailLimit(requestJson["email"].asString())) {
            ResponseJson response;
            response.setResult(ResultCode::tooFrequent);
            response.setMessage(i18n("tooFrequent"));
            http::fromJson(k429TooManyRequests, response.ref(), failedCb);
            return;
        }
    } catch (const exception &e) {
        LOG_ERROR << e.what();
        ResponseJson response;
        response.setResult(ResultCode::internalError);
        response.setMessage(i18n("internalError"));
        http::fromJson(k500InternalServerError, response.ref(), failedCb);
        return;
    }
    nextCb();
}
