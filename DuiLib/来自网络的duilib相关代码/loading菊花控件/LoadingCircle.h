#pragma once
#include "..\DuiLib\Core\UIControl.h"
#include <mutex>
#include <condition_variable>

enum StylePresets
{
	Custom,
	MacOSX,
	Firefox,
	IE7
};

class CLoadingCircle :
	public CControlUI
{
public:
	CLoadingCircle();
	~CLoadingCircle();

	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);

	virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	virtual void PaintBkImage(HDC hDC);
	void Start();
	void Stop();
protected:
	virtual void Init();
	void ThreadAni();
	Color* GenerateColorsPallet(Color* _objColor, bool _blnShadeColor, int _intNbSpoke);
	Color* GenerateColorsPallet(Color _objColor, bool _blnShadeColor, int _intNbSpoke);

protected:
	int                 m_nNumber;
	int                 m_nTime;
	std::atomic<bool>   m_bStop;
	std::atomic<bool>   m_bExit;
	std::thread*        m_pTrdAni;

	int m_NumberOfSpoke = 10;//��������
	int m_SpokeThickness = 4;//������ϸ
	int m_ProgressValue;//------------------------
	int m_OuterCircleRadius = 10;//��Ȧ
	int m_InnerCircleRadius = 8;//��Ȧ
	PointF m_CenterPoint;//��άƽ��ĵ�
	Color m_Color;
	Color* m_Colors;
	double* m_Angles;

	std::condition_variable m_condQueue;
	std::mutex m_mutx;
	std::unique_lock<std::mutex>* m_pLock;
};

CControlUI* CreateLoadingControl(LPCTSTR pstrType);
