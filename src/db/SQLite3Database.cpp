//
//  SQLite3Database.cpp
//  cross-platform-game
//
//  Created by Berci on 2020. 11. 11..
//  Copyright © 2020. Berci. All rights reserved.
//

#include <string>

#include "db/SQLite3Database.h"
#include "generated/tables.h"
#include "data/Timestamp.h"

#include "boost/uuid/uuid_io.hpp"

#include "sqlpp11/remove.h"
#include "sqlpp11/insert.h"
#include "sqlpp11/transaction.h"
#include "sqlpp11/select.h"

namespace S3D {

int SQLite3Database::from_operationType(const SetOperationType &type) {
    switch (type) {
        case SetOperationType::Union: return 0;
        case SetOperationType::Intersection: return 1;
        case SetOperationType::Subtraction: return 2;
    }
}

SetOperationType SQLite3Database::to_operationType(int typeId) {
    if (typeId == 0) { return SetOperationType::Union; }
    if (typeId == 1) { return SetOperationType::Intersection; }
    if (typeId == 2) { return SetOperationType::Subtraction; }
    throw 0; // todo
}

void SQLite3Database::upsertI(const ID& entity, const Coord& coord, bool deleted) {
    std::string eid = to_string(entity);
    uint64_t stamp = make_timestamp();

    auto tx = sqlpp::start_transaction(*db);
    Schema::Coord removeCoord;
    this->db->operator()(
        remove_from(removeCoord)
            .where(removeCoord.entity == eid
                   && removeCoord.timestamp <= stamp
                   || removeCoord.deleted == 1));

    Schema::Coord insertCoord;
    this->db->operator()(
        insert_into(insertCoord)
             .set(insertCoord.deleted = is_deleted(deleted),
                  insertCoord.timestamp = stamp,
                  insertCoord.entity = eid,
                  insertCoord.x = coord.x,
                  insertCoord.y = coord.y,
                  insertCoord.z = coord.z));

    tx.commit();
}

void SQLite3Database::upsertI(const ID &entity, const SetOperationType &type, bool deleted) {
    std::string eid = to_string(entity);
    uint64_t stamp = make_timestamp();

    auto tx = sqlpp::start_transaction(*db);
    Schema::Setoperationtype removeOp;
    this->db->operator()(
        remove_from(removeOp)
            .where(removeOp.entity == eid
                   && removeOp.timestamp <= stamp
                   || removeOp.deleted == 1));

    Schema::Setoperationtype insertOp;
    this->db->operator()(
        insert_into(insertOp)
             .set(insertOp.deleted = is_deleted(deleted),
                  insertOp.timestamp = stamp,
                  insertOp.entity = eid,
                  insertOp.type = from_operationType(type)));

    tx.commit();

}

void SQLite3Database::connectI(const ID &entity, const ID &entity_to, bool deleted) {
    std::string eid = to_string(entity);
    std::string eid_to = to_string(entity_to);
    uint64_t stamp = make_timestamp();

    auto tx = sqlpp::start_transaction(*db);
    Schema::Edge removeEdge;
    this->db->operator()(
        remove_from(removeEdge)
            .where(removeEdge.entity == eid
                   && removeEdge.entityTo == eid_to
                   || removeEdge.deleted == 1));

    Schema::Edge insertEdge;
    this->db->operator()(
        insert_into(insertEdge)
            .set(insertEdge.entity = eid,
                 insertEdge.entityTo = eid_to,
                 insertEdge.timestamp = stamp,
                 insertEdge.deleted = is_deleted(deleted)));

    tx.commit();
}

void SQLite3Database::createI(const ID &entity, const NodeType type, const ID &document, bool deleted) {
    std::string eid = to_string(entity);
    std::string doc = to_string(document);
    uint64_t stamp = make_timestamp();

    auto tx = sqlpp::start_transaction(*db);

    Schema::Document removeDoc;
    this->db->operator()(
        remove_from(removeDoc)
            .where(removeDoc.entity == eid
                   && removeDoc.document == doc
                   || removeDoc.deleted == 1));

    Schema::Document updateDoc;
    this->db->operator()(
        insert_into(updateDoc)
            .set(updateDoc.entity = eid,
                 updateDoc.type = to_integral(type),
                 updateDoc.document = doc,
                 updateDoc.timestamp = stamp,
                 updateDoc.deleted = is_deleted(deleted)));

    tx.commit();
}

void SQLite3Database::upsert(const ID &entity, const Coord &coord) {
    this->upsertI(entity, coord, false);
}

void SQLite3Database::retract(const ID &entity, const Coord &coord) {
    this->upsertI(entity, coord, false);
}

void SQLite3Database::upsertI(const ID &entity, const RADIUS &radius, bool deleted) {
    std::string eid = to_string(entity);
    uint64_t stamp = make_timestamp();

    auto tx = sqlpp::start_transaction(*db);

    Schema::Radius removeRadius;
    this->db->operator()(
        remove_from(removeRadius)
            .where(removeRadius.entity == eid
                   && removeRadius.timestamp <= stamp));

    Schema::Radius insertRadius;
    this->db->operator()(
        insert_into(insertRadius)
            .set(insertRadius.deleted = is_deleted(deleted),
                 insertRadius.timestamp = stamp,
                 insertRadius.entity = eid,
                 insertRadius.magnitude = radius));

    tx.commit();
}

void SQLite3Database::upsert(const ID &entity, const RADIUS &radius) {
    upsertI(entity, radius, false);
}

void SQLite3Database::retract(const ID &entity, const RADIUS &radius) {
    upsertI(entity, radius, false);
}

void SQLite3Database::upsert(const ID &entity, const SetOperationType &type) {
    upsertI(entity, type, false);
}

void SQLite3Database::retract(const ID &entity, const SetOperationType &type) {
    upsertI(entity, type, true);
}

void SQLite3Database::connect(const ID &entity, const ID &entity_to) {
    connectI(entity, entity_to, false);
}

void SQLite3Database::disconnect(const ID &entity, const ID &entity_to) {
    connectI(entity, entity_to, true);
}

void SQLite3Database::create(const ID &entity, const NodeType type, const ID &document) {
    createI(entity, type, document, false);
}

void SQLite3Database::remove(const ID &entity, const ID &document) {
    createI(entity, NodeType::Sphere, document, true);
}

std::vector<DocumentWithName> SQLite3Database::documents() {
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
                .limit(1u);

        auto mayBeName = this->db->operator()(nameLookup);

        if (not mayBeName.empty()) {
            std::stringstream stream{uid.document};
            ID uid;
            stream >> uid;
            names.push_back(DocumentWithName{uid, mayBeName.front().name});
        }
    }

