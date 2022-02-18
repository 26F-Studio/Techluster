//
// Created by Particle_G on 2021/8/19.
//

#include <controllers/Allocator.h>
#include <helpers/ResponseJson.h>
#include <magic_enum.hpp>
#include <utils/http.h>

using namespace drogon;
using namespace magic_enum;
using namespace std;
using namespace tech::api::v2;
using namespace tech::helpers;
using namespace tech::plugins;
using namespace tech::structures;
using namespace tech::types;
using namespace tech::utils;

Allocator::Allocator() :
        I18nHelper(CMAKE_PROJECT_NAME),
        _nodeManager(app().getPlugin<NodeManager>()) {}

void Allocator::allocate(
        const HttpRequestPtr &req,
        function<void(const HttpResponsePtr &)> &&callback
) {
    auto nodeType = req->attributes()->get<NodeType>("nodeType");

    HttpStatusCode code = HttpStatusCode::k200OK;
    ResponseJson response;
    try {
        if (nodeType == NodeType::transfer) {
            response.setData(_nodeManager->getAllNodes(nodeType));
        } else {
            response.setData(_nodeManager->getBestNode(nodeType));
        }
    } catch (exception &e) {
        code = HttpStatusCode::k503ServiceUnavailable;
        response.setResult(ResultCode::notAvailable);
        response.setMessage(i18n("notAvailable."s.append(enum_name(nodeType))));
    }
    http::fromJson(code, response.ref(), callback);
}