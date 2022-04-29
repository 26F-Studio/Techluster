//
// Created by ParticleG on 2022/2/4.
//

#pragma once

#include <drogon/drogon.h>

namespace tech::plugins {
    class WebHelper {
    public:
        enum class Color {
            none = 0,
            red = 31,
            green = 32,
            yellow = 33,
            blue = 34,
            magenta = 35,
            cyan = 36,
            white = 37,
        };

        virtual ~WebHelper() = default;

        static void colorOut(
                const std::string &msg,
                const Color &color = Color::none
        );

    protected:
        std::string _host;
        std::string _httpBase = "/tech/api/v2";
        std::string _wsBase = "/tech/ws/v2";

        Json::Value _httpRequest(
                const drogon::HttpMethod &method,
                const std::string &path,
                const std::unordered_map<std::string, std::string> &parameters = {},
                const std::unordered_map<std::string, std::string> &headers = {},
                const Json::Value &body = Json::Value(Json::nullValue)
        );

    };

    template<class T>
    class BaseTester :
            public drogon::Plugin<T>,
            public WebHelper {
    public:
        void initAndStart(const Json::Value &config) override {
            if (!config["host"].isString()) {
                LOG_FATAL << "Require 'host' in config json";
                abort();
            }
            _host = config["host"].asString();

            _typeName = typeid(T).name();
#ifdef __GNUC__
            _typeName.erase(0, _typeName.find_last_of("0123456789") + 1);
            _typeName.erase(_typeName.size() - 1);
#elif _MSC_VER
            _typeName.erase(0, _typeName.find_last_of(':') + 1);
#endif
            processConfig(config);

            LOG_INFO << _typeName << " loaded.";
        }

        void shutdown() override {
            LOG_INFO << _typeName << " shutdown.";
        }

        ~BaseTester() override = default;

    protected:
        std::string _typeName;

        virtual void processConfig(const Json::Value &config) {};
    };
}


