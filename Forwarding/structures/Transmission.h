//
// Created by particleg on 2021/10/8.
//

#pragma once

#include <drogon/drogon.h>
#include <helpers/DataJson.h>
#include <helpers/I18nHelper.h>
#include <helpers/MessageJson.h>
#include <helpers/RequestJson.h>
#include <plugins/ConnectionManager.h>

namespace tech::structures {
    /**
     * @brief Gaming room to store players and game data
     *
     * @param roomId: string
     * @param playerSet: unordered_set(id)
     * @param _transferSet: unordered_set(id)
     */
    class Transmission : helpers::I18nHelper<Transmission> {
    public:
        static constexpr char projectName[] = CMAKE_PROJECT_NAME;

    public:
        Transmission(
                std::string &&roomId,
                std::set<int64_t> &&playerSet
        );

        Transmission(Transmission &&transmission) noexcept;

        void join(int64_t userId);

        void leave(int64_t userId);

        void publish(const helpers::MessageJson &message, int64_t excludedId = -1) const;

        void history(int64_t userId, std::string &&message);

        Json::Value parse() const;

        ~Transmission() override;

    public:
        const std::string roomId;
        const std::set<int64_t> playerSet;

    private:
        plugins::ConnectionManager *_connectionManager;
        mutable std::shared_mutex _transmitterMutex, _historyMutex;
        std::set<int64_t> _transmitterSet;
        std::map<int64_t, std::string> _history;
    };
}
