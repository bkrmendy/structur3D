//
//  DatabaseImpl.cpp
//  cross-platform-game
//
//  Created by Berci on 2020. 11. 11..
//  Copyright © 2020. Berci. All rights reserved.
//

#include <string>

#include "db/DatabaseImpl.h"
#include "generated/tables.h"
#include "data/Timestamp.h"
#include "data/Coord.h"

#include "boost/uuid/uuid_io.hpp"

#include <sqlpp11/sqlpp11.h>
#include "sqlpp11/remove.h"
#include "sqlpp11/insert.h"
#include "sqlpp11/transaction.h"
#include "sqlpp11/select.h"

namespace S3D {
    constexpr int is_deleted(bool deleted) { return deleted ? 1 : 0; }

    template <typename Table>
    auto LastWriterWinsTimestamp(Table table, const std::string& eid) {
        return select(max(table.timestamp))
                .from(table)
                .where(table.deleted == is_deleted(false)
                        && table.entity == eid)
                .group_by(table.entity);
    }

    template<typename Table>
    auto CRDTCollectGarbage(Table table, const std::string& eid, Timestamp timestamp) {
        return remove_from(table)
                .where(table.timestamp < timestamp
                        && (table.entity == eid || table.deleted == is_deleted(true)));
    }

    void DatabaseImpl::upsertI(const ID& entity, const Coord& coord, Timestamp timestamp, bool deleted) {
        std::string eid = to_string(entity);

        auto tx = sqlpp::start_transaction(*db);
        Schema::Coord removeCoord;
        (*db)(CRDTCollectGarbage(removeCoord, eid, timestamp));

        Schema::Coord insertCoord;
        (*db)(
            insert_into(insertCoord)
                 .set(insertCoord.deleted = is_deleted(deleted),
                      insertCoord.timestamp = timestamp,
                      insertCoord.entity = eid,
                      insertCoord.x = coord.x,
                      insertCoord.y = coord.y,
                      insertCoord.z = coord.z));

        tx.commit();
    }

    void DatabaseImpl::upsertI(const ID &entity, const SetOperationType &type, Timestamp timestamp, bool deleted) {
        std::string eid = to_string(entity);

        auto tx = sqlpp::start_transaction(*db);
        Schema::Setoperationtype removeOp;
        (*db)(CRDTCollectGarbage(removeOp, eid, timestamp));

        Schema::Setoperationtype insertOp;
        auto typeAsInteger = from_operationType(type);
        this->db->operator()(
            insert_into(insertOp)
                 .set(insertOp.deleted = is_deleted(deleted),
                      insertOp.timestamp = timestamp,
                      insertOp.entity = eid,
                      insertOp.type = typeAsInteger));

        tx.commit();
    }

    void DatabaseImpl::upsertI(const ID &entity, const Name &name, Timestamp timestamp, bool deleted) {
        std::string eid = to_string(entity);

        auto tx = sqlpp::start_transaction(*db);
        Schema::Name removeName;
        (*db)(CRDTCollectGarbage(removeName, eid, timestamp));

        Schema::Name insertName;
        this->db->operator()(
                insert_into(insertName)
                    .set(insertName.entity = eid,
                         insertName.timestamp = timestamp,
                         insertName.deleted = is_deleted(deleted),
                         insertName.name = name.get()));

        tx.commit();
    }

    void DatabaseImpl::connectI(const ID &entity, const ID &entity_to, Timestamp timestamp, bool deleted) {
        std::string eid = to_string(entity);
        std::string eid_to = to_string(entity_to);

        auto tx = sqlpp::start_transaction(*db);
        Schema::Edge removeEdge;
        (*db)(remove_from(removeEdge)
                    .where(removeEdge.entity == eid
                           && removeEdge.entityTo == eid_to
                           && removeEdge.timestamp < timestamp
                           || (
                                   removeEdge.timestamp < timestamp
                                && removeEdge.deleted == is_deleted(true))));

        Schema::Edge insertEdge;
        (*db)(insert_into(insertEdge)
                .set(insertEdge.entity = eid,
                     insertEdge.entityTo = eid_to,
                     insertEdge.timestamp = timestamp,
                     insertEdge.deleted = is_deleted(deleted)));

        tx.commit();
    }

