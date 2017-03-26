// DataEntry.cpp : implementation file
//

#include "stdafx.h"
#include "MoneroManager.h"
#include "DataEntry.h"
#include "afxdialogex.h"

const char g_char_list[] =
{ 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
' ', '_', '~', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '+', '-', '=', '{', '}', '[', ']', ';', ':', '\"', '\'', '<', '>', ',', '.', '?', '/', '|', '\\' };

// DataEntry dialog

IMPLEMENT_DYNAMIC(DataEntry, CDialogEx)

DataEntry::DataEntry(CWnd* pParent /*=NULL*/)
	: CDialogEx(DataEntry::IDD, pParent)
{
	m_Char = 0;
}

DataEntry::~DataEntry()
{
}

void DataEntry::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(DataEntry, CDialogEx)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_BN_CLICKED(IDOK, &DataEntry::OnBnClickedOk)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_COMMAND(ID_NeedToUpdateKnob, &DataEntry::OnNeedtoupdateknob)
	ON_BN_CLICKED(ID_Backspace, &DataEntry::OnBnClickedBackspace)
END_MESSAGE_MAP()


// DataEntry message handlers


void DataEntry::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	COLORREF color = m_Knob.GetBKColor();

	RECT client_rect;
	GetClientRect(&client_rect);
	dc.FillSolidRect(&client_rect, color);

	m_Knob.OnPaint(&dc);

	if (m_Char)
	{
		dc.SelectObject(&m_Font);
		dc.SetBkMode(TRANSPARENT);

		SIZE sz;
		GetTextExtentPointA(dc, &m_Char, 1, &sz);

		RECT rect = m_Knob.GetKnobRect();
		
		int ix = ((rect.left + rect.right) / 2) - sz.cx/2;
		int iy = ((rect.top + rect.bottom) / 2) - sz.cy/2;

		TextOut(dc, ix, iy, &m_Char, 1);
	}

	// Write the length of the string so far
	if (m_String.length())
	{
		dc.SelectObject(&m_Font);
		dc.SetBkMode(OPAQUE);
		dc.SetBkColor(m_Knob.GetBKColor());

		char s[30];
		sprintf_s(s, sizeof(s), "%ld characters    ", m_String.length());

		TextOut(dc, 5, 5, s, strlen(s));
	}

}


int DataEntry::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_Knob.SetKnobRadius(100);
	m_Knob.SetKnobPosition(110, 140);
	m_Knob.SetWindow(m_hWnd);
	m_Knob.SetCallbackMessageID(ID_NeedToUpdateKnob);
	m_Knob.SetKnobDataMin(0);
	m_Knob.SetKnobDataMax(sizeof(g_char_list) - 1);

	m_Font.CreateFont(-30, 0, 0, 0, 400, 0, 0, 0, 0, 0, 0, 0, 0, _T("Arial"));

	return 0;
}

void DataEntry::OnBnClickedOk()
{

	CDialogEx::OnOK();
}

void DataEntry::randomize_offset(void)
{
	SYSTEMTIME st;
	GetSystemTime(&st);

	FILETIME ft;
	SystemTimeToFileTime(&st, &ft);

	unsigned __int64 t;
	memmove(&t, &ft, sizeof(t));

	t -= 100000;

	t = t % sizeof(g_char_list);

	m_Offset = (int)t;
}

void DataEntry::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_Knob.PointIsOnKnob(point.x, point.y))
	{
		m_Knob.TurnOnMouseTracking();
		randomize_offset();
	}

	CDialogEx::OnLButtonDown(nFlags, point);
}


void DataEntry::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!(nFlags & MK_LBUTTON))
	{
		m_Knob.TurnOffMouseTracking(); // Left mouse button is not down

		// Reset the character
		m_Char = 0;

		RECT rect = m_Knob.GetKnobRect();

		// Clear the letter
		InvalidateRect(&rect, FALSE);
	}

	m_Knob.OnMouseMove(point.x, point.y);

	CDialogEx::OnMouseMove(nFlags, point);
}


void DataEntry::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_Knob.TurnOffMouseTracking();

	// Add one more character to the string
	m_String.push_back(m_Char);

	// Reset the character
	m_Char = 0;

	// Clear the letter and update the character count
	Invalidate(FALSE);

	CDialogEx::OnLButtonUp(nFlags, point);
}


void DataEntry::OnNeedtoupdateknob()
{
	RECT rect = m_Knob.GetKnobRect();
	InvalidateRect(&rect, FALSE);

	double value = m_Knob.GetValue();
	int i = (int)round(value);

	if (m_Knob.GetLeftShiftStatus() == true)
	{
		if (i > 25 || i < 0)
			i %= 26;

		m_Char = 'a' + i;
	}
	else if (m_Knob.GetRightShiftStatus() == true)
	{
		if (i > 25 || i < 0)
			i %= 26;

		m_Char = 'A' + i;
	}
	else if (m_Knob.GetLeftControlStatus() == true)
	{
		if (i > 9 || i < 0)
			i %= 10;

		m_Char = '0' + i;
	}
	else if (m_Knob.GetRightControlStatus() == true)
	{
		if (i > 32 || i < 0)
			i %= 32;

		int idx = sizeof(g_char_list) - 32;
		m_Char = g_char_list[idx + i];
	}
	else
	{
		i = (i + m_Offset) % sizeof(g_char_list);
		m_Char = g_char_list[i];
	}
}

void DataEntry::OnBnClickedBackspace()
{
	if (m_String.size())
	{
		m_String.resize(m_String.size() - 1);
		Invalidate(FALSE);
	}
}
