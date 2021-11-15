#pragma once

#include <cmath>
#include <vector>
#include <ostream>
#include <sstream>
#include <limits>

#include "Point.h"

#include "../../Helpers/Assert.h"
#include "../../Helpers/Helpers.h"

namespace Geometry {

class Rectangle {

public:
    // Constructors
    Rectangle() : min(), max() {}
    Rectangle(const Point& p) : min(p), max(p) {}
    inline static Rectangle BoundingBox(const Point& a, const Point& b) {
        Rectangle r;
        r.min = Geometry::min(a, b);
        r.max = Geometry::max(a, b);
        Assert(r.isPositive());
        return r;
    }
    inline static Rectangle BoundingBox(const std::vector<Point>& points) {
        if (points.empty()) return Rectangle();
        Rectangle r(points[0]);
        for (const Point p : points) r.extend(p);
        Assert(r.isPositive());
        return r;
    }
    inline static Rectangle MinMax(const Point& min, const Point& max) {
        Rectangle r;
        r.min = min;
        r.max = Geometry::max(min, max);
        Assert(r.isPositive());
        return r;
    }
    inline static Rectangle Union(const Rectangle& a, const Rectangle& b) {
        Rectangle r;
        r.min = Geometry::min(a.min, b.min);
        r.max = Geometry::max(a.max, b.max);
        Assert(r.isPositive());
        return r;
    }
    inline static Rectangle Intersection(const Rectangle& a, const Rectangle& b) {
        Rectangle r;
        r.min = Geometry::max(a.min, b.min);
        r.max = Geometry::min(a.max, b.max);
        r.max.max(r.min);
        Assert(r.isPositive());
        return r;
    }
    inline static Rectangle Negative(const double d = 1000000) {
        Rectangle r;
        r.min = Point::XY(d, d);
        r.max = Point::XY(-d, -d);
        return r;
    }
    inline static Rectangle Empty() {
        return Negative(std::numeric_limits<int>::max());
    }
    inline static Rectangle Union(const std::vector<Rectangle>& rectangles) {
        Rectangle r = Empty();
        for (const Rectangle& rectangle : rectangles) {
            r.min = Geometry::min(r.min, rectangle.min);
            r.max = Geometry::max(r.max, rectangle.max);
        }
        return r;
    }

    // Modifiers & Operators
    inline bool operator==(const Rectangle& r) const {
        return min == r.min && max == r.max;
    }

    inline bool operator!=(const Rectangle &r) const {
        return !operator==(r);
    }

    inline void extend(const Point& p) {
        min.min(p);
        max.max(p);
        Assert(isPositive());
    }

    inline void extend(const Rectangle& r) {
        min.min(r.min);
        max.max(r.max);
        Assert(isPositive());
    }

    inline bool contains(const Point& p) const {
        if (p.x > max.x || p.x < min.x) return false;
        if (p.y > max.y || p.y < min.y) return false;
        return true;
    }

    inline bool contains(const Rectangle& r) const {
        if (r.max.x > max.x || r.min.x < min.x) return false;
        if (r.max.y > max.y || r.min.y < min.y) return false;
        return true;
    }

    inline void discretize(const double dx, const double dy) noexcept {
        min.x = floor(min.x, dx);
        min.y = floor(min.y, dy);
        max.x = ceil(max.x, dx);
        max.y = ceil(max.y, dy);
    }

    inline void clear(const Point& p = Point()) {
        min = p;
        max = p;
    }

    inline Point center() const {
        return 0.5 * (min + max);
    }

    inline double area() const {
        Assert(isPositive());
        return dx() * dy();
    }

    inline Point closestPoint(const Point& p) const {
        Point c = p;
        if (p.x < min.x) {
            c.x = min.x;
        } else if (p.x > max.x) {
            c.x = max.x;
        }
        if (p.y < min.y) {
            c.y = min.y;
        } else if (p.y > max.y) {
            c.y = max.y;
        }
        return c;
    }

    inline bool isPositive() const {
        return (min.x <= max.x) || (min.y <= max.y);
    }

    inline bool isPoint() const {
        return min == max;
    }

    // String conversion
    inline friend std::ostream& operator<<(std::ostream& os, const Rectangle& r) {
        std::stringstream ss;
        ss << "[" << r.min << " | " << r.max << "]";
        return os << ss.str();
    }

    // Access
    inline double dx() const {return max.x - min.x;}
    inline double dy() const {return max.y - min.y;}
    inline double d(const int dimension) const {return max[dimension] - min[dimension];}

public:
    union {
        Point topLeft;
        Point min;
    };
    union {
        Point bottomRight;
        Point max;
    };

};

// Free functions
inline double euclideanDistanceSquared(const Point& a, const Rectangle& b) {
    return euclideanDistanceSquared(a, b.closestPoint(a));
}

inline double euclideanDistance(const Point& a, const Rectangle& b) {
    return std::sqrt(euclideanDistanceSquared(a, b));
}

static_assert(sizeof(Rectangle) == 2 * sizeof(Point), "Rectangle layout is broken");

}
