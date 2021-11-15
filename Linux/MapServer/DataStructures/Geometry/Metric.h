#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <cmath>

#include "Point.h"
#include "Rectangle.h"

namespace Geometry {

struct EuclideanMetirc {

    inline double distanceSquare(const Point& a, const Point& b) const {return Geometry::euclideanDistanceSquared(a, b);}
    inline double distance(const Point& a, const Point& b) const {return Geometry::euclideanDistance(a, b);}
    inline double spread(const Rectangle& r, const int dimension) const {return r.d(dimension);}

};

struct GeoMetirc {

    inline double distanceSquare(const Point& a, const Point& b) const {
        const double d = geoDistanceInCM(a, b);
        return d * d;
    }

    inline double distance(const Point& a, const Point& b) const {
        return geoDistanceInCM(a, b);
    }

    inline double spread(const Rectangle& r, const int dimension) const {
        Point a = r.center();
        a[dimension] = r.min[dimension];
        Point b = r.center();
        b[dimension] = r.max[dimension];
        return geoDistanceInCM(a, b);
    }

};

struct GeoMetircAproximation {

    GeoMetircAproximation() : correction(Point::XY(1, 1)) {}
    inline static GeoMetircAproximation SetCorrection(const Point& c) {
        GeoMetircAproximation metric;
        metric.correction = c;
        return metric;
    }
    inline static GeoMetircAproximation ComputeCorrection(const Point& p) {
        GeoMetircAproximation metric;
        const Point x = Point::XY(p.x + 0.1, p.y);
        const Point y = Point::XY(p.x, p.y + 0.1);
        const double dx = geoDistanceInCM(p, x);
        const double dy = geoDistanceInCM(p, y);
        //metric.correction = Point::XY(1, dy / dx);
        metric.correction = Point::XY(dx, dy * dy / dx);
        return metric;
    }

    inline double distanceSquare(const Point& a, const Point& b) const {
        const double dx = (a.x - b.x) * correction.x;
        const double dy = (a.y - b.y) * correction.y;
        return (dx * dx) + (dy * dy);
    }

    inline double distance(const Point& a, const Point& b) const {
        return std::sqrt(distanceSquare(a, b));
    }

    inline double spread(const Rectangle& r, const int dimension) const {
        return r.d(dimension) * correction[dimension];
    }

    Point correction;

};

}
