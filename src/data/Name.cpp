//
// Created by Berci on 2020. 11. 26..
//

#include <set>

#include "data/Name.h"

namespace S3D {
    /// https://stackoverflow.com/a/15736518
    std::string Name::allowedCharacters = R"( !"#$%&\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~)";

    Name::Name(const std::string& name) {
        assert(!name.empty());
        std::set<char> allowedChars{allowedCharacters.begin(), allowedCharacters.end()};
        for (const auto& chr : name) {
            assert(allowedChars.count(chr) > 0);
        }
        this->value_ = name;
    }

    const std::string &Name::get() const {
        return value_;
    }

    bool operator==(const Name& left, const Name& right) {
        return left.get() == right.get();
    }

    bool operator<(const Name& left, const Name& right) {
        return left.get() < right.get();
    }

    std::ostream& operator<<(std::ostream& os, const Name& name) {
        return os << name.get();
    }
}