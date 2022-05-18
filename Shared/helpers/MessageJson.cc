//
// Created by ParticleG on 2022/2/9.
//

#include <helpers/MessageJson.h>
#include <magic_enum.hpp>

using namespace drogon;
using namespace magic_enum;
using namespace std;
using namespace tech::helpers;
using namespace tech::types;

MessageJson::MessageJson() : BasicJson() { setMessageType(MessageType::Client); }

MessageJson::MessageJson(Json::Value json) : BasicJson(move(json)) {}

MessageJson::MessageJson(const string &raw) : BasicJson(raw) {}

MessageJson::MessageJson(int action) : MessageJson() { setAction(action); }

void MessageJson::setMessageType(const MessageType &type) { _value["type"] = string(enum_name(type)); }

void MessageJson::setAction(int action) { _value["action"] = action; }

void MessageJson::setData(Json::Value data) { _value["data"] = move(data); }

void MessageJson::setData() { _value.removeMember("data"); }

void MessageJson::setReason(const exception &e) { setReason(e.what()); }

[[maybe_unused]] void MessageJson::setReason(const drogon::orm::DrogonDbException &e) { setReason(e.base().what()); }

void MessageJson::setReason(const string &reason) { _value["reason"] = reason; }

void MessageJson::sendTo(const WebSocketConnectionPtr &connectionPtr) const {
    if (connectionPtr->connected()) {
        connectionPtr->send(stringify());
    }
}

void MessageJson::closeWith(const WebSocketConnectionPtr &connectionPtr) const {
    if (connectionPtr->connected()) {
        connectionPtr->shutdown(CloseCode::kViolation, stringify());
    }
}


