typedef struct DataSetRead
{
    float *data;
    char name[30];
} DataSetRead;

void ReadFile(char *file_name, DataSetRead *data, int dataCount);