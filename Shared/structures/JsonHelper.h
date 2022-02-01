//
// Created by ParticleG on 2022/2/1.
//

#pragma once

#include <functional>
#include <json/json.h>
#include <variant>

namespace tech::structures {
    class JsonHelper {
    public:
        explicit JsonHelper(Json::Value json);

        explicit JsonHelper(Json::Value &&json);

        explicit JsonHelper(const std::string &raw);

        std::string stringify(const std::string &indentation = "");

        Json::Value &value();

        void canOverwrite(const bool &overwrite);

        void canSkip(const bool &skip);

        Json::Value retrieveByPath(const std::string &path);

        void modifyByPath(
                const std::string &path,
                const Json::Value &value
        );

    private:
        Json::Value _value;
        bool _overwrite{}, _skip{};

        static void _stoul(std::variant<std::string, uint32_t> &key);

        static void _keyHandler(
                const std::variant<std::string, uint32_t> &key,
                const std::function<void(const std::string &)> &objectHandler,
                const std::function<void(const uint32_t &)> &arrayHandler
        );

        void _try_overwrite(
                Json::Value *&target,
                const Json::ValueType &type
        ) const;

        void _try_skip(
                Json::Value *&target,
                const uint32_t &size
        ) const;

        void _modifyElement(
                Json::Value *&target,
                const std::variant<std::string, uint32_t> &key,
                const Json::Value &value
        );

        void _followElement(
                Json::Value *&target,
                const std::variant<std::string, uint32_t> &key
        );
    };
}
