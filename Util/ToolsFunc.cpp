#include <winsock2.h>
#include <Windows.h>
#include <time.h>
#include <tchar.h>
#include <CommCtrl.h>
#include <commoncontrols.h>
#include <OleCtl.h>
#include <ShellAPI.h>
#include <TlHelp32.h>
#include <io.h>
#include "ToolsFunc.h"
#pragma comment(lib,"shlwapi.lib")
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

BOOL CreateDir(const xstring& strDir)
{
	if (PathIsDirectory(strDir.c_str()) == FALSE)
		return CreateDirectory(strDir.c_str(),NULL);

	return TRUE;
}

//���е���Ļ����
void CenterWindowToScreen(HWND hWnd)
{
	RECT rcArea;
	RECT rcCenter;

#if WINVER < 0x0500
	::SystemParametersInfo(SPI_GETWORKAREA, NULL, &rcArea, NULL);
#else
	HMONITOR hMonitor = NULL;
	if(hWnd != NULL)
		hMonitor = ::MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
	else
		hMonitor = ::MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);

	MONITORINFO minfo;
	minfo.cbSize = sizeof(MONITORINFO);
	BOOL bResult = ::GetMonitorInfo(hMonitor, &minfo);

	rcArea = minfo.rcWork;
#endif
	::GetWindowRect(hWnd, &rcCenter);
	int DlgWidth = rcCenter.right - rcCenter.left;
	int DlgHeight = rcCenter.bottom - rcCenter.top;

	int xLeft = (rcArea.right-rcArea.left-DlgWidth)/2;
	int yTop = (rcArea.bottom-rcArea.top-DlgHeight)/2;

	::SetWindowPos(hWnd, NULL, xLeft, yTop, -1, -1,SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

//�Զ����д��ڡ�hParent-������,hWnd-Ҫ���еĴ���
void CenterWindowEx(HWND hParent,HWND hWnd)
{
	if (::IsWindow(hWnd) == FALSE)
		return;

	if (hParent == NULL || IsIconic(hParent))
	{
		CenterWindowToScreen(hWnd);
	}
	else
	{
		RECT rcDlg = { 0 };
		RECT rcCenter = { 0 };
		RECT rcArea = { 0 };
		::GetWindowRect(hWnd, &rcDlg);
		::GetWindowRect(hParent, &rcCenter);
		::SystemParametersInfo(SPI_GETWORKAREA, NULL, &rcArea, NULL);

		int DlgWidth = rcDlg.right - rcDlg.left;
		int DlgHeight = rcDlg.bottom - rcDlg.top;

		// Find dialog's upper left based on rcCenter
		int xLeft = (rcCenter.left + rcCenter.right) / 2 - DlgWidth / 2;
		int yTop = (rcCenter.top + rcCenter.bottom) / 2 - DlgHeight / 2;

		// The dialog is outside the screen, move it inside
		if( xLeft < rcArea.left ) xLeft = rcArea.left;
		else if( xLeft + DlgWidth > rcArea.right ) xLeft = rcArea.right - DlgWidth;
		if( yTop < rcArea.top ) yTop = rcArea.top;
		else if( yTop + DlgHeight > rcArea.bottom ) yTop = rcArea.bottom - DlgHeight;
		::SetWindowPos(hWnd, NULL, xLeft, yTop, -1, -1, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
	}
}

std::string GetNumFromStrBuf(const char* pszStrBuf)
{
	std::string strNum="";
	try
	{
		char ch;
		for (unsigned int i=0;i<strlen(pszStrBuf);i++)
		{
			ch = *(pszStrBuf+i);
			if (ch >= '0' && ch<='9')
				strNum.append(1,ch);
		}
	}
	catch (...)
	{}
	return strNum;
}

void PrintfLog(const TCHAR * format, ...)
{
	try
	{
		//ʱ��ǰ׺
		time_t nTime;  
		struct tm *tmDateTime;  
		TCHAR szDateTime[128] = {0};  
		time(&nTime);
		localtime_s(tmDateTime,&nTime); 
		_sntprintf_s(szDateTime,_countof(szDateTime),_TRUNCATE,_T("[%04d-%02d-%02d %02d:%02d:%02d]"),tmDateTime->tm_year+1900,tmDateTime->tm_mon+1,tmDateTime->tm_mday,tmDateTime->tm_hour,tmDateTime->tm_min,tmDateTime->tm_sec);

		//����
		TCHAR szLog[4096] = {0};
		va_list arg_ptr;  
		va_start(arg_ptr, format);
		_vsntprintf_s(szLog,_countof(szLog),_TRUNCATE, format, arg_ptr);
		va_end(arg_ptr);
		TCHAR szText[4224]={0};	//4096+128
		_sntprintf_s(szText,_countof(szText),_TRUNCATE,_T("%s%s\n"),szDateTime,szLog);
		//���
		OutputDebugString(szText);
	}
	catch (...)
	{}
}

BOOL IsPathExist(const TCHAR* szPath)
{
	if (szPath==NULL || _tcslen(szPath)==0)
		return FALSE;

	WIN32_FILE_ATTRIBUTE_DATA attrs = {0};
	return GetFileAttributesEx(szPath, GetFileExInfoStandard, &attrs);
}

BOOL IsPathExistA(const string& strPath)
{
	if ( strPath.empty() )
		return FALSE;

	WIN32_FILE_ATTRIBUTE_DATA attrs = {0};
	return GetFileAttributesExA(strPath.c_str(), GetFileExInfoStandard, &attrs);
}

bool ReadRegString(HKEY hKey,const xstring& strSubKey,const xstring& strKeyName,const DWORD& dwType ,xstring& strValue)
{
	strValue.clear();
	HKEY hKeyHander = NULL ;
	if (RegOpenKeyEx(hKey,strSubKey.c_str(),NULL,KEY_QUERY_VALUE,&hKeyHander) != ERROR_SUCCESS)
		return false;

	TCHAR szData[1024] = {0};
	DWORD dwSize = 1024;
	DWORD dwTp = dwType;
	LSTATUS lret = RegQueryValueEx(hKeyHander,strKeyName.c_str(),NULL,&dwTp,(LPBYTE)szData,&dwSize);
	if (lret == ERROR_SUCCESS)
	{
		if (dwType == REG_SZ)
			strValue = szData;
		else if (dwType==REG_DWORD||dwType==REG_BINARY)
		{
			_sntprintf_s(szData,_countof(szData),_TRUNCATE,_T("%ld"),(long)*(long *)szData);
			strValue = szData;
		}

		RegCloseKey(hKeyHander);
		return true;
	}
	else if (lret == ERROR_FILE_NOT_FOUND)	//��ʧ����ӷ�Wow6432Node������
	{
		if (RegOpenKeyEx(hKey,strSubKey.c_str(),NULL,KEY_QUERY_VALUE | KEY_WOW64_64KEY,&hKeyHander) != ERROR_SUCCESS)
			return false;
		lret = RegQueryValueEx(hKeyHander,strKeyName.c_str(),NULL,&dwTp,(LPBYTE)szData,&dwSize);
		if (lret == ERROR_SUCCESS)
		{
			if (dwType == REG_SZ)
				strValue = szData;
			else if (dwType==REG_DWORD||dwType==REG_BINARY)
			{
				_sntprintf_s(szData,_countof(szData),_TRUNCATE,_T("%ld"),(long)*(long *)szData);
				strValue = szData;
			}

			RegCloseKey(hKeyHander);
			return true;
		}
	}

	RegCloseKey(hKeyHander);
	return false;
}

bool WriteRegValue(HKEY hKey,const xstring& strSubKey,const xstring& strKeyName,const DWORD& dwType ,const BYTE* lpData,DWORD cbData)
{
	HKEY   hKeyHander = NULL;   
	//�ҵ�ϵͳ��������   
	long lRet = ::RegOpenKeyEx(hKey,strSubKey.c_str(),0,KEY_READ|KEY_WRITE,&hKeyHander);
	if(lRet != ERROR_SUCCESS)  
	{
		DWORD dwDisposition;
		lRet = RegCreateKeyEx(hKey,strSubKey.c_str(),0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL, &hKeyHander, &dwDisposition);
	}
	if (lRet == ERROR_SUCCESS)
	{
		lRet = RegSetValueEx(hKeyHander,strKeyName.c_str(),0,dwType,lpData,cbData);
	}
	if (hKeyHander != NULL)
		RegCloseKey(hKeyHander);

	if (lRet == ERROR_SUCCESS)
		return true;

	return false;  
}

bool DeleteRegKeyValue(HKEY hKey,const xstring& strSubKey,const xstring& strKeyName)
{
	HKEY hKeyHandle = NULL;                             
	LONG lRet = RegOpenKeyEx(hKey, strSubKey.c_str(), 0, KEY_QUERY_VALUE|KEY_WRITE, &hKeyHandle );                                
	if( lRet == ERROR_SUCCESS )    
	{
		lRet = RegDeleteValue(hKeyHandle, strKeyName.c_str());
		RegCloseKey(hKeyHandle);
		if (lRet == ERROR_SUCCESS)
		{
			return true;
		}
	}

	return false;
}

bool DeleteRegSubKey(HKEY hKey,const xstring& strSubKey)
{
	LONG lRet = SHDeleteKey(hKey,strSubKey.c_str());  
	if (lRet == ERROR_SUCCESS)
	{
		return true;
	}

	return false;
}

xstring GetAppPath(HMODULE hModul)
{
	//��ʼ��App·��
	TCHAR szAppPath[512] = {0};
	GetModuleFileName(hModul,szAppPath,512);	
	xstring strApp = szAppPath;
	unsigned int nPos = strApp.find_last_of(_T('\\'));
	if (nPos == std::string::npos)
		nPos = strApp.find_last_of(_T('/'));
	if (nPos != std::string::npos)
		strApp = strApp.substr(0,nPos);
	else
		OutputDebugString(_T("��ȡ����·������\n"));

	return strApp;
}


string GetLocalIp()
{
	WSADATA     wsaData; 
	WSAStartup(0x0202, &wsaData);
	//Before calling AddIPAddress we use GetIpAddrTable to get an adapter to which we can add the IP.
	PMIB_IPADDRTABLE pIPAddrTable;
	DWORD dwSize = 0;
	DWORD dwRetVal = 0;

	pIPAddrTable = (MIB_IPADDRTABLE*) malloc( sizeof( MIB_IPADDRTABLE) );

	// Make an initial call to GetIpAddrTable to get the necessary size into the dwSize variable
	if (GetIpAddrTable(pIPAddrTable, &dwSize, 0) == ERROR_INSUFFICIENT_BUFFER) 
	{
		free( pIPAddrTable );
		pIPAddrTable = (MIB_IPADDRTABLE *) malloc ( dwSize );
	}
	else
	{
		free( pIPAddrTable );
		WSACleanup();
		return "";
	}

	// Make a second call to GetIpAddrTable to get the actual data we want
	dwRetVal = GetIpAddrTable( pIPAddrTable, &dwSize, 0 );
	if ( dwRetVal != NO_ERROR ) 
	{
		free( pIPAddrTable );
		WSACleanup();
		return "";
	}

	for (DWORD i=0; i<pIPAddrTable->dwNumEntries; i++)
	{
		PMIB_IPADDRROW prow = &pIPAddrTable->table[i];
		if ( prow->dwAddr==0 || prow->dwMask ==0 || (prow->dwAddr&0xFF)==127 || (prow->wType & MIB_IPADDR_DISCONNECTED)  )
		{
			continue;
		}

		if (prow->dwMask != 0xFFFFFFFF)
		{
			IN_ADDR inAddr,inMask;
			inAddr.S_un.S_addr  = prow->dwAddr;
			inMask.S_un.S_addr = prow->dwMask;

			free(pIPAddrTable);
			string strIP = inet_ntoa(inAddr);
			WSACleanup();
			return strIP;
		}
	}

	free( pIPAddrTable );
	WSACleanup();
	return "";
}

string GetMAC()
{
	string strMac;
	ULONG outBufLen = sizeof(IP_ADAPTER_ADDRESSES);
	PIP_ADAPTER_ADDRESSES pAddresses = (IP_ADAPTER_ADDRESSES*)malloc(outBufLen);
	if (pAddresses == NULL) 
		return strMac;
	// Make an initial call to GetAdaptersAddresses to get the necessary size into the ulOutBufLen variable
	if(GetAdaptersAddresses(AF_UNSPEC, 0, NULL, pAddresses, &outBufLen) == ERROR_BUFFER_OVERFLOW)
	{
		free(pAddresses);
		pAddresses = (IP_ADAPTER_ADDRESSES*)malloc(outBufLen);
		if (pAddresses == NULL) 
			return strMac;
	}

	if(GetAdaptersAddresses(AF_UNSPEC, 0, NULL, pAddresses, &outBufLen) == NO_ERROR)
	{
		// If successful, output some information from the data we received
		for(PIP_ADAPTER_ADDRESSES pCurrAddresses = pAddresses; pCurrAddresses != NULL; pCurrAddresses = pCurrAddresses->Next)
		{
			// ȷ��MAC��ַ�ĳ���Ϊ 00-00-00-00-00-00
			if(pCurrAddresses->PhysicalAddressLength != 6)
				continue;
			char acMAC[20] = {0};
			sprintf(acMAC, "%02X-%02X-%02X-%02X-%02X-%02X",
				int (pCurrAddresses->PhysicalAddress[0]),
				int (pCurrAddresses->PhysicalAddress[1]),
				int (pCurrAddresses->PhysicalAddress[2]),
				int (pCurrAddresses->PhysicalAddress[3]),
				int (pCurrAddresses->PhysicalAddress[4]),
				int (pCurrAddresses->PhysicalAddress[5]));
			strMac = acMAC;
			break;
		}
	} 
	free(pAddresses);
	return strMac;
}

void GetIpAdapterInfoList(deque<IP_ADAPTER_INFO>& theIpAdapterList)
{
	//PIP_ADAPTER_INFO�ṹ��ָ��洢����������Ϣ  
	PIP_ADAPTER_INFO pIpAdapterInfo = new IP_ADAPTER_INFO();  
	//�õ��ṹ���С,����GetAdaptersInfo����  
	unsigned long stSize = sizeof(IP_ADAPTER_INFO);  
	//����GetAdaptersInfo����,���pIpAdapterInfoָ�����;����stSize��������һ��������Ҳ��һ�������  
	int nRel = GetAdaptersInfo(pIpAdapterInfo,&stSize);  
	if (ERROR_BUFFER_OVERFLOW == nRel)  
	{  
		//����������ص���ERROR_BUFFER_OVERFLOW  
		//��˵��GetAdaptersInfo�������ݵ��ڴ�ռ䲻��,ͬʱ�䴫��stSize,��ʾ��Ҫ�Ŀռ��С  
		//��Ҳ��˵��ΪʲôstSize����һ��������Ҳ��һ�������  
		//�ͷ�ԭ�����ڴ�ռ�  
		delete pIpAdapterInfo;  
		//���������ڴ�ռ������洢����������Ϣ  
		pIpAdapterInfo = (PIP_ADAPTER_INFO)new BYTE[stSize];  
		//�ٴε���GetAdaptersInfo����,���pIpAdapterInfoָ�����  
		nRel=GetAdaptersInfo(pIpAdapterInfo,&stSize);   
		if (ERROR_SUCCESS == nRel)  
		{  
			//���������Ϣ  
			//�����ж�����,���ͨ��ѭ��ȥ�ж�
			while (pIpAdapterInfo)  
			{  
				theIpAdapterList.push_back(*pIpAdapterInfo);
				pIpAdapterInfo = pIpAdapterInfo->Next;  
			}  
		}  
		//�ͷ��ڴ�ռ�   
		delete []pIpAdapterInfo;   
	} 
	else if(ERROR_SUCCESS == nRel)
	{
		theIpAdapterList.push_back(*pIpAdapterInfo);	
		//�ͷ��ڴ�ռ�   
		delete pIpAdapterInfo; 
	} 	
}

#ifndef __INCLUDE__STRINGTOOLS__H
#define __INCLUDE__STRINGTOOLS__H
#include "StringTools.h"
#endif
xstring GetExtensionFromFileName(const xstring& strFileName)
{
	//PathFindExtension()	//��ʱ�������api
	
	xstring strExtension;
	int nPos = strFileName.rfind(_T('.'));
	if (nPos != string::npos)
	{
		strExtension = strFileName.substr(nPos+1);
	}

	strExtension = StringConvertUpperOrLower(false,strExtension);
	return strExtension;
}


xstring GetDefaultBrowser()
{
	HKEY hDefBrowserPos = NULL;
	xstring strDefBrowserPath = _T("C:\\Program Files\\Internet Explorer\\iexplore.exe");

	xstring strValue;
	bool bRet = ReadRegString(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\Shell\\Associations\\UrlAssociations\\Ftp\\UserChoice"),_T("ProgId"),REG_SZ,strValue);
	if (bRet)
	{
		xstring strDefBrowserPos = strValue;
		strDefBrowserPos += _T("\\shell\\open\\command");
		
		bRet = ReadRegString(HKEY_CLASSES_ROOT,strDefBrowserPos,_T(""),REG_SZ,strValue);

		if (bRet)
		{
			// ��� exe ·��.
			strDefBrowserPath = strValue;
			xstring::size_type leftQuotation = strDefBrowserPath.find(_T('"'));
			if (leftQuotation != string::npos)
			{
				wstring::size_type rightQuotation = strDefBrowserPath.find(L'"', leftQuotation + 1);
				if (rightQuotation != string::npos)
				{
					strDefBrowserPath.assign(strDefBrowserPath.begin() + leftQuotation + 1,strDefBrowserPath.begin() + rightQuotation);
				}
			}
		}
	}
	return strDefBrowserPath;
}

void CreateDesktopIcon(const xstring& strIconName,const xstring& strExeFile,const xstring& strCmdLine,const xstring& strIconFile)
{
	int nSpecialFolder = CSIDL_DESKTOPDIRECTORY;
	LPITEMIDLIST pidl = NULL;
	LPMALLOC pShell = NULL;
	TCHAR szPath[512] = {0};

	TCHAR szLink[1024] = {0};

	if( SUCCEEDED(SHGetMalloc(&pShell)) )
	{
		if( SUCCEEDED(SHGetSpecialFolderLocation(NULL,nSpecialFolder,&pidl)) )
		{
			if(!SHGetPathFromIDList(pidl,szPath))
			{
				pShell->Free(pidl);
				pShell->Release();
				return ;
			}
			pShell->Release();

			_sntprintf_s(szLink,_countof(szLink),_TRUNCATE,_T("%s\\%s.lnk"),szPath,strIconName.c_str());

			IShellLink* psl = NULL;
			if(SUCCEEDED(CoCreateInstance(CLSID_ShellLink,NULL,CLSCTX_INPROC_SERVER,IID_IShellLink,(LPVOID*)&psl)))
			{
				psl->SetPath(strExeFile.c_str());
				psl->SetArguments(strCmdLine.c_str());
				psl->SetIconLocation(strIconFile.c_str(),0);
				psl->SetWorkingDirectory(_T("C:\\"));

				IPersistFile* ppf = NULL;
				if(SUCCEEDED(psl->QueryInterface(IID_IPersistFile,(LPVOID*)&ppf)))
				{
#ifdef UNICODE
					ppf->Save(szLink, TRUE);
#else
					ppf->Save(_W(szLink), TRUE);
#endif
					
					ppf->Release();
				}
				psl->Release();
			}
		}
	}
}

bool SaveIconFileFromExeFile(const xstring& strExe,const xstring& strDestFile)
{
	SHFILEINFO sfi; 
	ZeroMemory(&sfi, sizeof(SHFILEINFO)); 
	::SHGetFileInfo(strExe.c_str(), FILE_ATTRIBUTE_NORMAL, &sfi, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX); 

	HIMAGELIST* imageList = NULL; 
	//�ɴ�ͼ��Сͼ���γ�ȡ,ֱ����ȡ��Ϊֹ(SHIL_JUMBO���ܳ�ȡ������,����ʹ�����ʱ���������ico����,������������ȡ)
	HICON hIcon = NULL;
	HRESULT hResult = NULL;
	//HRESULT hResult = ::SHGetImageList(SHIL_JUMBO, IID_IImageList, (void**)&imageList);
	//if (hResult == S_OK)
	//{
	//	((IImageList*)imageList)->GetIcon(sfi.iIcon, ILD_NORMAL, &hIcon); 
	//}
	if (hIcon == NULL)
	{
		hResult = ::SHGetImageList(SHIL_EXTRALARGE, IID_IImageList, (void**)&imageList);
		if (hResult == S_OK)
		{
			((IImageList*)imageList)->GetIcon(sfi.iIcon, ILD_NORMAL, &hIcon); 
		}
	}
	if (hIcon == NULL)
	{
		hResult = ::SHGetImageList(SHIL_LARGE, IID_IImageList, (void**)&imageList);
		if (hResult == S_OK)
		{
			((IImageList*)imageList)->GetIcon(sfi.iIcon, ILD_NORMAL, &hIcon); 
		}
	}
	if (hIcon == NULL)
	{
		hResult = ::SHGetImageList(SHIL_SMALL, IID_IImageList, (void**)&imageList);
		if (hResult == S_OK)
		{
			((IImageList*)imageList)->GetIcon(sfi.iIcon, ILD_NORMAL, &hIcon); 
		}
	}
	if (hIcon == NULL)
	{
		return false;
	}

	//�����ļ�
	HANDLE hFile = CreateFile(strDestFile.c_str(),GENERIC_WRITE,0,NULL,CREATE_ALWAYS,NULL,NULL);
	//д��ͷ
	typedef struct
	{
		WORD idReserved; // Reserved (must be 0)
		WORD idType; // Resource Type (1 for icons)
		WORD idCount; // How many images?
	}ICONHEADER;	
	ICONHEADER icoHeader = {0,1,1};
	DWORD dwWrite = 0;
	WriteFile(hFile,&icoHeader,sizeof(icoHeader),&dwWrite,NULL);
	
	typedef struct
	{
		BYTE bWidth;
		BYTE bHeight;
		BYTE bColorCount;
		BYTE bReserved;
		WORD wPlanes; // for cursors, this field = wXHotSpot
		WORD wBitCount; // for cursors, this field = wYHotSpot
		DWORD dwBytesInRes;
		DWORD dwImageOffset; // file-offset to the start of ICONIMAGE
	}ICONDIR;

	SetFilePointer(hFile, sizeof(ICONDIR), 0, FILE_CURRENT);

	int nOffset = SetFilePointer(hFile, 0, 0, FILE_CURRENT);
	//д��ͼ��ͷ
	BITMAPINFOHEADER biHeader;
	ZeroMemory(&biHeader, sizeof(biHeader));
	biHeader.biSize = sizeof(biHeader);

	ICONINFO theIcon;
	GetIconInfo(hIcon,&theIcon);
	BITMAP bmColor,bmMask;
	::GetObject(theIcon.hbmColor, sizeof(BITMAP), &bmColor);
	::GetObject(theIcon.hbmMask, sizeof(BITMAP), &bmMask);
	biHeader.biWidth = bmColor.bmWidth;
	biHeader.biHeight = bmColor.bmHeight*2;
	biHeader.biPlanes = bmColor.bmPlanes;
	biHeader.biBitCount = bmColor.bmBitsPixel;
	DWORD dwBmcolorImageBytes,dwBmMaskImageBytes;
	dwBmcolorImageBytes = bmColor.bmWidthBytes;
	if (dwBmcolorImageBytes & 3)
		dwBmcolorImageBytes = (dwBmcolorImageBytes + 4) & ~3;
	dwBmcolorImageBytes = dwBmcolorImageBytes*bmColor.bmHeight;
	dwBmMaskImageBytes = bmMask.bmWidthBytes;
	if (dwBmMaskImageBytes & 3)
		dwBmMaskImageBytes = (bmMask.bmWidthBytes + 4) & ~3;
	dwBmMaskImageBytes = dwBmMaskImageBytes*bmMask.bmHeight;
	biHeader.biSizeImage = dwBmcolorImageBytes + dwBmMaskImageBytes;
	WriteFile(hFile, &biHeader, sizeof(biHeader), &dwWrite, 0);
	//д��bmColorͼ������
	BYTE* pIconData = (BYTE *)malloc(dwBmcolorImageBytes);
	GetBitmapBits(theIcon.hbmColor, dwBmcolorImageBytes, pIconData);
	for(int i = bmColor.bmHeight - 1; i >= 0; i--)
	{
		WriteFile(hFile,pIconData + (i * bmColor.bmWidthBytes), bmColor.bmWidthBytes, &dwWrite,0);
		// extend to a 32bit boundary (in the file) if necessary
		if(bmColor.bmWidthBytes & 3)
		{
			DWORD padding = 0;
			WriteFile(hFile, &padding, 4 - (bmColor.bmWidthBytes & 3), &dwWrite, 0);
		}
	}
	free(pIconData);
	//д��bmMaskͼ������
	BYTE* pIconDataMask = (BYTE *)malloc(dwBmMaskImageBytes);
	GetBitmapBits(theIcon.hbmMask, dwBmMaskImageBytes, pIconDataMask);
	for(int i = bmMask.bmHeight - 1; i >= 0; i--)
	{
		WriteFile(hFile,pIconDataMask + (i * bmMask.bmWidthBytes), bmMask.bmWidthBytes, &dwWrite,0);
		// extend to a 32bit boundary (in the file) if necessary
		if(bmMask.bmWidthBytes & 3)
		{
			DWORD padding = 0;
			WriteFile(hFile, &padding, 4 - (bmMask.bmWidthBytes & 3), &dwWrite, 0);
		}
	}
	free(pIconDataMask);

	SetFilePointer(hFile, sizeof(ICONHEADER), 0, FILE_BEGIN);
	//Lastly, skip back and write the icon directories.Write one icon directory entry - specify the index of the image

	ICONDIR iconDir;
	UINT nColorCount;
	if(bmColor.bmBitsPixel >= 8)
		nColorCount = 0;
	else
		nColorCount = 1 << (bmColor.bmBitsPixel * bmColor.bmPlanes);
	// Create the ICONDIR structure
	iconDir.bWidth = (BYTE)bmColor.bmWidth;
	iconDir.bHeight = (BYTE)bmColor.bmHeight;
	iconDir.bColorCount = nColorCount;
	iconDir.bReserved = 0;
	iconDir.wPlanes = bmColor.bmPlanes;
	iconDir.wBitCount = bmColor.bmBitsPixel;
	iconDir.dwBytesInRes = sizeof(BITMAPINFOHEADER) + dwBmcolorImageBytes + dwBmMaskImageBytes;
	iconDir.dwImageOffset = nOffset;
	WriteFile(hFile, &iconDir, sizeof(iconDir), &dwWrite, 0);

	//�ͷ���Դ
	::DeleteObject(theIcon.hbmColor);
	::DeleteObject(theIcon.hbmMask);
	DestroyIcon(hIcon);
	//�ر��ļ����
	CloseHandle(hFile);

	return true;
}

//�ɾ���·����ȡ�ļ���
xstring GetAppNameFromPath(const xstring& strAppPath)
{
	xstring strRet = strAppPath;
	int nPos = strRet.rfind(_T('\\'));
	if (nPos == string::npos)
	{
		nPos = strRet.rfind(_T('/'));
	}
	if (nPos != string::npos)
	{
		strRet = strRet.substr(nPos+1);
	}
	return strRet;
}

int IsValidIdCardNumber(const xstring& strIdCardNumber)
{
	//���֤�ų���
	if (strIdCardNumber.length() != 18)
		return -1;

	//�Ƿ��ַ�
	xstring strValue1 = strIdCardNumber.substr(0,17);
	if (strValue1.find_first_not_of(_T("0123456789"))!=strValue1.npos )
		return -2;
	xstring strValue2 = strIdCardNumber.substr(17,1);
	if (strValue2.find_first_not_of(_T("0123456789xX"))!=strValue2.npos )
		return -2;

	//�����·�
	int nMonth = _ttoi(strIdCardNumber.substr(10,2).c_str());
	if (nMonth<1 || nMonth>12)
		return -3;
	//��������	//�����жϵ����Ƿ������31��
	int nDay = _ttoi(strIdCardNumber.substr(12,2).c_str());
	if (nDay<1 || nDay>31)
		return -4;

	//��GB 11643-1999���й涨����֤У����
	int weight[]={7,9,10,5,8,4,2,1,6,3,7,9,10,5,8,4,2};
	xstring strValidate = _T("10X98765432");
	int sum = 0;

	for(unsigned int i=0;i<strValue1.length();i++)
	{
		sum = sum + (strValue1[i] - _T('0')) * weight[i];
	}
	int nIndex = sum % 11;
	if (strValidate[nIndex] != strValue2[0])
		return -5;
	
	return 1;
}

bool AddFollowSystemStart(const xstring& strName,const xstring& strFile)
{
	HKEY   hKey;   
	//�ҵ�ϵͳ��������   
	LPCTSTR lpszRun = _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run");   
	if(::RegOpenKeyEx(HKEY_LOCAL_MACHINE,lpszRun,0,KEY_READ|KEY_WRITE,&hKey)==ERROR_SUCCESS)  
	{  
		wchar_t lpData[512] = {0};
		DWORD cbData=512;   
		DWORD dwType = REG_SZ;   
		if(::RegQueryValueEx(hKey,strName.c_str(),0,&dwType,(LPBYTE)lpData,&cbData) == ERROR_SUCCESS) //�Ѵ���  
		{  
			if (_tcsicmp(strFile.c_str(),lpData) == 0)	//ȷʵ��Ҫ�����ĳ���
			{
				::RegCloseKey(hKey);
				return true;
			}			
		}  

		//������ע�����������ͬ������ֵ����ʱ
#ifdef UNICODE
		if( ::RegSetValueEx(hKey,strName.c_str(),0,REG_SZ,(const unsigned char*)(LPCTSTR)strFile.c_str(),strFile.length()*2) == ERROR_SUCCESS)
#else
		if( ::RegSetValueEx(hKey,strName.c_str(),0,REG_SZ,(const unsigned char*)(LPCTSTR)strFile.c_str(),strFile.length()) == ERROR_SUCCESS)
#endif		  
		{
			::RegCloseKey(hKey); 
			return true;
		}			
		::RegCloseKey(hKey); 
	}
	return false;  
}

bool DelFollowSystemStart(const xstring& strName,const xstring& strFile)
{
	HKEY   hKey;   
	//�ҵ�ϵͳ��������   
	LPCTSTR lpszRun = _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run");   
	if(::RegOpenKeyEx(HKEY_LOCAL_MACHINE,lpszRun,0,KEY_READ|KEY_WRITE,&hKey)==ERROR_SUCCESS)  
	{  
		wchar_t lpData[512] = {0};
		DWORD cbData=512;   
		DWORD dwType = REG_SZ;   
		if(::RegQueryValueEx(hKey,strName.c_str(),0,&dwType,(LPBYTE)lpData,&cbData) == ERROR_SUCCESS) //�Ѵ���  
		{  
			if (_tcsicmp(strFile.c_str(),lpData) == 0 || strFile.empty())	//ȷʵ��Ҫ�����ĳ���,����ֻ����ɾ��ָ���ļ�
			{
				if( RegDeleteValue(hKey,strName.c_str()) != ERROR_SUCCESS)	//ɾ��ʧ��
				{
					::RegCloseKey(hKey);
					return false;
				}			
			}
		}  
		::RegCloseKey(hKey);
	}
	return true;
}

//��ȡguid�ַ���
xstring GetGUID(bool bUpper/*=true*/)
{
	GUID guid;
	CoCreateGuid(&guid); 
	TCHAR szValue[256] = {0};  
	if (bUpper)
	{
		_sntprintf_s(szValue,_countof(szValue),_TRUNCATE,_T("%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X"), 
			guid.Data1, guid.Data2, guid.Data3,  
			guid.Data4[0], guid.Data4[1],  
			guid.Data4[2], guid.Data4[3],  
			guid.Data4[4], guid.Data4[5],  
			guid.Data4[6], guid.Data4[7]);  
	}
	else
	{
		_sntprintf_s(szValue,_countof(szValue),_TRUNCATE,_T("%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x"), 
			guid.Data1, guid.Data2, guid.Data3,  
			guid.Data4[0], guid.Data4[1],  
			guid.Data4[2], guid.Data4[3],  
			guid.Data4[4], guid.Data4[5],  
			guid.Data4[6], guid.Data4[7]);  
	}

	return szValue; 
}

string ReadAllFromFile(const xstring& strFile)
{
	string strRet;
	try
	{
		HANDLE hFile = CreateFile(strFile.c_str(),GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,NULL,NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			OutputDebugStringA("�ļ���ʧ��\n");
		}
		else
		{
			int nSize = GetFileSize(hFile, NULL)+4;		//��4��������Ķ��ڴ�պñ��ļ�����������,�ַ���ĩβû��'\0'��
			char* szBuf = (char*)malloc(nSize);
			memset(szBuf,0,nSize);
			DWORD dwReadSize=0;
			ReadFile(hFile, szBuf, nSize, &dwReadSize, NULL);
			strRet = szBuf;
			free(szBuf);
			CloseHandle(hFile);
		}
	}
	catch (...)
	{
		OutputDebugStringA("�ļ���д�쳣\n");
	}
	return strRet;
}
string ReadAllFromFileA(const string& strFile)
{
	string strRet;
	try
	{
		HANDLE hFile = CreateFileA(strFile.c_str(),GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,NULL,NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			OutputDebugStringA("�ļ���ʧ��\n");
		}
		else
		{
			int nSize = GetFileSize(hFile, NULL)+4;		//��4��������Ķ��ڴ�պñ��ļ�����������,�ַ���ĩβû��'\0'��
			char* szBuf = (char*)malloc(nSize);
			memset(szBuf,0,nSize);
			DWORD dwReadSize=0;
			ReadFile(hFile, szBuf, nSize, &dwReadSize, NULL);
			strRet = szBuf;
			free(szBuf);
			CloseHandle(hFile);
		}
	}
	catch (...)
	{
		OutputDebugStringA("�ļ���д�쳣\n");
	}
	return strRet;
}

typedef void (WINAPI *LPFN_PGNSI)(LPSYSTEM_INFO);
bool Is64BitOS()
{
	SYSTEM_INFO si = { 0 };
	LPFN_PGNSI pGNSI = (LPFN_PGNSI) GetProcAddress(GetModuleHandle(_T("kernel32.dll")), "GetNativeSystemInfo");
	if (pGNSI)
	{
		pGNSI(&si);
		if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 || si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64 )
		{
			return true;
		}
	}
	
	return false;
}

int CopyFolder(const xstring& strSource,const xstring& strDest)
{
	SHFILEOPSTRUCT FileOp;   
	//����MSDN�ϣ�ZeryMerory�ڵ����������ַ��������������ڵ�ʱ��  
	//�ᱻ�������Ż����Ӷ������������ݻᱻ���������׽����  
	//���������ȫ���⣬�ر��Ƕ���������Щ�Ƚ����е���Ϣ��˵��  
	//��SecureZeroMemory�򲻻����������⣬��֤�����������ݻᱻ��ȷ�����㡣  
	//����漰���Ƚ����е����ݣ�����ʹ��SecureZeroMemory������ 
	xstring strFrom = strSource + _T('\0');
	xstring strTo = strDest + _T('\0');
	SecureZeroMemory((void*)&FileOp, sizeof(SHFILEOPSTRUCT));//secureZeroMemory��ZeroMerory������  
 
	FileOp.fFlags = FOF_NOCONFIRMATION|FOF_NOCONFIRMMKDIR;		//������ȷ�ϱ�־   
	FileOp.hNameMappings = NULL;			//�ļ�ӳ��  
	FileOp.hwnd = NULL;						//��Ϣ���͵Ĵ��ھ����  
	FileOp.lpszProgressTitle = NULL;		//�ļ��������ȴ��ڱ���   
	FileOp.pFrom = strFrom.c_str();			//Դ�ļ���·��    //����Ҫ�ԡ�\0\0����β����Ȼɾ������  
	FileOp.pTo = strTo.c_str();				//Ŀ���ļ���·��
	FileOp.wFunc = FO_COPY;					//��������   
	return SHFileOperation(&FileOp); 
}
int CopyFolderA(const string& strSource,const string& strDest)
{
	SHFILEOPSTRUCTA FileOp;   
	//����MSDN�ϣ�ZeryMerory�ڵ����������ַ��������������ڵ�ʱ��  
	//�ᱻ�������Ż����Ӷ������������ݻᱻ���������׽����  
	//���������ȫ���⣬�ر��Ƕ���������Щ�Ƚ����е���Ϣ��˵��  
	//��SecureZeroMemory�򲻻����������⣬��֤�����������ݻᱻ��ȷ�����㡣  
	//����漰���Ƚ����е����ݣ�����ʹ��SecureZeroMemory������ 
	SecureZeroMemory((void*)&FileOp, sizeof(SHFILEOPSTRUCTA));//secureZeroMemory��ZeroMerory������  

	string strFrom = strSource + '\0';
	string strTo = strDest + '\0';
	SecureZeroMemory((void*)&FileOp, sizeof(SHFILEOPSTRUCT));//secureZeroMemory��ZeroMerory������  

	FileOp.fFlags = FOF_NOCONFIRMATION|FOF_NOCONFIRMMKDIR;		//������ȷ�ϱ�־   
	FileOp.hNameMappings = NULL;			//�ļ�ӳ��  
	FileOp.hwnd = NULL;						//��Ϣ���͵Ĵ��ھ����  
	FileOp.lpszProgressTitle = NULL;		//�ļ��������ȴ��ڱ���   
	FileOp.pFrom = strFrom.c_str();			//Դ�ļ���·��    //����Ҫ�ԡ�\0\0����β����Ȼɾ������  
	FileOp.pTo = strTo.c_str();				//Ŀ���ļ���·��
	FileOp.wFunc = FO_COPY;					//��������   
	return SHFileOperationA(&FileOp); 
}

int GetProcesssIdFromName(const xstring& strPorcessName)
{
	HANDLE hProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if( hProcess == INVALID_HANDLE_VALUE )
	{
		return -1;
	}

	TCHAR szShortPath[MAX_PATH*2] = { 0 };
	PROCESSENTRY32 pinfo; 
	MODULEENTRY32 minfo;
	pinfo.dwSize = sizeof(PROCESSENTRY32);
	minfo.dwSize = sizeof( MODULEENTRY32);

	BOOL report = Process32First(hProcess, &pinfo); 
	while(report) 
	{ 
		HANDLE hModule = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pinfo.th32ProcessID); 
		if( hModule != INVALID_HANDLE_VALUE )
		{
			if( Module32First( hModule, &minfo ) )
			{
				xstring strCurExeName = minfo.szExePath;
				int nPos = strCurExeName.rfind(_T('\\'));
				strCurExeName = strCurExeName.substr(nPos+1);
				if (strPorcessName == strCurExeName)
				{
					CloseHandle( hModule );
					CloseHandle( hProcess );
					return pinfo.th32ProcessID;
				}
			}

			CloseHandle( hModule );
		}
		report = Process32Next(hProcess, &pinfo);
	}

	CloseHandle( hProcess );
	return -1;
	
}
void GetProcesssIdFromName(const xstring& strPorcessName,deque<int>& dequeOutID)
{
	dequeOutID.clear();
	HANDLE hProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if( hProcess == INVALID_HANDLE_VALUE )
	{
		return ;
	}

	TCHAR szShortPath[MAX_PATH*2] = { 0 };
	PROCESSENTRY32 pinfo; 
	MODULEENTRY32 minfo;
	pinfo.dwSize = sizeof(PROCESSENTRY32);
	minfo.dwSize = sizeof( MODULEENTRY32);

	BOOL report = Process32First(hProcess, &pinfo); 
	while(report) 
	{ 
		HANDLE hModule = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pinfo.th32ProcessID); 
		if( hModule != INVALID_HANDLE_VALUE )
		{
			if( Module32First( hModule, &minfo ) )
			{
				xstring strCurExeName = minfo.szExePath;
				int nPos = strCurExeName.rfind(_T('\\'));
				strCurExeName = strCurExeName.substr(nPos+1);
				if (strPorcessName == strCurExeName)
				{
					dequeOutID.push_back(pinfo.th32ProcessID);
				}
			}

			CloseHandle( hModule );
		}
		report = Process32Next(hProcess, &pinfo);
	}

	CloseHandle( hProcess );
}

long GetFileSizeByte(const xstring& strFile)
{
	FILE* file = NULL;
#ifdef UNICODE
	fopen_s(&file,_A(strFile), "r");
#else
	fopen_s(&file,strFile.c_str(), "r");
#endif		
	if (file)
	{
		long nSize = _filelength(_fileno(file));
		fclose(file);
		return nSize;
	}
	return -1;
}

long GetFileSizeByteA(const string& strFile)
{
	FILE* file = NULL;
	fopen_s(&file,strFile.c_str(), "r");
	if (file)
	{
		long nSize = _filelength(_fileno(file));
		fclose(file);
		return nSize;
	}
	return -1;
}