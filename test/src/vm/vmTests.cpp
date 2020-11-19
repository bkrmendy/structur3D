//
// Created by Berci on 2020. 11. 19..
//

#include <memory>
#include <vector>
#include <optional>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "db/Database.h"
#include "vm/Viewmodel.h"

namespace S3D {
    class MockDatabase : S3D::Database {
    public:
        MOCK_METHOD(std::vector<DocumentWithName>, documents, (), (override));
        MOCK_METHOD(std::vector<IDWithType>, entities, (const ID&), (override));
        MOCK_METHOD(std::optional<Sphere>, sphere, (const ID&), (override));
    };
}

TEST(ViewModelTests, BrandNewViewModel) {
    auto db = std::make_shared<S3D::MockDatabase>(S3D::MockDatabase());

    auto vm = S3D::ViewModel{db};

    EXPECT_EQ(vm.document(), std::nullopt);
    EXPECT_TRUE(vm.message().empty());
}
