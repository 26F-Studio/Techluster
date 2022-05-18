//
// Created by ParticleG on 2022/2/3.
//

#pragma once

#include <drogon/drogon.h>

namespace tech::types {
    enum class ResultCode : uint32_t {
        // Message codes (100 ~ 199)

        // Success codes (200 ~ 299)
        Completed = 200,
        Continued = 201,

        // Failed codes (400 ~ 499)
        InvalidFormat = 400,
        InvalidArguments = 401,
        NotAvailable = 402,
        NoPermission = 403,
        NotFound = 404,
        NullValue = 405,
        NotAcceptable = 406,
        Conflict = 409,
        TooFrequent = 429,

        // Error codes (500 ~ 599)
        InternalError = 500,
        DatabaseError = 501,
        EmailError = 502,
    };
}