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

class ViewModelImpl final : public ViewModel {
public:
    std::unique_ptr<Database> db_;
    std::unique_ptr<ClientEndpoint> network_;

    std::vector<DocumentWithName> documents_;

    std::unique_ptr<Document> currentDocument_;

    std::shared_ptr<Interactor> databaseInteractor_;
    std::shared_ptr<Interactor> documentInteractor_;
    std::shared_ptr<Interactor> networkInteractor_;

    std::vector<std::future<void>> cancellables_;
    std::string message_;

    ViewModelImpl(std::unique_ptr<S3D::Database> db, std::unique_ptr<ClientEndpoint> network);

    std::shared_ptr<Interactor> makeDBInteractor() const;
    std::shared_ptr<Interactor> makeNetworkInteractor(std::shared_ptr<Interactor> dbInteractor);
    std::shared_ptr<Interactor> makeDocumentInteractor(std::shared_ptr<Interactor> dbInteractor);

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
