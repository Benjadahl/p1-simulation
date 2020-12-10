#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <time.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <limits.h>
#include "simulation.h"
#include "export.h"

void run_simulation(gsl_rng * r, simConfig config, DataSet * data,
                    int dataCount);
void calculateAveragePlot(int run, int events, DataSet * data,
                          DataSet * avgData, int dataCount);
void ExportData(int run, time_t runTime, DataSet * dataSets, int dataCount,
                int events, int yMax, int abosolute, simConfig simConfig);
int isValueCorrect(char input, int value, int min, int max);


int main(int argc, char *argv[])
{
    int i;
    int value;
    int graph = 0;
    int seedUsed;
    time_t runTime;

    DataSet data[PLOT_COUNT];
    DataSet avgData[PLOT_COUNT];

    const gsl_rng_type *T;
    gsl_rng *r;

    simConfig config;

    /*party */
    config.partyChance = 16;
    config.partyRisk = 0.15;

    /*Groups */
    config.primaryGroupRisk = 0.05; /*can */
    config.secondaryGroupRisk = 0.025;  /*can */
    config.contactsRisk = 0.03; /*can */
    config.groupSize[0].lowerbound = 5;
    config.groupSize[0].upperbound = 50;
    config.groupSize[0].varians = 400;
    config.groupSize[0].expectedValue =
        (config.groupSize[0].lowerbound +
         config.groupSize[0].upperbound) / 2;
    config.groupSize[1].lowerbound = 5;
    config.groupSize[1].upperbound = 30;
    config.groupSize[1].varians = 100;
    config.groupSize[1].expectedValue =
        (config.groupSize[1].lowerbound +
         config.groupSize[1].upperbound) / 2;
    config.groupSize[2].lowerbound = 2;
    config.groupSize[2].upperbound = 10;
    config.groupSize[2].varians = 9;
    config.groupSize[2].expectedValue = 5;
    config.groupSize[3].upperbound = 50;
    config.groupSize[3].lowerbound = 2;
    config.groupSize[3].expectedValue = 6;
    config.groupSize[3].varians = 9;
    config.toMeet[0].upperbound = 10;
    config.toMeet[0].lowerbound = 0;
    config.toMeet[0].expectedValue = 5;
    config.toMeet[0].varians = 9;
    config.toMeet[1].upperbound = 20;
    config.toMeet[1].lowerbound = 0;
    config.toMeet[1].expectedValue = 3;
    config.toMeet[1].varians = 9;
    config.toMeet[2].upperbound = 3;
    config.toMeet[2].lowerbound = 0;
    config.toMeet[2].expectedValue = 1;
    config.toMeet[2].varians = 2;
    config.toMeet[3].upperbound = 15;
    config.toMeet[3].lowerbound = 0;
    config.toMeet[3].expectedValue = 3;
    config.toMeet[3].varians = 9;
    config.passerbys.lowerbound = 0;
    config.passerbys.upperbound = 25;
    config.passerbys.varians = 8;
    config.passerbys.expectedValue = 4;

    /*App */
    config.chanceToHaveApp = 0; /*can */
    config.btThreshold = 0;
    config.btDecay = 7;

    /*Infections */
    config.infectionTime.lowerbound = 2;
    config.infectionTime.upperbound = 12;
    config.infectionTime.varians = 1;
    config.infectionTime.expectedValue = 4;
    config.amountOfStartInfected = 20;
    config.symptomaticPercent = 0.84; /*can */
    config.incubationTime.lowerbound = 1; /* CDC.gov */
    config.incubationTime.upperbound = 14;  /* CDC.gov */
    config.incubationTime.varians = 1;
    config.incubationTime.expectedValue = 5.1;  /* CDC.gov *//*can */
    config.isolationDelay.upperbound = 5;
    config.isolationDelay.lowerbound = 0;
    config.isolationDelay.expectedValue = 1;
    config.isolationDelay.varians = 1;

    /*Misc */
    config.simulationRuns = 1;
    config.maxEvents = 100;
    config.seed = 0;
    config.print = 1;
    config.amountOfAgents = 100000;
    config.willIsolatePercent = 0.9;  /*can */
    config.willTestPercent = 0.6; /*can */
    config.testResponseTime.upperbound = 3; /*can, this is a worst case */
    config.testResponseTime.lowerbound = 1;
    config.testResponseTime.expectedValue = 2;
    config.testResponseTime.varians = 1;
    config.chanceOfCorrectTest = 0.95;
    config.passerByRisk = 0.0048;
    config.makeConfigFile = 1;
    config.dataLabel = 1;

    /* indlaeser parametre */
    for (i = 0; i < argc; i++) {
        /*grunden til at vi har valgt at der skal være et '-' foran 
           en parameterinstilling er for at gøre det nemmer at opsamle i vores program */
        if (argv[i][0] == '-') {
            /*grunden til at vi har denne if statment er fordi at både 'g' og 'b'
               skal ikke efterføgles af en værdi, så for at sikre os at det inputs
               som skal have en værdi tilknyttede til sig har det bruge vi dette */
            if ((argv[i][1] == 'g' || argv[i][1] == 'b')
                && !isdigit(argv[i + 1][0])) {
                printf
                    ("ERROR: Invaild inputs detected.\nMake sure that every option is follow by a value.\nInvaild argument %c\n",
                     argv[i][1]);
                return EXIT_FAILURE;
            } else {
                /*dette sikre os at den ikke prøver at få fat i en værdi som ikke eksistere */
                if (i + 1 < argc) {
                    value = atoi(argv[i + 1]);
                }

                switch (argv[i][1]) {
                case 'z':      /*how many angents have sympums when infected */
                    if (isValueCorrect(argv[i][1], value, 0, 1)) config.symptomaticPercent = value;
                    else return 0;
                    break;

                case 'w':      /*chanc that angent will isolate */
                    if (isValueCorrect(argv[i][1], value, 0, 1)) config.willIsolatePercent = value;
                    else return 0;
                    break;

                case 'c':      /*risk of infetion */
                    if (isValueCorrect(argv[i][1], value, 0, 1)) config.contactsRisk = value;
                    else return 0;
                    break;

                case 'k':      /*amount of contacts pr agent */
                    if (isValueCorrect(argv[i][1], value, config.groupSize[2].lowerbound, config.groupSize[2].upperbound)) config.groupSize[2].expectedValue = value;
                    else return 0;
                    break;

                case 't':      /*size of primary group */
                    if (isValueCorrect(argv[i][1], value, config.groupSize[0].lowerbound, config.groupSize[0].upperbound)) config.groupSize[0].expectedValue = value;
                    else return 0;
                    break;

                case 'y':      /*size of secound group */
                    if (isValueCorrect(argv[i][1], value, config.groupSize[1].lowerbound, config.groupSize[1].upperbound)) config.groupSize[1].expectedValue = value;
                    else return 0;
                    break;

                case 'a':      /*amount of time incted */
                    if (isValueCorrect(argv[i][1], value, config.infectionTime.lowerbound, config.infectionTime.upperbound)) config.infectionTime.expectedValue = value;
                    else return 0;
                    break;

                case 'p':      /*total amount of agents */
                    if (isValueCorrect(argv[i][1], value, 1, INT_MAX)) config.amountOfAgents = value;
                    else return 0;
                    break;

                case 'i':      /*amount of infected at start of simulation */
                    if (isValueCorrect(argv[i][1], value, 1, INT_MAX)) config.amountOfStartInfected = value;
                    else return 0;
                    break;

                case 'e':      /*lenght of simulation */
                    if (isValueCorrect(argv[i][1], value, 0, INT_MAX)) config.maxEvents = value;
                    else return 0;
                    break;

                case 's':      /*seed */
                    if (isValueCorrect(argv[i][1], value, 0, INT_MAX)) config.seed = value;
                    else return 0;
                    break;

                case 'g':
                    graph = 1;
                    break;

                case 'b':
                    config.groupSize[0].lowerbound = 2;
                    config.groupSize[0].upperbound = 10;
                    config.groupSize[0].varians = 6;
                    config.groupSize[0].expectedValue = 5;
                    config.groupSize[1].lowerbound = 2;
                    config.groupSize[1].upperbound = 10;
                    config.groupSize[1].varians = 6;
                    config.groupSize[1].expectedValue = 5;
                    config.groupSize[2].lowerbound = 0;
                    config.groupSize[2].upperbound = 10;
                    config.groupSize[2].varians = 9;
                    config.groupSize[2].expectedValue = 5;
                    config.groupSize[3].upperbound = 20;
                    config.groupSize[3].lowerbound = 2;
                    config.groupSize[3].expectedValue = 6;
                    config.groupSize[3].varians = 9;
                    break;

                case 'd':
                    if (isValueCorrect(argv[i][1], value, 0, 1)) config.chanceToHaveApp = value;
                    else return 0;
                    break;

                case 'f':
                    if (isValueCorrect(argv[i][1], value, 0, INT_MAX)) config.btThreshold = value;
                    else return 0;
                    break;

                case 'h':
                    if (isValueCorrect(argv[i][1], value, 0, INT_MAX)) config.btDecay = value;
                    else return 0;
                    break;

                case 'j':
                    if (isValueCorrect(argv[i][1], value, 0, INT_MAX)) config.simulationRuns = value;
                    else return 0;
                    break;

                case 'l':
                    if (isValueCorrect(argv[i][1], value, 0, INT_MAX)) config.dataLabel = value;
                    else return 0;
                    break;
                }
            }
        }

    }

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
    data[5].name = "Healthy isolated";
    data[6].name = "Exposed & infectious isolated";

    for (i = 0; i < PLOT_COUNT; i++) {
        avgData[i].name = data[i].name;
    }

    runTime = time(NULL);

    if (!config.seed) {
        seedUsed = runTime;
    } else {
        seedUsed = config.seed;
    }

    /* Setup GSL */
    gsl_rng_env_setup();

    T = gsl_rng_default;
    r = gsl_rng_alloc(T);
    gsl_rng_set(r, seedUsed);

    for (i = 0; i < config.simulationRuns; i++) {
        run_simulation(r, config, data, PLOT_COUNT);
        ExportData(i, runTime, data, PLOT_COUNT, config.maxEvents,
                   config.amountOfAgents, 1, config);
        ExportData(i, runTime, data, PLOT_COUNT, config.maxEvents, 100, 0,
                   config);
        calculateAveragePlot(i, config.maxEvents, data, avgData,
                             PLOT_COUNT);
    }

    gsl_rng_free(r);

    printf("\nSeed used: %d\n", seedUsed);

    if (graph != 0) {
        printf("\nPlotting graph...\n");
        ExportData(-1, runTime, avgData, PLOT_COUNT, config.maxEvents, 100,
                   0, config);
    }
    for (i = 0; i < PLOT_COUNT; i++) {
        free(data[i].data);
        free(data[i].absoluteData);
        free(avgData[i].absoluteData);
        free(avgData[i].data);
    }
    return EXIT_SUCCESS;
}

int isValueCorrect(char input, int value, int min, int max){
    if (value >= min && value <= max) return 1;
    else printf("-%c %d is not accepted, the input value of paramter -%c must be in the interval [%d, %d]\n", input, value, input, min, max); return 0;
}