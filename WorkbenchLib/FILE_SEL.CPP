#include <StdAfx.h>

#include <File_Sel.h>

#include <string.h>


namespace WorkbenchLib
{

static STRING tmp_string;

CFileSpecSelection::CFileSpecSelection (BOOL bOpenFileDialog)
	: CFileDialog (bOpenFileDialog) 
{   
	m_idx_file_types = 0;
	m_flags = OFN_CREATEPROMPT | OFN_PATHMUSTEXIST | OFN_LONGNAMES | OFN_EXPLORER | OFN_ENABLEHOOK;
	int i;
	for (i=0; i<512; i++) m_file_types[i] = 0;
	for (i=0; i<2048; i++) m_initial_directory[i] = 0;
    for (i=0; i<32;  i++) m_filename[i] = 0;
	
	MemSet (m_path_and_file_name,0,sizeof(m_path_and_file_name));

	m_status.select_has_been_processed = TRUE;

	m_extension.Empty();
}

void CFileSpecSelection::HideReadOnly (BOOL state)
{
	if (state == TRUE)
		m_flags |= OFN_HIDEREADONLY;
	else
		m_flags &= (OFN_HIDEREADONLY ^ 0xFFFFFFFF);
}

void CFileSpecSelection::PromptIfFileDoesNotExist (BOOL state)
{
	if (state == TRUE)
		m_flags |= OFN_CREATEPROMPT;	
	else
		m_flags &= (OFN_CREATEPROMPT ^ 0xFFFFFFFF);
}

void CFileSpecSelection::PromptIfFileExists (BOOL state)
{
	if (state == TRUE)
		m_flags |= OFN_OVERWRITEPROMPT;
	else
		m_flags &= (OFN_OVERWRITEPROMPT ^ 0xFFFFFFFF);
}

void CFileSpecSelection::ReturnOnlyWritableFile (BOOL state)
{
	if (state == TRUE)
		m_flags |= OFN_NOREADONLYRETURN;
	else
		m_flags &= (OFN_NOREADONLYRETURN ^ 0xFFFFFFFF);
}

void CFileSpecSelection::FileMustExist (BOOL state)
{
	if (state == TRUE)
		m_flags |= OFN_FILEMUSTEXIST;
	else
		m_flags &= (OFN_FILEMUSTEXIST ^ 0xFFFFFFFF);
}

void CFileSpecSelection::AllowMultipleFileSelection (BOOL state)
{
	if (state == TRUE)
	{
		m_flags |= OFN_ALLOWMULTISELECT;
		m_flags |= OFN_EXPLORER;
	}
	else
		m_flags &= (OFN_ALLOWMULTISELECT ^ 0xFFFFFFFF);
}

void CFileSpecSelection::NoCreatePrompt (BOOL state)
{
	if (state == TRUE)
		m_flags &= (OFN_CREATEPROMPT ^ 0xFFFFFFFF);
	else
		m_flags |= OFN_CREATEPROMPT;
}

CFileSpecSelection::~CFileSpecSelection (void) {}

BOOL CFileSpecSelection::SetFileType (const char *type_name, const char *type_spec)
{
	if (strlen (type_name) + strlen (type_spec) + m_idx_file_types + 3 >= 512)
		return FALSE;
	
	short i = m_idx_file_types;
		
	StrMCpy (&m_file_types[i],type_name,512); 	i += (short)strlen (&m_file_types[i]) + 1;
	StrMCpy (&m_file_types[i],type_spec,512);    i += (short)strlen (&m_file_types[i]) + 1;
	
	m_idx_file_types = i;
	
	return TRUE;
}

BOOL CFileSpecSelection::SetStartPath (const char *start_path)
{
	if (strlen (start_path) < 2048)
	{
		StrMCpy (m_initial_directory,start_path,2048);
		return TRUE;
	}
	
	return FALSE;
}

BOOL CFileSpecSelection::SetFileName (const char *name)
{
	MemSet (m_path_and_file_name,0,sizeof(m_path_and_file_name));
	StrMCpy (m_path_and_file_name,name,sizeof(m_path_and_file_name)-1);
    return TRUE;
}

BOOL CFileSpecSelection::Select (void)
{
    // Terminate the file types list
	m_file_types[m_idx_file_types] = 0;  
	
	short i=0;
		
	for (i=0;i<64;i++)  m_buffer[i] = 0;
//#ifdef WIN32	
//	m_open_file_data.lStructSize = sizeof tagOFNA;
//#else
//	m_open_file_data.lStructSize = sizeof tagOFN;
//#endif
	m_ofn.lStructSize = sizeof(OPENFILENAME);
	m_ofn.hwndOwner = NULL;
	m_ofn.hInstance = NULL;           
	m_ofn.lpstrFilter = m_file_types;
	m_ofn.lpstrCustomFilter = m_buffer;
	m_ofn.nMaxCustFilter = 64;
	m_ofn.nFilterIndex = 0;
	m_ofn.lpstrFile = m_path_and_file_name;
	m_ofn.nMaxFile = sizeof(m_path_and_file_name);
	m_ofn.lpstrFileTitle = m_filename;
	m_ofn.nMaxFileTitle = 32; 
	m_ofn.lpstrInitialDir = m_initial_directory;
	m_ofn.lpstrTitle = m_title;
	m_ofn.Flags = m_flags;
//	m_ofn.lpfnHook = NULL;
	m_ofn.lpstrDefExt = m_extension;
	char file_selected = 1;

//	if (!(m_flags & OFN_CREATEPROMPT) && (m_flags & OFN_OVERWRITEPROMPT))
//	{
//		if (!GetSaveFileName (&m_open_file_data))
//			return FALSE;
//	}
//	else
//	{
//		if (!GetOpenFileName (&m_open_file_data))
//			return FALSE;        
//	}

	if (CFileDialog::DoModal () != IDOK)
		return FALSE;

//	*file_pattern = &m_path_and_file_name[m_open_file_data.nFileOffset];
	
//	char c = m_path_and_file_name[m_open_file_data.nFileOffset];
//	m_path_and_file_name[m_open_file_data.nFileOffset] = 0;
//	*path = m_path_and_file_name;
	
//	m_path_and_file_name[m_open_file_data.nFileOffset] = c;
	
	m_status.select_has_been_processed = TRUE;
	m_idx = m_ofn.nFileOffset;
	m_filter_index = m_ofn.nFilterIndex;
	if (m_path_and_file_name[m_idx-1]) // Single selection, convert to multiple sel format
	{
		int i = StrLen (m_path_and_file_name);
		while (i)
		{
			if (m_path_and_file_name[i] == '\\') 
			{
				m_path_and_file_name[i] = 0;
				m_idx = i+1;
				break;
			}
			i--;
		}
	}

	return TRUE;
}

const char *CFileSpecSelection::GetPath (void)
{   
	if (m_status.select_has_been_processed == FALSE)
		return 0;

	return m_path_and_file_name;
}

BOOL CFileSpecSelection::GetNextName (STRING &file_name)
{
	if (m_status.select_has_been_processed == FALSE)
		return 0;

	if (!m_path_and_file_name[m_idx])
		return FALSE;
		
	short idx0 = m_idx;
	
	file_name.SetValidFileName (GetPath (),&m_path_and_file_name[m_idx]);
	m_idx += StrLen (&m_path_and_file_name[m_idx]);
	m_idx++;

	return TRUE;
}

BOOL CFileSpecSelection::GetFileName (STRING &file_name)
{
	if (m_status.select_has_been_processed == FALSE)
		return FALSE;

	STRING file;
	file_name.SetValidFileName (GetPath (),&m_path_and_file_name[m_ofn.nFileOffset]);

	return TRUE;
}

void CFileSpecSelection::SetTitleString (const char *title)
{
	m_title = title;
}

void CFileSpecSelection::AlwaysReturnOnOK (void)
{
}





}; //namespace WorkbenchLib
