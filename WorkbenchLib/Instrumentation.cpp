#include "stdafx.h"

#include "math.h"
#include "Instrumentation.h"
#include "Bitmap.h"

namespace WorkbenchLib
{

	BOOL Draw3DCircle_to_array(UINT *array, int radius, double illum_dir_deg/*=-135*/, COLORREF bk_color/*=LITE_GRAY*/)
	{
		double theta_offset_deg = illum_dir_deg;
		double theta_offset_rad = theta_offset_deg * PI / 180.0;

		int x = radius;
		int y = radius;

		int i = 0;
		for (int iy = y - radius; iy <= y + radius; iy++)
		{
			for (int ix = x - radius; ix <= x + radius; ix++)
			{
				// compute distance from x,y and angle
				double r = sqrt((double)((ix - x)*(ix - x) + (iy - y)*(iy - y)));
				double t = atan2((double)(ix - x), (double)(iy - y));

				t += theta_offset_rad;
				while (t > PI) t -= 2 * PI;
				while (t < -PI) t += 2 * PI;

				COLORREF color;
				if (r <= radius)
				{
					double d0 = r / radius; // delta goes from 1 to 0 as we head to the center

					double d1 = 1 - d0*d0*d0*d0;
					d1 *= .25;
					d1 += .75;

					if (t >= -PI && t < -PI / 2)
					{
						t = -(t + PI);
					}
					else
						if (t >= PI / 2)
						{
							t = PI - t;
						}

					double illum_adj = (d0*d0*d0)*t / (PI)+1;
					double delta = d1 * illum_adj;

					color = AdjustBrightness(bk_color, delta);

					if (radius - r < 1) // less than one pixel
						color = AverageColors(bk_color, color, radius - r);

					array[i] = color;
				}

				i++;
			}
		}

		return TRUE;
	}

	BOOL Draw3DCircle(CDC *dc, int x, int y, int radius, double illum_dir_deg/*=-135*/, COLORREF bk_color/*=LITE_GRAY*/)
	{
		CBitmap bitmap;
		int width = 2 * radius + 1;
		int height = 2 * radius + 1;
		unsigned int nPlanes = 1;
		unsigned int nColors = 32;

		UINT *array = new UINT[width*height];
		int n = width * height;
		for (int i = 0; i<n; i++)
			array[i] = bk_color;

		BOOL iret = Draw3DCircle_to_array(array, radius, illum_dir_deg, bk_color);
		if (iret != TRUE)
		{
			delete[] array;
			return FALSE;
		}

		iret = bitmap.CreateBitmap(width, height, nPlanes, nColors, array);

		//////////////////////////////////////////////////////////////////////
		// This code adapted from MFC
		CDC memDC;
		iret = memDC.CreateCompatibleDC(dc);
		CBitmap* pOld = memDC.SelectObject(&bitmap);
		if (pOld == NULL)
			return FALSE;     // destructors will clean up

		iret = dc->BitBlt(x - radius, y - radius, width, height, &memDC, 0, 0, SRCCOPY);

		memDC.SelectObject(pOld);
		////////////////////////////////////////////////////////////////////////

		delete[] array;

		return TRUE;
	}

};