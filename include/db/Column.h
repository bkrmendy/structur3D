//
// Created by Berci on 2020. 12. 02..
//

#ifndef STRUCTUR3D_BASE_COLUMN_H
#define STRUCTUR3D_BASE_COLUMN_H

#include "data/Base.h"
#include "data/Timestamp.h"

namespace S3D {
    template<typename T>
    struct Column {
        virtual void upsert(const ID&, const T&, Timestamp timestamp) = 0;
        virtual void retract(const ID&, const T&, Timestamp timestamp) = 0;
        virtual T latest(const ID&) = 0;
        virtual std::vector<T> newer_than(ID&, Timestamp) = 0;
    };
}

#endif //STRUCTUR3D_BASE_COLUMN_H
