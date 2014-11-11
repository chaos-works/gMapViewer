                 // GraphData.cpp: implementation of the CGraphData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GraphData.h"
#include "math.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

void	WaitForAwhile(double dWaitTime);
int		CountValue(int nStart, double* pWaveData);
void	QuickSort (int lo, int hi, double* pWaveData);

/*
 *	사용방법
 		1. CGraphData graph;
 		2. graph.SetDataPath(csPathname);
		...
		4. graph.CalcDistance();

 */
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGraphData::CGraphData()
{
	Init();
}


CGraphData::~CGraphData()
{
	if(m_pData) {
		delete m_pData;
		m_pData = NULL;
	}
	if(m_BaseInfo.pRawData) {
		delete m_BaseInfo.pRawData;
		m_BaseInfo.pRawData = NULL;
	}
	if(m_BaseInfo.pWaveData) {
		delete m_BaseInfo.pWaveData;
		m_BaseInfo.pWaveData = NULL;
	}

	if(m_MeasureInfo.pRawData) {
		delete m_MeasureInfo.pRawData;
		m_MeasureInfo.pRawData = NULL;
	}
	if(m_MeasureInfo.pWaveData) {
		delete m_MeasureInfo.pWaveData;
		m_MeasureInfo.pWaveData = NULL;
	}
}

/*-----------------------------------------------------------------------------  
 * 함 수 명 : void CGraphData::Init()
 * 목    적 : 
 * 설    명 : 초기화
 * 작 성 자 : 김영규
 * 작성일자 : 2004.11.15
 * 변경사항 :
 *---------------------------------------------------------------------------*/
void CGraphData::Init()
{
	m_uEventMode			= EVENT_NONE;
	
	// 그래프 사용여부
	m_bUseDiff				= FALSE;
	m_bBaseLoaded			= FALSE;	// Base Graph 화일 읽음 여부
	m_bMeasureLoaded		= FALSE;	// Measure Graph 화일 읽음 여부

	// 측정그래프 정보
	m_MeasureInfo.nLength	= 0;
	m_MeasureInfo.nStart	= 0;
	m_MeasureInfo.pWaveData	= NULL;
	m_MeasureInfo.pRawData	= NULL;
	m_MeasureInfo.ptMin		= CPoint(0,0);
	m_MeasureInfo.ptMax		= CPoint(0,0);
	m_MeasureInfo.csFilename= "";
	m_MeasureInfo.csPathname= "";

	// 베이스 그래프 정보
	m_BaseInfo.nLength		= 0;
	m_BaseInfo.nStart		= 0;
	m_BaseInfo.pWaveData	= NULL;
	m_BaseInfo.pRawData		= NULL;
	m_BaseInfo.ptMin		= CPoint(0,0);
	m_BaseInfo.ptMax		= CPoint(0,0);
	m_BaseInfo.csFilename	= "";
	m_BaseInfo.csPathname	= "";

	// Peak Point
	m_nMinPoint		= 0;
	m_nMaxPoint		= 0;
	m_nPeakPoint	= 0;

	m_nMaxInRange	= 0;
	m_nMinInRange	= 0;
	m_dPeakHeight = 0.0;
	m_dHeightThreshold = 0.0;

    memset((void*)&m_WaveInfo, 0x00, sizeof(NANOWAVE_INFO));	// Wave Info
    memset((void*)&m_WaveData, 0x00, sizeof(NANOWAVE_DATA));	// Wave Data

	m_pData			= NULL;			// Real Data(원시 측정 데이타)
	m_uEventMode	= EVENT_NONE;	// 이벤트 모드

	m_nPulseWidth	= 0;			// Pulse Width
	m_dVolt			= 0;			// Volt
	m_dBRL			= 0;			// dBRL;
	m_dVOP			= 0;			// VOP(%)
	m_dThreshold	= 0;			// Threshold	
	m_dAvgValue		= 0;			// Avg

	m_dMostPopValue	= 0;			// 대표값
	m_dMeterDistance= 0.;			// 거리(m)
	m_wDataLength	= 0;			// Data Length
	m_nDotDistance	= 0;			// 거리(Dot)
	m_nScale		= 0;			// Scale
	m_nChannelNo	= 0;			// 채널번호

	m_dMeasureVop	= 0.0;			// 보정된 VOp값
	m_dEventThreshold = 0.0;		// Event Threshold
	m_nRepeatCount	= 0;			// Smoothing 적용횟수
	
	// 보정계수
	for(int i=0;i<4; i++) {
		m_dValueA[i] = 0.0;
		m_dValueB[i] = 0.0;
		m_dValueC[i] = 0.0;
		m_dValueD[i] = 0.0;

		m_dValueAs[i] = 0.0;
		m_dValueBs[i] = 0.0;
		m_dValueCs[i] = 0.0;
		m_dValueDs[i] = 0.0;
	}
	
	m_nBasePeakPos = 0;	// 2007.5.7 베이스의 피크점
	m_dTurnToPeakHeight = 0.0;
	m_bCoefficientLoaded = FALSE;

	m_bOpenBaseMethod = TRUE;	// Open 형 베이스 사용

	// 이벤트 정보
	memset((void*)&m_EventInfo, 0x00, sizeof(EVENTDETECT_INFO));
    memset((void*)&m_EventScanInfo, 0x00, sizeof(EVENTSCAN_INFO));	// Event Scan Info
}


/*-----------------------------------------------------------------------------  
 * 함 수 명 : BOOL CGraphData::ScanEventPos()
 * 목    적 : 
 * 설    명 : 지정한 그래프 데이타화일을 읽어서 주어진 포인트로부터 스캔하여 
              이벤트 위치를 구하여 이벤트 발생 지점까지의 거리를 미터 단위로 
			  반환한다. 
 * 작 성 자 : 김영규
 * 작성일자 : 2004.11.15
 * 변경사항 :
 *---------------------------------------------------------------------------*/
BOOL CGraphData::ScanEventPos(CString csPathName, int nStartPos, int nEndPos)
{
	EVENTDETECT_INFO EventInfo;

	if(SetDataPath(csPathName))
	{
		memset((void*)&EventInfo, 0x00, EVENTDETECT_INFO_SIZE);

		EventInfo.nRangeStart	= nStartPos;
		EventInfo.nRangeEnd		= nEndPos;

		SetEventInfo(EventInfo);
		return TRUE;
	}

	return FALSE;
}


/*-----------------------------------------------------------------------------  
 * 함 수 명 : BOOL CGraphData::ReadDataFile()
 * 목    적 : 
 * 설    명 : Data 화일을 읽음.
 * 작 성 자 : 김영규
 * 작성일자 : 2004.11.15
 * 변경사항 :
 *---------------------------------------------------------------------------*/
BOOL CGraphData::ReadDataFile(CString csPathName)
{
	CStdioFile		File;
	UINT			nRead;
	WORD			nDataCount=0;
	WORD			nDataLength=0;
	CFileException  fileException;

    NANOWAVE_INFO	WaveInfo;
    NANOWAVE_DATA	WaveData;

    NANOWAVE_2X_DATA	WaveData2X;
    NANOWAVE_2X_END	WaveData2XEnd;
	NANOWAVE_EX		NanowaveEx;
	
	if(File.Open(csPathName, CFile::modeRead | CFile::typeBinary, &fileException))
	{
		memset(&WaveInfo, 0x00, sizeof(NANOWAVE_INFO));
		memset(&WaveData, 0x00, sizeof(NANOWAVE_DATA));
		memset(&NanowaveEx, 0x00, sizeof(NANOWAVE_EX));

		memset(&WaveData2X, 0x00, sizeof(NANOWAVE_2X_DATA));
		memset(&WaveData2XEnd, 0x00, sizeof(NANOWAVE_2X_END));

		/*---------------------------------------------------------------------
		 * NanoWave Info 자료 읽기
		 *-------------------------------------------------------------------*/
		nRead = File.Read(&WaveInfo, sizeof(NANOWAVE_INFO));

		if(nRead > 0) {
			// Ver 1.x
			if(WaveInfo.wHead == 0xa070) {
//				TRACE("mTDR070 Ver_1.x\n");
				nRead = File.Read(&WaveData, sizeof(NANOWAVE_DATA));				

				memcpy((void*)&WaveData2X, &WaveData, sizeof(NANOWAVE_DATA));
				nDataCount = WaveData.wLength/2;				

//				TRACE("[chaos] << %d Channel Wave Info >>\n", GetChannelNo()+1);
//				TRACE("[chaos].......Head : %d\n", WaveInfo.wHead);
//				TRACE("[chaos]....Command : %d\n", WaveInfo.wCommand);
//				TRACE("[chaos].....Length : %d\n", WaveInfo.wLength);
//				TRACE("[chaos]...CheckSum : %d\n", WaveInfo.wCheckSum);
//				TRACE("\n");
				
//				TRACE("[chaos] << %d Channel Wave Data >>\n", GetChannelNo()+1);
//				TRACE("[chaos].......Head : %d\n", WaveData.wHead);
//				TRACE("[chaos]....Command : %d\n", WaveData.wCommand);
//				TRACE("[chaos].....Length : %d\n", nDataCount);
//				TRACE("[chaos]......Scale : %d\n", WaveData.wScale);
//				TRACE("[chaos]........VOP : %d\n", WaveData.wVop);
//				TRACE("[chaos]..Threshold : %d\n", WaveData.wThreshold);
//				TRACE("[chaos].Pulsewidth : %d\n", GetPulseWidth(WaveData.wPulseWidth));
//				TRACE("[chaos]......Start : %d\n", WaveData.dwStart);
//				TRACE("[chaos].......Name : %s\n", WaveData.szName);
//				TRACE("\n");
			}
			// Ver 2.x
			else if(WaveInfo.wHead == 0x070b) {
//				TRACE("mTDR070 Ver_2x1.0\n");
				nRead = File.Read(&WaveData2X, sizeof(NANOWAVE_2X_DATA));				
				
				nDataCount= WaveData2X.sDataValueCount/2;
//				TRACE("[chaos] << %d Channel Wave Info >>\n", GetChannelNo()+1);
//				TRACE("[chaos].......Head : %d\n", WaveInfo.wHead);
//				TRACE("[chaos]....Command : %d\n", WaveInfo.wCommand);
//				TRACE("[chaos].....Length : %d\n", WaveInfo.wLength);
//				TRACE("[chaos]...CheckSum : %d\n", WaveInfo.wCheckSum);
//				TRACE("\n");
				
//				TRACE("[chaos] << %d Channel Wave Data >>\n", GetChannelNo()+1);
//				TRACE("[chaos].......Head : %d\n", WaveData2X.sDeviceHeader);
//				TRACE("[chaos]....Command : %d\n", WaveData2X.sDataHeader);
//				TRACE("[chaos].....Length : %d\n", nDataCount);
//				TRACE("[chaos]......Scale : %d\n", WaveData2X.sScale);
//				TRACE("[chaos]........VOP : %d\n", WaveData2X.sVop);
//				TRACE("[chaos]..Threshold : %d\n", WaveData2X.sThreshold);
//				TRACE("[chaos].Pulsewidth : %d\n", GetPulseWidth(WaveData2X.sPulseWidth));
//				TRACE("[chaos]......Start : %d\n", WaveData2X.nStartPosition);
//				TRACE("[chaos].......Name : %s\n", WaveData2X.szName);
//				TRACE("\n");

				WaveData.wHead		= WaveData2X.sDeviceHeader;
				WaveData.wCommand	= WaveData2X.sDeviceHeader;
				WaveData.wLength	= WaveData2X.sDataValueCount;
				WaveData.wScale		= WaveData2X.sScale;
				WaveData.wVop		= WaveData2X.sVop;
				WaveData.wThreshold = WaveData2X.sThreshold;
				WaveData.wPulseWidth  = WaveData2X.sPulseWidth;
				WaveData.dwStart	= WaveData2X.nStartPosition;
			}
		}

		ORG_TYPE* pData = new ORG_TYPE[nDataCount];		
		
		nDataLength = nDataCount * sizeof(ORG_TYPE); // Bytes
		memset(pData, 0x00, nDataLength);

		/*---------------------------------------------------------------------
		 * NanoWave DataValue 읽기
		 *-------------------------------------------------------------------*/
		nRead = File.Read(pData, nDataLength);

		if(nRead != (nDataCount * sizeof(ORG_TYPE))) {
			if(pData) {
				delete pData;
				pData = NULL;
			}
			TRACE( "손상된 WAVE 화일입니다(%ld/%ld).\n", nRead, nDataLength);
			return FALSE;
		}
		
		// Ver 2x1.0
		if(WaveInfo.wHead == 0x070b) {
			/*---------------------------------------------------------------------
			 * NanoWave End 자료 읽기
			 *-------------------------------------------------------------------*/
			nRead = File.Read((void*)&WaveData2XEnd, sizeof(NANOWAVE_2X_END));
			if(nRead != sizeof(NANOWAVE_2X_END)) {
				TRACE("NanoWave End 자료 읽기 읽기 실패\n");
			}
		}

		int nFileSize=0;
		File.SeekToEnd();
		nFileSize = File.GetPosition();
		File.Seek(nFileSize - sizeof(NANOWAVE_EX), CFile::begin);
		/*---------------------------------------------------------------------
		 * NanoWave Data 자료 읽기
		 *-------------------------------------------------------------------*/
		nRead = File.Read((void*)&NanowaveEx, sizeof(NANOWAVE_EX));

		File.Close();
		/*---------------------------------------------------------------------
		 *	측정데이터를 멤버변수에 복사
		 *-------------------------------------------------------------------*/
		memcpy((void*)&m_WaveInfo, (void*)&WaveInfo, sizeof(NANOWAVE_INFO));
		memcpy((void*)&m_WaveData, (void*)&WaveData, sizeof(NANOWAVE_DATA));
		memcpy((void*)&m_NanowaveEx, (void*)&NanowaveEx, sizeof(NANOWAVE_EX));
			
		if(m_pData) {
			delete m_pData;
			m_pData = NULL;
		}

		m_pData = new ORG_TYPE[nDataCount];
		memset(m_pData, 0x00, nDataLength);
		memcpy((void*)m_pData, (void*)pData, nDataLength);
		
		// Temp 변수 반환
		if(pData) {
			delete pData;
			pData = NULL;
		}
		
		/*---------------------------------------------------------------------
		 *	채널별 측정데이타, 정보를 표시함.
		 *-------------------------------------------------------------------*/
		m_nScale		= m_WaveData.wScale;

		m_dVOP			= (double)m_WaveData.wVop / 10.0;	// VOP(%)
		m_dThreshold	= (double)m_WaveData.wThreshold;	// Threshold
		m_nPulseWidth	= (int)m_WaveData.wPulseWidth;		// Pulse Width
		m_wDataLength	= (WORD)nDataCount;				// Data Length

		TRACE("[chaos] -------------------------------------------------\n");
//		TRACE("     wFlag : 0(정상), 1(쇼트), 2(단선), 3(노이즈 파형), 4(노이즈 무시), 5(노이즈 파형)\n");
//		TRACE("wResStatus : %d(EVENT_NONE), %d(EVENT_OPEN), %d(EVENT_SHORT)\n", EVENT_NONE, EVENT_OPEN, EVENT_SHORT);
		TRACE("[chaos].......wFlag : %s(%d)\n", NanowaveEx.wFlag == 0 ? "정상" : 
												NanowaveEx.wFlag == 1 ? "쇼트" :
												NanowaveEx.wFlag == 2 ? "단선" : "노이즈", (int)NanowaveEx.wFlag);
		TRACE("[chaos]..wResStatus : %s(%d)\n", NanowaveEx.wResStatus == EVENT_OPEN ? "파손" :
												NanowaveEx.wResStatus == EVENT_SHORT ? "누수" : "정상" , NanowaveEx.wResStatus);
		TRACE("[chaos]...dResValue : %5.2f\n", NanowaveEx.dResValue);
		TRACE("[chaos] -------------------------------------------------\n");

		return TRUE;
	}
	else {
		TCHAR szError[1024]={0,};

		fileException.GetErrorMessage(szError, 1024);

		TRACE( "Can't open file, Error Code = %u\n Error Message = %s\n", 
			fileException.m_cause,
			szError);
	}
	return FALSE;
}


/*-----------------------------------------------------------------------------  
 * 함 수 명 : double CGraphData::ConvertToMeter()
 * 목    적 : 
 * 설    명 : Meter 단위로 변환
 * 작 성 자 : 김영규
 * 작성일자 : 2004.11.15
 * 변경사항 :
 *---------------------------------------------------------------------------*/
double CGraphData::ConvertToMeter(double dValue)
{
	double dMeter;

	if(m_nScale == 0) 
		return 0;

	dMeter = (double)dValue * (m_nScale / 100.0) * (m_dVOP / 66.7);

	return dMeter;
}


/*-----------------------------------------------------------------------------  
 * 함 수 명 : void SetChannel()
 * 목    적 : 채널을 설정한다.
 * 설    명 : 
 * 작 성 자 : 김영규
 * 작성일자 : 2006.03.02
 * 변경사항 : 
            - nChannel     : [0|1|2|3]
            - m_nChannelNo : [0|1|2|3]
 *---------------------------------------------------------------------------*/
void CGraphData::SetChannel(int nChannel)
{
	m_nChannelNo = nChannel; 
}

/*-----------------------------------------------------------------------------  
 * 함 수 명 : BOOL CGraphData::SetBasePath()
 * 목    적 : 
 * 설    명 : 
 * 작 성 자 : 김영규
 * 작성일자 : 2004.12.27
 * 변경사항 :
 *---------------------------------------------------------------------------*/
