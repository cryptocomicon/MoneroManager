#include "io.h"
#include "stdio.h"

#ifndef INC_BITMAP
#define INC_BITMAP


namespace WorkbenchLib
{

	inline void compress_bitmap_32_to_24_bits_per_pixel(int width, int height, UINT *pixel_array)
	{
		BYTE *b = (BYTE *)pixel_array;

		BYTE *b1 = b;

		int n = width*height;
		for (int i = 0; i<width; i++)
		{
			for (int j = 0; j<height; j++)
			{
				*b = *b1;
				b++;
				b1++;

				*b = *b1;
				b++;
				b1++;

				*b = *b1;
				b++;
				b1 += 2;
			}

			b += 1;
		}
	}

	inline void expand_bitmap_24_to_32_bits_per_pixel(const int width, const int height, const void *bmp_24bit, void *bmp_32bit)
	{
		const BYTE *b24 = (const BYTE *)bmp_24bit;
		BYTE *b32 = (BYTE *)bmp_32bit;

		for (int j = 0; j<height; j++)
		{
			for (int i = 0; i<width; i++)
			{
				*b32 = *b24;
				b32++;
				b24++;

				*b32 = *b24;
				b32++;
				b24++;

				*b32 = *b24;
				b32++;
				b24++;

				*b32 = 0;
				b32++;
			}

			*b32 = 0;
			b32++;
		}
	}

	inline void expand_bitmap_24_to_32_bits_per_pixel_no_offset(const int width, const int height, const void *bmp_24bit, void *bmp_32bit)
	{
		const BYTE *b24 = (const BYTE *)bmp_24bit;
		BYTE *b32 = (BYTE *)bmp_32bit;

		for (int j = 0; j<height; j++)
		{
			for (int i = 0; i<width; i++)
			{
				*b32 = *b24; // blue
				b32++;
				b24++;

				*b32 = *b24; // green 
				b32++;
				b24++;

				*b32 = *b24; // red
				b32++;
				b24++;

				*b32 = 0;
				b32++;

			}

			//	*b32 = 0;
			//	b32++;
		}
	}

	inline void compress_bitmap_32_to_24_bits_per_pixel_no_offset(int width, int height, UINT *pixel_array)
	{
		BYTE *b = (BYTE *)pixel_array;

		BYTE *b1 = b;

		int n = width*height;
		for (int i = 0; i<width; i++)
		{
			for (int j = 0; j<height; j++)
			{
				*b = *b1;
				b++;
				b1++;

				*b = *b1;
				b++;
				b1++;

				*b = *b1;
				b++;
				b1 += 2;
			}

			//		b += 1;
		}
	}

	inline UINT GetRed(COLORREF color)
	{
		return 0xFF & color;
	}

	inline UINT GetGreen(COLORREF color)
	{
		return 0xFF & color >> 8;
	}

	inline UINT GetBlue(COLORREF color)
	{
		return 0xFF & color >> 16;
	}

	inline COLORREF AdjustBrightness(COLORREF color, double percent)
	{
		percent = max(percent, 0.0);
		percent = min(percent, 10.0);

		double red = (double)GetRed(color);
		double green = (double)GetGreen(color);
		double blue = (double)GetBlue(color);

		red *= percent;
		red = min(red, 255);

		blue *= percent;
		blue = min(blue, 255);

		green *= percent;
		green = min(green, 255);

		COLORREF ret = RGB((BYTE)red, (BYTE)green, (BYTE)blue);
		return ret;
	}

	inline COLORREF AverageColors(COLORREF color1, COLORREF color2, double weight = 0.5)
	{
		weight = min(weight, 1.0);
		weight = max(weight, 0.0);

		double red1 = (double)GetRed(color1);
		double green1 = (double)GetGreen(color1);
		double blue1 = (double)GetBlue(color1);

		double red2 = (double)GetRed(color2);
		double green2 = (double)GetGreen(color2);
		double blue2 = (double)GetBlue(color2);

		double red = red1 * (1.0 - weight) + red2 * weight;
		double green = green1 * (1.0 - weight) + green2 * weight;
		double blue = blue1 * (1.0 - weight) + blue2 * weight;

		red = min(red, 255);
		red = max(red, 0);

		green = min(green, 255);
		green = max(green, 0);

		blue = min(blue, 255);
		blue = max(blue, 0);

		COLORREF ret = RGB((BYTE)red, (BYTE)green, (BYTE)blue);

		return ret;
	}

	inline void compress_bitmap_32_to_16_bits_per_pixel(int width, int height, UINT*pixel_array)
	{
		WORD *b = (WORD *)pixel_array;

		UINT *b1 = (UINT *)b;

		for (int i = 0; i<width; i++)
		{
			for (int j = 0; j<height; j++)
			{
				UINT red = GetRed(*b1);
				UINT green = GetGreen(*b1);
				UINT blue = GetBlue(*b1);

				*b = (WORD)(red >> 3);
				*b += (WORD)((green >> 2) << 5);
				*b += (WORD)((blue >> 3) << 11);

				b++;
				b1++;
			}
		}
	}

};

#endif
