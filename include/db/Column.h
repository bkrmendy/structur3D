//
// Created by Berci on 2020. 12. 02..
//

#ifndef STRUCTUR3D_BASE_COLUMN_H
#define STRUCTUR3D_BASE_COLUMN_H

#include <memory>
#include <sqlpp11/sqlpp11.h>
#include <sqlpp11/sqlite3/sqlite3.h>
#include <sqlpp11/sqlite3/connection.h>
#include <generated/tables.h>
#include <utils/to_underlying.h>

#include "data/Base.h"
#include "data/Timestamp.h"

#include "db/Database.h"
#include "generated/tables.h"

#include "utils/Serializator.h"

namespace sql = sqlpp::sqlite3;

namespace S3D {

    enum class ColumnAttribute: unsigned short int {
        Coordinate = 0,
        Radius = 1,
        Name = 2,
        SetOperationType = 3
    };

    constexpr int is_deleted(bool deleted) { return deleted ? 1 : 0; }

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
    struct Column {
        void upsert(std::unique_ptr<sql::connection>& db,
                    const ID& entity_id,
                    const T& thing,
                    Timestamp timestamp);
        void retract(std::unique_ptr<sql::connection>& db,
                     const ID& entity_id,
                     const T& thing,
                     Timestamp timestamp);
        T latest(std::unique_ptr<sql::connection>& db,
                 const ID& eid);
        std::vector<T> newer_than(std::unique_ptr<sql::connection>& db,
                                  ID&, Timestamp);
    };

    template<ColumnAttribute attribute, typename T>
    void Column<attribute, T>::upsert(std::unique_ptr<sql::connection> &db, const ID &entity_id, const T &thing,
                                      Timestamp timestamp) {
        const auto eid = to_string(entity_id);

        auto tx = sqlpp::start_transaction(*db);

        (*db)(CRDTCollectGarbage(attribute, eid, timestamp));

        auto serializer = Serializator{};
        auto buffer = serializer.make_buffer();

        auto size = serializer.serialize(buffer, thing);

        Schema::Attribute columnTable;

        (*db)(
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
    void Column<attribute, T>::retract(std::unique_ptr<sql::connection> &db, const ID &entity_id, const T &thing,
                                       Timestamp timestamp) {

    }

    template<ColumnAttribute attribute, typename T>
    T Column<attribute, T>::latest(std::unique_ptr<sql::connection> &db, const ID &) {
        return nullptr;
    }

    template<ColumnAttribute attribute, typename T>
    std::vector<T> Column<attribute, T>::newer_than(std::unique_ptr<sql::connection> &db, ID &, Timestamp) {
        return std::vector<T>();
    }
}

#endif //STRUCTUR3D_BASE_COLUMN_H
