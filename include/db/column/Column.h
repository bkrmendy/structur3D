//
// Created by Berci on 2020. 12. 02..
//

#ifndef STRUCTUR3D_BASE_COLUMN_H
#define STRUCTUR3D_BASE_COLUMN_H

#include <memory>
#include <utility>
#include <sqlpp11/sqlpp11.h>
#include <sqlpp11/sqlite3/sqlite3.h>
#include <sqlpp11/sqlite3/connection.h>
#include <generated/tables.h>
#include <utils/to_underlying.h>
#include <db/column/ColumnAttribute.h>

#include "data/Base.h"
#include "data/Timestamp.h"

#include "db/Database.h"
#include "db/db_utils.h"
#include "generated/tables.h"

#include "utils/Serializator.h"

namespace sql = sqlpp::sqlite3;

namespace S3D {

    auto CRDTCollectGarbage(ColumnAttribute attribute,
                            const std::string& entity_id,
                            Timestamp timestamp) {
        Schema::Attribute attributeTable;
        return remove_from(attributeTable)
                .where(attributeTable.colid == to_underlying(attribute)
                        && attributeTable.timestamp < timestamp
                        && (attributeTable.entity == entity_id
                           || attributeTable.deleted == is_deleted(true)));
    }

    template<ColumnAttribute attribute, typename T>
    class Column {
        std::shared_ptr<sql::connection> db_;

    public:
        Column() = delete;

        Column(std::shared_ptr<sql::connection> db)
            : db_{std::move(db)} { }

        void upsert(const ID& entity_id, const T& thing, Timestamp timestamp) const;

        void retract(const ID& entity_id, const T& thing, Timestamp timestamp) const;

        std::optional<T> latest(const ID& eid) const;

        std::vector<T> newer_than(ID&, Timestamp) const;
    };

    template<ColumnAttribute attribute, typename T>
    void Column<attribute, T>::upsert(const ID &entity_id, const T &thing, Timestamp timestamp) const {
        const auto eid = to_string(entity_id);

        auto tx = sqlpp::start_transaction(*db_);

        (*db_)(CRDTCollectGarbage(attribute, eid, timestamp));

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
    void Column<attribute, T>::retract(const ID &entity_id, const T &thing, Timestamp timestamp) const {
        const auto eid = to_string(entity_id);

        auto tx = sqlpp::start_transaction(*db_);

        (*db_)(CRDTCollectGarbage(attribute, eid, timestamp));

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
    std::optional<T> Column<attribute, T>::latest(const ID &) const {
        return std::nullopt;
    }

    template<ColumnAttribute attribute, typename T>
    std::vector<T> Column<attribute, T>::newer_than(ID &, Timestamp) const {
        return std::vector<T>();
    }
}

#endif //STRUCTUR3D_BASE_COLUMN_H