    void DatabaseImpl::createI(const ID &entity, const NodeType& type, const ID &document, Timestamp timestamp, bool deleted) {
        std::string eid = to_string(entity);
        std::string doc = to_string(document);

        auto tx = sqlpp::start_transaction(*db);

        Schema::Document removeDoc;
        this->db->operator()(
            remove_from(removeDoc)
                .where(removeDoc.entity == eid
                       && removeDoc.document == doc
                       && removeDoc.timestamp < timestamp
                       || removeDoc.deleted == is_deleted(true)));

        Schema::Document updateDoc;
        this->db->operator()(
            insert_into(updateDoc)
                .set(updateDoc.entity = eid,
                     updateDoc.type = to_integral(type),
                     updateDoc.document = doc,
                     updateDoc.timestamp = timestamp,
                     updateDoc.deleted = is_deleted(deleted)));

        tx.commit();
    }

    void DatabaseImpl::upsert(const ID &entity, const Coord &coord, Timestamp timestamp) {
        this->upsertI(entity, coord, timestamp, false);
    }

    void DatabaseImpl::retract(const ID &entity, const Coord &coord, Timestamp timestamp) {
        this->upsertI(entity, coord, timestamp, false);
    }

    void DatabaseImpl::upsertI(const ID &entity, const Radius &radius, Timestamp timestamp, bool deleted) {
        std::string eid = to_string(entity);

        auto tx = sqlpp::start_transaction(*db);

        Schema::Radius removeRadius;
        CRDTCollectGarbage(removeRadius, eid, timestamp);

        Schema::Radius insertRadius;
        (*db)(
            insert_into(insertRadius)
                .set(insertRadius.deleted = is_deleted(deleted),
                     insertRadius.timestamp = timestamp,
                     insertRadius.entity = eid,
                     insertRadius.magnitude = radius.magnitude()));

        tx.commit();
    }

    void DatabaseImpl::upsert(const ID &entity, const Radius &radius, Timestamp timestamp) {
        upsertI(entity, radius, timestamp, false);
    }

    void DatabaseImpl::retract(const ID &entity, const Radius &radius, Timestamp timestamp) {
        upsertI(entity, radius, timestamp, true);
    }

    void DatabaseImpl::upsert(const ID &entity, const SetOperationType &type, Timestamp timestamp) {
        upsertI(entity, type, timestamp, false);
    }

    void DatabaseImpl::upsert(const ID &eid, const Name &name, Timestamp timestamp) {
        upsertI(eid, name, timestamp, false);
    }

    void DatabaseImpl::retract(const ID &eid, const Name &name, Timestamp timestamp) {
        upsertI(eid, name, timestamp, true);
    }

    void DatabaseImpl::retract(const ID &entity, const SetOperationType &type, Timestamp timestamp) {
        upsertI(entity, type, timestamp, true);
    }

    void DatabaseImpl::connect(const ID &entity, const ID &entity_to, Timestamp timestamp) {
        connectI(entity, entity_to, timestamp, false);
    }

    void DatabaseImpl::disconnect(const ID &entity, const ID &entity_to, Timestamp timestamp) {
        connectI(entity, entity_to, timestamp, true);
    }

    void DatabaseImpl::create(const ID &entity, const NodeType& type, const ID &document, Timestamp timestamp) {
        createI(entity, type, document, timestamp, false);
    }

    void DatabaseImpl::remove(const ID &entity, const ID &document, Timestamp timestamp) {
        createI(entity, NodeType::Sphere, document, timestamp, true);
    }