BOOL CGraphData::SetBasePath(CString csPathName)
{
	if(GetChannelNo() < 0 && GetChannelNo() >= 4) {
		GetChannel(csPathName);
	}

	/*-------------------------------------------------------------------------
	 *	Base 화일을 읽어옴.
	 *-----------------------------------------------------------------------*/
	m_bBaseLoaded = FALSE;
	if(ReadBaseFile(csPathName) == FALSE) {
		return FALSE;
	}
	m_bBaseLoaded = TRUE;

	if(m_bBaseLoaded && m_bMeasureLoaded) {
		m_bUseDiff = TRUE;
	}

	return TRUE;
}

/*-----------------------------------------------------------------------------  
 * 함 수 명 : BOOL CGraphData::SetDataPath()
 * 목    적 : WaveData File Path 지정
 * 설    명 : - Data Path 지정하고, 패스 정보에서 채널번호를 얻음
              - 데이타 화일을 읽음.
 * 작 성 자 : 김영규
 * 작성일자 : 2004.11.15
 * 변경사항 :
 *---------------------------------------------------------------------------*/
BOOL CGraphData::SetDataPath(CString csPathName)
{
	if(GetChannelNo() < 0 && GetChannelNo() >= 4) {
		GetChannel(csPathName);
	}

	/*-------------------------------------------------------------------------
	 *	측정화일을 읽음
	 *-----------------------------------------------------------------------*/
	m_bMeasureLoaded = FALSE;
	if(ReadMeasureFile(csPathName) == FALSE) {
		return FALSE;
	}
	m_bMeasureLoaded = TRUE;

	m_bUseDiff = FALSE;
	if(m_bBaseLoaded && m_bMeasureLoaded) {
		m_bUseDiff = TRUE;
	}
	return TRUE;
}
/*-----------------------------------------------------------------------------  
 * 함 수 명 : BOOL CGraphData::ReadBaseFile()
 * 목    적 : Base 그래프 화일을 읽음.
 * 설    명 : 
              1. 프런트 가비지 데이타 부분을 제거함.
			  2. Smoothing 필터 적용
 * 작 성 자 : 김영규
 * 작성일자 : 2004.12.02
 * 변경사항 :
 *---------------------------------------------------------------------------*/
BOOL CGraphData::ReadBaseFile(CString csPathName)
{
	double* pWaveData=NULL;
	int nStart	= 0;
	int nPos	= 0;
	int nLength	= 0;
	char sFilename[MAX_PATH]={0,};

	if(csPathName == m_BaseInfo.csPathname) {
		return FALSE;
	}

	GetFilename(csPathName, sFilename);
	m_BaseInfo.csFilename = sFilename;
	m_BaseInfo.csPathname = csPathName;

	// 데이타 화일 읽기
	if(!ReadDataFile(m_BaseInfo.csPathname)) {
		return FALSE;
	}

	// 연산용 임시 그래프 저장변수 생성.
	nLength = GetDataLength();
	
	// 1. 원시 그래프 데이타
	if(m_BaseInfo.pRawData)	{
		delete m_BaseInfo.pRawData;
		m_BaseInfo.pRawData = NULL;
	}
	m_BaseInfo.pRawData = new double[nLength];

	if(m_BaseInfo.pRawData == NULL)	{
		AfxMessageBox("Memory 할당 에러");
		return FALSE;
	}

	memset((void*)m_BaseInfo.pRawData, 0x00, sizeof(double) * nLength);
	double dMaxValue=-99999999.;
	int nMaxPos=0;
	for(nPos=0;nPos<nLength;nPos++) {
		m_BaseInfo.pRawData[nPos] = (double)m_pData[nPos];
		if(dMaxValue < m_BaseInfo.pRawData[nPos])
		{
			nMaxPos = nPos;
			dMaxValue = m_BaseInfo.pRawData[nPos];
		}
	}
	// 인가 신호(Output signal)
	m_BaseInfo.nOutputSignalPos = nMaxPos;
	m_BaseInfo.dOutputSignal = dMaxValue;

	pWaveData = new double[nLength];
	if(pWaveData == NULL) {
		AfxMessageBox("Memory 할당 에러");
		return FALSE;
	}

	// 노이즈 제거 : Smoothing method
	Savitzky_Golay_Filter(m_pData, pWaveData);
	nLength = m_wDataLength = RemoveRearNoise(pWaveData);

	// 초기 노이즈 제거
	FindMinMax(pWaveData);
	nStart = RemoveFrontNoise(m_BaseInfo.pRawData, m_EventInfo.ptMax.x);
	m_BaseInfo.nStart	= nStart;
	m_BaseInfo.nLength	= nLength;
 
	// 2. 필터링(Smoothing)된 그래프 데이타
	if(m_BaseInfo.pWaveData)	{
		delete m_BaseInfo.pWaveData;
		m_BaseInfo.pWaveData = NULL;
	}
	m_BaseInfo.pWaveData = new double[nLength];

	if(m_BaseInfo.pWaveData == NULL)	{
		AfxMessageBox("Memory 할당 에러");
		return FALSE;
	}

	memset((void*)m_BaseInfo.pWaveData, 0x00, sizeof(double) * nLength);
	memcpy(m_BaseInfo.pWaveData, (void*)&pWaveData[0], sizeof(double)* nLength);

	if(pWaveData) {
		delete pWaveData;
		pWaveData = NULL;
	}

	// PulseWidth
	m_BaseInfo.nPulseWidth = GetPulseWidth(m_WaveData.wPulseWidth);

	// 최소값, 최대값 를 구한다.
	FindMinMax(m_BaseInfo.pWaveData);
	m_BaseInfo.ptMax = m_EventInfo.ptMax;
	m_BaseInfo.ptMin = m_EventInfo.ptMin;

	return TRUE;
}

/*-----------------------------------------------------------------------------  
 * 함 수 명 : BOOL CGraphData::ReadMeasureFile()
 * 목    적 : 측정데이타 화일 일기
 * 설    명 : 
              1. 프런트 가비지 데이타 부분을 제거함.
			  2. Smoothing 필터 적용
 * 작 성 자 : 김영규
 * 작성일자 : 2004.12.08
 * 변경사항 :
 *---------------------------------------------------------------------------*/
BOOL CGraphData::ReadMeasureFile(CString csPathName)
{
	double* pWaveData=NULL;
	int nPos	= 0;
	int nStart	= 0;
	int nLength	= 0;
	char sFilename[MAX_PATH]={0,};

	GetFilename(csPathName, sFilename);
	m_MeasureInfo.csFilename = sFilename;
	m_MeasureInfo.csPathname = csPathName;

	// 데이타 화일 읽기
	if(!ReadDataFile(m_MeasureInfo.csPathname)) {
		return FALSE;
	}
	
	nLength = GetDataLength();

	// 1. 원시 데이타를 전역 변수에 저장
	if(m_MeasureInfo.pRawData)	{
		delete m_MeasureInfo.pRawData;
		m_MeasureInfo.pRawData = NULL;
	}
	m_MeasureInfo.pRawData = new double[nLength];

	if(m_MeasureInfo.pRawData == NULL)	{
		AfxMessageBox("Memory 할당 에러");
		return FALSE;
	}
	
	memset((void*)m_MeasureInfo.pRawData, 0x00, sizeof(double) * nLength);
	double dMaxValue=-99999999.;
	int nMaxPos=0;
	for(nPos=0;nPos<nLength;nPos++) 
	{
		m_MeasureInfo.pRawData[nPos] = (double)m_pData[nPos];
		if(dMaxValue < m_MeasureInfo.pRawData[nPos])
		{
			nMaxPos = nPos;
			dMaxValue = m_MeasureInfo.pRawData[nPos];
		}
	}
	// 인가 신호(Output signal)
	m_MeasureInfo.nOutputSignalPos = nMaxPos;
	m_MeasureInfo.dOutputSignal = dMaxValue;
	
	// 메모리 할당
	pWaveData = new double[nLength];
	if(pWaveData == NULL) {
		AfxMessageBox("Memory 할당 에러");
		return FALSE;
	}
	// 노이즈 제거 : Smoothing method
	Savitzky_Golay_Filter(m_pData, pWaveData);
	nLength = m_wDataLength = RemoveRearNoise(pWaveData);

	// 초기 노이즈 제거
	FindMinMax(pWaveData);
	nStart = RemoveFrontNoise(m_MeasureInfo.pRawData, m_EventInfo.ptMax.x);
	m_MeasureInfo.nStart	= nStart;
	m_MeasureInfo.nLength	= nLength;
	
	// 2. 필터링(Smoothing)된 그래프 데이타를 전역 변수에 저장
	if(m_MeasureInfo.pWaveData)	{
		delete m_MeasureInfo.pWaveData;
		m_MeasureInfo.pWaveData = NULL;
	}
	m_MeasureInfo.pWaveData = new double[nLength];

	if(m_MeasureInfo.pWaveData == NULL)	{
		AfxMessageBox("Memory 할당 에러");
		return FALSE;
	}
	
	memset((void*)m_MeasureInfo.pWaveData, 0x00, sizeof(double) * nLength);
	memcpy(m_MeasureInfo.pWaveData, (void*)&pWaveData[0], sizeof(double) * nLength);

	if(pWaveData) {
		delete pWaveData;
		pWaveData = NULL;
	}
	
	// PulseWidth
	m_MeasureInfo.nPulseWidth = GetPulseWidth(m_WaveData.wPulseWidth);

	// 최소값, 최대값 를 구한다.
	FindMinMax(m_MeasureInfo.pWaveData);
	m_MeasureInfo.ptMax = m_EventInfo.ptMax;
	m_MeasureInfo.ptMin = m_EventInfo.ptMin;

	return TRUE;
}


int CGraphData::GetRepeatCount()
{
	if(m_nRepeatCount <=0) 
		return 2;

	return m_nRepeatCount;
}

void CGraphData::SetRepeatCount(int nNewRepeatCount)
{
	m_nRepeatCount = nNewRepeatCount;
}
/*-----------------------------------------------------------------------------  
 * 함 수 명 : void CGraphData::Savitzky_Golay_Filter()
 * 목    적 : 
 * 설    명 : Savitzky-Golay Filter 와 5-point unweighted smooth Algorithm를
              조합하여 노이즈를 제거함.
 * 작 성 자 : 김영규
 * 작성일자 : 2004.11.6
 * 변경사항 :
 *---------------------------------------------------------------------------*/
void CGraphData::Savitzky_Golay_Filter(long* pSrcData, double* pResultData)
{
	int nFilterWidth = 0;
	int nMidPoint	 = 0;
	int nLength		 = 0;
	int nRepeatCount = 0;
	register int nRepeat= 0;
	register int nIndex	= 0;

	double dAvg=0.;
	double dTotal=0.;
	double dValueTotal=0.;
	double dFilterTotal=0.;
	
	double dFilter_5[]  = { -3,  12, 17, 12, -3 };
	double dFilter_7[]  = { -2,   3,  6,  7,  6,  3, -2};
	double dFilter_9[]  = { -21, 14, 39, 54, 59, 54,  3, 14, -21 };
	double dFilter_11[] = { -36,  9, 44, 69, 84, 89, 84, 69,  44,  9, -36 };

	
	// 반복 횟수
	nRepeatCount = GetRepeatCount();

	// Data Length
	nLength = GetDataLength();
	for(nIndex=0; nIndex<nLength; nIndex++) {
		pResultData[nIndex] = (double)pSrcData[nIndex];
	}

	nFilterWidth = 11;
	nMidPoint = nFilterWidth/2;
	
	/*-------------------------------------------------------------------------
	 * Savitzky-Golay Filter	
	 *-----------------------------------------------------------------------*/
	for(nRepeat=0; nRepeat<nRepeatCount; nRepeat++) {
		for(nIndex=nMidPoint; nIndex<nLength-nMidPoint; nIndex++) {
			dValueTotal		= 0.;
			dFilterTotal	= 0.;
			dAvg			= 0.;

			// FilterWidth 개 만큼의 값의 평균을 구한다.
			for(int i= (-1) * nMidPoint; i<= nMidPoint; i++) {
				dFilterTotal += dFilter_7[i+nMidPoint];
				dValueTotal	 += dFilter_7[i+nMidPoint] * pResultData[nIndex+i];
			}

			dAvg = (double)(dValueTotal / (double)dFilterTotal);

			// 구해진 평균값을 3번째 항목의 값으로 설정한다.
			pResultData[nIndex + nMidPoint] = dAvg;		
		}
	}

	for(nIndex=nLength-nMidPoint; nIndex<nLength; nIndex++) {
		pResultData[nIndex] = dAvg;		
	}

	/*-------------------------------------------------------------------------
	 *	Moving Average Filter
	 *-----------------------------------------------------------------------*/
	
	nRepeatCount	= GetRepeatCount();	// 반복 횟수
	nFilterWidth	= MA_FILTER_WIDTH;	// Filter Width
	nMidPoint		= nFilterWidth/2;	// Mid Point Pos

	for(nRepeat=0; nRepeat<nRepeatCount; nRepeat++) {
		for(nIndex=0; nIndex<nLength - nMidPoint;nIndex++) {
			dTotal	= 0.;
			dAvg	= 0.;

			// FilterWidth 개 만큼의 값의 평균을 구한다.
			for(int i=0; i<nFilterWidth; i++) {
				dTotal += (double)pResultData[nIndex + i];
			}

			dAvg = (double)(dTotal / (double)nFilterWidth);
			
			// 구해진 평균값을 3번째 항목의 값으로 설정한다.
			pResultData[nIndex + nMidPoint] = dAvg;
		}
	}

	for(nIndex=nLength-nMidPoint; nIndex<nLength; nIndex++) {
		pResultData[nIndex] = dAvg;		
	}
}


/*-----------------------------------------------------------------------------  
 * 함 수 명 : void CGraphData::CalcDistance(EVENT_MODE uEventMode, double dResValue) 
 * 목    적 : 
 * 설    명 : 피크점을 탐색하는 범위를 저항값을 기준으로 하게 한다.
 * 작 성 자 : 김영규
 * 작성일자 : 2004.12.09
 * 변경내용 : 
 *---------------------------------------------------------------------------*/
BOOL CGraphData::CalcDistance(EVENT_MODE uEventMode, EVENTSCAN_INFO infoEventScan) 
{
	BOOL bRet=FALSE;

	memcpy((EVENTSCAN_INFO*)&m_EventScanInfo, (EVENTSCAN_INFO*)&infoEventScan, sizeof(EVENTSCAN_INFO));


	if(m_EventScanInfo.dResValue >= 999900) {
		TRACE ( "감지선 BNC연결잭이 분리 되었는지 확인해 보세요.\n");
	}

	bRet = CalcDistance(uEventMode);

	return bRet;
}

/*-----------------------------------------------------------------------------  
 * 함 수 명 : BOOL CGraphData::CalcDistance()
 * 목    적 : 
 * 설    명 : 거리를 계산한다.
 * 작 성 자 : 김영규
 * 작성일자 : 2004.12.09
 * 변경내용 : - 원본데이타(pRawData)에서 피크점을 구하고 그 값을 이용하여 보정계수를 구할 경우 
                m_MeasureInfo.pRawData 를 CalcDistance() 의 인수로 사용하고
				필터 데이타(pWaveData)를 이용했을 경우 m_MeasureInfo.pWaveData를 
				CalcDistance() 의 인수로 사용해야 함.
				2007.10.4 김영규
 *---------------------------------------------------------------------------*/
BOOL CGraphData::CalcDistance(EVENT_MODE uEventMode)
{
	m_uEventMode = uEventMode;

	if(m_bMeasureLoaded) {
		return CalcDistance(m_MeasureInfo.pWaveData);
	}
	else if(m_bBaseLoaded){
		return CalcDistance(m_BaseInfo.pWaveData);
	}
	return FALSE;
}

/*-----------------------------------------------------------------------------  
 * 함 수 명 : BOOL CGraphData::CalcDistance()
 * 목    적 : 이벤트가 발생한 지점까지의 거리계산
 * 설    명 : 주어진 웨이브 데이타를 분석하여 이벤트가 발생한 지점까지의 거리계산
 * 작 성 자 : 김영규
 * 작성일자 : 2004.11.15
 * 변경사항 :
 *---------------------------------------------------------------------------*/
BOOL CGraphData::CalcDistance(double* pWaveData)
{
	int	nEventPos = 0;

	// 측정 데이타가 있는지 검사
	if(!pWaveData) return FALSE;

	// 시작점
	m_EventInfo.nStart = 0;

	// Pulse Width
	m_EventInfo.nPulseWidth = GetPulseWidth(GetPulseWidthCode());

	/*-------------------------------------------------------------------------
	 * 이벤트 위치 탐색
	 *-----------------------------------------------------------------------*/
	nEventPos = DetectEvent(pWaveData);	
	if(nEventPos <= 0) {
		return FALSE;	// Event 아님
	}
	
	if(nEventPos > m_MeasureInfo.nLength) {
		nEventPos = 0;
	}
	m_EventInfo.nEvent = nEventPos;
	// Volt
	//  : Event 위치의 값과 시작점에서의 값의 차이
	m_dVolt = AbsValue(pWaveData[m_EventInfo.nStart] - pWaveData[m_EventInfo.nEvent]);
	
	// dBRL : 유실율
	//  : 20 * log10(피크점에서의 값과 시작점에서의 값의 차이)
	double dBRL = AbsValue(pWaveData[m_EventInfo.ptMax.x] - pWaveData[m_EventInfo.nStart]);
	m_dBRL = 20 * log10(dBRL);

	// 이벤트 지점까지의 거리
	m_nDotDistance = abs(m_EventInfo.nEvent);
	return TRUE;
}


