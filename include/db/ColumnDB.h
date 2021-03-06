//
// Created by Berci on 2020. 12. 04..
//

#ifndef STRUCTUR3D_BASE_COLUMNDB_H
#define STRUCTUR3D_BASE_COLUMNDB_H

#include "Database.h"
#include "sqlpp11/sqlite3/sqlite3.h"

#include "db/column/Column.h"
#include "db/attribute_priority.h"

namespace S3D {
    class ColumnDB {
        std::shared_ptr<sql::connection> db_;

        constexpr static const char* schema = R"(
            CREATE TABLE IF NOT EXISTS attribute (
                deleted INTEGER NOT NULL DEFAULT 0 CHECK (deleted = 0 OR deleted = 1),
                colid INTEGER NOT NULL,
                timestamp UNSIGNED BIG INT NOT NULL,
                entity TEXT NOT NULL,
                attrib BLOB NOT NULL,
                size UNSIGNED INT NOT NULL
            );
        )";

        constexpr static const char* index_entity = R"(CREATE INDEX attribute_entity_index ON attribute(entity);)";
        constexpr static const char* index_timestamp = R"(CREATE INDEX attribute_timestamp_index ON attribute(entity);)";
        constexpr static const char* index_colid = R"(CREATE INDEX attribute_colid_index ON attribute(colid);)";

    public:
        const Column<ColumnAttribute::Coordinate, Coord> coordinate;
        const Column<ColumnAttribute::Radius, Radius> radius;
        const Column<ColumnAttribute::Name, Name> name;
        const Column<ColumnAttribute::SetOperationType, SetOperationType> setOperationType;

        ColumnDB() = delete;

        explicit ColumnDB(std::shared_ptr<sql::connection> db)
            : db_{std::move(db)}
            , coordinate{db_, preferred}
            , radius{db_, preferred}
            , name{db_, preferred}
            , setOperationType{db_, preferred} {
            db_->execute(schema);
            db_->execute(index_entity);
            db_->execute(index_timestamp);
            db_->execute(index_colid);
        }
    };
}

#endif //STRUCTUR3D_BASE_COLUMNDB_H
