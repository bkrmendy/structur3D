//
// Created by Berci on 2020. 11. 27..
//

#include <cmath>
#include "utils/approximately_equal.h"

namespace S3D {
    /// https://stackoverflow.com/a/253874
    bool approximately_equal(float a, float b, float epsilon)
    {
        return fabs(a - b) <= ( (fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * epsilon);
    }
}
