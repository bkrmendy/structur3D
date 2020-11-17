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

class ViewModelImpl : public ViewModel {
public:
    std::shared_ptr<Database> db;
    std::vector<DocumentWithName> documents_;
    std::optional<std::unique_ptr<Document>> currentDocument;
    std::vector<std::future<void>> cancellables;
    std::string message_;

    ViewModelImpl(std::shared_ptr<Database> db);

    void open(const ID& document);
    void createDocument();
    const std::optional<std::unique_ptr<Document>>& document() const;
    const std::vector<DocumentWithName>& documents() const;
    const std::string& message() const;
};

}

#endif /* viewmodel_h */
