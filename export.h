typedef long time_t;

typedef struct DataSet
{
    double *data;
    double *absoluteData;
    char *name;

} DataSet;

void WriteFile(char *fileName, DataSet * dataSets, int absolute, int dataCount,
               int events, int able);
DataSet createDataSet(char *name, double *data);