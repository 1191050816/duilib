#include "zipx.h"
#include <stdio.h>

void PathAddBackslash2(xstring &strFilePath)
{
	int nLen = strFilePath.size();
	if(nLen > 0)
	{
		if(strFilePath[nLen-1] != _T('\\'))
		{
			strFilePath += _T('\\');
		}
	}
}

// �ж��ļ��Ƿ����
bool IsFileExist(const xstring& strFilePath)
{
	DWORD dwAttrib = GetFileAttributes(strFilePath.c_str());
	return ((INVALID_FILE_ATTRIBUTES != dwAttrib) && (0 == (dwAttrib & FILE_ATTRIBUTE_DIRECTORY)));
}
// �ж��ļ����Ƿ����
bool IsDirExist(const xstring & strFilePath)
{
	DWORD dwAttrib = GetFileAttributes(strFilePath.c_str());
	return ((INVALID_FILE_ATTRIBUTES != dwAttrib) && (0 != (dwAttrib & FILE_ATTRIBUTE_DIRECTORY)));
}
// �ж��ļ����ļ����Ƿ����
bool IsPathExist(const xstring & strFilePath)
{
	DWORD dwAttrib = GetFileAttributes(strFilePath.c_str());
	return (INVALID_FILE_ATTRIBUTES != dwAttrib);
}

// ��������(��˵�����һ��)������ע1
bool IsPathExistEx(const xstring & strFilePath)
{
	WIN32_FILE_ATTRIBUTE_DATA attrs = { 0 };
	return (0 != GetFileAttributesEx(strFilePath.c_str(), GetFileExInfoStandard, &attrs));
}


CZipx::CZipx(void)
{
}

CZipx::~CZipx(void)
{
}

bool CZipx::ZipFile(LPCTSTR lpszSrcFile, LPCTSTR lpszDstFile, const char *pszPassword)
{
	LPCTSTR pszTempDst = lpszDstFile;
	if(lpszDstFile == NULL)
	{
		pszTempDst = lpszSrcFile;
	}

	//Ŀ���ļ��Ƿ����
	if(!IsFileExist(lpszSrcFile))
	{
		return false;
	}

	// ȡ���ļ���
	LPCTSTR pszFind = _tcsrchr(pszTempDst, _T('\\'));
	xstring strFileName = pszFind;
	xstring strFileDir(pszTempDst, pszFind-pszTempDst);
	if(lpszDstFile == NULL)
	{
		//����Ŀ���ļ���׺
		string::size_type nFind = strFileName.find_last_of(_T('.'));
		if(nFind != string::npos)
		{
			strFileName = strFileName.substr(0, nFind) + _T(".zip");
		}
	}

	//�����ļ�Ŀ¼
	CreateDir(strFileDir.c_str());

	HZIP hz = CreateZip(lpszDstFile, pszPassword);
	ZipAdd(hz, strFileName.c_str(), lpszSrcFile);
	CloseZip(hz);
}

//Ŀ���ļ�����ָ��
BOOL CZipx::ZipFiles(LPCTSTR lpszSrcDir, LPCTSTR lpszDstFile, const char *pszPassword) 
{ 
	if(lpszDstFile == NULL || lpszSrcDir == NULL)
	{
		return FALSE;
	}

	//���ԴĿ¼�Ƿ����
	if(!IsDirExist(lpszSrcDir))
	{
		return false;
	}

	// ȡ���ļ���
	LPCTSTR pszFind = _tcsrchr(lpszDstFile, _T('\\'));
	if(pszFind == NULL)
	{
		return FALSE;
	}

	xstring strDstFileName = pszFind+1;
	xstring strDstFileDir(lpszDstFile, pszFind - lpszDstFile+1);
	if(strDstFileName.size() == 0)
	{
		return FALSE;
	}	

	//�����ļ�Ŀ¼
	m_strDstFilePath = lpszDstFile;
	m_strSrcFileDir = lpszSrcDir;
	CreateDir(strDstFileDir.c_str());

	//����ZIP�ļ� 
	HZIP hz=CreateZip(lpszDstFile, pszPassword); 
	if(hz == 0) 
	{ 
		//����Zip�ļ�ʧ�� 
		return FALSE; 
	} 

	//�ݹ��ļ���,����ȡ���ļ�����ZIP�ļ� 	
	AddFileFromDir(hz, m_strSrcFileDir); 

	//�ر�ZIP�ļ� 
	CloseZip(hz); 

	if (INVALID_FILE_ATTRIBUTES == GetFileAttributes(lpszDstFile)) 
	{ 
		//ѹ��ʧ��(δ����ѹ������ļ�) 
		return FALSE; 
	} 
	return TRUE; 
} 

