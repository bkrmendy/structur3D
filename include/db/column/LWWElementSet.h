//
// Created by Berci on 2021. 01. 04..
//

#ifndef STRUCTUR3D_BASE_LWWELEMENTSET_H
#define STRUCTUR3D_BASE_LWWELEMENTSET_H

#include <memory>
#include <utility>
#include <numeric>

#include <sqlpp11/sqlpp11.h>
#include <sqlpp11/sqlite3/sqlite3.h>
#include <sqlpp11/sqlite3/connection.h>

#include "utils/to_underlying.h"
#include "utils/Serializator.h"
#include "utils/attribute_serialization.h"

#include "data/Base.h"
#include "data/Timestamp.h"

#include "db/Database.h"
#include "db/column/ColumnAttribute.h"
#include "db/utils/db_utils.h"
#include "db/utils/crdt_utils.h"

#include "generated/tables.h"

namespace S3D {

    template<ColumnAttribute attribute, typename T>
    class LWWElementSet {
        std::shared_ptr<sql::connection> db_;

    public:
        LWWElementSet() = delete;

        LWWElementSet(std::shared_ptr<sql::connection> db) : db_(std::move(db)) {}

        void upsert(const ID& entity_id, const T& thing, Timestamp timestamp) const;

        void retract(const ID& entity_id, const T& thing, Timestamp timestamp) const;

        std::set<T> latest(const ID& eid) const;
    };

    template<ColumnAttribute attribute, typename T>
    void LWWElementSet<attribute, T>::upsert(const ID &entity_id, const T &thing, Timestamp timestamp) const {
        const auto eid = to_string(entity_id);

        auto tx = sqlpp::start_transaction(*db_);

        (*db_)(CRDTCollectGarbage<attribute>(eid, timestamp));

        auto serializer = Serializator{};
        auto buffer = serializer.make_buffer();

        auto size = serializer.serialize(buffer, thing);

        Schema::Attribute columnTable;

        (*db_)(
                insert_into(columnTable)
                        .set(columnTable.deleted = is_deleted(false),
                             columnTable.timestamp = timestamp,
                             columnTable.entity = eid,
                             columnTable.colid = to_underlying(attribute),
                             columnTable.attrib = buffer,
                             columnTable.size = size));

        tx.commit();
    }

    template<ColumnAttribute attribute, typename T>
    void LWWElementSet<attribute, T>::retract(const ID &entity_id, const T &thing, Timestamp timestamp) const {
        const auto eid = to_string(entity_id);

        auto tx = sqlpp::start_transaction(*db_);

        (*db_)(CRDTCollectGarbage<attribute>(eid, timestamp));

        auto serializer = Serializator{};
        auto buffer = serializer.make_buffer();

        auto size = serializer.serialize(buffer, thing);

        Schema::Attribute columnTable;

        (*db_)(
                insert_into(columnTable)
                        .set(columnTable.deleted = is_deleted(true),
                             columnTable.timestamp = timestamp,
                             columnTable.entity = eid,
                             columnTable.colid = to_underlying(attribute),
                             columnTable.attrib = buffer,
                             columnTable.size = size));

        tx.commit();
    }

    template<ColumnAttribute attribute, typename T>
    std::set<T> LWWElementSet<attribute, T>::latest(const ID &eid) const {
        std::string entity_id = to_string(eid);
        Schema::Attribute columnTable;
        auto lookup
                = select(columnTable.attrib, columnTable.size, max(columnTable.timestamp))
                        .from(columnTable)
                        .where(columnTable.entity == entity_id
                               && columnTable.colid == to_underlying(attribute))
                        .group_by(columnTable.attrib, columnTable.size);

        std::set<ID> res;

        auto serializer = S3D::Serializator{};

        for (const auto& row : (*db_)(lookup)) {
            Schema::Attribute thisColumn;
            auto latestEntries =
                    (*db_)(select(thisColumn.deleted)
                                  .from(thisColumn)
                                  .where(thisColumn.entity == entity_id
                                         && thisColumn.attrib == row.attrib
                                         && columnTable.colid == to_underlying(attribute)
                                         && thisColumn.size == row.size
                                         && thisColumn.timestamp == row.max));

            bool alive = false;
            for (const auto& entry : latestEntries) {
                int deleted = entry.deleted;
                alive |= deleted == 0;
            }

            if (alive) {
                T value{};
                Serializator::Buffer buffer = row.attrib;
                unsigned int size = row.size;
                serializer.deserialize(buffer, value, size);
                res.insert(value);
            }
        }

        return res;
    }
}

#endif //STRUCTUR3D_BASE_LWWELEMENTSET_H
