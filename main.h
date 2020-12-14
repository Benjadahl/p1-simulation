#ifndef MAIN
#define MAIN

#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <time.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <limits.h>
#include "simulation.h"
#include "export.h"
#include "allocationTest.h"

void initStandardConfig(simConfig * config);
void initConfigWithInputParameters(simConfig * config, double value,
                                   char input, int *graph);
double isValueCorrect(char input, double value, int min, int max);
void run_simulation(gsl_rng * r, simConfig config, DataSet * data,
                    int dataCount);
void exportData(int run, time_t runTime, DataSet * dataSets, int dataCount,
                int events, int yMax, int abosolute, simConfig simConfig);
void calculateAveragePlot(int run, int events, DataSet * data,
                          DataSet * avgData, int dataCount);
#endif