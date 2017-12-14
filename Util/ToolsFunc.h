#pragma once
#include <Shlwapi.h>
#include <WinBase.h>
#include <string>
#include <IPHlpApi.h>
#include <ShlObj.h>
#pragma comment(lib,"Shell32.lib")
#ifndef xstring
#ifdef _UNICODE
#define xstring		std::wstring
#else
#define	xstring		std::string
#endif
using namespace std;
#endif

//����һ��Ŀ¼,������Ҳ�᷵��true,���������Ҵ���ʧ��ʱ����false ע�⣺����һ�δ����༶Ŀ¼
BOOL CreateDir(const xstring& strDir);

//���е���Ļ����
void CenterWindowToScreen(HWND hWnd);
//�Զ����д��ڡ�hParent-������,hWnd-Ҫ���еĴ���,��������С������ΪNULLʱ�����е���Ļ
void CenterWindowEx(HWND hParent,HWND hWnd);

//����:����ȡһ���ַ����е���������
std::string GetNumFromStrBuf(const char* pszStrBuf);

void PrintfLog(const TCHAR * format, ...);

//�ļ����ļ����Ƿ����
BOOL IsPathExist(const TCHAR* szPath);

//��ȡע���
bool ReadRegString(HKEY hKey,const xstring& strSubKey,const xstring& strKeyName,const DWORD& dwType ,xstring& strValue);

//��ȡApp����Ŀ¼(��βû��'/'����'\\')
xstring GetAppPath(HMODULE hModul=NULL);

//��ȡ����IP
string GetLocalIp();

//��ȡ����MAC
string GetMAC();

//��ȡ�ļ���׺ (����ֵͳһΪСд) ������.
xstring GetExtensionFromFileName(const xstring& strFileName);

//��ȡ��ǰ��Ĭ�������
xstring GetDefaultBrowser();

//��������ͼ��
void CreateDesktopIcon(const xstring& strIconName,const xstring& strExeFile,const xstring& strCmdLine,const xstring& strIconFile);

//��ȡexe�е�Icon������Ϊ�ļ� (strDestFileһ����ico��β) (���������1����С��ͼ��,�����Ҫ���,�ο�ʵ�ִ�������޸ļ���)
bool SaveIconFileFromExeFile(const xstring& strExe,const xstring& strDestFile);

//�ɾ���·����ȡ�ļ���
xstring GetAppNameFromPath(const xstring& strAppPath);

//�Ƿ�����Ч�����֤�� (����1������Ч,������Ϊ����)
int IsValidIdCardNumber(const xstring& strIdCardNumber);