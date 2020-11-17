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
    : db{std::move(db)}
    , cancellables{}
    , documents_{}
    , message_{} {
        
        this->currentDocument = std::nullopt;
}

void ViewModelImpl::createDocument() {
    // TODO: implement
}

const std::vector<DocumentWithName>& ViewModelImpl::documents() const {
    return this->documents_;
}

void ViewModelImpl::open(const ID &document) {
    // TODO: implement
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

