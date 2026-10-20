#include <stdlib.h>
#include "crossroadTypes.h"

//Global Variables (later move to another file)
constexpr int trafficLightChangeCooldown = 3;


void initializeCrossroadManager(struct CrossroadManager* m) {
    m->currentTimestamp = 0;
    m->isHorizontalLightGreen = true;
    for (int i = {0}; i < maxDirections; i++) {
        const struct Vehicle vehiclesOnLane[1] = {1, 1};
        m->roads[i] = (struct Road){
            .direction = i,
            .lanes = {
                {0, (struct Vehicle*)malloc(sizeof (struct Vehicle))}
            }
        };
    }
}

void updateTrafficLights(struct CrossroadManager* m) {
    if (m->currentTimestamp % trafficLightChangeCooldown == 0) {
        m->isHorizontalLightGreen = !m->isHorizontalLightGreen;
    }
}

void runTickOfSimulation(struct CrossroadManager* m) {
    updateTrafficLights(m);
}


int main(void) {

    struct CrossroadManager manager;
    initializeCrossroadManager(&manager);

    return 0;
}
