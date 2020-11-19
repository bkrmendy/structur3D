//
// Created by Berci on 2020. 11. 19..
//

#include "gtest/gtest.h"

#include <memory>

#include <data/Base.h>
#include <data/SetOp.h>
#include <data/Coord.h>
#include <data/Edge.h>
#include <data/Sphere.h>
#include <data/Graph.h>

TEST(GraphTests, TestAllIsolated) {
    auto makeId = S3D::IDFactory();
    auto unionNode = std::make_shared<S3D::SetOp>(makeId(), S3D::SetOperationType::Union);
    auto sphere1 = std::make_shared<S3D::Sphere>(makeId(), S3D::Coord{1,2,3}, S3D::RADIUS{4});
    auto sphere2 = std::make_shared<S3D::Sphere>(makeId(), S3D::Coord{4,5,6}, S3D::RADIUS{7});

    std::vector<std::shared_ptr<S3D::Edge>> edges = { };
    std::vector<std::shared_ptr<S3D::Node>> nodes = { unionNode, sphere1, sphere2 };

    auto graph = S3D::Graph{edges, nodes};

    EXPECT_EQ(graph.roots().size(), 3);
    EXPECT_EQ(graph.trees().size(), 3);

    auto roots = graph.roots();
    for (const auto& root : roots) {
        EXPECT_EQ(graph.subTreeOf(root).children.size(), 0);
        EXPECT_EQ(graph.subTreeOf(root).node, root);
    }
}

TEST(GraphTests, TestSingleRoot) {
    auto makeId = S3D::IDFactory();

    auto unionNode = std::make_shared<S3D::SetOp>(makeId(), S3D::SetOperationType::Union);
    auto sphere1 = std::make_shared<S3D::Sphere>(makeId(), S3D::Coord{1,2,3}, S3D::RADIUS{4});
    auto sphere2 = std::make_shared<S3D::Sphere>(makeId(), S3D::Coord{4,5,6}, S3D::RADIUS{7});

    auto e1 = std::make_shared<S3D::Edge>(makeId(), unionNode, sphere1);
    auto e2 = std::make_shared<S3D::Edge>(makeId(), unionNode, sphere2);

    std::vector<std::shared_ptr<S3D::Edge>> edges = { e1, e2 };
    std::vector<std::shared_ptr<S3D::Node>> nodes = { unionNode, sphere1, sphere2 };

    auto graph = S3D::Graph{edges, nodes};

    EXPECT_EQ(graph.roots().size(), 1);
    EXPECT_EQ(graph.roots().at(0), unionNode);

    EXPECT_EQ(graph.trees().size(), 1);
    EXPECT_EQ(graph.trees().at(0).node, unionNode);

    EXPECT_EQ(graph.subTreeOf(unionNode).children.size(), 2);

    EXPECT_EQ(graph.subTreeOf(sphere1).children.size(), 0);
    EXPECT_EQ(graph.subTreeOf(sphere2).children.size(), 0);
}

TEST(GraphTests, MultipleRoots) {
    auto makeId = S3D::IDFactory();

    auto unionNode = std::make_shared<S3D::SetOp>(makeId(), S3D::SetOperationType::Union);
    auto sphere1 = std::make_shared<S3D::Sphere>(makeId(), S3D::Coord{1,2,3}, S3D::RADIUS{4});
    auto sphere2 = std::make_shared<S3D::Sphere>(makeId(), S3D::Coord{4,5,6}, S3D::RADIUS{7});

    auto e1 = std::make_shared<S3D::Edge>(makeId(), unionNode, sphere1);

    std::vector<std::shared_ptr<S3D::Edge>> edges = { e1 };
    std::vector<std::shared_ptr<S3D::Node>> nodes = { unionNode, sphere1, sphere2 };

    auto graph = S3D::Graph{edges, nodes};

    EXPECT_EQ(graph.roots().size(), 2);
    EXPECT_EQ(graph.trees().size(), 2);

    EXPECT_EQ(graph.subTreeOf(unionNode).children.size(), 1);
    EXPECT_EQ(graph.subTreeOf(unionNode).children.at(0).node, sphere1);

    EXPECT_EQ(graph.subTreeOf(sphere2).children.size(), 0);
}
