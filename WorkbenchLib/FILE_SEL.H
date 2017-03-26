#ifndef INC_FILE_SEL
#define INC_FILE_SEL

namespace WorkbenchLib
{

class STRING;

class CFileSpecSelection : public CFileDialog
{
	public:
	
	CFileSpecSelection (BOOL bOpenFileDialog=TRUE);
	~CFileSpecSelection (void);

	BOOL SetFileType (const char *type_name, const char *type_spec);
	BOOL SetStartPath (const char *start_path);

	BOOL SetFileExtension (const char *ext_name) { m_extension = ext_name; return TRUE; }

	UINT GetFilterIndex (void) { return m_filter_index; };

	void HideReadOnly (BOOL status); 
	void PromptIfFileDoesNotExist (BOOL status);
	void PromptIfFileExists (BOOL status);
	void FileMustExist (BOOL status);
	void ReturnOnlyWritableFile (BOOL state);
	void AllowMultipleFileSelection (BOOL state);
	void NoCreatePrompt (BOOL state);
	
	void SetTitleString (const char *title);
	void AlwaysReturnOnOK (void);

	BOOL SetFileName (const char *);
	BOOL Select (void);
	BOOL GetFileName (STRING &file_name);
	const char *GetPath (void);
	BOOL GetNextName (STRING &file_name); 
	
	protected:

//#ifdef WIN32
//	struct tagOFNA m_open_file_data;
//#else
//	struct tagOFN m_open_file_data;
//#endif
	char m_file_types[512];
	short m_idx_file_types;
	char m_path_and_file_name[0xFFFF];
	char m_filename[32];
	char m_initial_directory[2048];
	char m_buffer[64]; 
	unsigned long m_flags;
	CString m_title;
	CString m_extension;

	struct
	{
		UINT select_has_been_processed	: 1;
	} m_status;
	
	UINT m_filter_index;

	UINT m_idx;
};


}; //namespace WorkbenchLib


#endif


