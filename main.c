#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include "simulation.h"
#include "export.h"

void CreatePlotFromCVS(char *file_name, simConfig config);

int main(int argc, char *argv[])
{
    int i;
    int graf = 0;

    simConfig config;

    config.contactsRisk = 1;
    config.amountOfAgents = 100000;
    config.infectionTime = 4;
    config.amountOfStartInfected = 20;
    config.maxEvents = 100;
    config.symptomaticPercent = 25;
    config.maxIncubationTime = 14;
    config.willIsolatePercent = 98;
    config.seed = 0;
    config.groupSize[0] = 15;
    config.groupSize[1] = 10;
    config.primaryGroupRisk = 5;
    config.secondaryGroupRisk = 5;
    config.amountOfContactsPerAgent = 5;
    config.groupPercentageToInfect = 74;

    /* indlaeser parametre */
    for (i = 0; i < argc; i++) {
        if (argv[i][0] == '-') {

            if (argv[i][1] != 'g' && !isdigit(argv[i + 1][0])) {
                printf
                    ("ERROR: Invaild inputs detected.\nMake sure that every option is follow by a value.\n Running simulation with default perameters.\n");
                break;
            }

            switch (argv[i][1]) {
            case 'z':          /*how many angents have sympums when infected */
                config.symptomaticPercent = atoi(argv[i + 1]);
                break;

            case 'w':          /*chanc that angent will isolate */
                config.willIsolatePercent = atoi(argv[i + 1]);
                break;

            case 'c':          /*risk of infetion */
                config.contactsRisk = atoi(argv[i + 1]);
                break;

            case 'k':          /*amount of contacts pr agent */
                config.amountOfContacts = atoi(argv[i + 1]);
                break;

            case 't':          /*size of primary group */
                config.groupSize[0] = atoi(argv[i + 1]);
                break;

            case 'y':          /*size of secound group */
                config.groupSize[1] = atoi(argv[i + 1]);
                break;

            case 'a':          /*amount of time incted */
                config.infectionTime = atoi(argv[i + 1]);
                break;

            case 'p':          /*total amount of agents */
                config.amountOfAgents = atoi(argv[i + 1]);
                break;

            case 'i':          /*amount of infected at start of simulation */
                config.amountOfStartInfected = atoi(argv[i + 1]);
                break;

            case 'e':          /*lenght of simulation */
                config.maxEvents = atoi(argv[i + 1]);
                break;

            case 's':          /*seed */
                config.seed = atoi(argv[i + 1]);
                break;

            case 'g':
                graf = 1;
                break;
            }
        }

    }



    double succeptible_data[config.maxEvents];
    double infectious_data[config.maxEvents];
    double recovered_data[config.maxEvents];

    run_simulation(config, succeptible_data, infectious_data,
                   recovered_data);
    if (graf != 0) {
        ExportData(succeptible_data, infectious_data, recovered_data,
                   config.maxEvents);
        CreatePlotFromCVS("out.csv", config);
    }
    return 0;
}
