#ifndef CROSSROADSIMULATION_CARMANAGMENT_H
#define CROSSROADSIMULATION_CARMANAGMENT_H

#include "crossroadTypes.h"

void addSpecificCarToLane(struct Lane* lane, const struct Vehicle* car);
void addNewCarToLane(struct Lane* lane, int currentTimestamp);
struct Vehicle deleteNthCarFromLane(struct Lane* lane, int* vehiclesCounter, int n);
void deleteCarsFromLanes(struct CrossroadManager* m);
void moveCarsToAvailableLanes(const struct CrossroadManager* m);
void spawnNewCars(const struct CrossroadManager* m);
void simulateCarMovement(struct CrossroadManager* m);

#endif //CROSSROADSIMULATION_CARMANAGMENT_H