/*-----------------------------------------------------------------------------  
 * 함 수 명 : int FindMax()
 * 목    적 : 범위 이후의 최대값 탐색
 * 설    명 : 
 * 작 성 자 : 김영규
 * 작성일자 : 2004.12.10
 * 변경사항 :
 *---------------------------------------------------------------------------*/
int CGraphData::FindMax(int nStart, int nEnd, double* pWaveData)
{
	int		i			= 0;
	int		nMaxValueX	= 0; 
	double	dMaxValueY	= 0;
	int		nDateLength = 0;

	nDateLength = nEnd;

	/*-------------------------------------------------------------------------
	 * Max Peak 위치 탐색
	 *-----------------------------------------------------------------------*/
	for(i=nStart; i<nDateLength; i++) {
		// 최고점
		if(dMaxValueY < pWaveData[i]) {
			nMaxValueX = i;
			dMaxValueY = pWaveData[i];
		}
	}

	// 최대치 위치의 검증
	// 2005.2.27 김영규
	if(nStart == nMaxValueX) {
		for(i=nStart+6; i<nDateLength-6; i++) {

			// 최고점
			double dDiff = pWaveData[i+6] - pWaveData[i-6];
			if(dDiff >= 0 ) {
				break;
			}
		}
		dMaxValueY	= 0;
		for(; i<nDateLength; i++) {
			// 최고점
			if(dMaxValueY < pWaveData[i]) {
				nMaxValueX = i;
				dMaxValueY = pWaveData[i];
			}
		}
		
	}
	return nMaxValueX;
}

/*-----------------------------------------------------------------------------  
 * 함 수 명 : double CGraphData::CalcAverage()
 * 목    적 : 범위내의 데이타의 평균값 계산
 * 설    명 : 
 * 작 성 자 : 김영규
 * 작성일자 : 2005.02.24
 * 변경사항 :
 *---------------------------------------------------------------------------*/
double CGraphData::CalcAverage(int nStart, int nEnd, double* pWaveData)
{
	register short i = 0;
	double	dTotal = 0; 
	double	dAvg = 0;

	for(i=nStart; i<nEnd; i++) {
		if(pWaveData[i] > 0) {
			dTotal += pWaveData[i];
		}
	}
	dAvg = dTotal / (nEnd - nStart + 1);
	return dAvg;
}

/*-----------------------------------------------------------------------------  
 * 함 수 명 : int FindMax()
 * 목    적 : 범위 이후의 최대값 탐색
 * 설    명 : 
 * 작 성 자 : 김영규
 * 작성일자 : 2004.12.10
 * 변경사항 : Diffrence 데이타에서는 사용할수 없음. 2005.2.24 김영규
 *---------------------------------------------------------------------------*/
int CGraphData::FindMin(int nStart, int nEnd, double* pWaveData)
{
	int		i			= 0;
	int		nMinValueX	= 0; 
	double	dMinValueY	= 9999999999;
	int		nDateLength = 0;

	nDateLength = nEnd;

	/*-------------------------------------------------------------------------
	 * Min Peak 위치 탐색
	 *-----------------------------------------------------------------------*/
	for(i=nStart; i<nDateLength; i++) {
		// 최소점
		if(dMinValueY > pWaveData[i] && pWaveData[i] > 0) {
			nMinValueX = i;
			dMinValueY = pWaveData[i];
		}
	}
	return nMinValueX;
}
/*-----------------------------------------------------------------------------  
 * 함 수 명 : int CGraphData::FindMinMax()
 * 목    적 : 전구간에서 최고점/최저점을 구함.
 * 설    명 : 
 * 작 성 자 : 김영규
 * 작성일자 : 2004.11.17
 * 변경사항 :
 *---------------------------------------------------------------------------*/
void CGraphData::FindMinMax(double* pWaveData)
{
	int		i			= 0;
	int		nMinValueX	= 0; 
	double	dMinValueY	= 999999999;
	int		nMaxValueX	= 0; 
	double	dMaxValueY	= 0;
	int		nDateLength = 0;
	int		nCount		= 0;
	double	dTotal		= 0.;
	int		nScanPos	= GetScanPos();

	nDateLength = m_wDataLength;

	dTotal = 0;
	nCount = 0;
	/*-------------------------------------------------------------------------
	 * Peak 위치 탐색
	 *-----------------------------------------------------------------------*/
	for(i=0; i<nDateLength; i++) {
		if(nScanPos <= i) {
			// 최고점
			if(dMaxValueY < pWaveData[i]) {
				nMaxValueX = i;
				dMaxValueY = pWaveData[i];
			}
			// 최저점
			if(dMinValueY > pWaveData[i] && pWaveData[i] > 0) {
				nMinValueX = i;
				dMinValueY = pWaveData[i];
			}
		}
		nCount ++;
		dTotal += pWaveData[i];
	}
	m_dAvgValue = dTotal / (double)nCount;
	/*-------------------------------------------------------------------------
	 *	최저점
	 *-----------------------------------------------------------------------*/
	m_EventInfo.ptMin.x = nMinValueX;
	m_EventInfo.ptMin.y = (long)dMinValueY;

	/*-------------------------------------------------------------------------
	 *	최고점
	 *-----------------------------------------------------------------------*/
	m_EventInfo.ptMax.x = nMaxValueX;
	m_EventInfo.ptMax.y = (long)dMaxValueY;
}



/*-----------------------------------------------------------------------------  
 * 함 수 명 : int CGraphData::GetBorderOfZone()
 * 목    적 : 근거리/원거리 감시구간 경계를 참조하는 함수
 * 설    명 : 
 * 작 성 자 : 김영규
 * 작성일자 : 2005.06.13
 * 변경사항 : 500ns 이하의 펄스를 사용할 경우 0 ~ BORDER_OF_ZONE 까지 감시한다.
              500ns 이상의 펄스는 BORDER_OF_ZONE 이상을 감시한다.
			  
			- 픽셀 값을 거리값으로 환산하여 사용. 2006.5.1 김영규
 *---------------------------------------------------------------------------*/
int CGraphData::GetBorderOfZone(int nPulseWidth) 
{
	int nStartDot=0;
	double dStartMeter = DIFF_StartPos(nPulseWidth);		

	nStartDot = MeterToDot(dStartMeter);
	return nStartDot;
}

/*-----------------------------------------------------------------------------  
 * 함 수 명 : int CGraphData::MeterToDot()
 * 목    적 : 
 * 설    명 : 
 * 작 성 자 : 김영규
 * 작성일자 : 2006.05.10
 * 변경사항 :
 *---------------------------------------------------------------------------*/
int	CGraphData::MeterToDot(double dValue)
{
	int nValue=0;

	nValue = (int)(dValue * (66.7 / (m_nScale*m_dVOP)) * 100.0);
	return nValue;
}

/*-----------------------------------------------------------------------------  
 * 함 수 명 : double CGraphData::DotToMeter()
 * 목    적 : 
 * 설    명 : 
 * 작 성 자 : 김영규
 * 작성일자 : 2006.05.10
 * 변경사항 :
 *---------------------------------------------------------------------------*/
double CGraphData::DotToMeter(int nValue)
{
	double dValue=0;
	dValue = nValue * (m_nScale/100.0) * (m_dVOP/66.7);
	return dValue;
}


/*-----------------------------------------------------------------------------  
 * 함 수 명 : void CGraphData::MakeDiffData()
 * 목    적 : 차 (측정 데이타 - 베이스 데이타) 그래프 데이타
 * 설    명 : 
 * 작 성 자 : 김영규
 * 작성일자 : 2004.12.02
 * 변경사항 :
 *---------------------------------------------------------------------------*/
void CGraphData::MakeDiffData(int nStart, int nEnd, double * pDiffData, int* npMin, int* npMax)
{
	register int i=0;
	int		nMinValueX	= 0; 
	double	dMinValueY	= 99999999;
	int		nMaxValueX	= 0; 
	double	dMaxValueY	= -99999999;
	double  dValue		= 0;
	int		nDisplayGap = 0;

	nDisplayGap = (m_MeasureInfo.nStart - m_BaseInfo.nStart);

	// 이벤트 타입과 저항값에 따라 피크를 탐색하는 범위를 결정한다.
	// 2012년 2월 3일 김영규
	if(m_uEventMode == EVENT_OPEN) {
		if(m_EventScanInfo.dResValue < m_EventScanInfo.dOpenResValue) {
			nStart = m_EventScanInfo.nOpenScanPos; // 1900px = 4000m
		}
	}

	// 각 그래프의 보정된 시작위치로 부터 Difference 구함.
	for(i=nStart; i<nEnd; i++) {
		if(i+nDisplayGap < m_BaseInfo.nLength) {

			pDiffData[i] = m_MeasureInfo.pWaveData[i] - m_BaseInfo.pWaveData[i+nDisplayGap];

			// 최고점
			if(dMaxValueY < pDiffData[i]) {
				nMaxValueX = i;
				dMaxValueY = pDiffData[i];
			}
			// 최저점
			if(dMinValueY > pDiffData[i]) {
				nMinValueX = i;
				dMinValueY = pDiffData[i];
			}
		}
	}

	if(nMinValueX == nStart) {
		nMinValueX = 0;
	}
	if(nMaxValueX == nStart) {
		nMaxValueX = 0;
	}

	*npMin = nMinValueX;
	*npMax = nMaxValueX;
}


/*-----------------------------------------------------------------------------  
 * 함 수 명 : int CGraphData::DetectEvent()
 * 목    적 : 
 * 설    명 : 
 * 작 성 자 : 김영규
 * 작성일자 : 2006.11.20
 * 변경사항 : 
			- 아산 둔포 신항리에 배포하기 위해 수정함. 2006.11.20
			- 피크점을 구하고 피크점을 보정공식에 대입하여 변곡점을 구한다. 
			  2006.11.21 김영규
 *---------------------------------------------------------------------------*/
int CGraphData::DetectEvent(double* pWaveData)
{
	int SCAN_WIDTH=64;
	int nPulseWidth = GetPulseWidth(GetPulseWidthCode());
	int nLength = GetDataLength();
	int nStart = GetBorderOfZone(nPulseWidth) > 350 ? 350 : GetBorderOfZone(nPulseWidth);
//	int nStart = GetBorderOfZone(nPulseWidth);
	int nEnd = 0;
	int nEventPos = 0;
	int nMaxPos = 0;
	int nMinPos = 0;	
	double *pDiffData = NULL;
	
	if(m_uEventMode == EVENT_NONE) {
		return nEventPos;
	}
	m_EventInfo.uEventMode = m_uEventMode;

	if(m_BaseInfo.nLength > m_MeasureInfo.nLength)
		nEnd = m_MeasureInfo.nLength;
	else
		nEnd = m_BaseInfo.nLength;
	
	pDiffData = new double[nLength];
	memset((void*)pDiffData, 0x00, sizeof(double)*nLength);

	// 2. 해당 구간의 Min, Max 위치를 구한다.
	int nScanMin=0;
	int nScanMax=0;

	nMinPos = 0;
	nMaxPos = 0;

	// RawData 간의 Diff 구하기
	MakeDiffData(nStart, nEnd, pDiffData, &nMinPos, &nMaxPos);
	TRACE ( "c. nStart=%d, nEnd=%d, nMinPos=%d, nMaxPos=%d, m_nEventMode=%d\n", nStart, nEnd, nMinPos, nMaxPos, m_uEventMode);
	if(nMinPos>=0 && nMaxPos>=0) {
		m_EventInfo.uEventMode = m_uEventMode;
		for(int nRepeat=0; nRepeat<3; nRepeat++) 
		{
			if(m_uEventMode == EVENT_OPEN) 
			{
				m_nPeakPoint = nMaxPos;
				TRACE ( "d. [OPEN] m_nPeakPoint(%d pxl, %.1f m)\n", m_nPeakPoint, DotToMeter(m_nPeakPoint));
				nScanMin = m_nPeakPoint - SCAN_WIDTH > nStart ? m_nPeakPoint - SCAN_WIDTH : nStart;
				nScanMax = m_nPeakPoint + SCAN_WIDTH < nEnd   ? m_nPeakPoint + SCAN_WIDTH : nEnd;
			}
			else if(m_uEventMode == EVENT_SHORT) 
			{
				m_nPeakPoint = nMinPos;
				TRACE ("d. [SHORT] m_nPeakPoint(%d pxl, %.1f m)\n", m_nPeakPoint, DotToMeter(m_nPeakPoint));
				nScanMin = m_nPeakPoint - SCAN_WIDTH > nStart ? m_nPeakPoint - SCAN_WIDTH : nStart;
				nScanMax = m_nPeakPoint + SCAN_WIDTH < nEnd   ? m_nPeakPoint + SCAN_WIDTH : nEnd;
			}
			
			///////////////////////////////////////////////////////////////	
			// V, N자형 패턴 검증
			///////////////////////////////////////////////////////////////
			double dPeakValue=0;
			register int nPos=0;
			
			// 단선
			if(m_EventInfo.uEventMode == EVENT_OPEN) 
			{ 
				///////////////////////////////////////////////////////////
				// - m_nPeakPoint의 재조정 : 전처리한 파형데이타(pWaveData)에서  
				//    이벤트 위치 근방의 일정영역 안에서 피크위치를 재 탐색한다. 
				// 2006.5.19 김영규
				///////////////////////////////////////////////////////////
				dPeakValue = -99999999;
				nMaxPos = 0;
				for(nPos=nScanMin;nPos<nScanMax;nPos++) 
				{
					// max
					if(pWaveData[nPos] > dPeakValue) 
					{
						dPeakValue = pWaveData[nPos];
						nMaxPos = nPos;
					}
				}
				if(nMaxPos > 0) {
					m_nPeakPoint = nMaxPos;
				}
			}
			// 쇼트
			else 
			{						
				///////////////////////////////////////////////////////////
				// - m_nPeakPoint의 재조정 : RawData를 이벤트 위치 근방의  
				//   일정영역 안에서 피크위치를 재 탐색한다. 
				///////////////////////////////////////////////////////////
				dPeakValue = 99999999;
				nMinPos = 0;
				for(nPos=nScanMin;nPos<nScanMax;nPos++) 
				{
					// min
					if(pWaveData[nPos] < dPeakValue) 
					{
						dPeakValue = pWaveData[nPos];
						nMinPos = nPos;
					}
				}
				if(nMinPos > 0 && nMinPos != nScanMin && nMinPos != (nScanMax-1)) {
					m_nPeakPoint = nMinPos;
				}
				else {
					nMinPos = m_nPeakPoint;
					dPeakValue = pWaveData[nMinPos];
					m_nPeakPoint = nMinPos;
				}
			}
		}

		/*-------------------------------------------------------------------------
		 * 측정 거리 보정
		 *-----------------------------------------------------------------------*/
		nEventPos = DistanceCorrection(m_nPeakPoint, m_EventInfo.uEventMode);

		m_EventInfo.ptMax.x = nMaxPos;
		m_EventInfo.ptMax.y = (long)pDiffData[nMaxPos];
		m_EventInfo.ptMin.x = nMinPos;
		m_EventInfo.ptMin.y = (long)pDiffData[nMinPos];

		m_nMinPoint = nMinPos;
		m_nMaxPoint = nMaxPos;

	}

	if(pDiffData) 
	{
		delete pDiffData;
		pDiffData = NULL;
	}

	return nEventPos;
}

/*-----------------------------------------------------------------------------  
 * 함 수 명 : void CGraphData::SetBaseMethod()
 * 목    적 : 
 * 설    명 : bIsOpenBase = TRUE : 베이스 화일은 OPEN
 * 작 성 자 : 김영규
 * 작성일자 : 2006.05.02
 * 변경사항 :
 *---------------------------------------------------------------------------*/
void CGraphData::SetBaseMethod(BOOL bIsOpenBase)
{
	m_bOpenBaseMethod = bIsOpenBase;
}

/*-----------------------------------------------------------------------------  
 * 함 수 명 : BOOL CGraphData::GetBaseMethod()
 * 목    적 : 
 * 설    명 : 베이스 화일이 OPEN 이면 TRUE 반환
 * 작 성 자 : 김영규
 * 작성일자 : 2006.05.02
 * 변경사항 :
 *---------------------------------------------------------------------------*/
BOOL CGraphData::GetBaseMethod()
{
	return m_bOpenBaseMethod;
}

/*-----------------------------------------------------------------------------  
 * 함 수 명 : int CGraphData::DistanceCorrection()
 * 목    적 : 측정거리의 보정
 * 설    명 : 먼저 LoadCoefficient() 를 호출해야함.
 * 작 성 자 : 김영규
 * 작성일자 : 2006.03.02
 * 변경사항 : 
			 - 측정 그래프의 필터 그래프를 이용하여 거리 보정 공식을 구해야 함.
			 - 피크점의 거리(m)와 감지선간의 공식을 구함.
             uMethod (0:, 1:)
 *---------------------------------------------------------------------------*/
