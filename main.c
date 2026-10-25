#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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

        if (newVehiclesArray != nullptr) {
            lane->vehiclesOnLane = newVehiclesArray;
        }
        else
            printf("WARNING: MEMORY ERROR!");
    }
    lane->vehiclesOnLane[lane->vehiclesLength++] = (struct Vehicle){
        currentVehicleId,
        currentTimestamp,
    };
    ++currentVehicleId;
}

//not optimal, but good enough for this program
void deleteFirstCarFromLane(struct Lane* lane) {
    if (lane->vehiclesLength > 0) {
        for (int i = 1; i < lane->vehiclesLength; ++i) {
            lane->vehiclesOnLane[i-1] = lane->vehiclesOnLane[i];
        }
        --lane->vehiclesLength;
    }
}

void simulateCarMovement(struct CrossroadManager* m) {
    for (int i = 0; i < numberOfLanes; ++i) {
        if (m->isHorizontalLightGreen) {
            deleteFirstCarFromLane(&m->roads[north].lanes[i]);
            deleteFirstCarFromLane(&m->roads[south].lanes[i]);
        }
        else {
            deleteFirstCarFromLane(&m->roads[west].lanes[i]);
            deleteFirstCarFromLane(&m->roads[east].lanes[i]);
        }
    }
}

enum Direction getDirectionFromString(char string[]) {
    if (strcmp(string, "west") == 0)
        return west;
    if (strcmp(string, "east") == 0)
        return east;
    if (strcmp(string, "north") == 0)
        return north;
    if (strcmp(string, "south") == 0)
        return south;

    return maxDirections;
}

struct CrossroadManager initializeCrossroadManager(char filePath[]) {
    int timestamp = {};
    enum Direction direction = {};
    if (filePath == nullptr) {
        timestamp = 0;
        direction = maxDirections;
    } else {
        FILE *f = fopen(filePath, "r");
        if (f == nullptr) {
            perror("YOU TRIED OPEN FILE THAT DOESN'T EXIST!\n (initializing default crossroad instead)");
        }

        char tmp[100] = {};
        //get timestamp
        fgets(tmp, sizeof(tmp), f);
        timestamp = (int)strtol(tmp, nullptr, 10); //strtol is used to convert char* to int
        if (timestamp < 0) {
            perror("Timestamp in file should be positive! Resetting timestamp to 0!\n");
            timestamp = 0;
        }

        char directionString[7] = {};
        fgets(directionString, sizeof(directionString), f);
        directionString[strcspn(directionString, "\n")] = '\0';

        direction = getDirectionFromString(directionString);
        if (direction == maxDirections) {
            perror("Wrong direction specified in file! Allowed directions are west, east, north and south!\n");
        }
    }

    const struct Lane lane = {
        0,
        nullptr,
        0,
        0};

    struct Lane carLane = lane;
    addCarToLane(&carLane, 0);

    return (struct CrossroadManager){
        .currentTimestamp = timestamp,
        .isHorizontalLightGreen = true,
        .roads =
            {{north, direction == north ? carLane : lane},
            {west, direction == west ? carLane : lane},
            {south, direction == south ? carLane : lane},
            {east, direction == east ? carLane : lane}}
    };

}

void updateTrafficLights(struct CrossroadManager* m) {
    if (m->currentTimestamp % trafficLightChangeCooldown == 0) {
        m->isHorizontalLightGreen = !m->isHorizontalLightGreen;
    }
}

void runTickOfSimulation(struct CrossroadManager* m) {
    m->currentTimestamp += 1;
    simulateCarMovement(m);
    /*
    for (int i = 0; i < maxDirections; ++i) {
        addCarToLane(&m->roads[i].lanes[0], m->currentTimestamp);
    }
    */
    updateTrafficLights(m);
}

void printRoadInfo(const enum Direction direction, const struct CrossroadManager* m) {
    switch (direction) {
        case north:
            printf("North Road. Current light: ");
            m->isHorizontalLightGreen ? printf("Green") : printf("Red");
            break;
        case south:
            printf("South Road. Current light: ");
            m->isHorizontalLightGreen ? printf("Green") : printf("Red");
            break;
        case west:
            printf("West Road. Current light: ");
            !m->isHorizontalLightGreen ? printf("Green") : printf("Red");
            break;
        case east:
            printf("East Road. Current light: ");
            !m->isHorizontalLightGreen ? printf("Green") : printf("Red");
            break;
        default:
            printf("WRONG direction argument calling printRoadInfo()");
    }
    printf("\n");
}

void displaySimulation(const struct CrossroadManager* m) {
    printf("********************************\n");
    printf("Current timestamp: %d\n", m->currentTimestamp);

    printf("Crossroad state:\n");
    for (int i = 0; i < maxDirections; ++i) {
        printRoadInfo(i, m);
        for (int j = 0; j < numberOfLanes; ++j) {
            printf("\t%d:", j);
            for (int k = 0; k < m->roads[i].lanes[j].vehiclesLength; ++k) {
                printf(" car %d;", m->roads[i].lanes[j].vehiclesOnLane[k].id);
            }
            printf("\n");
        }
    }

    printf("************************************\n");


    //Planned future map look
/*
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
m->currentTimestamp / 10 != 0 ? + m->currentTimestamp % 10 : ' ',
m->isHorizontalLightGreen ? '+' : '0',
m->isHorizontalLightGreen ? '0' : '+',
m->isHorizontalLightGreen ? '0' : '+',
m->isHorizontalLightGreen ? '+' : '0');*/
}

//Made because C apparently DO NOT have a single function to clear entire buffer >:(
int getCharacter() {
    const int character = getchar();
    if (character != '\n') {
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
    }
    return character;
}

int main(void) {
    printf("Welcome to crossroad simulation!\n");
    printf("Enter - start simulation\n");
    printf("F - start simulation with file data\n");
    const int optionCharacter = getCharacter();
    char path[100] = {};

    if (optionCharacter == 'f') {
        printf("Enter path to file [can only be at max 99 characters long]: \n");
        fgets(path, sizeof path, stdin);
        path[strcspn(path, "\n")] = '\0';
    }

    struct CrossroadManager manager = initializeCrossroadManager(optionCharacter == 'f' ? path : nullptr);

    displaySimulation(&manager);
    while (getchar() == '\n'){
        runTickOfSimulation(&manager);
        displaySimulation(&manager);
    }

    return 0;
}
