//
// Created by particleg on 2021/9/27.
//

#include <filters/VerifyGamingNode.h>
#include <helpers/ResponseJson.h>
#include <plugins/NodeMaintainer.h>
#include <structures/Exceptions.h>
#include <types/NodeType.h>
#include <types/ResultCode.h>

using namespace drogon;
using namespace std;
using namespace tech::filters;
using namespace tech::helpers;
using namespace tech::plugins;
using namespace tech::structures;
using namespace tech::types;

void VerifyGamingNode::doFilter(
        const HttpRequestPtr &req,
        FilterCallback &&failedCb,
        FilterChainCallback &&nextCb
) {
    auto address = req->peerAddr().toIpPort();
    try {
        if (!app().getPlugin<NodeMaintainer>()->checkNode(NodeType::Gaming, address)) {
            ResponseJson response;
            response.setStatusCode(k401Unauthorized);
            response.setResultCode(ResultCode::NotAcceptable);
            response.setMessage(i18n("invalidNode"));
            response.httpCallback(failedCb);
            return;
        }
    } catch (const NetworkException &e) {
        ResponseJson response;
        response.setStatusCode(k503ServiceUnavailable);
        response.setResultCode(ResultCode::NotAvailable);
        response.setMessage(i18n("notAvailable"));
        response.setReason(e);
        response.httpCallback(failedCb);
        return;
    }
    nextCb();
}


