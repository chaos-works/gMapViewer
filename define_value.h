#ifndef _DEFINE_VALUE_H_
#define _DEFINE_VALUE_H_
/*
 *	공통 
 */

#define VIEW_MIN_SIZE 10
#define VIEW_GAP_SIZE 4

typedef enum tagVIEW_TYPE
{ 
	VT_ROADVIEW, 
	VT_WAVEVIEW
} VIEW_TYPE;

// sub window status
typedef struct tagSUB_WINDOW{
    BOOL	bVisible;
	BOOL	bEnable;
    CRect	crSize;
} SUB_WINDOW;

typedef struct tagAPP_INFO {
	VIEW_TYPE vtSelectedView;
    SUB_WINDOW	swRoadView;
    SUB_WINDOW	swWaveView;
} APP_INFO;

typedef enum ZOOM_DIR {ZOOM_UP, ZOOM_DOWN, ZOOM_LEFT, ZOOM_RIGHT};

typedef struct tagBASE_FILE {
	CString strPath; // 화일패스
	double *pWaveData; // 웨이브데이타
	BOOL bLoaded;
} BASE_FILE;

typedef struct tagMEASURE_FILE {
	CString strPath; // 화일패스
	double *pWaveData; // 웨이브데이타
	double *pFiltData; // 필터데이타
	BOOL bLoaded;
} MEASURE_FILE;

typedef struct tagDIFF_FILE {
	double *pWaveData; // 웨이브데이타
	double *pFiltData; // 필터데이타
	BOOL bLoaded;
} DIFF_FILE;

typedef struct tagGRAPH {
	BASE_FILE base; // 베이스화일
	MEASURE_FILE measure; // 측정화일
	DIFF_FILE diff; // DIFF화일
	
	CGraphData* m_pGraph;
	
	int nTdrVOP;
	int nScaleSave;
	double dThreshold;
	int nDistance;
} GRAPH;

typedef struct tagGRAPH_VIEW {
    CString csBasePath[4];
	CString csLastAccessDir;

	CString strCurDir; // 현재 디렉토리
	CStringArray saWaveFilelist;		// 화일리스트
	
	EVENT_MODE uEventMode;

	int	nCurFile;			// 현재화일 인덱스
	int	nFileCnt;			// 화일수

	int nCurPos1;
	int nCurPos2;

    BOOL bMeasureGraphVisible;
    BOOL bFilterGraphVisible;
    BOOL bBaseGraphVisible;
    BOOL bBaseFilterGraphVisible;
    BOOL bDiffGraphVisible;

	BOOL bUseBase;
	BOOL bFindPeak;
	BOOL bCursor1Visible;
	BOOL bCursor2Visible;

	int nTrackMode;
	BOOL bHoldZoom;
	double dMinX;
	double dMaxX;
	double dMinY;
	double dMaxY;
	UINT uZoomDir;			// Zoom 방향
	BOOL bZoomPressed;		// Zoom 버튼 클릭
	int nGraphSel;		// 현재 그래프 선택	
	
	int	nScanPos5;
	int	nScanPos15;
	int	nScanPos45;
	int	nScanPos100;
	int	nScanPos200;
	int	nScanPos500;
	int	nScanPos1000;
	int	nScanPos2000;
	int	nScanPos5000;
	int	nScanPos10000;

	int	nRepeatCnt;	// 노이즈 제거 처리(회수)	
	int	nSmoothErrCor;	// 평탄화 에러 보정치
	int	nCurChannel;		// 채널번호
	double dResValue; // 저항값.

	 
} GRAPH_VIEW;

#endif
