#include <stdlib.h>
#include <stdio.h>
#include "crossroadTypes.h"

//Global Variables (later move to another file)
constexpr int trafficLightChangeCooldown = 3;
constexpr int numberOfLanes = 1;

void addCarToLane(struct Lane* lane, const int currentTimestamp) {
    static int currentVehicleId = 0;
    if (lane->vehiclesOnLane == nullptr) {
        lane->vehiclesOnLane = (struct Vehicle*)malloc(sizeof (struct Vehicle) * ++lane->vehiclesCapacity);
    }
    else if (lane->vehiclesCapacity - lane->vehiclesLength == 0) {
        struct Vehicle* newVehiclesArray = realloc(lane->vehiclesOnLane,
            sizeof (struct Vehicle) * ++lane->vehiclesCapacity);

        if (newVehiclesArray != nullptr)
            lane->vehiclesOnLane = newVehiclesArray;
        else
            printf("WARNING: MEMORY ERROR!");
    }
    lane->vehiclesOnLane[lane->vehiclesLength++] = (struct Vehicle){
        currentVehicleId,
        currentTimestamp,
    };
    ++currentVehicleId;
}

struct CrossroadManager initializeCrossroadManager() {
    const struct Lane lane = {
        0,
        nullptr,
        0,
        0};

    return (struct CrossroadManager){
        .currentTimestamp = 0,
        .isHorizontalLightGreen = true,
        .roads =
            {{north, lane},
            {west, lane},
            {south, lane},
            {east, lane}}
    };
}

void updateTrafficLights(struct CrossroadManager* m) {
    if (m->currentTimestamp % trafficLightChangeCooldown == 0) {
        m->isHorizontalLightGreen = !m->isHorizontalLightGreen;
    }
}

void runTickOfSimulation(struct CrossroadManager* m) {
    m->currentTimestamp += 1;
    updateTrafficLights(m);

    for (int i = 0; i < maxDirections; ++i) {
        addCarToLane(&m->roads[i].lanes[0], m->currentTimestamp);
    }
}

void displaySimulation(const struct CrossroadManager* m) {
    printf("********************************\n");
    printf("Current timestamp: %d\n", m->currentTimestamp);

    if (m->isHorizontalLightGreen)
        printf("Current green light: North, South\n");
    else
        printf("Current green light: West, East\n");

    printf("Crossroad state:\n");
    for (int i = 0; i < maxDirections; ++i) {
        printf("%d\n", i);
        for (int j = 0; j < numberOfLanes; ++j) {
            printf("\t%d:", j);
            for (int k = 0; k < m->roads[i].lanes[j].vehiclesLength; ++k) {
                printf(" car %d;", m->roads[i].lanes[j].vehiclesOnLane[k].id);
            }
            printf("\n");
        }
    }

    printf("************************************\n");


    /* Planned future map look

constexpr int asciiNumberStart = 48;
printf("*******************************\n"
"* | | || |   * timestamp:%d%c   *\n"
"* %c | || | %c-*                *\n"
"*   | || |   *                *\n"
"*---/ || \\---*                *\n"
"*            *                *\n"
"*===  <>  ===*                *\n"
"*            *                *\n"
"*---\\ || /---*                *\n"
"*   | || |   *                *\n"
"* -%c| || | %c *                *\n"
"*   | || | | *                *\n"
"*******************************\n",
m->currentTimestamp / 10 == 0 ? m->currentTimestamp % 10 : m->currentTimestamp / 10,
m->currentTimestamp / 10 != 0 ? asciiNumberStart + m->currentTimestamp % 10 : ' ',
m->isHorizontalLightGreen ? '+' : '0',
m->isHorizontalLightGreen ? '0' : '+',
m->isHorizontalLightGreen ? '0' : '+',
m->isHorizontalLightGreen ? '+' : '0');*/
}

int main(void) {
    struct CrossroadManager manager = initializeCrossroadManager();

    displaySimulation(&manager);
    while (getchar() == '\n'){
        runTickOfSimulation(&manager);
        displaySimulation(&manager);
    }

    return 0;
}
