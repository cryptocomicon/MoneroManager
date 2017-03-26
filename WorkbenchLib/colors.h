#ifndef INC_COLORS
#define INC_COLORS

namespace WorkbenchLib
{

	extern COLORREF g_color_nearest_to_WHITE;

#define LITE_GRAY 	RGB(192,192,192)
#define DARK_GRAY   RGB(130,130,130)
#define BLACK		RGB(0,0,0)
#define WHITE		RGB(255,255,255)
#define RED			RGB(255,0,0)
#define LITE_RED	RGB(255,127,127)
#define CYAN		RGB(0,255,255)
#define DARK_CYAN   RGB(0,130,130)
#define BLUE		RGB(0,0,255)
#define GREEN		RGB(0,255,0)
#define DARK_GREEN  RGB(0,130,0) 
#define SANDY		RGB(128,128,0)
#define YELLOW		RGB(255,255,0)
#define PURPLE      RGB(128,0,128)
#define MAGENTA     RGB(255,0,255)

	inline BOOL IsWhite(COLORREF color)
	{
		if (color == WHITE)
			return TRUE;

		if (color == g_color_nearest_to_WHITE)
			return TRUE;

		return FALSE;
	}

}; // namespace WorkbenchLib

#endif