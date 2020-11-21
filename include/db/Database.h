//
//  database.h
//  cross-platform-game
//
//  Created by Berci on 2020. 11. 04..
//  Copyright © 2020. Berci. All rights reserved.
//

#ifndef database_h
#define database_h

#include <memory>
#include <vector>
#include <optional>
#include <data/Timestamp.h>

#include "./IDWithType.h"
#include "./DocumentWithName.h"

#include "../data/Base.h"
#include "../data/Sphere.h"
#include "../data/SetOp.h"
#include "../data/Coord.h"
#include "../data/SetOperationType.h"

namespace S3D {

struct Database {
    // Manipulation
    virtual void upsert(const ID&, const RADIUS&, Timestamp timestamp) = 0;
    virtual void retract(const ID&, const RADIUS&, Timestamp timestamp) = 0;

    virtual void upsert(const ID&, const Coord&, Timestamp timestamp) = 0;
    virtual void retract(const ID&, const Coord&, Timestamp timestamp) = 0;

    virtual void upsert(const ID&, const SetOperationType&, Timestamp timestamp) = 0;
    virtual void retract(const ID&, const SetOperationType&, Timestamp timestamp) = 0;

    virtual void upsert(const ID&, const std::string& name, Timestamp timestamp) = 0;
    virtual void retract(const ID&, const std::string& name, Timestamp timestamp) = 0;

    virtual void connect(const ID&, const ID&, Timestamp timestamp) = 0;
    virtual void disconnect(const ID&, const ID&, Timestamp timestamp) = 0;

    virtual void create(const ID& entity, const NodeType& type, const ID& document, Timestamp timestamp) = 0;
    virtual void remove(const ID& entity, const ID& document, Timestamp timestamp) = 0;

    // Querying
    virtual std::vector<DocumentWithName> documents() = 0;
    virtual std::vector<IDWithType> entities(const ID&) = 0;

    virtual std::optional<Sphere> sphere(const ID& from) = 0;
    virtual std::optional<SetOp> setop(const ID& from) = 0;

    virtual std::vector<ID> edges(const ID&) = 0;

    virtual ~Database(){};
};
}

#endif /* database_h */
