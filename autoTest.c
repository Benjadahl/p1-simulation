#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>
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
    double expectedValue[3] = { 90.40, 88.04, 68.32 };

    simConfig config;

    config.simulationRuns = 1;
    config.contactsRisk = 1;
    config.amountOfAgents = 100000;
    config.infectionTime = 4;
    config.amountOfStartInfected = 20;
    config.maxEvents = 100;
    config.symptomaticPercent = 25;
    config.maxIncubationTime = 14;
    config.willIsolatePercent = 50;
    config.partyChance = 5;
    config.maxPartySize = 50;
    config.minPartySize = 5;
    config.partyRisk = 75;
    config.partyMeetChance = 10;
    config.willTestPercent = 75;
    config.seed = 1;
    config.print = 0;
    config.groupSize[0] = 15;
    config.groupSize[1] = 10;
    config.primaryGroupRisk = 5;
    config.secondaryGroupRisk = 5;
    config.amountOfContactsPerAgent = 5;
    config.groupPercentageToInfect = 74;
    config.chanceToHaveApp = 35;
    config.isolationTime = 15;
    config.testResponseTime = 2;
    config.groupMaxAmountToMeet[0] = 10;
    config.groupMaxAmountToMeet[1] = 5;
    config.groupMaxAmountToMeet[2] = 3;
    config.groupMaxAmountToMeet[3] = 20;
    config.btThreshold = 6;
    config.btDecay = 3;
    config.groupSizeMaxMin[0] = 10;
    config.groupSizeMaxMin[1] = 50;
    config.groupSizeMaxMin[2] = 5;
    config.groupSizeMaxMin[3] = 30;


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
        printf("The result haven't changed in test %d\n", i);
        return 0;
    } else {
        /* This prints the value that needs to be used as the check */
        printf("\nThis is the value of tick %d: %.2lf\n", config.maxEvents,
               input);
        printf(">> Program output an unexpected value in test %d <<\n", i);
        return 1;
    }
}
