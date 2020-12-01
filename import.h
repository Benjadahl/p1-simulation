typedef struct DataSetRead
{
    float *data;
    char *name;
} DataSetRead;

void ReadFile(char *file_name, DataSetRead *data, int dataCount);