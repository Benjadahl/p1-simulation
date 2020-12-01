typedef struct DataSet
{
    double *data;
    char *name;
} DataSet;

void WriteFile(char *file_name, DataSet data_set1, DataSet data_set2, DataSet data_set3, DataSet data_set4, int data_size);
void ExportData(char *filename, double *data1, double *data2, double *data3, double *data4, int events);