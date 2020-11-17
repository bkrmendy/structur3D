//
//  NodeType.hpp
//  cross-platform-game
//
//  Created by Berci on 2020. 11. 11..
//  Copyright © 2020. Berci. All rights reserved.
//

#ifndef NodeType_hpp
#define NodeType_hpp

namespace S3D {

enum class NodeType {
    Sphere,
    SetOperation
};

int to_integral(const NodeType& type);

NodeType from_integral(int integral);

}
#endif /* NodeType_hpp */
