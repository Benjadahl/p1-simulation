#include <stdlib.h>
#include <ctype.h>
#include "simulation.h"
#include "export.h"
#define maxOptions 7
void CreatePlotFromCVS(char *file_name, simConfig config);

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
    config.amountOfAgents = 10000;
    config.infectionTime = 4;
    config.amountOfStartInfected = 20;
    config.maxEvents = 100;
    config.seed = 0;
    config.primaryGroupSize = 40;
    config.secondaryGroupSize = 20;
    config.primaryGroupRisk = 1;
    config.secondaryGroupRisk = 1;
    config.amountOfContacts = 5;

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

    config.amountOfPrimaryGroups =
        config.amountOfAgents / config.primaryGroupSize;
    config.amountOfSecondaryGroups =
        config.amountOfAgents / config.secondaryGroupSize;

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
