//
//  VIewmodel.cpp
//  cross-platform-game iOS
//
//  Created by Berci on 2020. 11. 04..
//  Copyright © 2020. Berci. All rights reserved.
//

#include <algorithm>

#include "vm/ViewmodelImpl.h"
#include "document/DocumentImpl.h"

namespace S3D {

ViewModelImpl::ViewModelImpl(std::shared_ptr<Database> db)
    : db{db}
    , cancellables{}
    , documents_{}
    , message_{} {
        this->currentDocument = std::nullopt;
        this->documents_ = this->db->documents();
}

void ViewModelImpl::createDocument(const std::string& name) {
    auto makeID = IDFactory();
    this->documents_.emplace_back(DocumentWithName{makeID(), name});
}

const std::vector<DocumentWithName>& ViewModelImpl::documents() const {
    return this->documents_;
}

void ViewModelImpl::open(const ID &document) {
    std::vector<std::shared_ptr<Node>> nodes{};
    std::vector<std::shared_ptr<Edge>> edges{};

    auto entities = this->db->entities(document);

    for (const auto& entity : entities) {
        if (entity.type == NodeType::Sphere) {
            auto maybeSphere = this->db->sphere(entity.uid);
            if (maybeSphere.has_value()) {
                nodes.push_back(std::make_shared<Sphere>(maybeSphere.value()));
            }
        } else if (entity.type == NodeType::SetOperation) {
            auto maybeSetOp = this->db->setop(entity.uid);
            if (maybeSetOp.has_value()) {
                nodes.push_back(std::make_shared<SetOp>(maybeSetOp.value()));
            }
        }
    }

    auto makeID = IDFactory();

    for (const auto& node : nodes) {
        for (const auto& toPtr : this->db->edges(node->id())) {
            for (const auto& toNode : nodes) {
                if (toNode->id() == toPtr) {
                    const auto id = makeID();
                    edges.emplace_back(std::make_shared<Edge>(id, node, toNode));
                }
            }
        }
    }

    this->currentDocument
        = std::make_unique<DocumentImpl>(document,
                                         this->db,
                                         std::make_unique<Graph>(edges, nodes),
                                         std::make_unique<MeshFactory>());
}

const std::optional<std::unique_ptr<Document>>& ViewModelImpl::document() const {
    return this->currentDocument;
}

const std::string& ViewModelImpl::message() const {
    return this->message_;
}

//auto gen = IDFactory{};
//std::shared_ptr<Node> sphere1 = std::make_unique<Sphere>(Sphere{gen(), Coord{2, 0, 0}, 1});
//std::shared_ptr<Node> sphere2 = std::make_unique<Sphere>(Sphere{gen(), Coord{-2, 0, 0}, 1});
//std::shared_ptr<Node> sphere3 = std::make_unique<Sphere>(Sphere{gen(), Coord{0, 2, 0}, 1});
//std::shared_ptr<Node> unionOp = std::make_unique<SetOp>(SetOp{gen(), SetOperationType::Union});
//
//std::shared_ptr<Edge> us3 = std::make_shared<Edge>(Edge{ gen(), unionOp, sphere3 });
//std::shared_ptr<Edge> us1 = std::make_shared<Edge>(Edge{ gen(), unionOp, sphere1 });
//std::shared_ptr<Edge> us2 = std::make_shared<Edge>(Edge{ gen(), unionOp, sphere2 });
//
//this->graph_
//    = std::make_unique<Graph>(Graph{{ us1, us2, us3 }, { sphere1, sphere2, unionOp, sphere3 }});
//
//for (auto& tree : this->trees()) {
//    this->meshes_.push_back(Mesh::fromTree(tree));
//}

}

