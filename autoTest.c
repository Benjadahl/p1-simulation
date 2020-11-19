#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include "simulation.h"

void printCheck(int i, simConfig config, double output[],
                double expectedValue);

int main()
{
    int seed;
    int i;
    double expectedValue[3] = { 80.3, 79.6, 79.7 };
    double results[3] = { 0, 0, 0 };

    simConfig config;

    config.contactsRisk = 1;
    config.amountOfAgents = 0;
    config.infectionTime = 4;
    config.amountOfStartInfected = 20;
    config.maxEvents = 100;
    config.symptomaticPercent = 25;
    config.maxIncubationTime = 14;
    config.willIsolatePercent = 98;
    config.seed = 0;
    config.print = 0;
    config.groupSize[0] = 15;
    config.groupSize[1] = 10;
    config.primaryGroupRisk = 5;
    config.secondaryGroupRisk = 5;
    config.amountOfContactsPerAgent = 5;
    config.groupPercentageToInfect = 74;

    double succeptible_data_test[config.maxEvents];
    double infectious_data_test[config.maxEvents];
    double recovered_data_test[config.maxEvents];

    for (i = 0; i < 3; i++) {
        config.amountOfAgents = 1000 * pow(10, i + 1);
        run_simulation(config, succeptible_data_test, infectious_data_test,
                       recovered_data_test);
        results[i] =
            floor(recovered_data_test[config.maxEvents - 1] * 100) / 100;
        printCheck(i, config, results, expectedValue[i]);
    }

    return 0;
}

void printCheck(int i, simConfig config, double output[],
                double expectedValue)
{

    if (output[i] != expectedValue) {
        /* This prints the value that needs to be used as the check */
        printf("\nThis is the value of tick %d: %lf\n", config.maxEvents,
               output[i]);
        printf(">> Program output an unexpected value in test %d <<\n", i);
    } else if (output[i] == expectedValue) {
        printf("The result haven't changed in test %d\n", i);
    } else {
        printf("FATAL ERROR IN TEST %d\n", i);
    }
}
