//
// Created by Berci on 2021. 01. 04..
//

#ifndef STRUCTUR3D_BASE_LWWELEMENTSET_H
#define STRUCTUR3D_BASE_LWWELEMENTSET_H

#include <memory>
#include <set>
#include <functional>
#include <utility>

#include <sqlpp11/sqlpp11.h>
#include <sqlpp11/sqlite3/sqlite3.h>
#include <sqlpp11/sqlite3/connection.h>

#include "data/Base.h"
#include "data/Timestamp.h"

#include "db/utils/crdt_utils.h"

namespace S3D {

    template<ColumnAttribute attribute, typename T>
    class LWWElementSet {
        std::shared_ptr<sql::connection> db_;
        std::function<T(T&, T&)> preferred_;

    public:
        LWWElementSet() = delete;

        LWWElementSet(std::shared_ptr<sql::connection> db, const std::function<T(T &, T &)> &preferred)
            : db_(std::move(db))
            , preferred_(preferred)
            {}

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
}

#endif //STRUCTUR3D_BASE_LWWELEMENTSET_H
