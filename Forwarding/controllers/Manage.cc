//
// Created by Particle_G on 2021/8/19.
//

#include <controllers/Manage.h>
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

Manage::Manage() :
        ResponseJsonHandler(
                [](const ResponseException &e, ResponseJson &response) {
                    response.setStatusCode(e.statusCode());
                    // TODO: Check if this causes too much copying
                    response(e.toJson());
                },
                [this](const orm::DrogonDbException &e, ResponseJson &response) {
                    LOG_ERROR << e.base().what();
                    response.setStatusCode(k500InternalServerError);
                    response.setResultCode(ResultCode::DatabaseError);
                    response.setMessage(i18n("databaseError"));
                },
                [this](const exception &e, ResponseJson &response) {
                    LOG_ERROR << e.what();
                    response.setStatusCode(k500InternalServerError);
                    response.setResultCode(ResultCode::InternalError);
                    response.setMessage(i18n("internalError"));
                    response.setReason(e);
                }
        ),
        _transferManager(app().getPlugin<TransmissionManager>()) {}

void Manage::create(const HttpRequestPtr &req, function<void(const HttpResponsePtr &)> &&callback) {
    ResponseJson response;
    handleExceptions([&]() {
        auto request = req->attributes()->get<RequestJson>("requestJson");
        set<int64_t> playerSet;
        for (const auto &player: request["players"]) {
            if (player.isInt64()) {
                playerSet.insert(player.asInt64());
            }
        }
        _transferManager->create(
                move(request["roomId"].asString()),
                move(playerSet)
        );
    }, response);
    response.httpCallback(callback);
}

void Manage::remove(const HttpRequestPtr &req, function<void(const HttpResponsePtr &)> &&callback) {
    ResponseJson response;
    handleExceptions([&]() {
        auto request = req->attributes()->get<RequestJson>("requestJson");
        _transferManager->remove(request["roomId"].asString());
    }, response);
    response.httpCallback(callback);
}