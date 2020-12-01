#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <time.h>
#include "simulation.h"
#include "export.h"

int main(int argc, char *argv[])
{
    int i;
    int value;
    int graph = 0;
    time_t runTime;

    simConfig config;

    config.simulationRuns = 3;
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
    config.willIsolatePercent = 98;
    config.willTestPercent = 75;
    config.seed = 0;
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
                    config.infectionTime = value;
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
    double succeptible_data[config.maxEvents],
        infectious_data[config.maxEvents],
        recovered_data[config.maxEvents], exposed_data[config.maxEvents];

    double avg_succeptible_data[config.maxEvents],
        avg_infectious_data[config.maxEvents],
        avg_recovered_data[config.maxEvents],
        avg_exposed_data[config.maxEvents];

    runTime = time(NULL);

    for (i = 0; i < config.simulationRuns; i++) {
        run_simulation(config, succeptible_data, infectious_data,
                       recovered_data, exposed_data);
        ExportData(i, runTime, succeptible_data, infectious_data,
                   recovered_data, exposed_data, config);
        calculateAveragePlot(i, config.maxEvents, avg_succeptible_data,
                             avg_infectious_data, avg_recovered_data,
                             avg_exposed_data, succeptible_data,
                             infectious_data, recovered_data,
                             exposed_data);
    }
    if (graph != 0) {
        ExportData(-1, runTime, avg_succeptible_data, avg_infectious_data,
                   avg_recovered_data, avg_exposed_data, config);
    }
    return EXIT_SUCCESS;
}
