#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include "simulation.h"
#include "export.h"

void CreatePlotFromCVS(char *file_name, simConfig config);

int main(int argc, char *argv[])
{
    int i;
    int j = 0;
    int k = 0;
    int seed;
    int *parSize;
    char *optionsSize;



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
    config.primaryGroupSize = 40;
    config.secondaryGroupSize = 20;
    config.primaryGroupRisk = 1;
    config.secondaryGroupRisk = 1;
    config.amountOfContacts = 5;


    optionsSize = malloc(sizeof(char) * floor(argc / 2));
    parSize = malloc(sizeof(int) * floor(argc / 2));

    if (argc % 2 == 0) {
        printf
            ("ERROR: Invaild inputs detected.\nMake sure that every option is follow by a value.\n");
        return 0;
    }

    /* indlaeser parametre */
    for (i = 0; i < argc; i++) {
        if (argv[i][0] == '-') {
            *(optionsSize + j) = argv[i][1];
            j++;
        } else if (isdigit(argv[i][0])) {
            *(parSize + k) = atoi(argv[i]);
            k++;
        }

    }


    /* Switch over command line options */
    for (i = 0; i < floor(argc / 2); i++) {
        switch (*(optionsSize + i)) {
        case 'z':              /*how many angents have sympums when infected */
            config.symptomaticPercent = *(parSize + i);
            break;

        case 'w':              /*chanc that angent will isolate */
            config.willIsolatePercent = *(parSize + i);
            break;

        case 'c':              /*risk of infetion */
            config.contactsRisk = *(parSize + i);
            break;

        case 'k':              /*amount of contacts pr agent */
            config.amountOfContacts = *(parSize + i);
            break;

        case 't':              /*size of primary group */
            config.primaryGroupSize = *(parSize + i);
            break;

        case 'y':              /*size of secound group */
            config.primaryGroupSize = *(parSize + i);
            break;

        case 'a':              /*amount of time incted */
            config.infectionTime = *(parSize + i);
            break;

        case 'p':              /*total amount of agents */
            config.amountOfAgents = *(parSize + i);
            break;

        case 'i':              /*amount of infected at start of simulation */
            config.amountOfStartInfected = *(parSize + i);
            break;

        case 'e':              /*lenght of simulation */
            config.maxEvents = *(parSize + i);
            break;

        case 's':              /*seed */
            config.seed = *(parSize + i);
            break;
        }

    }
    free(optionsSize);
    free(parSize);


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
    ExportData(succeptible_data, infectious_data, recovered_data,
               config.maxEvents);
    CreatePlotFromCVS("out.csv", config);

    return 0;
}
