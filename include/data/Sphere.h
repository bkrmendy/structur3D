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

namespace S3D {
class Sphere: public Node {
public:
    const ID uid;
    Coord coord;
    RADIUS radius;
    
    Sphere() = delete;
    Sphere(const ID uid, const Coord& c, RADIUS r) : uid{uid}, coord{c}, radius{r} { }

    virtual ID id() const { return this->uid; }

    ~Sphere() { }
};
}

#endif /* Sphere_h */
