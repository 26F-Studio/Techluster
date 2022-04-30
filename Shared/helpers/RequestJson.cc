//
// Created by ParticleG on 2022/2/9.
//

#include <helpers/RequestJson.h>

using namespace drogon;
using namespace std;
using namespace tech::helpers;
using namespace tech::structures;
using namespace tech::types;

RequestJson::RequestJson(const RequestJson &json) = default;

RequestJson::RequestJson(RequestJson &&json) noexcept { _value = std::move(json._value); }

RequestJson::RequestJson(Json::Value json) : BasicJson(std::move(json)) {}

RequestJson::RequestJson(const string &raw) : BasicJson(raw) {}

RequestJson::RequestJson(const drogon::HttpRequestPtr &req) {
    auto object = req->getJsonObject();
    if (!object) {
        throw json_exception::InvalidFormat(req->getJsonError());
    }
    _value = std::move(*object);
}

RequestJson::RequestJson(const HttpResponsePtr &res) {
    auto object = res->getJsonObject();
    if (!object) {
        throw json_exception::InvalidFormat(res->getJsonError());
    }
    _value = std::move(*object);
}

bool RequestJson::check(const string &path, JsonValue valueType) const {
    auto resultPtr = &_value;
    for (const auto &keyString: drogon::utils::splitString(path, ".")) {
        if (resultPtr->isNull()) {
            return valueType == JsonValue::Null;
        }
        resultPtr = &(*resultPtr)[keyString];
    }
    return _check(*resultPtr, valueType);
}

bool RequestJson::check(JsonValue valueType) const { return _check(_value, valueType); }

void RequestJson::require(const string &key, JsonValue valueType) const {
    if (!check(key, valueType)) {
        throw json_exception::WrongType(valueType);
    }
}

void RequestJson::require(JsonValue valueType) const {
    if (!check(valueType)) {
        throw json_exception::WrongType(valueType);
    }
}

void RequestJson::remove(const string &key) { _value.removeMember(key); }

void RequestJson::trim(const string &key, JsonValue valueType) {
    if (!check(key, valueType)) {
        remove(key);
    }
}

bool RequestJson::_check(
        const Json::Value &json,
        const JsonValue &valueType
) {
    switch (valueType) {
        case JsonValue::Null:
            return json.isNull();
        case JsonValue::Object:
            return json.isObject();
        case JsonValue::Array:
            return json.isArray();
        case JsonValue::Bool:
            return json.isBool();
        case JsonValue::Uint:
            return json.isUInt();
        case JsonValue::Uint64:
            return json.isUInt64();
        case JsonValue::Int:
            return json.isInt();
        case JsonValue::Int64:
            return json.isInt64();
        case JsonValue::Double:
            return json.isDouble();
        case JsonValue::String:
            return json.isString();
    }
    return false;
}
