#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <time.h>
#include "distribution.h"
#include "simulation.h"
#include "export.h"

void run_simulation(simConfig config, DataSet * data, int dataCount);
void calculateAveragePlot(int run, int events, DataSet * data,
                          DataSet * avgData, int dataCount);

int main(int argc, char *argv[])
{
    int i;
    int value;
    int graph = 0;
    time_t runTime;

    simConfig config;

    int seedUsed;

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
    config.print = 1;
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

    /* indlaeser parametre */
    for (i = 0; i < argc; i++) {

        if (argv[i][0] == '-') {

            if (argv[i][1] != 'g' && !isdigit(argv[i + 1][0])) {
                printf
                    ("ERROR: Invaild inputs detected.\nMake sure that every option is follow by a value.\nInvaild argument %c\n",
                     argv[i][1]);
                return EXIT_FAILURE;
            } else {

                if (argv[i][1] != 'g' && isdigit(argv[i + 1][0])
                    && i + 1 < argc) {
                    value = atoi(argv[i + 1]);
                }

                switch (argv[i][1]) {
                case 'z':      /*how many angents have sympums when infected */
                    config.symptomaticPercent = value;
                    break;

                case 'w':      /*chanc that angent will isolate */
                    config.willIsolatePercent = value;
                    break;

                case 'c':      /*risk of infetion */
                    config.contactsRisk = value;
                    break;

                case 'k':      /*amount of contacts pr agent */
                    config.amountOfContacts = value;
                    break;

                case 't':      /*size of primary group */
                    config.groupSize[0].expectedValue = value;
                    break;

                case 'y':      /*size of secound group */
                    config.groupSize[1].expectedValue = value;
                    break;

                case 'a':      /*amount of time incted */
                    config.infectionTime.expectedValue = value;
                    break;

                case 'p':      /*total amount of agents */
                    config.amountOfAgents = value;
                    break;

                case 'i':      /*amount of infected at start of simulation */
                    config.amountOfStartInfected = value;
                    break;

                case 'e':      /*lenght of simulation */
                    config.maxEvents = value;
                    break;

                case 's':      /*seed */
                    config.seed = value;
                    break;

                case 'g':
                    graph = 1;
                    break;
                }
            }
        }

    }

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
    data[5].name = "Healthy isolated";
    data[6].name = "Exosed & infectious isolated";

    for (i = 0; i < PLOT_COUNT; i++) {
        avgData[i].name = data[i].name;
    }

    runTime = time(NULL);

    if (!config.seed) {
        srand(runTime);
        seedUsed = runTime;
    } else {
        srand(config.seed);
        seedUsed = config.seed;
    }

    for (i = 0; i < config.simulationRuns; i++) {
        run_simulation(config, data, PLOT_COUNT);
        ExportData(i, runTime, data, PLOT_COUNT, config.maxEvents,
                   config.amountOfAgents, 1);
        ExportData(i, runTime, data, PLOT_COUNT, config.maxEvents, 100, 0);
        calculateAveragePlot(i, config.maxEvents, data, avgData,
                             PLOT_COUNT);
    }

    printf("\nSeed used: %d\n", seedUsed);

    if (graph != 0) {
        printf("\nPlotting graph...\n");
        ExportData(-1, runTime, avgData, PLOT_COUNT, config.maxEvents, 100,
                   0);
    }
    for (i = 0; i < PLOT_COUNT; i++) {
        free(data[i].data);
        free(data[i].absoluteData);
        free(avgData[i].absoluteData);
        free(avgData[i].data);
    }
    return EXIT_SUCCESS;
}
