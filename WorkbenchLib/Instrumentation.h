// Instrumentation.h
#include "colors.h"

#ifndef INC_INSTRUMENTATION
#define INC_INSTRUMENTATION

namespace WorkbenchLib
{
	BOOL Draw3DCircle_to_array(UINT *array, int radius, double illum_dir_deg = -135, COLORREF bk_color = LITE_GRAY);
	BOOL Draw3DCircle(CDC *dc, int x, int y, int radius, double illum_dir_deg = -135, COLORREF bk_color = LITE_GRAY);

};

#endif
