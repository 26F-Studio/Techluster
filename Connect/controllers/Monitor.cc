//
// Created by Particle_G on 2021/8/19.
//

#include <controllers/Monitor.h>
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

Monitor::Monitor() :
        I18nHelper(CMAKE_PROJECT_NAME),
        _nodeManager(app().getPlugin<NodeManager>()),
        _perfmon(app().getPlugin<Perfmon>()) {}

void Monitor::getInfo(const HttpRequestPtr &req, function<void(const HttpResponsePtr &)> &&callback) {
    ResponseJson response;
    auto nodeType = req->attributes()->get<NodeType>("nodeType");
    if (nodeType == NodeType::connect) {
        response.setData(_perfmon->parseInfo());
    } else if (nodeType == NodeType::all) {
        response.setData(_nodeManager->parseInfo());
    } else {
        try {
            response.setData(_nodeManager->parseInfo(nodeType));
        } catch (exception &e) {
            // TODO: Move this into NodeManager
            response.setStatusCode(k406NotAcceptable);
            response.setResultCode(ResultCode::invalidArguments);
            response.setMessage(i18n("notAvailable."s.append(enum_name(nodeType))));
            response.setReason(e);
        }
    }
    response.httpCallback(callback, "*");
}