int CGraphData::DistanceCorrection(int nEventPos, EVENT_MODE uEventMode)
{
	double dErrCor = 0;
	double dDist = 0;
	double dDistance = DotToMeter(nEventPos);
	int nChannel = GetChannelNo(); // {0,1,2,3}

	if(nEventPos <= 0) return 0;
	
	if(IsCoefficientLoaded()) 
	{
		if(m_strFitMethod[nChannel].Find("sinusoidal") != -1)
		{
			// 원거리
			/*-----------------------------------------------------------------
			 * Sinusoidal Fit: y=a+b*cos(cx+d)
			 *---------------------------------------------------------------*/
			TRACE ("g. 채널 %d :> 보정공식 :> y=a+b*cos(cx+d)\n", GetChannelNo()+1);

			switch(uEventMode) {
			case EVENT_OPEN:
				dDist = m_dValueA[m_nChannelNo]
					  + m_dValueB[m_nChannelNo] * cos(m_dValueC[m_nChannelNo] * dDistance + m_dValueD[m_nChannelNo]);

				TRACE ("A = %.15lf\n", m_dValueA[m_nChannelNo]);
				TRACE ("B = %.15lf\n", m_dValueB[m_nChannelNo]);
				TRACE ("C = %.15lf\n", m_dValueC[m_nChannelNo]);
				TRACE ("D = %.16lf\n", m_dValueD[m_nChannelNo]);
				break;

			case EVENT_SHORT:
				dDist = m_dValueAs[m_nChannelNo]
					  + m_dValueBs[m_nChannelNo] * cos(m_dValueCs[m_nChannelNo] * dDistance + m_dValueDs[m_nChannelNo]);
				TRACE ("As = %.15lf\n", m_dValueAs[m_nChannelNo]);
				TRACE ("Bs = %.15lf\n", m_dValueBs[m_nChannelNo]);
				TRACE ("Cs = %.15lf\n", m_dValueCs[m_nChannelNo]);
				TRACE ("Ds = %.16lf\n", m_dValueDs[m_nChannelNo]);
				break;
			default:
				return nEventPos;
			}
		}
		else {		
			/*-----------------------------------------------------------------
			 * Quadratic Fit : y = a + bx + cx^2 + dx^3
			 *---------------------------------------------------------------*/
			TRACE ("g. 채널 %d :> 보정공식 :> y = a + bx + cx^2 + dx^3 + ex^4\n", GetChannelNo()+1);

			switch(uEventMode) {
			case EVENT_OPEN:
				dDist = m_dValueA[m_nChannelNo]
					  + m_dValueB[m_nChannelNo] * dDistance 
					  + m_dValueC[m_nChannelNo] * pow(dDistance, 2) 
					  + m_dValueD[m_nChannelNo] * pow(dDistance, 3)
					  + m_dValueE[m_nChannelNo] * pow(dDistance, 4);
				TRACE ("A = %.15lf\n", m_dValueA[m_nChannelNo]);
				TRACE ("B = %.15lf\n", m_dValueB[m_nChannelNo]);
				TRACE ("C = %.15lf\n", m_dValueC[m_nChannelNo]);
				TRACE ("D = %.16lf\n", m_dValueD[m_nChannelNo]);
				TRACE ("E = %.23lf\n", m_dValueE[m_nChannelNo]);
				break;

			case EVENT_SHORT:
				dDist = m_dValueAs[m_nChannelNo]
					  + m_dValueBs[m_nChannelNo] * dDistance 
					  + m_dValueCs[m_nChannelNo] * pow(dDistance, 2)
					  + m_dValueDs[m_nChannelNo] * pow(dDistance, 3)
					  + m_dValueEs[m_nChannelNo] * pow(dDistance, 4);
				TRACE ("As = %.15lf\n", m_dValueAs[m_nChannelNo]);
				TRACE ("Bs = %.15lf\n", m_dValueBs[m_nChannelNo]);
				TRACE ("Cs = %.15lf\n", m_dValueCs[m_nChannelNo]);
				TRACE ("Ds = %.16lf\n", m_dValueDs[m_nChannelNo]);
				TRACE ("Es = %.21lf\n", m_dValueEs[m_nChannelNo]);
				break;
			default:
				return nEventPos;
			}
		}
	}
	m_dMeterDistance = dDist;
	// Pixel 단위로 환산
	nEventPos = (int)MeterToDot(dDist);

	TRACE ("h. 측정길이(%.1f) --> 보정길이(%.1f)\n", dDistance, dDist);

	return nEventPos;
}

/*-----------------------------------------------------------------------------  
 * 함 수 명 : double CGraphData::ValidPeak()
 * 목    적 : 
 * 설    명 : 
 * 작 성 자 : 김영규
 * 작성일자 : 2006.06.01
 * 변경사항 :
 *---------------------------------------------------------------------------*/
double CGraphData::ValidPeak(int nPeaktPos, EVENT_MODE uEventMode)
{
	double dDiff = 0;
	double dRate = 0;
	double dDistance = DotToMeter(nPeaktPos);
	double dPeakValue = 0;
	double dCorrPeakValue= 0;

	if(nPeaktPos <= 0) return 0;

	if(IsCoefficientLoaded()) {
		dPeakValue = m_MeasureInfo.pWaveData[nPeaktPos];
		dCorrPeakValue= m_MeasureInfo.pWaveData[nPeaktPos];

		///////////////////////////////////////////////////////////////////
		// 이벤트별 서로 다른 공식 적용
		///////////////////////////////////////////////////////////////////

		switch(uEventMode) {
		case EVENT_OPEN:
			dCorrPeakValue = m_dPeakGraphA
						   + m_dPeakGraphB * dDistance 
						   + m_dPeakGraphC * pow(dDistance, 2) 
						   + m_dPeakGraphD * pow(dDistance, 3);
//		TRACE( "단선 계수\n");
//		TRACE( "A = %f\n", m_dPeakGraphA);
//		TRACE( "B = %f\n", m_dPeakGraphB);
//		TRACE( "C = %f\n", m_dPeakGraphC);
//		TRACE( "D = %f\n", m_dPeakGraphD);
			break;
		case EVENT_SHORT:
			dCorrPeakValue = m_dPeakGraphAs
						   + m_dPeakGraphBs * dDistance 
						   + m_dPeakGraphCs * pow(dDistance, 2)
						   + m_dPeakGraphDs * pow(dDistance, 3);
//		TRACE( "쇼트 계수\n");
//		TRACE( "As = %f\n", m_dPeakGraphAs);
//		TRACE( "Bs = %f\n", m_dPeakGraphBs);
//		TRACE( "Cs = %f\n", m_dPeakGraphCs);
//		TRACE( "Ds = %f\n", m_dPeakGraphDs);
			break;
		default:
			return nPeaktPos;
		} 

		if(dCorrPeakValue) {
			dDiff = AbsValue(dPeakValue - dCorrPeakValue);
			dRate = dPeakValue/dCorrPeakValue * 100.0;
		}			
	}
/*
	TRACE("Diff Pos/Value [%d][%.1f] (%.1f-%.1f)\n", 
		nPeaktPos, 
		m_MeasureInfo.pRawData[nPeaktPos] - m_BaseInfo.pRawData[nPeaktPos],
		m_MeasureInfo.pRawData[nPeaktPos],
		m_BaseInfo.pRawData[nPeaktPos] );
*/
//	TRACE( "Peak(%.1f), 보정Peak(%.1f), 오차(%.1f), 오차비율(%.2f%%)\n", 
//		dPeakValue, dCorrPeakValue, dDiff, dRate);
//	TRACE( "오차(%.1f), 오차비율(%.2f%%)\n", dDiff, dRate);

	return dCorrPeakValue;
}


/*-----------------------------------------------------------------------------  
 * 함 수 명 : int CGraphData::FindEventPos()
 * 목    적 : 이벤트 발생지점을 찾음.
 * 설    명 : 일정한 크기의 윈도우를 피크점에서 부터 0 지점 방향으로 움직이면서
              기울기가 (-) -> (0) 로 전환하는 지점을 찾는다.
 * 작 성 자 : 김영규
 * 작성일자 : 2004.11.17
 * 변경사항 : 
              Pulse Width에 따라 윈도우 사이즈를 적정하게 설정해 주어야 한다.
 *---------------------------------------------------------------------------*/
int CGraphData::FindEventPos(EVENT_MODE uEventMode, int nPeakPos, double* pDiffData)
{
	int		nPulseWidth = GetPulseWidth(GetPulseWidthCode());
	int		nWidth		= (int)(SIZE_Window(nPulseWidth) / 2.0);
	int		i			= 0;

	double	dDiff		= 0.;
	BOOL	bStart		= FALSE;

	if(nPeakPos >= GetDataLength()) return 0;
	
	switch(uEventMode) {
	/*-------------------------------------------------------------------------
	 * 단선
	 *-----------------------------------------------------------------------*/
	case EVENT_OPEN:
		for(i=nPeakPos; i>nWidth; i--) {
			dDiff = pDiffData[i - nWidth] - pDiffData[i];

			if(dDiff < 0 && !bStart) {
				bStart = TRUE;
			}
			/*-----------------------------------------------------------------
			 *	(-) -> (+)
			 *---------------------------------------------------------------*/
			if(dDiff >= 0 && bStart) {
				break;
			}
		}
		break;

	/*-------------------------------------------------------------------------
	 * 쇼트
	 *-----------------------------------------------------------------------*/
	case EVENT_SHORT:
		for(i=nPeakPos; i>nWidth; i--) {
			dDiff = pDiffData[i - nWidth] - pDiffData[i];

			if(dDiff > 0 && !bStart) {
				bStart = TRUE;
			}
			/*-----------------------------------------------------------------
			 *	(+) -> (-)
			 *---------------------------------------------------------------*/
			if(dDiff <= 0 && bStart) {
				break;
			}
		}
	}
	
	return i;
}

/*-----------------------------------------------------------------------------  
 * 함 수 명 : WaitForAwhile()
 * 목    적 : 
 * 설    명 : bFlag가 참인 동안 대기함.
 * 작 성 자 : 김영규
 * 작성일자 : 2004.10.23
 * 변경사항 : 
 *---------------------------------------------------------------------------*/
void WaitForAwhile(double dWaitTime)
{
	MSG Message;
	double dCnt=0;

	while(1) {
		if (PeekMessage(&Message,NULL,0,0,PM_REMOVE)) {
			if (Message.message==WM_QUIT)
				break;
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}
		if( dCnt > dWaitTime) return;

		Sleep(1);
		dCnt += 1;	
	}
}

/*-----------------------------------------------------------------------------  
 * 함 수 명 : CountValue()
 * 목    적 : 특정 값의 빈도 계산
 * 설    명 : 
 * 작 성 자 : 김영규
 * 작성일자 : 2004.10.23
 * 변경사항 : 
 *---------------------------------------------------------------------------*/
int CountValue(int nStart, double* pWaveData)
{
	int nCount=0;
	double dValue = pWaveData[nStart];

	for(int i=nStart; i>0; i--) {
		if(dValue != pWaveData[i])
			break;

		nCount++;
	}
	return nCount;
}

/*-----------------------------------------------------------------------------  
 * 함 수 명 : QuickSort()
 * 목    적 : Double 값의 배열을 Quick Sort
 * 설    명 : 
 * 작 성 자 : 김영규
 * 작성일자 : 2004.10.23
 * 변경사항 : 
 *---------------------------------------------------------------------------*/
void QuickSort (int lo, int hi, double* pWaveData)
{
	//  lo is the lower index, hi is the upper index
	//  of the region of array a that is to be sorted
    int		i = lo;
	int		j = hi;
	double	dTempValue = 0.;
    double	dValue = pWaveData[(lo+hi)/2];

    //  partition
    do {    
        while (pWaveData[i] < dValue) i++; 
        while (pWaveData[j] > dValue) j--;
        if (i<=j) {
            dTempValue	 = pWaveData[i]; 
			pWaveData[i] = pWaveData[j]; 
			pWaveData[j] = dTempValue;
            i++; 
			j--;
        }
    } while (i <= j);

    //  recursion
    if (lo <  j) QuickSort(lo, j, pWaveData);
    if ( i < hi) QuickSort(i, hi, pWaveData);
}


/*-----------------------------------------------------------------------------  
 * 함 수 명 : CString CGraphData::GetFilename()
 * 목    적 : 화일의 이름을 구한다.
 * 설    명 : 
 * 작 성 자 : 김영규
 * 작성일자 : 2004.11.29
 * 변경사항 :
 *---------------------------------------------------------------------------*/
int CGraphData::GetFilename(CString csPathname, char* spFilename)
{
	CString csFilename;
	int nStrLen=0;

	if(csPathname.GetLength()) {

		csFilename = csPathname;
		nStrLen = csFilename.Find("\\");

		// Filename
		while(nStrLen != -1) {
			csFilename = csFilename.Mid(nStrLen+1);
			nStrLen = csFilename.Find("\\");
		}
		strcpy(spFilename, csFilename);

		return csFilename.GetLength();
	}
	return 0;
}

/*-----------------------------------------------------------------------------  
 * 함 수 명 : CString CGraphData::GetFilename()
 * 목    적 : 
 * 설    명 : 
 * 작 성 자 : 김영규
 * 작성일자 : 2004.11.29
 * 변경사항 :
 *---------------------------------------------------------------------------*/
CString CGraphData::GetFilename()
{
	return m_MeasureInfo.csFilename;
}


/*-----------------------------------------------------------------------------  
 * 함 수 명 : void CGraphData::RemoveFrontNoise()
 * 목    적 : 그래프 앞 부분의 노이즈 제거
 * 설    명 : 초반 노이즈를 제거한 실제 데이타가 시작되는 부분의 좌표를 반환
 * 작 성 자 : 김영규
 * 작성일자 : 2004.11.24
 * 변경사항 :
 *---------------------------------------------------------------------------*/
int CGraphData::RemoveFrontNoise(double* pWaveData, int nStart)
{
	int		nPos = 0;
	int		nOffSet = 0;
	double	dDiffValue = 0.;
	double	dValue = 0.;
	
	double dMaxValue=0;
	int nMaxPos=0;
	double dMinValue=99999999;
	int nMinPos=0;

	for(nPos = 0; nPos < 1500; nPos++) {
		if(nPos>0 && nPos < GetDataLength()) {
			ASSERT(pWaveData[nPos]);
			ASSERT(pWaveData[nPos-1]);
			dDiffValue = pWaveData[nPos-1] - pWaveData[nPos];

			if(dMinValue > dDiffValue) {
				dMinValue = dDiffValue;
				nMinPos = nPos;
			}

			if(dMaxValue < dDiffValue) {
				dMaxValue = dDiffValue;
				nMaxPos = nPos;
			}
		}
	}

//	TRACE("[카오스] 기준위치:%d, 최소(%d, %.1f), 최대(%d, %.1f)\n", 
//		nMaxPos, nMinPos, dMinValue/22.0, nMaxPos, dMaxValue/22.0);

	return nMaxPos;
}

/*-----------------------------------------------------------------------------  
 * 함 수 명 : void CGraphData::RemoveRearNoise()
 * 목    적 : 그래프 앞 부분의 노이즈 제거
 * 설    명 : 후방 노이즈 제거
 * 작 성 자 : 김영규
 * 작성일자 : 2004.11.24
 * 변경사항 : 
 *            - dDiffValue의 값을 16 -> 100 으로 변경함. 2006.11.22 김영규
 *---------------------------------------------------------------------------*/
int CGraphData::RemoveRearNoise(double* pWaveData)
{
	int		nPos = 0;
	int		nOffSet = 3;
	double	dDiffValue = 0.;
	double	dValue = 0.;
	int		nLength = GetDataLength();

	for(nPos=nLength; nPos>0; nPos--) {
		if(nPos-nOffSet >= 0) {
			dDiffValue = AbsValue(pWaveData[nPos-nOffSet] - pWaveData[nPos]);
			//TRACE("nPos(%d), nPos-nOffSet(%d), pWaveData[nPos-nOffSet]=%.1f, pWaveData[nPos]=%.1f\n", nPos, nPos-nOffSet, pWaveData[nPos-nOffSet], pWaveData[nPos]);
			if(dDiffValue < 100) {
				break;
			}
		}
	}

	return nPos;
}

/*-----------------------------------------------------------------------------  
 * 함 수 명 : int CGraphData::UpDownScan()
 * 목    적 : 2번째 피크점을 찾는다.
 * 설    명 : 1. Y 값의 스캔을 위해서 Quick Sort를 한다.
			  2. Y 값을 큰값부터 작은값순으로 스캔하면서 3개의 접점을 
                 이루는 값을 찾고 
			  3. 그 값의  X 값을 구한다.
 * 필수조건 : ScanPos는 첫번째 피크점을 지나서 설정되어 한다.

 * 작 성 자 : 김영규
 * 작성일자 : 2004.11.18
 * 변경사항 :
 *---------------------------------------------------------------------------*/
int CGraphData::UpDownScan(int nRangeStart, int nRangeEnd, double pWaveData[])
{
	int		i			= 0;	
	int		nCount		= 0;
	int		nOldCount	= 0;
	double	dTheValue	= 0.;
	double* pTempWave	= NULL;


	/*---------------------------------------------------------------------
	 * 탐색범위 내에서 2번째 피크점을 찾는다.
	 *-------------------------------------------------------------------*/
	// 데이타 크기순으로 퀵정렬
	pTempWave = new double[nRangeEnd];
	if(!pTempWave) {
		TRACE( "메모리 할당 에러! in CGraphData::UpDownScan()\n");
		return -1;
	}
	memcpy((void*)pTempWave, (void*)pWaveData, sizeof(double) * nRangeEnd );

	// Quick Sort	
	::QuickSort(nRangeStart, nRangeEnd-1, pTempWave);

	// 두번째 피크점의 값
	int nIndex=0;
	for(i=nRangeEnd-1; i>=nRangeStart;) {
		/*---------------------------------------------------------------------
		 * Updown 스캔하여 접점이 2개이상
		 * - 필수 조건 : ScanPos는 첫번째 피크점을 지나서 설정되어 한다.
		 *-------------------------------------------------------------------*/
		nCount = ::CountValue(i, pTempWave);
		if(nOldCount == 1 && nCount == 2) {
			nIndex = i;
			break;
		}
		i -= nCount;
		nOldCount = nCount;
	}
	dTheValue = pTempWave[nIndex];
	
	// 메모리 해제
	if(pTempWave) {
		delete pTempWave;
		pTempWave = NULL;
	}

	// 두번째 피크점의 위치
	BOOL bFound = FALSE;
	for(i=nRangeStart; i<nRangeEnd; i++) {
		if(dTheValue == pWaveData[i]) {
			if(!bFound) bFound = TRUE;
		}

		if(bFound && dTheValue != pWaveData[i]) {
			break;
		}
	}
	/*---------------------------------------------------------------------
	 * 2번째 피크점
	 *-------------------------------------------------------------------*/
	return i-1;
}

