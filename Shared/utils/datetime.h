//
// Created by particleg on 2021/9/27.
//

#pragma once

#include <string>
#include <sstream>
#include <trantor/utils/Date.h>

namespace tech::utils::datetime {
    std::string toString(const trantor::Date &date = trantor::Date::date());

    std::string toString(const int64_t &microseconds);

    trantor::Date toDate(const std::string &date = std::string());

    trantor::Date toDate(const int64_t &microseconds);
}