    std::vector<DocumentWithName> DatabaseImpl::documents() {
        Schema::Document doc;
        auto docLookup =
                select(doc.document)
                        .flags(sqlpp::distinct)
                        .from(doc)
                        .where(doc.deleted == is_deleted(false))
                        .group_by(doc.document);

        std::vector<DocumentWithName> names{};

        for (const auto& documentRow : (*db)(docLookup)) {
            Schema::Name nameTable;
            auto nameLookup
                = (*db)(select(nameTable.name, nameTable.timestamp)
                        .from(nameTable)
                        .where(nameTable.deleted == is_deleted(false)
                                && nameTable.entity == documentRow.document
                                && nameTable.timestamp == LastWriterWinsTimestamp(nameTable, documentRow.document)));

            if (!nameLookup.empty()) {
                auto latestName = Name{nameLookup.front().name};

                for (const auto& row : nameLookup) {
                    latestName = DatabaseImpl::preferredNameOf(latestName, Name{row.name});
                }

                std::stringstream stream{documentRow.document};
                ID uid_temp;
                stream >> uid_temp;
                names.emplace_back(uid_temp, latestName);
            }
        }

        return names;
    }

    std::vector<IDWithType> DatabaseImpl::entities(const ID &of_document) {
        std::string did = to_string(of_document);
        Schema::Document doc;
        auto lookup =
            dynamic_select(*db)
                .columns(doc.entity, doc.type)
                .from(doc)
                .where(doc.deleted == 0 && doc.document == did);

        std::vector<IDWithType> res;
        for (const auto& row : db->operator()(lookup)) {
            std::stringstream stream{row.entity};
            ID uid;
            stream >> uid;

            auto mayBeType = from_integral(row.type);
            if (mayBeType.has_value()) {
                res.emplace_back(uid, mayBeType.value() );
            }
        }

        return res;
    }

    std::vector<ID> DatabaseImpl::edges(const ID &of_entity) {
        std::string eid = to_string(of_entity);
        Schema::Edge edge;
        auto lookup
            = select(edge.entityTo, max(edge.timestamp))
                .from(edge)
                .where(edge.entity == eid)
                .group_by(edge.entityTo);

        std::vector<ID> res;

        for (const auto& row : (*db)(lookup)) {
            Schema::Edge thisEdge;
            auto latestEdges =
                    (*db)(select(thisEdge.deleted)
                        .from(thisEdge)
                        .where(thisEdge.entity == eid
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
                res.push_back(uid);
            }
        }

        return res;
    }

    std::optional<Sphere> DatabaseImpl::sphere(const ID &from) {
        std::string eid = to_string(from);

        Schema::Document documentLookup;
        auto lookup
            = (*db)(select(documentLookup.entity)
                        .from(documentLookup)
                        .where(documentLookup.deleted == is_deleted(false)
                                && documentLookup.entity == eid)
                        .order_by(documentLookup.timestamp.desc())
                        .limit(1U));

        if (lookup.empty()) {
            return std::nullopt;
        }

        Schema::Radius radiusTable;
        auto radiusRes
            = (*db)(select(radiusTable.magnitude)
                        .from(radiusTable)
                        .where(radiusTable.deleted == 0
                                && radiusTable.entity == eid
                                && radiusTable.timestamp == LastWriterWinsTimestamp(radiusTable, eid)));

        if (radiusRes.empty()) {
            return std::nullopt;
        }

        float maxRadius = radiusRes.front().magnitude;
        for (const auto& radius : radiusRes) {
            if (radius.magnitude > maxRadius) {
                maxRadius = radius.magnitude;
            }
        }

        Schema::Coord coordTable;
        auto coordRes
            = (*db)(select(coordTable.x, coordTable.y, coordTable.z)
                .from(coordTable)
                .where(coordTable.deleted == 0
                        && coordTable.entity == eid
                        && coordTable.timestamp == LastWriterWinsTimestamp(coordTable, eid)));

        if (coordRes.empty()) {
            return std::nullopt;
        }

        Coord farthestCoord = Coord{(float)coordRes.front().x,
                                    (float)coordRes.front().y,
                                    (float)coordRes.front().z};

        for (const auto& row : coordRes) {
            auto coord = Coord{(float)row.x,
                               (float)row.y,
                               (float)row.z};

            if (distance_from_origin(coord) > distance_from_origin(farthestCoord)) {
                farthestCoord = coord;
            }
        }

        return Sphere(from,
                      farthestCoord,
                      Radius(maxRadius));
    }

    std::optional<SetOp> DatabaseImpl::setop(const ID &from) {
        std::string eid = to_string(from);
        Schema::Setoperationtype optype;
        auto optypeLookup
            = dynamic_select(*db)
                .columns(optype.type)
                .from(optype)
                .where(optype.deleted == 0 && optype.entity == eid)
                .order_by(optype.timestamp.desc())
                .limit(1u);

        auto res = db->operator()(optypeLookup);

        if (res.empty()) {
            return std::nullopt;
        }

        auto type = to_operationType(res.front().type);
        if (!type.has_value()) {
            return std::nullopt;
        }

        return SetOp(from, type.value());
    }

    std::vector<std::string> DatabaseImpl::tables() {
        auto edge = R"(
        CREATE TABLE IF NOT EXISTS edge (
        deleted INTEGER NOT NULL DEFAULT 0,
        timestamp UNSIGNED BIG INT NOT NULL,
        entity TEXT NOT NULL,
        entity_to TEXT NOT NULL
        );
        )";

        auto radius = R"(
        CREATE TABLE IF NOT EXISTS radius (
             deleted INTEGER NOT NULL DEFAULT 0,
             timestamp UNSIGNED BIG INT NOT NULL,
             entity TEXT NOT NULL,
             magnitude REAL NOT NULL CHECK (magnitude >= 0)
         );
        )";

