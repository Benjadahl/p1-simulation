typedef long time_t;

typedef struct DataSet
{
    double *data;
    double *absoluteData;
    char *name;

} DataSet;

void WriteFile(char *fileName, DataSet * dataSets, int absolute, int dataCount,
               int events);
void ExportData(int run, time_t runTime, DataSet *dataSets, int dataCount, int events, int yMax, int abosolute);
DataSet createDataSet(char *name, double *data);