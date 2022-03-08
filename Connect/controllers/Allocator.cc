//
// Created by Particle_G on 2021/8/19.
//

#include <controllers/Allocator.h>
#include <helpers/ResponseJson.h>
#include <magic_enum.hpp>

using namespace drogon;
using namespace magic_enum;
using namespace std;
using namespace tech::api::v2;
using namespace tech::helpers;
using namespace tech::plugins;
using namespace tech::structures;
using namespace tech::types;

Allocator::Allocator() :
        I18nHelper(CMAKE_PROJECT_NAME),
        _nodeManager(app().getPlugin<NodeManager>()) {}

void Allocator::allocate(
        const HttpRequestPtr &req,
        function<void(const HttpResponsePtr &)> &&callback
) {
    auto nodeType = req->attributes()->get<NodeType>("nodeType");
    ResponseJson response;
    try {
        if (nodeType == NodeType::transfer) {
            response.setData(_nodeManager->getAllNodes(nodeType));
        } else {
            response.setData(_nodeManager->getBestNode(nodeType));
        }
    } catch (exception &e) {
        // TODO: Move this into NodeManager
        response.setResultCode(ResultCode::notAvailable);
        response.setStatusCode(k503ServiceUnavailable);
        response.setMessage(i18n("notAvailable."s.append(enum_name(nodeType))));
    }
    response.httpCallback(callback);
}