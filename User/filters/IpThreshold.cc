//
// Created by particleg on 2021/9/27.
//

#include <filters/IpThreshold.h>
#include <helpers/ResponseJson.h>
#include <plugins/DataManager.h>
#include <utils/http.h>

using namespace drogon;
using namespace std;
using namespace tech::filters;
using namespace tech::helpers;
using namespace tech::plugins;
using namespace tech::structures;
using namespace tech::types;
using namespace tech::utils;

IpThreshold::IpThreshold() : I18nHelper(CMAKE_PROJECT_NAME) {}

void IpThreshold::doFilter(
        const HttpRequestPtr &req,
        FilterCallback &&failedCb,
        FilterChainCallback &&nextCb
) {
    try {
        if (!app().getPlugin<DataManager>()->ipLimit(req->getPeerAddr().toIp())) {
            ResponseJson response;
            response.setResult(ResultCode::tooFrequent);
            response.setMessage(i18n("tooFrequent.ip"));
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
