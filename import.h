typedef struct DataSetRead
{
    float *data;
    char name[14];
} DataSetRead;

void ReadFile(char *file_name, DataSetRead *data, int dataCount);