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

void writeConfigInput(char *filename, simConfig config)
{
    int i;
    FILE *file = fopen(filename, "w+");

    fprintf(file, "simulationRuns: %d\n", config.simulationRuns);
    fprintf(file, "contactsRisk: %lf\n", config.contactsRisk);
    fprintf(file, "amountOfAgents: %d\n", config.amountOfAgents);
    fprintf(file,
            "infectionTime{\nupper: %d \nlower: %d\nvarias: %lf\nexpectedValue: %lf \n}\n",
            config.infectionTime.upperbound,
            config.infectionTime.lowerbound, config.infectionTime.varians,
            config.infectionTime.expectedValue);
    fprintf(file, "amountOfStartInfected: %d\n",
            config.amountOfStartInfected);
    fprintf(file, "maxEvents: %d\n", config.maxEvents);
    fprintf(file, "symptomaticPercent: %lf\n", config.symptomaticPercent);
    fprintf(file,
            "incubationTime{\nupper: %d \nlower: %d\nvarias: %lf\nexpectedValue: %lf \n}\n",
            config.incubationTime.upperbound,
            config.incubationTime.lowerbound,
            config.incubationTime.varians,
            config.incubationTime.expectedValue);
    fprintf(file, "willIsolatePercent: %lf\n", config.willIsolatePercent);
    fprintf(file, "partyChance: %lf\n", config.partyChance);
    fprintf(file, "partyRisk: %lf\n", config.partyRisk);
    fprintf(file, "partyMeetChance: %lf\n", config.partyMeetChance);
    fprintf(file, "willTestPercent: %lf\n", config.willTestPercent);
    fprintf(file, "seed: %d\n", config.seed);
    fprintf(file, "print: %d\n", config.print);
    fprintf(file, "primaryGroupRisk: %lf\n", config.primaryGroupRisk);
    fprintf(file, "secondaryGroupRisk: %lf\n", config.secondaryGroupRisk);
    fprintf(file, "chanceToHaveApp: %lf\n", config.chanceToHaveApp);
    fprintf(file, "isolationTime: %d\n", config.isolationTime);
    fprintf(file,
            "isolationDelay{\nupper: %d \nlower: %d\nvarias: %lf\nexpectedValue: %lf \n}\n",
            config.isolationDelay.upperbound,
            config.isolationDelay.lowerbound,
            config.isolationDelay.varians,
            config.isolationDelay.expectedValue);
    fprintf(file,
            "testResponseTime{\nupper: %d \nlower: %d\nvarias: %lf\nexpectedValue: %lf \n}\n",
            config.testResponseTime.upperbound,
            config.testResponseTime.lowerbound,
            config.testResponseTime.varians,
            config.testResponseTime.expectedValue);
    for (i = 0; i < amountOfGroups; i++) {
        fprintf(file, "groupAmounts[%d]: %d\n", i, config.groupAmounts[i]);
    }
    for (i = 0; i < amountOfGroups; i++) {
        fprintf(file,
                "toMeet[%d]{\nupper: %d \nlower: %d\nvarias: %lf\nexpectedValue: %lf \n}\n",
                i, config.toMeet[i].upperbound,
                config.toMeet[i].lowerbound, config.toMeet[i].varians,
                config.toMeet[i].expectedValue);
    }

    fprintf(file, "btThreshold: %d\n", config.btThreshold);
    fprintf(file, "btDecay: %d\n", config.btDecay);
    fprintf(file, "chanceOfCorrectTest: %lf\n",
            config.chanceOfCorrectTest);
    for (i = 0; i < amountOfGroups; i++) {
        fprintf(file,
                "groupSize[%d]{\nupper: %d \nlower: %d\nvarias: %lf\nexpectedValue: %lf \n}\n",
                i, config.groupSize[i].upperbound,
                config.groupSize[i].lowerbound,
                config.groupSize[i].varians,
                config.groupSize[i].expectedValue);
    }
    fprintf(file,
            "passerbys{\nupper: %d \nlower: %d\nvarias: %lf\nexpectedValue: %lf \n}\n",
            config.passerbys.upperbound, config.passerbys.lowerbound,
            config.passerbys.varians, config.passerbys.expectedValue);
    fprintf(file, "passerByRisk: %lf\n", config.passerByRisk);
    fclose(file);
    return;
}

void WriteFile(char *fileName, DataSet * dataSets, int absolute,
               int dataCount, int events, int label)
{
    int i, j;
    FILE *file = fopen(fileName, "w+");

    fprintf(file, ";");
    for (i = 0; i < dataCount; i++) {
        fprintf(file, "%s;", dataSets[i].name);
    }

    for (i = 0; i < events; i++) {
        fprintf(file, "\n%d;", label);
        for (j = 0; j < dataCount; j++) {
            if (absolute) {
                fprintf(file, "%f;", dataSets[j].absoluteData[i]);
            } else {
                fprintf(file, "%f;", dataSets[j].data[i]);
            }
        }
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
                    if (titleToken != ";") {
                        strcpy(data[j].name, titleToken);
                        titleToken = strtok(NULL, ";");
                        j++;
                    }
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
        if (data_set < dataCount + 1) {
            if (data_set % dataCount != 0) {
                data[data_set - 1].data[dataNum] = atof(token);
            }
        }
        data_set++;
        token = strtok(NULL, ";");
    }
}

void ExportData(int run, time_t runTime, DataSet * dataSets, int dataCount,
                int events, int yMax, int absolute, simConfig config)
{
    char foldername[90], filename[150], graphname[150], configName[150];
    struct tm *currentTime;

    currentTime = localtime(&runTime);
    sprintf(foldername, "output/%d-%02d-%02d-H%02dM%02dS%02d",
            currentTime->tm_year + 1900, currentTime->tm_mon + 1,
            currentTime->tm_mday, currentTime->tm_hour,
            currentTime->tm_min, currentTime->tm_sec);

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

    WriteFile(filename, dataSets, absolute, dataCount, events,
              config.dataLabel);

    if (config.makeConfigFile && run == 0) {
        sprintf(configName, "%s/config.txt", foldername);
        writeConfigInput(configName, config);
    }

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