        auto coord = R"(
        CREATE TABLE IF NOT EXISTS coord (
            deleted INTEGER NOT NULL DEFAULT 0,
            timestamp UNSIGNED BIG INT NOT NULL,
            entity TEXT NOT NULL,
            x real NOT NULL,
            y real NOT NULL,
            z real NOT NULL
        );
        )";

        auto setoperationtype = R"(
        CREATE TABLE IF NOT EXISTS setoperationtype (
            deleted INTEGER NOT NULL DEFAULT 0,
            timestamp UNSIGNED BIG INT NOT NULL,
            entity TEXT NOT NULL,
            type INTEGER NOT NULL CHECK (type = 0 or type = 1 or type = 2)
        );
        )";

        auto document = R"(
        CREATE TABLE IF NOT EXISTS document (
            deleted INTEGER NOT NULL DEFAULT 0,
            timestamp UNSIGNED BIG INT NOT NULL,
            entity TEXT NOT NULL,
            type integer NOT NULL,
            document TEXT NOT NULL
        );
        )";

        auto name = R"(
        CREATE TABLE IF NOT EXISTS name (
            deleted INTEGER NOT NULL DEFAULT 0,
            timestamp UNSIGNED BIG INT NOT NULL,
            entity TEXT NOT NULL,
            name TEXT NOT NULL
        );
        )";

        return { edge, radius, coord, setoperationtype, document, name };
    }

    std::vector<std::string> DatabaseImpl::indices() {
        auto edges = "CREATE INDEX edge_index ON edge(entity);";
        auto radius = "CREATE INDEX radius_index ON radius(entity);";
        auto coord = "CREATE INDEX coord_index ON coord(entity);";
        auto setOpType = "CREATE INDEX setoperationtype_index ON setoperationtype(entity);";
        auto document = "CREATE INDEX document_index ON document(document);";
        auto name = "CREATE INDEX name_index ON name(entity);";

        return { edges, radius, coord, setOpType, document, name };
    }

    Name DatabaseImpl::preferredNameOf(const Name &left, const Name &right) {
        if (left.get() < right.get()) {
            return left;
        }
        return right;
    }
}

