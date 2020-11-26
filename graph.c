#include"import.h"
#include"plot.h"
#include"simulation.h"

void CreatePlotFromCVS(char *file_name, char *output_name,
                       simConfig config);
void CreatePlot(char *file_name, double succeptible_data[],
                double infectious_data[], double recovered_data[],
                int time_length);

void CreatePlotFromCVS(char *file_name, char *output_name,
                       simConfig config)
{
    int i;
    float data1[config.maxEvents], data2[config.maxEvents],
        data3[config.maxEvents];
    double new_data1[config.maxEvents], new_data2[config.maxEvents],
        new_data3[config.maxEvents];
    ReadFile(file_name, data1, data2, data3);

    for (i = 0; i < config.maxEvents; i++) {
        new_data1[i] = (double) data1[i];
        new_data2[i] = (double) data2[i];
        new_data3[i] = (double) data3[i];
    }

    CreatePlot(output_name, new_data1, new_data2, new_data3,
               config.maxEvents);
}

void CreatePlot(char *file_name, double succeptible_data[],
                double infectious_data[], double recovered_data[],
                int time_length)
{
    double timeSeries[time_length];
    for (int i = 0; i < time_length; i++)
        timeSeries[i] = (double) i + 1;

    RGBABitmapImageReference canvasReference;
    RGBABitmapImage *combined_plots = CreateImage(2000, 2000, GetWhite());
    RGBABitmapImage *succeptible_img, *infectious_img, *recovered_img;

    canvasReference =
        PlotLineGraph(timeSeries, time_length, succeptible_data,
                      time_length, L"Succeptible (%)",
                      L"Number of succeptible people (%)", L"Time (event)",
                      time_length, 100);
    succeptible_img = canvasReference.image;

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

    DrawImageOnImage(combined_plots, succeptible_img, 0, 0);
    DrawImageOnImage(combined_plots, infectious_img, 0, 1000);
    DrawImageOnImage(combined_plots, recovered_img, 1000, 0);

    size_t length;
    double *pngdata = ConvertToPNG(&length, combined_plots);
    WriteToFile(pngdata, length, file_name);

}
