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
    int seedUsed;
    time_t runTime;

    simConfig config;

    /*party */
    config.partyChance = 5;
    config.partyDist.upperbound = 50;
    config.partyDist.lowerbound = 5;
    config.partyDist.expectedValue = (5 + 50) / 2;
    config.partyDist.varians = 1;
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
    config.amountOfContactsPerAgent.lowerbound = 0;
    config.amountOfContactsPerAgent.upperbound = 10;
    config.amountOfContactsPerAgent.varians = 1;
    config.amountOfContactsPerAgent.expectedValue = 5;
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

    /*Misc */
    config.simulationRuns = 1;
    config.maxEvents = 100;
    config.seed = 0;
    config.print = 1;
    config.amountOfAgents = 100000;
    config.willIsolatePercent = 90; /*can */
    config.willTestPercent = 60;  /*can */
    config.isolationTime = 7;   /*can */
    config.testResponseTime.upperbound = 3; /*can, this is a worst case */
    config.testResponseTime.lowerbound = 1;
    config.testResponseTime.expectedValue = 2;
    config.testResponseTime.varians = 1;
    config.chanceOfCorrectTest = 95;


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
                    config.groupSize[0] = value;
                    break;

                case 'y':      /*size of secound group */
                    config.groupSize[1] = value;
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
