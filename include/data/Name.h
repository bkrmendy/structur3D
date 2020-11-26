//
// Created by Berci on 2020. 11. 26..
//

#ifndef STRUCTUR3D_BASE_NAME_H
#define STRUCTUR3D_BASE_NAME_H

#include <string>

namespace S3D {
    struct Name {
    private:
        std::string value_;
    public:
        static std::string allowedCharacters;

        explicit Name(const std::string& name);

        const std::string& get() const;

    };
}

#endif //STRUCTUR3D_BASE_NAME_H
