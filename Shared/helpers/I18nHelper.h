//
// Created by ParticleG on 2022/2/4.
//

#pragma once

#include <drogon/drogon.h>

namespace tech::helpers {
    template<class T>
    class I18nHelper : trantor::NonCopyable {
    public:
        I18nHelper() {
            using namespace std;
            _basename = move(T::projectName + "."s);
            string typeName(typeid(T).name());
#ifdef __GNUC__
            typeName.erase(0, typeName.find_last_of("0123456789") + 1);
            typeName.erase(typeName.size() - 1);
#elif _MSC_VER
            typeName.erase(0, typeName.find_last_of(':') + 1);
#endif
            _basename.append(typeName).append(".");
        }

        [[nodiscard]] std::string i18n(const std::string &key) const {
            return _basename + key;
        }

        virtual ~I18nHelper() = default;

    private:
        std::string _basename;
    };
}

