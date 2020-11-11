#include "lib/pbPlots.h"
#include "lib/supportLib.h"

RGBABitmapImageReference PlotLineGraph(double xAxis[], int xSize,
                                       double yAxis[], int ySize,
                                       wchar_t title[], wchar_t xLable[],
                                       wchar_t yLable[], int xMax,
                                       int yMax)
{
    ScatterPlotSeries *series = GetDefaultScatterPlotSeriesSettings();
    series->xs = xAxis;
    series->xsLength = xSize;
    series->ys = yAxis;
    series->ysLength = ySize;
    series->linearInterpolation = true;
    series->lineType = L"solid";
    series->lineTypeLength = wcslen(series->lineType);
    series->lineThickness = 2;
    series->color = GetGray(0.3);

    ScatterPlotSettings *settings = GetDefaultScatterPlotSettings();
    settings->width = 1000;
    settings->height = 1000;
    settings->autoBoundaries = false;
    settings->xMin = 0;
    settings->yMin = 0;
    settings->xMax = xMax;
    settings->yMax = yMax;
    settings->autoPadding = true;
    settings->title = title;
    settings->titleLength = wcslen(settings->title);
    settings->xLabel = xLable;
    settings->xLabelLength = wcslen(settings->xLabel);
    settings->yLabel = yLable;
    settings->yLabelLength = wcslen(settings->yLabel);
    ScatterPlotSeries *s[] = { series };
    settings->scatterPlotSeries = s;
    settings->scatterPlotSeriesLength = 1;

    RGBABitmapImageReference *canvasReference =
        CreateRGBABitmapImageReference();
    DrawScatterPlotFromSettings(canvasReference, settings);

    return *canvasReference;
}
