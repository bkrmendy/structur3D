//
// Created by Berci on 2020. 12. 02..
//

#include "gtest/gtest.h"

#include "data/Name.h"
#include "utils/Serializator.h"
#include "utils/attribute_serialization.h"

// TODO: make it pass
//TEST(SerializationTest, NameSerialization) {
//    S3D::Name name{"name"};
//
//    S3D::Serializator seri{};
//
//    auto buffer = seri.make_buffer();
//
//    auto size = seri.serialize(buffer, name);
//
//    S3D::Name other{};
//
//    auto success = seri.deserialize(buffer, other, size);
//
//    EXPECT_TRUE(success);
//    EXPECT_EQ(name.get(), other.get());
//}

// TODO: make it compile
//TEST(SerializationTest, IDRoundTrip) {
//    auto make_id = S3D::IDFactory();
//    auto uid = make_id();
//
//    S3D::Serializator seri{};
//
//    auto buffer = seri.make_buffer();
//
//    auto size = seri.serialize(buffer, uid);
//
//    S3D::ID other;
//
//    auto success = seri.deserialize(buffer, other, size);
//
//    EXPECT_TRUE(success);
//    EXPECT_EQ(uid, other);
//}