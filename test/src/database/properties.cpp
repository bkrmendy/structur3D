//
// Created by Berci on 2020. 11. 21..
//

#include "gtest/gtest.h"
#include "rapidcheck.h"
#include "rapidcheck/gtest.h"

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
}

/// https://stackoverflow.com/a/253874
bool approximatelyEqual(float a, float b, float epsilon)
{
    return fabs(a - b) <= ( (fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * epsilon);
}

RC_GTEST_PROP(DatabasePropertyTests,
              RadiusCRDTProperty, ()) {
      auto entries = *rc::gen::container<
              std::vector<
                      std::tuple<
                              S3D::Timestamp,
                              float>>>(rc::gen::tuple(rc::gen::arbitrary<S3D::Timestamp>(), rc::gen::inRange(0, 1'000'000)));
      RC_PRE(entries.size() > 0);
      auto db = S3D::DatabaseImpl::inMemory(false);

      S3D::IDFactory factory = S3D::IDFactory();

      S3D::ID document = factory();
      S3D::ID entity = factory();
      S3D::Coord coord = S3D::Coord{1,2,3};

      db.upsert(document, "Test document", 0);

      db.create(entity, S3D::NodeType::Sphere, document, 0);
      db.upsert(entity, coord, 0);

      for (const auto& entry : entries) {
          db.upsert(entity, S3D::RADIUS(std::get<1>(entry)), std::get<0>(entry));
      }

      auto radiusFromDb = db.sphere(entity).value().radius.magnitude();

      auto latest_entry = std::max_element(entries.begin(), entries.end(), [](auto a, auto b){
          if (std::get<0>(a) == std::get<0>(b)) {
              return std::get<1>(a) < std::get<1>(b);
          }
          return std::get<0>(a) < std::get<0>(b);
      });

      auto latestRadius = S3D::RADIUS{std::get<1>(*latest_entry)}.magnitude();

      RC_ASSERT(approximatelyEqual(radiusFromDb, latestRadius, 1));
}

RC_GTEST_PROP(DatabasePropertyTests,
              CoordinateCRDTProperty,
              (const std::vector<std::tuple<S3D::Timestamp, S3D::Coord>>& entries)) {
              RC_PRE(entries.size() > 0);

              auto db = S3D::DatabaseImpl::inMemory(false);

              S3D::IDFactory factory = S3D::IDFactory();

              S3D::ID document = factory();
              S3D::ID entity = factory();
              auto radius = S3D::RADIUS{5};

              db.upsert(document, "Test document", 0);

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

              RC_ASSERT(approximatelyEqual(coordFromDb.x, farthestCoord.x, 1));
              RC_ASSERT(approximatelyEqual(coordFromDb.y, farthestCoord.y, 1));
              RC_ASSERT(approximatelyEqual(coordFromDb.z, farthestCoord.z, 1));
}