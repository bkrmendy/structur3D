//
//  SetOp.h
//  cross-platform-game
//
//  Created by Berci on 2020. 11. 04..
//  Copyright © 2020. Berci. All rights reserved.
//

#ifndef SetOp_h
#define SetOp_h

#include <memory>
#include <vector>

#include "./Base.h"
#include "./Node.h"
#include "./SetOperationType.h"

namespace S3D {
    class SetOp : public Node {
    public:
        const ID uid;
        const SetOperationType type;

        SetOp() = delete;
        SetOp(const ID uid, const SetOperationType type) : uid{uid}, type{type} {}

        ID id() const final { return this->uid; }

        ~SetOp() { }
    };
}

#endif /* SetOp_h */
