//
// Created by Berci on 2020. 11. 22..
//

#ifndef STRUCTUR3D_BASE_RADIUS_H
#define STRUCTUR3D_BASE_RADIUS_H

namespace S3D {
    class RADIUS {
        float magnitude_;

    public:
        explicit RADIUS(float magnitude);
        float magnitude() const;

    };
}

#endif //STRUCTUR3D_BASE_RADIUS_H
