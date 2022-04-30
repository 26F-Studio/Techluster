//
// Created by particleg on 2021/9/27.
//

#include <filters/NodeCheck.h>
#include <helpers/ResponseJson.h>

using namespace drogon;
using namespace std;
using namespace tech::filters;
using namespace tech::helpers;
using namespace tech::types;
using namespace trantor;

void NodeCheck::doFilter(
        const HttpRequestPtr &req,
        FilterCallback &&failedCb,
        FilterChainCallback &&nextCb
) {
    auto parts = drogon::utils::splitString(req->getParameter("address"), ":");
    if (parts.size() == 2) {
        auto address = InetAddress(parts[0], stoi(parts[1]));
        req->attributes()->insert("address", address);
    } else {
        ResponseJson response;
        response.setStatusCode(k406NotAcceptable);
        response.setResultCode(ResultCode::notAcceptable);
        response.setMessage(i18n("invalidAddress"));
        response.httpCallback(failedCb);
    }
}
