//
//  Document.h
//  cross-platform-game
//
//  Created by Berci on 2020. 11. 14..
//  Copyright © 2020. Berci. All rights reserved.
//

#ifndef Document_h
#define Document_h

#include <memory>
#include <vector>

#include "data/Edge.h"
#include "data/Node.h"
#include "data/Tree.h"
#include "data/Graph.h"
#include "mesh/Mesh.h"

namespace S3D {

struct Document {
    virtual void create(std::shared_ptr<Node> node) = 0;
    virtual void create(std::shared_ptr<Edge> edge) = 0;

    virtual void update(std::shared_ptr<Node> node) = 0;

    virtual void remove(std::shared_ptr<Node> node) = 0;
    virtual void remove(std::shared_ptr<Edge> edge) = 0;

    virtual const std::unique_ptr<Graph>& graph() const = 0;
    virtual const std::unique_ptr<Mesh>& mesh() const = 0;
    virtual const ID id() const = 0;

    virtual ~Document() = default;
};

}

#endif /* Document_h */
