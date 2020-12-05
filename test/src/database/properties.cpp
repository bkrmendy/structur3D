//
// Created by Berci on 2020. 11. 21..
//

#include <db/db_utils.h>
#include "gtest/gtest.h"
#include "rapidcheck.h"
#include "rapidcheck/gtest.h"

#include "utils/approximately_equal.h"
#include "data/Base.h"
#include "data/Coord.h"
#include "db/DatabaseImpl.h"


namespace rc {
    template <>
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
}

RC_GTEST_PROP(DatabasePropertyTests,
              RadiusCRDTProperty, ()) {
      auto entries = *rc::gen::container<
              std::vector<
                      std::tuple<
                              S3D::Timestamp,
                              float>>>(rc::gen::tuple(rc::gen::arbitrary<S3D::Timestamp>(), rc::gen::positive<float>()));
      RC_PRE(entries.size() > 0);
    auto db = S3D::DatabaseImpl{connection_in_memory(false)};

      S3D::IDFactory factory = S3D::IDFactory();

      S3D::ID document = factory();
      S3D::ID entity = factory();
      S3D::Coord coord = S3D::Coord{1,2,3};

      db.upsert(document, S3D::Name{"Test document"}, 0);

      db.create(entity, S3D::NodeType::Sphere, document, 0);
      db.upsert(entity, coord, 0);

      for (const auto& entry : entries) {
          db.upsert(entity, S3D::Radius(std::get<1>(entry)), std::get<0>(entry));
      }

      auto radiusFromDb = db.sphere(entity).value().radius.magnitude();

      auto latest_entry = std::max_element(entries.begin(), entries.end(), [](auto a, auto b){
          if (std::get<0>(a) == std::get<0>(b)) {
              return std::get<1>(a) < std::get<1>(b);
          }
          return std::get<0>(a) < std::get<0>(b);
      });

      auto latestRadius = S3D::Radius{std::get<1>(*latest_entry)}.magnitude();

      RC_ASSERT(S3D::approximately_equal(radiusFromDb, latestRadius, 1));
}

RC_GTEST_PROP(DatabasePropertyTests,
              CoordinateCRDTProperty,
              (const std::vector<std::tuple<S3D::Timestamp, S3D::Coord>>& entries)) {
              RC_PRE(entries.size() > 0);

              auto db = S3D::DatabaseImpl{connection_in_memory(false)};

              S3D::IDFactory factory = S3D::IDFactory();

              S3D::ID document = factory();
              S3D::ID entity = factory();
              auto radius = S3D::Radius{5};

              db.upsert(document, S3D::Name{"Test document"}, 0);

              db.create(entity, S3D::NodeType::Sphere, document, 0);
              db.upsert(entity, radius, 0);

              for (const auto& entry : entries) {
                  db.upsert(entity, std::get<1>(entry), std::get<0>(entry));
              }

              auto coordFromDb = db.sphere(entity).value().coord;

              S3D::Coord farthestCoord = std::get<1>(entries.at(0));
              auto maxCoordTimestamp = std::get<0>(entries.at(0));
              for (const auto& row : entries) {
                  auto coord = std::get<1>(row);
                  auto currentTimestamp = std::get<0>(row);

                  if (maxCoordTimestamp == currentTimestamp
                      && distance_from_origin(coord) > distance_from_origin(farthestCoord)) {
                      farthestCoord = coord;
                      maxCoordTimestamp = currentTimestamp;
                  } else if (currentTimestamp > maxCoordTimestamp) {
                      farthestCoord = coord;
                      maxCoordTimestamp = currentTimestamp;
                  }
              }

              RC_ASSERT(S3D::approximately_equal(coordFromDb.x, farthestCoord.x, 1));
              RC_ASSERT(S3D::approximately_equal(coordFromDb.y, farthestCoord.y, 1));
              RC_ASSERT(S3D::approximately_equal(coordFromDb.z, farthestCoord.z, 1));
}

RC_GTEST_PROP(DatabasePropertyTests,
              NameCRDTProperty,
              (const std::vector<std::tuple<S3D::Timestamp, S3D::Name>>& entries)) {
    RC_PRE(entries.size() > 0);
    auto db = S3D::DatabaseImpl{connection_in_memory(false)};

    S3D::IDFactory factory = S3D::IDFactory();

    S3D::ID document = factory();
    S3D::ID dummy = factory();
    db.create(dummy, S3D::NodeType::Sphere, document, 0);

    for (const auto& entry : entries){
        RC_PRE(std::get<0>(entry) < 9223372036854775807);
        db.upsert(document, std::get<1>(entry), std::get<0>(entry));
    }

    const auto nameFromDb = db.documents().at(0).name;

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
        latestName = S3D::DatabaseImpl::preferredNameOf(std::get<1>(entry), latestName);
    }

    RC_ASSERT(nameFromDb == latestName);
}

RC_GTEST_PROP(DatabasePropertyTests,
              PreferredNamePredicateCommutative,
              (const S3D::Name& left, const S3D::Name& right)) {
    RC_ASSERT(S3D::DatabaseImpl::preferredNameOf(left, right) == S3D::DatabaseImpl::preferredNameOf(right, left));
}

RC_GTEST_PROP(DatabasePropertyTests,
              ConnectDisconnectCRDTProperty,
              (const std::vector<std::tuple<S3D::Timestamp, bool>>& entries)) {
    RC_PRE(entries.size() > 0);
    auto db = S3D::DatabaseImpl{connection_in_memory(false)};
    S3D::IDFactory factory = S3D::IDFactory();

    S3D::ID document = factory();
    S3D::ID entityA = factory();
    S3D::ID entityB = factory();

    db.upsert(document, S3D::Name{"Test document"}, 0);
    db.create(entityA, S3D::NodeType::Sphere, document, 0);
    db.create(entityB, S3D::NodeType::Sphere, document, 0);

    auto latestEntry = entries.at(0);

    for (const auto& entry : entries) {
        RC_PRE(std::get<0>(entry) < 9223372036854775807);
        auto alive = std::get<1>(entry);
        auto timestamp = std::get<0>(entry);
        if (alive) {
            db.connect(entityA, entityB, timestamp);
        } else {
            db.disconnect(entityA, entityB, timestamp);
        }

        if (timestamp > std::get<0>(latestEntry)
            || (timestamp == std::get<0>(latestEntry) && alive)) {
            latestEntry = entry;
        }
    }

    auto edges = db.edges(entityA);
    if (std::get<1>(latestEntry)) {
        RC_ASSERT(edges.size() == 1);
    } else {
        RC_ASSERT(edges.empty());
    }
}