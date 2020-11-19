//
//  Mesh.cpp
//  cross-platform-game iOS
//
//  Created by Berci on 2020. 11. 06..
//  Copyright © 2020. Berci. All rights reserved.
//

#include <optional>

#include "mesh/Mesh.h"

#include "libfive.h"
#include "libfive/render/brep/mesh.hpp"
#include "libfive/render/brep/settings.hpp"

#include "data/Sphere.h"
#include "data/SetOp.h"

namespace S3D {
    std::optional<libfive::Tree> l5TreeFromTree(const Tree& tree) {
        if (auto sphere = std::dynamic_pointer_cast<Sphere>(tree.node)) {
            auto x = libfive::Tree::X();
            auto y = libfive::Tree::Y();
            auto z = libfive::Tree::Z();

            return (
                    square(x - sphere->coord.x)
                    + square(y - sphere->coord.y)
                    + square(z - sphere->coord.z)
                    - sphere->radius
            );
        } else if (auto setop = std::dynamic_pointer_cast<SetOp>(tree.node)) {
            std::vector<libfive::Tree> trees{};

            for (const auto& subtree : tree.children) {
                auto maybeTree = l5TreeFromTree(subtree);
                if (maybeTree.has_value()) {
                    trees.push_back(maybeTree.value());
                }
            }

            if (trees.size() < 1) {
                return std::nullopt;
            } else if (trees.size() == 1) {
                return trees.at(0);
            }

            if (setop->type == SetOperationType::Union) {
                auto unionMesh = min(trees.at(0), trees.at(1));
                for (size_t i = 2; i < trees.size(); i++) {
                    unionMesh = min(trees.at(i), unionMesh);
                }
                return unionMesh;
                // TODO: implement
                // } else if (setop->type == SetOperationType::Subtraction) {

            } else if (setop->type == SetOperationType::Intersection) {
                auto intersectionMesh = max(trees.at(0), trees.at(1));
                for (size_t i = 2; i < trees.size(); i++) {
                    intersectionMesh = max(trees.at(i), intersectionMesh);
                }
                return intersectionMesh;
            }
        }

        return std::nullopt;
    }
    std::unique_ptr<Mesh> Mesh::fromTree(const Tree& tree) {
        auto shape = l5TreeFromTree(tree);

        if (!shape.has_value()) { return nullptr; }

        int bound = 5;
        auto bounds = libfive::Region<3>({-bound, -bound, -bound}, {bound, bound, bound});
        auto mesh = libfive::Mesh::render(shape.value(), bounds, libfive::BRepSettings());

        std::vector<S3DVertex> vertices;
        vertices.reserve(mesh->verts.size());

        std::vector<uint32_t> indices;
        indices.reserve(mesh->branes.size() * 3);

        for (const auto& v : mesh->verts) {
            auto vertex = S3DVertex{
                .position = { v.x(), v.y(), v.z(), 1.0f },
                .normal = { 0, 0, 0, 0 }
            };

            vertices.push_back(vertex);
        }

        for (const auto& t : mesh->branes) {
            indices.push_back(t[0]);
            indices.push_back(t[1]);
            indices.push_back(t[2]);
        }

        for (size_t i = 0; i < indices.size(); i += 3) {
            auto i0 = indices[i];
            auto i1 = indices[i+1];
            auto i2 = indices[i+2];

            auto v0 = &vertices[i0];
            auto v1 = &vertices[i1];
            auto v2 = &vertices[i2];

            vector_float3 p0 = v0->position.xyz;
            vector_float3 p1 = v1->position.xyz;
            vector_float3 p2 = v2->position.xyz;

            vector_float3 cross = simd_cross((p1 - p0), (p2 - p0));
            vector_float4 cross4 = { cross.x, cross.y, cross.z, 0 };

            v0->normal += cross4;
            v1->normal += cross4;
            v2->normal += cross4;
        }

        for (auto & vertex : vertices) {
            vertex.normal = simd_normalize(vertex.normal);
        }

        return std::make_unique<Mesh>(Mesh{vertices, indices});
    }

}

