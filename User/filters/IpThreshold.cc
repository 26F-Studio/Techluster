//
// Created by particleg on 2021/9/27.
//

#include <filters/IpThreshold.h>
#include <helpers/ResponseJson.h>
#include <plugins/DataManager.h>

using namespace drogon;
using namespace std;
using namespace tech::filters;
using namespace tech::helpers;
using namespace tech::plugins;
using namespace tech::structures;
using namespace tech::types;

IpThreshold::IpThreshold() : I18nHelper(CMAKE_PROJECT_NAME) {}

void IpThreshold::doFilter(
        const HttpRequestPtr &req,
        FilterCallback &&failedCb,
        FilterChainCallback &&nextCb
) {
    try {
        if (!app().getPlugin<DataManager>()->ipLimit(req->getPeerAddr().toIp())) {
            ResponseJson response;
            response.setStatusCode(k429TooManyRequests);
            response.setResultCode(ResultCode::tooFrequent);
            response.setMessage(i18n("tooFrequent"));
            response.httpCallback(failedCb);
            return;
        }
    } catch (const exception &e) {
        LOG_ERROR << e.what();
        ResponseJson response;
        response.setStatusCode(k500InternalServerError);
        response.setResultCode(ResultCode::internalError);
        response.setMessage(i18n("internalError"));
        response.httpCallback(failedCb);
        return;
    }
    nextCb();
}
