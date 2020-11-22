//
// Created by Berci on 2020. 11. 21..
//

#include "gtest/gtest.h"
#include "rapidcheck.h"

#include "data/Base.h"
#include "data/Coord.h"
#include "db/DatabaseImpl.h"


namespace rc {
    template <>
    struct Arbitrary<S3D::Coord> {
        static Gen<S3D::Coord> arbitrary() {
            return gen::build<S3D::Coord>(
                    gen::set(&S3D::Coord::x, gen::inRange(-100000, 100000)),
                    gen::set(&S3D::Coord::y, gen::inRange(-100000, 100000)),
                    gen::set(&S3D::Coord::z, gen::inRange(-100000, 100000)));
        }
    };

    template <>
    struct Arbitrary<S3D::SetOperationType> {
        static Gen<S3D::SetOperationType> arbitrary () {
            return gen::element(S3D::SetOperationType::Union,
                                S3D::SetOperationType::Intersection,
                                S3D::SetOperationType::Subtraction);
        }
    };
}

TEST(DatabaseImplTests, RadiusCRDTProperty) {
    rc::check("entry with highest timestamp read after several inserts",
              [](const std::vector<std::tuple<S3D::Timestamp, float>>& entries){
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
                      RC_PRE(0 <= std::get<1>(entry) && std::get<1>(entry) < 1'000'000);
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

                  EXPECT_NEAR(radiusFromDb, latestRadius, 1);
              });
}

TEST(DatabaseImplTests, CoordinateCRDTProperty) {
    rc::check("entry with highest timestamp read after several inserts",
              [](const std::vector<std::tuple<S3D::Timestamp, S3D::Coord>>& entries){
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

                  auto latest_entry = std::max_element(entries.begin(), entries.end(), [](auto a, auto b){
                      if (std::get<0>(a) == std::get<0>(b)) {
                          return S3D::distance_from_origin(std::get<1>(a)) < S3D::distance_from_origin(std::get<1>(b));
                      }
                      return std::get<0>(a) < std::get<0>(b);
                  });

                  auto coordFromDb = db.sphere(entity).value().coord;
                  auto latestCoord = std::get<1>(*latest_entry);

                  if (fabs(coordFromDb.x - latestCoord.x) > 5
                        || fabs(coordFromDb.y - latestCoord.y) > 5
                        || fabs(coordFromDb.z - latestCoord.z) > 5) {
                      int y = 8;
                  }

                  EXPECT_FLOAT_EQ(coordFromDb.x, latestCoord.x);
                  EXPECT_FLOAT_EQ(coordFromDb.y, latestCoord.y);
                  EXPECT_FLOAT_EQ(coordFromDb.z, latestCoord.z);
              });
}

TEST(DatabaseImplTests, SetOperationTypeCRDTProperty) {
    rc::check("entry with highest timestamp read after several inserts",
              [](const std::vector<std::tuple<S3D::Timestamp, S3D::SetOperationType>>& entries){
                  RC_PRE(entries.size() > 0);
                  auto db = S3D::DatabaseImpl::inMemory(false);

                  S3D::IDFactory factory = S3D::IDFactory();

                  S3D::ID document = factory();
                  S3D::ID entity = factory();

                  db.upsert(document, "Test document", 0);

                  db.create(entity, S3D::NodeType::SetOperation, document, 0);

                  for (const auto& entry : entries) {
                      db.upsert(entity, std::get<1>(entry), std::get<0>(entry));
                  }

                  auto latest_entry = std::max_element(entries.begin(), entries.end(), [](auto a, auto b){
                      return std::get<0>(a) < std::get<0>(b);
                  });

                  EXPECT_EQ(db.setop(entity).value().type, std::get<1>(*latest_entry));
              });
}
