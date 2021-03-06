//
// Created by Parti on 2021/2/4.
//

#pragma once

#include <drogon/plugins/Plugin.h>
#include <helpers/DataJson.h>
#include <plugins/BaseTester.h>

namespace tech::plugins {
    class ConnectTester : public BaseTester<ConnectTester> {
    public:
        ConnectTester() = default;

        void processConfig(const Json::Value &config) override;

        Json::Value allocate(const std::string &type);

        bool report(const std::string &type, uint32_t port);

        std::string toHost(uint32_t port);

        helpers::DataJson monitor(const std::string &type);

    private:
        Json::Value _heartbeatBody;
        std::unordered_map<std::string, std::string> _credentialHeaders;
    };
}

