//
// Created by Parti on 2021/2/4.
//

#pragma once

#include <drogon/plugins/Plugin.h>
#include <plugins/BaseTester.h>

namespace tech::plugins {
    class BotTester : public BaseTester<BotTester> {
    public:
        BotTester() = default;
    };
}

