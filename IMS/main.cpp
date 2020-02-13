#include <iostream>
#include <random>

///maximal car speed
#define V_MAX 6
#define V_IN 2

struct cars {
    bool here = false;
    int v = 0;
    int vLast = 0;
};

int speedChange(int v, int dist);

int min(int a, int b);

int max(int a, int b);

int randomGen();

bool isTrue(double p);

void carStart(cars road[], int length);

int findPred(int c, cars road[], int length);

bool moveCar(cars road[], int position, int length);

void carReset(cars road[], int position);

int min(int a, int b) {
    return b >= a ? a : b;
}

int max(int a, int b) {
    return b <= a ? a : b;
}

/**
 * code for true random numbers from
 *  https://stackoverflow.com/questions/7560114/random-number-c-in-some-range
 */
int randomGen() {
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 eng(rd()); // seed the generator
    std::uniform_int_distribution<> distr(0, 100); // define the range
    return distr(eng);
}

bool isTrue(double p) {
    return randomGen() <= (p * 100);
}

void carStart(cars road[], int length) {
    road[0].here = true;
    int pred = findPred(0, road, length);
    road[0].v = min(V_IN, speedChange(road[pred].v, pred) - 1);
    if (road[0].v < 0)
        road[0].v = 0;
    road[0].vLast = road[0].v;
}

int speedChange(int v, int dist) {
    int x = (int) (0.5 * sqrt(8 * dist - 7 + 4 * v * (v - 1)) - 0.5);
    return min(x, V_MAX);
}

int findPred(int c, cars road[], int length) {
    int act = c;
    while (c != (length - 1)) {
        c++;
        if (road[c].here)
            return c - act;
    }
    return -1;
}

bool moveCar(cars road[], int position, int length) {
    int velocity = road[position].v;
    bool left = false;
    if (velocity != 0) {
        if ((position + velocity) < length) {
            if (!road[position + velocity].here) {
                road[position + velocity] = road[position];
                carReset(road, position);
            }
        } else {
            left = true;
            carReset(road, position);
        }
    }
    return left;
}

void carReset(cars road[], int position) {
    road[position].vLast = 0;
    road[position].v = 0;
    road[position].here = false;
}


int main(int argc, char *argv[]) {
    //std::cout << "Hello, World!\n";
    ///number of cars to generate
    int carsNum = std::stoi(argv[1]);
    int carsLeft = carsNum;
    ///number of cars that left the system
    int carsOut = 0;
    ///length of road
    int length = std::stoi(argv[2]);
    ///probability of deceleration
    double pAcc = std::stod(argv[3]);

    int stepsCounter = 0;

    cars road[length];

    ///simulation
    while (carsOut != carsNum) {
        ///insert new car if spot 0 is empty
        if (!road[0].here) {
            if (carsLeft != 0) {
                carStart(road, length);
                ///cars left to process --
                carsLeft--;
            }
        }
        for (int i = length - 1; i >= 0; i--) {
            if (road[i].here) {
                int distance = 0;
                if ((distance = findPred(i, road, length)) != -1) {
                    ///if there is a predecessor
                    if (road[i].vLast + 1 <= speedChange(road[i + distance].vLast, distance)) {
                        if (isTrue(pAcc)) {
                            road[i].vLast = road[i].v;
                            road[i].v++;
                        }
                    } else {
                        road[i].vLast = road[i].v;
                        road[i].v = speedChange(road[i + distance].vLast, distance);
                    }
                }
                    ///if there isn't a predecessor
                else {
                    //if (i) {
                        road[i].vLast = road[i].v;
                        road[i].v = min(road[i].v + 1, V_MAX);
                    //}
                }
            }
        }

        for (int i = 0; i < length; i++) {
            if (road[i].here) {
                printf("\u26aa ");
            } else printf("- ");
        }

        for (int i = length - 1; i >= 0; i--) {
            if (road[i].here) {
                if (i) {
                    if (isTrue(1 - pAcc)) {
                        road[i].vLast = road[i].v;
                        road[i].v = max(0, road[i].v - 1);
                    }
                }
                //printf("\u26aa ");
                if (moveCar(road, i, length)) {
                    carsOut++;
                    //printf("Number %d leaving\n", carsOut);
                }
                //printf("\u26aa ");
            } //else printf("- ");
        }

        printf("\n");
        stepsCounter++;
    }
    printf("\nSimulation ended after %d steps.\n", stepsCounter);

    return 0;
}