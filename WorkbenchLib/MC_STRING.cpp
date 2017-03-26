#include "stdafx.h"
#include "stdio.h"
#include "memory.h"
#include "MC_STRING.h"
#include "Math.h"

namespace WorkbenchLib
{

STRING **STRING::m_ring_buf = 0;
int STRING::m_ring_buf_sz = 0;
int STRING::m_ring_buf_idx = 0;
int STRING::m_break_at_ring_buf_idx = 0xFFFFFFFF;
	
BOOL STRING::StartLeakDetection (int ring_buf_sz)
{
	if (m_ring_buf)
		free (m_ring_buf);

	m_ring_buf = (STRING **)malloc (ring_buf_sz * sizeof(STRING *));
	if (!m_ring_buf)
		return FALSE;

	MemSet (m_ring_buf,0,ring_buf_sz * sizeof(STRING *));

	m_ring_buf_sz = ring_buf_sz;
	m_ring_buf_idx = 0;

	return TRUE;
}

BOOL STRING::BreakAtLeak (int idx)
{
	m_break_at_ring_buf_idx = idx%m_ring_buf_sz;
	return TRUE;
}

BOOL STRING::StopLeakDetection (void)
{
	if (m_ring_buf)
		free (m_ring_buf);

	m_ring_buf_sz = 0;
	m_ring_buf_idx = 0;
	m_ring_buf = 0;
	m_break_at_ring_buf_idx = 0;

	return TRUE;
}

STRING *STRING::GetNextLeak (int &idx)
{
	if (!m_ring_buf_sz)
		return 0;

	idx = idx%m_ring_buf_sz;

	int idx0 = idx;
	int n = 0;
	for (idx=idx0; n<m_ring_buf_sz; idx=(idx+1)%m_ring_buf_sz)
	{
		n++;
		if (m_ring_buf[idx])
			return m_ring_buf[idx++];
	}

	return 0;
}

void STRING::UPDATE_LEAK_DETECTION (void)
{
	if (m_ring_buf && m_ring_buf_sz)
	{
		if (m_ring_buf_idx == m_break_at_ring_buf_idx)
			ASSERT (FALSE);

		m_ring_buf[m_ring_buf_idx%m_ring_buf_sz] = this;
		m_ring_buf_idx++;
	}
}


//////////////////////////////////////////////////////////////////
// Begin defintion of STRING class
//

BOOL STRING::SetTag (const char *tag, const char *value)
{
	int istart=-1,iend=-1;

	if (IncludesTag (tag,0,&istart,&iend) == FALSE)
	{
		int len = StrLen (tag);
		
		char *s_tag = (char *)alloca (len+4);

		s_tag[0] = '<';

		int i=0;
		while (i<len)
		{
			s_tag[i+1] = tag[i];
			i++;
		}
		s_tag[i+1] = '>';
		s_tag[i+2] = 0;

		Append (s_tag);
		Append (value);
		MemInsert (s_tag,len+3,1,1,"/");
		Append (s_tag);

		return TRUE;
	}

	if (Remove (istart,iend-istart+1) == FALSE)
		return FALSE;

	if (Insert (istart,value) == FALSE)
		return FALSE;

	return TRUE;
}

BOOL STRING::GetTag (const char *tag, STRING &value) const
{
	return IncludesTag (tag,&value);
}

BOOL STRING::IncludesTag (const char *tag, STRING *value/*=0*/, int *istart/*=0*/, int *iend/*=0*/) const
{
	if (tag == 0)
		return FALSE;

	int len = StrLen (tag);
	if (len > 128)
		len = 128;
	
	char *s_tag = (char *)alloca (len+10);
	
	s_tag[0] = '<';

	int i=0;
	while (i<len)
	{
		s_tag[i+1] = tag[i];
		i++;
	}
	s_tag[i+1] = '>';
	s_tag[i+2] = 0;
	
	const char *start = StrStr (s,s_tag);

	if (start == 0)
		return FALSE;

	MemInsert (s_tag,len+3,1,1,"/");

	const char *end = StrStr (start,s_tag);
	if (!end)
		return FALSE;

	if (value)
		value->Assign (start+len+2,end-start-(len+2));

	if (istart)
		*istart = start+len+2-s;

	if (iend)
		*iend = end-1-s;

	return TRUE;

}

BOOL STRING::AppendSpaces (int num)
{
	int len_old = StrLen (s);

	int len_new = len_old + num;

	char *s_new = new char[len_new+1];
	if (!s_new)
		return FALSE;

	MemMove (s_new,s,len_old);
	MemSet (s_new+len_old,' ',num);
	s_new[len_new] = 0;

	delete [] s;

	s = s_new;

	return TRUE;
}

BOOL STRING::ConvertToAbnormalHtml (void)
{
	int len = StrLen (s);
	char *s1 = new char[3*len+1];
	MemSet (s1,0,3*len+1);

	int j=0;
	for (int i=0; i<len; i++)
	{
		if (s[i] >= 'a' && s[i] <= 'z')
		{
			s1[j++] = s[i];
			continue;
		}

		if (s[i] >= 'A' && s[i] <= 'Z')
		{
			s1[j++] = s[i];
			continue;
		}

		if (s[i] >= '0' && s[i] <= '9')
		{
			s1[j++] = s[i];
			continue;
		}

		if (s[i] == '_')
		{
			s1[j++] = s[i];
			continue;
		}

		s1[j++] = '%';
		sprintf_s (&s1[j],(3*len+1)-j,"%02X",(WORD)s[i]);
		j += 2;
	}

	*this = s1;
	delete [] s1;

	return TRUE;
}

long STRING::WordCount ()
{
	long N = 0;
	long i = 0;

	while (1)
	{
		while (s[i] && IsWhiteSpaceChar (s[i])) i++;
		if (!s[i]) break;
		N++;

		while (s[i] && IsWhiteSpaceChar (s[i]) == FALSE) i++;
		if (!s[i]) break;
	}

	return N;
}

BOOL STRING::NormalizeHtml (void)
{
	Replace ("+"," ");

	int n = StrLen (s);
	for (int i=0; i<n-2; i++)
	{
		if (s[i] == '%')
		{
			BYTE b;
			if (get_BYTE_hex (&s[i+1],b) == TRUE)
			{
				s[i] = (char) b;
				MemRemove (&s[i+1],StrLen(&s[i+1])+1,2,0);
			}
		}
	}

	return TRUE;
}

const char *STRING::AppendUpTo (const char *s0, const char *token)
{
	const char *c = StrStr (s0,token);
	if (c == 0)
	{
		int len0 = StrLen (s);
		Insert (len0,s0);
		return 0;
	}

	int len0 = StrLen (s);
	int len1 = c - s0;
	Insert (len0,s0,len1);
	return c;
}

BOOL STRING::ForceSuffix (const char *suffix, BOOL case_sensitive/*=FALSE*/)
{
	int len = StrLen (s);
	int suffix_len = StrLen (suffix);

	if (len >= suffix_len)
	{
		if (IsEqualTo (&s[len-suffix_len],suffix,case_sensitive) == TRUE)
			return TRUE;
	}

	*this += suffix;

	return TRUE;
}


BOOL STRING::Append_s (const char *s0,const char *s1/*=0*/,const char *s2/*=0*/,const char *s3/*=0*/,const char *s4/*=0*/,const char *s5/*=0*/,const char *s6/*=0*/,const char *s7/*=0*/,const char *s8/*=0*/,const char *s9/*=0*/)
{
	int len = 0;
	while (1)
	{
		if (s0) len += StrLen (s0); else break;
		if (s1) len += StrLen (s1); else break;
		if (s2) len += StrLen (s2); else break;
		if (s3) len += StrLen (s3); else break;
		if (s4) len += StrLen (s4); else break;
		if (s5) len += StrLen (s5); else break;
		if (s6) len += StrLen (s6); else break;
		if (s7) len += StrLen (s7); else break;
		if (s8) len += StrLen (s8); else break;
		if (s9) len += StrLen (s9); else break;
		break;
	}

	char *tmp = (char *) alloca (len+1);

	int i = 0;
	int n;
	while (1)
	{
		if (s0) { n = StrLen (s0); MemMove (tmp+i,s0,n); i += n; } else break;
		if (s1) { n = StrLen (s1); MemMove (tmp+i,s1,n); i += n; } else break;
		if (s2) { n = StrLen (s2); MemMove (tmp+i,s2,n); i += n; } else break;
		if (s3) { n = StrLen (s3); MemMove (tmp+i,s3,n); i += n; } else break;
		if (s4) { n = StrLen (s4); MemMove (tmp+i,s4,n); i += n; } else break;
		if (s5) { n = StrLen (s5); MemMove (tmp+i,s5,n); i += n; } else break;
		if (s6) { n = StrLen (s6); MemMove (tmp+i,s6,n); i += n; } else break;
		if (s7) { n = StrLen (s7); MemMove (tmp+i,s7,n); i += n; } else break;
		if (s8) { n = StrLen (s8); MemMove (tmp+i,s8,n); i += n; } else break;
		if (s9) { n = StrLen (s9); MemMove (tmp+i,s9,n); i += n; } else break;
		break;
	}

	tmp[i] = 0;

	*this = tmp;

	return TRUE;
}

BOOL STRING::operator >= (const char *s0)
{
	if (strcmp (s,s0) >= 0) return TRUE;
	return FALSE;
}

BOOL STRING::operator <= (const char *s0)
{
	if (strcmp (s,s0) <= 0) return TRUE;
	return FALSE;
}


BOOL STRING::operator == (const char *s0)
{
	if (!strcmp (s0,s))
		return TRUE;
	
	return FALSE;
}

BOOL STRING::operator != (const char *s0)
{
	if (strcmp (s0,s))
		return TRUE;
		
	return FALSE;
}

BOOL STRING::operator >= (STRING &s0)
{
	return (*this >= (const char *) s0);
}                                       

BOOL STRING::operator <= (STRING &s0)
{
	return (*this <= (const char *) s0);
}

BOOL STRING::operator != (STRING &s0)
{
	return (*this == (const char *) s0);
}

#ifdef __AFX_H__

BOOL STRING::operator >= (CString &s0)
{
	return (*this >= (const char *) s0);
}                                       

BOOL STRING::operator <= (CString &s0)
{
	return (*this <= (const char *) s0);
}

BOOL STRING::operator != (CString &s0)
{
	return (*this == (const char *) s0);
}

STRING &STRING::operator = (CString &s0)
{
	*this = (const char *)s0;
	return (*this);
}

STRING &STRING::operator += (CString &s0)
{
	*this += (const char *)s0;
	return (*this);
}

#endif

BOOL STRING::HasLen (void) const
{
	if (s == 0)
		return FALSE;

    if (s[0]) return TRUE;
    return FALSE;
}

void STRING::UpperCase (void)
{   
	int n = len ();
	for (int i=0; i<n; i++)
	{
		if (s[i] >= 'a' && s[i] <= 'z')
			s[i] += 'A' - 'a';
	}
} 

void STRING::LowerCase (void)
{   
	int n = len ();
	for (int i=0; i<n; i++)
	{
		if (s[i] >= 'A' && s[i] <= 'Z')
			s[i] += 'a' - 'A';
	}
}

void STRING::Truncate (int len)
{
	if (STRING::len () < len) return;
	s[len] = 0;
} 

BOOL STRING::AssignBeforeSubString (const char *s0, const char *substr)
{
	const char *c = strstr (s0,substr);
	
	if (!c)
	{
		*this = "";
		return FALSE;
	}
	
	int n = (int) (c - s0);
	Assign (s0,n);
	
	return TRUE;
}
	

BOOL STRING::AssignAfterSubString (const char *s0, const char *substr, int instance/*=1*/)
{
	if (instance < 1 || instance > 1000)
		return FALSE;

	const char *c = 0;
	while (instance)
	{
		c = strstr (s0,substr);
		
		if (!c)
		{
			*this = "";
			return FALSE;
		}

		instance--;

		s0 = c + strlen (substr);
	}

	*this = (const char *) (c + strlen (substr));
	return TRUE;
}

int STRING::FirstDifference (const char *s0)
{
	const char *c = (const char *) *this;
	
	int i=0;
	
	while (1)
	{
		if (s0[i] != c[i]) return (i);
		if (!s0[i])
		{
			if (!c[i]) return (-1);
			return (i);
		}                     
		
		if (!c[i]) return (i);
		
		i++;
	}
}

BOOL STRING::RemoveSubString (const char *s0)
{
	if (s0[0] == 0)
		return FALSE;

	const char *c;
	BOOL iret = FALSE;
	
	while (1)
	{
		c = strstr (s,s0);
		if (!c) break;
		
		iret = TRUE;
		
		if (c)
		{
			int i = c - s;
			int n = strlen (s0);
			
			while (1)
			{
				s[i] = s[i+n];
				i++;
				if (!s[i]) break;
			}   

			break;
		}
	}               
	
	return iret;
}

int STRING::len (void)
{
	return (strlen (s));
}

void STRING::Trim (BOOL leading/*=TRUE*/, BOOL extra/*=TRUE*/, BOOL trailing/*=TRUE*/)
{
	trim (s,leading,extra,trailing);
}

int STRING::GetNextSubStr (int i, STRING &sub_str)
{
	int l = StrLen (s);
	if (i >= l)
		return -1;

	while (IsWhiteSpaceChar (s[i])) i++;
	if (s[i] == 0)
		return -1;

	int i0 = i;

	while (s[i] && !IsWhiteSpaceChar (s[i])) i++;

	sub_str.Insert (0,&s[i0],i-i0);
	
	return i;
}
	

int STRING::NextWord (int i)
{   
	int l = len ();
	
	if (!l) 
		return (0);
	
	i++;
	
	if (i >= l)
		return (l);
		
	while (1)
	{
	    if (!s[i])
	    	return i;
	    	
		if (i)
		{
			if (IsWhiteSpaceChar (s[i-1]) && !IsWhiteSpaceChar (s[i]))
				return (i);
		}
		
		i++;
	}
}

BOOL STRING::SetFilePath (const char *path)
{
	if (!path)
		return FALSE;

	STRING tmp;
	if (tmp.LoadFileName (s,FALSE,TRUE,TRUE) != TRUE)
		return FALSE;

	STRING tmp1;
	if (tmp1.SetValidFileName (path,tmp) != TRUE)
		return FALSE;

	*this = (const char *) tmp1;
	return TRUE;
}

BOOL STRING::LoadFileName (const char *file_name, BOOL include_path, BOOL include_name, BOOL include_ext)
{
	int ipath=0, iname=-1, iext=-1;

	while (IsWhiteSpaceChar (file_name[ipath])) ipath++;

	if (!file_name[ipath]) return FALSE;

    int ilast_period = -1;
	int ilast_slash = -1;
    int j=0;
    while (file_name[j])
    {
        if (file_name[j] == '.')
            ilast_period = j;

		if (file_name[j] == '\\')
			ilast_slash = j;

        j++;
    }

	int i=ipath;
	int icolon = -1;
	int islash = -1;
	while (1)
	{
		if (file_name[i] == ':')	{ icolon = i;	i++;	continue; }
		if (file_name[i] == '\\')	{ islash = i;	i++;	continue; }

        if (file_name[i] == '.' && ilast_period >= 0 && i != ilast_period) 
        { 
            i++; 
            continue; 
        }

		if (!file_name[i] || file_name[i] == '.') 
		{ 

			if (icolon == -1 && islash == -1)
			{
				// There was not path spec
				iname = ipath;
				ipath = -1;
				break;
			}
			else
			{
				if (islash >= 0) { iname = islash+1; break;}
				if (icolon >= 0) { iname = icolon+1; break; }
			}

			ASSERT (FALSE);
		}

		i++;
	}

	if (ilast_slash != -1)
	{
		if (iname <= ilast_slash)
			iname = ilast_slash+1;

		if (i < iname)
			i = iname;
	}

	int iend_of_name;

	if (ilast_period > iname)
		iext = ilast_period;
	else
		iend_of_name = i;

	while (!IsWhiteSpaceChar (file_name[i]) && file_name[i]) i++;

	if (include_path == TRUE && include_name == TRUE && include_ext == TRUE)
	{
		return (Assign (&file_name[ipath],i-ipath));
	}

	if (include_path == TRUE && include_name == TRUE && include_ext == FALSE)
	{
		if (ipath == -1) ipath++;
		return (Assign (&file_name[ipath],iext-ipath));
	}

	if (include_path == TRUE && include_name == FALSE && include_ext == FALSE)
	{
		if (ipath == -1) { *this = ""; return TRUE; }
		return (Assign (&file_name[ipath],iname-ipath));
	}

	if (include_path == FALSE && include_name == TRUE && include_ext == TRUE)
	{
		return (Assign (&file_name[iname],i-iname));
	}

	if (include_path == FALSE && include_name == TRUE && include_ext == FALSE)
	{
		if (iext >= 0)
			return (Assign (&file_name[iname],iext-iname));
		else
			return (Assign (&file_name[iname],iend_of_name-iname));
	}

	if (include_path == FALSE && include_name == FALSE && include_ext == TRUE)
	{
		return (Assign (&file_name[iext],i-iext));
	}

	return FALSE;
}

BOOL STRING::AppendFILETIME (FILETIME &ft,BOOL bHumanReadable/*=FALSE*/)
{
	if (bHumanReadable)
	{
		SYSTEMTIME st;
		FileTimeToSystemTime (&ft,&st);
		char s0[32];
		sprintf_s (s0,sizeof(s),"%04d/%02d/%02d %02d:%02d:%02d",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
		*this += s0;
		return TRUE;
	}

	AppendHexValue (ft.dwHighDateTime);
	AppendHexValue (ft.dwLowDateTime);
	return TRUE;
}

void STRING::AppendHexValue (DWORD u4)
{
	char buffer[64];
	sprintf_s (buffer,sizeof(buffer),"%08lX",u4);
	(*this) += buffer;
}

void STRING::AppendHexValue (BYTE u1)
{
	char buffer[64];
	sprintf_s (buffer,sizeof(buffer),"%02X",(WORD)(u1));
	(*this) += buffer;
}

void STRING::AppendSeparatedHexValue (DWORD u4)
{
	char buffer[64];
	WORD u2_lower = (WORD)(u4);
	WORD u2_upper = (WORD)(u4>>16);
	sprintf_s (buffer,sizeof(buffer),"%04X-%04X",u2_upper,u2_lower);
	(*this) += buffer;
}

int STRING::GetSubDirCount (void)
{
	int i=0;
	int cnt = 0;
	while (s[i])
	{
		if (s[i++] == '\\') cnt++;
	}

	cnt--;
	return cnt;
}

BOOL STRING::AppendSubDir (const char *name)
{
	Trim (TRUE,FALSE,TRUE); // Trim leading and trailing spaces
	if (LastChar () != '\\')
	{
		*this += "\\";
		*this += name;
		return TRUE;
	}

	*this += name;

	return TRUE;
}

BOOL STRING::RemoveSubDir (STRING *removed_name/*=0*/)
{
	if (!s) return FALSE;

	int n = StrLen (s);
	if (n < 2) return FALSE;

	for (int i=n-2; i>=0; i--)
	{
		if (s[i] == '\\')
		{
			if (removed_name)
				*removed_name = (const char *)(&s[i+1]);

			Remove (i,n-i);
			return TRUE;
		}
	}

	return FALSE;
}

BOOL STRING::MakeValidFileName (void)
{
	int n = StrLen (s);

	if (n > 1024)
		return FALSE;

	for (int i=0; i<n; i++)
	{
		if (IsValidFileNameChar (s[i]) == FALSE)
			s[i] = '_';
	}

	return TRUE;
}


BOOL STRING::SetValidFileName (const char *path, const char *file_name)
{
	if (StrLen (path))
	{                        
		*this = path;


		Trim (TRUE,FALSE,TRUE);
		
		int n = len ();
		
		if (n)
		{
			if (s[n-1] != '\\')
				*this += "\\";
			
			*this += file_name;
		
			return TRUE;
		}
	}
	
	*this = file_name;
	return TRUE;
}

int STRING::PrevWord (int i)
{   
	int l = len ();
	
	if (!l) 
		return (0);
	
	if (!i)
		return (0); 
		
	if (i==1)
		return (0);
		
	i--;
		
	while (1)
	{
	    if (!i)
	    	return (0);
	    	
		if (s[i-1] == ' ' && s[i] != ' ')
			return (i);
		
		i--;
	}
}


//
// Does s[] include substring (case insensitive) s0[]?
//
BOOL STRING::includes (const char *s0)
{
    int l = len ();
    int l0 = strlen (s0);
    int j=0;
    char c,c0;

    if (l0 > l)
        return (0);

    for (int i=0; i<l; i++)
    {
		c  = s[i];
        c0 = s0[j];

        if (c >= 'a' && c <= 'z')
            c += 'A' - 'a';

        if (c0 >= 'a' && c0 <= 'z')
            c0 += 'A' - 'a';

        if (c == c0)
        {
            j++;
            if (!s0[j])
                return (1);
        }
        else
        {
            j=0;
            if (l-i < l0)
                return (0);
        }
    }
    return (0);
}

STRING& STRING::operator += (STRING &s0)
{
	*this += (const char *)s0;
	return (*this);
}

BOOL STRING::operator == (STRING &s0)
{
	return (IsEqualTo (s,s0,TRUE));
}						 

STRING::operator short ()
{
	return (atoi (s));
}

STRING::operator unsigned short ()
{   
	unsigned short n;
	if (sscanf_s (s,"%u",&n) != 1) return (0);
	return (n);
}

STRING::operator unsigned long ()
{
	unsigned long n;
	if (sscanf_s (s,"%ul",&n) != 1) return (0);
	return n;
}

STRING::operator long ()
{
	return (atol (s));
}

STRING::operator double ()
{
	return (atof (s));
}

STRING& STRING::operator += (short i)
{
	if (err) return (*this);
	
	char s_i[20];
	sprintf_s (s_i,sizeof(s_i),"%d",i);
	
	*this += s_i;    
	
	return (*this);
}

STRING& STRING::operator += (unsigned short i)
{
    if (err) return (*this);

    char s_i[20];
    sprintf_s (s_i,sizeof(s_i),"%u",i);

    *this += s_i;

    return (*this);
}

STRING& STRING::operator = (unsigned short i)
{
    if (err) return (*this);

    char s_i[20];
    sprintf_s (s_i,sizeof(s_i),"%u",i);

    *this = s_i;

    return (*this);
}

STRING& STRING::operator += (double v)
{
	if (err) return (*this);
	
	char s_v[50];
    DoubleToString (v,s_v,10);
	
	*this += s_v;
	
	return (*this);
}

STRING& STRING::operator += (float v)
{
	if (err) return (*this);
	
	char s_v[50];
    FloatToString (v,s_v,7);
	
	*this += s_v;
	
	return (*this);
}

STRING& STRING::operator = (double v)
{
	if (err) return (*this);
	
	char s_v[50];
    DoubleToString (v,s_v,10);
	
	*this = s_v;
	
	return (*this);
}

STRING& STRING::operator = (float v)
{
	if (err) return (*this);
	
	char s_v[50];
    FloatToString (v,s_v,7);
	
	*this = s_v;
	
	return (*this);
}

STRING& STRING::operator += (UINT i)
{
	if (err) return (*this);
	
	char s_i[20];
	
	if (sizeof i == 2)	
		sprintf_s (s_i,sizeof(s_i),"%u",i);
	else
	if (sizeof i == 4)
		sprintf_s (s_i,sizeof(s_i),"%lu",i);
		
	*this += s_i;
	
	return (*this);
}

STRING& STRING::operator = (UINT i)
{
	if (err) return (*this);
	
	char s_i[20];
	
	if (sizeof i == 2)	
		sprintf_s (s_i,sizeof(s_i),"%u",i);
	else
	if (sizeof i == 4)
		sprintf_s (s_i,sizeof(s_i),"%lu",i);
		
	*this = s_i;
	
	return (*this);
}

STRING& STRING::operator += (unsigned long l)
{
	if (err) return (*this);
	
	char s0[20];
	sprintf_s (s0,sizeof (s0),"%lu",l);
	
	*this += s0;    
	
	return (*this);
}

STRING& STRING::operator = (unsigned long l)
{
	if (err) return (*this);
	
	char s0[20];
	sprintf_s (s0,sizeof(s0),"%lu",l);
	
	*this = s0;    
	
	return (*this);
}

STRING& STRING::operator += (long l)
{
	if (err) return (*this);
	
	char s0[20];
	sprintf_s (s0,sizeof(s0),"%ld",l);
	
	*this += s0;    
	
	return (*this);
}

STRING& STRING::operator = (long l)
{
	if (err) return (*this);
	
	char s0[20];
	sprintf_s (s0,sizeof(s0),"%ld",l);
	
	*this = s0;    
	
	return (*this);
}

STRING& STRING::operator = (__int64 i64)
{
	if (err) return (*this);

	char s0[64];
	sprintf_s (s0,sizeof(s0),"%I64d",i64);

	*this = s0;

	return (*this);
}

STRING& STRING::operator += (__int64 i64)
{
	if (err) return (*this);

	char s0[64];
	sprintf_s (s0,sizeof (s0),"%I64d",i64);

	*this += s0;

	return (*this);
}

BOOL STRING::AppendMax (const char *s0, int n_max)
{
	if (n_max <= StrLen (s0))
		return Append (s0);

	if (!s0)
		return FALSE;

	if (err)
		return FALSE;

	int len1 = n_max;
	int len0 = StrLen (s);

	char *s_old = s;
	
	s = new char[len0+len1+1];

	ASSERT (s);

	MemMove (s,s_old,len0);
	MemMove (s+len0,s0,len1);

	s[len0+len1] = 0;
	
	delete s_old;

	return TRUE;


}

BOOL STRING::Append (const char *s0)
{
	if (!s0)
		return FALSE;

	if (err)
		return FALSE;

	int len1 = StrLen (s0);
	int len0 = StrLen (s);

	char *s_old = s;
	
	s = new char[len0+len1+1];

	ASSERT (s);

	MemMove (s,s_old,len0);
	MemMove (s+len0,s0,len1+1);
	
	delete s_old;

	return TRUE;
}

STRING& STRING::operator += (const char *s0)
{
	Append (s0);
	return (*this);
/*
    if (!s0) return (*this);
	if (err) return (*this);
	
	int newlen = StrLen (s0);
	newlen += StrLen (s);

	char *s_old = s;
	
	s = new char[newlen+1];
	
//	UPDATE_STRING_LEAK_DETECTION();
	ASSERT (s);

	StrCpy (s,s_old);
	strcat (s,s0);
	
	delete s_old;
	
	return (*this);*/
}

BOOL STRING::Assign (const char *c0, const char *c1/*=0*/, const char *c2/*=0*/, const char *c3/*=0*/, const char *c4/*=0*/, const char *c5/*=0*/, const char *c6/*=0*/, const char *c7/*=0*/)
{
	if (!c0)
	{
		*this = "";
		return TRUE;
	}

	long len = 0;
	while (1)
	{
		len += StrLen (c0);
		if (!c1) break;
		len += StrLen (c1);
		if (!c2) break;
		len += StrLen (c2);
		if (!c3) break;
		len += StrLen (c3);
		if (!c4) break;
		len += StrLen (c4);
		if (!c5) break;
		len += StrLen (c5);
		if (!c6) break;
		len += StrLen (c6);
		if (!c7) break;
		len += StrLen (c7);
		break;
	}

	char *_s = (char *) _alloca (len+1);

	long i=0;
	while (1)
	{
		StrCpy (_s+i,c0);
		i += StrLen (c0);
		if (!c1) break;
		StrCpy (_s+i,c1);
		i += StrLen (c1);
		if (!c2) break;
		StrCpy (_s+i,c2);
		i += StrLen (c2);
		if (!c3) break;
		StrCpy (_s+i,c3);
		i += StrLen (c3);
		if (!c4) break;
		StrCpy (_s+i,c4);
		i += StrLen (c4);
		if (!c5) break;
		StrCpy (_s+i,c5);
		i += StrLen (c5);
		if (!c6) break;
		StrCpy (_s+i,c6);
		i += StrLen (c6);
		if (!c7) break;
		StrCpy (_s+i,c7);
		i += StrLen (c7);
		break;
	}

	*this = _s;

	return TRUE;
}

BOOL STRING::Assign (const char *s0, int n)
{
	if (s0 == 0)
		return FALSE;

    if (n <= 0)
        return FALSE;

	delete s;

	int i=0;
	while (i < n)
	{
		i++;
		if (s0[i] == 0)
			break;
	}

	if (i < n)
		n = i;

	s = new char[n+1];

//	UPDATE_STRING_LEAK_DETECTION();	
	ASSERT (s);
	
	MemMove (s,s0,n+1);
	s[n] = 0;
	
	return TRUE;
}
	

void STRING::clear (void)
{
	delete (s);

    s = new char[1];

//	UPDATE_STRING_LEAK_DETECTION();	

    if (!s)
        err=1;

    s[0]=0;
}

STRING &STRING::operator = (short i)
{
	if (err) return (*this);
	
	char s_i[20];
	sprintf_s (s_i,sizeof(s_i),"%d",i);
	
	*this = s_i;   
	
	return (*this);
}

STRING::STRING (void)
{
	UPDATE_LEAK_DETECTION ();

	s = new char[1];

	s[0] = 0;
    err=0;
}

BOOL STRING::Error (void)
{
	if (err) return TRUE;
	return FALSE;
}

STRING::STRING(const wchar_t *ws)
{
	UPDATE_LEAK_DETECTION();

	s = 0;
	err = 0;

	int len = 0;
	const int maxlen = 100000;
	while (1)
	{
		if (ws[len] == 0)
		{
			len++;
			break;
		}

		len++;
		if (len == maxlen)
			break;
	}

	s = new char[len + 1];

	if (!s)
	{
		err = 1;
		return;
	}

	for (int i = 0; i < len; i++)
	{
		s[i] = (char)ws[i];
	}

	s[len] = 0;
}

STRING::STRING (const STRING &s_s0)
{
	UPDATE_LEAK_DETECTION ();

    s=0;
    err=0;
	  
	const char *s1 = (const char *) s_s0.GetStringConst ();
    int len = strlen (s1);
    
    s = new char[len+1];

    if (!s)
    {
        err=1;
        return;
    }
    
    strcpy_s (s,len+1,s1);
}

//STRING::STRING (STRING &s_s0)
//{
//	UPDATE_LEAK_DETECTION ();
//
//    s=0;
//    err=0;
//	  
//	const char *s1 = (const char *) s_s0.GetStringConst ();
//    int len = strlen (s1);
//    
//    s = new char[len+1];
//
//    if (!s)
//    {
//        err=1;
//        return;
//    }
//    
//    strcpy_s (s,len+1,s1);
//}

STRING::STRING (const char *s0)
{
	UPDATE_LEAK_DETECTION ();

    s=0;
    err=0;
    
	if (!s0)
	{
		s = new char[1];
				
		s[0]=0;
		return;
	}

    int len = strlen (s0);
    
    s = new char[len+1];

    if (!s)
    {
        err=1;
        return;
    }
    
    strcpy_s (s,len+1,s0);
}

STRING::STRING (unsigned long u4)
{
	UPDATE_LEAK_DETECTION ();

	s = new char[1];
	
	s[0] = 0;
    err=0;
	*this = u4;
}

STRING::STRING (unsigned short u2)
{
	UPDATE_LEAK_DETECTION ();

	s = new char[1];
	
	s[0] = 0;
    err=0;
	*this = u2;
}

STRING::STRING (long i4)
{
	UPDATE_LEAK_DETECTION ();

	s = new char[1];

	s[0] = 0;
    err=0;
	*this = i4;
}

STRING::STRING (double v)
{
	UPDATE_LEAK_DETECTION ();

	s = new char[1];

	s[0] = 0;
    err=0;
	*this = v;
}

STRING::~STRING (void)
{
	if (m_ring_buf)
	{
		for (int i=0; i<m_ring_buf_sz; i++)
		{
			if (m_ring_buf[i] == this)
			{
				m_ring_buf[i] = 0;
				break;
			}
		}
	}

	delete s;
	s = 0;
}

BOOL STRING::Overwrite (int i, const char *c, int width)
{   
    int org_len = len ();
	int j;
	
	if (org_len < i + width)
	{
		if (!c) return TRUE;
		
		int idx_of_last_non_space=0;
		j=0;
		
		while (c[j])
		{
			if (c[j] != ' ') 
				idx_of_last_non_space = j;
				
			j++;
		}
		
		if (idx_of_last_non_space+i < org_len)
		{ 	// No need to reallocate
			width = org_len - i;
		}
		else
		{
			int alloc_size = idx_of_last_non_space+2+i;
			char *tmp = new char[alloc_size];
			ASSERT (tmp);
			
			strcpy_s (tmp,alloc_size,s);
			
			delete s; s = tmp;
			
			// Pad with spaces
			for (j=org_len; j<alloc_size; j++)
				s[j] = ' ';
			        
			// Make sure that s[] is null terminated
			s[alloc_size-1] = 0;
			width = min (width,idx_of_last_non_space+1);
		}
	}
	
	int n=0;
	char c0; 
	j=0;
	
	while (n < width)
	{   
		if (c)
			c0 = c[j];
		else
			c0 = ' ';
		
		if (c0) 
			j++;
		else 
			c0 = ' ';
			
		s[i] = c0;
		
		i++; n++;
	}
	
	return TRUE;
}

char STRING::LastChar (void)
{
	if (!s) return 0;
	int n = StrLen (s);
	if (!n) return 0;
	return s[n-1];
}


BOOL STRING::Insert (int i, const char *c, int width/*=0*/)
{   
	if (!c)
		return TRUE;

	if (width == 0)
		width = StrLen (c);
		
    int org_len = len ();
    
	int idx_of_last_non_space=-1;
	int j=0;
	
	int nalloc;
		
	if (i >= org_len)
	{
		while (c[j])
		{
			if (c[j] != ' ') 
				idx_of_last_non_space = j;
				
			j++;
		}
		
		if (idx_of_last_non_space < 0)
			return TRUE;
		
		width = min (idx_of_last_non_space+1,width);
		
		nalloc = org_len + (i-org_len) + width + 1;
	}
	else
		nalloc = org_len + width + 1;
	
	if (!width)
		return TRUE;
		
	char *tmp = new char[nalloc];
	ASSERT (tmp);
	
	strcpy_s (tmp,nalloc,s);
	delete s; s = tmp;
	
	if (i >= org_len)
	{      
		// Fill with spaces from the end of the old line to the beginning of the insert section
		for (j=org_len; j<i; j++)
			s[j] = ' ';
	}
	else
	{	
		// Shift the contents of the original string forward to make room for the insert		
		for (j=org_len+width-1; j>=i; j--)
		{
			if (j-width < 0)
				break;

			s[j] = s[j-width];
		}
	}
    
    j=0;
    int n=0;
    char c0;
	while (n < width)
	{
		c0 = c[j];
		
		if (c0)
			j++;
		else
			c0 = ' ';
			
		s[i++] = c0;
		n++;
	}
	
	s[nalloc-1] = 0;
	
	return TRUE;
}
	
BOOL STRING::Remove (int i, int width)
{         
	int org_len = len ();
	
	if (i<0) return FALSE;
	if (i >= org_len) return TRUE;
	
	if (i+width >= org_len)
	{
		s[i] = 0;
		
		char *tmp = new char[i+1];
		ASSERT (tmp);
		
		strcpy_s (tmp,i+1,s);
		delete s; s=tmp;
		
		return TRUE;
	}               
	           
	int j=i+width;
	
	while (1)
	{
		s[i] = s[j];
		
		if (!s[j])
			break;
			
		i++; j++;
	}
		
	return TRUE;
}	

BOOL STRING::ReplaceAfterLast(const char *s_find, const char *replace_with)
{
	if (s_find == 0)
		return FALSE;

	if (replace_with == 0)
		return FALSE;

	int len = StrLen(s);

	int len_find = StrLen(s_find);

	int i = len - len_find;

	while (i >= 0)
	{
		if (IsEqualTo(&s[i], s_find, TRUE, len_find) == TRUE)
		{
			Remove(i + len_find, len - (i + len_find));
			return Append(replace_with);
		}

		i--;
	}

	Append(s_find);
	return Append(replace_with);
}


int STRING::Replace (const char *target, const char *replace_with, int max_num/*=0x7FFF*/)
{
	if (target == 0 || StrLen (target) == 0)
		return 0;

	int i = 0;
	int n = 0;
	
	ASSERT (max_num >= 0);

	BOOL same_size = FALSE;
	if (StrLen (target) == StrLen (replace_with))
		same_size = TRUE;

	while (1)
	{
		const char *c = StrStr (&s[i],target,TRUE);
		if (!c) 
			return n;

		if (same_size)
		{
			i = c-s;
			const char *c0 = replace_with;
			while (*c0)
			{
				s[i++] = *c0;
				c0++;
			}
			n++;
		}
		else
		{
			int j = c-s;
			STRING *s0 = Cut (j,StrLen (target));
			Insert (j,replace_with,StrLen (replace_with));
			i = j+StrLen(replace_with);
			delete s0;
			n++;
		}

		if (n >= max_num)
			return n;
	}

	return n;
}
	

STRING *STRING::Cut (int i, int width /* = -1 */)
{        
	STRING *ret = new (STRING);
	ASSERT (ret);
	
	int org_len = len ();
	
	if (i >= org_len) 
		return ret;
		
	if (width < 0 || width+i >= org_len)
	{
		*ret = &s[i];
		s[i] = 0;
		
		char *tmp = new char[i+1];
		ASSERT (tmp);
		
		strcpy_s (tmp,i+1,s);
		
		delete s; s = tmp;
		
		return ret;
	}
	
	char c = s[width+i];
	
	s[width+i] = 0;
	
	*ret = &s[i];
	
	s[width+i] = c;
	
	int j=width+i;
	
	while (1)
	{
		s[i] = s[j];
		
		if (!s[i]) 
			break;
			
		i++; j++;
	}
	
	char *tmp = new char[i+1];
	ASSERT (tmp);
	
	strcpy_s (tmp,i+1,s);
	
	delete s;
	s = tmp;
	
	return ret;
}


inline BOOL decimal_point_exists (const char *s_num)
{
	int i=0;

	while (s_num[i])
	{
		if (s_num[i] == '.') 
			return TRUE;

		i++;
	}

	return FALSE;
}

inline void eliminate_redundant_characters_from_number (char *s_num)
{
	BOOL exponent_found = FALSE;

	// if last character is a decimal point, add 1 trailing zero
	{
		int n = StrLen (s_num);
		if (s_num[n-1] == '.')
		{
			s_num[n] = '0';
			s_num[n] = 0;
		}
	}

	// Eliminate leading zeros in the exponent
	int i=1;
	while (1)
	{
		if (s_num[i] == 'e' || s_num[i] == 'E')
		{
			exponent_found = TRUE;
			i++;
			if (s_num[i] == '-') i++; else
			if (s_num[i] == '+') i++;
			int i0=i;
			while (s_num[i] == '0') i++;
			while (s_num[i])
			{
				s_num[i0++] = s_num[i++];
			}
			s_num[i0] = 0;

			break;
		}
		i++;
		if (!s_num[i]) break;
	}

	// Eliminate decimal point if possible
	i=1;
	while (s_num[i])
	{
		if (s_num[i] == 'e' || s_num[i] == 'E')
		{
			if (s_num[i-1] == '.')
			{
				while (s_num[i])
				{
					s_num[i-1] = s_num[i];
					i++;
				}
				s_num[i-1] = 0;
				break;
			}
		}
		i++;
	}

	// Eliminate trailing zeros
	if (!exponent_found && decimal_point_exists (s_num))
	{
		int len = StrLen (s_num);
		while (s_num[len-1] == '0')
		{
			if (!len) break;
			len--;
			if (s_num[len-1] != '.')
				s_num[len] = 0;
		}
	}
}

BOOL DoubleToString (double d, char *s_ret, short max_len)
{
    if (max_len < 10) return FALSE;

    if (!d) 
	{ 
		s_ret[0] = '0'; 
		s_ret[1] = 0; 
		return TRUE; 
	}

	char *s = (char *) alloca (max_len+10);
	ASSERT (s);

    if (!d) 
	{ 
		s_ret[0] = '0'; 
		s_ret[1] = 0; 
		return TRUE; 
	}

    while (1)
    {
        if (d < 0.0F)
        {
            if (d < -1.0e6)
                break;

            if (d > -1.0e-6)
                break;

            if (d == (long) d) // Integer valued
            {
                sprintf_s (s,max_len+10,"%ld",(long)d);
				StrMCpy (s_ret,s,max_len+1);
                return TRUE;
            }

			short sig_dig = max_len-1;

			_gcvt_s (s, max_len+10, (double)d, sig_dig);
			eliminate_redundant_characters_from_number (s);
			if (StrLen (s) > max_len)
			{
				sig_dig -= StrLen (s)-max_len;
				if (sig_dig < 1)
				{
					StrMCpy (s_ret,s,max_len+1);
					return FALSE;
				}

				if (sig_dig < 6)
					sig_dig = 6;

				while (1)
				{
					_gcvt_s(s,max_len+10,(double)d, sig_dig);
					eliminate_redundant_characters_from_number (s);

					if (StrLen (s) > max_len)
					{
						sig_dig--;
						continue;
					}

					break;
				}

				ASSERT (StrLen (s) <= max_len);
			}

			StrMCpy (s_ret,s,max_len+1);
			return TRUE;
        }

        if (d > 1.0e6)
            break;

        if (d < 1.0e-6)
            break;

        if (d == (long) d) // Integer valued
        {
            sprintf_s (s,max_len+1,"%ld",(long)d);
			StrMCpy (s_ret,s,max_len+1);
            return TRUE;
        }

		short sig_dig = max_len-1;

		_gcvt_s(s,max_len+10,(double)d, sig_dig);
		eliminate_redundant_characters_from_number (s);
		if (StrLen (s) > max_len)
		{
			sig_dig -= StrLen (s)-max_len;
			if (sig_dig < 1)
			{
				StrMCpy (s_ret,s,max_len+1);
				return FALSE;
			}

			while (1)
			{
				_gcvt_s(s,max_len+10,(double)d, sig_dig);
				eliminate_redundant_characters_from_number (s);

				if (StrLen (s) > max_len)
				{
					sig_dig--;
					continue;
				}

				break;
			}

			ASSERT (StrLen (s) <= max_len);
		}

		StrMCpy (s_ret,s,max_len+1);
		return TRUE;
   }

    BOOL iret = EngineeringNotation ( d, s, max_len, min (max_len-8,12)  );

	StrMCpy (s_ret,s,max_len+1);

    return iret;
}

BOOL FloatToString (float d, char *s_ret, short max_len, short sig_digits  /* = 6 */, BOOL fixed_decimal /* = FALSE */)
{
    if (max_len < 7) return FALSE;

	char *s = (char *) alloca (2*max_len+1);
	ASSERT (s);

	if (fixed_decimal)
	{
		double max_abs_value = pow ((double)10,(double)max_len);
		
		if (fabs (d) >= max_abs_value)
			return FALSE;

		if (fabs (d) <= 1.0/max_abs_value)
		{	
			s_ret[0] = '0';
			s_ret[1] = '.';
			MemSet (&s_ret[2],'0',sig_digits);
			s_ret[2+sig_digits] = 0;
			return TRUE;
		}

		STRING fmt = "%-";
		fmt += max_len;
		fmt += ".";
		fmt += sig_digits;
		fmt += "f";
		sprintf_s (s_ret,2*max_len+1,fmt,d);

		s_ret[max_len] = 0;

		if (s_ret[max_len-1] == '.')
			s_ret[max_len-1] =0;


		return TRUE;
	}

    if (!d) 
	{ 
		s_ret[0] = '0'; 
		s_ret[1] = 0; 
		return TRUE; 
	}

    while (1)
    {
        if (d < 0.0F)
        {
            if (d < -1.0e3F)
                break;

            if (d > -1.0e-3F)
                break;

            if (d == (long) d) // Integer valued
            {
                sprintf_s (s,2*max_len+1,"%ld",(long)d);
				StrMCpy (s_ret,s,max_len+1);
                return TRUE;
            }

			short sig_dig;
			if (sig_digits) sig_dig = sig_digits;
			else sig_dig = max_len-1;

			_gcvt_s(s,2*max_len+1,(double)d, sig_dig);
			eliminate_redundant_characters_from_number (s);
			if (StrLen (s) > max_len)
			{
				sig_dig -= StrLen (s)-max_len;
				if (sig_dig < 1)
				{
					StrMCpy (s_ret,s,max_len+1);
					return FALSE;
				}

				while (1)
				{
					_gcvt_s(s, 2*max_len+1, (double)d, sig_dig);
					eliminate_redundant_characters_from_number (s);

					if (StrLen (s) > max_len)
					{
						sig_dig--;
						continue;
					}

					break;
				}

				ASSERT (StrLen (s) <= max_len);
			}

			StrMCpy (s_ret,s,max_len+1);
			return TRUE;
        }

        if (d > 1.0e3F)
            break;

        if (d < 1.0e-3F)
            break;

        if (d == (long) d) // Integer valued
        {
            sprintf_s (s,2*max_len+1,"%ld",(long)d);
			StrMCpy (s_ret,s,max_len+1);
            return TRUE;
        }

		short sig_dig;
		if (sig_digits) sig_dig = sig_digits;
		else sig_dig = max_len-1;

		_gcvt_s(s,2*max_len+1,(double)d, sig_dig );
		eliminate_redundant_characters_from_number (s);
		if (StrLen (s) > max_len)
		{
			sig_dig -= StrLen (s)-max_len;
			if (sig_dig < 1)
			{
				StrMCpy (s_ret,s,max_len+1);
				return FALSE;
			}

			while (1)
			{
				_gcvt_s(s,2*max_len+1,(double)d, sig_dig);
				eliminate_redundant_characters_from_number (s);

				if (StrLen (s) > max_len)
				{
					sig_dig--;
					continue;
				}

				break;
			}

			ASSERT (StrLen (s) <= max_len);
		}

		StrMCpy (s_ret,s,max_len+1);
		return TRUE;
   }

    BOOL iret = EngineeringNotation ((double) d, s, max_len, min (max_len-5,7)  );

	StrMCpy (s_ret,s,max_len+1);

	return iret;
}

inline void __fail_proof_double_to_string 
(
	char sign,
	double input_variable,
	char *output_var_str,
	short max_output_str_len,
	short sig_digits
)
{
	char s_tmp[128];

	_gcvt_s(s_tmp,sizeof(s_tmp),input_variable, max(sig_digits,(short)10));

	eliminate_redundant_characters_from_number (s_tmp);

	if ((short)strlen(s_tmp) <= max_output_str_len)
	{
		memmove(output_var_str, s_tmp, strlen(s_tmp)+1);
		return;
	}
	
	// look for the exponent
	int i=0;
	int iexp=-1;
	while (s_tmp[i] != 'e' && s_tmp[i] != 'E' && s_tmp[i]) i++;

	if (s_tmp[i])
		iexp = i;

	// no exponent, okay to cut off the number
	strncpy_s (output_var_str,max_output_str_len+1,s_tmp,min (StrLen (s_tmp)+1,(int)max_output_str_len));

	if (iexp != -1) 
	{
		// exponent exists
		int exp_slen = StrLen (&s_tmp[iexp]);
		i = max_output_str_len - exp_slen;
		if (i > 2) // Add the exponent to the end if there is room
		{
			strcpy_s (&output_var_str[i],max_output_str_len+1-i,&s_tmp[iexp]);
			iexp = i;
		}
	}

	if (sign == '-')
	{
		if (iexp == -1)
		{
			for (i=max_output_str_len-1; i>0; i--)
				output_var_str[i] = output_var_str[i-1];
		}
		else
		{
			for (i=iexp-1; i>0; i--)
			{
				output_var_str[i] = output_var_str[i-1];
			}
		}

		output_var_str[0] = '-';
	}

	output_var_str[max_output_str_len] = 0;	
}

BOOL EngineeringNotation
(
	double input_variable,
	char *output_var_str,
	short max_output_str_len,
	short sig_digits
)
{
	char sign=0;
	short i,j;

	for (i=0;i<max_output_str_len;i++)
		output_var_str[i] = 0;

	// Determine the sign of the input variable.
	if (input_variable < 0.0)
	{
		input_variable = -input_variable;
		sign = '-';
	}
	else
		sign = '+';

	double mantessa,exponent;
	short iexponent;
	_exception _ex;

	short junk=0;

	if (input_variable == 0.0)
	{
		mantessa = 0.0;
		iexponent = 0;
	}
	else
	{
		double lg = log10 (input_variable);
		if (!_matherr (&_ex))
		{
			__fail_proof_double_to_string (sign,input_variable,output_var_str,max_output_str_len,sig_digits);
			return FALSE;
		}

		double tmp = modf (lg,&exponent);
		if (!_matherr (&_ex))
		{
			__fail_proof_double_to_string (sign,input_variable,output_var_str,max_output_str_len,sig_digits);
			return FALSE;
		}

		mantessa = pow (10.0,tmp);
		if (!_matherr (&_ex))
		{
			__fail_proof_double_to_string (sign,input_variable,output_var_str,max_output_str_len,sig_digits);
			return FALSE;
		}

		iexponent = (short) exponent;

		while (mantessa < 1.0)
		{
			if (iexponent < -100 || iexponent > 100)
			{
				__fail_proof_double_to_string (sign,input_variable,output_var_str,max_output_str_len,sig_digits);
				return FALSE;
			}

			mantessa *= 10;
			iexponent--;
		}
	}

	short decimal_location=1;

	while (abs (iexponent)%3)
	{
		iexponent--;
		decimal_location++;
	}

	int junk0=0,junk1=0;
	char s_number[128];
	_fcvt_s (s_number,sizeof(s_number),mantessa,sig_digits,&junk0,&junk1);

	i=0;
	if (i<max_output_str_len)
		output_var_str[i++] = sign;

	j=0;
	while (i<max_output_str_len && j < decimal_location)
	{
		output_var_str[i++] = s_number[j++];
	}

	if (i<max_output_str_len)
		output_var_str[i++] = '.';

	while (i<max_output_str_len && j < sig_digits)
		output_var_str[i++] = s_number[j++];

	char s_exponent[10];
	sprintf_s (s_exponent,sizeof(s_exponent),"%+03d",iexponent);

	if (i<max_output_str_len)
		output_var_str[i++] = 'E';

	j=0;
	while (i<max_output_str_len && s_exponent[j])
		output_var_str[i++] = s_exponent[j++];

	eliminate_redundant_characters_from_number (output_var_str);

	if (i==max_output_str_len)
		return (FALSE); // Error

	return (TRUE);
}

// [WS]HHHHHHHHHHHHHHHH[WS][NULL] - 64 bit (8 byte) hex
BOOL ValidateIsU8Hex (const char *data, __int64 *u8/*=0*/)
{
	long i = 0;
	
	while (IsWhiteSpaceChar (data[i])) 
		i++;
	
	data += i;
	i = 0;
	
	while (IsHexChar (data[i])) 
		i++; 

	if (IsWhiteSpaceChar (data[i]) == FALSE && data[i]) 
		return FALSE;

	if (i < 1) 
		return FALSE;

	if (i > 16)
		return FALSE;

	if (u8)
	{
		if (sscanf_s (data,"%I64X",u8) != 1)
			return FALSE;
	}

	return TRUE; 
}

// [WS]HHHH-HHHH[WS][NULL] 
BOOL ValidateIsU4Hex (const char *data, DWORD *u4/*=0*/)
{
	long i = 0;
	
	while (IsWhiteSpaceChar (data[i])) 
		i++;
	
	data += i;
	i = 0;
	
	while (IsHexChar (data[i])) 
		i++; 

	if (IsWhiteSpaceChar (data[i]) == FALSE && data[i]) 
		return FALSE;

	if (i > 8)
		return FALSE;

	if (i < 1) 
		return FALSE;


	if (u4)
	{
		if (sscanf_s (data,"%lX",u4) != 1)
			return FALSE;
	}

	return TRUE; 
}

// [WS]HHHH-HHHH[WS][NULL] 
BOOL ValidateIsSeparatedU4Hex (const char *data, DWORD *u4/*=0*/)
{
	long i = 0;
	
	while (IsWhiteSpaceChar (data[i])) 
		i++;
	
	data += i;
	i = 0;
	
	while (IsHexChar (data[i])) 
		i++; 

	if (data[i] != '-') 
		return FALSE;

	if (i != 4)
		return FALSE;

	i++;

	while (IsHexChar (data[i])) 
		i++; 


	if (i != 9)
		return FALSE;

	if (!IsWhiteSpaceChar (data[i]) && data[i])
		return FALSE;

	if (u4)
	{
		char tmp[9];
		MemMove (&tmp[0],&data[0],4);
		MemMove (&tmp[4],&data[5],4);
		tmp[8] = 0;

		if (sscanf_s (tmp,"%lX",u4) != 1)
			return FALSE;
	}

	return TRUE; 
}

#ifdef __AFX_H__

#include "intrin.h"

STRING::STRING (CString &s_s0)
{
	UPDATE_LEAK_DETECTION ();

    s=0;
    err=0;
    
	const char *s1 = (const char *) s_s0;
    short len = strlen (s1);
    
    s = new char[len+1];
    if (!s)
    {
		__debugbreak();
        err=1;
        return;
    }
    
    strcpy_s (s,len+1,s1);
}

BOOL LoadFileName (CString &target, const char *source, BOOL path, BOOL name, BOOL ext)
{
	STRING s;
	if (s.LoadFileName (source,path,name,ext) == FALSE)
		return FALSE;

	target = (const char *) s;

	return TRUE;
}
#endif

BOOL STRING::InsertAtDelimitedPosition (const char *v, int zero_based_idx, char delim_char /* = '/t' */)
{
	char sdelim[2] = { 0, 0 };
	sdelim[0] = delim_char;

	const char *c0 = this->s;

	for (int i=0; i<zero_based_idx-1; i++)
	{
		c0 = StrStr (c0,sdelim);
		if (!c0)
			return FALSE;

		c0++;
	}

	const char *c1 = StrStr (c0+1,sdelim);
	if (!c1) // Append at the end of the line
	{
		*this += sdelim;
		*this += v;
		return TRUE;
	}


	// Need to insert into the middle of the line
	c1++;
	c0 = c1;

	c1 = StrStr (c0,sdelim);
	if (!c1)
		c1 = this->s + strlen (this->s);

	STRING *end_part = Cut (c0-this->s);
	*this += v;

	const char *c = *end_part;
	c += c1-c0;

	*this += c;

	delete end_part;

	return TRUE;
}

BOOL STRING::InsertAtDelimitedPosition (float  v, int zero_based_idx, char delim_char /* = '/t' */)
{
	STRING sv = v;
	return InsertAtDelimitedPosition ((const char *)sv,zero_based_idx,delim_char);
}

BOOL STRING::InsertAtDelimitedPosition (double v, int zero_based_idx, char delim_char /* = '/t' */)
{
	STRING sv = v;
	return InsertAtDelimitedPosition ((const char *)sv,zero_based_idx,delim_char);
}

BOOL STRING::InsertAtDelimitedPosition (long   v, int zero_based_idx, char delim_char /* = '/t' */)
{
	STRING sv = v;
	return InsertAtDelimitedPosition ((const char *)sv,zero_based_idx,delim_char);
}

BOOL STRING::MakeIntoAbsolutePath (void)
{
	long i=0;
	long previous_sub_dir_idx = -1;

	while (s[i])
	{
		if (IsEqualTo (&s[i],"\\..\\",TRUE,4))
		{
			if (previous_sub_dir_idx != -1)
			{
				Remove ((int)previous_sub_dir_idx,(int)(i-previous_sub_dir_idx+3));
				i -= (i-previous_sub_dir_idx);
				previous_sub_dir_idx = i;
				continue;
			}
		}

		if (s[i] == '\\')
		{
			if (i==0 || s[i-1] != '\\')
				previous_sub_dir_idx = i;

			i++;
		}

		i++;
	}

	return TRUE;
}

inline const char *GetTabDelimited_double_01 (const char *c, double &v, BOOL &success)
{
	success = TRUE;

	while (IsWhiteSpaceChar (*c) && *c != '\t') c++;

	const char *c0 = c;

	while (1)
	{
		if (*c == '\t' || *c == 0)
			break;

		if (IsDigit (*c) == FALSE && IsSign (*c) == FALSE && *c != '.' && *c != 'E' && *c != 'e')
		{
			success = FALSE;
		}

		c++;
	}

	if (success == TRUE)
	{
		if (sscanf_s (c0,"%lf",&v) != 1)
			success = FALSE;
	}

	if (*c == 0)
		return 0;

	return c+1;
}

double STRING::ComputeMean (void)
{
	// Count the tabs
	int count = Replace ("\t","\t");

	count++;

	double *v = (double *)alloca (sizeof(double)*count);
	if (!v)
		return 0;

	MemSet (v,0,sizeof(double)*count);

	long n = 0;
	const char *c = s;
	BOOL success;
	long i;
	for (i=0; i<count; i++)
	{
		c = GetTabDelimited_double_01 (c,v[n],success);
		if (success)
			n++;

		if (!c)
			break;
	}

	if (!n)
		return 0;

	double sum = 0;
	for (i=0; i<n; i++)
	{
		sum = sum + v[i];
	}

	return (sum/n);
}


double STRING::ComputeStDev (void)
{
	// Count the tabs
	int count = Replace ("\t","\t");

	count++;

	double *v = (double *)alloca (sizeof(double)*count);
	if (!v)
		return 0;

	MemSet (v,0,sizeof(double)*count);

	long n = 0;
	const char *c = s;
	BOOL success;
	long i;
	for (i=0; i<count; i++)
	{
		c = GetTabDelimited_double_01 (c,v[n],success);
		if (success)
			n++;

		if (!c)
			break;
	}

	if (n < 2)
		return 0;

	double sum = 0;
	double sq_sum = 0;
	for (i=0; i<n; i++)
	{
		sum += v[i];
		sq_sum += v[i] * v[i];
	}

	double stdev = sqrt ((sq_sum - sum*sum/n) / (n-1));
	return (stdev);
}

double STRING::ComputeMax (void)
{
	// Count the tabs
	int count = Replace ("\t","\t");

	count++;

	double *v = (double *)alloca (sizeof(double)*count);
	if (!v)
		return 0;

	MemSet (v,0,sizeof(double)*count);

	long n = 0;
	const char *c = s;
	BOOL success;
	long i;
	for (i=0; i<count; i++)
	{
		c = GetTabDelimited_double_01 (c,v[n],success);
		if (success)
			n++;

		if (!c)
			break;
	}

	if (!n)
		return 0;

	double fmax = -1e308;
	for (i=0; i<n; i++)
	{
		fmax = max (v[i],fmax);
	}

	return (fmax);
}

double STRING::ComputeMin (void)
{
	// Count the tabs
	int count = Replace ("\t","\t");

	count++;

	double *v = (double *)alloca (sizeof(double)*count);
	if (!v)
		return 0;

	MemSet (v,0,sizeof(double)*count);

	long n = 0;
	const char *c = s;
	BOOL success;
	
	long i;
	for (i=0; i<count; i++)
	{
		c = GetTabDelimited_double_01 (c,v[n],success);
		if (success)
			n++;

		if (!c)
			break;
	}

	if (!n)
		return 0;

	double fmin = +1e308;
	for (i=0; i<n; i++)
	{
		fmin = min (v[i],fmin);
	}

	return (fmin);
}

bool STRING::AssignDelimitedItem(const char *s, int item, char delimiter)
{
	// Count the number of items
	int i = 0;
	int ndelimeters = 0;
	int quote = 0;

	while (1)
	{
		if (s[i] == 0)
			break;

		if (s[i] == '\"')
			quote ? quote = 1 : quote = 0;

		if (quote)
		{
			i++;
			continue;
		}

		if (s[i] == ' ')
		{
			i++;
			continue;
		}

		if (s[i] == delimiter)
		{
			ndelimeters++;
			i++;
			continue;
		}

		i++;
	}

	if (ndelimeters < item)
		return false;

	int is = 0;
	i = 0;
	int nitems = 0;

	while (s[i])
	{
		// Process each item
		int istart = -1, iend = -1;

		// Trim leading spaces
		while (s[i] == ' ') i++;

		if (s[i] == '\"') // starting quote found
		{
			i++;
			istart = i;
			while (s[i] && s[i] != '\"') i++; // find ending quote
			if (s[i] == 0) break; // no matching quote found
			iend = i - 1;

			if (nitems == ndelimeters + 1) break; // this should not happen

			if (nitems == item)
			{
				this->Insert(0, &s[istart], iend - istart + 1);
				return true;
			}
			nitems++;
			i++;
			continue; // finished processing quoted item
		}

		// start non quoted item
		istart = i;
		while (s[i] && s[i] != delimiter) i++;
		iend = i - 1;

		if (nitems == ndelimeters + 1) break; // this should not happen

		if (nitems == item)
		{
			this->Insert(0, &s[istart], iend - istart + 1);
			return true;
		}

		nitems++;

		if (s[i] == 0)
			break;

		i++;
	}

	return false;
}

int STRING::GetArrayOfDelimitedItemsEx (STRING *&s_array, char delimiter)
{
	if (delimiter == ' ')
		return 0;

	// Count the number of items
	int i=0;
	int ndelimeters=0;
	int quote=0;

	while (1)
	{
		if (s[i] == 0)
			break;

		if (s[i] == '\"')
			quote ? quote = 1 : quote = 0;

		if (quote)
		{
			i++;
			continue;
		}
		
		if (s[i] == ' ')
		{
			i++;
			continue;
		}

		if (s[i] == delimiter)
		{
			ndelimeters++;
			i++;
			continue;
		}

		i++;
	}

	s_array = new STRING[ndelimeters+1];

	int is =0;
	i=0;
	int nitems = 0;

	while (s[i])
	{
		// Process each item
		int istart=-1,iend=-1;

		// Trim leading spaces
		while (s[i] == ' ') i++;


		if (s[i] == '\"') // starting quote found
		{
			i++;
			istart = i;
			while (s[i] && s[i] != '\"') i++; // find ending quote
			if (s[i] == 0) break; // no matching quote found
			iend = i-1;
			
			if (nitems == ndelimeters+1) break; // this should not happen

			s_array[nitems].Insert (0,&s[istart],iend-istart+1);
			nitems++;
			i++;
			continue; // finished processing quoted item
		}

		// start non quoted item
		istart = i;
		while (s[i] && s[i] != delimiter) i++;
		iend = i-1;

		if (nitems == ndelimeters+1) break; // this should not happen

		s_array[nitems].Insert (0,&s[istart],iend-istart+1);
		nitems++;

		if (s[i] == 0)
			break;

		i++;
	}

	return nitems;
}

int STRING::GetArrayOfDelimitedItems (STRING *&array, char delimiter)
{
	char d[2];
	d[0] = delimiter;
	d[1] = 0;

	array = 0;
	int n = Replace (d,d);

	if (n == 0)
	{
		if (s[0] == 0)
			return 0;

		array = new STRING[1];
		array[0] = (const char *)s;

		return 1;
	}

	array = new STRING[n+1];

	const char *c0 = s;
	const char *c1 = strchr (s,delimiter);

    int i;
	for (i=0; i<n; i++)
	{
		array[i].Assign (c0,c1-c0);
		c0 = c1 + 1;
		c1 = strchr (c0,delimiter);
	}

	array[i].Assign (c0);

	return n+1;
}

bool STRING::SwapInSingleWildcardCharactersFromNamingConvention (const char *naming_convention, STRING *s_swapped/*=0*/)
{
	int idx_scw[20];
	for (int i=0; i<20; i++)
		idx_scw[i] = -1;

	int idx_number;
	BOOL status = matches_naming_convention (s,naming_convention,idx_number,0,0,1,idx_scw,20);
	if (status == FALSE)
		return false;

	for (int i=0; i<20; i++)
	{
		if (idx_scw[i] == -1)
			return true;

		if (s_swapped)
		{
			char sw[2];			
			
			if (i > 0 && idx_scw[i] - idx_scw[i-1] == 2)
			{
				sw[0] = s[idx_scw[i]-1];
				sw[1] = 0;
				s_swapped->Append (sw);
			}

			sw[0] = s[idx_scw[i]];
			sw[1] = 0;
			s_swapped->Append (sw);
		}

		Insert (idx_scw[i],"?");
		Remove (idx_scw[i]+1,1);
	}

	return true;
}

BOOL STRING::MatchesNamingConvention (const char *naming_convention, const char *sample_name/*=0*/, BOOL exclude_path/*=TRUE*/)
{
	int number_idx;
	int number_n;
	BOOL status = matches_naming_convention (s,naming_convention,number_idx,&number_n,sample_name,exclude_path);

	if (status == FALSE)
		return FALSE;

	return TRUE;
}

int STRING::ReplaceInvalidFileNameCharacters (char replace_char)
{
	if (!s)
		return 0;

	if (IsValidFileNameChar (replace_char) == FALSE)
		return 0;

	int i=0;
	int n=0;
	while (s[i])
	{
		if (IsValidFileNameChar(s[i]))
		{
			i++;
			continue;
		}

		s[i] = replace_char;
		n++;
		i++;
	}

	return n;
}

BOOL STRING::RemoveIndexFromName (const char *naming_convention, BOOL exclude_path/*=TRUE*/)
{
	int number_idx,number_n;

	char *s0 = s;
	if (matches_naming_convention (s0,naming_convention,number_idx,&number_n,0,exclude_path) == FALSE)
		return FALSE;

	const char *c = s;
	if (number_idx && s[number_idx-1] == '.' || s[number_idx-1] == '_')
	{
		number_n++;
		number_idx--;
	}

	return Remove (number_idx,number_n);
}

inline int get_number_of_number_char_in_naming_convention_index (const char *naming_convention)
{
	int n_nc_number = 0;
	int n_nc = StrLen (naming_convention);

	for (int i=0; i<n_nc; i++)
	{
		if (naming_convention[i] >= '0' && naming_convention[i] <= '9')
		{
			n_nc_number++;
		}
		else
		{
			if (n_nc_number)
				break;
		}
	}

	return n_nc_number;
}

BOOL STRING::GetIndexUsingNamingConvention (const char *naming_convention, long &index, BOOL exclude_path/*=TRUE*/)
{
	index = -1;

	int number_idx;
	if (matches_naming_convention (s,naming_convention,number_idx,0,0,exclude_path) == FALSE)
		return FALSE;

	if (number_idx < 0)
		return FALSE;

	int n=0;
	while (s[number_idx+n] >= '0' && s[number_idx+n] <= '9')
		n++;

	if (n < 1)
		return FALSE;

	char *buf = (char *)alloca (n+1);
	MemMove (buf,&s[number_idx],n);
	buf[n] = 0;

	index = atoi (buf);
	return TRUE;
}

BOOL STRING::RemovePath (void)
{
	int i=0;
	if (!s)
		return FALSE;

	int j=-1;
	while (s[i])
	{
		if (s[i] == '\\')
			j = i;

		i++;
	}

	if (j == -1)
		return TRUE;

	j++;
	
	MemMove (s,&s[j],i-j);
	s[i-j] = 0;

	return TRUE;
}

BOOL STRING::SetIndexUsingNamingConvention (const char *naming_convention, long index, BOOL exclude_path/*=TRUE*/)
{
	int number_idx;

	if (matches_naming_convention (s,naming_convention,number_idx,0,0,exclude_path) == FALSE)
		return FALSE;

	if (number_idx < 0)
		return FALSE;

	int n=0;
	while (s[number_idx+n] >= '0' && s[number_idx+n] <= '9')
		n++;

	if (n < 1)
		return FALSE;

	if (n > 10)
		return FALSE;

	char fmt[32];
	sprintf_s (fmt,sizeof(fmt),"%%0%ldld",n);

	char snum[32];
	sprintf_s (snum,sizeof(snum),fmt,index);

	int n_nc_number = get_number_of_number_char_in_naming_convention_index (naming_convention);

	if (n_nc_number != 1)
	{
		if (StrLen (snum) != n)
			return FALSE;
	}

	if (Remove (number_idx,n) == FALSE)
		return FALSE;

	int len = StrLen (snum);
	if (Insert (number_idx,snum,len) == FALSE)
		return FALSE;

	return TRUE;

}

BOOL STRING::ConvertToFileFindSpecBasedOnNamingConvention (const  char *naming_convention,BOOL exclude_path/*=TRUE*/)
{
	int number_idx;

	if (matches_naming_convention (s,naming_convention,number_idx,0,0,exclude_path) == FALSE)
		return FALSE;

	if (number_idx < 0)
		return FALSE;

	int n=0;
	while (s[number_idx+n] >= '0' && s[number_idx+n] <= '9')
		n++;

	if (n < 1)
		return FALSE;

	if (n > 10)
		return FALSE;


	int n_nc_number = get_number_of_number_char_in_naming_convention_index (naming_convention);

	char snum[32];
	if (n_nc_number == 1)
	{
		snum[0] = '*';
		snum[1] = 0;
	}
	else
	{
        int i;
		for (i=0; i<n_nc_number; i++)
			snum[i] = '?';
		snum[i] = 0;
	}

	if (n_nc_number != 1)
	{
		if (StrLen (snum) != n)
			return FALSE;
	}

	if (Remove (number_idx,n) == FALSE)
		return FALSE;

	int len = StrLen (snum);
	if (Insert (number_idx,snum,len) == FALSE)
		return FALSE;

	return TRUE;
}	
	
BOOL STRING::RemoveFileNameExtension (void)
{
	STRING tmp;
	if (tmp.LoadFileName (s,TRUE,TRUE,FALSE) == FALSE)
		return FALSE;

	*this = (const char *) tmp;
	return TRUE;
}


BOOL STRING::ConvertBetweenNamingConventions (const char *naming_convention_src, const char *naming_convention_tgt, BOOL exclude_path/*=TRUE*/)
{
	// count the number of '*' characters in each naming convention
	int n_star_src=0, n_star_tgt=0;
	int n_dot_src=0, n_dot_tgt=0;

#define MAX_STAR 32
#define MAX_DOT  32

	int isrc=0,itgt=0;

	int i_num_src=0;
	int i_num_tgt=0;
	int n_num_src=0;
	int n_num_tgt=0;

	int i_star_src[MAX_STAR];
	int i_star_tgt[MAX_STAR];
	int i_dot_src[MAX_DOT];
	int i_dot_tgt[MAX_DOT];

	int N_src = 0;
	int N_tgt = 0;
	int i,j,n;
	int OFFSET=0;
	int INDEX=0;

	MemSet (i_star_src,0,sizeof(i_star_src));
	MemSet (i_star_tgt,0,sizeof(i_star_tgt));
	MemSet (i_dot_src,0,sizeof(i_dot_src));
	MemSet (i_dot_tgt,0,sizeof(i_dot_tgt));

	while (naming_convention_src[isrc])
	{
		if (naming_convention_src[isrc] == '*')
		{
			if (n_star_src == MAX_STAR)
				return FALSE;

			i_star_src[n_star_src] = isrc;
			n_star_src++;
		}
		else
		if (naming_convention_src[isrc] == '.')
		{
			if (n_dot_src == MAX_DOT)
				return FALSE;

			i_dot_src[n_dot_src] = isrc;
			n_dot_src++;
		}
		else
		if (!n_num_src)
		{
			if (naming_convention_src[isrc] >= '0' && naming_convention_src[isrc] <= '9')
			{
				i_num_src = isrc;

				while (naming_convention_src[isrc] >= '0' && naming_convention_src[isrc] <= '9')
				{
					isrc++;
					n_num_src++;
				}

				isrc--;
			}
		}

		isrc++;
	}

	while (naming_convention_tgt[itgt])
	{
		if (naming_convention_tgt[itgt] == '*')
		{
			if (n_star_tgt == MAX_STAR)
				return FALSE;

			i_star_tgt[n_star_tgt] = itgt;
			n_star_tgt++;
		}
		else
		if (naming_convention_tgt[itgt] == '.')
		{
			if (n_dot_tgt == MAX_DOT)
				return FALSE;

			i_dot_tgt[n_dot_tgt] = itgt;
			n_dot_tgt++;
		}
		else
		if (!n_num_tgt)
		{
			if (naming_convention_tgt[itgt] >= '0' && naming_convention_tgt[itgt] <= '9')
			{
				i_num_tgt = itgt;

				while (naming_convention_tgt[itgt] >= '0' && naming_convention_tgt[itgt] <= '9')
				{
					itgt++;
					n_num_tgt++;
				}

				itgt--;
			}
		}

		itgt++;
	}

	if (n_num_src == 0)
		return FALSE;
	
	if (n_num_tgt == 0)
		return FALSE;

	if (n_num_tgt > 10)
		return FALSE;

	if (n_num_src > 10)
		return FALSE;

	n=0;
	for (i=i_num_src+n_num_src-1; i>=i_num_src; i--)
	{
		int N = naming_convention_src[i] - '0';
		for (j=0;j<n;j++) N *= 10;
		N_src += N;
		n++;
	}

	n=0;
	for (i=i_num_tgt+n_num_tgt-1; i>=i_num_tgt; i--)
	{
		int N = naming_convention_tgt[i] - '0';
		for (j=0;j<n;j++) N *= 10;
		N_tgt += N;
		n++;
	}

	OFFSET = N_tgt - N_src;

	///////////////////////////////////////////////////////////////////////////////////////////
	// Process the index
	if (GetIndexUsingNamingConvention (naming_convention_src,(long&)INDEX,exclude_path) == FALSE)
		return FALSE; // does not match naming convention

	char fmt_num_tgt[32],s_num_tgt[32];

	if (n_num_tgt == 1)
		sprintf_s (fmt_num_tgt,sizeof(fmt_num_tgt),"%%ld");
	else
		sprintf_s (fmt_num_tgt,sizeof(fmt_num_tgt),"%%0%ldld",(long)(n_num_tgt));

	sprintf_s (s_num_tgt,sizeof(s_num_tgt),fmt_num_tgt,(long)(INDEX+OFFSET));

	int n_leading_dots_src = 0;
	for (isrc=0; isrc<n_dot_src; isrc++)
	{
		if (i_dot_src[isrc] < i_num_src)
			n_leading_dots_src++;
	}

	int n_trailing_dots_src = n_dot_src - n_leading_dots_src;

	int n_leading_dots_tgt = 0;
	for (itgt=0; itgt<n_dot_tgt; itgt++)
	{
		if (i_dot_tgt[itgt] < i_num_tgt)
			n_leading_dots_tgt++;
	}

	int n_trailing_dots_tgt = n_dot_tgt - n_leading_dots_tgt;

	int new_alloc = StrLen (s);
	new_alloc += StrLen (naming_convention_tgt);
	new_alloc += 10;

	char *tmp = new char[new_alloc];
	MemSet (tmp,0,new_alloc);

	// Special case.  No '*' after the index
	int index_of_number_src = -1;
	if (n_star_src && i_star_src[n_star_src-1] < i_num_src)
	{
		index_of_number_src = StrLen (s) - (StrLen (naming_convention_src) - i_num_src);
	}

	int i_new_dot_insert_location_tgt = 0;

	i=0;
	j=0;
	for (isrc=0; isrc<i_num_src; isrc++)
	{
		if (isrc == i_num_src)
			break;

		if (naming_convention_src[isrc] == '*')
		{
			while (index_of_number_src != -1 && n_star_src && i_star_src[n_star_src-1] == isrc)
			{
				// only one star in the naming_convention_src and it comes before the number
				if (i != index_of_number_src)
				{
					ASSERT(j<new_alloc);
					tmp[j++] = s[i++];
					continue;
				}
				else
				{
					i_new_dot_insert_location_tgt = j;
					break;
				}
			}

			if (i_new_dot_insert_location_tgt == 0)
			{
				while (s[i] != naming_convention_src[isrc+1])
				{
					ASSERT(j<new_alloc);
					tmp[j++] = s[i++];
					continue;
				}

				i_new_dot_insert_location_tgt = j;
			}
		}
		else if (naming_convention_src[isrc] == '.')
		{
			ASSERT (s[i] == '.');
			if (n_leading_dots_tgt)
			{
				ASSERT(j<new_alloc);
				tmp[j++] = s[i++];
				n_leading_dots_tgt--;
			}
			else
			{
				ASSERT(j<new_alloc);
				tmp[j++] = '_'; // convert to underscore
			}

			continue;
		}
		else
		{
			if (index_of_number_src != -1 && i == index_of_number_src)
				continue;

			ASSERT(s[i]==naming_convention_src[isrc]);
			ASSERT(j<new_alloc);
			tmp[j++] = s[i++];
		}
	}

	/////////////////////////////////////////////////////////////////////////////
	// Insert more dot delimited sections if necessary
	char insert_char = 'A';
	if (n_leading_dots_tgt)
	{
		char *insert_buffer = (char *) alloca (n_leading_dots_tgt*2+1);
		int idot;
		for (idot=0; idot<n_leading_dots_tgt; idot++)
		{
			insert_buffer[idot*2] = insert_char++;
			insert_buffer[1+idot*2] = '.';
		}
		insert_buffer[idot*2] = 0;

		ASSERT(j+n_leading_dots_tgt*2 <= new_alloc);
		
		if (tmp[i_new_dot_insert_location_tgt] != '.')
		{
			MemInsert (tmp,j,1,i_new_dot_insert_location_tgt,".");
			j++; i_new_dot_insert_location_tgt++;
			ASSERT(j+n_leading_dots_tgt*2 <= new_alloc);

			if (i_new_dot_insert_location_tgt > 1 && tmp[i_new_dot_insert_location_tgt-2] == '_')
			{
				MemRemove (tmp,j,1,i_new_dot_insert_location_tgt-2);
				j--; i_new_dot_insert_location_tgt--;
			}
		}

		MemInsert (tmp,j,n_leading_dots_tgt*2,i_new_dot_insert_location_tgt,insert_buffer);
	}
	else
	{
		if (n_leading_dots_src)
			i++;
	}
	///////////////////////////////////////////////////////////////////////////////////////

	// Insert the number
	ASSERT(StrLen(s_num_tgt)==n_num_tgt);
	for (int inum=i; inum<i+n_num_tgt; inum++)
	{
		ASSERT(j<new_alloc);
		tmp[j++] = s_num_tgt[inum-i];
	}

	i += n_num_src;

	isrc=i_num_src+n_num_src;

	BOOL b_ThrowAway = FALSE;
	while (naming_convention_src[isrc])
	{
		ASSERT (s[i]);

		if (naming_convention_src[isrc] == '*')
		{
			while (s[i] != naming_convention_src[isrc+1])
			{
				if (b_ThrowAway == FALSE)
				{
					ASSERT(j<new_alloc);
					tmp[j++] = s[i++];
				}
				else
					i++;

				continue;
			}
		}
		else
		if (naming_convention_src[isrc] == '.')
		{
			ASSERT (s[i] == '.');

			i_new_dot_insert_location_tgt = j+1;

			if (n_trailing_dots_src > n_trailing_dots_tgt)
			{
				n_trailing_dots_src--;
				b_ThrowAway = TRUE;
				i++;
			}
			else
			{
				b_ThrowAway = FALSE;
				tmp[j++] = s[i++];
			}
		}
		else
		{
			if (b_ThrowAway)
				i++;
			else
			{
				ASSERT(s[i]==naming_convention_src[isrc]);
				ASSERT(j<new_alloc);
				tmp[j++] = s[i++];
			}
		}

		isrc++;
	}


	/////////////////////////////////////////////////////////////////////////////
	// Insert more dot delimited sections if necessary
	if (n_trailing_dots_tgt)
	{
		char *insert_buffer = (char *) alloca (n_trailing_dots_tgt*2+1);
		int idot;
		for (idot=0; idot<n_trailing_dots_tgt; idot++)
		{
			insert_buffer[idot*2] = insert_char++;
			insert_buffer[1+idot*2] = '.';
		}
		insert_buffer[idot*2] = 0;

		ASSERT(j+n_trailing_dots_tgt*2 <= new_alloc);
		MemInsert (tmp,j,n_leading_dots_tgt*2,i_new_dot_insert_location_tgt,insert_buffer);
	}
	///////////////////////////////////////////////////////////////////////////////////////

	delete [] s;
	s = tmp;

	return TRUE;
}

BOOL STRING::TrimPath (void)
{
	int len = StrLen (s);
	if (len && s[len-1] == '\\')
	{
		s[len-1] = 0;
		return TRUE;
	}

	return FALSE;
}

bool ConvertToWORD (const char *c, WORD &u2, int *ioffset/*=0*/)
{
	if (!c)
		return false;

	DWORD u4=0;
	if (sscanf_s (c,"%ul",&u4) == 1)
	{
		u2 = (WORD) u4;

		if (ioffset)
		{
			*ioffset = 0;

			while (IsDigit (c[*ioffset]) == false && c[*ioffset])
				(*ioffset)++;

			while (IsDigit (c[*ioffset]))
				(*ioffset)++;

			while (IsDigit (c[*ioffset]) == false && c[*ioffset])
				(*ioffset)++;
		}

		return true;
	}

	return false;
}


}; // namespace WorkbenchLib
