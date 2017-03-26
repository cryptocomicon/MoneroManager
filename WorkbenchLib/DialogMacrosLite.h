

#define SET_TEXT(IDC,s)\
{\
	CWnd *__wnd__ = GetDlgItem (IDC);\
	if ((__wnd__) && (const char *)s)\
			{\
		__wnd__->SetWindowText (s);\
	}\
}

#define GET_TEXT(IDC,s)\
{\
	CWnd *__wnd__ = GetDlgItem (IDC);\
	if ((__wnd__))\
	{\
		char __s0__[1024];\
		__s0__[0] = 0;\
		__wnd__->GetWindowText (__s0__,1023);\
		s = __s0__;\
	}\
}

#define BROWSE_FOR_FILE(IDC,TITLE)\
{\
	STRING path,s,file_name,ext;\
\
	GET_TEXT(IDC,s)\
	path.LoadFileName (s,TRUE,FALSE,FALSE);\
	ext.LoadFileName (s,FALSE,FALSE,TRUE);\
	ext.Insert (0,"*");\
\
    CFileSpecSelection file_sel;\
\
    STRING extension;\
\
    file_sel.SetTitleString (TITLE);\
    file_sel.HideReadOnly (TRUE);\
    file_sel.FileMustExist (TRUE);\
	file_sel.PromptIfFileExists (FALSE);\
	file_sel.PromptIfFileDoesNotExist (FALSE);\
	file_sel.SetFileName (ext);\
\
	file_sel.SetStartPath (path);\
\
    if (file_sel.Select () == TRUE)\
	{\
		file_sel.GetFileName (file_name);\
		SET_TEXT(IDC,file_name)\
	}\
}

#define LOAD_FROM_PROFILE(SECTION,ITEM,IDC) \
{\
	char s[1024];\
	GetPrivateProfileString (SECTION,ITEM,"",s,1023,g_profile_file);\
	SET_TEXT(IDC,s)\
}

#define SAVE_TO_PROFILE(SECTION,ITEM,IDC)\
{\
	CString __s__;\
	GET_TEXT(IDC,__s__)\
	WritePrivateProfileString (SECTION,ITEM,__s__,g_profile_file);\
}

#define CONFORM_RIGHT_SIDE(IDC,OFFSET,MIN_WIDTH)\
{\
	RECT rect;\
	GetClientRect (&rect);\
	ClientToScreen (&rect);\
\
	CWnd *wnd = GetDlgItem (IDC);	\
	if (wnd && IsWindow (wnd->m_hWnd) && wnd->IsWindowVisible())\
		{\
		RECT rect_images;\
		wnd->GetClientRect (&rect_images);\
		wnd->ClientToScreen (&rect_images);\
		\
				while (1)\
						{\
			if (rect_images.right < rect.right - OFFSET)\
						{\
				rect_images.right = rect.right - OFFSET;\
				break;\
						}\
						else\
						{\
				rect_images.right = rect.right - OFFSET;\
				rect_images.right = max (rect_images.right,rect_images.left + MIN_WIDTH - 1);\
				break;\
						}\
						}\
\
		WINDOWPLACEMENT wp;\
		wnd->GetWindowPlacement (&wp);\
		wp.rcNormalPosition.right = wp.rcNormalPosition.left + (rect_images.right-rect_images.left+1);\
		wnd->SetWindowPlacement (&wp);\
		}\
}

#define CONFORM_BOTTOM_SIDE(IDC,OFFSET,MIN_HEIGHT)\
{\
	RECT rect;\
	GetClientRect (&rect);\
	ClientToScreen (&rect);\
\
	CWnd *wnd = GetDlgItem (IDC);	\
	if (wnd && IsWindow (wnd->m_hWnd) && wnd->IsWindowVisible())\
		{\
		RECT rect_images;\
		wnd->GetClientRect (&rect_images);\
		wnd->ClientToScreen (&rect_images);\
		\
				while (1)\
						{\
			if (rect_images.bottom < rect.bottom - OFFSET)\
						{\
				rect_images.bottom = rect.bottom - OFFSET;\
				break;\
						}\
						else\
						{\
				rect_images.bottom = rect.bottom - OFFSET;\
				rect_images.bottom = max (rect_images.bottom,rect_images.top + MIN_HEIGHT - 1);\
				break;\
						}\
						}\
\
		WINDOWPLACEMENT wp;\
		wnd->GetWindowPlacement (&wp);\
		wp.rcNormalPosition.bottom = wp.rcNormalPosition.top + (rect_images.bottom-rect_images.top+1);\
		wnd->SetWindowPlacement (&wp);\
		}\
}

