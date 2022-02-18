//
// Created by ParticleG on 2022/2/9.
//

#include <helpers/DataJson.h>
#include <utils/data.h>

using namespace std;
using namespace tech::helpers;
using namespace tech::utils::data;

void DataJson::canOverwrite(const bool &overwrite) { _overwrite = overwrite; }

void DataJson::canSkip(const bool &skip) { _skip = skip; }

Json::Value DataJson::retrieveByPath(const string &path) {
    auto resultPtr = &_value;
    for (const auto &keyString: drogon::utils::splitString(path, ".")) {
        if (resultPtr->isNull()) {
            return {Json::nullValue};
        }
        variant<string, uint32_t> key = keyString;
        _stoul(key);
        _keyHandler(key, [&resultPtr](const std::string &name) {
            resultPtr = &(*resultPtr)[name];
        }, [&resultPtr](const uint32_t &index) {
            resultPtr = &(*resultPtr)[index];
        });
    }
    return *resultPtr;
}

void DataJson::modifyByPath(
        const string &path,
        const Json::Value &value
) {
    if (path.empty()) {
        _value = value;
        return;
    }
    auto *tempInput = &_value;
    auto keys = drogon::utils::splitString(path, ".");
    for (uint32_t keyIndex = 0; keyIndex < keys.size(); keyIndex++) {
        variant<string, uint32_t> key = keys[keyIndex];
        _stoul(key);
        if (keyIndex == keys.size() - 1) {
            _modifyElement(tempInput, key, value);
        } else {
            _followElement(tempInput, key);
        }
    }
}

void DataJson::_stoul(variant<string, uint32_t> &key) {
    if (holds_alternative<string>(key)) {
        const auto &keyString = get<string>(key);
        if (all_of(keyString.begin(), keyString.end(), ::isdigit)) {
            key = stoul(keyString);
        }
    }
}

void DataJson::_keyHandler(
        const variant<string, uint32_t> &key,
        const function<void(const std::string &)> &objectHandler,
        const function<void(const uint32_t &)> &arrayHandler
) {
    if (holds_alternative<string>(key)) {
        objectHandler(get<string>(key));
    } else {
        arrayHandler(get<uint32_t>(key));
    }
}

void DataJson::_try_overwrite(
        Json::Value *&target,
        const Json::ValueType &type
) const {
    if (!target->isNull() && target->type() != type) {
        if (_overwrite) {
            *target = Json::Value(type);
        } else {
            throw Json::LogicError("Can't overwrite non-null element's type");
        }
    }
}

void DataJson::_try_skip(
        Json::Value *&target,
        const uint32_t &size
) const {
    if (_skip) {
        target->resize(size);
    } else {
        throw overflow_error("Can't skip array index");
    }
}

void DataJson::_modifyElement(
        Json::Value *&target,
        const variant<string, uint32_t> &key,
        const Json::Value &value
) {
    if (value.isNull()) {
        _keyHandler(key, [&target](const std::string &name) {
            target->removeMember(name, nullptr);
        }, [&target](const uint32_t &index) {
            target->removeIndex(index, nullptr);
        });
    } else {
        _keyHandler(key, [this, &target, &value](const std::string &name) {
            _try_overwrite(target, Json::objectValue);
            (*target)[name] = value;
        }, [this, &target, &value](const uint32_t &index) {
            _try_overwrite(target, Json::arrayValue);
            if (target->size() >= index) {
                (*target)[index] = value;
            } else {
                _try_skip(target, index);
                target->append(value);
            }
        });
    }
}

void DataJson::_followElement(
        Json::Value *&target,
        const variant<std::string, uint32_t> &key
) {
    _keyHandler(key, [this, &target](const std::string &name) {
        _try_overwrite(target, Json::objectValue);
        target = &(*target)[name];
    }, [this, &target](const uint32_t &index) {
        _try_overwrite(target, Json::arrayValue);
        if (target->size() < index + 1) {
            _try_skip(target, index + 1);
        }
        target = &(*target)[index];
    });
}