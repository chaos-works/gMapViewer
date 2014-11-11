#ifndef _DEFINE_H_
#define _DEFINE_H_
/*
 *	공통 
 */
//#ifdef _TDR_VER_8_
typedef LONG ORG_TYPE;
//#else
//typedef WORD ORG_TYPE;
//#endif

#define TIMER_SECOND			1	// 자동측정 시의 메인 클럭(1초)
#define TIMER_START				2	// 시작 버튼 클릭 후의 딜레이
#define TIMER_TIMEOUT			3	// TDR 명령전송 후의 타임아웃 타이머(1분)
#define TIMER_RESEND			4	// 응답확인 후 명령 재전송
#define TIMER_NEXT_COMMAND		5	// 측정명령 전송후 > 딜레이 > 다음 명령 전송
#define TIMER_DOG				6

#define DEVICE_RESPONSE_TIME	15000 // 응답대기 시간 : 15초
#define CHANNEL_NUM				8 //8채널 사용

#define _IP_SYNC_WITH_SMS_		1	// 유동IP를 사용하고 서버와의 IP 동기화를 SMS로 처리

#define SAFE_STR(str) ((str==NULL) ? _T("") : str)
#define pMainWnd ((CRtdFirmwareDlg*)AfxGetMainWnd())

#define	WM_COMM_READ			(WM_USER+1)
#define WM_COPYDATA_PROCESS		(WM_USER + 100)
#define WM_TRANSFER_COMPLETE	(WM_USER + 101)
#define WM_RAS_FAILED			(WM_USER + 102)
#define WM_FTP_FAILED			(WM_USER + 103)
#define WM_FTP_CONNECTED		(WM_USER + 104)
#define WM_FTP_DISCONNECTED		(WM_USER + 105)
#define WM_FTPIP_REQ			(WM_USER + 106)
#define UM_NEW_MESSAGE			(WM_USER + 107)
#define UM_CONFIG_LOADED		(WM_USER + 108)
#define UM_BREAK				(WM_USER + 109)

// 보정계수 저장화일
#define szCoefFile "Coefficient.ini"

// MDB 패스워드
//#define DB_PASSWD		"rtdlog"
#define szTimeString	"%Y-%m-%d %a %H:%M:%S"
typedef enum tag_FtpDataType 
{ 
	TYPE_BmpToJpg, 
	TYPE_Binary, 
	TYPE_Text 
} FTP_DATATYPE;

// LOG 타입
enum Type_Loggers
{
    Type_None = 0,
    Type_CDMA,
    Type_TDR,
    Type_ChannelDivider,
    Type_FTP,
    Type_CORREL,
    Type_SMS
};

// 바이트 정렬 때문에 해줌 Waveview
#pragma pack(push,1)
/*
 * 측정데이타 화일을 읽기위한 구조체 정의
 * Ver 1.x
 */

// 측정화일의 웨이브 정보
typedef struct tagNANOWAVE_INFO {
    WORD	wHead;
    WORD	wCommand;
    WORD	wLength;
    WORD	wCheckSum;
} NANOWAVE_INFO, *LPNANOWAVE_INFO;
#define NANOWAVE_INFO_SIZE sizeof(NANOWAVE_INFO)
// 측정화일의 웨이브 데이타
typedef struct tagNANOWAVE_DATA {
    WORD	wHead;
    WORD	wCommand;
    WORD	wLength;
    WORD	wScale;
    WORD	wVop;
    WORD	wThreshold;
//	DWORD	dwThresholdDefaultValue;
    WORD	wPulseWidth;
    DWORD	dwStart;
    BYTE	szName[6];
} NANOWAVE_DATA, *LPNANOWAVE_DATA;
/*
 * 측정데이타 화일을 읽기위한 구조체 정의
 * Ver 2.x
 */
// 측정화일의 웨이브 정보
typedef struct tagNANOWAVE_2X_INFO {
    short	sDeviceHeader;
    short	sFileHeader;
	short	sDataInfoCount;			// 파일개수 정보 : 현재는 1
    short	sFileHeaderEndCheck;	// 0xF0F0
} NANOWAVE_2X_INFO, *LPNANOWAVE_2X_INFO;

