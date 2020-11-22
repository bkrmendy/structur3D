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
#include <future>
#include <mesh/MeshFactory.h>
#include <interactor/Interactor.h>

#include "Document.h"
#include "db/Database.h"

namespace S3D {

class DocumentImpl : public Document {
    std::shared_ptr<Interactor> interactor_;

    const ID id_;
    std::unique_ptr<Graph> graph_;
    std::unique_ptr<Mesh> mesh_;
    std::unique_ptr<MeshFactory> factory_;

    std::vector<std::future<void>> cancellables;

    void regenMesh();
public:
    DocumentImpl(const ID& id,
                 std::shared_ptr<Interactor> interactor,
                 std::unique_ptr<Graph> graph,
                 std::unique_ptr<MeshFactory> factory)
        : id_{id}
        , interactor_{std::move(interactor)}
        , graph_{std::move(graph)}
        , mesh_{{}, {}}
        , factory_{std::move(factory)}
        { }

    void create(std::shared_ptr<Node> node) final;
    void create(std::shared_ptr<Edge> edge) final;

    void update(std::shared_ptr<Node> node) final;

    void remove(std::shared_ptr<Node> node) final;
    void remove(std::shared_ptr<Edge> edge) final;

    const std::unique_ptr<Graph>& graph() const final;

    const std::unique_ptr<Mesh>& mesh() const final;
    const ID id() const final;
};

}

#endif /* DocumentImpl_hpp */
