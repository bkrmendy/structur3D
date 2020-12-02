//
// Created by Berci on 2020. 11. 26..
//

#ifndef STRUCTUR3D_BASE_NAME_H
#define STRUCTUR3D_BASE_NAME_H

#include <string>
#include <iostream>

namespace S3D {
    class Name {
        std::string value_;
    public:
        static std::string allowedCharacters;

        Name() = default;
        explicit Name(const std::string& name);

        const std::string& get() const;
        std::string get();
    };

    bool operator==(const Name& left, const Name& right);
    bool operator<(const Name& left, const Name& right);
    std::ostream& operator<<(std::ostream& os, const Name& name);
}

#endif //STRUCTUR3D_BASE_NAME_H
