#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include "simulation.h"

void printCheck(int i, int *failures, simConfig config, double output[],
                double expectedValue);

int main()
{
    int seed;
    int i;
    int failures = 0;
    double expectedValue[3] = { 79.09, 80.37, 79.62 };
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
    config.seed = 1;
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
        config.amountOfAgents = 100 * pow(10, i + 1);
        runSimulation(config, succeptible_data_test, infectious_data_test,
                      recovered_data_test);
        results[i] =
            floor(recovered_data_test[config.maxEvents - 1] * 100) / 100;
        printCheck(i, &failures, config, results, expectedValue[i]);
    }

    return failures;
}

void printCheck(int i, int *f, simConfig config, double output[],
                double expectedValue)
{

    if (output[i] == expectedValue) {
        printf("The result haven't changed in test %d\n", i);
    } else {
        /* This prints the value that needs to be used as the check */
        printf("\nThis is the value of tick %d: %.2lf\n", config.maxEvents,
               output[i]);
        printf(">> Program output an unexpected value in test %d <<\n", i);
        (*f)++;
    }
}
