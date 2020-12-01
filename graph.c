#include"import.h"
#include"export.h"
#include"plot.h"
#include"simulation.h"

void CreatePlotFromCVS(char *file_name, int dataCount, char *output_name,
                       simConfig config);
void CreatePlot(char *file_name, double succeptible_data[],
                double infectious_data[], double recovered_data[],
                double isolated_data[], int time_length);

void CreatePlotFromCVS(char *file_name, int dataCount, char *output_name,
                       simConfig config)
{
    int i, j;
    DataSetRead data[dataCount];
    DataSet newData[dataCount];

    for (i = 0; i < dataCount; i++) {
        data[i].data = malloc(sizeof(float) * config.maxEvents);
        newData[i].data = malloc(sizeof(double) * config.maxEvents);
    }

    ReadFile(file_name, data, dataCount);

    for (i = 0; i < config.maxEvents; i++) {
        for (j = 0; j < dataCount; j++) {
            newData[j].data[i] = (double) data[j].data[i];
        }
    }

    CreatePlot(output_name, newData[0].data, newData[1].data,
               newData[2].data, newData[3].data, config.maxEvents);
}

void CreatePlot(char *file_name, double succeptible_data[],
                double exposed_data[], double infectious_data[],
                double recovered_data[], int time_length)
{
    double timeSeries[time_length];
    char recoveredName[50], exposedName[50], infectiousName[50],
        succeptibleName[50];
    for (int i = 0; i < time_length; i++)
        timeSeries[i] = (double) i + 1;

    RGBABitmapImageReference canvasReference;
    RGBABitmapImage *succeptible_img, *exposed_img, *infectious_img,
        *recovered_img;

    canvasReference =
        PlotLineGraph(timeSeries, time_length, succeptible_data,
                      time_length, L"Succeptible (%)",
                      L"Number of succeptible people (%)", L"Time (event)",
                      time_length, 100);
    succeptible_img = canvasReference.image;

    canvasReference =
        PlotLineGraph(timeSeries, time_length, exposed_data, time_length,
                      L"Exposed (%)", L"Number of exposed people (%)",
                      L"Time (event)", time_length, 100);
    exposed_img = canvasReference.image;

    canvasReference =
        PlotLineGraph(timeSeries, time_length, infectious_data,
                      time_length, L"Infectious (%)",
                      L"Number of infectious people (%)", L"Time (event)",
                      time_length, 100);
    infectious_img = canvasReference.image;

    canvasReference =
        PlotLineGraph(timeSeries, time_length, recovered_data, time_length,
                      L"Recovered (%)", L"Number of recovered people (%)",
                      L"Time (event)", time_length, 100);
    recovered_img = canvasReference.image;


    sprintf(succeptibleName, "%s-succeptible.png", file_name);
    sprintf(exposedName, "%s-exposed.png", file_name);
    sprintf(recoveredName, "%s-recovered.png", file_name);
    sprintf(infectiousName, "%s-infectious.png", file_name);

    size_t length;
    double *pngdata = ConvertToPNG(&length, infectious_img);
    WriteToFile(pngdata, length, infectiousName);
    pngdata = ConvertToPNG(&length, exposed_img);
    WriteToFile(pngdata, length, exposedName);
    pngdata = ConvertToPNG(&length, succeptible_img);
    WriteToFile(pngdata, length, succeptibleName);
    pngdata = ConvertToPNG(&length, recovered_img);
    WriteToFile(pngdata, length, recoveredName);
}
