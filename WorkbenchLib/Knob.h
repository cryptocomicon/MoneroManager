#include "instrumentation.h"
#include "colors.h"
#include "math.h"

#ifndef INC_KNOB
#define INC_KNOB

namespace WorkbenchLib
{

// Reverse the green and blue bytes
inline void reverse_rb(const COLORREF &color, COLORREF &color1)
{
	BYTE b[4];
	memmove(b, &color, sizeof(color));
	BYTE b1[4];
	b1[0] = b[2];
	b1[1] = b[1];
	b1[2] = b[0];
	b1[3] = b[3];
	memmove(&color1, b1, sizeof(color1));
}

class Knob
{
public:
	inline Knob (void)
	{
		m_hWnd = 0;
		m_mouse_tracking_on = FALSE;
		m_radius = 0;;
		m_pos_ix = 0;
		m_pos_iy = 0;
		m_value = 0.0;
		m_theta_start = 0; //-PI / 8;
		m_theta_end = 0; //+PI / 8;
		m_data_min = 0;
		m_data_max = 100.0;
		m_pixel_array = 0;
		m_bk_color = m_bk_color = RGB(200, 208, 212);
		m_callback_message_id = 0;

		m_lshift = false;
		m_rshift = false;
		m_lcontrol = false;
		m_rcontrol = false;
	}

	inline ~Knob (void)
	{
		if (m_pixel_array)
			delete [] m_pixel_array;
	}

	inline void SetBKColor (COLORREF color)
	{
		COLORREF color1;
		reverse_rb(color, color1);
		m_bk_color = color1;
	}

	inline COLORREF GetBKColor(void) const
	{
		COLORREF ret_color;
		reverse_rb(m_bk_color, ret_color);
		return ret_color;
	}

	inline void SetWindow (HWND hwnd)
	{
		m_hWnd = hwnd;
	}

	inline void SetCallbackMessageID (UINT ID)
	{
		m_callback_message_id = ID;
	}

	BOOL SetKnobRadius (int radius);

	inline void SetKnobPosition (int ix, int iy)
	{
		m_pos_ix = ix;
		m_pos_iy = iy;
	}

	inline BOOL SetKnobDataMax (double max)
	{
		m_data_max = max;
		return TRUE;
	}

	inline BOOL SetKnobDataMin (double min)
	{
		m_data_min = min;
		return TRUE;
	}

	inline double GetValue (void)
	{
		return m_value;
	}

	inline BOOL SetValue (double value)
	{
		value = max (m_data_min,value);
		value = min (m_data_max,value);

		if ((value != m_value) && m_hWnd)
		{
			RECT rect;
			rect = get_knob_rect ();
			InvalidateRect (m_hWnd,&rect,FALSE);
		}

		m_value = value;

		return TRUE;
	}

	BOOL SetValueUsingMouse (int ix, int iy);
	BOOL OnPaint (CDC *dc, RECT *rect=0);

	inline void TurnOnMouseTracking (void)
	{
		m_mouse_tracking_on = TRUE;
	}

	inline void TurnOffMouseTracking (void)
	{
		m_mouse_tracking_on = FALSE;
	}

	inline void OnMouseMove (int ix, int iy)
	{
		if (m_mouse_tracking_on == FALSE)
			return;

		m_lshift = GetKeyState(VK_LSHIFT) & (0x01 << 16) ? true : false;
		m_rshift = GetKeyState(VK_RSHIFT) & (0x01 << 16) ? true : false;
		m_lcontrol = GetKeyState(VK_LCONTROL) & (0x01 << 16) ? true : false;
		m_rcontrol = GetKeyState(VK_RCONTROL) & (0x01 << 16) ? true : false;

		if (m_lshift || m_rshift)
		{
			SetKnobDataMax(25);
		}
		else if (m_lcontrol)
		{
			SetKnobDataMax(10);
		}
		else if (m_rcontrol)
		{
			SetKnobDataMax(32);
		}

		SetValueUsingMouse (ix,iy);
	}

	inline BOOL PointIsOnKnob (int ix, int iy)
	{
		RECT rect = get_knob_rect ();
		
		if (ix < rect.left) 
			return FALSE;
		
		if (ix > rect.right)
			return FALSE;
		
		if (iy < rect.top)
			return FALSE;

		if (iy > rect.bottom)
			return FALSE;

		return TRUE;
	}

	inline RECT GetKnobRect(void) const
	{
		return get_knob_rect();
	}

	inline bool GetLeftShiftStatus(void) const
	{
		return m_lshift;
	}

	inline bool GetRightShiftStatus(void) const
	{
		return m_rshift;
	}

	inline bool GetLeftControlStatus(void) const
	{
		return m_lcontrol;
	}

	inline bool GetRightControlStatus(void) const
	{
		return m_rcontrol;
	}

protected:

	inline RECT get_knob_rect (void) const
	{
		RECT rect;

		rect.left = m_pos_ix - m_radius;
		rect.right = m_pos_ix + m_radius;
		rect.top = m_pos_iy - m_radius;
		rect.bottom = m_pos_iy + m_radius;

		return rect;
	}

	inline BOOL get_value_from_pixel (int ix, int iy, double &value)
	{
		// Compute theta
		double t = atan2 ((double)(ix-m_pos_ix), (double)(iy-m_pos_iy)); // -PI to +PI

		if (t > 0 && t < m_theta_end)
		{
			value = m_data_max;
			return TRUE;
		}

		if (t < 0 && t > m_theta_start)
		{
			value = m_data_min;
			return TRUE;
		}

		double mid = (m_data_max - m_data_min) / 2.0;

		if (t < m_theta_start)
		{
			value = mid - mid * ((t + PI)/(m_theta_start + PI));
			return TRUE;
		}

		if (t > m_theta_end)
		{
			value = mid + mid * (t - PI)/(m_theta_end - PI);
			return TRUE;
		}

		return FALSE;
	}

	HWND m_hWnd;
	BOOL m_mouse_tracking_on;
	long m_radius;
	long m_pos_ix, m_pos_iy;
	double m_value;
	double m_theta_start,m_theta_end;
	double m_data_min,m_data_max;
	UINT *m_pixel_array;
	COLORREF m_bk_color;
	UINT m_callback_message_id;
	bool m_lshift, m_rshift, m_lcontrol, m_rcontrol;
};


};

#endif
