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
    int k = 0;
    int chk = 0;
    int seed;
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


    options = malloc(sizeof(char) * (argc-1)/2);
    par = malloc(sizeof(int) * (argc-1)/2);

    if (argc%2 ==0)
    {
        printf("ERROR: Invaild inputs detected.\nMake sure that every option is follow by a value.\n Running simulation with default perameters.\n");
        chk = 1;
    }
    if (chk == 0)
    {

        /* indlaeser parametre */
        for (i = 0; i < argc; i++) {
            if (argv[i][0] == '-') {
                *(options + j) = argv[i][1];
                j++;
            } else if (isdigit(argv[i][0])) {
                *(par + k) = atoi(argv[i]);
                k++;
            }
            
        }
        

        /* Switch over command line options */
        for (i = 0; i < (argc-1)/2; i++) {
            switch (*(options + i)) {
            case 'z':/*how many angents have sympums when infected*/
                config.symptomaticPercent = *(par +i);
                break;

            case 'w': /*chanc that angent will isolate*/
                config.willIsolatePercent = *(par +i);
                break;

            case 'c':/*risk of infetion*/
                config.contactsRisk = *(par +i);
                break;

            case 'k':/*amount of contacts pr agent*/
                config.amountOfContacts = *(par +i);
                break;

            case 't':/*size of primary group*/
                config.primaryGroupSize = *(par +i);
                break;
            
            case 'y':/*size of secound group*/
                config.primaryGroupSize = *(par +i);
                break;

            case 'a':/*amount of time incted*/
                config.infectionTime = *(par +i);
                break;

            case 'p':/*total amount of agents*/
                config.amountOfAgents = *(par +i);
                break;

            case 'i':/*amount of infected at start of simulation*/
                config.amountOfStartInfected = *(par +i);
                break;

            case 'e':/*lenght of simulation*/
                config.maxEvents = *(par +i);
                break;

            case 's':/*seed*/
                config.seed = *(par +i);
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
    ExportData(succeptible_data, infectious_data, recovered_data,
               config.maxEvents);
    CreatePlotFromCVS("out.csv", config);

    return 0;
}
