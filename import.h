typedef struct DataSetRead
{
    float *data;
    char name[30];
} DataSetRead;

void readFile(char *file_name, DataSetRead *data, int dataCount);