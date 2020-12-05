//
// Created by Berci on 2020. 12. 05..
//

#include "gtest/gtest.h"
#include "rapidcheck.h"
#include "rapidcheck/gtest.h"

#include "data/Coord.h"
#include "data/Name.h"
#include "data/SetOperationType.h"
#include "data/Radius.h"

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

    template<>
    struct Arbitrary<S3D::Radius> {
        static Gen<S3D::Radius> arbitrary() {
            return gen::construct<S3D::Radius>(gen::positive<float>());
        }
    };
}

RC_GTEST_PROP(PreferredCommutativeTests,
        PreferredNamePredicateCommutative,
(const S3D::Name& left, const S3D::Name& right)) {
    RC_ASSERT(preferred(left, right) == preferred(right, left));
}

RC_GTEST_PROP(PreferredCommutativeTests,
              PreferredCoordPredicateCommutative,
              (const S3D::Coord& left, const S3D::Coord& right)) {
    RC_ASSERT(preferred(left, right) == preferred(right, left));
}

RC_GTEST_PROP(PreferredCommutativeTests,
              PreferredRadiusPredicateCommutative,
              (const S3D::Radius& left, const S3D::Radius& right)) {
    RC_ASSERT(preferred(left, right) == preferred(right, left));
}

RC_GTEST_PROP(PreferredCommutativeTests,
              PreferredSetOpPredicateCommutative,
              (const S3D::SetOperationType& left, const S3D::SetOperationType& right)) {
    RC_ASSERT(preferred(left, right) == preferred(right, left));
}


