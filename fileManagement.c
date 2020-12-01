#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include <ctype.h>
#include <time.h>
#include <sys/stat.h>
#include"import.h"
#include"export.h"
#include"simulation.h"

void SplitLine(int dataCount, DataSetRead * data, int dataNum, char *t);

void CreatePlotFromCVS(char *file_name, int dataCount, char *output_name,
                       simConfig config);
DataSet createDataSet(char *name, double *data);

void WriteFile(char *fileName, DataSet * dataSets, int dataCount,
               int events)
{
    int i, j;
    char data_print[50];
    FILE *file = fopen(fileName, "w+");

    for (i = 0; i < dataCount; i++) {
        fprintf(file, "%s;", dataSets[i].name);
    }
    fprintf(file, "\n");

    for (i = 0; i < events; i++) {
        for (j = 0; j < dataCount; j++) {
            fprintf(file, "%f;", dataSets[j].data[i]);
        }
        fprintf(file, "\n");
    }

    fclose(file);
    return;
}

void ReadFile(char *file_name, DataSetRead * data, int dataCount)
{
    FILE *file = fopen(file_name, "r");
    char line[200];
    int i = 0, j;
    if (file == NULL) {
        printf("File %s not found.", file_name);
        return;
    }

    while (fgets(line, sizeof(line), file)) {
        char *token;
        token = strtok(line, ",");
        while (token != NULL) {
            char *token_token = strtok(token, "\n");
            if (isdigit(token[0])) {
                SplitLine(dataCount, data, i, token_token);
                i++;
            }
            token = strtok(NULL, ",");
        }
    }
    fclose(file);
}

void SplitLine(int dataCount, DataSetRead * data, int dataNum, char *t)
{
    int data_set = 0;
    char *token = strtok(t, ";");
    while (token != NULL) {
        if (data_set < dataCount) {
            data[data_set].data[dataNum] = atof(token);
        }
        data_set++;
        token = strtok(NULL, ";");
    }
}

void ExportData(int run, time_t runTime, double *data1, double *data2,
                double *data3, double *data4, simConfig config)
{
    char foldername[90], filename[100], graphname[100];
    struct tm *currentTime;

    currentTime = localtime(&runTime);
    sprintf(foldername, "output/H%02dM%02dS%02d-%02d-%02d-%d",
            currentTime->tm_hour, currentTime->tm_min, currentTime->tm_sec,
            currentTime->tm_mday, currentTime->tm_mon + 1,
            currentTime->tm_year - 100);

    if (run == 0) {
        mkdir(foldername, 0777);
    }

    if (run == -1) {
        sprintf(filename, "%s/avg.csv", foldername);
    } else {
        sprintf(filename, "%s/%d.csv", foldername, run);
    }

    DataSet dataSets[4];
    dataSets[0] = createDataSet("Succeptible", data1);
    dataSets[1] = createDataSet("Exposed", data2);
    dataSets[2] = createDataSet("Infectious", data3);
    dataSets[3] = createDataSet("Recovered", data4);

    WriteFile(filename, dataSets, 4, config.maxEvents);

    if (run == -1) {
        sprintf(graphname, "%s/avg-graph", foldername);
        CreatePlotFromCVS(filename, 4, graphname, config);
    }
}

DataSet createDataSet(char *name, double *data)
{
    DataSet newSet;
    newSet.data = data;
    newSet.name = name;
    return newSet;
}
