
// MoneroManagerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MoneroManager.h"
#include "MoneroManagerDlg.h"
#include "afxdialogex.h"
#include "DataEntry.h"
#include "DialogMacrosLite.h"
#include "io.h"
#include "direct.h"
#include "OutputDlg.h"
#include "vector"



STRING g_profile_file;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define Timer_ID_Save 1000

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMoneroManagerDlg dialog



CMoneroManagerDlg::CMoneroManagerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMoneroManagerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_inside_load = false;
}

void CMoneroManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMoneroManagerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CMoneroManagerDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_SelectWalletExe, &CMoneroManagerDlg::OnBnClickedSelectwalletexe)
	ON_BN_CLICKED(IDC_SelectWalletFile, &CMoneroManagerDlg::OnBnClickedSelectwalletfile)
	ON_BN_CLICKED(IDC_EnterPassword, &CMoneroManagerDlg::OnBnClickedEnterpassword)
	ON_EN_CHANGE(IDC_WalletExe, &CMoneroManagerDlg::OnEnChangeWalletexe)
	ON_EN_CHANGE(IDC_WalletFile, &CMoneroManagerDlg::OnEnChangeWalletfile)
END_MESSAGE_MAP()


// CMoneroManagerDlg message handlers

BOOL CMoneroManagerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	load_from_profile();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMoneroManagerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMoneroManagerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMoneroManagerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMoneroManagerDlg::OnBnClickedOk()
{
	DataEntry dlg;
	dlg.DoModal();

	CDialogEx::OnOK();
}


void CMoneroManagerDlg::OnBnClickedSelectwalletexe()
{
	BROWSE_FOR_FILE(IDC_WalletExe, "Select monero-wallet-cli.exe");
}


void CMoneroManagerDlg::OnBnClickedSelectwalletfile()
{
	BROWSE_FOR_FILE(IDC_WalletFile, "Select Monero Wallet .keys file");
}


void CMoneroManagerDlg::OnBnClickedEnterpassword()
{
	DataEntry dlg;
	if (dlg.DoModal() == IDOK)
	{
		m_Pwd = dlg.GetString();

		SetCursor(::LoadCursor(NULL, IDC_WAIT));
		LaunchWallet();
		SetCursor(::LoadCursor(NULL, IDC_ARROW));
	}
}

class CommandLineParams
{
public:
	STRING m_working_dir;
	STRING m_command_line;
	CWnd *m_output_wnd;
	std::string m_Pwd;
};

DWORD WINAPI RunCommandLine(void *params);

void CMoneroManagerDlg::LaunchWallet(void)
{
	STRING wallet_exe;
	GET_TEXT(IDC_WalletExe, wallet_exe);

	if (_access(wallet_exe, 00))
	{
		STRING msg = "file: ";
		msg += wallet_exe;
		msg += " Does not exist.";
		AfxMessageBox(msg, MB_ICONERROR);
		return;
	}

	STRING wallet_file;
	GET_TEXT(IDC_WalletFile, wallet_file);

	if (_access(wallet_file, 00))
	{
		STRING msg = "file: ";
		msg += wallet_file;
		msg += " Does not exist.";
		AfxMessageBox(msg, MB_ICONERROR);
		return;
	}

	CommandLineParams *params = new CommandLineParams;

	params->m_command_line = wallet_exe;
	params->m_command_line += " --trusted-daemon --wallet-file=";
	params->m_command_line += wallet_file;

	params->m_working_dir.LoadFileName(wallet_exe, TRUE, FALSE, FALSE);

	CWnd *output_wnd = g_output_dlg.GetDlgItem(IDC_Output);
	if (output_wnd)
		output_wnd->SetWindowTextA("");

	params->m_output_wnd = output_wnd;

	// Transfer the password to the params object to be used in the thread
	params->m_Pwd = m_Pwd;
	
	// Clear the password
	for (int i = 0; i < (int)m_Pwd.length(); i++)
		m_Pwd[i] = ' ';

	// Resize it to zero
	m_Pwd.resize(0);

	g_output_dlg.ShowWindow(SW_SHOW);

	CreateThread(NULL, 0, RunCommandLine, params, 0, 0);
}

