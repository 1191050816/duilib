//д��־����
#pragma once

#include <Windows.h>
#include "StringTools.h"

using namespace std;

class CWriteLogFile
{
public:
	//pszLogFileName��־�ļ�����,nLogFileMaxSize-��־�ļ�����С(KB),Ĭ��1024KB,������Сʱ�´ε��ù��캯��ʱ��ɾ����־�ļ�
	CWriteLogFile(const wstring& strLogFileName,int nLogFileMaxSize = 1024);
	CWriteLogFile();
	virtual ~CWriteLogFile();
	
	//pszLogFileName��־�ļ�����,nLogFileMaxSize-��־�ļ�����С(KB),Ĭ��1024KB,������Сʱ�´ε��ù��캯��ʱ��ɾ����־�ļ�
	void SetLogFilePathName(const wstring& strLogFileName,int nLogFileMaxSize = 1024);
	wstring GetLogFilePathName();	
	//�����Ƿ�д����־�ļ���
	void SetWriteLog(bool bIsWrite);	
	//lpszCodeFileʹ�ú�__FILEW__, nCodeLineʹ�ú�__LINE__
	void WriteLogPrintW(LPCWSTR lpszCodeFile,const int& nCodeLine,LPCWSTR format, ...);
	//lpszCodeFileʹ�ú�__FILEW__, nCodeLineʹ�ú�__LINE__
	void WriteLogW(LPCWSTR lpszCodeFile,const int& nCodeLine,LPCWSTR lpszLog);

protected:
	//��ȡ��־�ļ���ǰ��С
	DWORD GetLogFileCurSize(const wstring& strLogFileName);

private:
	wstring m_wstrFileName;
	DWORD m_dwLogFileMaxSize;		//��־�ļ�����С(KB)
	bool m_bWrite;

	CRITICAL_SECTION    m_CritSec;
};