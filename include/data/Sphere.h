//
//  Sphere.h
//  cross-platform-game
//
//  Created by Berci on 2020. 11. 04..
//  Copyright © 2020. Berci. All rights reserved.
//

#ifndef Sphere_h
#define Sphere_h

#include "./Base.h"
#include "./Node.h"
#include "./Coord.h"
#include "Radius.h"

namespace S3D {
struct Sphere: public Node {
    const ID uid;
    Coord coord;
    Radius radius;
    
    Sphere() = delete;
    Sphere(const ID uid, const Coord& c, Radius r) : uid{uid}, coord{c}, radius{r} { }

    virtual ID id() const { return this->uid; }

    ~Sphere() = default;
};
}

#endif /* Sphere_h */
