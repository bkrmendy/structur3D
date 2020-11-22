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
#include <wire/Network.h>
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
    std::unique_ptr<Network> network_;
    std::vector<DocumentWithName> documents_;
    std::unique_ptr<Document> currentDocument_;
    std::vector<std::future<void>> cancellables_;
    std::string message_;

    ViewModelImpl(std::unique_ptr<S3D::Database> db, std::unique_ptr<Network> network);

    /*
     * Interactor interface
     */
    void create(std::shared_ptr<Node> node, const ID& document) override;
    void connect(const ID& from, const ID& to) override;
    void update(const ID& name, const Attribute& attribute) override;
    void remove(std::shared_ptr<Node> node, const ID& document) override;
    void disconnect(const ID& from, const ID& to) override;

    /*
     * Viewmodel interface
     */
    void open(const ID& document) override;
    void createDocument(const std::string& name) override;
    const std::unique_ptr<Document>& document() const override;
    const std::vector<DocumentWithName>& documents() const override;
    const std::string& message() const override;
};

}

#endif /* viewmodel_h */