/*-----------------------------------------------------------------------------  
 * 함 수 명 : void CGraphData::SetScanPos()
 * 목    적 : 스캔 시작 위치 지정
 * 설    명 : 
 * 작 성 자 : 김영규
 * 작성일자 : 2004.11.22
 * 변경사항 :
 *---------------------------------------------------------------------------*/
void CGraphData::SetScanPos(int nScanPos)
{
	m_EventInfo.nRangeStart = (int)(nScanPos / m_WaveData.wScale) * 100;
}

/*-----------------------------------------------------------------------------  
 * 함 수 명 : int CGraphData::GetScanPos()
 * 목    적 : 스캔시작 위치 참조
 * 설    명 : 
 * 작 성 자 : 김영규
 * 작성일자 : 2004.11.22
 * 변경사항 :
 *---------------------------------------------------------------------------*/
int CGraphData::GetScanPos()
{
	return m_EventInfo.nRangeStart;
}

/*-----------------------------------------------------------------------------  
 * 함 수 명 : void CGraphData::GetChannel()
 * 목    적 : 
 * 설    명 : 채널번호 지정
 * 작 성 자 : 김영규
 * 작성일자 : 2004.11.15
 * 변경사항 :
 *---------------------------------------------------------------------------*/
void CGraphData::GetChannel(CString csPathName)
{
	int nRet = 0;

	nRet = GetChannelNo(csPathName);
	
	if(nRet >= 0)
		m_nChannelNo = nRet;
}


/*-----------------------------------------------------------------------------  
 * 함 수 명 : int CGraphData::GetChannelNo()
 * 목    적 : Pathname에서 채널번호를 구함.
 * 설    명 : 채널 번호를 구하지 못하면 1을 반환함.
 * 작 성 자 : 김영규
 * 작성일자 : 2004.11.15
 * 변경사항 :
 *---------------------------------------------------------------------------*/
int CGraphData::GetChannelNo(CString csPathname)
{
	int nPos = 0;
	int nChannel = -1;
	CString csChannel;

	// Channel No
	nPos = csPathname.Find( "Tdr", 0 );

	if(nPos > 0) {
		csChannel	= csPathname.Mid(nPos+7, 1);
		nChannel	= atoi(csChannel) - 1;
	}
	else {
		nPos = csPathname.Find( "-Ch", 0 );
		if(nPos > 0) {
			csChannel	= csPathname.Mid(nPos+3, 1);
			nChannel	= atoi(csChannel) - 1;
		}
	}

	return nChannel;
}

/*-----------------------------------------------------------------------------  
 * 함 수 명 : void CGraphData::GetEventInfo()
 * 목    적 : 
 * 설    명 : 
 * 작 성 자 : 김영규
 * 작성일자 : 2004.11.16
 * 변경사항 :
 *---------------------------------------------------------------------------*/
void CGraphData::GetEventInfo(EVENTDETECT_INFO* pEventInfo)
{
	memcpy((void*)pEventInfo, (void*)&m_EventInfo, sizeof(EVENTDETECT_INFO));
}

/*-----------------------------------------------------------------------------  
 * 함 수 명 : void CGraphData::SetEventInfo()
 * 목    적 : 
 * 설    명 : 
 * 작 성 자 : 김영규
 * 작성일자 : 2004.11.16
 * 변경사항 :
 *---------------------------------------------------------------------------*/
void CGraphData::SetEventInfo(EVENTDETECT_INFO EventInfo)
{
	memcpy((void*)&m_EventInfo, (void*)&EventInfo, sizeof(EVENTDETECT_INFO));
}

/*-----------------------------------------------------------------------------  
 * 함 수 명 : void CGraphData::GetEventPos()
 * 목    적 : 시작포인트 및  이벤트 포인트 위치를 구한다.
 * 설    명 : 
 * 작 성 자 : 김영규
 * 작성일자 : 2004.11.15
 * 변경사항 :
 *---------------------------------------------------------------------------*/
void CGraphData::GetEventPos(int* npStartPos, int* npEventPos)
{
	*npStartPos = m_EventInfo.nStart;	// 측정 시작점
	*npEventPos = m_EventInfo.nEvent;	// 이벤트 발생지점
}


/*-----------------------------------------------------------------------------  
 * 함 수 명 : CString CGraphData::GetPathName()
 * 목    적 : 웨이브 화일의 패스를 구함.
 * 설    명 : 
 * 작 성 자 : 김영규
 * 작성일자 : 2004.11.15
 * 변경사항 :
 *---------------------------------------------------------------------------*/
CString	CGraphData::GetPathName()
{
	return m_MeasureInfo.csPathname;
}

/*-----------------------------------------------------------------------------  
 * 함 수 명 : double CGraphData::GetVolt()
 * 목    적 : 
 * 설    명 : 
 * 작 성 자 : 김영규
 * 작성일자 : 2004.11.15
 * 변경사항 :
 *---------------------------------------------------------------------------*/
double CGraphData::GetVolt()
{
	return m_dVolt;
}

/*-----------------------------------------------------------------------------  
 * 함 수 명 : double CGraphData::GetdBRL()
 * 목    적 : 
 * 설    명 : 
 * 작 성 자 : 김영규
 * 작성일자 : 2004.11.15
 * 변경사항 :
 *---------------------------------------------------------------------------*/
double CGraphData::GetdBRL()
{
	return m_dBRL;
}


/*-----------------------------------------------------------------------------  
 * 함 수 명 : double CGraphData::GetDistance()
 * 목    적 : 이벤트 발생지점까지의 거리(단위 : m)
 * 설    명 : 
 * 작 성 자 : 김영규
 * 작성일자 : 2004.11.15
 * 변경사항 :
 *---------------------------------------------------------------------------*/
double CGraphData::GetDistance()
{
	return m_dMeterDistance;
}


/*-----------------------------------------------------------------------------  
 * 함 수 명 : int CGraphData::GetDotDistance()
 * 목    적 : 이벤트 발생지점까지의 거리(단위 : Dot)
 * 설    명 : 
 * 작 성 자 : 김영규
 * 작성일자 : 2004.11.15
 * 변경사항 :
 *---------------------------------------------------------------------------*/
int CGraphData::GetDotDistance()
{
	return m_nDotDistance;
}


/*-----------------------------------------------------------------------------  
 * 함 수 명 : double CGraphData::GetThreshold()
 * 목    적 : Threshold
 * 설    명 : 
 * 작 성 자 : 김영규
 * 작성일자 : 2004.11.15
 * 변경사항 :
 *---------------------------------------------------------------------------*/
double CGraphData::GetThreshold()
{
	return m_dThreshold;
}


/*-----------------------------------------------------------------------------  
 * 함 수 명 : double CGraphData::GetVOP()
 * 목    적 : VOP
 * 설    명 : 
 * 작 성 자 : 김영규
 * 작성일자 : 2004.11.15
 * 변경사항 :
 *---------------------------------------------------------------------------*/
double CGraphData::GetVOP()
{
	return m_dVOP;
}

/*-----------------------------------------------------------------------------  
 * 함 수 명 : void CGraphData::SetMeasureVOP()
 * 목    적 : 
 * 설    명 : 
 * 작 성 자 : 김영규
 * 작성일자 : 2005.03.24
 * 변경사항 :
 *---------------------------------------------------------------------------*/
void CGraphData::SetMeasureVOP(double dVop)
{
	m_dMeasureVop = dVop;
}

double CGraphData::GetMeasureVOP()
{
	return m_dMeasureVop;
}


/*-----------------------------------------------------------------------------  
 * 함 수 명 : double CGraphData::GetPulseWidth()
 * 목    적 : Pulse Width Code
 * 설    명 : 
 * 작 성 자 : 김영규
 * 작성일자 : 2004.11.15
 * 변경사항 :
 *---------------------------------------------------------------------------*/
int CGraphData::GetPulseWidthCode()
{
	return m_nPulseWidth;
}

/*-----------------------------------------------------------------------------  
 * 함 수 명 : int CGraphData::GetPulseWidth()
 * 목    적 : Pulse Width
 * 설    명 : 
 * 작 성 자 : 김영규
 * 작성일자 : 2004.11.19
 * 변경사항 :
 *---------------------------------------------------------------------------*/
int CGraphData::GetPulseWidth(int nPulseWidthCode)
{
	int nPulseWidth=0;

	switch(nPulseWidthCode) {
	case 0:
		nPulseWidth = 5;
		break;
	case 1:
		nPulseWidth = 15;
		break;
	case 2:
		nPulseWidth = 45;
		break;
	case 3:
		nPulseWidth = 100;
		break;
	case 4:
		nPulseWidth = 200;
		break;
	case 5:
		nPulseWidth = 500;
		break;
	case 6:
		nPulseWidth = 1000;
		break;
	case 7:
		nPulseWidth = 2000;
		break;
	case 8:
		nPulseWidth = 5000;
		break;
	default:
		nPulseWidth = 10000;
	}
	
	return nPulseWidth;
}


/*-----------------------------------------------------------------------------  
 * 함 수 명 : int CGraphData::GetScale()
 * 목    적 : 스캐일
 * 설    명 : 
 * 작 성 자 : 김영규
 * 작성일자 : 2004.11.15
 * 변경사항 :
 *---------------------------------------------------------------------------*/
int CGraphData::GetScale()
{
	return m_nScale;
}


/*-----------------------------------------------------------------------------  
 * 함 수 명 : WORD CGraphData::GetEventMode()
 * 목    적 : 이벤트 모드
 * 설    명 : 쇼트, 단락
 * 작 성 자 : 김영규
 * 작성일자 : 2004.11.15
 * 변경사항 :
 *---------------------------------------------------------------------------*/
WORD CGraphData::GetEventMode()
{
	return m_uEventMode;
}

/*-----------------------------------------------------------------------------  
 * 함 수 명 : int CGraphData::GetChannelNo()
 * 목    적 : 채널번호
 * 설    명 : 
 * 작 성 자 : 김영규
 * 작성일자 : 2004.11.15
 * 변경사항 :
 *---------------------------------------------------------------------------*/
int CGraphData::GetChannelNo()
{
	return m_nChannelNo;
}


/*-----------------------------------------------------------------------------  
 * 함 수 명 : double CGraphData::GetAvg()
 * 목    적 : 평균값
 * 설    명 : 
 * 작 성 자 : 김영규
 * 작성일자 : 2004.11.15
 * 변경사항 :
 *---------------------------------------------------------------------------*/
double CGraphData::GetAvg()
{
	return m_dAvgValue;
}


/*-----------------------------------------------------------------------------  
 * 함 수 명 : WORD CGraphData::GetDataLength()
 * 목    적 : 
 * 설    명 : Dot 단위의 거리를 구한다.
 * 작 성 자 : 김영규
 * 작성일자 : 2004.11.15
 * 변경사항 :
 *---------------------------------------------------------------------------*/
WORD CGraphData::GetDataLength()
{
	return m_WaveData.wLength / 2;
}


/*-----------------------------------------------------------------------------  
 * 함 수 명 : BOOL CGraphData::IsEmpty()
 * 목    적 : 
 * 설    명 : Data가 로딩 되었는지 확인한다.
 * 작 성 자 : 김영규
 * 작성일자 : 2004.11.15
 * 변경사항 :
 *---------------------------------------------------------------------------*/
BOOL CGraphData::IsEmpty()
{
	if(m_pData == NULL)
		return TRUE;
	else 
		return FALSE;
}

/*-----------------------------------------------------------------------------  
 * 함 수 명 : double CGraphData::AbsValue()
 * 목    적 : DOUBLE 타입의 변수의 절대값을 구함.
 * 설    명 : 
 * 작 성 자 : 김영규
 * 작성일자 : 2004.11.15
 * 변경사항 :
 *---------------------------------------------------------------------------*/
double CGraphData::AbsValue(double dValue)
{
	if(dValue < 0)
		return dValue * -1;
	else 
		return dValue;
}

/*-----------------------------------------------------------------------------  
 * 함 수 명 : double* CGraphData::GetFilteredWaveData()
 * 목    적 : 필터링된 데이타의 포인터를 구함.
 * 설    명 : 
 * 작 성 자 : 김영규
 * 작성일자 : 2004.11.15
 * 변경사항 :
 *---------------------------------------------------------------------------*/
double* CGraphData::GetMeasureWaveData()
{
	if(m_MeasureInfo.pWaveData)
		return m_MeasureInfo.pWaveData;
	else 
		return NULL;
}

/*-----------------------------------------------------------------------------  
 * 함 수 명 : double* CGraphData::GetBaseWaveData()
 * 목    적 : 베이스 그래프 데이타를 얻음
 * 설    명 : 
 * 작 성 자 : 김영규
 * 작성일자 : 2004.12.03
 * 변경사항 :
 *---------------------------------------------------------------------------*/
double* CGraphData::GetBaseWaveData()
{
	if(m_BaseInfo.pWaveData)
		return m_BaseInfo.pWaveData;
	else 
		return NULL;
}

/*-----------------------------------------------------------------------------  
 * 함 수 명 : double CGraphData::GetValue()
 * 목    적 : 
 * 설    명 : 
 * 작 성 자 : 김영규
 * 작성일자 : 2004.12.10
 * 변경사항 :
 *---------------------------------------------------------------------------*/
double CGraphData::GetValue(int nPos)
{
	if(m_pData) {
		if(nPos < GetDataLength()) 
			return m_pData[nPos];
	}
	return 0;
}

/*-----------------------------------------------------------------------------  
 * 함 수 명 : double CGraphData::GetBaseValue()
 * 목    적 : 
 * 설    명 : 
 * 작 성 자 : 김영규
 * 작성일자 : 2004.12.10
 * 변경사항 :
 *---------------------------------------------------------------------------*/
double CGraphData::GetBaseValue(int nPos)
{
	if(m_BaseInfo.pWaveData) {
		if(nPos < GetDataLength() && nPos >= 0) 
			return m_BaseInfo.pWaveData[nPos];
	}
	return 0;
}

/*-----------------------------------------------------------------------------  
 * 함 수 명 : double CGraphData::GetMeasureValue()
 * 목    적 : 
 * 설    명 : 
 * 작 성 자 : 김영규
 * 작성일자 : 2004.12.10
 * 변경사항 :
 *---------------------------------------------------------------------------*/
double CGraphData::GetMeasureValue(int nPos)
{
	if(m_MeasureInfo.pWaveData) {
		if(nPos < GetDataLength() && nPos >= 0) 
			return m_MeasureInfo.pWaveData[nPos];
	}
	return 0;
}

/*-----------------------------------------------------------------------------  
 * 함 수 명 : double CGraphData::FindPeak()
 * 목    적 : 주어진 구간에서의 피크점을 찾는다
 * 설    명 : Down to Up 스캔하면서 피크점을 찾는다.
 * 작 성 자 : 김영규
 * 작성일자 : 2005.1.16
 * 변경사항 :
 *---------------------------------------------------------------------------*/
int CGraphData::FindPeak(double*pWaveData, int nStart, int nEnd)
{
	int		i			= 0;	
	int		nCount		= 0;
	int		nOldCount	= 0;
	double	dTheValue	= 0.;
	double* pTempWave	= NULL;
	int		nLength		= 0;

	nLength = m_wDataLength;
	/*---------------------------------------------------------------------
	 * 탐색범위 내에서 2번째 피크점을 찾는다.
	 *-------------------------------------------------------------------*/
	// 데이타 크기순으로 퀵정렬
	pTempWave = new double[nLength];
	if(!pTempWave) {
		return -1;
	}
	memcpy((void*)pTempWave, (void*)pWaveData, sizeof(double) * (nLength));

	// Quick Sort	
	::QuickSort(nStart, nEnd, pTempWave);

	// 두번째 피크점의 값
	int nIndex=0;
	for(i=nStart; i<nEnd;) {
		/*---------------------------------------------------------------------
		 * Updown 스캔하여 접점이 2개이상
		 * - 필수 조건 : ScanPos는 첫번째 피크점을 지나서 설정되어 한다.
		 *-------------------------------------------------------------------*/
		nCount = ::CountValue(i, pTempWave);
		if(nOldCount == 1 && nCount == 2) {
			nIndex = i;
			break;
		}
		i += nCount;
		nOldCount = nCount;
	}
	dTheValue = pTempWave[nIndex];
	
	// 메모리 해제
	if(pTempWave) {
		delete pTempWave;
		pTempWave = NULL;
	}

	// 두번째 피크점의 위치
	BOOL bFound = FALSE;
	for(i=nStart; i<nEnd; i++) {
		if(dTheValue == pWaveData[i]) {
			if(!bFound) bFound = TRUE;
		}

		if(bFound && dTheValue != pWaveData[i]) {
			break;
		}
	}
	/*---------------------------------------------------------------------
	 * 2번째 피크점
	 *-------------------------------------------------------------------*/
	return i-1;
}

