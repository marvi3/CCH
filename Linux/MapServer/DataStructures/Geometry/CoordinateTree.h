#pragma once

#include <cmath>
#include <vector>
#include <sstream>

#include "Point.h"
#include "Rectangle.h"
#include "Metric.h"

#include "../../Helpers/Helpers.h"
#include "../../Helpers/Types.h"
#include "../../Helpers/Assert.h"
#include "../../Helpers/Vector/Vector.h"
#include "../../Helpers/Vector/Permutation.h"
#include "../../Helpers/Debug.h"

template<typename METRIC = Geometry::EuclideanMetirc>
class CoordinateTree {

private:
    struct Node {
        inline bool isLeaf() const {return (minChild < 0) && (maxChild < 0);}
        inline void makeLeaf() {minChild = -1; maxChild = -1;}
        int minChild;
        int maxChild;
        union {
            struct { // Data for leaf nodes: Interval of points represented by the leaf
                int begin;
                int end;
            };
            struct { // Data for inner nodes: Index of split dimension and max (respectively min) value for the splitDimension of the child nodes
                int splitDimension;
                double minChildMax;
                double maxChildMin;
            };
        };
    };

public:
    CoordinateTree(const METRIC& metric, const std::vector<Geometry::Point>& coordinates, int maxLeafSize = 10) :
        metric(metric),
        coordinates(coordinates),
        vertexCount(coordinates.size()),
        maxLeafSize(maxLeafSize),
        order(Vector::id(numVertices())),
        nearestVertex(-1),
        nearestDistance(-1) {
        Assert(!coordinates.empty());
        nodes.reserve((coordinates.size() / maxLeafSize) * 4);
        makeBoundingBox(0, numVertices(), boundingBox);
        divideTree(newNode(), 0, numVertices(), boundingBox);
    }
    CoordinateTree(const std::vector<Geometry::Point>& coordinates, int maxLeafSize = 10) :
        coordinates(coordinates),
        vertexCount(coordinates.size()),
        maxLeafSize(maxLeafSize),
        order(Vector::id(numVertices())),
        nearestVertex(-1),
        nearestDistance(-1) {
        Assert(!coordinates.empty());
        nodes.reserve((coordinates.size() / maxLeafSize) * 4);
        makeBoundingBox(0, numVertices(), boundingBox);
        divideTree(newNode(), 0, numVertices(), boundingBox);
    }

    inline Vertex getNearestNeighbour(const Geometry::Point& p) const {
        nearestVertex = 0;
        nearestDistance = metric.distanceSquare(p, coordinates[nearestVertex]);
        this->p = p;
        searchLevel(0, boundingBox.closestPoint(p));
        return nearestVertex;
    }

    inline std::vector<Vertex> getNeighbours(const Geometry::Point& p, const double maxDistance) const {
        neighbours.clear();
        nearestDistance = maxDistance * maxDistance;
        this->p = p;
        searchNeighbours(0, boundingBox.closestPoint(p));
        return neighbours;
    }

    inline int numVertices() const {
        return vertexCount;
    }

    inline const Geometry::Point& getCoordinates(int vertex) const {
        return coordinates[vertex];
    }

    inline void clear() {
        order.clear();
        nodes.clear();
    }

    inline const std::vector<Vertex>& getOrder() const {
        return order;
    }

    inline int numNodes() const {
        return nodes.size();
    }

    inline int size() const {
        return sizeof(*this) + Vector::byteSize(order) + Vector::byteSize(nodes);
    }

