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

class ViewModelImpl : public ViewModel, public Interactor, public std::enable_shared_from_this<ViewModelImpl> {
public:
    std::unique_ptr<Database> db_;
    std::unique_ptr<Network> network_;
    std::vector<DocumentWithName> documents_;
    std::optional<std::unique_ptr<Document>> currentDocument_;
    std::vector<std::future<void>> cancellables_;
    std::string message_;

    explicit ViewModelImpl(std::unique_ptr<Database> db, std::unique_ptr<Network> network);

    /*
     * Interactor interface
     */
    void create(std::shared_ptr<Node> node, const ID& document) final;
    void connect(const ID& from, const ID& to) final;
    void update(const ID& name, const Attribute& attribute) final;
    void remove(std::shared_ptr<Node> node, const ID& document) final;
    void disconnect(const ID& from, const ID& to) final;

    /*
     * Viewmodel interface
     */
    void open(const ID& document) final;
    void createDocument(const std::string& name) final;
    const std::optional<std::unique_ptr<Document>>& document() const final;
    const std::vector<DocumentWithName>& documents() const final;
    const std::string& message() const final;
};

}

#endif /* viewmodel_h */
