//
// Created by ParticleG on 2022/2/9.
//

#pragma once

#include <helpers/BasicJson.h>
#include <structures/Exceptions.h>
#include <utils/data.h>
#include <variant>

namespace tech::helpers {
    class RequestJson : public BasicJson {
    public:


        RequestJson() = default;

        RequestJson(const RequestJson &json);

        RequestJson(RequestJson &&json) noexcept;

        explicit RequestJson(Json::Value json);

        explicit RequestJson(const std::string &raw);

        explicit RequestJson(const drogon::HttpRequestPtr &req);

        [[nodiscard]] bool check(
                const std::string &path,
                const types::JsonValue &valueType
        ) const;

        void remove(const std::string &key);

        void trim(
                const std::string &key,
                const types::JsonValue &valueType
        );

        void require(
                const std::string &key,
                const types::JsonValue &valueType
        ) const;

        template<internal::JsonTypes T>
        [[nodiscard]] bool check(
                const std::string &key
        ) const {
            return _value[key].is<T>();
        }

        template<internal::JsonTypes T>
        [[nodiscard]] bool equal(
                const std::string &key,
                const T &value
        ) const {
            if (!check<T>(key)) {
                return false;
            }
            return _value[key].as<T>() == value;
        }

        template<internal::JsonTypes T>
        [[nodiscard]] bool notEqual(
                const std::string &key,
                const T &value
        ) const {
            if (!check<T>(key)) {
                return false;
            }
            return _value[key].as<T>() != value;
        }

        const Json::Value &operator[](Json::ArrayIndex index) const;

        const Json::Value &operator[](int index) const;

        const Json::Value &operator[](const char *key) const;

        const Json::Value &operator[](const Json::String &key) const;

    private:
        static bool _check(
                const Json::Value &json,
                const types::JsonValue &valueType
        );
    };
}