    inline void check() const {
        std::vector<bool> seen(numVertices());
        check(0, boundingBox, seen);
        for (int i = 0; i < seen.size(); i++) {
            Assert(seen[i]);
        }
    }

private:
    void divideTree(const int nodeIndex, const int beginIndex, const int endIndex, const Geometry::Rectangle& boundingBox) noexcept {
        AssertMsg(isBoundingBox(beginIndex, endIndex, boundingBox), boundingBoxError(beginIndex, endIndex, boundingBox));
        AssertMsg(endIndex > beginIndex, "endIndex = " << endIndex << " <= beginIndex = " << beginIndex << "!");
        AssertMsg(isNode(nodeIndex), "Index = " << nodeIndex << " is not a node!");

        Node& node = nodes[nodeIndex];
        if ((endIndex - beginIndex) <= maxLeafSize || boundingBox.isPoint()) {
            node.makeLeaf();
            node.begin = beginIndex;
            node.end = endIndex;
        } else {
            if (metric.spread(boundingBox, 0) > metric.spread(boundingBox, 1)) {
                node.splitDimension = 0;
            } else {
                node.splitDimension = 1;
            }
            double splitValue = boundingBox.center()[node.splitDimension];
            Geometry::Rectangle minBoundingBox(Geometry::Rectangle::Negative());
            Geometry::Rectangle maxBoundingBox(Geometry::Rectangle::Negative());
            int i = beginIndex;
            int j = endIndex - 1;
            while (i <= j) {
                while (i <= j && coordinates[order[i]][node.splitDimension] < splitValue) {
                    minBoundingBox.extend(coordinates[order[i++]]);
                }
                while (i <= j && coordinates[order[j]][node.splitDimension] >= splitValue) {
                    maxBoundingBox.extend(coordinates[order[j--]]);
                }
                if (i >= j) break;
                std::swap(order[i], order[j]);
                minBoundingBox.extend(coordinates[order[i++]]);
                maxBoundingBox.extend(coordinates[order[j--]]);
            }
            Assert(i - 1 == j);
            Assert(i > beginIndex);
            Assert(i < endIndex);
            node.minChildMax = minBoundingBox.max[node.splitDimension];
            node.maxChildMin = maxBoundingBox.min[node.splitDimension];
            const int minChild = newNode();
            divideTree(minChild, beginIndex, i, minBoundingBox);
            const int maxChild = newNode();
            divideTree(maxChild, i, endIndex, maxBoundingBox);
            nodes[nodeIndex].minChild = minChild;
            nodes[nodeIndex].maxChild = maxChild;
        }
    }

    void searchLevel(const int nodeIndex, const Geometry::Point& closest) const noexcept {
        AssertMsg(isNode(nodeIndex), "Index = " << nodeIndex << " is not a node!");

        const Node& node = nodes[nodeIndex];
        if (node.isLeaf()) {
            for (int i = node.begin; i < node.end; i++) {
                const double distance = metric.distanceSquare(p, coordinates[order[i]]);
                if (nearestDistance > distance) {
                    nearestDistance = distance;
                    nearestVertex = order[i];
                }
            }
        } else {
            Geometry::Point minClosest = closest;
            minClosest[node.splitDimension] = Variadic::min(closest[node.splitDimension], node.minChildMax);
            const double minChildDistance = metric.distanceSquare(p, minClosest);
            Geometry::Point maxClosest = closest;
            maxClosest[node.splitDimension] = Variadic::max(closest[node.splitDimension], node.maxChildMin);
            const double maxChildDistance = metric.distanceSquare(p, maxClosest);
            if (minChildDistance < maxChildDistance) {
                if (nearestDistance <= minChildDistance) return;
                searchLevel(node.minChild, minClosest);
                if (nearestDistance <= maxChildDistance) return;
                searchLevel(node.maxChild, maxClosest);
            } else {
                if (nearestDistance <= maxChildDistance) return;
                searchLevel(node.maxChild, maxClosest);
                if (nearestDistance <= minChildDistance) return;
                searchLevel(node.minChild, minClosest);
            }
        }
    }

    void searchNeighbours(const int nodeIndex, const Geometry::Point& closest) const noexcept {
        AssertMsg(isNode(nodeIndex), "Index = " << nodeIndex << " is not a node!");

        const Node& node = nodes[nodeIndex];
        if (node.isLeaf()) {
            for (int i = node.begin; i < node.end; i++) {
                const double distance = metric.distanceSquare(p, coordinates[order[i]]);
                if (nearestDistance >= distance) neighbours.emplace_back(order[i]);
            }
        } else {
            Geometry::Point minClosest = closest;
            minClosest[node.splitDimension] = Variadic::min(closest[node.splitDimension], node.minChildMax);
            const double minChildDistance = metric.distanceSquare(p, minClosest);
            if (nearestDistance >= minChildDistance) searchNeighbours(node.minChild, minClosest);
            Geometry::Point maxClosest = closest;
            maxClosest[node.splitDimension] = Variadic::max(closest[node.splitDimension], node.maxChildMin);
            const double maxChildDistance = metric.distanceSquare(p, maxClosest);
            if (nearestDistance >= maxChildDistance) searchNeighbours(node.maxChild, maxClosest);
        }
    }

