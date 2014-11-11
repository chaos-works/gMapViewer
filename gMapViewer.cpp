// gMapViewer.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "gMapViewer.h"
#include "gMapViewerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGMapViewerApp

BEGIN_MESSAGE_MAP(CGMapViewerApp, CWinApp)
	//{{AFX_MSG_MAP(CGMapViewerApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGMapViewerApp construction

CGMapViewerApp::CGMapViewerApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CGMapViewerApp object

CGMapViewerApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CGMapViewerApp initialization

BOOL CGMapViewerApp::InitInstance()
{
	char szDrive[256]={0,};
	char szDir[256]={0,};
	char szProgramPath[_MAX_PATH]={0,}; 
	
    m_hInstanceMutex = CreateMutex(NULL, FALSE, "gMapVieWer");
	
    if( GetLastError() == ERROR_ALREADY_EXISTS )
    {
		TRACE("gMapVieWer.exe 가 실행중입니다.\n");
        return FALSE;
    }
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.
	SetRegistryKey(_T("WACON"));

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// 실횅화일의 패스를 구함.
	GetModuleFileName( NULL, szProgramPath, _MAX_PATH);
	_splitpath(szProgramPath, szDrive, szDir, NULL, NULL);
	
	m_csExecPath.Format("%s%s", szDrive, szDir);
	m_csCoefPath.Format("%s%s", m_csExecPath, szCoefFile);
    m_csSettingPath.Format("%s%s", m_csExecPath, "RtdSetting.ini");
	
	m_coef.SetPathName(m_csCoefPath);
	LoadConfig();
	
	//m_bRealTimeCalcDist = TRUE;
	TRACE("실행화일 패스 : %s\n", m_csExecPath);
	TRACE("보정계수 화일 패스 : %s\n", m_csCoefPath);

	CGMapViewerDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	return FALSE;
}

