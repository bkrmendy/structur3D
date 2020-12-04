//
// Created by Berci on 2020. 12. 02..
//

#ifndef STRUCTUR3D_BASE_COLUMN_H
#define STRUCTUR3D_BASE_COLUMN_H

#include <memory>

#include "data/Base.h"
#include "data/Timestamp.h"

#include "db/Database.h"

namespace S3D {
    template<typename T>
    struct Column {
        virtual void upsert(std::unique_ptr<Database>& db,
                            const ID& entity_id,
                            const T& thing,
                            Timestamp timestamp) = 0;
        virtual void retract(std::unique_ptr<Database>& db,
                             const ID& entity_id, const T&, Timestamp timestamp) = 0;
        virtual T latest(std::unique_ptr<Database>& db, const ID&) = 0;
        virtual std::vector<T> newer_than(std::unique_ptr<Database>& db, ID&, Timestamp) = 0;
    };
}

#endif //STRUCTUR3D_BASE_COLUMN_H
