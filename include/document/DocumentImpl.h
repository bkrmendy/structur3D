//
//  DocumentImpl.hpp
//  cross-platform-game
//
//  Created by Berci on 2020. 11. 14..
//  Copyright © 2020. Berci. All rights reserved.
//

#ifndef DocumentImpl_hpp
#define DocumentImpl_hpp

#include <utility>

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
    DocumentImpl(const ID& id, std::shared_ptr<Database> db, std::unique_ptr<Graph> graph)
        : id_{id}
        , db{std::move(db)}
        , graph_{std::move(graph)} { }

    void create(std::shared_ptr<Node> node) final;
    void create(std::shared_ptr<Edge> edge) final;

    void update(std::shared_ptr<Node> node) final;

    void remove(std::shared_ptr<Node> node) final;
    void remove(std::shared_ptr<Edge> edge) final;

    const std::unique_ptr<Graph>& graph() const final;

    const std::vector<std::unique_ptr<Mesh>>& meshes() const final;
    const ID id() const final;

    ~DocumentImpl() {} ;
};

}

#endif /* DocumentImpl_hpp */
