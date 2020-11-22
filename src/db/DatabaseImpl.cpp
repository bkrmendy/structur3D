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

#include "sqlpp11/remove.h"
#include "sqlpp11/insert.h"
#include "sqlpp11/transaction.h"
#include "sqlpp11/select.h"

namespace S3D {

    void DatabaseImpl::upsertI(const ID& entity, const Coord& coord, Timestamp timestamp, bool deleted) {
        std::string eid = to_string(entity);

        auto tx = sqlpp::start_transaction(*db);
        Schema::Coord removeCoord;
        (*db)(
            remove_from(removeCoord)
                .where(removeCoord.entity == eid
                       && removeCoord.timestamp <= timestamp
                       || removeCoord.deleted == is_deleted(true)));

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
        this->db->operator()(
            remove_from(removeOp)
                .where(removeOp.entity == eid
                       && removeOp.timestamp <= timestamp
                       || removeOp.deleted == 1));

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

    void DatabaseImpl::upsertI(const ID &entity, const std::string &name, Timestamp timestamp, bool deleted) {
        std::string eid = to_string(entity);

        auto tx = sqlpp::start_transaction(*db);
        Schema::Name removeName;
        this->db->operator()(
                remove_from(removeName)
                        .where(removeName.entity == eid
                               && removeName.timestamp <= timestamp
                               || removeName.deleted == 1));

        Schema::Name insertName;
        this->db->operator()(
                insert_into(insertName)
                    .set(insertName.entity = eid,
                         insertName.timestamp = timestamp,
                         insertName.deleted = is_deleted(deleted),
                         insertName.name = name));

        tx.commit();
    }

    void DatabaseImpl::connectI(const ID &entity, const ID &entity_to, Timestamp timestamp, bool deleted) {
        std::string eid = to_string(entity);
        std::string eid_to = to_string(entity_to);

        auto tx = sqlpp::start_transaction(*db);
        Schema::Edge removeEdge;
        this->db->operator()(
            remove_from(removeEdge)
                .where(removeEdge.entity == eid
                       && removeEdge.entityTo == eid_to
                       && removeEdge.timestamp < timestamp
                       || removeEdge.deleted == 1));

        Schema::Edge insertEdge;
        this->db->operator()(
            insert_into(insertEdge)
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
                       || removeDoc.deleted == 1));

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

    void DatabaseImpl::upsertI(const ID &entity, const RADIUS &radius, Timestamp timestamp, bool deleted) {
        std::string eid = to_string(entity);

        auto tx = sqlpp::start_transaction(*db);

        Schema::Radius removeRadius;
        (*db)(
            remove_from(removeRadius)
                .where(removeRadius.entity == eid
                       && removeRadius.timestamp <= timestamp
                       || removeRadius.deleted == is_deleted(true)));

        Schema::Radius insertRadius;
        (*db)(
            insert_into(insertRadius)
                .set(insertRadius.deleted = is_deleted(deleted),
                     insertRadius.timestamp = timestamp,
                     insertRadius.entity = eid,
                     insertRadius.magnitude = radius.magnitude()));

        tx.commit();
    }

    void DatabaseImpl::upsert(const ID &entity, const RADIUS &radius, Timestamp timestamp) {
        upsertI(entity, radius, timestamp, false);
    }

    void DatabaseImpl::retract(const ID &entity, const RADIUS &radius, Timestamp timestamp) {
        upsertI(entity, radius, timestamp, true);
    }

    void DatabaseImpl::upsert(const ID &entity, const SetOperationType &type, Timestamp timestamp) {
        upsertI(entity, type, timestamp, false);
    }

    void DatabaseImpl::upsert(const ID &eid, const std::string &name, Timestamp timestamp) {
        upsertI(eid, name, timestamp, false);
    }

    void DatabaseImpl::retract(const ID &eid, const std::string &name, Timestamp timestamp) {
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
            dynamic_select(*db)
                .columns(doc.document)
                .flags(sqlpp::distinct)
                .from(doc)
                .where(doc.deleted == 0)
                .group_by(doc.document);

        std::vector<DocumentWithName> names{};

        for (const auto& uid : db->operator()(docLookup)) {
            Schema::Name name;
            auto nameLookup
                = dynamic_select(*db)
                    .columns(name.name)
                    .from(name)
                    .where(name.deleted == 0 && name.entity == uid.document)
                    .order_by(name.timestamp.desc())
                    .limit(1U);

            auto mayBeName = this->db->operator()(nameLookup);

            if (not mayBeName.empty()) {
                std::stringstream stream{uid.document};
                ID uid_temp;
                stream >> uid_temp;
                names.emplace_back(uid_temp, mayBeName.front().name);
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

    template <typename Table>
    auto latestTimeStampQuery(Table table, const std::string& eid) {
        return select(max(table.timestamp))
                    .from(table)
                    .where(table.deleted == 0 && table.entity == eid)
                    .group_by(table.timestamp);
    }

    std::vector<ID> DatabaseImpl::edges(const ID &of_entity) {
        std::string eid = to_string(of_entity);
        Schema::Edge edge;
        auto lookup
            = dynamic_select(*db)
                .columns(edge.entityTo)
                .from(edge)
                .where(edge.deleted == 0 && edge.entity == eid);

        std::vector<ID> res;
        for (const auto& row : db->operator()(lookup)) {
            std::stringstream stream{row.entityTo};
            ID uid;
            stream >> uid;
            res.push_back(uid);
        }

        return res;
    }

    std::optional<Sphere> DatabaseImpl::sphere(const ID &from) {
        std::string eid = to_string(from);

        Schema::Document documentLookup;
        auto lookup
            = (*db)(select(documentLookup.entity)
                        .from(documentLookup)
                        .where(documentLookup.deleted == 0 && documentLookup.entity == eid)
                        .order_by(documentLookup.timestamp.desc())
                        .limit(1U));

        if (lookup.empty()) {
            return std::nullopt;
        }

        Schema::Radius radiusTable;
        auto radiusRes
            = (*db)(select(radiusTable.magnitude, radiusTable.timestamp)
                        .from(radiusTable)
                        .where(radiusTable.deleted == 0
                                && radiusTable.entity == eid));

        if (radiusRes.empty()) {
            return std::nullopt;
        }

        auto maxTimestamp = radiusRes.front().timestamp;
        float maxRadius = radiusRes.front().magnitude;
        for (const auto& radius : radiusRes) {
            if (radius.timestamp == maxTimestamp
                && radius.magnitude > maxRadius) {
                maxRadius = radius.magnitude;
                maxTimestamp = radius.timestamp;
            } else if (radius.timestamp > maxTimestamp) {
                maxRadius = radius.magnitude;
                maxTimestamp = radius.timestamp;
            }
        }

        Schema::Coord coordTable;
        auto coordRes
            = (*db)(select(coordTable.x, coordTable.y, coordTable.z, coordTable.timestamp)
                .from(coordTable)
                .where(coordTable.deleted == 0
                        && coordTable.entity == eid));

        if (coordRes.empty()) {
            return std::nullopt;
        }

        Coord farthestCoord = Coord{(float)coordRes.front().x,
                                    (float)coordRes.front().y,
                                    (float)coordRes.front().z};

        auto maxCoordTimestamp = coordRes.front().timestamp;
        for (const auto& row : coordRes) {
            auto coord = Coord{(float)row.x,
                               (float)row.y,
                               (float)row.z};

            if (maxCoordTimestamp == row.timestamp
                && distance_from_origin(coord) > distance_from_origin(farthestCoord)) {
                farthestCoord = coord;
                maxCoordTimestamp = row.timestamp;
            } else if (row.timestamp > maxCoordTimestamp) {
                farthestCoord = coord;
                maxCoordTimestamp = row.timestamp;
            }
        }

        return Sphere(from,
                      farthestCoord,
                      RADIUS(maxRadius));
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
        timestamp INTEGER NOT NULL,
        entity TEXT NOT NULL,
        entity_to TEXT NOT NULL
        );
        )";

        auto radius = R"(
        CREATE TABLE IF NOT EXISTS radius (
             deleted INTEGER NOT NULL DEFAULT 0,
             timestamp INTEGER NOT NULL,
             entity TEXT NOT NULL,
             magnitude REAL NOT NULL CHECK (magnitude >= 0)
         );
        )";

        auto coord = R"(
        CREATE TABLE IF NOT EXISTS coord (
            deleted INTEGER NOT NULL DEFAULT 0,
            timestamp INTEGER NOT NULL,
            entity TEXT NOT NULL,
            x real NOT NULL,
            y real NOT NULL,
            z real NOT NULL
        );
        )";

        auto setoperationtype = R"(
        CREATE TABLE IF NOT EXISTS setoperationtype (
            deleted INTEGER NOT NULL DEFAULT 0,
            timestamp INTEGER NOT NULL,
            entity TEXT NOT NULL,
            type INTEGER NOT NULL CHECK (type = 0 or type = 1 or type = 2)
        );
        )";

        auto document = R"(
        CREATE TABLE IF NOT EXISTS document (
            deleted INTEGER NOT NULL DEFAULT 0,
            timestamp INTEGER NOT NULL,
            entity TEXT NOT NULL,
            type integer NOT NULL,
            document TEXT NOT NULL
        );
        )";

        auto name = R"(
        CREATE TABLE IF NOT EXISTS name (
            deleted INTEGER NOT NULL DEFAULT 0,
            timestamp INTEGER NOT NULL,
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
}

