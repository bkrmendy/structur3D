//
// Created by Berci on 2020. 11. 26..
//

#include "utils/random_string.h"
#include  <random>
#include  <iterator>

namespace S3D {
    /// https://stackoverflow.com/a/16421677
    template<typename Iter, typename RandomGenerator>
    Iter select_randomly(Iter start, Iter end, RandomGenerator& g) {
        std::uniform_int_distribution<> dis(0, std::distance(start, end) - 1);
        std::advance(start, dis(g));
        return start;
    }

    template<typename Iter>
    Iter select_randomly(Iter start, Iter end) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        return select_randomly(start, end, gen);
    }

    std::string random_string(size_t length, std::string allowedChars) {
        std::string result;
        result.resize(length);

        for (size_t i = 0; i < length; i++) {
            result[i] = *select_randomly(allowedChars.begin(), allowedChars.end());
        }

        return result;
    }
}

