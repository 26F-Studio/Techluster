//
// Created by ParticleG on 2022/2/9.
//

#pragma once

#include <helpers/BasicJson.h>
#include <shared_mutex>
#include <variant>

namespace tech::helpers {
    class DataJson : public BasicJson {
    public:
        DataJson() : BasicJson() {}

        explicit DataJson(Json::Value json) : BasicJson(std::move(json)) {}

        explicit DataJson(const std::string &raw) : BasicJson(raw) {}

        // Retrieve the value of the json object
        Json::Value retrieveByPath(const std::string &path);

        // Modify the value of the json object
        void canOverwrite(const bool &overwrite);

        void canSkip(const bool &skip);

        void modifyByPath(
                const std::string &path,
                const Json::Value &value
        );

    private:
        mutable std::shared_mutex _sharedMutex;
        bool _overwrite{}, _skip{};

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
