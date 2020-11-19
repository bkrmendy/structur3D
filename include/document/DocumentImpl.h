//
//  DocumentImpl.hpp
//  cross-platform-game
//
//  Created by Berci on 2020. 11. 14..
//  Copyright © 2020. Berci. All rights reserved.
//

#ifndef DocumentImpl_hpp
#define DocumentImpl_hpp

#include "Document.h"
#include "db/Database.h"

namespace S3D {

class DocumentImpl : public Document {
    std::shared_ptr<Database> db;

    const ID id_;
    std::unique_ptr<Graph> graph_;
    std::vector<std::unique_ptr<Mesh>> meshes_;

    std::vector<std::future<void>> cancellables;

    void regenMesh();
public:
    DocumentImpl(const ID& id, std::shared_ptr<Database> db) : id_{id}, db{db} { }

    void create(const std::shared_ptr<Node> node);
    void create(const std::shared_ptr<Edge> edge);

    void update(const std::shared_ptr<Node> node);

    void remove(const std::shared_ptr<Node> node);
    void remove(const std::shared_ptr<Edge> edge);

    Tree subTreeOf(const std::shared_ptr<Node> node) const;

    std::vector<Tree> trees() const;
    const std::unique_ptr<Graph>& graph() const;

    const std::vector<std::unique_ptr<Mesh>>& meshes() const;
    ID id() const;

    ~DocumentImpl() {}
};

}

#endif /* DocumentImpl_hpp */
