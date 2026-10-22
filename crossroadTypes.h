#ifndef CROSSROADSIMULATION_CROSSROADTYPES_H
#define CROSSROADSIMULATION_CROSSROADTYPES_H

enum Direction {
    north,
    east,
    south,
    west,
    maxDirections,
};

enum RelativeDirection {
    right,
    forward,
    left,
};

struct Vehicle {
    int id;
    int creationTimestamp;
};

struct Lane {
    int id;
    struct Vehicle* vehiclesOnLane;
    int vehiclesLength;
    int vehiclesCapacity;
};
struct Road {
    enum Direction direction;
    struct Lane lanes[1];
};

struct CrossroadManager {
    int currentTimestamp;
    bool isHorizontalLightGreen;
    struct Road roads[4];
};


#endif //CROSSROADSIMULATION_CROSSROADTYPES_H