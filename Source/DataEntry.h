#pragma once

#include "Knob.h"
#include <string>

// DataEntry dialog

class DataEntry : public CDialogEx
{
	DECLARE_DYNAMIC(DataEntry)

public:
	DataEntry(CWnd* pParent = NULL);   // standard constructor
	virtual ~DataEntry();

	const char *GetString(void) const
	{
		return m_String.c_str();
	}

private:
	WorkbenchLib::Knob m_Knob;
	char m_Char;
	std::string m_String;
	CFont m_Font;
	int m_Offset; // Random offset for knob vs. value

	void randomize_offset(void);


// Dialog Data
	enum { IDD = IDD_DataEntry };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBnClickedOk();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnNeedtoupdateknob();
	afx_msg void OnBnClickedBackspace();
};
