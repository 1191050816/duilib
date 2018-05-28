// Date 2006/05/17	Ver. 1.22	Psytec Inc.
#pragma once

/////////////////////////////////////////////////////////////////////////////
#include <Windows.h>

//����ȼ�
#define QR_LEVEL_L	0	//7%
#define QR_LEVEL_M	1	//15%
#define QR_LEVEL_Q	2	//25%
#define QR_LEVEL_H	3	//30%

//����ģʽ
#define QR_MODE_NUMERAL		0	//����ģʽ
#define QR_MODE_ALPHABET	1	//��ĸ����ģʽ
#define QR_MODE_8BIT		2	//8bit����ģʽ
#define QR_MODE_KANJI		3	//����ģʽ(�������ĺ���)

//�汾��������
#define QR_VRESION_S	0 // 1~9
#define QR_VRESION_M	1 // 10 ~ 26
#define QR_VRESION_L	2 // 27 ~ 40

#define MAX_ALLCODEWORD	 3706 // ��������//�t���`�ɥ�`�������
#define MAX_DATACODEWORD 2956 // ���ݱ��������ֵ(�����Ϣ����)//�ǩ`�����`�ɥ�`�����(�Щ`�����40-L)
#define MAX_CODEBLOCK	  153 // �����ݱ����������ֵ(����������)//�֥�å��ǩ`�����`�ɥ�`�������(�ңӥ��`�ɥ�`�ɤ򺬤�)
#define MAX_MODULESIZE	  177 // һ��ģ�������ֵ(ÿ�ߵ�ģ����)//һ�x�⥸��`�������

//λͼ����ʱ(ÿ���汾���ӵ�������)//�ӥåȥޥå��軭�r�ީ`����
#define QR_MARGIN	4


/////////////////////////////////////////////////////////////////////////////
//QR����Ÿ��汾�ľ�����Ч
typedef struct tagRS_BLOCKINFO
{
	int ncRSBlock;		// ����Ŀ���
	int ncAllCodeWord;	// ��������
	int ncDataCodeWord;	// ָ������ȼ��µ�����������

} RS_BLOCKINFO, *LPRS_BLOCKINFO;


/////////////////////////////////////////////////////////////////////////////
// QR��汾���ͺţ������Ϣ
typedef struct tagQR_VERSIONINFO
{
	int nVersionNo;	   // Ver  1~40
	int ncAllCodeWord; // ��������=��������+��������

	// ָ������ȼ��µ��������� (0 = L, 1 = M, 2 = Q, 3 = H)
	int ncDataCodeWord[4];	//���ݱ����������ܴ�������- RS����������

	int ncAlignPoint;	// У��ͼ�θ���
	int nAlignPoint[6];	// У��ͼ����������

	RS_BLOCKINFO RS_BlockInfo1[4]; // �����1
	RS_BLOCKINFO RS_BlockInfo2[4]; // �����2 

} QR_VERSIONINFO, *LPQR_VERSIONINFO;


/////////////////////////////////////////////////////////////////////////////
class CQR_Encode
{
public:
	CQR_Encode();
	~CQR_Encode();

public:
	int m_nLevel;		// ������ 0~3
	int m_nVersion;		// �汾==0,�汾�Զ�   �汾 1~40
	BOOL m_bAutoExtent;	// �汾���ͺţ��Զ���չָ����־//�Щ`�����(�ͷ�)�ԄӒ���ָ���ե饰
	int m_nMaskingNo;	// ��Ĥ =-1����Ҫ���� ��ģͼ������ 0~7��ֱ��ָ��

public:
	int m_nSymbleSize;	//ָ���汾��ģ����
	BYTE m_byModuleData[MAX_MODULESIZE][MAX_MODULESIZE]; // [x][y]
	// bit 5������ģ�飨���˶����⣩��־//bit5:�C�ܥ⥸��`�루�ޥ����󥰌����⣩�ե饰
	// bit 4������ģ���������//bit4:�C�ܥ⥸��`���軭�ǩ`��
	// bit 1����������//bit1:���󥳩`�ɥǩ`��
	// bit 0�����ֺ�����������//bit0:�ޥ����ᥨ�󥳩`���軭�ǩ`��
	// 20 h���߼��͸�����ģ���ж���11 h���߼��͸���ͼ������BOOLֵ����//20h�Ȥ�Փ��ͤˤ��C�ܥ⥸��`���ж���11h�Ȥ�Փ��ͤˤ���軭����K�Ĥˤ�BOOL������

private:
	int m_ncDataCodeWordBit; // ʵ����Ϣ������λ��������Ϣ�����ɼ���������=CHY_EncodeData_Len_Before/������ //���ݱ�����λ��
	BYTE m_byDataCodeWord[MAX_DATACODEWORD]; //��Ϣ����Array  ��Ϣ+����� 0b11101100 + 0b00010001// �������ݱ�������

	int m_ncDataBlock;
	BYTE m_byBlockMode[MAX_DATACODEWORD];
	int m_nBlockLength[MAX_DATACODEWORD];

	int m_ncAllCodeWord; // ������Len
	BYTE m_byAllCodeWord[MAX_ALLCODEWORD]; // ������Array
	BYTE m_byRSWork[MAX_CODEBLOCK]; // ��������Array

public:
	BOOL EncodeData(int nLevel, int nVersion, BOOL bAutoExtent, int nMaskingNo, LPCSTR lpsSource, int ncSource = 0,LPCWSTR lpDestFile = NULL);

private:
	int GetEncodeVersion(int nVersion, LPCSTR lpsSource, int ncLength);
	BOOL EncodeSourceData(LPCSTR lpsSource, int ncLength, int nVerGroup);

	int GetBitLength(BYTE nMode, int ncData, int nVerGroup);

	int SetBitStream(int nIndex, WORD wData, int ncData);

	BOOL IsNumeralData(unsigned char c);
	BOOL IsAlphabetData(unsigned char c);
	BOOL IsKanjiData(unsigned char c1, unsigned char c2);

	BYTE AlphabetToBinaly(unsigned char c);
	WORD KanjiToBinaly(WORD wc);

	void GetRSCodeWord(LPBYTE lpbyRSWork, int ncDataCodeWord, int ncRSCodeWord);

	BOOL SaveImgFile(LPCWSTR lpDestFile);
private:
	void FormatModule();

	void SetFunctionModule();
	void SetFinderPattern(int x, int y);
	void SetAlignmentPattern(int x, int y);
	void SetVersionPattern();
	void SetCodeWordPattern();
	void SetMaskingPattern(int nPatternNo);
	void SetFormatInfoPattern(int nPatternNo);
	int CountPenalty();
};
