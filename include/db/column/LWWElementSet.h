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

    template<ColumnAttribute attribute, typename T>
    std::set<T> LWWElementSet<attribute, T>::latest(const ID &eid) const {
        std::string entity_id = to_string(eid);
        Schema::Edge edge;
        auto lookup
                = select(edge.entityTo, max(edge.timestamp))
                        .from(edge)
                        .where(edge.entity == entity_id)
                        .group_by(edge.entityTo);

        std::set<ID> res;

        for (const auto& row : (*db_)(lookup)) {
            Schema::Edge thisEdge;
            auto latestEdges =
                    (*db_)(select(thisEdge.deleted)
                                  .from(thisEdge)
                                  .where(thisEdge.entity == entity_id
                                         && thisEdge.entityTo == row.entityTo
                                         && thisEdge.timestamp == row.max));

            bool alive = false;
            for (const auto& entry : latestEdges) {
                int deleted = entry.deleted;
                alive |= deleted == 0;
            }

            if (alive) {
                std::stringstream stream{row.entityTo};
                ID uid;
                stream >> uid;
                res.insert(uid);
            }
        }

        return res;
    }
}

#endif //STRUCTUR3D_BASE_LWWELEMENTSET_H
