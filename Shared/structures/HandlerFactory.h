//
// Created by Parti on 2021/2/19.
//

#pragma once

#include <memory>
#include <unordered_map>
#include <shared_mutex>
#include <structures/Exceptions.h>

namespace tech::structures {
    template<class baseHandler>
    class HandlerFactory final {
    public:
        HandlerFactory() = default;

        ~HandlerFactory() = default;

        HandlerFactory(const HandlerFactory &) = delete;

        const HandlerFactory &operator=(const HandlerFactory &) = delete;

        template<class Handler>
        void registerHandler(int action) {
            std::unique_lock<std::shared_mutex> lock(_sharedMutex);
            _handlerRegistrarsMap[action] = std::make_unique<Handler>();
        }

        baseHandler &getHandler(int action) {
            std::shared_lock<std::shared_mutex> lock(_sharedMutex);
            auto iter = _handlerRegistrarsMap.find(action);
            if (iter != _handlerRegistrarsMap.end()) {
                return *iter->second;
            }
            throw action_exception::ActionNotFound("Invalid action");
        }

    private:
        mutable std::shared_mutex _sharedMutex;
        std::unordered_map<int, std::unique_ptr<baseHandler>> _handlerRegistrarsMap;
    };
}