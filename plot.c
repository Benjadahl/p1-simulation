#include "pbPlots.h"
#include "supportLib.h"

void PlotLineGraph(double xAxis[], int xSize, double yAxis[], int ySize)
{
    RGBABitmapImageReference *imgRef = CreateRGBABitmapImageReference();
	DrawScatterPlot(imgRef, 500, 500, xAxis, xSize, yAxis, ySize);
    
    size_t length;
	double *pngdata = ConvertToPNG(&length, imgRef->image);
	WriteToFile(pngdata, length, "linePlot.png");
}

void PlotBarGraph(double data[], int dataSize)
{
    RGBABitmapImage *img = DrawBarPlot(500, 500, data, dataSize);
    
    size_t length;
	double *pngdata = ConvertToPNG(&length, img);
	WriteToFile(pngdata, length, "barPlot.png");
}

