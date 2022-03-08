//
// Created by particleg on 2021/9/27.
//

#include <filters/CheckUserId.h>

using namespace drogon;
using namespace std;
using namespace tech::filters;
using namespace tech::helpers;

CheckUserId::CheckUserId() : I18nHelper(CMAKE_PROJECT_NAME) {}

void CheckUserId::doFilter(
        const HttpRequestPtr &req,
        FilterCallback &&failedCb,
        FilterChainCallback &&nextCb
) {
    const auto &id = req->getParameter("userId");
    req->attributes()->insert("userId", id.empty() ? -1 : stoll(id));
    nextCb();
}


