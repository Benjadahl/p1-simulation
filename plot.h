#ifndef PLOT
#define PLOT
#include "lib/pbPlots.h"
#include "lib/supportLib.h"

RGBABitmapImageReference PlotLineGraph(double xAxis[], int xSize, double yAxis[], int ySize, wchar_t title[], wchar_t xLable[], wchar_t yLable[], int xMax, int yMax);
#endif