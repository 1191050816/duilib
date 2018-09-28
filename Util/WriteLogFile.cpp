
#include "WriteLogFile.h"
#include <time.h>
#include <io.h>

CWriteLogFile::CWriteLogFile(const wstring& strLogFileName,int nLogFileMaxSize /* = 1024 */)
{
	try
	{
		//��ʼ��ʱ����һ���ļ���С
		int nFileSize = (GetLogFileCurSize(strLogFileName));
		if ( nFileSize>nLogFileMaxSize*1024 || nFileSize==0)
			DeleteFileW(strLogFileName.c_str());
		
		m_wstrFileName = strLogFileName;
		m_hEvent = CreateEvent(NULL,TRUE,TRUE,NULL);	//��Ҫ�ֶ�������/���ź�,��ʼ���ź�

		m_dwLogFileMaxSize = nLogFileMaxSize;
	}
	catch (...)
	{
		if (m_hEvent != INVALID_HANDLE_VALUE)
			CloseHandle(m_hEvent);
		m_hEvent = INVALID_HANDLE_VALUE;
	}
	m_bWrite = true;
}

CWriteLogFile::CWriteLogFile()
{
	try
	{
		m_hEvent = CreateEventA(NULL,TRUE,TRUE,NULL);	//��Ҫ�ֶ�������/���ź�,��ʼ���ź�
	}
	catch (...)
	{
		if (m_hEvent != INVALID_HANDLE_VALUE)
			CloseHandle(m_hEvent);
		m_hEvent = INVALID_HANDLE_VALUE;
	}
	m_bWrite = true;
}

void CWriteLogFile::SetLogFilePathName(const wstring& strLogFileName,int nLogFileMaxSize /* = 1024 */)
{
	//��ʼ��ʱ����һ���ļ���С
	int nFileSize = (GetLogFileCurSize(strLogFileName));
	if ( nFileSize>nLogFileMaxSize*1024 || nFileSize==0)
		DeleteFileW(strLogFileName.c_str());

	m_wstrFileName = strLogFileName;
	m_dwLogFileMaxSize = nLogFileMaxSize;
}

CWriteLogFile::~CWriteLogFile()
{
	if (m_hEvent != INVALID_HANDLE_VALUE)
		CloseHandle(m_hEvent);
	m_hEvent = INVALID_HANDLE_VALUE;
};

DWORD CWriteLogFile::GetLogFileCurSize(const wstring& strLogFileName)
{
	try
	{
		WIN32_FIND_DATAW fileInfo;
		HANDLE hFind;
		DWORD fileSize=0x0;
		hFind = FindFirstFileW(strLogFileName.c_str() ,&fileInfo);
		if(hFind != INVALID_HANDLE_VALUE)
			fileSize = fileInfo.nFileSizeLow;
		FindClose(hFind);
		
		//������һ�ֲ����ļ���С�ķ����ٴλ�ȡ
		FILE* file = NULL;
		_wfopen_s(&file,strLogFileName.c_str(),L"r");	
		if (file)
		{
			long nSize = _filelength(_fileno(file));
			fclose(file);
			if (fileSize != nSize)
				return nSize;
		}
		return fileSize;
	}
	catch (...)
	{
		return 0;
	}
}

void CWriteLogFile::WriteLogW(bool bPrintToConsole,LPCWSTR format, ...)
{
	if (m_bWrite == false)
		return ;

	WaitForSingleObject(m_hEvent,3000);
	if (m_hEvent == INVALID_HANDLE_VALUE)
		return ;
	ResetEvent(m_hEvent);
	try
	{
		if ( GetLogFileCurSize(m_wstrFileName)>m_dwLogFileMaxSize*1024)
			DeleteFileW(m_wstrFileName.c_str());

		WIN32_FILE_ATTRIBUTE_DATA attrs = {0};
		BOOL bHave = GetFileAttributesExW(m_wstrFileName.c_str(), GetFileExInfoStandard, &attrs);

		//���
		FILE* fp = NULL;
		_wfopen_s(&fp,m_wstrFileName.c_str(),L"ab+");	
		if (fp)
		{
			
			if ( !bHave )  
			{  
				// �´�������־�ļ�����д��Unicodeͷ  
				BYTE chUnicodeHead[2] = { 0xff, 0xfe }; // Unicodeͷ  
				fwrite( chUnicodeHead, sizeof(BYTE), sizeof(chUnicodeHead), fp );  
			}  

			//����
			wchar_t sLogOutput[10240] = {0};
			va_list arg_ptr;  
			va_start(arg_ptr,format);
			_vsnwprintf_s(sLogOutput,_countof(sLogOutput),_TRUNCATE, format, arg_ptr);
			va_end(arg_ptr);

			SYSTEMTIME time;  
			::GetLocalTime( &time );  
			fwprintf(fp, L"[%04d-%02d-%02d %02d:%02d:%02d]%s\r\n", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, sLogOutput );  
			fclose( fp );  

			if (bPrintToConsole)
				OutputDebugString(sLogOutput);
		}
	}
	catch (...)
	{}
	SetEvent(m_hEvent);
}

void CWriteLogFile::SetWriteLog(bool bIsWrite)
{
	m_bWrite = bIsWrite;
}