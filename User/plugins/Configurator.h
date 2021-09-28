//
// Created by Parti on 2021/2/4.
//

#pragma once

#include <drogon/plugins/Plugin.h>

namespace tech::plugins {
    class Configurator : public drogon::Plugin<Configurator> {
    public:
        Configurator() = default;

        void initAndStart(const Json::Value &config) override;

        void shutdown() override;

        [[nodiscard]] std::string getEmailUsername() const;

        [[nodiscard]] std::string getEmailPassword() const;

        [[nodiscard]] std::string getEmailAddress() const;

        [[nodiscard]] std::string getEmailName() const;

        [[nodiscard]] uint32_t getEmailPort() const;

        [[nodiscard]] std::string getEmailHost() const;

    private:
        uint32_t _emailPort{};
        std::string _emailUsername{}, _emailPassword{}, _emailAddress{}, _emailName{}, _emailHost{};
    };
}

