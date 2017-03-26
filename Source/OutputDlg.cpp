// OutputDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MoneroManager.h"
#include "OutputDlg.h"
#include "afxdialogex.h"
#include "DialogMacrosLite.h"


// OutputDlg dialog

IMPLEMENT_DYNAMIC(OutputDlg, CDialogEx)

OutputDlg::OutputDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(OutputDlg::IDD, pParent)
{

}

OutputDlg::~OutputDlg()
{
}

void OutputDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(OutputDlg, CDialogEx)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// OutputDlg message handlers


void OutputDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	CONFORM_RIGHT_SIDE(IDC_Output, 0, 30);	
	CONFORM_BOTTOM_SIDE(IDC_Output, 0, 30);
}
