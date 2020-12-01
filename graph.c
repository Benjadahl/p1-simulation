#include"import.h"
#include"plot.h"
#include"simulation.h"

void CreatePlotFromCVS(char *file_name, char *output_name,
                       simConfig config);
void CreatePlot(char *file_name, double succeptible_data[],
                double infectious_data[], double recovered_data[],
                double isolated_data[], int time_length);

void CreatePlotFromCVS(char *file_name, char *output_name,
                       simConfig config)
{
    int i;
    float data1[config.maxEvents], data2[config.maxEvents],
        data3[config.maxEvents], data4[config.maxEvents];
    double new_data1[config.maxEvents], new_data2[config.maxEvents],
        new_data3[config.maxEvents], new_data4[config.maxEvents];
    ReadFile(file_name, data1, data2, data3, data4);

    for (i = 0; i < config.maxEvents; i++) {
        new_data1[i] = (double) data1[i];
        new_data2[i] = (double) data2[i];
        new_data3[i] = (double) data3[i];
        new_data4[i] = (double) data4[i];
    }

    CreatePlot(output_name, new_data1, new_data2, new_data3, new_data4,
               config.maxEvents);
}

void CreatePlot(char *file_name, double succeptible_data[], double exposed_data[],
                double infectious_data[], double recovered_data[], int time_length)
{
    double timeSeries[time_length];
    char recoveredName[50], exposedName[50], infectiousName[50], succeptibleName[50];
    for (int i = 0; i < time_length; i++)
        timeSeries[i] = (double) i + 1;

    RGBABitmapImageReference canvasReference;
    RGBABitmapImage *succeptible_img, *exposed_img, *infectious_img, *recovered_img;

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