void CGMapViewerApp::LoadConfig()
{
	CString csValue;
	/*-------------------------------------------------------------------------
	 *	최근에 억세스한 폴드 저장
	 *-----------------------------------------------------------------------*/
	m_gvInfo.csLastAccessDir = GetProfileString("Settings", "Last Access Directory", "G:");

	/*-------------------------------------------------------------------------
	 *	Base File Path
	 *-----------------------------------------------------------------------*/
    m_gvInfo.csBasePath[0] = GetProfileString("Settings", "Ch1 Basefile Path", ".");
	if(m_gvInfo.csBasePath[0] == ".") {
		m_gvInfo.csBasePath[0] = theApp.m_ini.GetString("Base File", "Ch-1 Base", "n/a");
	}
    m_gvInfo.csBasePath[1] = GetProfileString("Settings", "Ch2 Basefile Path", ".");
	if(m_gvInfo.csBasePath[1] == ".") {
		m_gvInfo.csBasePath[1] = theApp.m_ini.GetString("Base File", "Ch-2 Base", "n/a");
	}
    m_gvInfo.csBasePath[2] = GetProfileString("Settings", "Ch3 Basefile Path", ".");
	if(m_gvInfo.csBasePath[2] == ".") {
		m_gvInfo.csBasePath[2] = theApp.m_ini.GetString("Base File", "Ch-3 Base", "n/a");
	}
    m_gvInfo.csBasePath[3] = GetProfileString("Settings", "Ch4 Basefile Path", ".");
	if(m_gvInfo.csBasePath[3] == ".") {
		m_gvInfo.csBasePath[3] = theApp.m_ini.GetString("Base File", "Ch-4 Base", "n/a");
	}
	/*-------------------------------------------------------------------------
	 *	Graph Visible
	 *-----------------------------------------------------------------------*/
    m_gvInfo.bMeasureGraphVisible = GetProfileInt("Settings", "Measure Graph Visable", 1) ? TRUE:FALSE;
    m_gvInfo.bFilterGraphVisible = GetProfileInt("Settings", "Filtered Graph Visable", 1) ? TRUE:FALSE;
    m_gvInfo.bBaseGraphVisible = GetProfileInt("Settings", "Base Graph Visable", 1) ? TRUE:FALSE;
    m_gvInfo.bBaseFilterGraphVisible = GetProfileInt("Settings", "Filtered Base Graph Visable", 1) ? TRUE:FALSE;
    m_gvInfo.bDiffGraphVisible = GetProfileInt("Settings", "Difference Graph Visable", 1) ? TRUE:FALSE;

    m_gvInfo.bHoldZoom = GetProfileInt("Settings", "Zoom Hold", 0) ? TRUE:FALSE;
	m_gvInfo.bFindPeak = GetProfileInt("Settings", "Find Peak", 0) ? TRUE:FALSE;
	int nEventMode = GetProfileInt("Settings", "Event Mode", 1);
	switch(nEventMode)
	{
	case 0:
		m_gvInfo.uEventMode = EVENT_OPEN;
		break;
	case 1:
		m_gvInfo.uEventMode = EVENT_SHORT;
	    break;
	default:
		m_gvInfo.uEventMode = EVENT_NONE;
	}
		

    csValue = GetProfileString("Settings", "Zoom MinX", "0");
	m_gvInfo.dMinX = atof(csValue);
    csValue = GetProfileString("Settings", "Zoom MaxX", "1");
	m_gvInfo.dMaxX = atof(csValue);
    csValue = GetProfileString("Settings", "Zoom MinY", "1");
	m_gvInfo.dMinY = atof(csValue);
    csValue = GetProfileString("Settings", "Zoom MaxY", "0");
	m_gvInfo.dMaxY = atof(csValue);
	

	/*-------------------------------------------------------------------------
	 *	Cursor Visible
	 *-----------------------------------------------------------------------*/
	m_gvInfo.bCursor1Visible = GetProfileInt("Settings", "Cursor1 Visable", 0) ? TRUE:FALSE;
	m_gvInfo.bCursor2Visible = GetProfileInt("Settings", "Cursor2 Visable", 1) ? TRUE:FALSE;
    
	{
		m_gvInfo.bUseBase = GetProfileInt("Settings", "Use Basedata", 1) ? TRUE:FALSE;
	}
	/*-------------------------------------------------------------------------
	 *	Track Mode
	 *-----------------------------------------------------------------------*/
    m_gvInfo.nTrackMode			= GetProfileInt("Settings", "Track Mode", 2);

	/*-------------------------------------------------------------------------
	 *	Scan Pos
	 *-----------------------------------------------------------------------*/
	m_gvInfo.nScanPos5 = GetProfileInt("Settings", "ScanPos 5", 5);
	m_gvInfo.nScanPos15	= GetProfileInt("Settings", "ScanPos 15", 5);
	m_gvInfo.nScanPos45	= GetProfileInt("Settings", "ScanPos 45", 10);
	m_gvInfo.nScanPos100 = GetProfileInt("Settings", "ScanPos 100", 1000);
	m_gvInfo.nScanPos200 = GetProfileInt("Settings", "ScanPos 200", 1000);
	m_gvInfo.nScanPos500 = GetProfileInt("Settings", "ScanPos 500", 1000);
	m_gvInfo.nScanPos1000 = GetProfileInt("Settings", "ScanPos 1000", 1000);
	m_gvInfo.nScanPos2000 = GetProfileInt("Settings", "ScanPos 2000", 1000);
	m_gvInfo.nScanPos5000 = GetProfileInt("Settings", "ScanPos 5000", 1000);
	m_gvInfo.nScanPos10000 = GetProfileInt("Settings", "ScanPos 10000", 1000);

	/*-------------------------------------------------------------------------
	 *	Smoothing 적용횟수
	 *-----------------------------------------------------------------------*/
    m_gvInfo.nRepeatCnt	= GetProfileInt("Settings", "Smoothing Repeat Count", 3);

	/*-------------------------------------------------------------------------
	 * 에러보정
	 *-----------------------------------------------------------------------*/
    m_gvInfo.nSmoothErrCor = GetProfileInt("Settings", "Smoothing Error Correct", 90);
}

