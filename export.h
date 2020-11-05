typedef struct DataSet
{
    double *data;
    char *name;
} DataSet;

void WriteFile(char *file_name, DataSet data_set1, DataSet data_set2, DataSet data_set3, int data_size);