/*-----------------------------------------------------------------------------  
 * 함 수 명 : int GetMeasureTime()
 * 목    적 : 
 * 설    명 : 
 * 작 성 자 : 김영규
 * 작성일자 : 2005.03.10
 * 변경사항 :
 *---------------------------------------------------------------------------*/
int CGraphData::GetMeasureTime()
{
	CString csFileName;
	CString csTime;
	int		nHour=0;
	int		nPos=0;

	if(m_bMeasureLoaded) {
		csFileName = m_MeasureInfo.csFilename;
		
		// TdrData1-2005_03_04 00_02_11.bin
		int nPos = csFileName.Find(" ");
		if(nPos > 0) {
			csTime	= csFileName.Mid(nPos + 1, 8);
			nHour	= atoi(csTime.Mid(0, 2));
			return nHour;
		}
	}
	return -1;
}

/*-----------------------------------------------------------------------------  
 * 함 수 명 : void CGraphData::SetEventThreshold()
 * 목    적 : 
 * 설    명 : 
		// 채널1 : 오성파출소 방향
		Threshold = 150;

		// 채널2 : 웨딩홀 방향
		Threshold = 90;

  * 작 성 자 : 김영규
 * 작성일자 : 2005.03.18
 * 변경사항 :
 *---------------------------------------------------------------------------*/
void CGraphData::SetEventThreshold(double dValue)
{
	m_dEventThreshold = dValue;
}

/*-----------------------------------------------------------------------------  
 * 함 수 명 : void CGraphData::GetEventThreshold()
 * 목    적 : 
 * 설    명 : 
 * 작 성 자 : 김영규
 * 작성일자 : 2005.03.18
 * 변경사항 :
 *---------------------------------------------------------------------------*/
double CGraphData::GetEventThreshold()
{
	return m_dEventThreshold;
}


/*-----------------------------------------------------------------------------  
 * 함 수 명 : void CGraphData::GetEventZone()
 * 목    적 : 
 * 설    명 : 
 * 작 성 자 : 김영규
 * 작성일자 : 2005.03.18
 * 변경사항 :
 *---------------------------------------------------------------------------*/
void CGraphData::GetEventZone(int* npStart, int* npEnd)
{
	*npStart	= m_EventInfo.nRangeStart;
	*npEnd		= m_EventInfo.nRangeEnd;
}

/*-----------------------------------------------------------------------------  
 * 함 수 명 : void CGraphData::SetEventZone()
 * 목    적 : 이벤트 감시구간 설정
 * 설    명 : m 단위로 설정한다.
 * 작 성 자 : 김영규
 * 작성일자 : 2005.03.18
 * 변경사항 :
 *---------------------------------------------------------------------------*/
void CGraphData::SetEventZone(int nStart, int nEnd)
{
	m_EventInfo.nRangeStart = nStart;
	m_EventInfo.nRangeEnd	= nEnd;
}

/*-----------------------------------------------------------------------------  
 * 함 수 명 : int CGraphData::SIZE_Window()
 * 목    적 : 
 * 설    명 : 
 * 작 성 자 : 김영규
 * 작성일자 : 2005.03.05
 * 변경사항 :
 *---------------------------------------------------------------------------*/
int CGraphData::SIZE_Window(int nPulseWidth) 
{
	int nWindowSize=0;

	switch(nPulseWidth) {
	case 5:
		nWindowSize=3;
		break;
	case 15:
		nWindowSize=4;
		break;
	case 45:
		nWindowSize=5;
		break;
	case 100:
		nWindowSize=8;
		break;
	case 200:
		nWindowSize=12;
		break;
	case 500:
		nWindowSize=36;
		break;
	case 1000:
		nWindowSize=48;
		break;
	case 2000:
		nWindowSize=64;
		break;
	case 5000:
		nWindowSize=96;
		break;
	default:
		nWindowSize=128;
	}
	return nWindowSize;
}


/*-----------------------------------------------------------------------------  
 * 함 수 명 : int CGraphData::LINE_Length()
 * 목    적 : 이벤트 검출을 위해서 기울기를 구하는 구간의 길이
 * 설    명 : 
 * 작 성 자 : 김영규
 * 작성일자 : 2005.03.16
 * 변경사항 :
 *---------------------------------------------------------------------------*/
int CGraphData::LINE_Length(int nPulseWidth)
{
	int nLength = 0;

	if(nPulseWidth <= 5) {
		nLength = 20;
	}
	else if(nPulseWidth <= 15) {
		nLength = 40;
	}
	else if(nPulseWidth <= 45) {
		nLength = 60;
	}
	else if(nPulseWidth <= 100) {
		nLength = 100;
	}
	else if(nPulseWidth <= 200) {
		nLength = 200;
	}
	else if(nPulseWidth <= 500) {
		nLength = 300;
	}
	else if(nPulseWidth <= 1000) {
		nLength = 500;
	}
	else if(nPulseWidth <= 2000) {
		nLength = 800;
	}
	else if(nPulseWidth <= 5000) {
		nLength = 1200;
	}
	else if(nPulseWidth <= 10000) {
		nLength = 1800;
	}
	return nLength;
}


/*-----------------------------------------------------------------------------  
 * 함 수 명 : int CGraphData::DIFF_Length()
 * 목    적 : 이벤트 감시를 하는 구간
 * 설    명 : 
 * 작 성 자 : 김영규
 * 작성일자 : 2005.03.04
 * 변경사항 :
 *---------------------------------------------------------------------------*/
int CGraphData::DIFF_Length(int nPulseWidth)
{
	int nLength = 0;

	if(nPulseWidth <= 5) {
		nLength = 20;
	}
	else if(nPulseWidth <= 15) {
		nLength = 40;
	}
	else if(nPulseWidth <= 45) {
		nLength = 60;
	}
	else if(nPulseWidth <= 100) {
		nLength = 100;
	}
	else if(nPulseWidth <= 200) {
		nLength = 200;
	}
	else if(nPulseWidth <= 500) {
		nLength = 300;
	}
	else if(nPulseWidth <= 1000) {
		nLength = 500;
	}
	else if(nPulseWidth <= 2000) {
		nLength = 800;
	}
	else if(nPulseWidth <= 5000) {
		nLength = 1200;
	}
	else if(nPulseWidth <= 10000) {
		nLength = 1800;
	}
	return nLength;
}

/*-----------------------------------------------------------------------------  
 * 함 수 명 : int CGraphData::DIFF_StartPos()
 * 목    적 : 펄스별 Difference Data 를 구하는 구간의 시작위치(m)
 * 설    명 : 
 * 작 성 자 : 김영규
 * 작성일자 : 2005.03.04
 * 변경사항 :
 *---------------------------------------------------------------------------*/
int CGraphData::DIFF_StartPos(int nPulseWidth)
{
	int nLength = 0;

	if(nPulseWidth <= 5) {
		nLength = 0;
	}
	else if(nPulseWidth <= 15) {
		nLength = 0;
	}
	else if(nPulseWidth <= 45) {
		nLength = 10;
	}
	else if(nPulseWidth <= 100) {
		nLength = 10;
	}
	else if(nPulseWidth <= 200) {
		nLength = 10;
	}
	else if(nPulseWidth <= 500) {
		nLength = 10;
	}
	else if(nPulseWidth <= 1000) {
		nLength = 250;
	}
	else if(nPulseWidth <= 2000) { 
		nLength = 320; //m
	}
	else if(nPulseWidth <= 5000) {
		nLength = 1200;	// 2006.5.15 김영규
	}
	else if(nPulseWidth <= 10000) {
		nLength = 1450;
	}
	
	return nLength;
}


/*-----------------------------------------------------------------------------  
 * 함 수 명 : void GetDiffTotal()
 * 목    적 : 관심 구간의 상하 이동을 보정하고 평균값을 구한다.
 * 설    명 : 평균값을 이용하여 이벤트 여부를 확인한다.
 * 작 성 자 : 김영규
 * 작성일자 : 2005.03.04
 * 변경사항 : 
    - 분산 : 편차의 제곱의 평균
	         n개의 변량 x1, x2, ... xn의 평균 m
	         Pow(s,2) = pow(x1-m, 2) + pow(x2-m, 2) + ... + pow(xn-m, 2)
	- 표준편차 : 분산의 양의 제곱근
 *---------------------------------------------------------------------------*/
double CGraphData::GetDiffTotal()
{
	register int nPos=0;
	int		nStart=0;
	int		nEnd=0;
	double	dDiffereceValue=0.0;
	double	dDiffTotal=0.0;
	double	dDiffAvg=0.0;
	double	dDist=0.0;

	nStart = (int)DotToMeter(m_EventInfo.nRangeStart);
	nEnd = (int)DotToMeter(m_EventInfo.nRangeEnd);

	if(nStart <= 0 || nEnd <= 0) 
		return -1;

	if(!m_bMeasureLoaded || !m_bBaseLoaded) {
		return -1;
	}

	if(m_BaseInfo.nLength < m_MeasureInfo.nLength) {
		if(nEnd > m_BaseInfo.nLength)
			nEnd = m_BaseInfo.nLength;
	}
	else {
		if(nEnd > m_MeasureInfo.nLength)
			nEnd = m_MeasureInfo.nLength;
	}
	
	if(nStart > nEnd) {
		nStart = GetScanPos();
	}
	// 상하 평균 이동거리
	dDist = ((m_BaseInfo.pWaveData[nStart] - m_MeasureInfo.pWaveData[nStart]) 
	       + (m_BaseInfo.pWaveData[nEnd] - m_MeasureInfo.pWaveData[nEnd])) / 2.0;

	/*-------------------------------------------------------------------------
	 * 구간의 평균을 구함.
	 *-----------------------------------------------------------------------*/
	dDiffTotal = 0;
	for(nPos=nStart; nPos<nEnd; nPos++) {
		dDiffereceValue = m_BaseInfo.pWaveData[nPos] - m_MeasureInfo.pWaveData[nPos];
		
		// Diff Total
		dDiffTotal += dDiffereceValue;
	}
	// 구간의 평균
	dDiffAvg = dDiffTotal / (nEnd - nStart + 1);

	/*-------------------------------------------------------------------------
	 * 데이타 보정 : 평균에 대해서 수직 이동한다.
	 *-----------------------------------------------------------------------*/
	dDiffTotal = 0;
	for(nPos=nStart; nPos<nEnd; nPos++) {
		// Diff Total
		dDiffereceValue = m_BaseInfo.pWaveData[nPos] - (m_MeasureInfo.pWaveData[nPos] + dDist);
		dDiffTotal += AbsValue(dDiffereceValue);
	}

	dDiffAvg = dDiffTotal/(nEnd - nStart + 1);
	TRACE( ">>>>>> nStart=%d, nEnd=%d, Diff Avg=%.1f\n", nStart, nEnd, dDiffAvg);

	return dDiffAvg;
}



/*-----------------------------------------------------------------------------  
 * 함 수 명 : EVENT_MODE CGraphData::GetPeakPoint()
 * 목    적 : 관심구간의 양끝점을 잇는 직선을 상하 이동하여 만나는 그래프 상의
              점을 구한다. 
 * 설    명 : 
 * 작 성 자 : 김영규
 * 작성일자 : 2005.03.06
 * 변경사항 : 한 직선과 만나는 그래프상의 최소점 최고점을 구한다.
				y = ax + b
				b = y - ax
				즉, b의 최소값과 최고값을 구한다.
 *---------------------------------------------------------------------------*/
EVENT_MODE CGraphData::GetPeakPoint(int nStartPos, double* pWaveData, int* npEventPos)
{
	register int nIndex	=0;
	double	dRateX		=0.0;
	int		nPulseWidth	=GetPulseWidth(GetPulseWidthCode());
	int		nEndPos		=0;
	int		nMinPos		=0;
	int		nMaxPos		=0;
	double	dMaxValue	=-9999999999.;
	double	dMinValue	=9999999999.;
	double	dValue		= 0.0;
	double	dStartValue = 0.0;

	int		nMaxPosInRange	= 0;
	int		nMinPosInRange	= 0;
	double	dMaxValueInRange= -9999999999.0;
	double	dMinValueInRange= 9999999999.0;
	EVENT_MODE uEventMode = EVENT_NONE;

	nEndPos = GetScanPos() + (int)(LINE_Length(nPulseWidth) / m_WaveData.wScale) * 100;
	if(nEndPos > GetDataLength()) {
		nEndPos = GetDataLength()-1;
	}

	TRACE( "이벤트 탐색구간(%.1fm ~ %.1fm)\n", DotToMeter(nStartPos), DotToMeter(nEndPos));
	
	// 기울기 (a) = (y2 - y1) / (x2 - x1)
	dRateX = (pWaveData[nEndPos] - pWaveData[nStartPos]) / (nEndPos - nStartPos);

	dStartValue = pWaveData[nStartPos] - dRateX * nIndex;	
	for(nIndex=nStartPos; nIndex<nEndPos; nIndex++) {
		// 기울기가 a 이고 한점(x1, y1) 을 지나는 직선
		// y = a * (x - x1) + y1;
		dValue = pWaveData[nIndex] - dRateX * nIndex;

		if(dValue < dMinValue) {
			dMinValue	= dValue;
			nMinPos		= nIndex;
		}

		if(dValue > dMaxValue) {
			dMaxValue	= dValue;
			nMaxPos		= nIndex;
		}
	}

	TRACE( "피크점 Min(%d, %.1fm), Max(%d, %.1fm)\n", nStartPos, DotToMeter(nStartPos), 
											           nEndPos, DotToMeter(nEndPos));
	/*-------------------------------------------------------------------------
	 *	이벤트 종류 판별
	 *-----------------------------------------------------------------------*/
	if(pWaveData[nStartPos] > pWaveData[nMinPos]) {
		/*---------------------------------------------------------------------
		 * SHORT
		 *-------------------------------------------------------------------*/ 
		uEventMode = EVENT_SHORT;
		TRACE( "이벤트 : EVENT_SHORT(%.1f > %.1f)\n", pWaveData[nStartPos], pWaveData[nMinPos]);
		m_nPeakPoint = nMinPos;
		if(dRateX > 0) {
			// 위치 보정 (+- 100 구간에서만 보정함)
			int nEndPos = nMinPos - 100;

			dMinValue = 9999999999.;
			// 기울기가 (+) 일 경우 y값이 최소인 점을 구한다.
			for(nIndex=nMinPos; nIndex>nEndPos; nIndex--) {
				if(pWaveData[nIndex] < dMinValue) {
					dMinValue	= pWaveData[nIndex];
					nMinPos		= nIndex;
				}
			}
		}

		// 관심구간 안(nStartPos, nMinPos)에서의 최고값의 위치 탐색
		nEndPos = nMinPos;
		dMaxValueInRange = -9999999999;
		for(nIndex=nStartPos; nIndex<nEndPos; nIndex++) {

			dValue = pWaveData[nIndex];
			if(dValue > dMaxValueInRange) {
				dMaxValueInRange	= dValue;
				nMaxPosInRange		= nIndex;
			}
		}

		// 최고점(nMaxPosInRange)과 최소점(nMinPos) 를 지나는 직선의 방정식을 구하고
		// a = (y2 - y1) / (x2 - x1)
		nStartPos	= nMaxPosInRange;
		nEndPos		= nMinPos;
		dRateX		= (pWaveData[nEndPos] - pWaveData[nStartPos]) / (nEndPos - nStartPos);

		dMaxValue = -9999999.0;
		// max(b) 값을 찾는다.
		for(nIndex=nStartPos; nIndex<nEndPos; nIndex++) {
			// b = y - a*x
			dValue = pWaveData[nIndex] - dRateX * nIndex;

			if(dValue > dMaxValue) {
				dMaxValue	= dValue;
				nMaxPos		= nIndex;
			}
		}
		// 보정된 위치값을 배정한다.
		*npEventPos = nMaxPos;
	}
	else {
		/*---------------------------------------------------------------------
		 * BROKEN
		 *-------------------------------------------------------------------*/ 
		uEventMode = EVENT_OPEN;
		TRACE( "이벤트 : EVENT_OPEN(%.1f < %.1f)\n", pWaveData[nStartPos], pWaveData[nMinPos]);
		m_nPeakPoint = nMaxPos;

		if(dRateX < 0) {
			// 위치 보정 (+- 100 구간에서만 보정함)
			int nEndPos = nMaxPos - 100;

			dMaxValue = -9999999.0;
			// 기울기가 (-) 일 경우 y값이 최고인 점을 찾는다.
			for(nIndex=nMinPos; nIndex>nEndPos; nIndex--) {
				if(pWaveData[nIndex] > dMaxValue) {
					dMaxValue	= pWaveData[nIndex];
					nMaxPos		= nIndex;
				}
			}
		}

		// 관심구간 안(nStartPos, nMaxPos)에서의 최소값의 위치 탐색
		nEndPos = nMaxPos;
		dMinValueInRange = 9999999999.0;
		for(nIndex=nStartPos; nIndex<nEndPos; nIndex++) {

			dValue = pWaveData[nIndex];
			if(dValue < dMinValueInRange) {
				dMinValueInRange	= dValue;
				nMinPosInRange		= nIndex;
			}
		}

		// 최소점(nMinPosInRange)과 최고점(nMaxPos) 를 지나는 직선의 방정식을 구하고
		// a = (y2 - y1) / (x2 - x1)
		nStartPos	= nMinPosInRange;
		nEndPos		= nMaxPos;
		dRateX		= (pWaveData[nEndPos] - pWaveData[nStartPos]) / (nEndPos - nStartPos);

		dMinValue = 999999999.0;
		// min(b) 값을 찾는다.
		for(nIndex=nStartPos; nIndex<nEndPos; nIndex++) {
			// b = y - a*x
			dValue = pWaveData[nIndex] - dRateX * nIndex;

			if(dValue < dMinValue) {
				dMinValue	= dValue;
				nMinPos		= nIndex;
			}
		}
		// 보정된 위치값을 배정한다.
		*npEventPos = nMinPos;
	}

	return uEventMode;
}

