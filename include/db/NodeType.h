//
//  NodeType.hpp
//  cross-platform-game
//
//  Created by Berci on 2020. 11. 11..
//  Copyright © 2020. Berci. All rights reserved.
//

#ifndef NodeType_hpp
#define NodeType_hpp

#include <optional>

namespace S3D {

enum class NodeType {
    Sphere,
    SetOperation
};

size_t  to_integral(const NodeType& type);

std::optional<NodeType> from_integral(size_t integral);

}
#endif /* NodeType_hpp */
