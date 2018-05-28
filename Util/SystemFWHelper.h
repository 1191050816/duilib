#pragma once
#include "netfw.h"
#include <string>
using namespace std;

//������windows����ǽ������������

class CSystemFWHelper
{
public:
	CSystemFWHelper(void);
	~CSystemFWHelper(void);

private:
	INetFwOpenPorts *m_pFwAuthPorts;
	INetFwAuthorizedApplications *m_pFwAuthApps;

private:
	void Init();
	void UnInit();

public:
	//��ӵ�����ǽ
	bool AddNewApp(const wstring& strAppPath,const wstring& strAppDisName);
	//����Ƿ��ڷ���ǽ
	bool CheckAppAndEnable(const wstring& strAppPath);
};

