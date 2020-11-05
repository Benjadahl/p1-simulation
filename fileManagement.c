#include<stdio.h>
#include<string.h>
#include<stdlib.h>

void WriteFile(char *file_name, double *data, int data_size);
FILE CreatFile(char *file_name);
void ReadFile(char *file_name, float *data);


void WriteFile(char *file_name, double *data, int data_size)
{
    int i;
    double data_num;
    char data_print[7];
    FILE *file = fopen(file_name, "w");
    if(file == NULL)
        *file = CreatFile(file_name);
    
    for (i = 0; i < data_size; i++){
        data_num = data[i];
        snprintf(data_print,7,"%f",data_num);
        fprintf(file, "\n%s", data_print);
    }

    return;    
}

FILE CreatFile(char *file_name)
{
    FILE *new_file;
    new_file = fopen(file_name, "w+");
    return *new_file;
}

void ReadFile(char *file_name, float *data)
{
    FILE *file = fopen(file_name, "r");
    char line[100];
    int i = 0;
    if(file == NULL)
    {
        printf("File %s not found.", file_name);
        return;
    }
    
    while (fgets(line, sizeof(line), file))
    {
        char *token;
        token = strtok(line, ";");
        while (token != NULL)
        {
            char *token_token = strtok(token, "\n");
            data[i] = atof(token_token);
            i++;
            token = strtok(NULL,";");
        }
    }
}

 