void CGMapViewerApp::SaveConfig()
{
	CString csValue;

    WriteProfileString("Settings", "Ch1 Basefile Path", m_gvInfo.csBasePath[0]);
    WriteProfileString("Settings", "Ch2 Basefile Path", m_gvInfo.csBasePath[1]);
    WriteProfileString("Settings", "Ch3 Basefile Path", m_gvInfo.csBasePath[2]);
    WriteProfileString("Settings", "Ch4 Basefile Path", m_gvInfo.csBasePath[3]);

    WriteProfileInt("Settings", "Track Mode", m_gvInfo.nTrackMode);	
    WriteProfileInt("Settings", "Cursor1 Visable", m_gvInfo.bCursor1Visible?1:0);
    WriteProfileInt("Settings", "Cursor2 Visable", m_gvInfo.bCursor2Visible?1:0);
    WriteProfileInt("Settings", "Measure Graph Visable", m_gvInfo.bMeasureGraphVisible?1:0);
    WriteProfileInt("Settings", "Filtered Graph Visable", m_gvInfo.bFilterGraphVisible?1:0);
    WriteProfileInt("Settings", "Base Graph Visable", m_gvInfo.bBaseGraphVisible?1:0);
    WriteProfileInt("Settings", "Filtered Base Graph Visable", m_gvInfo.bBaseFilterGraphVisible?1:0);
    WriteProfileInt("Settings", "Difference Graph Visable", m_gvInfo.bDiffGraphVisible?1:0);

    WriteProfileInt("Settings", "Zoom Hold", m_gvInfo.bHoldZoom?1:0);
	WriteProfileInt("Settings", "Find Peak", m_gvInfo.bFindPeak?1:0);
	int nEventMode=0;
	switch(m_gvInfo.uEventMode)
	{
	case EVENT_OPEN:
		nEventMode = 0;
		break;
	case EVENT_SHORT:
		nEventMode = 1;
	    break;
	default:
		nEventMode = 4;
	}
	WriteProfileInt("Settings", "Event Mode", nEventMode);

	csValue.Format("%f", m_gvInfo.dMinX);
    WriteProfileString("Settings", "Zoom MinX", csValue);
	csValue.Format("%f", m_gvInfo.dMaxX);
    WriteProfileString("Settings", "Zoom MaxX", csValue);
	csValue.Format("%f", m_gvInfo.dMinY);
    WriteProfileString("Settings", "Zoom MinY", csValue);
	csValue.Format("%f", m_gvInfo.dMaxY);
    WriteProfileString("Settings", "Zoom MaxY", csValue);

    WriteProfileInt("Settings", "Use Basedata", m_gvInfo.bUseBase?1:0);

    WriteProfileInt("Settings", "ScanPos 5", m_gvInfo.nScanPos5);
    WriteProfileInt("Settings", "ScanPos 15", m_gvInfo.nScanPos15);
    WriteProfileInt("Settings", "ScanPos 45", m_gvInfo.nScanPos45);
    WriteProfileInt("Settings", "ScanPos 100", m_gvInfo.nScanPos100);
    WriteProfileInt("Settings", "ScanPos 200", m_gvInfo.nScanPos200);
    WriteProfileInt("Settings", "ScanPos 500", m_gvInfo.nScanPos500);
    WriteProfileInt("Settings", "ScanPos 1000", m_gvInfo.nScanPos1000);
    WriteProfileInt("Settings", "ScanPos 2000", m_gvInfo.nScanPos2000);
    WriteProfileInt("Settings", "ScanPos 5000", m_gvInfo.nScanPos5000);
    WriteProfileInt("Settings", "ScanPos 10000", m_gvInfo.nScanPos10000);

	WriteProfileString("Settings", "Last Access Directory", m_gvInfo.csLastAccessDir);

	/*-------------------------------------------------------------------------
	 *	Smoothing 적용횟수
	 *-----------------------------------------------------------------------*/
    WriteProfileInt("Settings", "Smoothing Repeat Count", m_gvInfo.nRepeatCnt);

	/*-------------------------------------------------------------------------
	 * 에러보정
	 *-----------------------------------------------------------------------*/
    WriteProfileInt("Settings", "Smoothing Error Correct", m_gvInfo.nSmoothErrCor);	
}

BOOL CGMapViewerApp::GetFilename(CString csPathname, char* spFolderName, char* spFilename)
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
		
		int nPos = csPathname.Find(csFilename);
		strcpy(spFolderName, csPathname.Left(nPos-1));
		
		return TRUE;
	}
	return FALSE;
}

BOOL CGMapViewerApp::GetResult(CString csFilename, NANOWAVE_EX* pNanowaveEx)
{
	CFile File=NULL;
	UINT nReadCnt = 0;
	UINT nRead;
	int nFileSize=0;
	
	if(!File.Open(csFilename, CFile::modeRead
								| CFile::shareExclusive
								| CFile::typeBinary)) 
	{
		TRACE ("Can not open source file: %s\n", csFilename);
		return FALSE;
	}
	
	File.SeekToEnd();
	
	nFileSize = File.GetPosition();
	File.Seek(nFileSize - sizeof(NANOWAVE_EX), CFile::begin);
	/*---------------------------------------------------------------------
	 * WavedataEx 자료 읽기
	 *-------------------------------------------------------------------*/
	nRead = File.Read((void*)pNanowaveEx, sizeof(NANOWAVE_EX));

	File.Close();

	return TRUE;
}

int CGMapViewerApp::ExitInstance() 
{
	SaveConfig();
	
	return CWinApp::ExitInstance();
}
