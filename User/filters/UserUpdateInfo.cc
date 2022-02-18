//
// Created by particleg on 2021/9/27.
//

#include <filters/UserUpdateInfo.h>
#include <helpers/RequestJson.h>

using namespace drogon;
using namespace std;
using namespace tech::filters;
using namespace tech::helpers;
using namespace tech::types;

void UserUpdateInfo::doFilter(
        const HttpRequestPtr &req,
        FilterCallback &&failedCb,
        FilterChainCallback &&nextCb
) {
    handleExceptions([&]() {
        auto request = RequestJson(req);
        request.remove("id");
        request.remove("email");
        request.remove("avatar_hash");
        request.remove("is_new");
        request.trim("password", JsonValue::String);
        request.trim("username", JsonValue::String);
        request.trim("motto", JsonValue::String);
        request.trim("region", JsonValue::Int64);
        request.trim("avatar", JsonValue::String);
        request.trim("avatar_frame", JsonValue::Int64);
        request.trim("clan", JsonValue::String);
        req->attributes()->insert("requestJson", request);
        nextCb();
    }, failedCb);
}
