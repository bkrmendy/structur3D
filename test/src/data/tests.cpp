//
// Created by Berci on 2020. 11. 17..
//

#include "gtest/gtest.h"

#include "data/SetOperationType.h"

TEST(DataTests, testFromOperationType) {
    EXPECT_EQ(S3D::from_operationType(S3D::SetOperationType::Union), 0);
    EXPECT_EQ(S3D::from_operationType(S3D::SetOperationType::Intersection), 1);
    EXPECT_EQ(S3D::from_operationType(S3D::SetOperationType::Subtraction), 2);
}

TEST(DataTests, testToOperationType) {
    EXPECT_EQ(S3D::to_operationType(0), S3D::SetOperationType::Union);
    EXPECT_EQ(S3D::to_operationType(1), S3D::SetOperationType::Intersection);
    EXPECT_EQ(S3D::to_operationType(2), S3D::SetOperationType::Subtraction);
    for (size_t i = 3; i < 100; i++) {
        EXPECT_EQ(S3D::to_operationType(i), std::nullopt);
    }
}