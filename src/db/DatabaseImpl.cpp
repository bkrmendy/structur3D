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

#include "boost/uuid/uuid_io.hpp"

#include "sqlpp11/remove.h"
#include "sqlpp11/insert.h"
#include "sqlpp11/transaction.h"
#include "sqlpp11/select.h"

namespace S3D {

void DatabaseImpl::upsertI(const ID& entity, const Coord& coord, bool deleted) {
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

void DatabaseImpl::upsertI(const ID &entity, const SetOperationType &type, bool deleted) {
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
    auto typeAsInteger = from_operationType(type);
    this->db->operator()(
        insert_into(insertOp)
             .set(insertOp.deleted = is_deleted(deleted),
                  insertOp.timestamp = stamp,
                  insertOp.entity = eid,
                  insertOp.type = typeAsInteger));

    tx.commit();
}

void DatabaseImpl::upsertI(const ID &entity, const std::string &name, bool deleted) {
    std::string eid = to_string(entity);
    uint64_t stamp = make_timestamp();

    auto tx = sqlpp::start_transaction(*db);
    Schema::Name removeName;
    this->db->operator()(
            remove_from(removeName)
                    .where(removeName.entity == eid
                           && removeName.timestamp <= stamp
                           || removeName.deleted == 1));

    Schema::Name insertName;
    this->db->operator()(
            insert_into(insertName)
                .set(insertName.entity = eid,
                     insertName.timestamp = stamp,
                     insertName.deleted = is_deleted(deleted),
                     insertName.name = name));

    tx.commit();
}

void DatabaseImpl::connectI(const ID &entity, const ID &entity_to, bool deleted) {
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

void DatabaseImpl::createI(const ID &entity, const NodeType type, const ID &document, bool deleted) {
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

void DatabaseImpl::upsert(const ID &entity, const Coord &coord) {
    this->upsertI(entity, coord, false);
}

void DatabaseImpl::retract(const ID &entity, const Coord &coord) {
    this->upsertI(entity, coord, false);
}

void DatabaseImpl::upsertI(const ID &entity, const RADIUS &radius, bool deleted) {
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

void DatabaseImpl::upsert(const ID &entity, const RADIUS &radius) {
    upsertI(entity, radius, false);
}

void DatabaseImpl::retract(const ID &entity, const RADIUS &radius) {
    upsertI(entity, radius, false);
}

void DatabaseImpl::upsert(const ID &entity, const SetOperationType &type) {
    upsertI(entity, type, false);
}

void DatabaseImpl::upsert(const ID &eid, const std::string &name) {
    upsertI(eid, name, false);
}

void DatabaseImpl::retract(const ID &eid, const std::string &name) {
    upsertI(eid, name, true);
}

void DatabaseImpl::retract(const ID &entity, const SetOperationType &type) {
    upsertI(entity, type, true);
}

void DatabaseImpl::connect(const ID &entity, const ID &entity_to) {
    connectI(entity, entity_to, false);
}

void DatabaseImpl::disconnect(const ID &entity, const ID &entity_to) {
    connectI(entity, entity_to, true);
}

void DatabaseImpl::create(const ID &entity, const NodeType type, const ID &document) {
    createI(entity, type, document, false);
}

void DatabaseImpl::remove(const ID &entity, const ID &document) {
    createI(entity, NodeType::Sphere, document, true);
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
                .limit(1u);

        auto mayBeName = this->db->operator()(nameLookup);

        if (not mayBeName.empty()) {
            std::stringstream stream{uid.document};
            ID uid_temp;
            stream >> uid_temp;
            names.push_back(DocumentWithName{uid_temp, mayBeName.front().name});
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
    for (auto& row : db->operator()(lookup)) {
        std::stringstream stream{row.entity};
        ID uid;
        stream >> uid;

        auto mayBeType = from_integral(row.type);
        if (mayBeType.has_value()) {
            res.push_back(IDWithType{uid, mayBeType.value() });
        }
    }

    return res;
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
    for (auto& row : db->operator()(lookup)) {
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
        = dynamic_select(*db)
                    .columns(documentLookup.entity)
                    .from(documentLookup)
                    .where(documentLookup.deleted == 0 && documentLookup.entity == eid)
                    .order_by(documentLookup.timestamp.desc())
                    .limit(1u);

    if (this->db->operator()(lookup).empty()) {
        return std::nullopt;
    }

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

std::vector<std::string> DatabaseImpl::schema() {
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

}

