//
// Created by Particle_G on 2021/8/19.
//

#pragma once

#include <drogon/HttpController.h>
#include <plugins/NodeManager.h>

namespace tech::api::v2 {
    class Allocator :
            public drogon::HttpController<Allocator>,
            public helpers::I18nHelper<Allocator> {
    public:
        Allocator();

        METHOD_LIST_BEGIN
            METHOD_ADD(Allocator::allocate, "", drogon::Get, "tech::filters::CheckNodeType");
        METHOD_LIST_END

        void allocate(
                const drogon::HttpRequestPtr &req,
                std::function<void(const drogon::HttpResponsePtr &)> &&callback
        );

    private:
        plugins::NodeManager *_nodeManager;
    };
}

