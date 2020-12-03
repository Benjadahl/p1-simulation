#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include <ctype.h>
#include <time.h>
#include <sys/stat.h>
#include <dirent.h>
#include"import.h"
#include"simulation.h"
#include"export.h"

void SplitLine(int dataCount, DataSetRead * data, int dataNum, char *t);

void CreatePlotFromCSV(char *file_name, int dataCount, char *output_name,
                       int events, int yMax);
DataSet createDataSet(char *name, double *data);

void WriteFile(char *fileName, DataSet * dataSets, int absolute,
               int dataCount, int events)
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
            if (absolute) {
                fprintf(file, "%f;", dataSets[j].absoluteData[i]);
            } else {
                fprintf(file, "%f;", dataSets[j].data[i]);
            }
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
    int i = 0, j = 0;
    if (file == NULL) {
        printf("File %s not found.", file_name);
        return;
    }

    while (fgets(line, sizeof(line), file)) {
        char *lineToken;
        lineToken = strtok(line, ",");
        while (lineToken != NULL) {
            char *dataToken = strtok(lineToken, "\n");
            if (isdigit(lineToken[0])) {
                SplitLine(dataCount, data, i, dataToken);
                i++;
            } else {
                char *titleToken = strtok(lineToken, ";");
                while (titleToken != NULL) {
                    strcpy(data[j].name, titleToken);
                    titleToken = strtok(NULL, ";");
                    j++;
                }
            }
            lineToken = strtok(NULL, ",");
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

void ExportData(int run, time_t runTime, DataSet * dataSets, int dataCount,
                int events, int yMax, int absolute)
{
    char foldername[90], filename[150], graphname[150];
    struct tm *currentTime;

    currentTime = localtime(&runTime);
    sprintf(foldername, "output/H%02dM%02dS%02d-%02d-%02d-%d",
            currentTime->tm_hour, currentTime->tm_min, currentTime->tm_sec,
            currentTime->tm_mday, currentTime->tm_mon + 1,
            currentTime->tm_year - 100);

    if (run == 0) {
        if (opendir("output") == NULL) {
            mkdir("output", 0777);
        }
        mkdir(foldername, 0777);
    }

    if (run == -1) {
        if (absolute) {
            sprintf(filename, "%s/absolute-avg.csv", foldername);
        } else {
            sprintf(filename, "%s/avg.csv", foldername);
        }
    } else {
        if (absolute) {
            sprintf(filename, "%s/absolute-%d.csv", foldername, run);
        } else {
            sprintf(filename, "%s/%d.csv", foldername, run);
        }
    }

    WriteFile(filename, dataSets, absolute, dataCount, events);

    if (run == -1) {
        if (absolute) {
            sprintf(graphname, "%s/absolute-avg-graph", foldername);
        } else {
            sprintf(graphname, "%s/avg-graph", foldername);
        }
        CreatePlotFromCSV(filename, dataCount, graphname, events, yMax);
    }
}

DataSet createDataSet(char *name, double *data)
{
    DataSet newSet;
    newSet.data = data;
    newSet.name = name;
    return newSet;
}
