#include "main.h"

int main(int argc, char *argv[])
{
    int i;
    int graph = 0;
    int seedUsed;
    double value;
    char *emPtr;
    time_t runTime;

    DataSet data[PLOT_COUNT];
    DataSet avgData[PLOT_COUNT];

    const gsl_rng_type *T;
    gsl_rng *r;

    simConfig config;

    initStandardConfig(&config);

    /*party */
    config.partyChance = 0.16;
    config.partyRisk = 0.15;


    /* indlaeser parametre */
    for (i = 0; i < argc; i++) {
        /*grunden til at vi har valgt at der skal være et '-' foran 
           en parameterinstilling er for at gøre det nemmer at opsamle i vores program */
        if (argv[i][0] == '-') {
            /*grunden til at vi har denne if statment er fordi at både 'g', 'b' og 'r'
               skal ikke efterføgles af en værdi, så for at sikre os at det inputs
               som skal have en værdi tilknyttede til sig har det bruge vi dette */
            if ((argv[i][1] != 'g' && argv[i][1] != 'b' && argv[i][1] != 'r')
                && !isdigit(argv[i + 1][0])) {
                printf
                    ("ERROR: Invaild inputs detected.\nMake sure that every option is follow by a value.\nInvaild argument %c\n",
                     argv[i][1]);
                return EXIT_FAILURE;
            } else {
                /*dette sikre os at den ikke prøver at få fat i en værdi som ikke eksistere */
                if (i + 1 < argc) {
                    value = strtod(argv[i + 1], &emPtr);
                }
            }

            initConfigWithInputParameters(&config, value, argv[i][1],
                                          &graph);
        }
    }

    for (i = 0; i < PLOT_COUNT; i++) {
        data[i].data = calloc(config.maxEvents, sizeof(double));
        data[i].absoluteData = calloc(config.maxEvents, sizeof(double));
        avgData[i].data = calloc(config.maxEvents, sizeof(double));
        avgData[i].absoluteData = calloc(config.maxEvents, sizeof(double));

        isAllocated(data[i].data);
        isAllocated(data[i].absoluteData);
        isAllocated(avgData[i].data);
        isAllocated(avgData[i].absoluteData);
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
        exportData(i, runTime, data, PLOT_COUNT, config.maxEvents,
                   config.amountOfAgents, 1, config);
        exportData(i, runTime, data, PLOT_COUNT, config.maxEvents,
                   config.amountOfAgents, 0, config);
        calculateAveragePlot(i, config.maxEvents, data, avgData,
                             PLOT_COUNT);
    }

    gsl_rng_free(r);

    printf("\nSeed used: %d\n", seedUsed);

    if (graph != 0) {
        printf("\nPlotting graph...\n");
        exportData(-1, runTime, avgData, PLOT_COUNT, config.maxEvents, 100,
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

void initStandardConfig(simConfig * config)
{
    /*party */
    config->partyChance = 16;
    config->partyRisk = 0.15;

    /*Groups */
    config->primaryGroupRisk = 0.05;  /*can */
    config->secondaryGroupRisk = 0.025; /*can */
    config->contactsRisk = 0.03;  /*can */
    config->groupSize[0].lowerbound = 5;
    config->groupSize[0].upperbound = 50;
    config->groupSize[0].varians = 400;
    config->groupSize[0].expectedValue =
        (config->groupSize[0].lowerbound +
         config->groupSize[0].upperbound) / 2;
    config->groupSize[1].lowerbound = 5;
    config->groupSize[1].upperbound = 30;
    config->groupSize[1].varians = 100;
    config->groupSize[1].expectedValue =
        (config->groupSize[1].lowerbound +
         config->groupSize[1].upperbound) / 2;
    config->groupSize[2].lowerbound = 2;
    config->groupSize[2].upperbound = 10;
    config->groupSize[2].varians = 9;
    config->groupSize[2].expectedValue = 5;
    config->groupSize[3].upperbound = 50;
    config->groupSize[3].lowerbound = 2;
    config->groupSize[3].expectedValue = 6;
    config->groupSize[3].varians = 9;
    config->toMeet[0].upperbound = 10;
    config->toMeet[0].lowerbound = 0;
    config->toMeet[0].expectedValue = 5;
    config->toMeet[0].varians = 9;
    config->toMeet[1].upperbound = 20;
    config->toMeet[1].lowerbound = 0;
    config->toMeet[1].expectedValue = 3;
    config->toMeet[1].varians = 9;
    config->toMeet[2].upperbound = 3;
    config->toMeet[2].lowerbound = 0;
    config->toMeet[2].expectedValue = 1;
    config->toMeet[2].varians = 2;
    config->toMeet[3].upperbound = 15;
    config->toMeet[3].lowerbound = 0;
    config->toMeet[3].expectedValue = 3;
    config->toMeet[3].varians = 9;
    config->passerbys.lowerbound = 0;
    config->passerbys.upperbound = 25;
    config->passerbys.varians = 8;
    config->passerbys.expectedValue = 4;

    /*App */
    config->chanceToHaveApp = 0;  /*can */
    config->btThreshold = 0;
    config->btDecay = 7;

    /*Infections */
    config->infectionTime.lowerbound = 2;
    config->infectionTime.upperbound = 12;
    config->infectionTime.varians = 1;
    config->infectionTime.expectedValue = 4;
    config->amountOfStartInfected = 1;
    config->symptomaticPercent = 0.84;  /*can */
    config->incubationTime.lowerbound = 1;  /* CDC.gov */
    config->incubationTime.upperbound = 14; /* CDC.gov */
    config->incubationTime.varians = 1;
    config->incubationTime.expectedValue = 5.1; /* CDC.gov *//*can */
    config->isolationDelay.upperbound = 5;
    config->isolationDelay.lowerbound = 0;
    config->isolationDelay.expectedValue = 1;
    config->isolationDelay.varians = 1;

    /*Misc */
    config->simulationRuns = 1;
    config->maxEvents = 100;
    config->seed = 0;
    config->print = 1;
    config->amountOfAgents = 100000;
    config->willIsolatePercent = 0.9; /*can */
    config->willTestPercent = 0.6;  /*can */
    config->testResponseTime.upperbound = 3;  /*can, this is a worst case */
    config->testResponseTime.lowerbound = 1;
    config->testResponseTime.expectedValue = 2;
    config->testResponseTime.varians = 1;
    config->chanceOfCorrectTest = 0.95;
    config->passerByRisk = 0.0048;
    config->makeConfigFile = 1;
    config->dataLabel = 1;
    config->isolateOnAppInform = 0;
}

void initConfigWithInputParameters(simConfig * config, double value,
                                   char input, int *graph)
{
    switch (input) {
    case 'r':
        config->isolateOnAppInform = 1;
        break;
    case 'z':                  /*how many angents have sympums when infected */
        config->symptomaticPercent = isValueCorrect(input, value, 0, 1);
        break;

    case 'w':                  /*chanc that angent will isolate */
        config->willIsolatePercent = isValueCorrect(input, value, 0, 1);
        break;

    case 'c':                  /*risk of infetion */
        config->contactsRisk = isValueCorrect(input, value, 0, 1);
        break;

    case 'k':                  /*amount of contacts pr agent */
        config->groupSize[2].expectedValue = isValueCorrect
            (input, value, config->groupSize[2].lowerbound,
             config->groupSize[2].upperbound);
        break;

    case 't':                  /*size of primary group */
        config->groupSize[0].expectedValue = isValueCorrect
            (input, value, config->groupSize[0].lowerbound,
             config->groupSize[0].upperbound);
        break;

    case 'y':                  /*size of secound group */
        config->groupSize[1].expectedValue = isValueCorrect
            (input, value, config->groupSize[1].lowerbound,
             config->groupSize[1].upperbound);
        break;

    case 'a':                  /*amount of time incted */
        config->infectionTime.expectedValue = isValueCorrect
            (input, value,
             config->infectionTime.lowerbound,
             config->infectionTime.upperbound);
        break;

    case 'p':                  /*total amount of agents */
        config->amountOfAgents = isValueCorrect(input, value, 1, INT_MAX);
        break;

    case 'i':                  /*amount of infected at start of simulation */
        config->amountOfStartInfected =
            isValueCorrect(input, value, 1, INT_MAX);
        break;

    case 'e':                  /*lenght of simulation */
        config->maxEvents = isValueCorrect(input, value, 0, INT_MAX);
        break;

    case 's':                  /*seed */
        config->seed = isValueCorrect(input, value, 0, INT_MAX);
        break;

    case 'g':
        *graph = 1;
        break;

    case 'b':
        config->groupSize[0].lowerbound = 2;
        config->groupSize[0].upperbound = 10;
        config->groupSize[0].varians = 6;
        config->groupSize[0].expectedValue = 5;
        config->groupSize[1].lowerbound = 2;
        config->groupSize[1].upperbound = 10;
        config->groupSize[1].varians = 6;
        config->groupSize[1].expectedValue = 5;
        config->groupSize[2].lowerbound = 0;
        config->groupSize[2].upperbound = 10;
        config->groupSize[2].varians = 9;
        config->groupSize[2].expectedValue = 5;
        config->groupSize[3].upperbound = 20;
        config->groupSize[3].lowerbound = 2;
        config->groupSize[3].expectedValue = 6;
        config->groupSize[3].varians = 9;
        break;

    case 'd':
        config->chanceToHaveApp = isValueCorrect(input, value, 0, 1);
        break;

    case 'f':
        config->btThreshold = isValueCorrect(input, value, 0, INT_MAX);
        break;

    case 'h':
        config->btDecay = isValueCorrect(input, value, 0, INT_MAX);
        break;

    case 'j':
        config->simulationRuns = isValueCorrect(input, value, 0, INT_MAX);
        break;

    case 'l':
        config->dataLabel = isValueCorrect(input, value, 0, INT_MAX);
        
    case 'm':
        config->toMeet[0].expectedValue =
            isValueCorrect(config->toMeet[0].expectedValue,
                            value, config->toMeet[0].lowerbound,
                            config->toMeet[0].upperbound);
        break;

    case 'n':
        config->toMeet[1].expectedValue =
            isValueCorrect(config->toMeet[1].expectedValue,
                            value, config->toMeet[1].lowerbound,
                            config->toMeet[1].upperbound);
        break;

    case 'o':
        config->toMeet[2].expectedValue =
            isValueCorrect(config->toMeet[2].expectedValue,
                            value, config->toMeet[2].lowerbound,
                            config->toMeet[2].upperbound);
        break;

    case 'u':
        config->toMeet[3].expectedValue =
            isValueCorrect(config->toMeet[3].expectedValue,
                            value, config->toMeet[3].lowerbound,
                            config->toMeet[3].upperbound);
        break;
                
    }

}

double isValueCorrect(char input, double value, int min, int max)
{
    if (value >= min && value <= max)
        return value;
    else
        printf
            ("-%c %f is not accepted, the input value of paramter -%c must be in the interval [%d, %d]\n",
             input, value, input, min, max);
    exit(EXIT_FAILURE);
}
