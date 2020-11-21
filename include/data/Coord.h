//
//  Coord.h
//  cross-platform-game
//
//  Created by Berci on 2020. 11. 11..
//  Copyright © 2020. Berci. All rights reserved.
//

#ifndef Coord_h
#define Coord_h

namespace S3D {
    struct Coord {
        float x;
        float y;
        float z;

        Coord() : x{0}, y{0}, z{0} {};

        Coord(float x, float y, float z) : x{x}, y{y}, z{z} {}
    };
}


#endif /* Coord_h */
