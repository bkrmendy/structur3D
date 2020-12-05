//
// Created by Berci on 2020. 12. 05..
//

#include "gtest/gtest.h"
#include "rapidcheck.h"
#include "rapidcheck/gtest.h"

#include <db/ColumnDB.h>
#include <data/Timestamp.h>
#include <utils/approximately_equal.h>

#include "db/utils/db_utils.h"
#include "db/attribute_priority.h"

namespace rc {
    template<>
    struct Arbitrary<S3D::Coord> {
        static Gen<S3D::Coord> arbitrary() {
            return gen::build<S3D::Coord>(
                    gen::set(&S3D::Coord::x, gen::inRange(0, 1'000)),
                    gen::set(&S3D::Coord::y, gen::inRange(0, 1'000)),
                    gen::set(&S3D::Coord::z, gen::inRange(0, 1'000)));
        }
    };

    template<>
    struct Arbitrary<S3D::Name> {
        static Gen<S3D::Name> arbitrary() {
            return gen::construct<S3D::Name>(
                    gen::nonEmpty(
                            gen::container<std::string>(
                                    gen::elementOf(S3D::Name::allowedCharacters))));
        }
    };

    template<>
    struct Arbitrary<S3D::SetOperationType> {
        static Gen<S3D::SetOperationType> arbitrary() {
            return gen::element(S3D::SetOperationType::Union,
                                S3D::SetOperationType::Intersection);
        }
    };
}

RC_GTEST_PROP(ColumnDBPropertyTests,
              RadiusCRDTProperty,
              ()) {
    auto entries = *rc::gen::container<
            std::vector<
                    std::tuple<
                            S3D::Timestamp,
                            float>>>(rc::gen::tuple(rc::gen::arbitrary<S3D::Timestamp>(), rc::gen::positive<float>()));
    RC_PRE(entries.size() > 0);
    auto db = S3D::ColumnDB{shared_connection_in_memory(false)};

    auto entity = S3D::IDFactory()();

    for (const auto& entry : entries) {
        db.radius.upsert(entity, S3D::Radius(std::get<1>(entry)), std::get<0>(entry));
    }

    auto radiusFromDb = db.radius.latest(entity);

    auto latest_entry = std::max_element(entries.begin(), entries.end(), [](auto a, auto b){
        if (std::get<0>(a) == std::get<0>(b)) {
            return std::get<1>(a) < std::get<1>(b);
        }
        return std::get<0>(a) < std::get<0>(b);
    });

    auto latestRadius = S3D::Radius{std::get<1>(*latest_entry)};

    RC_ASSERT(radiusFromDb.has_value());
    RC_ASSERT(S3D::approximately_equal(radiusFromDb->magnitude(), latestRadius.magnitude(), 1));
}

RC_GTEST_PROP(ColumnDBPropertyTests,
              CoordinateCRDTProperty,
              (const std::vector<std::tuple<S3D::Timestamp, S3D::Coord>>& entries)) {
    RC_PRE(entries.size() > 0);

    auto db = S3D::ColumnDB{shared_connection_in_memory(false)};

    auto entity = S3D::IDFactory()();

    for (const auto& entry : entries) {
        db.coordinate.upsert(entity, std::get<1>(entry), std::get<0>(entry));
    }

    auto coordFromDb = db.coordinate.latest(entity);

    S3D::Coord farthestCoord = std::get<1>(entries.at(0));
    auto maxCoordTimestamp = std::get<0>(entries.at(0));
    for (const auto& row : entries) {
        auto coord = std::get<1>(row);
        auto currentTimestamp = std::get<0>(row);

        if (maxCoordTimestamp == currentTimestamp) {
            farthestCoord = preferred_coord(farthestCoord, coord);
        } else if (currentTimestamp > maxCoordTimestamp) {
            farthestCoord = coord;
            maxCoordTimestamp = currentTimestamp;
        }
    }

    RC_ASSERT(coordFromDb.has_value());
    RC_ASSERT(S3D::approximately_equal(coordFromDb.value().x, farthestCoord.x, 1));
    RC_ASSERT(S3D::approximately_equal(coordFromDb.value().y, farthestCoord.y, 1));
    RC_ASSERT(S3D::approximately_equal(coordFromDb.value().z, farthestCoord.z, 1));
}

RC_GTEST_PROP(ColumnDBPropertyTests,
              NameCRDTProperty,
              (const std::vector<std::tuple<S3D::Timestamp, S3D::Name>>& entries)) {
    RC_PRE(entries.size() > 0);
    auto db = S3D::ColumnDB{shared_connection_in_memory(false)};

    S3D::IDFactory factory = S3D::IDFactory();

    S3D::ID entity = factory();

    for (const auto& entry : entries){
        RC_PRE(std::get<0>(entry) < 9223372036854775807);
        db.name.upsert(entity,std::get<1>(entry), std::get<0>(entry));
    }

    const auto nameFromDb = db.name.latest(entity);

    S3D::Timestamp latestTimestamp = std::get<0>(entries.at(0));
    for (const auto& entry : entries) {
        latestTimestamp = std::max(latestTimestamp, std::get<0>(entry));
    }

    std::vector<std::tuple<S3D::Timestamp, S3D::Name>> latestEntries{};

    std::copy_if(entries.begin(), entries.end(), std::back_inserter(latestEntries), [&latestTimestamp](auto entry) {
        return std::get<0>(entry) == latestTimestamp;
    });

    S3D::Name latestName = std::get<1>(latestEntries.at(0));
    for (const auto& entry : latestEntries) {
        latestName = preferred_name(std::get<1>(entry), latestName);
    }

    RC_ASSERT(nameFromDb == latestName);
}

RC_GTEST_PROP(ColumnDBPropertyTests,
              SetOperationTypeCRDTProperty,
              (const std::vector<std::tuple<S3D::Timestamp, S3D::SetOperationType>>& entries)) {
    RC_PRE(entries.size() > 0);
    auto db = S3D::ColumnDB{shared_connection_in_memory(false)};

    S3D::IDFactory factory = S3D::IDFactory();

    S3D::ID entity = factory();

    for (const auto& entry : entries){
        RC_PRE(std::get<0>(entry) < 9223372036854775807);
        db.setOperationType.upsert(entity,std::get<1>(entry), std::get<0>(entry));
    }

    const auto fromDb = db.setOperationType.latest(entity);

    S3D::Timestamp latestTimestamp = std::get<0>(entries.at(0));
    for (const auto& entry : entries) {
        latestTimestamp = std::max(latestTimestamp, std::get<0>(entry));
    }

    std::vector<std::tuple<S3D::Timestamp, S3D::SetOperationType>> latestEntries{};

    std::copy_if(entries.begin(), entries.end(), std::back_inserter(latestEntries), [&latestTimestamp](auto entry) {
        return std::get<0>(entry) == latestTimestamp;
    });

    S3D::SetOperationType latest = std::get<1>(latestEntries.at(0));
    for (const auto& entry : latestEntries) {
        latest = preferred_setoperation_type(std::get<1>(entry), latest);
    }

    RC_ASSERT(fromDb == latest);
}



