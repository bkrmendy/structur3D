//
//  DatabaseImpl.h
//  cross-platform-game
//
//  Created by Berci on 2020. 11. 11..
//  Copyright © 2020. Berci. All rights reserved.
//

#ifndef SQLite3Database_h
#define SQLite3Database_h

#include <iostream>
#include <memory>

#include "Database.h"
#include "sqlpp11/sqlite3/sqlite3.h"

namespace sql = sqlpp::sqlite3;

namespace S3D {

class DatabaseImpl : public Database {
    std::unique_ptr<sql::connection> db;
public:
    static DatabaseImpl inMemory(bool debug) {
        sql::connection_config config;
        config.path_to_database = ":memory:";
        config.flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
        config.debug = debug;

        return DatabaseImpl{std::make_unique<sql::connection>(config)};
    }

    static DatabaseImpl atPath(const char* path) {
        sql::connection_config config;
        config.path_to_database = path;
        config.flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;

        return DatabaseImpl{std::make_unique<sql::connection>(config)};
    }

    static std::vector<std::string> schema();

    explicit DatabaseImpl(std::unique_ptr<sql::connection> db) : db{std::move(db)} {
        for (auto& column : DatabaseImpl::schema()) {
            this->db->execute(column);
        }
    }

    int is_deleted(bool deleted) { return deleted ? 1 : 0; }

    void upsertI(const ID& entity, const Coord& coord, bool deleted);

    void upsertI(const ID& entity, const RADIUS& radius, bool deleted);

    void upsertI(const ID& entity, const SetOperationType& type, bool deleted);

    void upsertI(const ID& entity, const std::string& name, bool deleted);

    void connectI(const ID& entity, const ID& entity_to, bool deleted);

    void createI(const ID& entity, const NodeType type, const ID& document, bool deleted);

    void upsert(const ID& entity, const Coord& coord) override;
    void retract(const ID& entity, const Coord& coord) override;

    void upsert(const ID& entity, const RADIUS& radius) override;
    void retract(const ID& entity, const RADIUS& radius) override;

    void upsert(const ID& entity, const SetOperationType& type) override;
    void retract(const ID& entity, const SetOperationType&) override;

    void upsert(const ID &, const std::string &name) override;
    void retract(const ID &, const std::string &name) override;

    void connect(const ID& entity, const ID& entity_to) override;
    void disconnect(const ID& entity, const ID& entity_to) override;

    void create(const ID& entity, const NodeType type, const ID& document) override;
    void remove(const ID& entity, const ID& document) override;

    std::vector<DocumentWithName> documents() override;
    std::vector<IDWithType> entities(const ID& of_document) override;
    std::vector<ID> edges(const ID& entity) override;

    std::optional<Sphere> sphere(const ID& from) override;
    std::optional<SetOp> setop(const ID& from) override;
};

}


#endif /* SQLite3Database_h */
