//
// Created by Particle_G on 2021/8/19.
//

#pragma once

#include <drogon/drogon.h>
#include <shared_mutex>

namespace tech::structures {
    class NodeServer {
    public:
        enum class Type {
            message,
            gaming,
            transfer,
            user
        };

        NodeServer(
                std::string host,
                Type type,
                std::string description
        );

        NodeServer(NodeServer &&nodeServer) noexcept;

        std::string getHost() const;

        std::string getDescription() const;

        Type getType() const;

        Json::Value getInfo() const;

        void setInfo(Json::Value info);

        uint64_t getFailureCount() const;

        void incrementFailureCount();

        void clearFailureCount();

    private:
        mutable std::shared_mutex _sharedMutex;
        const std::string _host, _description;
        const Type _type;
        Json::Value _info;
        std::atomic<uint64_t> _failureCount{};
    };
}
