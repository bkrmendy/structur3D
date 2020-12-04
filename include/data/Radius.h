//
// Created by Berci on 2020. 11. 22..
//

#ifndef STRUCTUR3D_BASE_RADIUS_H
#define STRUCTUR3D_BASE_RADIUS_H

namespace S3D {
    class Radius {
        float magnitude_;

    public:
        explicit Radius(float magnitude);
        float& magnitude();
        float magnitude() const;
    };

    bool operator==(const Radius& left, const Radius& right);
}

#endif //STRUCTUR3D_BASE_RADIUS_H
