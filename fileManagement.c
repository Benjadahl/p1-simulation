#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include <ctype.h>
#include"import.h"
#include"export.h"

FILE CreateFile(char *file_name);
void SplitLine(float *data1, float *data2, float *data3, float *data4,
               char *t);

void WriteFile(char *file_name, DataSet data_set1, DataSet data_set2,
               DataSet data_set3, DataSet data_set4, int data_size)
{
    int i;
    char data_print[50];
    FILE *file = fopen(file_name, "w");
    if (file == NULL)
        *file = CreateFile(file_name);

    snprintf(data_print, 50, "%s;%s;%s;%s;", data_set1.name,
             data_set2.name, data_set3.name, data_set4.name);
    fprintf(file, "\n%s", data_print);
    for (i = 0; i < data_size; i++) {
        snprintf(data_print, 50, "%f;%f;%f;%f", data_set1.data[i],
                 data_set2.data[i], data_set3.data[i], data_set4.data[i]);
        fprintf(file, "\n%s", data_print);
    }
    fclose(file);
    return;
}

FILE CreateFile(char *file_name)
{
    FILE *new_file;
    new_file = fopen(file_name, "w+");
    return *new_file;
}

void ReadFile(char *file_name, float *data1, float *data2, float *data3,
              float *data4)
{
    FILE *file = fopen(file_name, "r");
    char line[200];
    int i = 0;
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
                SplitLine(&data1[i], &data2[i], &data3[i], &data4[i],
                          token_token);
                i++;
            }
            token = strtok(NULL, ",");
        }
    }
    fclose(file);
}

void SplitLine(float *data1, float *data2, float *data3, float *data4,
               char *t)
{
    int data_set = 0;
    char *token = strtok(t, ";");
    while (token != NULL) {
        switch (data_set) {
        case 0:
            *data1 = atof(token);
            break;
        case 1:
            *data2 = atof(token);
            break;
        case 2:
            *data3 = atof(token);
            break;
        case 3:
            *data4 = atof(token);
            break;
        default:
            break;
        }
        data_set++;
        token = strtok(NULL, ";");
    }
}

void ExportData(char *filename, double *data1, double *data2,
                double *data3, double *data4, int events)
{
    DataSet data_set1, data_set2, data_set3, data_set4;
    data_set1.data = data1;
    data_set1.name = "Succeptible";
    data_set2.data = data2;
    data_set2.name = "Infectious";
    data_set3.data = data3;
    data_set3.name = "Recovered";
    data_set4.data = data4;
    data_set4.name = "Isolated";
    WriteFile(filename, data_set1, data_set2, data_set3, data_set4,
              events);
}
