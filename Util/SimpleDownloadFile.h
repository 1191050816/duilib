#pragma once
#include <string>
#include <deque>
#include <urlmon.h>
#include "KCritSec.h"
using namespace std;

#pragma comment(lib, "urlmon.lib")

//pVoid-DownloadFileʱ���ĵ�һ������ pszUrl-Ҫ���ص��ļ���url pszPath-���·�����ļ���
typedef void (*PDownloadResultCallback)(LPVOID pVoid,bool bDownloadOK,LPCTSTR pszPath);	//���ؽ���ص�

#ifdef _UNICODE
	#define xstring wstring
#else
	#define xstring string 
#endif

typedef struct tagDownloadInfo
{
	tagDownloadInfo()
	{
		pCallback = NULL;
		pVoid = NULL;
	};

	tagDownloadInfo& operator=(const tagDownloadInfo& value)
	{
		strUrl = value.strUrl;
		strPath = value.strPath;
		pCallback = value.pCallback;
		pVoid = value.pVoid;
		return *this;
	};

	xstring strUrl;
	xstring strPath;
	PDownloadResultCallback pCallback;
	LPVOID pVoid;
}SDownloadInfo;

class CSimpleDownloadFile :	public IBindStatusCallback
{
public:
	CSimpleDownloadFile(void);
	~CSimpleDownloadFile(void);

	//����:pVoid-�ص�ʱ��������ָ�� pszUrl-Դurl pszPath-Ŀ��·�����ļ��� false-�Ƿ�ɾ���Ѿ����ڵ�ͬ���ļ�(��ɾ���п��ܵ����ļ����ݲ������)
	void DownloadFile(LPVOID pVoid,LPCTSTR pszUrl,LPCTSTR pszPath,PDownloadResultCallback pCallback = NULL,bool bDeleteOld = true);

public:
	HRESULT STDMETHODCALLTYPE OnStartBinding(/* [in] */ DWORD dwReserved,/* [in] */ __RPC__in_opt IBinding *pib);
	HRESULT STDMETHODCALLTYPE GetPriority(/* [out] */ __RPC__out LONG *pnPriority);
	HRESULT STDMETHODCALLTYPE OnLowResource(/* [in] */ DWORD reserved);
	HRESULT STDMETHODCALLTYPE OnStopBinding(/* [in] */ HRESULT hresult,/* [unique][in] */ __RPC__in_opt LPCWSTR szError);
	HRESULT STDMETHODCALLTYPE GetBindInfo(/* [out] */ DWORD *grfBINDF,/* [unique][out][in] */ BINDINFO *pbindinfo);
	HRESULT STDMETHODCALLTYPE OnDataAvailable(/* [in] */ DWORD grfBSCF,/* [in] */ DWORD dwSize,/* [in] */ FORMATETC *pformatetc,/* [in] */ STGMEDIUM *pstgmed);
	HRESULT STDMETHODCALLTYPE QueryInterface(/* [in] */ REFIID riid,/* [iid_is][out] */ __RPC__deref_out void __RPC_FAR *__RPC_FAR *ppvObject);
	ULONG STDMETHODCALLTYPE AddRef( void);
	ULONG STDMETHODCALLTYPE Release( void);
	HRESULT STDMETHODCALLTYPE OnObjectAvailable(/* [in] */ __RPC__in REFIID riid,/* [iid_is][in] */ __RPC__in_opt IUnknown *punk);
	HRESULT STDMETHODCALLTYPE OnProgress(/* [in] */ ULONG ulProgress,/* [in] */ ULONG ulProgressMax,/* [in] */ ULONG ulStatusCode,/* [unique][in] */ __RPC__in_opt LPCWSTR szStatusText);

	//��ӵ������б�β��
	void AddTailToDownloadDeque(SDownloadInfo& tInfo);
	//ȡ�������б��ͷ��(��ȡ�߶����ǻ�ȡ)	����ֵ:false-��ȡʧ��
	bool GetHeadFromDownloadDeque(SDownloadInfo& tInfo);
	//��ȡ�����б��еĽڵ�����
	int GetCountForDownloadDeque();

public:
	deque<SDownloadInfo> m_dequeDownloadInfo;		//�����ļ����б�

	HANDLE m_hEvent;								//�ȴ������ź�
	HANDLE m_hThread;								//�����߳̾��
	CKCritSec m_lockDeque;							//Deque������,��֤Deque���̰߳�ȫ��
};
