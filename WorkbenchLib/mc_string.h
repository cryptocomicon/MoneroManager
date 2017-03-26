#ifndef INC_STRING
#define INC_STRING

#include "memory.h" 
#include "string.h"
#include "malloc.h"
#include "stdio.h"


namespace WorkbenchLib
{

bool ConvertToWORD (const char *c, WORD &u2, int *ioffset=0);

BOOL DoubleToString (double d, char *s, short max_len);
BOOL FloatToString (float d, char *s_ret, short max_len, short sig_digits  = 6 , BOOL fixed_decimal = FALSE);
BOOL EngineeringNotation
(
	double input_variable,
	char *output_var_str,
	short max_output_str_len,
	short sig_digits
);

inline BOOL IsWhiteSpaceChar (char c)
{
    if (c == ' ') return TRUE;   // SPACE
    if (c == 0x09 ) return TRUE;  // TAB
    return FALSE;
}

inline BOOL IsDigit (const char &c)
{
	if (c < '0') return FALSE;
	if (c > '9') return FALSE;
	return TRUE;
}

inline BOOL IsSign (const char &c)
{
	if (c == '+') return TRUE;
	if (c == '-') return TRUE;
	return FALSE;
}

inline BOOL IsNameStartChar (char c)
{
    if (c >= 'a' && c <= 'z') return TRUE;
    if (c >= 'A' && c <= 'Z') return TRUE;
    if (c == '_') return TRUE;
    return FALSE;
}

inline BOOL IsNameChar (char c)
{
    if (IsNameStartChar (c)) return TRUE;

    if (c >= '0' && c <= '9') return TRUE;
    return FALSE;
}

inline const char *GetCommaDelimited_long (const char *c, long &v)
{
	while (IsWhiteSpaceChar (*c)) c++;

	const char *c0 = c;
	if (IsSign(*c)) c++;
	while (IsDigit(*c)) c++;

	if (*c != ',')
		return 0;

	if (sscanf_s (c0,"%ld",&v) != 1)
		return 0;

	return c+1;
}

inline const char *GetTabDelimited_long (const char *c, long &v)
{
	while (IsWhiteSpaceChar (*c) && *c != '\t') c++;

	const char *c0 = c;
	if (IsSign(*c)) c++;
	while (IsDigit(*c)) c++;

	if (*c != '\t')
		return 0;

	if (sscanf_s (c0,"%ld",&v) != 1)
		return 0;

	return c+1;
}

inline const char *GetCommaDelimited_float (const char *c, float &v)
{
	while (IsWhiteSpaceChar (*c)) c++;

	const char *c0 = c;

	while (1)
	{
		if (*c == ',')
			break;

		if (*c == 0)
			break;

		if (*c == 0x0d)
			break;

		if (*c == 0x0a)
			break;

		if (IsDigit (*c) == FALSE && IsSign (*c) == FALSE && *c != '.' && *c != 'E' && *c != 'e')
			return 0;

		c++;
	}

	if (sscanf_s (c0,"%f",&v) != 1)
		return 0;

	return c+1;
}

inline const char *GetTabDelimited_float (const char *c, float &v)
{
	while (IsWhiteSpaceChar (*c) && *c != '\t') c++;

	const char *c0 = c;

	while (1)
	{
		if (*c == '\t')
			break;

		if (*c == 0)
			break;

		if (*c == 0x0d)
			break;

		if (*c == 0x0a)
			break;

		if (IsDigit (*c) == FALSE && IsSign (*c) == FALSE && *c != '.' && *c != 'E' && *c != 'e')
			return 0;

		c++;
	}

	if (sscanf_s (c0,"%f",&v) != 1)
		return 0;

	return c+1;
}

inline const char *GetCommaDelimited_double (const char *c, double &v)
{
	while (IsWhiteSpaceChar (*c)) c++;

	const char *c0 = c;

	while (1)
	{
		if (*c == ',')
			break;

		if (IsDigit (*c) == FALSE && IsSign (*c) == FALSE && *c != '.' && *c != 'E' && *c != 'e')
			return 0;

		c++;
	}

	if (sscanf_s (c0,"%lf",&v) != 1)
		return 0;

	return c+1;
}

inline const char *GetTabDelimited_double (const char *c, double &v)
{
	while (IsWhiteSpaceChar (*c) && *c != '\t') c++;

	const char *c0 = c;

	while (1)
	{
		if (*c == '\t')
			break;

		if (IsDigit (*c) == FALSE && IsSign (*c) == FALSE && *c != '.' && *c != 'E' && *c != 'e')
			return 0;

		c++;
	}

	if (sscanf_s (c0,"%lf",&v) != 1)
		return 0;

	return c+1;
}

inline BOOL IsSameCharCaseInsensitive (char c0, char c1)
{
	if (c0 >= 'a' && c0 <= 'z')
	{
		c0 -= 'a';
		c0 += 'A';

	}

	if (c1 >= 'a' && c1 <= 'z')
	{
		c1 -= 'a';
		c1 += 'A';
	}

	return c0 == c1;
}


inline void MemMove (void *buffer, const void *src, const int bytes)
{
	if (!(bytes%sizeof(int)))
	{
		unsigned int *ib = (unsigned int *) buffer;
        unsigned int *is = (unsigned int *) src;
		int n = bytes/sizeof(int);
		for (int i=0; i<n; i++) { *ib = *is; ib++; is++; }
		return;
	}

	unsigned char *c = (unsigned char *) buffer;
    unsigned char *cs = (unsigned char *) src;
	for (int i=0; i<bytes; i++) { *c = *cs; c++; cs++; }
}

inline BOOL BuffersAreIdentical (const void *b0, const void *b1, const int bytes)
{
	if (!(bytes%sizeof(int)))
	{
		unsigned int *ib = (unsigned int *) b0;
        unsigned int *is = (unsigned int *) b1;
		int n = bytes/sizeof(int);
		for (int i=0; i<n; i++) 
		{ 
			if (*ib != *is) 
				return FALSE; 
			ib++; is++; 
		}

		return TRUE;
	}

	unsigned char *c = (unsigned char *) b0;
    unsigned char *cs = (unsigned char *) b1;

	for (int i=0; i<bytes; i++) 
	{
		if (*c != *cs)
			return FALSE;
		c++; cs++; 
	}

	return TRUE;
}

inline void MemRemove (void *dest_buffer,
					   const int &old_size,
					   const int &remove_size,
					   const int &remove_offset)
{
	if (remove_size == 0)
		return;

	unsigned char *cd = (unsigned char *) dest_buffer;
	int n_to_move = old_size - remove_offset - remove_size;

	int id = remove_offset;
	int is = remove_offset + remove_size;

	while (n_to_move--) { cd[id++] = cd[is++]; }
}

inline void	MemInsert (void *dest_buffer, 
					   const int &old_size, 
					   const int &insert_size, 
					   const int &insert_offset, 
					   const void *insert_buffer)
{
	if (insert_size == 0)
		return;

	if (insert_size%sizeof(int) || old_size%sizeof(int))
	{
		///////////////////////////////////////
		// Must process one byte at a time

		// Shift the contents of dest_buffer
		unsigned char *cd = (unsigned char *) dest_buffer;

		int n_to_move = old_size - insert_offset;
		ASSERT (n_to_move >= 0);

		int is = old_size-1;
		int id = old_size+insert_size-1;

		while (n_to_move--)	{ cd[id--] = cd[is--]; }

		// Copy from insert_buffer to dest_buffer
		is = 0;
		id = insert_offset;

		const unsigned char *cs = (const unsigned char *) insert_buffer;
		int n_to_copy = insert_size;

		while (n_to_copy--) { cd[id++] = cs[is++]; }
	}
	else
	{
		/////////////////////////////////////////////////////////////////////////
		// Process in chunks that the machine is designed to handle optimally

		// Shift the contents of dest_buffer
		unsigned int *cd = (unsigned int *) dest_buffer;

		int n_to_move = old_size - insert_offset;
		n_to_move /= sizeof(int);

		int is = old_size/sizeof(int) - 1;
		int id = old_size/sizeof(int) + insert_size/sizeof(int) - 1;

		while (n_to_move--)	{ cd[id--] = cd[is--]; }

		// Copy from insert_buffer to dest_buffer
		is = 0;
		id = insert_offset/sizeof(int);

		const unsigned int *cs = (const unsigned int *) insert_buffer;
		int n_to_copy = insert_size/sizeof(int);

		while (n_to_copy--) 
		{ 
			cd[id++] = cs[is++]; 
		}
	}
}

inline BOOL MemReplace 
(
	void *&buffer,
	int &buffer_len,
	int offset,
	int len_of_existing_item,
	const void *new_item,
	int len_of_new_item,
	BOOL reallocate_if_larger=TRUE,
	BOOL reallocate_if_smaller=FALSE
)
{
	unsigned char *b = (unsigned char *) buffer;

	if (offset + len_of_existing_item > buffer_len)
		return FALSE;

	if (len_of_existing_item > len_of_new_item)
	{
		MemMove (b+offset,new_item,len_of_new_item);

		int remove_size = len_of_existing_item - len_of_new_item;
		int remove_offset = offset + len_of_new_item;

		MemRemove (b,buffer_len,remove_size,remove_offset);

		buffer_len -= remove_size;

		if (reallocate_if_smaller == FALSE)
			return TRUE;

		BYTE *tmp = new BYTE[buffer_len];
		if (!tmp)
			return FALSE;

		MemMove (tmp,b,buffer_len);
		delete [] b;
		buffer = tmp;

		return TRUE;
	}

	if (len_of_existing_item == len_of_new_item)
	{
		MemMove (b+offset,new_item,len_of_new_item);
		return TRUE;
	}

	/////////////////////////////////////////////////////////////////////////////
	// len_of_existing_item < length_of_new_item

	if (reallocate_if_larger == FALSE)
		return FALSE; // can't do this without reallocation

	// Remove the existing item and then reallocate
	MemRemove (b,buffer_len,len_of_existing_item,offset);

	buffer_len -= len_of_existing_item;

	BYTE *tmp = new BYTE[buffer_len+len_of_new_item];
	if (!tmp)
		return FALSE;

	MemMove (tmp,b,offset);
	MemMove (tmp+offset,new_item,len_of_new_item);
	MemMove (tmp+offset+len_of_new_item,b+offset,buffer_len-offset);

	delete [] b;
	buffer = (void *)tmp;
	buffer_len += len_of_new_item;

	return TRUE;
}

inline BOOL IsValidFileNameChar (const char &c, BOOL including_path=FALSE)
{
	if (c >= 'a' && c <= 'z') return TRUE;
	if (c >= 'A' && c <= 'Z') return TRUE;
	if (c >= '0' && c <= '9') return TRUE;

	switch (c)
	{
	case '.':
	case ',':
	case '!':
	case '@':
	case '#':
	case '$':
	case '%':
	case '^':
//	case '*':
	case '(':
	case ')':
	case '_':
	case '-':
	case '+':
	case '=':
//	case '|':
	case '~':
	case '<':
	case '>':
//	case '?':
	case '/':
	case ';':
//	case '\"':
//	case '\'':
	case '[':
	case ']':
	case '{':
	case '}':
	case ' ':
//	case 0x09:
		return TRUE;


	case ':':
	case '\\':
		if (including_path == FALSE)
			return FALSE;
		return TRUE;
	}

	return FALSE;
}

inline BOOL IsSlash (const char &c)
{
	if (c == '\\')
		return TRUE;
	
	if (c == '/')
		return TRUE;

	return FALSE;
}

inline BOOL IsValidFullyQualifiedFileName (const char *s)
{
	int i=0;

	int nslashes=0;
	for (i=0; i<4096; i++)
	{
		if (s[i] == 0)
			break;

		if (IsValidFileNameChar (s[i],TRUE) == FALSE)
			return FALSE;

		if (IsSlash (s[i]))
			nslashes++;
	}

	if (s[i] != 0)
		return FALSE;

	if (i < 3)
		return FALSE;

	if ((s[0] < 'A' || s[0] > 'Z') && (s[0] < 'a' || s[0] > 'z'))
	{
		// First character is not a drive letter, must have // at the beginning
		if (nslashes < 3)
			return FALSE;

		if (IsSlash (s[0]) == FALSE)
			return FALSE;
		if (IsSlash (s[1]) == FALSE)
			return FALSE;

		if (IsValidFileNameChar (s[2],FALSE) == FALSE)
			return FALSE; // Third character must not be \ or :

		if (IsValidFileNameChar(s[i-1],FALSE) == FALSE)
			return FALSE;  // last char must not be \ or :

		return TRUE;
	}

	// First letter is drive letter
	if (s[1] != ':')
		return FALSE;

	if (IsSlash (s[2]) == FALSE)
		return FALSE;

	if (IsValidFileNameChar (s[3],FALSE) == FALSE) // Fourth character must not be \ or :
		return FALSE;

	if (IsValidFileNameChar(s[i-1],FALSE) == FALSE)
		return FALSE;  // last char must not be \ or :

	return TRUE;
}


inline BOOL IsValidASCIIChar (const char &c)
{
	if (c >= 'a' && c <= 'z') return TRUE;
	if (c >= 'A' && c <= 'Z') return TRUE;
	if (c >= '0' && c <= '9') return TRUE;
	switch (c)
	{
	case '.':
	case ',':
	case '!':
	case '@':
	case '#':
	case '$':
	case '%':
	case '^':
	case '*':
	case '(':
	case ')':
	case '_':
	case '-':
	case '+':
	case '=':
	case '|':
	case '\\':
	case '~':
	case '<':
	case '>':
	case '?':
	case '/':
	case ':':
	case ';':
	case '\"':
	case '\'':
	case '[':
	case ']':
	case '{':
	case '}':
	case ' ':
	case 0x09:
		return TRUE;
	}

	return FALSE;
}

inline char upper_case (char c)
{
	if (c >= 'a' && c <= 'z') return (c - 0x20);
	return c;
}

inline char lower_case (char c)
{
	if (c >= 'A' && c <= 'Z') return (c + 0x20);
	return c;
}

inline short upper_case_u (short c)
{
	if (c >= 'a' && c <= 'z') return (c - 0x20);
	return c;
}

inline short lower_case_u (short c)
{
	if (c >= 'A' && c <= 'Z') return (c + 0x20);
	return c;
}

inline int StrLenM (const char *s, int max_len, BOOL excluding_whitespaces=FALSE)
{
	if (!s)
		return 0;

	if (excluding_whitespaces == FALSE)
	{
		int n=0;
		while (s[n] && n<max_len) n++;
		return n;
	}
	else
	{
		int n=0,i=0;
		while (s[i])
		{
			if (i == max_len)
				return max_len;

			if (IsWhiteSpaceChar (s[i]) == FALSE) 
				n++;

			i++;
		}

		return n;
	}
}

inline int StrLen (const char *s, BOOL excluding_whitespaces=FALSE)
{
	if (!s)
		return 0;

	if (excluding_whitespaces == FALSE)
	{
		int n=0;
		while (s[n]) n++;
		return n;
	}
	else
	{
		int n=0,i=0;
		while (s[i])
		{
			if (IsWhiteSpaceChar (s[i]) == FALSE) 
				n++;

			i++;
		}

		return n;
	}
}

inline bool HasChar (const char *s, char c)
{
	int i=0;
	while (1)
	{
		if (s[i] == 0)
			return false;

		if (s[i] == c)
			return true;

		i++;
	}
}

inline void EliminateTrailingSpaces (char *s)
{
	if (!s)
		return;

	int n = StrLen (s);
	int i;
	for (i=n-1;i>=0;i--)
	{
		if (!(s[i] == ' ' || s[i] == 0x09)) break;
	}
	s[i+1] = 0;
}

inline void StrCpy (char *dest, const char *src)
{
	if (dest == 0)
		return;

	if (src == 0)
		return;

	long i=0;
	while (src[i]) { dest[i] = src[i]; i++; }
	dest[i] = 0;
}

inline BOOL StrAppend (char *buf, int &ibuf, int buf_sz, const char *sappend)
{
	int n = StrLen (sappend);

	if (n + 1 + ibuf >= buf_sz)
		return FALSE;

	StrCpy (&buf[ibuf],sappend);
	ibuf += n;

	return TRUE;
}

inline int StrCmp (const char *s0, const char *s1)
{
	if (s0 == 0)
		return 0;

	if (s1 == 0)
		return 0;

	long i=0;
	while (s0[i])
	{
		if (s0[i] < s1[i]) return -1;
		if (s0[i] > s1[i]) return 1;
		if (!s1[i]) return 1;
		i++;
	}

	if (!s1[i]) return 0;
	return -1;
}

inline BOOL IsNotEmptyString (const char *c)
{
	if (c == 0)
		return FALSE;

	if (c[0] == 0)
		return FALSE;

	int i=0;
	while (c[i] && IsWhiteSpaceChar (c[i])) i++;

	if (c[i] == 0)
		return FALSE;

	return TRUE;
}

inline BOOL IsEqualTo (const char *c0, const char *c1, BOOL case_sensitive=FALSE, int len=0)
{
	if (c0 == 0 && c1)
		return FALSE;

	if (c1 == 0 && c0)
		return FALSE;

	if (c0 == 0 && c1 == 0)
		return TRUE;

	int i=0;
	while (1)
	{
		if (case_sensitive==FALSE) 
        {
			if (upper_case (c0[i]) != upper_case(c1[i])) 
                return FALSE;
        }
        else
        {
		    if (c0[i] != c1[i]) 
				return FALSE;
        }

		if (!c0[i]) 
			return TRUE;
		if (len && i == len) 
			return TRUE;
		i++;
		if (len && i == len) 
			return TRUE;
	}
}

inline BOOL IsEqualToU (const WORD *c0, const WORD *c1, BOOL case_sensitive=FALSE, int len=0)
{
	if (c0 == 0 && c1)
		return FALSE;

	if (c1 == 0 && c0)
		return FALSE;

	if (c0 == 0 && c1 == 0)
		return TRUE;

	int i=0;
	while (1)
	{
		if (case_sensitive==FALSE) 
        {
			if (upper_case_u(c0[i]) != upper_case_u(c1[i])) 
                return FALSE;
        }
        else
        {
		    if (c0[i] != c1[i]) 
				return FALSE;
        }

		if (!c0[i]) 
			return TRUE;
		if (len && i == len) 
			return TRUE;
		i++;
		if (len && i == len) 
			return TRUE;
	}
}

inline int StrMCpy (char *dest, const char *src, int n)
{
	if (dest == 0)
		return 0;

	if (src == 0)
		return 0;

	if (n <= 0)
		return 0;
    
    int i;
	for (i=0; i<n-1; i++)
	{
		dest[i] = src[i];
		if(!dest[i]) return i;
	}

	dest[i] = 0;
	return i;
}

inline int StrNCpy (char *dest, const char *src, int n)
{
	if (dest == 0)
		return 0;

	if (src == 0)
		return 0;

    int i;
	for (i=0; i<n; i++)
	{
		dest[i] = src[i];
		if(!dest[i]) return i;
	}
	
	return i;
}

// Returns StrLen (dest) after the operation is performed
inline int StrMCat (char *dest, const char *src, int n)
{
	if (dest == 0)
		return 0;

	if (src == 0)
		return 0;

	int len = StrLen (dest);
	if (len >= n-1) return len;
	int i_added = StrMCpy (&dest[len],src,n-len);
	return i_added + len;
}


inline BOOL IsEndOfWordChar (const char &c)
{
    if (c >= '0' && c <= '9') 
        return FALSE;

    if (c >= 'a' && c <= 'z')
        return FALSE;

    if (c >= 'A' && c <= 'Z')
        return FALSE;

    if (c == '_')
        return FALSE;

    return TRUE;
}


inline int StrNCmp (const char *s0, const char *s1, int n)
{
	if (s0 == 0 && s1)
		return -1;

	if (s1 == 0 && s0)
		return 1;

	if (s0 == 0 && s1 == 0)
		return 0;

	long i=0;
	while (s0[i] && i<n)
	{
		if (s0[i] < s1[i]) return -1;
		if (s0[i] > s1[i]) return 1;
		if (!s1[i]) return 1;
		i++;
	}

	if (i==n) return 0;

	if (!s1[i]) return 0;
	return -1;
}

inline BOOL EndsWith
(
	const char *s0,
	const char *sub_str,
	BOOL case_sensitive=TRUE
)
{
	if (s0 == 0)
		return FALSE;

	if (sub_str == 0)
		return FALSE;

	int n0 = StrLen (s0);
	int n1 = StrLen (sub_str);

	if (n0 < n1)
		return FALSE;

	return IsEqualTo (&s0[n0-n1],sub_str,case_sensitive,n1);
}

inline const char *StrStr 
(
    const char *s0, 
    const char *sub_str, 
    BOOL case_sensitive=TRUE, 
    BOOL whole_word=FALSE,
	int max_len = -1
)
{
	if (!s0)
		return 0;

	if (!sub_str)
		return 0;

	int n;

	int len0 = StrLen (s0);
	int len1 = StrLen (sub_str);

	if (len0 < len1)
		return 0;

	if (max_len == -1)
		n = len0 - len1;
	else
		n = max_len - len1;

    int n_sub_str = StrLen (sub_str);

    for (int i=0; i<=n; i++)
    {
        if (IsEqualTo (&s0[i],sub_str,case_sensitive,n_sub_str))
        {
            if (whole_word)
            {
                if (IsEndOfWordChar (s0[i+n_sub_str]))
				{
					if ((i && IsEndOfWordChar (s0[i-1])) || i == 0)
						return &s0[i];

					continue;
				}
                else
                    continue;
            }
            return &s0[i];
        }
    }

    return 0;
}

inline const char *StrMem
(
    const char *s0, 
    const char *sub_str, 
	int max_len,
    BOOL case_sensitive=TRUE, 
    BOOL whole_word=FALSE
)
{
	int n=max_len;

    int n_sub_str = StrLen (sub_str);

    for (int i=0; i<=n; i++)
    {
        if (IsEqualTo (&s0[i],sub_str,case_sensitive,n_sub_str))
        {
            if (whole_word)
            {
                if (IsEndOfWordChar (s0[i+n_sub_str]))
				{
					if ((i && IsEndOfWordChar (s0[i-1])) || i == 0)
						return &s0[i];

					continue;
				}
                else
                    continue;
            }
            return &s0[i];
        }
    }

    return 0;
}

inline const char *StrRStr 
(
	const char *s0, 
	int idx1, 
	const char *sub_str,
	BOOL case_sensitive=TRUE,
	BOOL whole_word = FALSE)
{
	int n = StrLen (sub_str);
	
	while (idx1 >= n)
	{
		if (IsEqualTo (&s0[idx1-n],sub_str,case_sensitive,n))
		{
			if (whole_word)
			{
                if (IsEndOfWordChar (s0[idx1]))
                    return &s0[idx1-n];
			}
			else
			{
				return &s0[idx1-n];
			}
		}

		idx1--;
	}

	return 0;
}

inline void MemSet (void *buffer, const unsigned char byte, const int bytes)
{
	if (buffer == 0)
		return;

	// Assign bytes in sets of sizeof(int)
	union 
	{
		unsigned char c[sizeof(int)];
		unsigned int  i;
	} u;

    unsigned int i;
	for (i=0;i<sizeof(int);i++)	u.c[i] = byte;

	unsigned int n = bytes/sizeof(int);

	unsigned int *ibuffer = (unsigned int *) buffer;

	for (i=0;i<n;i++) { *ibuffer = u.i; ibuffer++; }

	// Assign the remaining bytes individually
	unsigned char *cbuffer = (unsigned char *) ibuffer;
	n = bytes%sizeof(int);
	for (i=0;i<n;i++) { *cbuffer = byte; cbuffer++; }
}

inline BOOL IsLessThan (const char *c0, const char *c1, BOOL case_sensitive=FALSE, int count=0)
{
	if (c0 == 0 && c1)
		return TRUE;

	if (c1 == 0 && c0)
		return FALSE;

	if (c0 == 0 && c1 == 0)
		return FALSE;

	int i=0;
	while (1)
	{
		if (!c0[i])
		{
			if (c1[i]) return TRUE;
			return FALSE;
		}

		if (!c1[i]) 
			return FALSE;

		if (case_sensitive==FALSE) 
		{
			if (upper_case (c0[i]) > upper_case(c1[i])) 
				return FALSE;
			if (upper_case (c0[i]) < upper_case(c1[i])) 
				return TRUE;
		}
		else
		{
			if (c0[i] > c1[i]) 
				return FALSE;
			if (c0[i] < c1[i]) 
				return TRUE;
		}

		i++;

		if (count && i==count)
			return FALSE; // equal to
	}
}

inline BOOL IsGreaterThan (const char *c0, const char *c1, BOOL case_sensitive=FALSE, int count=0)
{
	if (c0 == 0 && c1)
		return FALSE;

	if (c1 == 0 && c0)
		return TRUE;

	if (c0 == 0 && c1 == 0)
		return FALSE;

	int i=0;
	while (1)
	{
		if (!c1[i]) 
		{
			if (!c0[i]) 
				return FALSE;
			return TRUE;
		}

		if (!c0[i]) 
			return FALSE;

		if (case_sensitive==FALSE) 
		{
			if (upper_case (c0[i]) < upper_case (c1[i])) return FALSE;
			if (upper_case (c0[i]) > upper_case (c1[i])) return TRUE;
		}
		else
		{
			if (c0[i] < c1[i]) 
				return FALSE;
			if (c0[i] > c1[i]) 
				return TRUE;
		}

		i++;

		if (count && i==count)
			return FALSE; // equal to
	}
}

inline BOOL IsLessThan (FILETIME &ft0, FILETIME &ft1)
{
	if (ft0.dwHighDateTime > ft1.dwHighDateTime)
		return FALSE;

	if (ft0.dwHighDateTime < ft1.dwHighDateTime)
		return TRUE;

	if (ft0.dwLowDateTime > ft1.dwLowDateTime)
		return FALSE;

	if (ft0.dwLowDateTime < ft1.dwLowDateTime)
		return TRUE;

	return FALSE;
}

inline BOOL IsGreaterThan (FILETIME &ft0, FILETIME &ft1)
{
	if (ft0.dwHighDateTime < ft1.dwHighDateTime)
		return FALSE;

	if (ft0.dwHighDateTime > ft1.dwHighDateTime)
		return TRUE;

	if (ft0.dwLowDateTime < ft1.dwLowDateTime)
		return FALSE;

	if (ft0.dwLowDateTime > ft1.dwLowDateTime)
		return TRUE;

	return FALSE;
}

inline BOOL IsLessEqualTo (FILETIME &ft0, FILETIME &ft1)
{
	if (ft0.dwHighDateTime > ft1.dwHighDateTime)
		return FALSE;

	if (ft0.dwHighDateTime < ft1.dwHighDateTime)
		return FALSE;

	if (ft0.dwLowDateTime > ft1.dwLowDateTime)
		return FALSE;

	if (ft0.dwLowDateTime < ft1.dwLowDateTime)
		return FALSE;

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////
// Eliminate leading/trailing/extra whitespace characters from a string     
//
inline void trim (char *s, BOOL leading/*=TRUE*/, BOOL extra/*=TRUE*/, BOOL trailing/*=TRUE*/)
{
    int i,j,k,n=StrLen (s);

    // Terminate the string at the first CR or LF character
    for (i=0;i<n;i++)
    {
        if (s[i] == 0x0d || s[i] == 0x0a)
        {
            s[i] = 0;
            n = i+1;
            break;
        }
    }

    // Eliminate leading spaces
	if (leading == TRUE)
	{
		for (i=0;i<n;i++)
		{
			if (!IsWhiteSpaceChar (s[i]))
				break;
		}

		k=i;
		for (j=0;j<n-k;j++)
			s[j] = s[i++];

		s[j] = 0;

		n -= k;  // new length of the string
	}

    // Eliminate trailing spaces
	if (trailing == TRUE)
	{
		for (i=n-1;i>0;i--)
		{
			if (!IsWhiteSpaceChar (s[i]))
				break;
		}

		s[i+1] = 0;

		n = i+1;   // new length of the string
	}

    // Eliminate extra spaces
	if (extra == TRUE)
	{
		if (n>1)
		{
			i=1;
			for (j=1;j<n;j++)
			{
				if (IsWhiteSpaceChar (s[j]) && IsWhiteSpaceChar (s[j-1]))
					continue;

				s[i++] = s[j];
			}
			s[i] = 0;
		}
	}
}

inline BOOL IsHexChar (char c)
{
	if (c >= '0' && c <= '9') 
		return TRUE;

	if (c >= 'A' && c <= 'F')
		return TRUE;

	if (c >= 'a' && c <= 'f')
		return TRUE;

	return FALSE;
}

inline BOOL LookForItem 
(
	int &idx,
	const char *item,
	const char *buffer, 
	int buffer_sz, 
	const char *term_search_str, 
	BOOL case_sensitive = FALSE
)
{
	idx = 0;

	int item_len = StrLen (item);
	int term_search_str_len = StrLen (term_search_str);

    int i;
	for (i=0; i<=buffer_sz-item_len; i++)
	{
		if (IsEqualTo (buffer+i,item,case_sensitive,item_len) == TRUE)
		{
			idx = i;
			return TRUE;
		}

		if (i <= buffer_sz-term_search_str_len)
		{
			if (IsEqualTo (buffer+i,term_search_str,case_sensitive,term_search_str_len) == TRUE)
			{
				idx = i + term_search_str_len;
				return FALSE;
			}
		}
	}

	idx = i+buffer_sz;
	return FALSE;
}


// [WS]HHHHHHHH[WS][NULL] 
BOOL ValidateIsU4Hex (const char *data, DWORD *u4=0);

// [WS]HHHH-HHHH[WS][NULL] 
BOOL ValidateIsSeparatedU4Hex (const char *data, DWORD *u4=0);

// [WS]HHHHHHHHHHHHHHHH[WS][NULL] - 64 bit (8 byte) hex
BOOL ValidateIsU8Hex (const char *data, __int64 *u8=0);


// Defined in the MISC.CPP module
class MCTRL_EXPORT STRING 
{
public:


	STRING (void);
	STRING (const char *);
	STRING (const STRING &);	
	//STRING (STRING &);
	STRING (unsigned short);
	STRING (unsigned long);
	STRING (long);
	STRING (double);
	~STRING (void);
	STRING(const wchar_t *);

	BOOL Error ();

	///////////////////////////////////////////////////////////////////////////////////////
	// START LEAK DETECTION
protected:
	static STRING **m_ring_buf;
	static int m_ring_buf_sz;
	static int m_ring_buf_idx;
	static int m_break_at_ring_buf_idx;
	
public:
	static BOOL StartLeakDetection (int ring_buf_sz);
	static BOOL StopLeakDetection (void);
	static STRING *GetNextLeak (int &idx);
	static BOOL BreakAtLeak (int idx);
	void UPDATE_LEAK_DETECTION (void);
	// END LEAK DETECTION
	///////////////////////////////////////////////////////////////////////////////////////

	inline long GetStorageSizeBytes (void) const
	{
		return StrLen (s)+1;
	}

	inline long SaveToBuffer (void *buffer) const
	{
		char *b = (char *) buffer;
		long i=0;
		while (s[i])
		{
			b[i] = s[i];
			i++;
		}

		b[i] = 0;
		return i+1;
	}

	inline long LoadFromBuffer (const void *buffer)
	{
		*this = (const char *) buffer;
		return StrLen (s) + 1;
	}

private:
	char err;

public:

	inline operator const char* ()
	{ 
		return s; 
	} 

	inline const char *GetStringConst (void) const 
	{
		return s;
	}

	operator short ();
	operator long ();
	operator unsigned short ();
	operator unsigned long ();
	operator double ();

	inline STRING &operator = (STRING &snew)
	{
		if (IsEqualTo (snew,s,TRUE)) return *this;

		int len = StrLen (snew);
		if (StrLen (s) == len) 
			MemMove (s,(const char *)snew,len+1);

		Assign (snew,len);
		return (*this);
	}
		
	STRING &operator += (STRING &);

	inline STRING &operator = (const char *snew)
	{
		if (snew == 0)
		{
			s[0] = 0;
			return *this;
		}

		if (IsEqualTo (snew,s,TRUE)) return *this;

		int len = StrLen (snew);
		if (StrLen (s) == len) 
			MemMove (s,(const char *)snew,len+1);

		if (len == 0)
		{
			s[0] = 0;
			return (*this);
		}

		Assign (snew,len);
		return (*this);
	}

	BOOL Append (const char *s0);
	BOOL AppendMax (const char *s, int n_max);


	STRING &operator += (const char*);
	
	STRING &operator =  (short);
	STRING &operator += (short);

    STRING &operator = (unsigned short);
    STRING &operator += (unsigned short);

	STRING &operator =  (long);
	STRING &operator += (long);

    STRING &operator = (unsigned long);
    STRING &operator += (unsigned long);
	
	STRING &operator =  (UINT);
	STRING &operator += (UINT);

	STRING &operator = (double v);
	STRING &operator += (double v);
	
	STRING &operator = (float v);
	STRING &operator += (float v);

	STRING &operator = (__int64 v);
	STRING &operator += (__int64 v);
	
	inline BOOL operator < (const char *s0)
	{
		return IsLessThan (s,s0,TRUE);
	}

	inline BOOL operator > (const char *s0)
	{
		return IsGreaterThan (s,s0,TRUE);
	}

	BOOL operator >= 	(const char *);
	BOOL operator <= 	(const char *);
	BOOL operator == 	(const char *);
	BOOL operator != 	(const char *);
	
	inline BOOL operator < 	(STRING &s0)
	{
		return IsLessThan (s,s0,TRUE);
	}

	inline BOOL operator > 	(STRING &s0)
	{
		return IsGreaterThan (s,s0,TRUE);
	}

	BOOL operator >= 	(STRING &);
	BOOL operator <= 	(STRING &);
	BOOL operator == 	(STRING &);
	BOOL operator != 	(STRING &);
	
#ifdef __AFX_H__
	STRING (CString &);
	BOOL operator >= 	(CString &);
	BOOL operator <= 	(CString &);
	BOOL operator == 	(CString &);
	BOOL operator != 	(CString &);
	STRING & operator =     (CString &);
	STRING & operator +=     (CString &);
#endif
	
	int len (void);
	BOOL includes (const char *);
	void clear (void);
    void Trim (BOOL leading=TRUE, BOOL extra=TRUE, BOOL trailing=TRUE);
    void LowerCase (void);
    void UpperCase (void);
    void Truncate (int len);
    BOOL HasLen (void) const;
    
	BOOL MakeValidFileName (void);
    BOOL SetValidFileName (const char *path, const char *file_name);
	BOOL LoadFileName (const char *file_name, BOOL include_path, BOOL include_name, BOOL include_ext);
	inline BOOL StripDriveLetter (void);
	inline BOOL StripComputerName (void);
	int ReplaceInvalidFileNameCharacters (char replace_char);
	BOOL RemoveSubString (const char *s0);
	inline BOOL IsRemoteFileSpec (void);
	inline BOOL IncludesFilePathSpec (void);
	BOOL RemoveFileNameExtension (void);

	BOOL RemoveSubDir (STRING *removed_name=0);
	BOOL AppendSubDir (const char *name);
	BOOL MakeIntoAbsolutePath (void);
	int GetSubDirCount (void);
	BOOL SetFilePath (const char *path);
	BOOL RemovePath (void);
	BOOL TrimPath (void);

	BOOL NormalizeHtml (void);
	BOOL ConvertToAbnormalHtml (void);

	long WordCount (void);

    BOOL Assign (const char *s, int n);
	BOOL Append_s (const char *s0,const char *s1=0,const char *s2=0,const char *s3=0,const char *s4=0,const char *s5=0,const char *s6=0,const char *s7=0,const char *s8=0,const char *s9=0);
	BOOL AppendSpaces (int num);
	const char *AppendUpTo (const char *s0, const char *token);

    char LastChar (void);
    
    BOOL Overwrite (int i, const char *c, int width);
    BOOL Insert    (int i, const char *c, int width=0);
    BOOL Remove    (int i, int width);
    STRING *Cut	   (int i, int width = -1); // Default width - to the end of the string

	int Replace (const char *target, const char *replace_with, int max_num=0x7FFF);
	BOOL ReplaceAfterLast(const char *s_find, const char *replace_with);
    
    BOOL AssignAfterSubString (const char *s0, const char *substr, int instance=1);
    BOOL AssignBeforeSubString (const char *s0, const char *substr);
    int FirstDifference (const char *);
    
    int PrevWord (int i);
    int NextWord (int i);
	int GetNextSubStr (int i, STRING &sub_str);

	BOOL Assign (const char *c0, const char *c1=0, const char *c2=0, const char *c3=0, const char *c4=0, const char *c5=0, const char *c6=0, const char *c7=0);
	BOOL ForceSuffix (const char *suffix, BOOL case_sensitive=FALSE);

	inline void CleanUp (void)
	{
		int len = StrLen (s);
		for (int i=0; i<len; i++)
		{
			if (!IsValidASCIIChar (s[i]))
				s[i] = ' ';
		}
	}

	inline int EliminateSpaces (void)
	{
		int len = StrLen (s);
		int n = 0;
		for (int i=0; i<len; i++)
		{
			if (s[i] == ' ')
			{
				int j=i;
				while (s[j])
				{
					s[j] = s[j+1];
					j++;
				}
				len--;
				n++;
			}
		}

		return n;
	}

	inline BOOL Indent (int n)
	{
		char *c = (char *) alloca(n+1);
		MemSet(c,' ',n);
		c[n] = 0;
		return Insert (0,c,n);
	}

	BOOL AppendDelimitedString (const char *c, const char *delimiter, BOOL append_delimeter = TRUE);
	const char *LoadDelimitedString (const char *c, const char *delimiter);

	BOOL AppendQuotedCommaDelimited (const char *s0=0, const char *s1=0, const char *s2=0, const char *s3=0, const char *s4=0, const char *s5=0, const char *s6=0, const char *s7=0, const char *s8=0, const char *s9=0);

	// Returns the number of characters adavanced
	long AssignFromCommaDelimitedQuotedText (const char *c);

	void AppendHexValue (DWORD u4);
	void AppendHexValue (BYTE  u1);
	void AppendSeparatedHexValue (DWORD u4);
	BOOL AppendFILETIME (FILETIME &ft, BOOL bHumanReadable=FALSE);

	BOOL GetAsU8 (unsigned __int64 &u64);
	BOOL GetAsU4 (unsigned long &u32);

	BOOL InsertAtDelimitedPosition (const char *v, int zero_based_idx, char delim_char = 0x09);
	BOOL InsertAtDelimitedPosition (float  v, int zero_based_idx, char delim_char = 0x09);
	BOOL InsertAtDelimitedPosition (double v, int zero_based_idx, char delim_char = 0x09);
	BOOL InsertAtDelimitedPosition (long   v, int zero_based_idx, char delim_char = 0x09);

	int GetArrayOfDelimitedItems (STRING *&array, char delimiter);
	int GetArrayOfDelimitedItemsEx (STRING *&s_array, char delimiter);
	bool AssignDelimitedItem(const char *s, int item, char delimeter);

	// Must be tab delimited values
	double ComputeMean (void);
	double ComputeStDev (void);
	double ComputeMax (void);
	double ComputeMin (void);

	BOOL MatchesNamingConvention (const char *naming_convention, const char *sample=0, BOOL exclude_path=TRUE);
	BOOL GetIndexUsingNamingConvention (const char *naming_convention, long &index, BOOL exclude_path=TRUE);
	BOOL SetIndexUsingNamingConvention (const char *naming_convention, long index, BOOL exclude_path=TRUE);
	BOOL RemoveIndexFromName (const char *naming_convention, BOOL exclude_path=TRUE);
	BOOL ConvertBetweenNamingConventions (const char *naming_convention_source, const char *naming_convention_target, BOOL exclude_path=TRUE);
	BOOL ConvertToFileFindSpecBasedOnNamingConvention (const  char *naming_convention, BOOL exclude_path=TRUE);
	bool SwapInSingleWildcardCharactersFromNamingConvention (const char *naming_convention, STRING *s_swapped=0);

	static inline const char *GetNumberString (WORD i, STRING &s0)
	{
		s0 = i;
		return s0;
	}

	inline BOOL InvertInteger (void);

	BOOL IncludesTag (const char *tag, STRING *value=0, int *istart=0, int *iend=0) const;
	BOOL GetTag (const char *tag, STRING &value) const;
	BOOL SetTag (const char *tag, const char *value);

private:
	char *s;
};

inline BOOL get_BYTE_hex (const char *s, BYTE &b)
{
	BYTE b0 = 0;

	int i=0;

	if (s[i] >= '0' && s[i] <= '9')
		b0 = (s[i] - '0');
	else if (s[i] >= 'A' && s[i] <= 'F')
		b0 = (s[i] - 'A' + 10);
	else if (s[i] >= 'a' && s[i] <= 'f')
		b0 = (s[i] - 'a' + 10);
	else
		return FALSE;

	b0 = (b0 << 4);

	b = 0;

	i++;
	if (s[i] >= '0' && s[i] <= '9')
		b += (s[i] - '0');
	else if (s[i] >= 'A' && s[i] <= 'F')
		b += (s[i] - 'A' + 10);
	else if (s[i] >= 'a' && s[i] <= 'f')
		b += (s[i] - 'a' + 10);
	else
		return FALSE;

	b = b | b0;

	return TRUE;
}

inline BOOL get_WORD_hex (const char *s, WORD &u2)
{
	BYTE b0 = 0;
	BYTE b1 = 0;

	if (get_BYTE_hex (s,b1) == FALSE)
		return FALSE;

	if (get_BYTE_hex (s+2,b0) == FALSE)
		return FALSE;

	u2 = b0 + ((WORD)b1 << 8);

	return TRUE;
}

inline BOOL get_DWORD_hex (const char *s, DWORD &u4)
{
	WORD u0 = 0;
	WORD u1 = 0;

	if (get_WORD_hex (s,u1) == FALSE)
		return FALSE;

	if (get_WORD_hex (s+4,u0) == FALSE)
		return FALSE;

	u4 =  u0 + ((DWORD)u1 << 16);

	return TRUE;
}

inline BOOL STRING::StripDriveLetter (void)
{
	if (!s)
		return FALSE;

	if ((s[0] >= 'a' && s[0] <= 'z') || (s[0] >= 'A' && s[0] <= 'Z'))
	{
		if (s[1] == ':')
			return Remove (0,2);
	}

	return FALSE;
}

inline BOOL STRING::StripComputerName (void)
{
	if (!s)
		return FALSE;

	if (s[0] != '\\')
		return FALSE;

	if (s[1] != '\\')
		return FALSE;

	int i = 2;
	while (s[i] && s[i] != '\\') i++;

	if (!s[i])
		return FALSE;

	return Remove (0,i);
}

inline BOOL STRING::InvertInteger (void)
{
	long i;
	if (sscanf_s (s,"%ld",&i) == 1)
	{
		*this = (-i);
		return TRUE;
	}

	return FALSE;

}

inline BOOL STRING::IsRemoteFileSpec (void)
{
	if (!s)
		return FALSE;

	if (IsEqualTo (s,"http://",TRUE))
		return TRUE;

	if (IsEqualTo (s,"ftp://",TRUE))
		return TRUE;

	return FALSE;
}

inline BOOL STRING::IncludesFilePathSpec (void)
{
	if (!s)
		return FALSE;

	if (s[0] == '\\')
		return TRUE;

	if ((s[0] >= 'a' && s[0] <= 'z') ||
		(s[0] >= 'A' && s[0] <= 'Z') )
	{
		if (s[1] == ':' && s[2] == '\\')
			return TRUE;
	}

	return FALSE;
}

inline BOOL StringToU8 (const char *s, unsigned __int64 &u64)
{
	long i=0;
	while (IsWhiteSpaceChar (s[i]))
		i++;

	u64 = 0;

	long i0 = i;
	while (s[i] >= '0' && s[i] <= '9')
		i++;

	unsigned __int64 f = 0;

	for (long j=i-1; j>=i0; j--)
	{
		switch ((i-1)-j)
		{
		case 0:  f = 1; break;
		case 1:  f = 10; break;
		case 2:  f = 100; break;
		case 3:  f = 1000; break;
		case 4:  f = 10000; break;
		case 5:  f = 100000; break;
		case 6:  f = 1000000; break;
		case 7:  f = 10000000; break;
		case 8:  f = 100000000; break;
		case 9:  f = 1000000000; break;
		case 10: f = 10000000000; break;
		case 11: f = 100000000000; break;
		case 12: f = 1000000000000; break;
		case 13: f = 10000000000000; break;
		case 14: f = 100000000000000; break;
		case 15: f = 1000000000000000; break;
		case 16: f = 10000000000000000; break;
		case 17: f = 100000000000000000; break;
		case 18: f = 1000000000000000000; break;
		case 19: f = 10000000000000000000; break;
		}


		if (f == 0)
			return FALSE;

		u64 += (s[j]-'0') * f;
	}

	return TRUE;
}

inline BOOL StringToU4 (const char *s, unsigned long &u32)
{
	long i=0;
	while (IsWhiteSpaceChar (s[i]))
		i++;

	u32 = 0;

	long i0 = i;
	while (s[i] >= '0' && s[i] <= '9')
		i++;

	unsigned long f = 0;

	for (long j=i-1; j>=i0; j--)
	{
		switch ((i-1)-j)
		{
		case 0:  f = 1; break;
		case 1:  f = 10; break;
		case 2:  f = 100; break;
		case 3:  f = 1000; break;
		case 4:  f = 10000; break;
		case 5:  f = 100000; break;
		case 6:  f = 1000000; break;
		case 7:  f = 10000000; break;
		case 8:  f = 100000000; break;
		case 9:  f = 1000000000; break;
		}


		if (f == 0)
			return FALSE;

		u32 += (s[j]-'0') * f;
	}

	return TRUE;
}

inline BOOL STRING::GetAsU8 (unsigned __int64 &u8)
{
	if (!s)
		return FALSE;

	return StringToU8 (s,u8);
}

inline BOOL STRING::GetAsU4 (unsigned long &u4)
{
	if (!s)
		return FALSE;

	return StringToU4 (s,u4);
}

DWORD CreateTemporaryFilename (const char *path, const char *prefix, STRING &file_name);

inline const char *GetCommaDelimitedVariableName (const char *c, STRING &name)
{
	while (IsWhiteSpaceChar (*c)) c++;

	const char *c0 = c;
	long n = 0;
	if (IsNameStartChar(*c)) { n++; c++; }
	while (IsNameChar(*c)) { n++; c++; }

	while (IsWhiteSpaceChar (*c)) c++;

	if (*c != ','  &&
		*c != 0    && 
		*c != 0x0d &&
		*c != 0x0a)
		return 0;

	name.Assign (c0,(int)n);

	if (*c == 0 ||
		*c == 0x0d ||
		*c == 0x0a)
		return 0;

	return c+1;
}

inline const char *GetTabDelimitedVariableName (const char *c, STRING &name)
{
	while (IsWhiteSpaceChar (*c) && *c != '\t') c++;

	const char *c0 = c;
	long n = 0;
	if (IsNameStartChar(*c)) { n++; c++; }
	while (IsNameChar(*c)) { n++; c++; }

	while (IsWhiteSpaceChar (*c) && *c != '\t') c++;

	if (*c != '\t'  &&
		*c != 0    && 
		*c != 0x0d &&
		*c != 0x0a)
		return 0;

	name.Assign (c0,(int)n);

	if (*c == 0 ||
		*c == 0x0d ||
		*c == 0x0a)
		return 0;

	return c+1;
}

inline const char *GetTabDelimitedName (const char *c, STRING &name)
{
	while (IsWhiteSpaceChar (*c) && *c != '\t') c++;

	const char *c0 = c;
	long n = 0;
	if (IsNameStartChar(*c)) { n++; c++; }
	while (IsNameChar(*c) || *c == ' ') { n++; c++; }

	if (*c != '\t'  &&
		*c != 0    && 
		*c != 0x0d &&
		*c != 0x0a)
		return 0;

	name.Assign (c0,(int)n);

	if (*c == 0 ||
		*c == 0x0d ||
		*c == 0x0a)
		return 0;

	return c+1;
}



inline BOOL ReadParameterizedValue_float (const char *c, const char *name, float &v)
{
	c = StrStr (c,name,TRUE,TRUE);
	if (c == 0)
		return FALSE;

	c += StrLen (name);
	while (IsWhiteSpaceChar (*c)) c++;
	if (*c != '=')
		return FALSE;
	c++;
	while (IsWhiteSpaceChar (*c)) c++;

	if (sscanf_s (c,"%f",&v) == 1)
		return TRUE;

	return FALSE;
}


inline BOOL matches_naming_convention 
(
	const char *s, 
	const char *naming_convention, 
	int &idx_number,
	int *ret_n_number = 0,
	const char *sample_name = 0,
	BOOL exclude_path = TRUE,
	int *idx_of_single_character_wildcards = 0,
	int n_alloc_sz_idx_of_single_character_wildcards = 0
)
{
	int offset=0;
	int i_idx_of_single_character_wildcards = 0;

	if (exclude_path)
	{
		const char *c = strrchr (s,'\\');
		if (c)
		{
			offset = (int)(1+c-s);
			s = c+1;
		}
	}

	int i,n,i_nc,n_nc,n_nc_number,n_number=0;

	if (ret_n_number)
		*ret_n_number = 0;

	idx_number = -1;
	
	n = StrLen (s);
	n_nc = StrLen (naming_convention);
	n_nc_number = 0;

	for (i=0; i<n_nc; i++)
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

	if (n < n_nc)
		return FALSE;

	i_nc = 0;
	i = 0;

	// Special case, wildcard and end of name match
	while (naming_convention[0] == '*')
	{
		// No wildcard after first char in naming convention
		if (StrStr (&naming_convention[1],"*") == 0)
		{
			i_nc = 1;
			i = n - n_nc + 1;

			while (s[i])
			{
				if (s[i] == naming_convention[i_nc])
				{
					if (s[i] >= '0' && s[i] <= '9')
					{
						if (idx_number == -1)
						{
							idx_number = i+offset;
						}

						if (ret_n_number)
							(*ret_n_number)++;
					}

					i++; i_nc++;
					continue;
				}

				if (s[i] != 0 && naming_convention[i_nc] == '?')
				{
						if (idx_of_single_character_wildcards && i_idx_of_single_character_wildcards < n_alloc_sz_idx_of_single_character_wildcards)
							idx_of_single_character_wildcards[i_idx_of_single_character_wildcards++] = i;

						i++; i_nc++;
						continue;				
				}

				if (s[i] >= '0' && s[i] <= '9')
				{
					if (naming_convention[i_nc] >= '0' && naming_convention[i_nc] <= '9')
					{
						if (idx_number == -1)
						{
							idx_number = i+offset;
						}

						if (ret_n_number)
							(*ret_n_number)++;

						i++; i_nc++;
						continue;
					}

					return FALSE;
				}

				return FALSE;
			}
		}
		else
			break;

		goto MatchConfirmed;
	}

	while (1)
	{
		if (naming_convention[i_nc] == '*')
		{
			if (i_nc+1 == n_nc)
				return TRUE;

			if (s[i] != naming_convention[i_nc+1])
			{
				i++;
			
				if (i == n)
					return FALSE;

				continue;
			}

			i_nc += 2;
			i++;
			if (i_nc == n_nc)
			{
				if (i == n)
					break;
				else
					return FALSE;
			}

			if (i >= n)
				return FALSE;

			if (i_nc >= n_nc)
				return FALSE;
		}
		else
		{
			if (s[i] >= '0' && s[i] <= '9')
			{
				if (idx_number == -1)
				{
					idx_number = i+offset;
				}

				n_number++;
				if (ret_n_number)
					(*ret_n_number)++;
			}

			if (IsSameCharCaseInsensitive (s[i],naming_convention[i_nc]) == FALSE)  
			{
				if (naming_convention[i_nc] >= '0' && naming_convention[i_nc] <= '9')
				{
					if (s[i] >= '0' && s[i] <= '9')
					{
						i++;
						i_nc++;
						continue;
					}
				}
				else
				if (n_nc_number == 1) // single number means use as many numbers as necessary
				{
					if (s[i] >= '0' && s[i] <= '9')
					{
						i++; // advance the name but not the naming convention
						continue;
					}
				}

				return FALSE;
			}

			i++;
			i_nc++;

			if (i == n && i_nc == n_nc)
				break;

			if (i >= n)
				return FALSE;

			if (i_nc >= n_nc)
				return FALSE;
		}
	}

MatchConfirmed:

	if (!sample_name)
		return TRUE;

	for (i=0; i<idx_number; i++)
	{
		if (s[i] != sample_name[i])
			return FALSE;
	}

	i += n_number;
	while (1)
	{
		if (s[i] != sample_name[i])
			return FALSE;

		if (s[i] == 0)
			return TRUE;

		i++;
	}
}


#define GET_LAST_ERROR_STRING(ERR_MSG) \
{\
					STRING s;\
					LPVOID lpMsgBuf;\
					FormatMessage( \
						FORMAT_MESSAGE_ALLOCATE_BUFFER | \
						FORMAT_MESSAGE_FROM_SYSTEM | \
						FORMAT_MESSAGE_IGNORE_INSERTS,\
						NULL,\
						GetLastError(),\
						MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), /* Default language */ \
						(LPTSTR) &lpMsgBuf,\
						0,\
						NULL \
					);\
					s = (LPCSTR)lpMsgBuf;\
					LocalFree( lpMsgBuf );\
					s.Replace ("\r","");\
					s.Replace ("\n","");\
\
					ERR_MSG = (const char *)s;\
}


}; // namespace WorkbenchLib
#endif