    inline int newNode() noexcept {
        nodes.push_back(Node());
        return nodes.size() - 1;
    }

    inline bool isNode(const int index) const noexcept {
        return (index >= 0) && (index < nodes.size());
    }

    inline void makeBoundingBox(const int beginIndex, const int endIndex, Geometry::Rectangle& boundingBox) noexcept {
        Assert(endIndex > beginIndex);
        boundingBox.clear(coordinates[order[beginIndex]]);
        for (int i = beginIndex + 1; i < endIndex; i++) {
            boundingBox.extend(coordinates[order[i]]);
        }
    }

    inline bool isBoundingBox(const int beginIndex, const int endIndex, const Geometry::Rectangle& boundingBox) noexcept {
        Geometry::Rectangle realBoundingBox;
        makeBoundingBox(beginIndex, endIndex, realBoundingBox);
        return (realBoundingBox == boundingBox);
    }

    inline std::string boundingBoxError(const int beginIndex, const int endIndex, const Geometry::Rectangle& boundingBox) const noexcept {
        std::stringstream ss;
        ss << "Bounding box " << boundingBox << " does not match the coordinates from index " << beginIndex << " to index " << endIndex << "!";
        printStackTrace();
        return ss.str();
    }

    void check(const int nodeIndex, const Geometry::Rectangle& bb, std::vector<bool>& seen) const noexcept {
        AssertMsg(isNode(nodeIndex), "Index = " << nodeIndex << " is not a node!");

        const Node& node = nodes[nodeIndex];
        if (node.isLeaf()) {
            for (int i = node.begin; i < node.end; i++) {
                if (!bb.contains(coordinates[order[i]])) {
                    std::cout << "nodeIndex: " << nodeIndex << std::endl << std::flush;
                    std::cout << "bb: " << bb << std::endl << std::flush;
                    std::cout << "i: " << i << std::endl << std::flush;
                    std::cout << "order[i]: " << order[i] << std::endl << std::flush;
                    std::cout << "coordinates[order[i]]: " << coordinates[order[i]] << std::endl << std::flush;
                }
                Assert(bb.contains(coordinates[order[i]]));
                Assert(!seen[order[i]]);
                seen[order[i]] = true;
            }
        } else {
            Geometry::Rectangle minBB = bb;
            minBB.max[node.splitDimension] = node.minChildMax;
            if (!bb.contains(minBB)) {
                std::cout << "nodeIndex: " << nodeIndex << std::endl << std::flush;
                std::cout << "bb: " << bb << std::endl << std::flush;
                std::cout << "minBB: " << minBB << std::endl << std::flush;
                std::cout << "node.splitDimension: " << node.splitDimension << std::endl << std::flush;
                std::cout << "node.minChildMax: " << node.minChildMax << std::endl << std::flush;
                std::cout << "node.maxChildMin: " << node.maxChildMin << std::endl << std::flush;
            }
            Assert(bb.contains(minBB));
            check(node.minChild, minBB, seen);
            Geometry::Rectangle maxBB = bb;
            maxBB.min[node.splitDimension] = node.maxChildMin;
            Assert(bb.contains(maxBB));
            check(node.maxChild, maxBB, seen);
        }
    }

private:
    METRIC metric;

    const std::vector<Geometry::Point>& coordinates;
    const int vertexCount;
    const int maxLeafSize;

    Geometry::Rectangle boundingBox;
    std::vector<Vertex> order;
    std::vector<Node> nodes; // nodes[0] = root of tree

    mutable Vertex nearestVertex;
    mutable double nearestDistance;
    mutable Geometry::Point p;
    mutable std::vector<Vertex> neighbours;

};

