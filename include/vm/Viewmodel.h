//
//  Viewmodel.h
//  cross-platform-game
//
//  Created by Berci on 2020. 11. 13..
//  Copyright © 2020. Berci. All rights reserved.
//

#ifndef Viewmodel_h
#define Viewmodel_h

#include <optional>
#include <memory>
#include <vector>
#include <string>

#include "../data/Base.h"
#include "../db/DocumentWithName.h"
#include "../document/Document.h"

namespace S3D {

class ViewModel {
public:
    virtual const std::unique_ptr<Document>& document() const = 0;
    virtual void open(const ID& document) = 0;
    virtual void createDocument(const Name& name) = 0;
    virtual const std::vector<DocumentWithName>& documents() const = 0;
    virtual const std::string& message() const = 0;

    virtual ~ViewModel() = default;
};

}

#endif /* Viewmodel_h */
