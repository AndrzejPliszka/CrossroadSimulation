#include <stdlib.h>
#include <stdio.h>

#include "carManagment.h"
#include "globalVariables.h"

void addSpecificCarToLane(struct Lane* lane, const struct Vehicle* car) {
    if (lane->vehiclesOnLane == nullptr) {
        lane->vehiclesOnLane = (struct Vehicle*)malloc(sizeof (struct Vehicle) * ++lane->vehiclesCapacity);
    }
    else if (lane->vehiclesCapacity - lane->vehiclesLength == 0) {
        struct Vehicle* newVehiclesArray = realloc(lane->vehiclesOnLane,
            sizeof (struct Vehicle) * ++lane->vehiclesCapacity);

        if (newVehiclesArray != nullptr) {
            lane->vehiclesOnLane = newVehiclesArray;
        }
        else
            fprintf(stderr, "WARNING: MEMORY ERROR!");
    }
    lane->vehiclesOnLane[lane->vehiclesLength++] = *car;
}

void addNewCarToLane(struct Lane* lane, const int currentTimestamp) {
    static int currentVehicleId = 0;
    const struct Vehicle car = {currentVehicleId, currentTimestamp};
    addSpecificCarToLane(lane, &car);
    ++currentVehicleId;
}

struct Vehicle deleteNthCarFromLane(struct Lane* lane, int* vehiclesCounter, const int n) {
    if (n < 0 || lane->vehiclesLength <= n) {
        fprintf(stderr, "INVALID n VALUE!! returning wrong object!!!");
        return (struct Vehicle){-1, -1};
    }
    struct Vehicle deletedVehicle = {};
    for (int i = n; i < lane->vehiclesLength; ++i) {
        if (i == n)
            deletedVehicle = lane->vehiclesOnLane[i];
        lane->vehiclesOnLane[i] = lane->vehiclesOnLane[i+1];
    }
    --lane->vehiclesLength;
    if (vehiclesCounter != nullptr && n == 0) //if n == 0, it means that car has driven through crossroad
        ++(*vehiclesCounter);

    return deletedVehicle;
}

void deleteCarsFromLanes(struct CrossroadManager* m) {
    for (int i = 0; i < numberOfLanes; ++i) {
        if (m->isHorizontalLightGreen) {
            if (m->roads[north].lanes[i].vehiclesLength > 0)
                deleteNthCarFromLane(&m->roads[north].lanes[i], &m->carCounter, 0);
            if (m->roads[south].lanes[i].vehiclesLength > 0)
                deleteNthCarFromLane(&m->roads[south].lanes[i], &m->carCounter, 0);
        }
        else {
            if (m->roads[west].lanes[i].vehiclesLength > 0)
                deleteNthCarFromLane(&m->roads[west].lanes[i], &m->carCounter, 0);
            if (m->roads[east].lanes[i].vehiclesLength > 0)
                deleteNthCarFromLane(&m->roads[east].lanes[i], &m->carCounter, 0);
        }
    }
}

void moveCarsToAvailableLanes(const struct CrossroadManager* m) {
    for (int i = 0; i < maxDirections; ++i) {
        for (int j = 0; j < numberOfLanes; ++j) {
            if (j > 0) {
                const int currentLaneLength = m->roads[i].lanes[j].vehiclesLength;
                const int leftLaneLength = m->roads[i].lanes[j-1].vehiclesLength;
                const int carDifference = currentLaneLength - leftLaneLength;
                if (carDifference >= 2) {
                    struct Vehicle movedCar = deleteNthCarFromLane(
                        &m->roads[i].lanes[j],
                        nullptr,
                        leftLaneLength +1);
                    addSpecificCarToLane(&m->roads[i].lanes[j-1], &movedCar);
                }
            }

            if (j < numberOfLanes - 1) {
                const int currentLaneLength = m->roads[i].lanes[j].vehiclesLength;
                const int rightLaneLength = m->roads[i].lanes[j+1].vehiclesLength;
                const int carDifference = currentLaneLength - rightLaneLength;
                if (carDifference >= 2) {
                    struct Vehicle movedCar = deleteNthCarFromLane(
                        &m->roads[i].lanes[j],
                        nullptr,
                        rightLaneLength +1);
                    addSpecificCarToLane(&m->roads[i].lanes[j+1], &movedCar);
                }
            }
        }
    }
}

void spawnNewCars(const struct CrossroadManager* m) {
    for (int i = 0; i < maxDirections; ++i) {
        for (int j = 0; j < numberOfLanes; ++j) {
            const double randomNum = (double)rand()/(double)RAND_MAX;

            printf("%f", randomNum);
            if (randomNum <= m->roads[i].carSpawnProbability)
                addNewCarToLane(&m->roads[i].lanes[j], m->currentTimestamp);
        }
    }
}

void simulateCarMovement(struct CrossroadManager* m) {
    deleteCarsFromLanes(m);
    if (numberOfLanes > 1)
        moveCarsToAvailableLanes(m);
}