void CZipx::AddFileFromDir(HZIP hz, const xstring &strFileDir) 
{ 
	WIN32_FIND_DATA wfd = {0}; 
	xstring strDir = strFileDir; 
	PathAddBackslash2(strDir);

	HANDLE hFind = FindFirstFile((strDir+_T("*.*")).c_str(), &wfd); 

	if (hFind == INVALID_HANDLE_VALUE)
	{
		return;
	}

	do
	{
		if (wfd.cFileName[0] == _T('.'))
		{
			if (wfd.cFileName[1] == _T('\0') ||
				(wfd.cFileName[1] == _T('.') &&	wfd.cFileName[2] == _T('\0')))
			{
				continue;
			}
		}

		xstring strFilePath = strDir + wfd.cFileName;

		//�����һ����Ŀ¼���õݹ��������һ���� 
		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			xstring subPath; 
			GetRelativePath(strFilePath, subPath); 

			ZipAddFolder(hz, subPath.c_str()); 
			AddFileFromDir(hz, strFilePath); 
		} 
		else
		{ 
			//�ų���ѹ���ĵ�
			if(_tcsicmp(m_strDstFilePath.c_str(), strFilePath.c_str()) == 0)
			{
				continue;
			}

			//��ʾ��ǰ���ʵ��ļ�(����·��) 
			xstring subPath; 
			GetRelativePath(strFilePath, subPath); 

			//���ļ���ӵ�ZIP�ļ�
			ZipAdd(hz, subPath.c_str(), strFilePath.c_str()); 
		} 
	}
	while( 0 != FindNextFile(hFind, &wfd));

	FindClose(hFind);
} 

bool CZipx::UnZipFile(LPCTSTR lpszSrcFile, LPCTSTR lpszDstDir, const char *pszPassword)
{
	if(lpszSrcFile == NULL || lpszDstDir == NULL)
		return false;

	//���Դ�ļ��Ƿ����
	if(!IsFileExist(lpszSrcFile))
	{
		return false;
	}

	CreateDir(lpszDstDir);

	//��ZIP�ļ� 
	HZIP hz = OpenZip(lpszSrcFile, pszPassword); 
	if(hz == 0) 
	{ 
		//��Zip�ļ�ʧ�� 
		return false; 
	} 

	ZRESULT zr = SetUnzipBaseDir(hz, lpszDstDir); 
	if(zr != ZR_OK) 
	{ 
		//��Zip�ļ�ʧ�� 
		CloseZip(hz); 
		return FALSE;       
	} 

	ZIPENTRY ze;
	zr=GetZipItem(hz, -1, &ze); 
	if(zr != ZR_OK) 
	{ 
		//��ȡZip�ļ�����ʧ�� 
		CloseZip(hz); 
		return FALSE;       
	} 

	int nNumItems = ze.index; 
	for (int i=0; i<nNumItems; i++) 
	{ 
		zr = GetZipItem(hz, i, &ze); 
		zr = UnzipItem(hz, i, ze.name); 

		if(zr != ZR_OK) 
		{ 
			//��ȡZip�ļ�����ʧ�� 
			CloseZip(hz); 
			return false;       
		} 
	} 

	CloseZip(hz); 

	return true;
}

