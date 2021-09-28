//
// Created by particleg on 2021/9/27.
//

#include <utils/datetime.h>

using namespace std;
using namespace tech::utils;
using namespace trantor;

string datetime::toString(const Date &date) {
    return date.toDbStringLocal();
}

std::string datetime::toString(const int64_t &microseconds) {
    return trantor::Date(microseconds).toDbStringLocal();
}

Date datetime::toDate(const string &date) {
    if (date.empty()) {
        return Date::now();
    }
    return Date::fromDbStringLocal(date);
}

trantor::Date datetime::toDate(const int64_t &microseconds) {
    return trantor::Date(microseconds);
}
