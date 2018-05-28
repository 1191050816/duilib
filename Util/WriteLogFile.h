//д��־����
#pragma once

#include <Windows.h>
#include "StringTools.h"

using namespace std;

class CWriteLogFile
{
public:
	//pszLogFileName��־�ļ�����,nLogFileMaxSize-��־�ļ�����С(KB),Ĭ��1024KB,������Сʱ�´ε��ù��캯��ʱ��ɾ����־�ļ�
	CWriteLogFile(const xstring& strLogFileName,int nLogFileMaxSize = 1024);
	CWriteLogFile();
	virtual ~CWriteLogFile();
	
	//pszLogFileName��־�ļ�����,nLogFileMaxSize-��־�ļ�����С(KB),Ĭ��1024KB,������Сʱ�´ε��ù��캯��ʱ��ɾ����־�ļ�
	void SetLogFilePathName(const xstring& strLogFileName,int nLogFileMaxSize = 1024);
	////д��־ bPrintToConsole-�Ƿ����������̨
	//void WriteLog(bool bPrintToConsole,LPCTSTR format, ...);
	void WriteLogA(bool bPrintToConsole,LPCSTR format, ...);
	void WriteLogW(bool bPrintToConsole,LPCWSTR format, ...);

protected:
	//��ȡ��־�ļ���ǰ��С
	DWORD GetLogFileCurSize(const xstring& strLogFileName);

private:
	HANDLE m_hEvent;
	xstring m_strFileName;
	DWORD m_dwLogFileMaxSize;		//��־�ļ�����С(KB)
};