#define NANOWAVE_2X_INFO_SIZE sizeof(NANOWAVE_2X_INFO)

// 측정화일의 웨이브 데이타
// Data Info Count 만큼 반복 : 현재는 1이므로 한번
typedef struct tagNANOWAVE_2X_DATA {
    short	sDeviceHeader;
    short	sDataHeader;
    short	sDataValueCount; // 총 Data 개수 정보
    short	sScale;
    short	sVop;
	short	sThreshold;
    INT		nThresholdDefaultValue;
    short	sPulseWidth;
    INT		nStartPosition;
    BYTE	szName[6];
} NANOWAVE_2X_DATA, *LPNANOWAVE_2X_DATA;

typedef struct tagNANOWAVE_2X_END {
//  INT	sCursor1Position;
//  INT	sCursor2Position;
//	INT	sZeroPosition;
    short	sDataHeaderEndCheck;
} NANOWAVE_2X_END, *LPNANOWAVE_2X_END;

// 거리 계산 결과 데이타
typedef struct tagNANOWAVE_EX {
    WORD	wHead;		// 33793 (=0x8401)
    WORD	wFlag;		// 0:Normal, 1:누수, 2:파손, 3:노이즈
    double	dInitLine;	// 인입선 길이
    WORD	wStartPos;	// Start Scan Pos
    WORD	wEndPos;	// End Scan Pos
    double	dLength;	// Distance
    WORD	wResStatus;	// 저항 측정결과 -- 0:Normal, 1:누수, 2:파손, 3:노이즈
    double	dResValue;	// 저항측정값
} NANOWAVE_EX, *LPNANOWAVE_EX;
#pragma pack(pop)

#define NANOWAVE_DATA_SIZE sizeof(NANOWAVE_DATA)
#define NANOWAVE_2X_DATA_SIZE sizeof(NANOWAVE_2X_DATA)
#define NANOWAVE_HEADSIZE sizeof(NANOWAVE_INFO) + sizeof(NANOWAVE_DATA)
#define NANOWAVE_2X_HEADSIZE sizeof(NANOWAVE_2X_INFO) + sizeof(NANOWAVE_2X_DATA)

// Live Signal 구분
typedef enum tagSIGNAL_FLAG { 
	STARTUP_SIGNAL,
	LIVE_SIGNAL	
} SIGNAL_FLAG;

// Track Mode
typedef enum tagTRACK_MODE { 
	NONE,
	TRACK,
	ZOOM,
	PANXY,
	PANX,
	PANY,
	CURSOR
} TRACK_MODE;

// 이벤트 여부
typedef enum tagEVENT_ONOFF { 
	EVENT_ON,
	EVENT_OFF
} EVENT_ONOFF;

// 단선, 쇼트 구분
typedef enum tagEVENT_MODE { 
	EVENT_OPEN,				// 단선
	EVENT_SHORT,			// 합선
	EVENT_DETECTED,			// 노이즈 파형	2007.2.5 김영규
	EVENT_SHORT_DETECTED,	// SHORT 파형 : 무시 2007.9.5 김영규
	EVENT_NONE				// 정상
} EVENT_MODE;

// 계산결과를 전송할 Target App 구분
typedef enum tagTARGET_APP { 
	PIPEGUARD_SVRMGR,
	PIPEGUARD,
	RTDFIRMWARE,
	APP_NONE
} TARGET_APP;

// RAS/SMS구분
typedef enum tagCOMM_MODE { 
	TCPIP,
	RAS,
	SMS
} CONN_MODE;

// Connection Mode
#define CONN_SMS	1000
#define CONN_TCPIP	1001
#define CONN_MODEM	1002

// 측정 웨이브 데이타의 잡음제거를 위해 사용하는 필터구분
typedef enum tagSMOOTHING_FILTER {
	MOVING_AVG_FILTER,
	BUFFER_WORTH_FILTER,
	SAVITZKY_GOLAY_FILTER,
	CHAOSWORKS_FILTER,
	NONE_FILTER
} SMOOTHING_FILTER;

