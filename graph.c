#include"import.h"
#include"plot.h"

void CreatePlotFromCVS(char *file_name);
void CreatePlot(char *file_name, double succeptible_data[],
                double infectious_data[], double recovered_data[]);

void CreatePlotFromCVS(char *file_name)
{
    int i;
    float data1[100], data2[100], data3[100];
    double new_data1[100], new_data2[100], new_data3[100];
    ReadFile(file_name, data1, data2, data3);

    for (i = 0; i < 100; i++) {
        new_data1[i] = (double) data1[i];
        new_data2[i] = (double) data2[i];
        new_data3[i] = (double) data3[i];
    }

    CreatePlot("Graph.png", new_data1, new_data2, new_data3);
}

void CreatePlot(char *file_name, double succeptible_data[],
                double infectious_data[], double recovered_data[])
{
    double timeSeries[100];
    for (int i = 0; i < 100; i++)
        timeSeries[i] = (double) i + 1;

    RGBABitmapImageReference canvasReference;
    RGBABitmapImage *combined_plots = CreateImage(2000, 2000, GetWhite());
    RGBABitmapImage *succeptible_img, *infectious_img, *recovered_img;

    canvasReference =
        PlotLineGraph(timeSeries, 100, succeptible_data, 100,
                      L"Succeptible (%)",
                      L"Number of succeptible people (%)", L"Time (event)",
                      100, 100);
    succeptible_img = canvasReference.image;

    canvasReference =
        PlotLineGraph(timeSeries, 100, infectious_data, 100,
                      L"Infectious (%)",
                      L"Number of infectious people (%)", L"Time (event)",
                      100, 100);
    infectious_img = canvasReference.image;

    canvasReference =
        PlotLineGraph(timeSeries, 100, recovered_data, 100,
                      L"Recovered (%)", L"Number of recovered people (%)",
                      L"Time (event)", 100, 100);
    recovered_img = canvasReference.image;

    DrawImageOnImage(combined_plots, succeptible_img, 0, 0);
    DrawImageOnImage(combined_plots, infectious_img, 0, 1000);
    DrawImageOnImage(combined_plots, recovered_img, 1000, 0);

    size_t length;
    double *pngdata = ConvertToPNG(&length, combined_plots);
    WriteToFile(pngdata, length, file_name);

}
