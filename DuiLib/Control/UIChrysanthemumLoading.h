#pragma once

namespace DuiLib
{
#define EVENT_TIME_ID	100

	class CChrysanthemumLoadingUI : public CControlUI
	{
	public:
		CChrysanthemumLoadingUI(void);
		~CChrysanthemumLoadingUI(void);

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		void	DoInit();
		bool	DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);
		void	DoEvent(TEventUI& event);		
		void	SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		void SetTimeInterval(const int& nValue);
		int GetTimeInterval();
		void SetNumberOfSpoke(const int& nValue);
		int GetNumberOfSpoke();
		void SetSpokeThickness(const int& nValue);
		int GetSpokeThickness();
		void SetOuterCircleRadius(const int& nValue);
		int GetOuterCircleRadius();
		void SetInnerCircleRadius(const int& nValue);
		int GetInnerCircleRadius();

		void SetBkColor(DWORD dwBackColor);

	protected:
		void InitChrysanthemumLoading();
	
	protected:
		int		m_nTimeInterval;			//ʱ���� ��λms
		int		m_nNumberOfSpoke;			//��������	
		int		m_nSpokeThickness;			//������ϸ
		int		m_nProgressValue;			//
		int		m_nOuterCircleRadius;		//��Ȧ�뾶 ��λ������
		int		m_nInnerCircleRadius;		//��Ȧ�뾶
		PointF	m_CenterPoint;				//����Բ��

		Color*	m_pColors;					//��������ɫ
		double* m_pAngles;					//�����ĽǶ�
	};
}	// namespace DuiLib
