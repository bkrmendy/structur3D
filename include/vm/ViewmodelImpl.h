//
//  viewmodel.h
//  cross-platform-game
//
//  Created by Berci on 2020. 11. 04..
//  Copyright © 2020. Berci. All rights reserved.
//

#ifndef viewmodel_h
#define viewmodel_h

#include <future>
#include <wire/ClientEndpoint.h>
#include <interactor/Interactor.h>

#include "./Viewmodel.h"

#include "../data/Base.h"
#include "../data/Sphere.h"
#include "../data/SetOp.h"
#include "../data/Edge.h"
#include "../data/Tree.h"
#include "../data/Graph.h"
#include "../db/Database.h"

#include "../mesh/Mesh.h"

namespace S3D {

class ViewModelImpl final : public ViewModel, public Interactor, public std::enable_shared_from_this<ViewModelImpl> {
public:
    std::unique_ptr<Database> db_;
    std::unique_ptr<ClientEndpoint> network_;
    std::vector<DocumentWithName> documents_;
    std::unique_ptr<Document> currentDocument_;
    std::vector<std::future<void>> cancellables_;
    std::string message_;

    ViewModelImpl(std::unique_ptr<S3D::Database> db, std::unique_ptr<ClientEndpoint> network);

    /*
     * Interactor interface
     */
    void create(std::shared_ptr<Node> node, const ID& document, Timestamp timestamp) override;
    void connect(const ID& from, const ID& to, Timestamp timestamp) override;
    void upsert(const ID& name, const Attribute& attribute, Timestamp timestamp) override;
    void retract(const ID& name, const Attribute& attribute, Timestamp timestamp) override;
    void remove(std::shared_ptr<Node> node, const ID& document, Timestamp timestamp) override;
    void disconnect(const ID& from, const ID& to, Timestamp timestamp) override;

    /*
     * Viewmodel interface
     */
    void open(const ID& document) override;
    void createDocument(const Name& name) override;
    const std::unique_ptr<Document>& document() const override;
    const std::vector<DocumentWithName>& documents() const override;
    const std::string& message() const override;
};

}

#endif /* viewmodel_h */