/*-----------------------------------------------------------------------------  
 * 함 수 명 : EVENT_MODE CGraphData::CalcAngle()
 * 목    적 : 관심구간의 양끝점을 잇는 직선을 상하 이동하여 만나는 그래프 상의
              점을 구한다. 
 * 설    명 : 
 * 작 성 자 : 김영규
 * 작성일자 : 2005.03.06
 * 변경사항 : 한 직선과 만나는 그래프상의 최소점 최고점을 구한다.
				y - y1 = a(x - x1) + b
				b = (y - y1) - a(x - x1)
				즉, b의 최소값과 최고값을 구한다.
 *---------------------------------------------------------------------------*/
EVENT_MODE CGraphData::FindEventWithAngle(int nStartPos, double* pWaveData, int* npEventPos)
{
	register int nIndex	=0;
	int		nPulseWidth	=GetPulseWidth(GetPulseWidthCode());
	int		nWindowSize	=SIZE_Window(nPulseWidth);
	int		nEndPos		=0;
	int		nMinPos		=0;
	int		nMaxPos		=0;
	double	dMaxValue	=-9999999999.;
	double	dMinValue	=9999999999.;
	double	dValue		= 0;

	EVENT_MODE	uEventMode	= EVENT_NONE;
	nWindowSize = 80;
	
	nEndPos = GetScanPos() + (int)(DIFF_Length(nPulseWidth) / m_WaveData.wScale) * 100;
	if(nEndPos > GetDataLength()) {
		nEndPos = GetDataLength();
	}

	for(nIndex=nStartPos; nIndex<nEndPos; nIndex++) {
		dValue = GetDegree(nIndex - nWindowSize, nIndex, nIndex + nWindowSize, pWaveData);
		
		if(dValue < dMinValue) {
			dMinValue = dValue;
			nMinPos = nIndex;
		}

		if(dValue > dMaxValue) {
			dMaxValue = dValue;
			nMaxPos = nIndex;
		}
	}

	/*-------------------------------------------------------------------------
	 *	이벤트 종류 판별
	 *-----------------------------------------------------------------------*/
	if(AbsValue(dMinValue) > AbsValue(dMaxValue)) {
		/*---------------------------------------------------------------------
		 * SHORT
		 *-------------------------------------------------------------------*/ 
		TRACE( "1. nMinPos = %d\n", nMinPos);
		*npEventPos = nMinPos;
		uEventMode = EVENT_SHORT;		
	}
	else {
		/*---------------------------------------------------------------------
		 * BROKEN
		 *-------------------------------------------------------------------*/ 
		TRACE( "1. nMaxPos = %d\n", nMaxPos);

		// 보정된 위치값을 배정한다.
		*npEventPos = nMinPos;
		uEventMode = EVENT_OPEN;
	}

	return uEventMode;
}

/*-----------------------------------------------------------------------------  
 * 함 수 명 : void CGraphData::GetAngle()
 * 목    적 : 
 * 설    명 : 
 * 작 성 자 : 김영규
 * 작성일자 : 2005.03.07
 * 변경사항 : 코사인 제2법칙 : a²= b²+ c²- 2bc cosA
 *---------------------------------------------------------------------------*/
double CGraphData::GetDegree(int nPrevPos, int nCurPos, int nNextPos, double* pWaveData)
{
	double PI = 3.14159265;
	double dPrevDist=0;
	double dCurDist=0;
	double dNextDist=0;
	double dCosineValue=0;
	double dDegree=0;

	dPrevDist	= sqrt(pow(pWaveData[nCurPos]  - pWaveData[nPrevPos], 2) + pow(nCurPos	- nPrevPos, 2));
	dNextDist	= sqrt(pow(pWaveData[nNextPos] - pWaveData[nCurPos]	, 2) + pow(nNextPos - nCurPos , 2));
	dCurDist	= sqrt(pow(pWaveData[nNextPos] - pWaveData[nPrevPos], 2) + pow(nNextPos - nPrevPos, 2));

	dCosineValue = (pow(dPrevDist,2) + pow(dNextDist,2) - pow(dCurDist,2)) / (2*dPrevDist*dNextDist);
	dDegree = acos(dCosineValue) * 180. / PI;

	return dDegree;
}

/*-----------------------------------------------------------------------------  
 * 함 수 명 : BOOL CGraphData::LoadCoefficients()
 * 목    적 : 보정계수를 화일에서 읽어온다.
 * 설    명 : 
 * 작 성 자 : 김영규
 * 작성일자 : 2006.03.02
 * 변경사항 :
 *---------------------------------------------------------------------------*/