/*
 *	Value Defines
 */
#define GET_REGISTRY	TRUE
#define PUT_REGISTRY	FALSE

#define STEP_SIZE		100
#define SG_FILTER_WIDTH	 7  // Savitzky-Golay Filter의 필터 크기
#define MA_FILTER_WIDTH	 7  // Moving Average Filter의 필터 크기

typedef struct tagEVENTSCAN_INFO {
	double dResValue;			// 저항값
	double dOpenResValue;		// Open 파형의 탐색위치 결정하는 저항값
	int nOpenScanPos;		// Open 파형 탐색위치
	double dShortResValue;		// Short 파형의 탐색위치 결정하는 저항값
	int  nShortScanPos;		// Short 파형 탐색위치
} EVENTSCAN_INFO, *LPEVENTSCAN_INFO;

 /*-----------------------------------------------------------------------------
 * 이벤트 거리계산
 *---------------------------------------------------------------------------*/
// 거리계산에 사용되는 구조체
typedef struct tagEVENTDETECT_INFO {
    int nRangeStart;		// 스캔 범위 시작점
    int nRangeEnd;			// 스캔 범위 끝점
    int nStart;				// 시작점(=0)
    int nEvent;				// 이벤트 발생지점
	int nPulseWidth;		// Pulse Width
	POINT ptMin;			// 전구간에서의 Min 
	POINT ptMax;			// 전구간에서의 Max
	double dGround;			// 신호의 기준값
	EVENT_MODE uEventMode;	// Event Mode
} EVENTDETECT_INFO, *LPEVENTDETECT_INFO;
#define EVENTDETECT_INFO_SIZE sizeof(EVENTDETECT_INFO)

// 그래프별 구조체
typedef struct tagGRAPH_INFO {
    int nStart;				// 시작점(=0)
	int nLength;			// 데이타 길이
	int nPulseWidth;		// Pulse Width
	POINT ptMin;			// 전구간에서의 Min 
	POINT ptMax;			// 전구간에서의 Max
	// dBRL
	int nOutputSignalPos;	// 인가신호 (위치)
	double dOutputSignal;	// 인가신호

	CString csFilename;		// 화일명
	CString csPathname;		// 패스명

	double* pRawData;		// Raw Data	
	double* pWaveData;		// Filtered Data	
} GRAPH_INFO, *LPGRAPH_INFO;
#define GRAPH_INFO_SIZE sizeof(GRAPH_INFO)

#define  ERROR_RANGE 0.01

// 근거리 & 원거리를 감시구간의 경계
// 근거리 : 0 ~ 2500 pixel
// 원거리 : 2500 ~ 16000 pixel
#define BORDER_OF_ZONE 500

#define SHORT_PULSE 500

#define TURN2PEAK_LOW 75
#define TURN2PEAK_HIGH 150

// 다중펄스를 이용한 이벤트 감지
typedef struct tagMULTIPULSEEVENT_INFO {
	int nChannel;				// 채널
	BOOL bShortDistCheck;		// 근거리 이벤트 탐색 처리중
	double dHeightPercent;		// 피크대 높이값(%)
	EVENTDETECT_INFO EventInfo;	// 이벤트 정보
} MULTIPULSEEVENT_INFO, *LPMULTIPULSEEVENT_INFO;
#define MULTIPULSEEVENT_INFO_SIZE sizeof(MULTIPULSEEVENT_INFO)

// 코렐 전위 측정기
typedef struct tagCORREL_INFO {
	BOOL	bConnected;			// 연결 여부
	CString csVersion;			// 버젼 정보
	CString	csErrorCode;		// 에러코드
	double	dCurVolt;			// 현재 전위값
    BOOL	bEvent;				// 이벤트 발생 여부
	CString dDayVolt[24];		// 일 전위값
} CORREL_INF;

#define VOLT_THRESHOLD 800		// 기준 전위(800 ~ 1500)

typedef LPTOP_LEVEL_EXCEPTION_FILTER (WINAPI *pSetUnhandledExceptionFilter) (LPTOP_LEVEL_EXCEPTION_FILTER );		

#endif
