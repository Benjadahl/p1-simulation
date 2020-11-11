#include <stdlib.h>
#include "simulation.h"
#include "export.h"
#include "graph.h"

int main(int argc, char *argv[])
{   
    int i;
    int j;
    int k;
    int seed;
    int par[7];
    char options[7];

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
    config.amountOfPrimaryGroups =
        config.amountOfAgents / config.primaryGroupSize;
    config.amountOfSecondaryGroups =
        config.amountOfAgents / config.secondaryGroupSize;

    double succeptible_data[config.maxEvents];
    double infectious_data[config.maxEvents];
    double recovered_data[config.maxEvents];

    j = 0;
    k = 0;
    /* indlaeser parametre */
    for (i = 0; i < argc; i++) {
        if (argv[i][0] == '-') {
            options[j] = argv[i][1];
            j++;
        } else if ((argv[i][0] > 47) && (argv[i][0] < 58)) {
            par[k] = atoi(argv[i]);
            k++;
        }
    }
    


    /* switch */
    for (i = 0; i < 7; i++) {
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

    run_simulation(config, succeptible_data, infectious_data,
                   recovered_data);
    ExportData(succeptible_data, infectious_data, recovered_data,
               config.maxEvents);
    CreatePlotFromCVS("out.csv");

    return 0;
}
