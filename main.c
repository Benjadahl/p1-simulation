#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include "simulation.h"
#include "export.h"

void CreatePlotFromCVS(char *file_name, simConfig config);

int main(int argc, char *argv[])
{
    int i;
    int j = 0;
    char convert = '0';
    int vaildInput = 0;
    int seed;
    int graf = 0;
    int *par;
    char *options;

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
    config.primaryGroupSize = 10;
    config.secondaryGroupSize = 10;
    config.primaryGroupRisk = 7;
    config.secondaryGroupRisk = 7;
    config.amountOfContacts = 5;

    options = malloc(sizeof(char) * (argc - 1));

    /* indlaeser parametre */
    for (i = 0; i < argc; i++) {
        if (argv[i][0] == '-') {
            *(options + j) = argv[i][1];
            j++;

            if (argv[j][1] != 'g' && !isdigit(argv[j + 1][0])) {
                printf
                    ("ERROR: Invaild inputs detected.\nMake sure that every option is follow by a value.\n Running simulation with default perameters.\n");
                vaildInput = 1;
                break;
            }
        } else if (isdigit(argv[i][0])) {
            *(options + j) = atoi(argv[i]);
            j++;

        }

    }

    if (vaildInput == 0) {

        /* Switch over command line options */
        for (i = 0, j = 0; i < (argc - 1); i++, j = i + 1) {

            switch (*(options + i)) {
            case 'z':          /*how many angents have sympums when infected */
                config.symptomaticPercent = *(options + j);
                break;

            case 'w':          /*chanc that angent will isolate */
                config.willIsolatePercent = *(options + j);
                break;

            case 'c':          /*risk of infetion */
                config.contactsRisk = *(options + j);
                break;

            case 'k':          /*amount of contacts pr agent */
                config.amountOfContacts = *(options + j);
                break;

            case 't':          /*size of primary group */
                config.primaryGroupSize = *(options + j);
                break;

            case 'y':          /*size of secound group */
                config.primaryGroupSize = *(options + j);
                break;

            case 'a':          /*amount of time incted */
                config.infectionTime = *(options + j);
                break;

            case 'p':          /*total amount of agents */
                config.amountOfAgents = *(options + j);
                break;

            case 'i':          /*amount of infected at start of simulation */
                config.amountOfStartInfected = *(options + j);
                break;

            case 'e':          /*lenght of simulation */
                config.maxEvents = *(options + j);
                break;

            case 's':          /*seed */
                config.seed = *(options + j);
                break;

            case 'g':
                graf = 1;
                break;
            }
        }
    }

    free(options);
    free(par);

    config.amountOfPrimaryGroups =
        config.amountOfAgents / config.primaryGroupSize;
    config.amountOfSecondaryGroups =
        config.amountOfAgents / config.secondaryGroupSize;
    config.groupPercentageToInfect = 74;

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
