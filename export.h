typedef long time_t;
typedef struct simConfig simConfig;

typedef struct DataSet
{
    double *data;
    char *name;
} DataSet;

void WriteFile(char *fileName, DataSet *dataSets, int dataCount, int events);
void ExportData(int run, time_t runTime, double *data1, double *data2,
                double *data3, double *data4, simConfig config);