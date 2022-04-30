//
// Created by ParticleG on 2022/2/3.
//

#pragma once

#include <drogon/drogon.h>

namespace tech::types {
    enum class ResultCode : uint32_t {
        // Message codes (100 ~ 199)

        // Success codes (200 ~ 299)
        completed = 200,
        continued = 201,

        // Failed codes (400 ~ 499)
        invalidFormat = 400,
        invalidArguments = 401,
        notAvailable = 402,
        noPermission = 403,
        notFound = 404,
        nullValue = 405,
        notAcceptable = 406,
        conflict = 409,
        tooFrequent = 429,

        // Error codes (500 ~ 599)
        internalError = 500,
        databaseError = 501,
        emailError = 502,
    };
}