    return names;
}

std::vector<IDWithType> SQLite3Database::entites(const ID &of_document) {
    std::string did = to_string(of_document);
    Schema::Document doc;
    auto lookup =
        dynamic_select(*db)
            .columns(doc.entity, doc.type)
            .from(doc)
            .where(doc.deleted == 0 && doc.document == did);

    std::vector<IDWithType> res;
    for (auto& row : db->operator()(lookup)) {
        std::stringstream stream{row.entity};
        ID uid;
        stream >> uid;

        res.push_back(IDWithType{uid, from_integral(row.type) });
    }

    return res;
}

std::vector<ID> SQLite3Database::edges(const ID &of_entity) {
    std::string eid = to_string(of_entity);
    Schema::Edge edge;
    auto lookup
        = dynamic_select(*db)
            .columns(edge.entityTo)
            .from(edge)
            .where(edge.deleted == 0 && edge.entity == eid);

    std::vector<ID> res;
    for (auto& row : db->operator()(lookup)) {
        std::stringstream stream{row.entityTo};
        ID uid;
        stream >> uid;
        res.push_back(uid);
    }

    return res;
}

std::optional<Sphere> SQLite3Database::sphere(const ID &from) {
    std::string eid = to_string(from);
    Schema::Radius radius;
    auto radiusLookup
        = dynamic_select(*db)
            .columns(radius.magnitude)
            .from(radius)
            .where(radius.deleted == 0 && radius.entity == eid)
            .order_by(radius.timestamp.desc())
            .limit(1u);

    auto radiusRes = db->operator()(radiusLookup);

    if (radiusRes.empty()) {
        return std::nullopt;
    }

    Schema::Coord coord;
    auto coordLookup
        = dynamic_select(*db)
            .columns(coord.x, coord.y, coord.z)
            .from(coord)
            .where(coord.deleted == 0 && coord.entity == eid)
            .order_by(coord.timestamp.desc())
            .limit(1u);

    auto coordRes = db->operator()(coordLookup);

    if (coordRes.empty()) {
        return std::nullopt;
    }

    return Sphere(from,
                  Coord(coordRes.front().x,
                        coordRes.front().y,
                        coordRes.front().z),
                  radiusRes.front().magnitude);
}

std::optional<SetOp> SQLite3Database::setop(const ID &from) {
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

    return SetOp(from, to_operationType(res.front().type));
}

std::vector<std::string> SQLite3Database::schema() {
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
        type INTEGER NOT NULL CHECK (type = 0 || type = 1 || type = 2)
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

}

