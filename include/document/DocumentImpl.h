//
//  DocumentImpl.hpp
//  cross-platform-game
//
//  Created by Berci on 2020. 11. 14..
//  Copyright © 2020. Berci. All rights reserved.
//

#ifndef DocumentImpl_hpp
#define DocumentImpl_hpp

#include "./Document.h"

namespace S3D {

class DocumentImpl : public Document {
    std::unique_ptr<Graph> graph_;
    std::vector<std::unique_ptr<Mesh>> meshes_;
    void regenMesh();
public:
    void create(const std::shared_ptr<Node> node);
    void create(const std::shared_ptr<Edge> edge);

    void update(const std::shared_ptr<Node> node);

    void remove(const std::shared_ptr<Node> node);
    void remove(const std::shared_ptr<Edge> edge);

    Tree subTreeOf(const std::shared_ptr<Node> node) const;

    const std::vector<Tree> trees() const;
    const std::unique_ptr<Graph>& graph() const;

    const std::vector<std::unique_ptr<Mesh>>& meshes() const;

    ~DocumentImpl() {}
};

}

#endif /* DocumentImpl_hpp */
