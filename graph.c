#include<stdlib.h>
#include"import.h"
#include"export.h"
#include"plot.h"
#include "distribution.h"
#include"simulation.h"

typedef struct GraphData {
    double *data;
    wchar_t title[14];
} GraphData;

void CreatePlot(char *file_name, int dataCount, GraphData * dataSets,
                int time_length, int yMax);

void CreatePlotFromCSV(char *file_name, int dataCount, char *output_name,
                       int events, int yMax)
{
    int i, j;
    DataSetRead data[dataCount];
    GraphData newData[dataCount];

    for (i = 0; i < dataCount; i++) {
        data[i].data = malloc(sizeof(float) * events);
        newData[i].data = malloc(sizeof(double) * events);
    }

    ReadFile(file_name, data, dataCount);

    for (i = 0; i < dataCount; i++) {
        mbstowcs(newData[i].title, data[i].name, 14);
    }

    for (i = 0; i < events; i++) {
        for (j = 0; j < dataCount; j++) {
            newData[j].data[i] = (double) data[j].data[i];
        }
    }

    CreatePlot(output_name, dataCount, newData, events, yMax);

    for (i = 0; i < dataCount; i++) {
        free(data[i].data);
        free(newData[i].data);
    }
}

void CreatePlot(char *file_name, int dataCount, GraphData * dataSets,
                int time_length, int yMax)
{
    int i;
    double timeSeries[time_length];
    char graphName[50], graphNameFinal[50];
    for (int i = 0; i < time_length; i++)
        timeSeries[i] = (double) i + 1;

    RGBABitmapImageReference canvasReference;

    for (i = 0; i < dataCount; i++) {
        canvasReference =
            PlotLineGraph(timeSeries, time_length, dataSets[i].data,
                          time_length, dataSets[i].title,
                          L"Number of people (%)", L"Time (event)",
                          time_length, yMax);
        wcstombs(graphName, dataSets[i].title, 50);
        sprintf(graphNameFinal, "%s-%s.png", file_name, graphName);
        size_t length;
        double *pngdata = ConvertToPNG(&length, canvasReference.image);
        WriteToFile(pngdata, length, graphNameFinal);
    }
}
