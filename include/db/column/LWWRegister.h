//
// Created by Berci on 2020. 12. 02..
//

#ifndef STRUCTUR3D_BASE_LWWREGISTER_H
#define STRUCTUR3D_BASE_LWWREGISTER_H

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

namespace sql = sqlpp::sqlite3;

namespace S3D {
    template<ColumnAttribute attribute, typename T>
    class LWWRegister {
        std::shared_ptr<sql::connection> db_;
        std::function<T(T&, T&)> preferred_;

    public:
        LWWRegister() = delete;

        LWWRegister(std::shared_ptr<sql::connection> db, std::function<T(T&, T&)> preferred)
            : db_{std::move(db)}
            , preferred_{std::move(preferred)}
            { }

        void upsert(const ID& entity_id, const T& thing, Timestamp timestamp) const;

        void retract(const ID& entity_id, const T& thing, Timestamp timestamp) const;

        std::optional<T> latest(const ID& eid) const;

        std::vector<T> newer_than(ID&, Timestamp) const;
    };

    template<ColumnAttribute attribute, typename T>
    void LWWRegister<attribute, T>::upsert(const ID &entity_id, const T &thing, Timestamp timestamp) const {
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
    void LWWRegister<attribute, T>::retract(const ID &entity_id, const T &thing, Timestamp timestamp) const {
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
    std::optional<T> LWWRegister<attribute, T>::latest(const ID& entity_id) const {
        std::string eid = to_string(entity_id);
        Schema::Attribute attributeTable;
        auto result
                = (*db_)(select(attributeTable.attrib, attributeTable.size)
                                 .from(attributeTable)
                                 .where(attributeTable.deleted == is_deleted(false)
                                        && attributeTable.colid == to_underlying(attribute)
                                        && attributeTable.entity == eid
                                        && attributeTable.timestamp == LastWriterWinsTimestamp<attribute>(eid)));

        if (result.empty()) {
            return std::nullopt;
        }

        auto serializer = S3D::Serializator{};

        std::vector<T> results{};
        for (const auto& row : result) {
            T value{};
            Serializator::Buffer buffer = row.attrib;
            unsigned int size = row.size;
            serializer.deserialize(buffer, value, size);
            results.push_back(value);
        }

        auto preferred = std::reduce(results.begin(), results.end(), results.at(0), preferred_);

        return preferred;
    }

    template<ColumnAttribute attribute, typename T>
    std::vector<T> LWWRegister<attribute, T>::newer_than(ID& entity_id, Timestamp timestamp) const {
        std::string eid = to_string(entity_id);
        Schema::Attribute attributeTable;
        auto result
                = (*db_)(select(attributeTable.attrib, attributeTable.size)
                                 .from(attributeTable)
                                 .where(attributeTable.deleted == is_deleted(false)
                                        && attributeTable.colid == to_underlying(attribute)
                                        && attributeTable.entity == eid
                                        && attributeTable.timestamp > timestamp));

        auto serializer = S3D::Serializator{};

        std::vector<T> results{};
        for (const auto& row : result) {
            T value{};
            Serializator::Buffer buffer = row.attrib;
            unsigned int size = row.size;
            serializer.deserialize(buffer, value, size);
            results.push_back(value);
        }

        return results;
    }
}

#endif //STRUCTUR3D_BASE_LWWREGISTER_H