bool CZipx::UnZipFile(LPCTSTR lpszSrcFile, LPCTSTR lpszFileName, LPCTSTR lpszDstDir, const char *pszPassword)
{
	if(lpszSrcFile == NULL || lpszDstDir == NULL)
		return false;

	//���Դ�ļ��Ƿ����
	if(!IsFileExist(lpszSrcFile))
	{
		return false;
	}

	CreateDir(lpszDstDir);

	//��ZIP�ļ� 
	HZIP hz = OpenZip(lpszSrcFile, pszPassword); 
	if(hz == 0) 
	{ 
		//��Zip�ļ�ʧ�� 
		return false; 
	} 

	ZRESULT zr = SetUnzipBaseDir(hz, lpszDstDir); 
	if(zr != ZR_OK) 
	{ 
		//��Zip�ļ�ʧ�� 
		CloseZip(hz); 
		return FALSE;       
	} 

	int i = 0; 
	ZIPENTRY ze;
	zr = FindZipItem(hz, lpszFileName, true, &i, &ze);
	if(zr == ZR_OK) 
	{
		zr = UnzipItem(hz, i, ze.name);
	}
	CloseZip(hz);
	 
	return (zr == ZR_OK) ;
}

bool CZipx::UnZipFileFromMem(void* pData,DWORD dwDataSize,LPCTSTR lpszDstDir, const char *pszPassword/* =NULL */)
{
	if(pData == NULL || dwDataSize==0)
		return false;

	CreateDir(lpszDstDir);

	//��ZIP�ļ� 
	HZIP hz = OpenZip(pData,dwDataSize, pszPassword); 
	if(hz == 0) 
	{ 
		//��Zip�ļ�ʧ�� 
		return false; 
	} 

	ZRESULT zr = SetUnzipBaseDir(hz, lpszDstDir); 
	if(zr != ZR_OK) 
	{ 
		//��Zip�ļ�ʧ�� 
		CloseZip(hz); 
		return FALSE;       
	} 

	ZIPENTRY ze;
	zr=GetZipItem(hz, -1, &ze); 
	if(zr != ZR_OK) 
	{ 
		//��ȡZip�ļ�����ʧ�� 
		CloseZip(hz); 
		return FALSE;       
	} 

	int nNumItems = ze.index; 
	for (int i=0; i<nNumItems; i++) 
	{ 
		zr = GetZipItem(hz, i, &ze); 
		zr = UnzipItem(hz, i, ze.name); 

		if(zr != ZR_OK) 
		{ 
			//��ȡZip�ļ�����ʧ�� 
			CloseZip(hz); 
			return false;       
		} 
	} 

	CloseZip(hz); 

	return true;
}

bool CZipx::CreateDir(LPCTSTR lpszDir)
{
	if (IsDirExist(lpszDir))       //���Ŀ¼�Ѵ��ڣ�ֱ�ӷ���
	{
		return TRUE;
	}

	std::xstring strPath;
	TCHAR pszSrc[MAX_PATH] = { 0 };
	_tcscpy_s(pszSrc, lpszDir);
	TCHAR *pszContent;
	TCHAR *ptoken = _tcstok_s(pszSrc, _T("\\/"), &pszContent);
	while (ptoken)
	{
		strPath += ptoken;
		strPath += _T("\\");
		if (!IsDirExist(strPath.c_str()))
		{
			//����ʧ��ʱ��Ӧɾ���Ѵ������ϲ�Ŀ¼���˴���
			if (!CreateDirectory(strPath.c_str(), NULL))
			{
				DWORD dw = GetLastError();
				return FALSE;
			}
		}
		ptoken = _tcstok_s(NULL, _T("\\"), &pszContent);
	}
	return TRUE;

}

void CZipx::GetRelativePath(const xstring& strFilePath, xstring& strSubPath) 
{ 
	strSubPath = strFilePath;
	if(_tcsnicmp(strFilePath.c_str(), m_strSrcFileDir.c_str(), m_strSrcFileDir.size()) == 0)
	{
		strSubPath = strFilePath.substr(m_strSrcFileDir.size());
	}
}






