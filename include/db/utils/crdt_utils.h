//
// Created by Berci on 2020. 12. 05..
//

#ifndef STRUCTUR3D_BASE_CRDT_UTILS_H
#define STRUCTUR3D_BASE_CRDT_UTILS_H

#include "data/Timestamp.h"

#include "db/column/ColumnAttribute.h"
#include "db_utils.h"

#include "generated/tables.h"

namespace S3D {
    template<ColumnAttribute attribute>
    auto CRDTCollectGarbage(const std::string& entity_id,
                            Timestamp timestamp) {
        Schema::Attribute attributeTable;
        return remove_from(attributeTable)
                .where(attributeTable.colid == to_underlying(attribute)
                       && attributeTable.timestamp < timestamp
                       && (attributeTable.entity == entity_id
                           || attributeTable.deleted == is_deleted(true)));
    }

    template<ColumnAttribute attribute>
    auto LastWriterWinsTimestamp(const std::string& entity_id) {
        Schema::Attribute attributeTable;
        return select(max(attributeTable.timestamp))
                .from(attributeTable)
                .where(attributeTable.deleted == is_deleted(false)
                       && attributeTable.colid == to_underlying(attribute)
                       && attributeTable.entity == entity_id)
                .group_by(attributeTable.entity);
    }
}

#endif //STRUCTUR3D_BASE_CRDT_UTILS_H
