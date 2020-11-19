#include <stdlib.h>
#include <ctype.h>
#include "simulation.h"
#include "export.h"
#define maxOptions 7
void createPlotFromCVS(char *file_name, simConfig config);

int main(int argc, char *argv[])
{
    int i;
    int j = 0;
    int k = 0;
    int seed;
    int par[maxOptions];
    char options[maxOptions];
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
    config.print = 1;           /* Printer ikke hvis den er lig 0 */
    config.groupSize[0] = 15;
    config.groupSize[1] = 10;
    config.primaryGroupRisk = 5;
    config.secondaryGroupRisk = 5;
    config.amountOfContactsPerAgent = 5;
    config.groupPercentageToInfect = 74;

    /* indlaeser parametre */
    for (i = 0; i < argc; i++) {
        if (argv[i][0] == '-') {
            options[j] = argv[i][1];
            j++;
        } else if (isdigit(argv[i][0])) {
            par[k] = atoi(argv[i]);
            k++;
        }
    }

    /* Switch over command line options */
    for (i = 0; i < maxOptions; i++) {
        switch (options[i]) {
        case 'c':
            config.contactsRisk = par[i];
            break;

        case 'k':
            config.amountOfContacts = par[i];
            break;

        case 'a':
            config.infectionTime = par[i];
            break;

        case 'p':
            config.amountOfAgents = par[i];
            break;

        case 'i':
            config.amountOfStartInfected = par[i];
            break;

        case 't':
            config.maxEvents = par[i];
            break;

        case 's':
            config.seed = par[i];
            break;
        }

    }



    double succeptible_data[config.maxEvents];
    double infectious_data[config.maxEvents];
    double recovered_data[config.maxEvents];

    runSimulation(config, succeptible_data, infectious_data,
                  recovered_data);
    exportData(succeptible_data, infectious_data, recovered_data,
               config.maxEvents);
    createPlotFromCVS("out.csv", config);

    return 0;
}