namespace UnitTests {

inline void coordinateTree() noexcept {
    std::vector<Geometry::Point> coordinates;
    coordinates.emplace_back(Geometry::Point::XY(0, 0));
    coordinates.emplace_back(Geometry::Point::XY(0, 1));
    coordinates.emplace_back(Geometry::Point::XY(1, 0));
    coordinates.emplace_back(Geometry::Point::XY(1, 1));
    coordinates.emplace_back(Geometry::Point::XY(1, 2));
    coordinates.emplace_back(Geometry::Point::XY(1, 5));
    coordinates.emplace_back(Geometry::Point::XY(1, 8));
    coordinates.emplace_back(Geometry::Point::XY(1, 10));
    coordinates.emplace_back(Geometry::Point::XY(3, 0));
    coordinates.emplace_back(Geometry::Point::XY(3, 2));
    coordinates.emplace_back(Geometry::Point::XY(3, 10));
    coordinates.emplace_back(Geometry::Point::XY(4, 2));
    coordinates.emplace_back(Geometry::Point::XY(4, 5));
    coordinates.emplace_back(Geometry::Point::XY(6, 3));
    coordinates.emplace_back(Geometry::Point::XY(6, 7));
    coordinates.emplace_back(Geometry::Point::XY(8, 5));
    coordinates.emplace_back(Geometry::Point::XY(10, 0));
    coordinates.emplace_back(Geometry::Point::XY(10, 3));
    coordinates.emplace_back(Geometry::Point::XY(10, 7));
    coordinates.emplace_back(Geometry::Point::XY(13, 0));
    coordinates.emplace_back(Geometry::Point::XY(13, 3));
    CoordinateTree<Geometry::EuclideanMetirc> ct(Geometry::EuclideanMetirc(), coordinates, 3);
    for (size_t i = 0; i < coordinates.size(); i++) {
        AssertMsg(ct.getNearestNeighbour(coordinates[i]) == i, "Point i = " << coordinates[i] << " has nearest neighbour " << ct.getNearestNeighbour(coordinates[i]) << " = " << coordinates[ct.getNearestNeighbour(coordinates[i])] << "!");
    }
    AssertMsg(ct.getNearestNeighbour(Geometry::Point::XY(20, 0)) == 19, "Point (20, 0) has nearest neighbour " << ct.getNearestNeighbour(Geometry::Point::XY(20, 0)) << " != 19!");
    AssertMsg(ct.getNearestNeighbour(Geometry::Point::XY(-1, -1)) == 0, "Point (-1, -1) has nearest neighbour " << ct.getNearestNeighbour(Geometry::Point::XY(-1, -1)) << " != 0!");
    AssertMsg(ct.getNearestNeighbour(Geometry::Point::XY(0.5, 0.5)) < 4, "Point (0.5, 0.5) has nearest neighbour " << ct.getNearestNeighbour(Geometry::Point::XY(0.5, 0.5)) << " >= 4!");
    AssertMsg(ct.getNearestNeighbour(Geometry::Point::XY(2, 7)) == 6, "Point (2, 7) has nearest neighbour " << ct.getNearestNeighbour(Geometry::Point::XY(2, 7)) << " != 6!");
    AssertMsg(ct.getNearestNeighbour(Geometry::Point::XY(6.00001, 5)) == 15, "Point (6.00001, 5) has nearest neighbour " << ct.getNearestNeighbour(Geometry::Point::XY(6.00001, 5)) << " != 15!");
    std::vector<Vertex> n1 = ct.getNeighbours(Geometry::Point::XY(6, 5), 2);
    AssertMsg(n1.size() == 4, "Point (6, 5) has " << n1.size() << " neighbours != 4");
    AssertMsg(Vector::contains(n1, 12), "Neighbours of point (6, 5) is missing point 12 = " << coordinates[12] << "!");
    AssertMsg(Vector::contains(n1, 13), "Neighbours of point (6, 5) is missing point 13 = " << coordinates[13] << "!");
    AssertMsg(Vector::contains(n1, 14), "Neighbours of point (6, 5) is missing point 14 = " << coordinates[14] << "!");
    AssertMsg(Vector::contains(n1, 15), "Neighbours of point (6, 5) is missing point 15 = " << coordinates[15] << "!");
    std::vector<Vertex> n2 = ct.getNeighbours(Geometry::Point::XY(1, 1.5), 1.5);
    AssertMsg(n2.size() == 4, "Point (1, 1.5) has " << n2.size() << " neighbours != 4");
    AssertMsg(Vector::contains(n2, 1), "Neighbours of point (1, 1.5) is missing point 1 = " << coordinates[1] << "!");
    AssertMsg(Vector::contains(n2, 2), "Neighbours of point (1, 1.5) is missing point 2 = " << coordinates[2] << "!");
    AssertMsg(Vector::contains(n2, 3), "Neighbours of point (1, 1.5) is missing point 3 = " << coordinates[3] << "!");
    AssertMsg(Vector::contains(n2, 4), "Neighbours of point (1, 1.5) is missing point 4 = " << coordinates[4] << "!");
}

}
