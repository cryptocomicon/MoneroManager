#include "stdafx.h"
#include "math.h"
#include "Instrumentation.h"
#include "Bitmap.h"
#include "Knob.h"

namespace WorkbenchLib
{

BOOL Knob::OnPaint (CDC *dc, RECT *paint_rect/*=0*/)
{
	int width = 2 * m_radius + 1;
	int height = 2 * m_radius + 1;

	if (paint_rect)
	{
		//////////////////////////////////////////////////////////////////////////////////////////////////
		// Make sure that we don't do any processing for knobs outside the paint rgn
		RECT rect;

		rect.left = m_pos_ix-m_radius;
		rect.top =	m_pos_iy-m_radius;
		rect.right = rect.left + width - 1;
		rect.bottom = rect.top + height - 1;

		if (paint_rect->right < rect.left)
			return FALSE;

		if (paint_rect->left > rect.right)
			return FALSE;

		if (paint_rect->top > rect.bottom)
			return FALSE;

		if (paint_rect->bottom < rect.top)
			return FALSE;
		//////////////////////////////////////////////////////////////////////////////////////////////////
	}

	int n = (m_radius * 2 + 1) * (m_radius * 2 + 1);

	int i;
	for (i=0; i<n; i++)
			m_pixel_array[i] = m_bk_color;

	BOOL iret = Draw3DCircle_to_array (m_pixel_array, m_radius, -135, m_bk_color);

	double t = 0;

	while (1)
	{
		if (m_value < m_data_min)
		{
			t = m_theta_start;
			break;
		}

		if (m_value > m_data_max)
		{
			t = m_theta_end;
			break;
		}

		double mid = (m_data_max + m_data_min) / 2;
		if (m_value < mid)
		{
			t = -PI + (PI + m_theta_start) * (mid - m_value) / (mid - m_data_min);
			break;
		}

		if (m_value >= mid)
		{
			t = PI - (PI - m_theta_end) * (m_value - mid) / (m_data_max - mid);
			break;
		}

		return FALSE;
	}

/*	{ // Line 
		double y = m_radius * cos (t);
		double x = m_radius * sin (t);

		int ix = (int) (x + 0.5);
		int iy = (int) (y + 0.5);

		dc->MoveTo (m_pos_ix,m_pos_iy);
		dc->LineTo (m_pos_ix+ix,m_pos_iy+iy);
	} */

	{
		double y = (3.0 * m_radius / 5.0) * cos (t);
		double x = (3.0 * m_radius / 5.0) * sin (t);

		int ix = (int) (x + 0.5);
		int iy = (int) (y + 0.5);

		ix += m_radius;
		iy += m_radius;

		int radius = (int) (m_radius/5.0);
		n = (2*radius+1)*(2*radius+1);
		UINT *sm_array = new UINT[n];
		if (sm_array)
		{
			for (i=0; i<n; i++)
			{
				sm_array[i] = 0xFFFFFFFF;
			}
		}
			
		iret = Draw3DCircle_to_array (sm_array, radius, 45, m_bk_color);

		int width = m_radius * 2 + 1;
		i = 0;
		if (iret == TRUE)
		{
			for (int jy = iy-radius; jy <= iy+radius; jy++)
			{
				for (int jx = ix-radius; jx <= ix+radius; jx++)
				{
					if (sm_array[i] != 0xFFFFFFFF)
					{
						m_pixel_array[jy*width + jx] = sm_array[i];
					}
					i++;
				}
			}
		}

		delete [] sm_array;
	}

	int ibits_per_pixel = dc->GetDeviceCaps (BITSPIXEL);

	if (ibits_per_pixel != 32)
	{
		if (ibits_per_pixel == 24)
		{
			compress_bitmap_32_to_24_bits_per_pixel (width,height,m_pixel_array);
		}

		if (ibits_per_pixel == 16)
		{
			compress_bitmap_32_to_16_bits_per_pixel (width,height,m_pixel_array);
		}
	}

	CBitmap bitmap;
	unsigned int nPlanes = 1;
	unsigned int nColors = ibits_per_pixel;

	iret = bitmap.CreateBitmap (width,height,nPlanes,nColors,m_pixel_array);

    //////////////////////////////////////////////////////////////////////
    // This code adapted from MFC
    CDC memDC;
    iret = memDC.CreateCompatibleDC(dc);
    CBitmap* pOld = memDC.SelectObject(&bitmap);
    if (pOld == NULL)
	{
		return FALSE;     // destructors will clean up
	}

	iret = dc->BitBlt(m_pos_ix-m_radius, m_pos_iy-m_radius, width, height, &memDC, 0, 0, SRCCOPY);

    memDC.SelectObject(pOld);  
    ////////////////////////////////////////////////////////////////////////

	return TRUE;
}

BOOL Knob::SetKnobRadius (int radius)
{
	m_radius = radius;

	if (m_pixel_array)
		delete [] m_pixel_array;

	m_pixel_array = new UINT[(2*m_radius+1)*(2*m_radius+1)];
	if (!m_pixel_array)
		return FALSE;

	return TRUE;
}

BOOL Knob::SetValueUsingMouse (int ix, int iy)
{
	double value;
	if (get_value_from_pixel (ix,iy,value) != TRUE)
		return FALSE;

	if (m_callback_message_id && m_hWnd)
		PostMessage (m_hWnd,WM_COMMAND,m_callback_message_id,NULL);

	return SetValue (value);
}

};  // namespace WorkbenchLib
