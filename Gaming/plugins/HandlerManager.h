//
// Created by Parti on 2021/2/19.
//

#pragma once

#include <drogon/plugins/Plugin.h>
#include <structures/HandlerManagerBase.h>
#include <utils/websocket.h>

namespace tech::plugins {
    class HandlerManager : public structures::HandlerManagerBase<HandlerManager> {
    public:
        void initAndStart(const Json::Value &config) override;

        void shutdown() override;
    };
}