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

#include "../data/Edge.h"
#include "../data/Node.h"
#include "../data/Tree.h"
#include "../data/Graph.h"
#include "../mesh/Mesh.h"

namespace S3D {

struct Document {
    virtual void create(const std::shared_ptr<Node> node) = 0;
    virtual void create(const std::shared_ptr<Edge> edge) = 0;

    virtual void update(const std::shared_ptr<Node> node) = 0;

    virtual void remove(const std::shared_ptr<Node> node) = 0;
    virtual void remove(const std::shared_ptr<Edge> edge) = 0;

    virtual const std::vector<Tree> trees() const = 0;
    virtual const std::unique_ptr<Graph>& graph() const = 0;
    virtual const std::vector<std::unique_ptr<Mesh>>& meshes() const = 0;

    virtual ~Document() { }
};

}

#endif /* Document_h */
