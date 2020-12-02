//
// Created by Berci on 2020. 12. 02..
//

#include "gtest/gtest.h"
#include "rapidcheck.h"
#include "rapidcheck/gtest.h"

#include <array>

#include <data/Coord.h>
#include <data/Name.h>
#include <utils/Serializator.h>
#include <utils/attribute_serialization.h>


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

    template<>
    struct Arbitrary<S3D::SetOperationType> {
        static Gen<S3D::SetOperationType> arbitrary() {
            return gen::element(S3D::SetOperationType::Union,
                                S3D::SetOperationType::Intersection,
                                S3D::SetOperationType::Subtraction);
        }
    };
}

RC_GTEST_PROP(SerializationTest,
              CoordRoundTrip,
              (S3D::Coord coord)) {
    S3D::Serializator seri{};

    auto buffer = seri.make_buffer();

    auto size = seri.serialize(buffer, coord);

    S3D::Coord other{};

    auto success = seri.deserialize(buffer, other, size);
    RC_ASSERT(success
                && coord.x == other.x
                && coord.y == other.y
                && coord.z == other.z);
}

RC_GTEST_PROP(SerializationTest,
              SetOperationTypeRoundTrip,
              (S3D::SetOperationType setOpType)) {
    S3D::Serializator seri{};

    auto buffer = seri.make_buffer();

    auto size = seri.serialize(buffer, setOpType);

    S3D::SetOperationType other{};

    auto success = seri.deserialize(buffer, other, size);
    RC_ASSERT(success
              && setOpType == other);
}