BOOL CGraphData::LoadCoefficient(CString csCoefFile)
{
	#define _MAX_LENGTH_	128
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	m_csCoefFilePath = csCoefFile;
    CString csValue;
	DWORD dwRet=0;
	
	CFile coefFile;
	/*---------------------------------------------------------------------
	 * 보정계수 저장
	 *-------------------------------------------------------------------*/
	CFileException ex;
//	TRACE("보정계수  화일 : %s\n", csCoefFile);
	if (!coefFile.Open(csCoefFile, CFile::modeRead | CFile::shareDenyWrite, &ex))
	{
	  // complain if an error happened
	  // no need to delete the ex object

		if(!coefFile.Open(csCoefFile, CFile::modeWrite 
							   | CFile::shareExclusive 
							   | CFile::modeCreate))
		{
			TRACE ("Can't open Coefficient file: %s\n", csCoefFile);
//			API_Trace (Type_None, "Can't open Coefficient file: %s\n", csCoefFile);
			return FALSE;
		}
		TRACE( "화일 생성:[%s]\n", csCoefFile);
		coefFile.Close();

		///////////////////////////////////////////////////////////////////////
		// 거리보정 계수
		///////////////////////////////////////////////////////////////////////

		// 보정계수 정보
		WritePrivateProfileString("Information", "Title", "보정계수", csCoefFile);
		WritePrivateProfileString("Information", "LastModifiedDate", "1900-01-01", csCoefFile);

		// 단락 파형에 대한 보정계수
		WritePrivateProfileString("Channel 1", "Coefficient a", "19.392116", csCoefFile);
		WritePrivateProfileString("Channel 1", "Coefficient b", "0.83156346", csCoefFile);
		WritePrivateProfileString("Channel 1", "Coefficient c", "-0.000023591973", csCoefFile);
		WritePrivateProfileString("Channel 1", "Coefficient d", "0.0000000013855242", csCoefFile);
		WritePrivateProfileString("Channel 1", "Coefficient e", "0.0", csCoefFile);

		WritePrivateProfileString("Channel 2", "Coefficient a", "1.0720338", csCoefFile);
		WritePrivateProfileString("Channel 2", "Coefficient b", "0.80259793", csCoefFile);
		WritePrivateProfileString("Channel 2", "Coefficient c", "-0.00002070932", csCoefFile);
		WritePrivateProfileString("Channel 2", "Coefficient d", "0.0000000019319946", csCoefFile);
		WritePrivateProfileString("Channel 2", "Coefficient e", "0.0", csCoefFile);
		
		WritePrivateProfileString("Channel 3", "Coefficient a", "-0.0000000000014130919", csCoefFile);
		WritePrivateProfileString("Channel 3", "Coefficient b", "0.81104266", csCoefFile);
		WritePrivateProfileString("Channel 3", "Coefficient c", "-0.000003095534", csCoefFile);
		WritePrivateProfileString("Channel 3", "Coefficient d", "-0.0000000010381589", csCoefFile);
		WritePrivateProfileString("Channel 3", "Coefficient e", "0.0", csCoefFile);

		WritePrivateProfileString("Channel 4", "Coefficient a", "0.45567593", csCoefFile);
		WritePrivateProfileString("Channel 4", "Coefficient b", "0.81206127", csCoefFile);
		WritePrivateProfileString("Channel 4", "Coefficient c", "-0.000020658855", csCoefFile);
		WritePrivateProfileString("Channel 4", "Coefficient d", "0.0000000015000136", csCoefFile);
		WritePrivateProfileString("Channel 4", "Coefficient e", "0.0", csCoefFile);
		
		// 쇼트 파형에 대한 보정계수
		WritePrivateProfileString("Channel 1", "Coefficient as", "19.392116", csCoefFile);
		WritePrivateProfileString("Channel 1", "Coefficient bs", "0.83156346", csCoefFile);
		WritePrivateProfileString("Channel 1", "Coefficient cs", "-0.000023591973", csCoefFile);
		WritePrivateProfileString("Channel 1", "Coefficient ds", "0.0000000013855242", csCoefFile);
		WritePrivateProfileString("Channel 1", "Coefficient es", "0.0", csCoefFile);

		WritePrivateProfileString("Channel 2", "Coefficient as", "1.0720338", csCoefFile);
		WritePrivateProfileString("Channel 2", "Coefficient bs", "0.80259793", csCoefFile);
		WritePrivateProfileString("Channel 2", "Coefficient cs", "-0.00002070932", csCoefFile);
		WritePrivateProfileString("Channel 2", "Coefficient ds", "0.0000000019319946", csCoefFile);
		WritePrivateProfileString("Channel 2", "Coefficient es", "0.0", csCoefFile);
		
		WritePrivateProfileString("Channel 3", "Coefficient as", "-0.0000000000014130919", csCoefFile);
		WritePrivateProfileString("Channel 3", "Coefficient bs", "0.81104266", csCoefFile);
		WritePrivateProfileString("Channel 3", "Coefficient cs", "-0.000003095534", csCoefFile);
		WritePrivateProfileString("Channel 3", "Coefficient ds", "-0.0000000010381589", csCoefFile);
		WritePrivateProfileString("Channel 3", "Coefficient es", "0.0", csCoefFile);

		WritePrivateProfileString("Channel 4", "Coefficient as", "0.45567593", csCoefFile);
		WritePrivateProfileString("Channel 4", "Coefficient bs", "0.81206127", csCoefFile);
		WritePrivateProfileString("Channel 4", "Coefficient cs", "-0.000020658855", csCoefFile);
		WritePrivateProfileString("Channel 4", "Coefficient ds", "0.0000000015000136", csCoefFile);
		WritePrivateProfileString("Channel 4", "Coefficient es", "0.0", csCoefFile);
		
		///////////////////////////////////////////////////////////////////////
		// 피크점와 동선거리 간의 함수
		///////////////////////////////////////////////////////////////////////		
		WritePrivateProfileString("Channel 1 : Peak to Distance", "Coefficient a", "-344.39018", csCoefFile);
		WritePrivateProfileString("Channel 1 : Peak to Distance", "Coefficient b", "0.92335433", csCoefFile);
		WritePrivateProfileString("Channel 1 : Peak to Distance", "Coefficient c", "0", csCoefFile);
		WritePrivateProfileString("Channel 1 : Peak to Distance", "Coefficient d", "0", csCoefFile);

		WritePrivateProfileString("Channel 2 : Peak to Distance", "Coefficient a", "-344.39018", csCoefFile);
		WritePrivateProfileString("Channel 2 : Peak to Distance", "Coefficient b", "0.92335433", csCoefFile);
		WritePrivateProfileString("Channel 2 : Peak to Distance", "Coefficient c", "0", csCoefFile);
		WritePrivateProfileString("Channel 2 : Peak to Distance", "Coefficient d", "0", csCoefFile);

		WritePrivateProfileString("Channel 3 : Peak to Distance", "Coefficient a", "-344.39018", csCoefFile);
		WritePrivateProfileString("Channel 3 : Peak to Distance", "Coefficient b", "0.92335433", csCoefFile);
		WritePrivateProfileString("Channel 3 : Peak to Distance", "Coefficient c", "0", csCoefFile);
		WritePrivateProfileString("Channel 3 : Peak to Distance", "Coefficient d", "0", csCoefFile);

		WritePrivateProfileString("Channel 4 : Peak to Distance", "Coefficient a", "-344.39018", csCoefFile);
		WritePrivateProfileString("Channel 4 : Peak to Distance", "Coefficient b", "0.92335433", csCoefFile);
		WritePrivateProfileString("Channel 4 : Peak to Distance", "Coefficient c", "0", csCoefFile);
		WritePrivateProfileString("Channel 4 : Peak to Distance", "Coefficient d", "0", csCoefFile);

		///////////////////////////////////////////////////////////////////////
		// 피크점이 그리는 그래프 함수
		///////////////////////////////////////////////////////////////////////		
		WritePrivateProfileString("Peak Graph", "Coefficient a", "554463.1", csCoefFile);
		WritePrivateProfileString("Peak Graph", "Coefficient b", "-40.018333", csCoefFile);
		WritePrivateProfileString("Peak Graph", "Coefficient c", "0.003128078", csCoefFile);
		WritePrivateProfileString("Peak Graph", "Coefficient d", "-0.000000081714941", csCoefFile);
		
		// 쇼트 파형에 대한 보정계수
		WritePrivateProfileString("Peak Graph", "Coefficient as", "202995.82", csCoefFile);
		WritePrivateProfileString("Peak Graph", "Coefficient bs", "56.254267", csCoefFile);
		WritePrivateProfileString("Peak Graph", "Coefficient cs", "-0.006143864", csCoefFile);
		WritePrivateProfileString("Peak Graph", "Coefficient ds", "0.00000022931292", csCoefFile);
		
		///////////////////////////////////////////////////////////////////////
		// 변곡점과 Peak 점간의 높이
		///////////////////////////////////////////////////////////////////////
		WritePrivateProfileString("Channel 1 : Turn To Peak Height", "Coefficient a", "202995.82", csCoefFile);
		WritePrivateProfileString("Channel 1 : Turn To Peak Height", "Coefficient a", "202995.82", csCoefFile);
		WritePrivateProfileString("Channel 1 : Turn To Peak Height", "Coefficient a", "202995.82", csCoefFile);
		WritePrivateProfileString("Channel 1 : Turn To Peak Height", "Coefficient a", "202995.82", csCoefFile);

		WritePrivateProfileString("Channel 1 : Turn To Peak Height", "Coefficient as", "202995.82", csCoefFile);
		WritePrivateProfileString("Channel 1 : Turn To Peak Height", "Coefficient as", "202995.82", csCoefFile);
		WritePrivateProfileString("Channel 1 : Turn To Peak Height", "Coefficient as", "202995.82", csCoefFile);
		WritePrivateProfileString("Channel 1 : Turn To Peak Height", "Coefficient as", "202995.82", csCoefFile);

		WritePrivateProfileString("Channel 2 : Turn To Peak Height", "Coefficient a", "202995.82", csCoefFile);
		WritePrivateProfileString("Channel 2 : Turn To Peak Height", "Coefficient a", "202995.82", csCoefFile);
		WritePrivateProfileString("Channel 2 : Turn To Peak Height", "Coefficient a", "202995.82", csCoefFile);
		WritePrivateProfileString("Channel 2 : Turn To Peak Height", "Coefficient a", "202995.82", csCoefFile);

		WritePrivateProfileString("Channel 2 : Turn To Peak Height", "Coefficient as", "202995.82", csCoefFile);
		WritePrivateProfileString("Channel 2 : Turn To Peak Height", "Coefficient as", "202995.82", csCoefFile);
		WritePrivateProfileString("Channel 2 : Turn To Peak Height", "Coefficient as", "202995.82", csCoefFile);
		WritePrivateProfileString("Channel 2 : Turn To Peak Height", "Coefficient as", "202995.82", csCoefFile);
	}

	///////////////////////////////////////////////////////////////////////////
	// 보정계수 정보
	///////////////////////////////////////////////////////////////////////////
	GetPrivateProfileString("Information", "Title", "보정계수", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
	m_csCoefTitle = csValue;
	
	GetPrivateProfileString("Information", "LastModifiedDate", "2006-06-13", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
	m_csCoefLastModifiedDate = csValue;

	// 보정공식
	GetPrivateProfileString("Channel 1", "fit method", "n/a", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
	if(csValue != "n/a") m_strFitMethod[0] = csValue;
	GetPrivateProfileString("Channel 2", "fit method", "n/a", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
	if(csValue != "n/a") m_strFitMethod[1] = csValue;
	GetPrivateProfileString("Channel 3", "fit method", "n/a", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
	if(csValue != "n/a") m_strFitMethod[2] = csValue;
	GetPrivateProfileString("Channel 4", "fit method", "n/a", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
	if(csValue != "n/a") m_strFitMethod[3] = csValue;

	///////////////////////////////////////////////////////////////////////////
	// 단락 파형에 대한 보정계수
	///////////////////////////////////////////////////////////////////////////
	GetPrivateProfileString("Channel 1", "Coefficient a", "0", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dValueA[0] = atof(csValue);
    GetPrivateProfileString("Channel 1", "Coefficient b", "0", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dValueB[0] = atof(csValue);
    GetPrivateProfileString("Channel 1", "Coefficient c", "0", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dValueC[0] = atof(csValue);
    GetPrivateProfileString("Channel 1", "Coefficient d", "0", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dValueD[0] = atof(csValue);
    GetPrivateProfileString("Channel 1", "Coefficient e", "0", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dValueE[0] = atof(csValue);


	GetPrivateProfileString("Channel 2", "Coefficient a", "0", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dValueA[1] = atof(csValue);
    GetPrivateProfileString("Channel 2", "Coefficient b", "0", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dValueB[1] = atof(csValue);
    GetPrivateProfileString("Channel 2", "Coefficient c", "0", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dValueC[1] = atof(csValue);
    GetPrivateProfileString("Channel 2", "Coefficient d", "0", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dValueD[1] = atof(csValue);
    GetPrivateProfileString("Channel 2", "Coefficient e", "0", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dValueE[1] = atof(csValue);


	GetPrivateProfileString("Channel 3", "Coefficient a", "0", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dValueA[2] = atof(csValue);
    GetPrivateProfileString("Channel 3", "Coefficient b", "0", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dValueB[2] = atof(csValue);
    GetPrivateProfileString("Channel 3", "Coefficient c", "0", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dValueC[2] = atof(csValue);
    GetPrivateProfileString("Channel 3", "Coefficient d", "0", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dValueD[2] = atof(csValue);
    GetPrivateProfileString("Channel 3", "Coefficient e", "0", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dValueE[2] = atof(csValue);


	GetPrivateProfileString("Channel 4", "Coefficient a", "0", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dValueA[3] = atof(csValue);
    GetPrivateProfileString("Channel 4", "Coefficient b", "0", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dValueB[3] = atof(csValue);
    GetPrivateProfileString("Channel 4", "Coefficient c", "0", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dValueC[3] = atof(csValue);
    GetPrivateProfileString("Channel 4", "Coefficient d", "0", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dValueD[3] = atof(csValue);
    GetPrivateProfileString("Channel 4", "Coefficient e", "0", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dValueE[3] = atof(csValue);
	
	//////////////////////////////////////////////////////////////////////////
	// 쇼트 파형에 대한 보정계수
	//////////////////////////////////////////////////////////////////////////
	GetPrivateProfileString("Channel 1", "Coefficient as", "0", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dValueAs[0] = atof(csValue);
    GetPrivateProfileString("Channel 1", "Coefficient bs", "0", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dValueBs[0] = atof(csValue);
    GetPrivateProfileString("Channel 1", "Coefficient cs", "0", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dValueCs[0] = atof(csValue);
    GetPrivateProfileString("Channel 1", "Coefficient ds", "0", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dValueDs[0] = atof(csValue);
    GetPrivateProfileString("Channel 1", "Coefficient es", "0", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dValueEs[0] = atof(csValue);


	GetPrivateProfileString("Channel 2", "Coefficient as", "0", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dValueAs[1] = atof(csValue);
    GetPrivateProfileString("Channel 2", "Coefficient bs", "0", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dValueBs[1] = atof(csValue);
    GetPrivateProfileString("Channel 2", "Coefficient cs", "0", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dValueCs[1] = atof(csValue);
    GetPrivateProfileString("Channel 2", "Coefficient ds", "0", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dValueDs[1] = atof(csValue);
    GetPrivateProfileString("Channel 2", "Coefficient es", "0", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dValueEs[1] = atof(csValue);


	GetPrivateProfileString("Channel 3", "Coefficient as", "0", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dValueAs[2] = atof(csValue);
    GetPrivateProfileString("Channel 3", "Coefficient bs", "0", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dValueBs[2] = atof(csValue);
    GetPrivateProfileString("Channel 3", "Coefficient cs", "0", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dValueCs[2] = atof(csValue);
    GetPrivateProfileString("Channel 3", "Coefficient ds", "0", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dValueDs[2] = atof(csValue);
    GetPrivateProfileString("Channel 3", "Coefficient es", "0", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dValueEs[2] = atof(csValue);


	GetPrivateProfileString("Channel 4", "Coefficient as", "0", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dValueAs[3] = atof(csValue);
    GetPrivateProfileString("Channel 4", "Coefficient bs", "0", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dValueBs[3] = atof(csValue);
    GetPrivateProfileString("Channel 4", "Coefficient cs", "0", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dValueCs[3] = atof(csValue);
    GetPrivateProfileString("Channel 4", "Coefficient ds", "0", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dValueDs[3] = atof(csValue);
    GetPrivateProfileString("Channel 4", "Coefficient es", "0", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dValueEs[3] = atof(csValue);

	//////////////////////////////////////////////////////////////////////////
	// 피크점와 감지선길이 간의 함수
	//////////////////////////////////////////////////////////////////////////
	// OPEN 보정계수
    GetPrivateProfileString("Channel 1 : Peak to Distance", "Coefficient a", "0", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dPeakValueA[0] = atof(csValue);
    GetPrivateProfileString("Channel 1 : Peak to Distance", "Coefficient b", "0", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dPeakValueB[0] = atof(csValue);
    GetPrivateProfileString("Channel 1 : Peak to Distance", "Coefficient c", "0", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dPeakValueC[0] = atof(csValue);
    GetPrivateProfileString("Channel 1 : Peak to Distance", "Coefficient d", "0", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dPeakValueD[0] = atof(csValue);

    GetPrivateProfileString("Channel 2 : Peak to Distance", "Coefficient a", "0", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dPeakValueA[1] = atof(csValue);
    GetPrivateProfileString("Channel 2 : Peak to Distance", "Coefficient b", "0", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dPeakValueB[1] = atof(csValue);
    GetPrivateProfileString("Channel 2 : Peak to Distance", "Coefficient c", "0", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dPeakValueC[1] = atof(csValue);
    GetPrivateProfileString("Channel 2 : Peak to Distance", "Coefficient d", "0", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dPeakValueD[1] = atof(csValue);

    GetPrivateProfileString("Channel 3 : Peak to Distance", "Coefficient a", "0", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dPeakValueA[2] = atof(csValue);
    GetPrivateProfileString("Channel 3 : Peak to Distance", "Coefficient b", "0", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dPeakValueB[2] = atof(csValue);
    GetPrivateProfileString("Channel 3 : Peak to Distance", "Coefficient c", "0", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dPeakValueC[2] = atof(csValue);
    GetPrivateProfileString("Channel 3 : Peak to Distance", "Coefficient d", "0", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dPeakValueD[2] = atof(csValue);

    GetPrivateProfileString("Channel 4 : Peak to Distance", "Coefficient a", "0", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dPeakValueA[3] = atof(csValue);
    GetPrivateProfileString("Channel 4 : Peak to Distance", "Coefficient b", "0", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dPeakValueB[3] = atof(csValue);
    GetPrivateProfileString("Channel 4 : Peak to Distance", "Coefficient c", "0", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dPeakValueC[3] = atof(csValue);
    GetPrivateProfileString("Channel 4 : Peak to Distance", "Coefficient d", "0", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dPeakValueD[3] = atof(csValue);

	///////////////////////////////////////////////////////////////////////////
	// 피크점이 그리는 그래프 함수
	///////////////////////////////////////////////////////////////////////////		
    GetPrivateProfileString("Peak Graph", "Coefficient a", "554463.1", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dPeakGraphA = atof(csValue);
    GetPrivateProfileString("Peak Graph", "Coefficient b", "-40.018333", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dPeakGraphB = atof(csValue);
    GetPrivateProfileString("Peak Graph", "Coefficient c", "0.003128078", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dPeakGraphC = atof(csValue);
    GetPrivateProfileString("Peak Graph", "Coefficient d", "-0.000000081714941", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dPeakGraphD = atof(csValue);

    GetPrivateProfileString("Peak Graph", "Coefficient as", "202995.82", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dPeakGraphAs = atof(csValue);
    GetPrivateProfileString("Peak Graph", "Coefficient bs", "56.254267", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dPeakGraphBs = atof(csValue);
    GetPrivateProfileString("Peak Graph", "Coefficient cs", "-0.006143864", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dPeakGraphCs = atof(csValue);
    GetPrivateProfileString("Peak Graph", "Coefficient ds", "0.00000022931292", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dPeakGraphDs = atof(csValue);


	///////////////////////////////////////////////////////////////////////////
	// 변곡점과 Peak 점간의 높이
	///////////////////////////////////////////////////////////////////////////
	GetPrivateProfileString("Channel 1 : Turn To Peak Height", "Coefficient a", "-8796.409", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dTurnToPeakA[0] = atof(csValue);
    GetPrivateProfileString("Channel 1 : Turn To Peak Height", "Coefficient b", "0.3960794", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dTurnToPeakB[0] = atof(csValue);
    GetPrivateProfileString("Channel 1 : Turn To Peak Height", "Coefficient c", "673154070000", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dTurnToPeakC[0] = atof(csValue);
    GetPrivateProfileString("Channel 1 : Turn To Peak Height", "Coefficient d", "0", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dTurnToPeakD[0] = atof(csValue);

    GetPrivateProfileString("Channel 1 : Turn To Peak Height", "Coefficient as", "119916.5", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dTurnToPeakAs[0] = atof(csValue);
    GetPrivateProfileString("Channel 1 : Turn To Peak Height", "Coefficient bs", "-26.077865", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dTurnToPeakBs[0] = atof(csValue);
    GetPrivateProfileString("Channel 1 : Turn To Peak Height", "Coefficient cs", "0.0018837164", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dTurnToPeakCs[0] = atof(csValue);
    GetPrivateProfileString("Channel 1 : Turn To Peak Height", "Coefficient ds", "-0.000000045440333", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dTurnToPeakDs[0] = atof(csValue);


	GetPrivateProfileString("Channel 2 : Turn To Peak Height", "Coefficient a", "0.09952698", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dTurnToPeakA[1] = atof(csValue);
    GetPrivateProfileString("Channel 2 : Turn To Peak Height", "Coefficient b", "0.99927308", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dTurnToPeakB[1] = atof(csValue);
    GetPrivateProfileString("Channel 2 : Turn To Peak Height", "Coefficient c", "1.8518757", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dTurnToPeakC[1] = atof(csValue);
    GetPrivateProfileString("Channel 2 : Turn To Peak Height", "Coefficient d", "0", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dTurnToPeakD[1] = atof(csValue);

    GetPrivateProfileString("Channel 2 : Turn To Peak Height", "Coefficient as", "246715.57", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dTurnToPeakAs[1] = atof(csValue);
    GetPrivateProfileString("Channel 2 : Turn To Peak Height", "Coefficient bs", "-1.8612275", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dTurnToPeakBs[1] = atof(csValue);
    GetPrivateProfileString("Channel 2 : Turn To Peak Height", "Coefficient cs", "-0.00099543975", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dTurnToPeakCs[1] = atof(csValue);
    GetPrivateProfileString("Channel 2 : Turn To Peak Height", "Coefficient ds", "1.6594859", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dTurnToPeakDs[1] = atof(csValue);


	GetPrivateProfileString("Channel 3 : Turn To Peak Height", "Coefficient a", "0", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dTurnToPeakA[2] = atof(csValue);
    GetPrivateProfileString("Channel 3 : Turn To Peak Height", "Coefficient b", "0", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dTurnToPeakB[2] = atof(csValue);
    GetPrivateProfileString("Channel 3 : Turn To Peak Height", "Coefficient c", "0", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dTurnToPeakC[2] = atof(csValue);
    GetPrivateProfileString("Channel 3 : Turn To Peak Height", "Coefficient d", "0", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dTurnToPeakD[2] = atof(csValue);

    GetPrivateProfileString("Channel 3 : Turn To Peak Height", "Coefficient as", "0", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dTurnToPeakAs[2] = atof(csValue);
    GetPrivateProfileString("Channel 3 : Turn To Peak Height", "Coefficient bs", "0", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dTurnToPeakBs[2] = atof(csValue);
    GetPrivateProfileString("Channel 3 : Turn To Peak Height", "Coefficient cs", "0", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dTurnToPeakCs[2] = atof(csValue);
    GetPrivateProfileString("Channel 3 : Turn To Peak Height", "Coefficient ds", "0", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dTurnToPeakDs[2] = atof(csValue);


	GetPrivateProfileString("Channel 4 : Turn To Peak Height", "Coefficient a", "0", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dTurnToPeakA[3] = atof(csValue);
    GetPrivateProfileString("Channel 4 : Turn To Peak Height", "Coefficient b", "0", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dTurnToPeakB[3] = atof(csValue);
    GetPrivateProfileString("Channel 4 : Turn To Peak Height", "Coefficient c", "0", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dTurnToPeakC[3] = atof(csValue);
    GetPrivateProfileString("Channel 4 : Turn To Peak Height", "Coefficient d", "0", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dTurnToPeakD[3] = atof(csValue);

    GetPrivateProfileString("Channel 4 : Turn To Peak Height", "Coefficient as", "0", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dTurnToPeakAs[3] = atof(csValue);
    GetPrivateProfileString("Channel 4 : Turn To Peak Height", "Coefficient bs", "0", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dTurnToPeakBs[3] = atof(csValue);
    GetPrivateProfileString("Channel 4 : Turn To Peak Height", "Coefficient cs", "0", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dTurnToPeakCs[3] = atof(csValue);
    GetPrivateProfileString("Channel 4 : Turn To Peak Height", "Coefficient ds", "0", csValue.GetBuffer(_MAX_LENGTH_), _MAX_LENGTH_, csCoefFile);
    m_dTurnToPeakDs[3] = atof(csValue);


	double dSum= m_dPeakValueA[0] + m_dPeakValueB[1] + m_dPeakValueC[2] + m_dPeakValueD[3];

	if(dSum != 0) {
		m_bCoefficientLoaded = TRUE;
//		TRACE ("보정계수 [%s] Loaded. LastModified[%s]\n", m_csCoefTitle, m_csCoefLastModifiedDate);
		TRACE("보정계수 [%s] Loaded. LastModified[%s]\n", m_csCoefTitle, m_csCoefLastModifiedDate);
	}

	return TRUE;
}

/*-----------------------------------------------------------------------------  
 * 함 수 명 : BOOL CGraphData::IsCoefficientLoaded()
 * 목    적 : 
 * 설    명 : 
 * 작 성 자 : 김영규
 * 작성일자 : 2006.03.02
 * 변경사항 :
 *---------------------------------------------------------------------------*/
BOOL CGraphData::IsCoefficientLoaded()
{
	return m_bCoefficientLoaded;
}

/*-----------------------------------------------------------------------------  
 * 함 수 명 : BOOL CGraphData::IsSamePulseWidth()
 * 목    적 : 
 * 설    명 : 
 * 작 성 자 : 김영규
 * 작성일자 : 2006.06.02
 * 변경사항 :
 *---------------------------------------------------------------------------*/
BOOL CGraphData::IsSamePulseWidth()
{
	if(m_bBaseLoaded) {
		if(m_bMeasureLoaded) {
			if(m_BaseInfo.nPulseWidth == m_MeasureInfo.nPulseWidth) {
				return TRUE;
			}
		}
	}

	return FALSE;
}

/*-----------------------------------------------------------------------------  
 * 함 수 명 : double CGraphData::GetSignalValue()
 * 목    적 : 
 * 설    명 : 
 * 작 성 자 : 김영규
 * 작성일자 : 2006.06.13
 * 변경사항 :
 *---------------------------------------------------------------------------*/
void CGraphData::GetSignalValue(int nChannel, double dDist, double* dpOpenValue, double* dpShortValue)
{
	CString csValue;

	if(nChannel<0 || nChannel > 3) return;
	if(dDist <=0) return;
	
	switch(nChannel+1) {
	case 1:
		// Heat Capacity Model: y=a+bx+c/x^2
		*dpOpenValue = m_dTurnToPeakA[nChannel] 
					 + m_dTurnToPeakB[nChannel] * dDist
					 + m_dTurnToPeakC[nChannel] / pow(dDist, 2);
		// 3rd degree Polynomial Fit:  y=a+bx+cx^2+dx^3...
		*dpShortValue = m_dTurnToPeakAs[nChannel] 
					  + m_dTurnToPeakBs[nChannel] * dDist
					  + m_dTurnToPeakCs[nChannel] * pow(dDist, 2)
					  + m_dTurnToPeakDs[nChannel] * pow(dDist, 3);
		break;
	case 2:
		// Hoerl Model: y=a*(b^x)*(x^c)
		*dpOpenValue = m_dTurnToPeakA[nChannel] 
					 * pow(m_dTurnToPeakB[nChannel], dDist) 
					 * pow(dDist, m_dTurnToPeakC[nChannel]);
		// Richards Model: y=a/(1+exp(b-cx)^(1/d))
		*dpShortValue = m_dTurnToPeakAs[nChannel]/(1+pow(exp(m_dTurnToPeakBs[nChannel]-m_dTurnToPeakCs[nChannel]*dDist), (1/m_dTurnToPeakDs[nChannel]))); 
		break;

	case 3:
		// Hoerl Model: y=a*(b^x)*(x^c)
		*dpOpenValue = m_dTurnToPeakA[nChannel] 
					 * pow(m_dTurnToPeakB[nChannel], dDist) 
					 * pow(dDist, m_dTurnToPeakC[nChannel]);
		// Richards Model: y=a/(1+exp(b-cx)^(1/d))
		*dpShortValue = m_dTurnToPeakAs[nChannel]/(1+pow(exp(m_dTurnToPeakBs[nChannel]-m_dTurnToPeakCs[nChannel]*dDist), (1/m_dTurnToPeakDs[nChannel]))); 
		break;
	
	case 4:
		// Hoerl Model: y=a*(b^x)*(x^c)
		*dpOpenValue = m_dTurnToPeakA[nChannel] 
					 * pow(m_dTurnToPeakB[nChannel], dDist) 
					 * pow(dDist, m_dTurnToPeakC[nChannel]);
		// Richards Model: y=a/(1+exp(b-cx)^(1/d))
		*dpShortValue = m_dTurnToPeakAs[nChannel]/(1+pow(exp(m_dTurnToPeakBs[nChannel]-m_dTurnToPeakCs[nChannel]*dDist), (1/m_dTurnToPeakDs[nChannel]))); 
		break;
	}
}
