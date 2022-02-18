//
// Created by ParticleG on 2022/2/11.
//

#pragma once

namespace tech::types {
    enum class JsonValue {
        Null,
        Object,
        Array,
        Bool,
        Uint,
        Uint64,
        Int,
        Int64,
        Double,
        String,
    };
}