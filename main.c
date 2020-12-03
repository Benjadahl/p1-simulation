#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <time.h>
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
    config.maxIncubationTime = 14;
    config.willIsolatePercent = 50;
    config.partyChance = 5;
    config.partyDist.upperbound = 50;
    config.partyDist.lowerbound = 5;
    config.partyDist.expectedValue = (5 + 50) / 2;
    config.partyDist.varians = 1;
    config.partyRisk = 75;
    config.partyMeetChance = 10;
    config.willTestPercent = 75;
    config.seed = 0;
    config.print = 1;
    config.groupSize[0] = 15;
    config.groupSize[1] = 10;
    config.primaryGroupRisk = 5;
    config.secondaryGroupRisk = 5;
    config.amountOfContactsPerAgent = 5;
    config.groupPercentageToInfect = 74;
    config.chanceToHaveApp = 35;
    config.contactTickLength = 7;
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

    for (i = 0; i < PLOT_COUNT; i++) {
        avgData[i].name = data[i].name;
    }

    runTime = time(NULL);

    for (i = 0; i < config.simulationRuns; i++) {
        run_simulation(config, data, PLOT_COUNT);
        ExportData(i, runTime, data, PLOT_COUNT, config.maxEvents,
                   config.amountOfAgents, 1);
        ExportData(i, runTime, data, PLOT_COUNT, config.maxEvents, 100, 0);
        calculateAveragePlot(i, config.maxEvents, data, avgData,
                             PLOT_COUNT);
    }
    if (graph != 0) {
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
