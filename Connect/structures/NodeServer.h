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
            gaming,
            message,
            transfer,
            user
        };

        NodeServer(
                const std::string &ip,
                const uint16_t &port,
                const Type &type,
                const double &taskInterval,
                std::string description
        );

        NodeServer(NodeServer &&nodeServer) noexcept;

        std::string getIpPort() const;

        uint64_t getNetEndian() const;

        Type getType() const;

        double getInterval() const;

        std::string getDescription() const;

        Json::Value getInfo() const;

        void setInfo(Json::Value info);

        uint64_t getTimerId() const;

        void setTimerId(const uint64_t &timerId);

        int64_t getLastSeen() const;

        void updateLastSeen();

        Json::Value parseNode() const;

    private:
        mutable std::shared_mutex _sharedMutex;

        const trantor::InetAddress _inetAddress;
        const Type _type;
        const double _taskInterval;
        const std::string _description;
        std::atomic<uint64_t> _deactivateTimerId;
        trantor::Date _lastSeen;
        Json::Value _info;
    };
}
