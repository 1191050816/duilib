#pragma once

#include <string>
#include <map>
using namespace std;
#include "../curl/include/curl.h"
#pragma comment(lib, "../curl/lib/libcurl.lib")
#pragma comment ( lib, "ws2_32.lib" )
#pragma comment ( lib, "wldap32.lib" )

//���ؽ��Ȼص� ��������ǲ�������ص��� ����ֵ:true-���� false-ȡ������
typedef bool (*PDownloadProcessingCallback)(LPVOID pVoid,__int64 nTotal,__int64 nCurDownload,double dbSpeed);	//���ؽ���ص�

class CLibCurlAux
{
public:
	CLibCurlAux(void);
	~CLibCurlAux(void);

private:
	string  m_strhttpHdrRecved;
	string  m_httpDataRecved;
	CURL*  m_easy_handle;
	string  m_strErrTxt;   //��������ʱ�ı�����

	struct curl_httppost *m_pPostStart;
	struct curl_httppost *m_pPostEnd;

public:
	wstring m_strDestFile;

public:
	static bool InitGlobalCurl();
	static void UninitGlobalCurl();

	bool InitLibCurl(LPCSTR lpszCookieFile=NULL);
	//nTransTimeOut-�������ݳ�ʱ(��λ:��) nConnTimeOut-���ӳ�ʱ(��λ:��)
	bool SetTimeOut(int nTransTimeOut, int nConnTimeOut);
	void UninitLibCurl();
	long OpenUrl(const char *szWebUrl, bool bHttpGet, string& strRepData, const char *pszAppendHeads=NULL,const wstring& strFile=L"");
	void SetSimplePostData(const char *pszPostData);
	void AddMultiPartPostData(const char *pszKey, const char *pszVal);

	void ReceiveData(void* buffer, size_t size, size_t nmemb);
	void ReceiveHdr(void* buffer, size_t size, size_t nmemb);
	bool SendHttpQuest(const char *pszUrl, bool bHttpGet=true, const char *pszAppendHeaders = NULL);
	long  GetResponseInfo();
	string& GetResponseHdr();
	string& GetResponseData();
	string GetErrText(){return m_strErrTxt;};	

	string sendSyncRequest(const string &url,const map<string, string> &paramsMap,const map<string, string> &headers = map<string, string>());

public:
	//�����Ľӿں;��,����Ԥ�ȵ���InitLibCurl,��Ϻ�Ҳ�������UninitLibCurl
	bool ConnectOnly(const string& strUrl,int nTimeOut);

	//ͬ�����غ���������ֵ��ʾ������û�г���ע�⣺û����Ҳ�������ļ�һ��������ȷ��
	//�����Ľӿں;��,����Ԥ�ȵ���InitLibCurl,��Ϻ�Ҳ�������UninitLibCurl
	bool SyncDownLoadFile(LPVOID pUserData, const char *szWebUrl, const wstring& strFile,PDownloadProcessingCallback pDownloadProingCall=NULL);

	//����Ŀ¼,��һ�δ����༶Ŀ¼
	BOOL CreateDirW(const wstring& strDir);
};

