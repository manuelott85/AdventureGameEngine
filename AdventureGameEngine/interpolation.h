//======================================================================
// NOT DONE BY MYSELF (Manuel)!
// Source: http://www.cplusplus.com/forum/general/216928/
//======================================================================

#pragma once

#include <iostream>
#include <iomanip>
#include <vector>
using namespace std;

//======================================================================
//	Returns interpolated value at x from parallel arrays ( xData, yData )
//	Assumes that xData has at least two elements, is sorted and is strictly monotonic increasing
//	boolean argument extrapolate determines behaviour beyond ends of array (if needed)

double interpolate(vector<double> &xData, vector<double> &yData, double x, bool extrapolate)
{
	int size = (int)xData.size();

	int i = 0;	// find left end of interval for interpolation
	if (x >= xData[size - 2])	// special case: beyond right end
	{
		i = size - 2;
	} else {
		while (x > xData[i + 1]) i++;
	}

	double xL = xData[i], yL = yData[i], xR = xData[i + 1], yR = yData[i + 1];	// points on either side (unless beyond ends)
	if (!extrapolate)	// if beyond ends of array and not extrapolating
	{
		if (x < xL) yR = yL;
		if (x > xR) yL = yR;
	}

	double dydx = (yR - yL) / (xR - xL);	// gradient
	return yL + dydx * (x - xL);	// linear interpolation
}