void prepare_s_for_rendering(CString &s, const int maxlines)
{
	int len = StrLen(s);

	char *c = new char[len + 1];
	
	int j = 0;

	for (int i = 0; i < len; i++)
	{
		// Don't pass along invalid characters
		if ((s[i] < 32 || s[i] > 126) && s[i] != 0x0a && s[i] != 0x0d)
			continue;

		c[j++] = s[i];
	}

	len = j;
	c[len] = 0;

	// look for the first stand alone cr
	int cr_last = -1;
	for (int i = len - 2; i > 0; i--)
	{
		if (c[i] == 0x0d && c[i + 1] != 0x0a)
		{
			cr_last = i;
			break;
		}
	}

	// stand alone cr found
	int lf_next = -1;
	if (cr_last != -1)
	{
		// find the next lf character, looking back
		for (int i = cr_last - 1; i > 0; i--)
		{
			if (c[i] == 0x0a)
			{
				lf_next = i;
				break;
			}
		}

		if (lf_next != -1)
		{
			int nremove = cr_last - lf_next;
			MemRemove(c, len+1, nremove, lf_next + 1);
		}
	}

	s = c;
	delete[] c;
}

DWORD WINAPI RunCommandLine(void *p)
{
	CommandLineParams *params = (CommandLineParams *)p;

	const char *command = params->m_command_line.GetStringConst();
	const char *working_dir = params->m_working_dir.GetStringConst();
	CWnd *output_wnd = params->m_output_wnd;

	/////////////////////////////////////////////////////////////////////////////////////
	// Create the pipes that will be used to communicate with the process
	HANDLE hReadChildStdOut, hWriteChildStdOut;
	HANDLE hReadChildStdIn, hWriteChildStdIn;

	// Allocate a SECURITY_ATTRIBUTES structure, zero it except for
	// the nLength and the bInheritHandle members.
	SECURITY_ATTRIBUTES sec_attrib;
	MemSet(&sec_attrib, 0, sizeof(SECURITY_ATTRIBUTES));
	sec_attrib.nLength = sizeof(SECURITY_ATTRIBUTES);
	// This parameter must be true in order to pipe the stdout and stderr streams
	sec_attrib.bInheritHandle = TRUE;

	// Create an anonymous pipe with a buffer size of 1024 bytes.
	BOOL iret = CreatePipe(&hReadChildStdOut, &hWriteChildStdOut, &sec_attrib, 1024);
	ASSERT(iret);

	iret = CreatePipe(&hReadChildStdIn, &hWriteChildStdIn, &sec_attrib, 1024);
	ASSERT(iret);

	/////////////////////////////////////////////////////////////////////////////////////

	BOOL				bInheritHandles;
	DWORD				dwCreationFlags;
	STARTUPINFO			StartupInfo;

	PROCESS_INFORMATION ProcessInfo;

	// This parameter must be true in order to pipe the stdout and stderr streams
	bInheritHandles = TRUE;
	dwCreationFlags = 0;
	MemSet(&StartupInfo, 0, sizeof(StartupInfo));
	MemSet(&ProcessInfo, 0, sizeof(ProcessInfo));

	StartupInfo.cb = sizeof(STARTUPINFO);
	StartupInfo.dwFlags = STARTF_USESHOWWINDOW;
	StartupInfo.wShowWindow = SW_HIDE;

	// Transfer the pipes to the new process via this assignment to StartupInfo
	StartupInfo.dwFlags |= STARTF_USESTDHANDLES;
	StartupInfo.hStdInput = hReadChildStdIn;
	StartupInfo.hStdOutput = hWriteChildStdOut;
	StartupInfo.hStdError = NULL;

	char *_command_line = (char *)alloca(StrLen(command)+1);
	ASSERT(_command_line);
	strcpy_s(_command_line, StrLen(command) + 1, command);

	if (!CreateProcess(NULL,
		_command_line,
		NULL,
		NULL,
		bInheritHandles,
		dwCreationFlags,
		NULL,					// environment block - use Create's
		working_dir,		// current directory of child process 
		&StartupInfo,
		&ProcessInfo))
	{
		CloseHandle(hReadChildStdOut);
		CloseHandle(hWriteChildStdOut);
		CloseHandle(hReadChildStdIn);
		CloseHandle(hWriteChildStdIn);

		// CreateProcess failed - find out why and tell user
		DWORD dwLastError;

		dwLastError = GetLastError();

		STRING msg = "Problem creating process with command line: ";
		msg += _command_line;
		msg += "\n\n";
		msg += "Error code: ";
		msg += dwLastError;
		AfxMessageBox(msg, MB_ICONERROR);

		return 0;
	}

	DWORD nread;
	char buffer[10000];
	int ipos = 0;

	DWORD nwritten;

	STRING pwd = params->m_Pwd.c_str();
	params->m_Pwd.push_back('\r');
	params->m_Pwd.push_back('\n');

	iret = WriteFile(hWriteChildStdIn, params->m_Pwd.c_str(), params->m_Pwd.length(), &nwritten, NULL);

	// clear the password in memory
	int len = params->m_Pwd.length();
	for (int i = 0; i < len; i++)
		params->m_Pwd[i] = ' ';

	params->m_Pwd.resize(0);

	COMMTIMEOUTS timeout_params;
	memset(&timeout_params, 0, sizeof(timeout_params));
	timeout_params.ReadIntervalTimeout = MAXWORD;

	SetCommTimeouts(hReadChildStdOut, &timeout_params);

	bool bReturnAfterNextRead = false;

	CString s;

	DWORD ret;

	while (ReadFile(hReadChildStdOut, buffer, sizeof(buffer), &nread, NULL) == TRUE)
	{
		if (nread)
		{
			if (output_wnd)
			{
				output_wnd->GetWindowTextA(s);
				s.Append(buffer, nread);

				int maxlines = 20;
				prepare_s_for_rendering(s, maxlines);

				output_wnd->SetWindowTextA(s);
			}

			const char *c = strstr(s, "Balance: ");
			if (c)
			{
				nwritten = 0;
				iret = WriteFile(hWriteChildStdIn, "exit\n", 5, &nwritten, NULL);

				if (iret == TRUE && nwritten == 5)
				{
					output_wnd->GetWindowTextA(s);
					s += "exit\r\n";
					output_wnd->SetWindowTextA(s);
				}

				const char *cend = strchr(c, '\r');
				
				STRING sout;
				if (cend)
					sout.Insert(0, c, cend - c);
				else
					sout = c;
	
				AfxMessageBox(sout, MB_OK);

				// Wait 20 sec for process to die
				DWORD ret = WaitForSingleObject(ProcessInfo.hThread, 20000);
				if (ret == WAIT_TIMEOUT)
					TerminateProcess(ProcessInfo.hProcess, 0); // kill the process if it did not die

				goto Cleanup;
			}

			if (strstr(s, "Error: failed to load wallet: invalid password"))
				break;

			if (strstr(s, "Error: failed to load wallet: std::bad_alloc"))
				break;
		}

		if (bReturnAfterNextRead)
			break;

		ret = WaitForSingleObject(ProcessInfo.hThread, 1000);
		if (ret != WAIT_TIMEOUT)
			bReturnAfterNextRead = true;
	}

	ret = WaitForSingleObject(ProcessInfo.hThread, 10);
	if (ret != 0)
		TerminateProcess(ProcessInfo.hProcess, 0);

	if (strstr(s, "Error: failed to load wallet: invalid password"))
		AfxMessageBox("Error: failed to load wallet: invalid password\n", MB_ICONERROR);
	else
	if (strstr(s, "Error: failed to load wallet: std::bad_alloc"))
		AfxMessageBox("key file is incompatible with the monero-wallet-cli.exe", MB_ICONERROR);

Cleanup:

	CloseHandle(hReadChildStdOut);
	CloseHandle(hWriteChildStdOut);
	CloseHandle(hReadChildStdIn);
	CloseHandle(hWriteChildStdIn);

	return 0;
}

void CMoneroManagerDlg::OnEnChangeWalletexe()
{
	if (m_inside_load)
		return;

	save_to_profile();
}


void CMoneroManagerDlg::OnEnChangeWalletfile()
{
	if (m_inside_load)
		return;

	save_to_profile();
}

bool CMoneroManagerDlg::save_to_profile(void)
{
	const char *Section = "CMoneroManagerDlg";
	SAVE_TO_PROFILE(Section, "WalletExe", IDC_WalletExe);
	SAVE_TO_PROFILE(Section, "WalletFile", IDC_WalletFile);
	return true;
}

bool CMoneroManagerDlg::load_from_profile(void)
{
	m_inside_load = true;
	const char *Section = "CMoneroManagerDlg";
	LOAD_FROM_PROFILE(Section, "WalletExe", IDC_WalletExe);
	LOAD_FROM_PROFILE(Section, "WalletFile", IDC_WalletFile);
	m_inside_load = false;
	return true;
}




