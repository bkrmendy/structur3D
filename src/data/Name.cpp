//
// Created by Berci on 2020. 11. 26..
//

#include <set>

#include "data/Name.h"

namespace S3D {
    std::string Name::allowedCharacters = "1234567890qwertyuiopasdfghjklzxcvbnm /.,'\"";

    Name::Name(const std::string& name) {
        std::set<char> allowedChars{allowedCharacters.begin(), allowedCharacters.end()};
        for (const auto& chr : name) {
            assert(allowedChars.count(chr) > 0);
        }
        this->value_ = name;
    }

    const std::string &Name::get() const {
        return value_;
    }
}