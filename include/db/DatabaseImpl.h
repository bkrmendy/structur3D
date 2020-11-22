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

    static std::vector<std::string> tables();
    static std::vector<std::string> indices();

    explicit DatabaseImpl(std::unique_ptr<sql::connection> db) : db{std::move(db)} {
        for (const auto& column : DatabaseImpl::tables()) {
            this->db->execute(column);
        }

        for (const auto& index : DatabaseImpl::indices()) {
            this->db->execute(index);
        }
    }

    constexpr int is_deleted(bool deleted) { return deleted ? 1 : 0; }

    void upsertI(const ID& entity, const Coord& coord, Timestamp timestamp, bool deleted);

    void upsertI(const ID& entity, const Radius& radius, Timestamp timestamp, bool deleted);

    void upsertI(const ID& entity, const SetOperationType& type, Timestamp timestamp, bool deleted);

    void upsertI(const ID& entity, const std::string& name, Timestamp timestamp, bool deleted);

    void connectI(const ID& entity, const ID& entity_to, Timestamp timestamp, bool deleted);

    void createI(const ID& entity, const NodeType& type, const ID& document, Timestamp timestamp, bool deleted);

    void upsert(const ID& entity, const Coord& coord, Timestamp timestamp) override;
    void retract(const ID& entity, const Coord& coord, Timestamp timestamp) override;

    void upsert(const ID& entity, const Radius& radius, Timestamp timestamp) override;
    void retract(const ID& entity, const Radius& radius, Timestamp timestamp) override;

    void upsert(const ID& entity, const SetOperationType& type, Timestamp timestamp) override;
    void retract(const ID& entity, const SetOperationType& type, Timestamp timestamp) override;

    void upsert(const ID& entity, const std::string &name, Timestamp timestamp) override;
    void retract(const ID& entity, const std::string &name, Timestamp timestamp) override;

    void connect(const ID& entity, const ID& entity_to, Timestamp timestamp) override;
    void disconnect(const ID& entity, const ID& entity_to, Timestamp timestamp) override;

    void create(const ID& entity, const NodeType& type, const ID& document, Timestamp timestamp) override;
    void remove(const ID& entity, const ID& document, Timestamp timestamp) override;

    std::vector<DocumentWithName> documents() override;
    std::vector<IDWithType> entities(const ID& of_document) override;
    std::vector<ID> edges(const ID& entity) override;

    std::optional<Sphere> sphere(const ID& from) override;
    std::optional<SetOp> setop(const ID& from) override;
};

}


#endif /* SQLite3Database_h */
