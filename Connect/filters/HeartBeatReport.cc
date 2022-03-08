//
// Created by particleg on 2021/9/27.
//

#include <filters/HeartBeatReport.h>
#include <helpers/RequestJson.h>
#include <structures/Exceptions.h>

using namespace drogon;
using namespace std;
using namespace tech::filters;
using namespace tech::helpers;
using namespace tech::structures;
using namespace tech::types;

void HeartBeatReport::doFilter(
        const HttpRequestPtr &req,
        FilterCallback &&failedCb,
        FilterChainCallback &&nextCb
) {
    handleExceptions([&]() {
        auto request = RequestJson(req);
        request.require("ip", JsonValue::String);
        request.require("port", JsonValue::Uint);
        request.require("taskInterval", JsonValue::Double);
        request.require("description", JsonValue::String);
        request.require("info", JsonValue::Object);
        req->attributes()->insert("requestJson", request);
        nextCb();
    }, failedCb);
}
