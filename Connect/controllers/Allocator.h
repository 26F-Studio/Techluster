//
// Created by Particle_G on 2021/8/19.
//

#pragma once

#include <drogon/HttpController.h>
#include <services/Allocator.h>

namespace tech::api::v2 {
    class Allocator : public drogon::HttpController<Allocator> {
    public:
        METHOD_LIST_BEGIN
            METHOD_ADD(Allocator::message, "/message", drogon::Get);
            METHOD_ADD(Allocator::gaming, "/gaming", drogon::Get);
            METHOD_ADD(Allocator::transfers, "/transfer", drogon::Get);
            METHOD_ADD(Allocator::transfer, "/transfer", drogon::Post);
            METHOD_ADD(Allocator::user, "/user", drogon::Get);
        METHOD_LIST_END

        void message(const drogon::HttpRequestPtr &req, std::function<void(const drogon::HttpResponsePtr &)> &&callback);

        void gaming(const drogon::HttpRequestPtr &req, std::function<void(const drogon::HttpResponsePtr &)> &&callback);

        void transfers(const drogon::HttpRequestPtr &req, std::function<void(const drogon::HttpResponsePtr &)> &&callback);

        void transfer(const drogon::HttpRequestPtr &req, std::function<void(const drogon::HttpResponsePtr &)> &&callback);

        void user(const drogon::HttpRequestPtr &req, std::function<void(const drogon::HttpResponsePtr &)> &&callback);

    private:
        services::Allocator _service;
    };
}

