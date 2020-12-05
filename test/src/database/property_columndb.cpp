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

RC_GTEST_PROP(ColumnDBPropertyTests,
              RadiusCRDTProperty, ()) {
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

    RC_ASSERT(S3D::approximately_equal(radiusFromDb->magnitude(), latestRadius.magnitude(), 1));
}