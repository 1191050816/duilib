#pragma once

#include <Windows.h>
#include <string>
#include <vector>

#pragma warning(disable:4190)
using namespace std;

#ifndef xstring 
#ifdef UNICODE
#define xstring wstring
#else
#define xstring string
#endif
#endif

std::string  UnicodeToAnsi(const std::wstring& strSource);
std::wstring AnsiToUnicode(const std::string& strSource);
std::wstring Utf8ToUnicode(const std::string& strSrouce);
std::string UnicodeToUtf8(const std::wstring& strSource);
std::string AnsiToUtf8(const std::string& strSource);
std::string Utf8ToAnsi(const std::string& strSource);
std::string URLEncodeGB2312(const string& strUtf8/*, char* pBuf, int cbBufLen*/);			//�ո�תΪ'+'
std::string URLEncodeGB2312Forspace(const string& strUtf8/*, char* pBuf, int cbBufLen*/);	//�ո�תΪ'%20'
char* Utf8ToGBK(const char* strSource);
char* GBKToUtf8(const char* strSource);
bool UrlEncode(const char* szSrc, char* pBuf, int cbBufLen, bool bUpperCase);
bool Utf8UrlDecode(const char* szSrc, char* pBuf, int cbBufLen);
bool UrlDecode(const char* szSrc, char* pBuf, int cbBufLen);
std::string EscapeToAnsi(const std::string& strSource);

#ifndef _W
#define _W(x) AnsiToUnicode(x).data()
#endif

#ifndef _A
#define _A(x) UnicodeToAnsi(x).data()
#endif

//�ַ����滻 ���strText�е�����strOld�������滻,��������滻��Ĵ��ٴμ���滻.���罫"1001"�е�"1"�滻Ϊ"1234",��ô����ֵΪ"1234001234"
//strText-ȫ������ strOld-��Ҫ���滻������ strNew-�µ������滻��
xstring StringReplace(const xstring& strText, const xstring& strOld, const xstring& strNew);

inline bool IsGB2312(const unsigned char *pszIn) 
{ 
	if (*pszIn>=0xB0 && *pszIn<=0xF7 && *(pszIn+1)>=0xA0 && *(pszIn+1)<=0xFE) 
		return true; 
	else 
		return false; 
}

inline bool IsGBK(const unsigned char *pszIn) 
{ 
	if (*pszIn>=0x81 && *pszIn<=0xFE && *(pszIn+1)>=0x40 && *(pszIn+1)<=0xFE) 
		return true; 
	else 
		return false; 
}

inline bool IsBIG5(const unsigned char *pszIn) 
{ 
	if (*pszIn>=0x81 && *pszIn<=0xFE && ((*(pszIn+1)>=0x40 && *(pszIn+1)<=0x7E) || (*(pszIn+1)>=0xA1 && *(pszIn+1)<=0xFE)))
		return true; 
	else 
		return false; 
}

inline bool IsValidEnglishChar(const unsigned char *pszIn) 
{
	if((*pszIn>='0' && *pszIn <= '9') || (*pszIn>='a' && *pszIn <= 'z') || (*pszIn>='A' && *pszIn <= 'Z'))
	{
		return true;
	}
	else
		return false;
}

inline bool IsValidName(const unsigned char *pszIn, int nLen) 
{
	for (int i=0; i < nLen; )
	{
		if(IsValidEnglishChar(pszIn+i))
		{
			i++;
			continue;
		}
		else if(((i+1) < nLen) && IsGBK(pszIn+i))
		{
			i += 2;
			continue;
		}
		else
			return false;
	}
	return true;
}

inline bool IsValidQQ(const unsigned char *pszIn, int nLen) 
{
	for (int i=0; i < nLen; )
	{
		if(isdigit(*(pszIn+i)))
		{
			i++;
			continue;
		}
		else
			return false;
	}
	return true;

}

inline void CutChineseChar(char *pszSrc,unsigned int nNeedLen)
{
	if (strlen(pszSrc) >= nNeedLen)
	{
		//�������λ����Ӣ���ַ���Ҳ����һ����Ч�ĺ��֣������λ��һ�����ֶ����һ���ֽ�
		if(!IsValidEnglishChar((const unsigned char *)(pszSrc+nNeedLen-2)) && !IsGBK((const unsigned char *)(pszSrc+nNeedLen-2)))
		{
			*(pszSrc+nNeedLen-3) = '.';
		}
		*(pszSrc+nNeedLen-2) = '.';
		*(pszSrc+nNeedLen-1) = '.';
		*(pszSrc+nNeedLen) = '\0';
	}
}
