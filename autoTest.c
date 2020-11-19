#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include "simulation.h"

void test10k(simConfig config, double output[]);
void test100k(simConfig config, double output[]);
void test1M(simConfig config, double output[]);

int main()
{
    int seed, i;
    double results[3] = {0, 0, 0};
    simConfig config;

    config.contactsRisk = 1;
    config.amountOfAgents = 0;
    config.infectionTime = 4;
    config.amountOfStartInfected = 20;
    config.maxEvents = 100;
    config.symptomaticPercent = 25;
    config.maxIncubationTime = 14;
    config.willIsolatePercent = 98;
    config.seed = 1;
    config.groupSize[0] = 15;
    config.groupSize[1] = 10;
    config.primaryGroupRisk = 5;
    config.secondaryGroupRisk = 5;
    config.amountOfContactsPerAgent = 5;
    config.groupPercentageToInfect = 74;

    double succeptible_data[config.maxEvents];
    double infectious_data[config.maxEvents];
    double recovered_data[config.maxEvents];

    test10k(config, results);
    test100k(config, results);
    test1M(config, results);

    /* Swap the new value with the previous if your change was expecting a different value */
    if(results[0] != 80.3){
        /* This prints the value that needs to be used as the check */
        printf("\nThis is the value of tick %d: %lf\n", config.maxEvents, results[0]);
        printf(">> Program output an unexpected value in test 10k <<\n");
    }
    else if(results[0] == 80.3){
        printf("The result haven't changed in test 10k\n");
    }
    else{
        printf("FATAL ERROR IN TEST 10K\n");
    }
    
    if(results[1] != 79.6){
        /* This prints the value that needs to be used as the check */
        printf("\nThis is the value of tick %d: %lf\n", config.maxEvents, results[1]);
        printf(">> Program output an unexpected value in test 100k <<\n");
    }
    else if(results[1] == 79.6){
        printf("The result haven't changed in test 100k\n");
    }
    else{
        printf("FATAL ERROR IN TEST 100K\n");
    }

    if(results[2] != 79.7){
        /* This prints the value that needs to be used as the check */
        printf("\nThis is the value of tick %d: %lf\n", config.maxEvents, results[2]);
        printf(">> Program output an unexpected value in test 1M <<\n");
    }
    else if(results[2] == 79.7){
        printf("The result haven't changed in test 1M\n");
    }
    else{
        printf("FATAL ERROR IN TEST 1M\n");
    }

    return 0;
}

void test10k(simConfig config, double output[])
{
    config.amountOfAgents = 10000;
    double succeptible_data_test10k[config.maxEvents];
    double infectious_data_test10k[config.maxEvents];
    double recovered_data_test10k[config.maxEvents];

    run_simulation(config, succeptible_data_test10k, infectious_data_test10k,
                   recovered_data_test10k);

    output[0] = floor(recovered_data_test10k[config.maxEvents - 1] * 10) / 10;
}

void test100k(simConfig config, double output[])
{
    config.amountOfAgents = 100000;
    double succeptible_data_test100k[config.maxEvents];
    double infectious_data_test100k[config.maxEvents];
    double recovered_data_test100k[config.maxEvents];

    run_simulation(config, succeptible_data_test100k, infectious_data_test100k,
                   recovered_data_test100k);

    output[1] = floor(recovered_data_test100k[config.maxEvents - 1] * 10) / 10;
}

void test1M(simConfig config, double output[])
{
    config.amountOfAgents = 1000000;
    double succeptible_data_test1M[config.maxEvents];
    double infectious_data_test1M[config.maxEvents];
    double recovered_data_test1M[config.maxEvents];

    run_simulation(config, succeptible_data_test1M, infectious_data_test1M,
                   recovered_data_test1M);

    output[2] = floor(recovered_data_test1M[config.maxEvents - 1] * 10) / 10;
}