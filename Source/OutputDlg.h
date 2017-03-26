#pragma once


// OutputDlg dialog

class OutputDlg : public CDialogEx
{
	DECLARE_DYNAMIC(OutputDlg)

public:
	OutputDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~OutputDlg();

// Dialog Data
	enum { IDD = IDD_Output };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
};

extern OutputDlg g_output_dlg;
