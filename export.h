typedef long time_t;

typedef struct DataSet
{
    double *data;
    char *name;

} DataSet;

void WriteFile(char *fileName, DataSet *dataSets, int dataCount, int events);
void ExportData(int run, time_t runTime, DataSet *dataSets, int dataCount, int events);
DataSet createDataSet(char *name, double *data);