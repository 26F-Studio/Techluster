//
// Created by Particle_G on 2021/9/9.
//

#pragma once

#include <drogon/plugins/Plugin.h>
#include <unordered_set>

namespace tech::plugins {
    class Authorizer : public drogon::Plugin<Authorizer> {
    public:
        Authorizer() = default;

        void initAndStart(const Json::Value &config) override;

        void shutdown() override;

        [[nodiscard]] bool isSuperusers(int64_t uid) const;

        [[nodiscard]] std::string getCredential() const;

        [[nodiscard]] bool checkCredential(const std::string &credential) const;

    private:
        std::string _credential{};
        std::unordered_set<int64_t> _superusers{};
    };
}
