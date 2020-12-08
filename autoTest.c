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
    double expectedValue[3] = { 97.10, 96.91, 97.28 };

    simConfig config;

    config.partyChance = 5;
    config.groupSize[3].upperbound = 50;
    config.groupSize[3].lowerbound = 5;
    config.groupSize[3].expectedValue = (5 + 50) / 2;
    config.groupSize[3].varians = 1;
    config.partyRisk = 10;
    config.partyMeetChance = 10;

    /*Groups */
    config.primaryGroupRisk = 5;  /*can */
    config.secondaryGroupRisk = 5;  /*can */
    config.groupPercentageToInfect = 66;
    config.groupMaxAmountToMeet[0] = 10;  /*Prim */
    config.groupMaxAmountToMeet[1] = 5; /*second */
    config.groupMaxAmountToMeet[2] = 3; /*loos */
    config.groupMaxAmountToMeet[3] = 20;  /*party */
    config.groupSize[2].lowerbound = 0;
    config.groupSize[2].upperbound = 10;
    config.groupSize[2].varians = 1;
    config.groupSize[2].expectedValue = 5;
    config.groupSize[0].lowerbound = 10;
    config.groupSize[0].upperbound = 50;
    config.groupSize[0].varians = 1;
    config.groupSize[0].expectedValue =
        (config.groupSize[0].lowerbound +
         config.groupSize[0].upperbound) / 2;
    config.groupSize[1].lowerbound = 5;
    config.groupSize[1].upperbound = 30;
    config.groupSize[1].varians = 1;
    config.groupSize[1].expectedValue =
        (config.groupSize[1].lowerbound +
         config.groupSize[1].upperbound) / 2;
    config.passerbys.lowerbound = 0;
    config.passerbys.upperbound = 25;
    config.passerbys.varians = 20;
    config.passerbys.expectedValue = 7;

    /*App */
    config.chanceToHaveApp = 25;  /*can */
    config.btThreshold = 6;
    config.btDecay = 3;

    /*Infections */
    config.contactsRisk = 10;   /*can */
    config.infectionTime.lowerbound = 2;
    config.infectionTime.upperbound = 12;
    config.infectionTime.varians = 1;
    config.infectionTime.expectedValue = 4;
    config.amountOfStartInfected = 20;
    config.symptomaticPercent = 84; /*can */
    config.incubationTime.lowerbound = 1; /* CDC.gov */
    config.incubationTime.upperbound = 14;  /* CDC.gov */
    config.incubationTime.varians = 1;
    config.incubationTime.expectedValue = 5.1;  /* CDC.gov *//*can */

    config.isolationDelay.lowerbound = 1;
    config.isolationDelay.upperbound = 5;
    config.isolationDelay.varians = 4;
    config.isolationDelay.expectedValue = 2;

    /*Misc */
    config.simulationRuns = 1;
    config.maxEvents = 100;
    config.seed = 1;
    config.print = 0;
    config.amountOfAgents = 100000;
    config.willIsolatePercent = 90; /*can */
    config.willTestPercent = 60;  /*can */
    config.isolationTime = 7;   /*can */
    config.testResponseTime = 3;  /*can, this is a worst case */
    config.chanceOfCorrectTest = 95;


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
