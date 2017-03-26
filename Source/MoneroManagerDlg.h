
// MoneroManagerDlg.h : header file
//

#include <string>

#pragma once


// CMoneroManagerDlg dialog
class CMoneroManagerDlg : public CDialogEx
{
// Construction
public:
	CMoneroManagerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_APPLICATIONMANAGER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

private:

	::std::string m_Pwd;

	void LaunchWallet(void);

	bool m_inside_load;

	bool save_to_profile(void);
	bool load_from_profile(void);

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedSelectwalletexe();
	afx_msg void OnBnClickedSelectwalletfile();
	afx_msg void OnBnClickedEnterpassword();
	afx_msg void OnEnChangeWalletexe();
	afx_msg void OnEnChangeWalletfile();
};
