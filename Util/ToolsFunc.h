#pragma once
#include <Shlwapi.h>
#include <WinBase.h>
#include <string>
#include <deque>
#include <IPHlpApi.h>
#pragma comment(lib,"Iphlpapi.lib") //��Ҫ���Iphlpapi.lib��  
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
//�ļ����ļ����Ƿ����
BOOL IsPathExistA(const string& strPath);

//��ȡע���
bool ReadRegString(HKEY hKey,const xstring& strSubKey,const xstring& strKeyName,const DWORD& dwType ,xstring& strValue);
//дע���
bool WriteRegValue(HKEY hKey,const xstring& strSubKey,const xstring& strKeyName,const DWORD& dwType ,const BYTE* lpData,DWORD cbData);
//ɾ��ע����ֵ
bool DeleteRegKeyValue(HKEY hKey,const xstring& strSubKey,const xstring& strKeyName);
//ɾ��ע�������Լ��������������
bool DeleteRegSubKey(HKEY hKey,const xstring& strSubKey);

//��ȡApp����Ŀ¼(��βû��'/'����'\\')
xstring GetAppPath(HMODULE hModul=NULL);

//��ȡ����IP
string GetLocalIp();
//��ȡ���������������Ϣ
void GetIpAdapterInfoList(deque<IP_ADAPTER_INFO>& theIpAdapterList);

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

//���뿪������		//strNameע����������еļ��� strFile-�������еļ�ֵ,��Ҫ����·��
bool AddFollowSystemStart(const xstring& strName,const xstring& strFile);
//ɾ����������		//strNameע����������еļ��� strFile-�������еļ�ֵ,��Ҫ����·��,����Ϊ��
bool DelFollowSystemStart(const xstring& strName,const xstring& strFile);

//��ȡguid�ַ��� bUpper-�Ƿ��д
xstring GetGUID(bool bUpper=true);

//���ļ��ж�ȡȫ������
string ReadAllFromFile(const xstring& strFile);
string ReadAllFromFileA(const string& strFile);

//��ȡ��ǰ����ϵͳ����
string GetOSName();
//�Ƿ���64λOS
bool Is64BitOS();

//����Ŀ¼ //�����Ŀ¼��Ҫ��"\\"����"/"��β
int CopyFolder(const xstring& strSource,const xstring& strDest);
int CopyFolderA(const string& strSource,const string& strDest);

//���ҽ���ID (32λ����ֻ�ܲ���32λ����)
int GetProcesssIdFromName(const xstring& strPorcessName);
void GetProcesssIdFromName(const xstring& strPorcessName,deque<int>& dequeOutID);

//��ȡ�ļ���С(�ֽ�) //���2G
long GetFileSizeByte(const xstring& strFile);
long GetFileSizeByteA(const string& strFile);