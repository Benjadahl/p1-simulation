#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include "distribution.h"
#include "simulation.h"
#include "export.h"

void run_simulation(simConfig config, DataSet * data, int dataCount);
int printCheck(int i, simConfig config, double input,
               double expectedValue);

int main()
{
    int seed;
    int i;
    int failures = 0;
    double results[3] = { 0, 0, 0 };
<<<<<<< HEAD
    double expectedValue[3] = { 98.50, 99.04, 98.97 };
=======
    double expectedValue[3] = { 97.10, 96.91, 97.28 };
>>>>>>> main

    simConfig config;

    config.simulationRuns = 1;
    config.contactsRisk = 1;
    config.amountOfAgents = 100000;
    config.infectionTime.lowerbound = 2;
    config.infectionTime.upperbound = 12;
    config.infectionTime.varians = 1;
    config.infectionTime.expectedValue = 4;
    config.amountOfStartInfected = 20;
    config.maxEvents = 100;
    config.symptomaticPercent = 25;
    config.incubationTime.lowerbound = 1; /* CDC.gov */
    config.incubationTime.upperbound = 14;  /* CDC.gov */
    config.incubationTime.varians = 1;
    config.incubationTime.expectedValue = 5.1;  /* CDC.gov */
    config.willIsolatePercent = 50;
    config.partyChance = 5;
    config.groupSize[3].upperbound = 50;
    config.groupSize[3].lowerbound = 5;
    config.groupSize[3].expectedValue = (5 + 50) / 2;
    config.groupSize[3].varians = 1;
    config.partyRisk = 75;
    config.partyMeetChance = 10;
    config.willTestPercent = 75;
    config.seed = 0;
    config.print = 0;
    config.primaryGroupRisk = 5;
    config.secondaryGroupRisk = 5;
    config.groupSize[2].lowerbound = 0;
    config.groupSize[2].upperbound = 10;
    config.groupSize[2].varians = 9;
    config.groupSize[2].expectedValue = 5;
    config.groupPercentageToInfect = 74;
    config.chanceToHaveApp = 35;
    config.isolationTime = 15;
    config.testResponseTime = 2;
    config.toMeet[0].upperbound = 10;
    config.toMeet[0].lowerbound = 0;
    config.toMeet[0].expectedValue = 10;
    config.toMeet[0].varians = 25;
    config.toMeet[1].upperbound = 20;
    config.toMeet[1].lowerbound = 0;
    config.toMeet[1].expectedValue = 5;
    config.toMeet[1].varians = 25;
    config.toMeet[2].upperbound = 3;
    config.toMeet[2].lowerbound = 0;
    config.toMeet[2].expectedValue = 2;
    config.toMeet[2].varians = 2;
    config.toMeet[3].upperbound = 20;
    config.toMeet[3].lowerbound = 0;
    config.toMeet[3].expectedValue = 10;
    config.toMeet[3].varians = 49;
    config.btThreshold = 6;
    config.btDecay = 3;
    config.chanceOfCorrectTest = 95;
<<<<<<< HEAD
    config.groupSize[0].lowerbound = 10;
    config.groupSize[0].upperbound = 50;
    config.groupSize[0].varians = 400;
    config.groupSize[0].expectedValue =
        (config.groupSize[0].lowerbound +
         config.groupSize[0].upperbound) / 2;
    config.groupSize[1].lowerbound = 5;
    config.groupSize[1].upperbound = 30;
    config.groupSize[1].varians = 100;
    config.groupSize[1].expectedValue =
        (config.groupSize[1].lowerbound +
         config.groupSize[1].upperbound) / 2;
=======
    config.primaryGroupSize.lowerbound = 10;
    config.primaryGroupSize.upperbound = 50;
    config.primaryGroupSize.varians = 1;
    config.primaryGroupSize.expectedValue =
        (config.primaryGroupSize.lowerbound +
         config.primaryGroupSize.upperbound) / 2;
    config.secondaryGroupSize.lowerbound = 5;
    config.secondaryGroupSize.upperbound = 30;
    config.secondaryGroupSize.varians = 1;
    config.secondaryGroupSize.expectedValue =
        (config.secondaryGroupSize.lowerbound +
         config.secondaryGroupSize.upperbound) / 2;
    config.passerbys.lowerbound = 0;
    config.passerbys.upperbound = 50;
    config.passerbys.varians = 1;
    config.passerbys.expectedValue = 25;

>>>>>>> main

    DataSet data[PLOT_COUNT];
    DataSet avgData[PLOT_COUNT];


    for (i = 0; i < PLOT_COUNT; i++) {
        data[i].data = calloc(config.maxEvents, sizeof(double));
        data[i].absoluteData = calloc(config.maxEvents, sizeof(double));
        avgData[i].data = calloc(config.maxEvents, sizeof(double));
        avgData[i].absoluteData = calloc(config.maxEvents, sizeof(double));
    }

    data[0].name = "Succeptible";
    data[1].name = "Exposed";
    data[2].name = "Infectious";
    data[3].name = "Recovered";
    data[4].name = "Isolated";

    for (i = 0; i < PLOT_COUNT; i++) {
        avgData[i].name = data[i].name;
    }

    srand(config.seed);

    for (i = 0; i < 3; i++) {
        config.amountOfAgents = 100 * pow(10, i + 1);
        run_simulation(config, data, PLOT_COUNT);
        results[i] = floor(data[3].data[config.maxEvents - 1] * 100) / 100;
        failures += printCheck(i, config, results[i], expectedValue[i]);
    }
    printf(" %d", failures);
    return failures;
}

int printCheck(int i, simConfig config, double input, double expectedValue)
{

    if (input == expectedValue) {
        printf("The result haven't changed in test %d\n", i + 1);
        return 0;
    } else {
        /* This prints the value that needs to be used as the check */
        printf("\nThis is the value of tick %d: %.2lf\n", config.maxEvents,
               input);
        printf(">> Program output an unexpected value in test %d <<\n",
               i + 1);
        return 1;
    }
}
