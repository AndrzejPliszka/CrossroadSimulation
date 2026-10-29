#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "crossroadTypes.h"

//Global Variables (later move to another file)
constexpr int trafficLightChangeCooldown = 10;
constexpr int numberOfLanes = 4;

void addCarToLane(struct Lane* lane, const struct Vehicle* car) {
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

void spawnCarOnLane(struct Lane* lane, const int currentTimestamp) {
    static int currentVehicleId = 0;
    const struct Vehicle car = {currentVehicleId, currentTimestamp};
    addCarToLane(lane, &car);
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

void moveCarsToAvailableLanes(struct CrossroadManager* m) {
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
                    addCarToLane(&m->roads[i].lanes[j-1], &movedCar);
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
                    addCarToLane(&m->roads[i].lanes[j+1], &movedCar);
                }
            }
        }
    }
}

void simulateCarMovement(struct CrossroadManager* m) {
    deleteCarsFromLanes(m);
    if (numberOfLanes > 1)
        moveCarsToAvailableLanes(m);
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

char* getStringFromDirection(const enum Direction direction) {
    switch (direction) {
        case west:
            return "west";
        case east:
            return "east";
        case north:
            return "north";
        case south:
            return "south";
        default:
            return nullptr;
    }
}

struct CrossroadManager initializeCrossroadManager(char filePath[]) {
    int timestamp = 0;
    enum Direction direction = maxDirections;
    if (filePath != nullptr) {
        FILE *f = fopen(filePath, "r");
        if (f == nullptr) {
            perror("YOU TRIED TO OPEN FILE THAT DOESN'T EXIST!\nInitializing default crossroad instead\n");
            initializeCrossroadManager(nullptr);
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
    spawnCarOnLane(&carLane, 0);

    struct Road roads[4];
    for (int i = 0; i < 4; ++i) {
        const struct Road road = {
            .direction = i,
            .lanes = malloc(sizeof(struct Lane) * numberOfLanes)
        };

        for (int laneIndex = 0; laneIndex < numberOfLanes; ++laneIndex) {
            road.lanes[laneIndex] = direction == i ? carLane : lane;
        }

        roads[i] = road;
    }

    return (struct CrossroadManager){
        .currentTimestamp = timestamp,
        .isHorizontalLightGreen = true,
        .roads = {roads[0], roads[1], roads[2], roads[3]},
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

    for (int i = 0; i < maxDirections; ++i) {
        spawnCarOnLane(&m->roads[i].lanes[0], m->currentTimestamp);
    }

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

char* getNumberEndString(const int number) {
    if (number % 10 == 1)
        return "st";
    if (number % 10 == 2)
        return "nd";
    if (number % 10 == 3)
        return "rd";
    return "st";
}

void displaySimulation(const struct CrossroadManager* m) {
    printf("********************************\n");
    printf("Current timestamp: %d\n", m->currentTimestamp);

    printf("Crossroad state:\n");
    for (int i = 0; i < maxDirections; ++i) {
        printRoadInfo(i, m);
        for (int j = 0; j < numberOfLanes; ++j) {
            printf("\t%d%s lane: %d cars [",
                j + 1,
                getNumberEndString(j+1),
                m->roads[i].lanes[j].vehiclesLength);

            for (int k = 0; k < m->roads[i].lanes[j].vehiclesLength; ++k) {
                printf(k == 0 ? "%d" : ", %d", m->roads[i].lanes[j].vehiclesOnLane[k].id);
            }
            printf("]\n");
        }
    }

    printf("************************************\n");
    printf("Press Enter to continue, press any other key to end.\n");


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

void displaySummary(const struct CrossroadManager *m) {
    printf("Total amount of cars driven through crossroad: %d\n", m->carCounter);
    for (int i = 0; i < maxDirections; ++i) {
        for (int j = 0; j < numberOfLanes; ++j) {
            printf("%d%s %s road had %d car(s)\n",
                j+1,
                getNumberEndString(j+1),
                getStringFromDirection(i),
                m->roads[i].lanes[j].vehiclesLength);
        }
    }
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

    displaySummary(&manager);

    return 0;
}
