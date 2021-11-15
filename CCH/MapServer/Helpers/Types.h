#pragma once

#include <limits>
#include <vector>
#include <string>
using std::string;
using std::vector;

using Id = uint32_t;

typedef int Vertex;
typedef int Edge;
typedef Vertex Stop;
typedef int Trip;

constexpr Vertex noVertex = -1;
constexpr Stop noStop = -1;
constexpr Edge noEdge = -1;
constexpr Trip noTrip = -1;
constexpr int noWeight = 0;
constexpr int noLength = 0;
constexpr int noDistance = 0;
constexpr int noTravelTime = 0;
constexpr int noTime = -1;

constexpr int intMax = std::numeric_limits<int>::max();
constexpr double doubleMax = std::numeric_limits<double>::max();
constexpr int never = intMax;

constexpr int EARTH_RADIUS_IN_CENTIMETRE = 637813700;
constexpr double PI = 3.141592653589793;

constexpr int INFTY = std::numeric_limits<int>::max() / 2;
constexpr int DAY = 24 * 60 * 60;
constexpr int VOID = -32768;

constexpr int FORWARD = 0;
constexpr int BACKWARD = 1;

struct NO_OPERATION {
    constexpr inline bool operator() () {return false;}
    inline void operator() (const Vertex) {}
    inline void operator() (const Edge, const Vertex) {}
};

NO_OPERATION NoOperation;
