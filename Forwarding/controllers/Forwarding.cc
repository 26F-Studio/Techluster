//
// Created by Parti on 2021/3/27.
//

#include <controllers/Forwarding.h>
#include <magic_enum.hpp>
#include <structures/Transmitter.h>
#include <types/Action.h>

using namespace drogon;
using namespace magic_enum;
using namespace std;
using namespace tech::helpers;
using namespace tech::plugins;
using namespace tech::structures;
using namespace tech::ws::v2;
using namespace tech::types;

Forwarding::Forwarding() :
        _connectionManager(app().getPlugin<ConnectionManager>()),
        _transmissionManager(app().getPlugin<TransmissionManager>()) {}

void Forwarding::handleNewConnection(
        const HttpRequestPtr &req,
        const WebSocketConnectionPtr &wsConnPtr
) {
    auto id = req->getAttributes()->get<int64_t>("id");
    wsConnPtr->setContext(make_shared<Transmitter>(id));
    wsConnPtr->setPingMessage("", chrono::seconds(10));
    _connectionManager->subscribe(wsConnPtr);
}

void Forwarding::handleConnectionClosed(const WebSocketConnectionPtr &wsConnPtr) {
    const auto &transmitter = wsConnPtr->getContext<Transmitter>();
    if (transmitter) {
        if (!transmitter->getRoomId().empty()) {
            try {
                _transmissionManager->transmissionLeave(
                        enum_integer(Action::transmissionLeave),
                        wsConnPtr
                );
            } catch (const internal::BaseException &e) {
                LOG_DEBUG << "Closed without leaving room: " << e.what();
            }
        }
        _connectionManager->unsubscribe(wsConnPtr);
    }
}

std::string Forwarding::reason(const string &param) const {
    return i18n(param);
}
