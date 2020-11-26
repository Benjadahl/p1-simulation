#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "simulation.h"
#include "export.h"


void CreatePlotFromCVS(char *file_name,char *output_name, simConfig config);

int main(int argc, char *argv[])
{
    int i;
    int value;
    int graph = 0;
    char foldername[90];
    char filename[100], graphname[100];
    time_t runTime;
    struct tm *currentTime;

    simConfig config;

    config.simulationRuns = 5;
    config.contactsRisk = 1;
    config.amountOfAgents = 100000;
    config.infectionTime = 4;
    config.amountOfStartInfected = 20;
    config.maxEvents = 100;
    config.symptomaticPercent = 25;
    config.maxIncubationTime = 14;
    config.willIsolatePercent = 50;
    config.ParyChance = 5;
    config.maxPartySize = 50;
    config.minPartySize = 5;
    config.partyTransmissionChance = 75;
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

    /* indlaeser parametre */
    for (i = 0; i < argc; i++) {

        if (argv[i][0] == '-') {

            if (argv[i][1] != 'g' && !isdigit(argv[i + 1][0])) {
                printf
                    ("ERROR: Invaild inputs detected.\nMake sure that every option is follow by a value.\nRunning simulation with user perameters op to %c.\nAlle other inputs will be defualt perameters.\n",
                     argv[i][1]);
                break;
            }
            value = atoi(argv[i + 1]);
            switch (argv[i][1]) {
            case 'z':          /*how many angents have sympums when infected */
                config.symptomaticPercent = value;
                break;

            case 'w':          /*chanc that angent will isolate */
                config.willIsolatePercent = value;
                break;

            case 'c':          /*risk of infetion */
                config.contactsRisk = value;
                break;

            case 'k':          /*amount of contacts pr agent */
                config.amountOfContacts = value;
                break;

            case 't':          /*size of primary group */
                config.groupSize[0] = value;
                break;

            case 'y':          /*size of secound group */
                config.groupSize[1] = value;
                break;

            case 'a':          /*amount of time incted */
                config.infectionTime = value;
                break;

            case 'p':          /*total amount of agents */
                config.amountOfAgents = value;
                break;

            case 'i':          /*amount of infected at start of simulation */
                config.amountOfStartInfected = value;
                break;

            case 'e':          /*lenght of simulation */
                config.maxEvents = value;
                break;

            case 's':          /*seed */
                config.seed = value;
                break;

            case 'g':
                graph = 1;
                break;
            }
        }

    }



    double succeptible_data[config.maxEvents];
    double infectious_data[config.maxEvents];
    double recovered_data[config.maxEvents];

    double avg_succeptible_data[config.maxEvents];
    double avg_infectious_data[config.maxEvents];
    double avg_recovered_data[config.maxEvents];

    for (i = 0; i < config.maxEvents; i++)
    {
        avg_succeptible_data[i] = 0;
        avg_infectious_data[i] = 0;
        avg_recovered_data[i] = 0;
    }
    

    runTime = time(NULL);
    currentTime = localtime(&runTime);;
    sprintf(foldername, "output/H%02dM%02dS%02d-%02d-%02d-%d",  currentTime->tm_hour, currentTime->tm_min, currentTime->tm_sec, currentTime->tm_mday, currentTime->tm_mon+1, currentTime->tm_year- 100);
    mkdir(foldername, 0777);
    for (i = 0; i < config.simulationRuns; i++)
    {
        sprintf(filename, "%s/%d.csv", foldername, i);
        run_simulation(config, succeptible_data, infectious_data,
                   recovered_data);
        ExportData(filename, succeptible_data, infectious_data, recovered_data,
                    config.maxEvents);
        
        if(i == 0){
            avg_succeptible_data[i] = succeptible_data[i];
            avg_infectious_data[i] = infectious_data[i];
            avg_recovered_data[i] = recovered_data[i];
        }
        else{
            avg_succeptible_data[i] = (avg_succeptible_data[i] + succeptible_data[i]) / 2;
            avg_infectious_data[i] = (avg_succeptible_data[i] + infectious_data[i]) / 2;
            avg_recovered_data[i] = (avg_succeptible_data[i] + recovered_data[i]) / 2;
        }
    }
    if (graph != 0) {
        sprintf(filename, "%s/avg.csv", foldername);
        ExportData(filename, succeptible_data, infectious_data, recovered_data,
                    config.maxEvents);
        sprintf(graphname, "%s/avg-graph.png", foldername);
        CreatePlotFromCVS(filename,graphname, config);
    }
    return 0;
}
