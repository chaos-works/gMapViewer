// gMapViewerDlg.cpp : implementation file
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
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGMapViewerDlg dialog

CGMapViewerDlg::CGMapViewerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGMapViewerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGMapViewerDlg)
	m_strUrl = _T("");
	m_strOutput = _T("");
	m_dDistant = 0.0;
	m_strWaveFilePath = _T("");
	m_bBaseFilterView = FALSE;
	m_bDiffView = FALSE;
	m_bMeasureView = FALSE;
	m_bMeasureFilterView = FALSE;
	m_bBaseView = FALSE;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

    m_strUrl = _T("http://www.wacon.co.kr/map/view.php?tid=1&cno=0&dist=834.0");
}

void CGMapViewerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGMapViewerDlg)
	DDX_Control(pDX, IDC_BUTTON_REDRAW, m_cbRedraw);
	DDX_Control(pDX, IDC_BUTTON_CONFIG_GRAPH, m_cbConfig);
	DDX_Control(pDX, IDC_BUTTON_OPEN, m_cbOpenWaveFile);
	DDX_Control(pDX, IDC_COMBO_TID, m_cmbTdrID);
	DDX_Control(pDX, IDC_COMBO_CID, m_cmbChannelId);
	DDX_Text(pDX, IDC_EDIT_URL, m_strUrl);
	DDX_Text(pDX, IDC_STATIC_OUTPUT, m_strOutput);
	DDX_Text(pDX, IDC_EDIT_LENGTH, m_dDistant);
	DDX_Control(pDX, IDC_NTGRAPHCTRL1, m_viewGraph);
	DDX_Control(pDX, IDC_EXPLORER2, m_viewRoad);
	DDX_Control(pDX, IDC_EXPLORER1, m_viewMap);
	DDX_Text(pDX, IDC_EDIT_WAVEFILE, m_strWaveFilePath);
	DDX_Check(pDX, IDC_CHECK_BF, m_bBaseFilterView);
	DDX_Check(pDX, IDC_CHECK_D, m_bDiffView);
	DDX_Check(pDX, IDC_CHECK_M, m_bMeasureView);
	DDX_Check(pDX, IDC_CHECK_MF, m_bMeasureFilterView);
	DDX_Check(pDX, IDC_CHECK_B, m_bBaseView);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CGMapViewerDlg, CDialog)
	//{{AFX_MSG_MAP(CGMapViewerDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON2, OnButton2)
	ON_BN_CLICKED(IDC_BUTTON_VIEWPOINT, OnButtonViewpoint)
	ON_CBN_SELCHANGE(IDC_COMBO_CID, OnSelchangeComboCid)
	ON_CBN_SELCHANGE(IDC_COMBO_TID, OnSelchangeComboTid)
	ON_EN_CHANGE(IDC_EDIT_LENGTH, OnChangeEditLength)
	ON_EN_UPDATE(IDC_EDIT_LENGTH, OnUpdateEditLength)
	ON_BN_CLICKED(IDC_BUTTON_ROADVIEW, OnButtonRoadview)
	ON_BN_CLICKED(IDC_BUTTON_WAVEVIEW, OnButtonWaveview)
	ON_BN_CLICKED(IDC_BUTTON_OPEN, OnButtonWaveFileOpen)
	ON_BN_CLICKED(IDC_RADIO_TRACK, OnRadioTrack)
	ON_BN_CLICKED(IDC_RADIO_ZOOM, OnRadioZoom)
	ON_BN_CLICKED(IDC_RADIO_PAN, OnRadioPan)
	ON_BN_CLICKED(IDC_RADIO_CURSOR, OnRadioCursor)
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_CHECK_B, OnCheckB)
	ON_BN_CLICKED(IDC_CHECK_BF, OnCheckBf)
	ON_BN_CLICKED(IDC_CHECK_M, OnCheckM)
	ON_BN_CLICKED(IDC_CHECK_MF, OnCheckMf)
	ON_BN_CLICKED(IDC_CHECK_D, OnCheckD)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_REDRAW, OnButtonRedraw)
	ON_BN_CLICKED(IDC_RADIO_ENONE, OnRadioEnone)
	ON_BN_CLICKED(IDC_RADIO_ESHORT, OnRadioEshort)
	ON_BN_CLICKED(IDC_RADIO_EOPEN, OnRadioEopen)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGMapViewerDlg message handlers

BOOL CGMapViewerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	m_nTID = 1; // TDR ID 
	m_nCID = 0; // Channel ID
	mysql_init(&m_mySQL);

	// TODO: Add extra initialization here
	DisplayVersion();
	InitControl();

	InitMySQL();
	InitGraph();
	OnButtonWaveview();
	OnRadioTrack();
	
	OnButton2();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

BOOL CGMapViewerDlg::InitMySQL()
{
	BOOL bReturn=FALSE;

	if(mysql_real_connect( &m_mySQL, "121.254.168.78", "wacon", "wacon3737", "dbwacon",0, NULL, 0))
	{
		bReturn = TRUE;
		TRACE("Connected to MYSQL\n");
	}
	else {
		bReturn = FALSE;
		TRACE("Failed to connect to mysql : Error:%s\n", mysql_error(&m_mySQL));
	}
	return bReturn;
}

void CGMapViewerDlg::InitGraph()
{
	m_Graph.m_pGraph = new CGraphData;

	m_Graph.measure.pWaveData = NULL;
	m_Graph.base.pWaveData = NULL;

	BOOL bRet = m_Graph.m_pGraph->LoadCoefficient(theApp.m_csCoefPath);	// 보정계수를 읽어옴.

	theApp.m_gvInfo.nCurFile = 0;		// 현재화일 인덱스
	theApp.m_gvInfo.nFileCnt = 0;		// 화일수
	theApp.m_gvInfo.nGraphSel = 0;

	memset((void*)&m_AppInfo, 0x00, sizeof(APP_INFO));
}

void CGMapViewerDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	DelMySQL();
	
	if(m_Graph.m_pGraph) {
		delete m_Graph.m_pGraph;
		m_Graph.m_pGraph = NULL;
	}
	theApp.m_gvInfo.saWaveFilelist.RemoveAll();		
}	

void CGMapViewerDlg::DelMySQL()
{
	mysql_close(&m_mySQL);
}


void CGMapViewerDlg::DisplayVersion(void){
	DWORD dwMajor=0, dwMinor=0, dwBuild=0;
	CString strVersion, strIEVersion;
	
	HRESULT hr = GetLibraryVersion(&dwMajor, &dwMinor, &dwBuild);
	if(!hr) {
		strVersion.Format("%d.%d.%d", dwMajor, dwMinor, dwBuild);
	}

	switch(dwMajor) {
	case 4:
		switch(dwMinor) {
		case 40:
			switch(dwBuild) {
			case 308:
				strIEVersion.Format("1.0(Plus!)");
				break;
			case 520:
				strIEVersion.Format("2.0");
				break;
			}
			break;
		case 70:
			switch(dwBuild) {
			case 1155:
				strIEVersion.Format("3.0");
				break;
			case 1158:
				strIEVersion.Format("3.0(SR2)");
				break;
			case 1215:
				strIEVersion.Format("3.01");
				break;
			case 1300:
				strIEVersion.Format("3.02");
				break;
			}
			break;
		case 71:
			switch(dwBuild) {
			case 1712:
				strIEVersion.Format("4.0");
				break;
			}
			break;
		case 72:
			switch(dwBuild) {
			case 2106:
				strIEVersion.Format("4.01");
				break;
			case 3110:
				strIEVersion.Format("4.01 SP1");
				break;
			}
			break;
		}
		break;
	case 5:
		switch(dwMinor) {
		case 0:
			strIEVersion.Format("5");
			break;
		case 50:
			strIEVersion.Format("5.5");
			break;
		}
		break;
	case 6:
		switch(dwMinor) {
		case 0:
			switch(dwBuild) {
			case 2600:
				strIEVersion.Format("6.0");
				break;
			case 2900:
				strIEVersion.Format("7 (Windows XP)");
				break;
			case 6000:
				strIEVersion.Format("7 (Windows Vista)");
				break;
			}
			break;
		}
		break;
	}
	TRACE("IE Version : %s(%s)\n", strIEVersion, strVersion);
	GetDlgItem(IDC_STATIC_VERSION)->SetWindowText(strIEVersion);
}

void CGMapViewerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CGMapViewerDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CGMapViewerDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CGMapViewerDlg::OnOK() 
{
	CDialog::OnOK();
}

BEGIN_EVENTSINK_MAP(CGMapViewerDlg, CDialog)
    //{{AFX_EVENTSINK_MAP(CGMapViewerDlg)
	ON_EVENT(CGMapViewerDlg, IDC_EXPLORER1, 259 /* DocumentComplete */, OnDocumentCompleteExplorer1, VTS_DISPATCH VTS_PVARIANT)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CGMapViewerDlg::OnDocumentCompleteExplorer1(LPDISPATCH pDisp, VARIANT FAR* URL) 
{
	// TODO: Add your control notification handler code here
	m_strOutput = "Document Completed.";
	UpdateData(FALSE);
}

HRESULT GetLibraryVersion(LPDWORD pdwMajor, LPDWORD pdwMinor, LPDWORD pdwBuild)
{
    if(IsBadWritePtr(pdwMajor, sizeof(DWORD)) ||
		IsBadWritePtr(pdwMinor, sizeof(DWORD)) ||
		IsBadWritePtr(pdwBuild, sizeof(DWORD)))
		return E_POINTER;
	
    HRESULT hr = S_OK;
    
    *pdwMajor = 0;
    *pdwMinor = 0;
    *pdwBuild = 0;
	
    // Load the DLL.
    HINSTANCE hBrowser = LoadLibrary(TEXT("shdocvw.dll"));
    
    if(hBrowser)
    {
        DLLGETVERSIONPROC pDllGetVersion;
		
        // You must get this function explicitly.
        pDllGetVersion = (DLLGETVERSIONPROC)GetProcAddress(hBrowser, TEXT("DllGetVersion"));
		
        if(pDllGetVersion)
        {
            DLLVERSIONINFO dvi;
            ZeroMemory(&dvi, sizeof(dvi));
            dvi.cbSize = sizeof(dvi);
            
            hr = (*pDllGetVersion)(&dvi);
			
            if(SUCCEEDED(hr))
            {
                *pdwMajor = dvi.dwMajorVersion;
                *pdwMinor = dvi.dwMinorVersion;
                *pdwBuild = dvi.dwBuildNumber;
            }
        }
        else
        {
            // If GetProcAddress failed, there is a problem with the DLL.
            hr = E_FAIL;
        }
		
        FreeLibrary(hBrowser);
    }
    else
    {
		// Error loading module -- fail as securely as possible
		hr = E_FAIL;
    }
    
    return hr;
}

void CGMapViewerDlg::OnButtonViewpoint() 
{
	UpdateData(TRUE);

	CString strTdrID;
	CString strChannel;
	double dDistant = m_dDistant;
}

void CGMapViewerDlg::InitControl()
{
	m_cmbTdrID.Clear();
	m_cmbTdrID.AddString("#0:소재공장");
	m_cmbTdrID.AddString("#1:버스공장");
	m_cmbTdrID.SetCurSel(m_nTID);


	m_cmbChannelId.Clear();
	m_cmbChannelId.AddString("#0:채널1");
	m_cmbChannelId.AddString("#1:채널2");
	m_cmbChannelId.AddString("#2:채널3");
	m_cmbChannelId.SetCurSel(m_nCID);

	m_viewGraph.SetFrameStyle(1);
	m_viewGraph.SetControlFrameColor(RGB(192,192,192));
	m_viewGraph.SetPlotAreaColor(RGB(0,0,0));
	m_viewGraph.SetXLabel("Distance(m)");
	m_viewGraph.SetYLabel("Voltage(mV)");
	m_viewGraph.SetCaption("");

	// Button
	m_cbRedraw.SetIcon(IDI_ICON_REFRESH);
	m_cbConfig.SetIcon(IDI_ICON_CONFIG);
	m_cbOpenWaveFile.SetIcon(IDI_ICON_OPEN);

	switch(theApp.m_gvInfo.nTrackMode) {
	case TRACK:
		((CButton*)GetDlgItem(IDC_RADIO_TRACK))->SetCheck(TRUE);
		((CButton*)GetDlgItem(IDC_RADIO_ZOOM))->SetCheck(FALSE);
		((CButton*)GetDlgItem(IDC_RADIO_PAN))->SetCheck(FALSE);
		((CButton*)GetDlgItem(IDC_RADIO_CURSOR))->SetCheck(FALSE);
		break;
	case ZOOM:
		((CButton*)GetDlgItem(IDC_RADIO_TRACK))->SetCheck(FALSE);
		((CButton*)GetDlgItem(IDC_RADIO_ZOOM))->SetCheck(TRUE);
		((CButton*)GetDlgItem(IDC_RADIO_PAN))->SetCheck(FALSE);
		((CButton*)GetDlgItem(IDC_RADIO_CURSOR))->SetCheck(FALSE);
		break;
	case PANXY:
		((CButton*)GetDlgItem(IDC_RADIO_TRACK))->SetCheck(FALSE);
		((CButton*)GetDlgItem(IDC_RADIO_ZOOM))->SetCheck(FALSE);
		((CButton*)GetDlgItem(IDC_RADIO_PAN))->SetCheck(TRUE);
		((CButton*)GetDlgItem(IDC_RADIO_CURSOR))->SetCheck(FALSE);
		break;
	case CURSOR:
		((CButton*)GetDlgItem(IDC_RADIO_TRACK))->SetCheck(FALSE);
		((CButton*)GetDlgItem(IDC_RADIO_ZOOM))->SetCheck(FALSE);
		((CButton*)GetDlgItem(IDC_RADIO_PAN))->SetCheck(FALSE);
		((CButton*)GetDlgItem(IDC_RADIO_CURSOR))->SetCheck(TRUE);
		break;
	}

	// 저장된 설정 세팅	
	((CButton*)GetDlgItem(IDC_CHECK_B))->SetCheck(theApp.m_gvInfo.bBaseGraphVisible);
	((CButton*)GetDlgItem(IDC_CHECK_BF))->SetCheck(theApp.m_gvInfo.bBaseFilterGraphVisible);
	((CButton*)GetDlgItem(IDC_CHECK_M))->SetCheck(theApp.m_gvInfo.bMeasureGraphVisible);
	((CButton*)GetDlgItem(IDC_CHECK_MF))->SetCheck(theApp.m_gvInfo.bFilterGraphVisible);
	((CButton*)GetDlgItem(IDC_CHECK_D))->SetCheck(theApp.m_gvInfo.bDiffGraphVisible);

	GetDlgItem(IDC_CHECK_M)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHECK_MF)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHECK_B)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHECK_BF)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHECK_D)->EnableWindow(FALSE);

	// 이벤트
	((CButton*)GetDlgItem(IDC_RADIO_ENONE))->SetCheck(TRUE);
	((CButton*)GetDlgItem(IDC_RADIO_ESHORT))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_RADIO_EOPEN))->SetCheck(FALSE);

	UpdateData(TRUE);
}

void CGMapViewerDlg::ApplyConfig()
{
	/*-------------------------------------------------------------------------
	 *	Graph Visible
	 *-----------------------------------------------------------------------*/
	if(!m_Graph.m_pGraph->IsEmpty()) {
		// Measure Graph
		m_viewGraph.SetElement(0);
		m_viewGraph.SetElementShow(theApp.m_gvInfo.bMeasureGraphVisible);
		GetDlgItem(IDC_CHECK_M)->EnableWindow(theApp.m_gvInfo.bMeasureGraphVisible);

		// Filter Graph
		m_viewGraph.SetElement(1);
		m_viewGraph.SetElementShow(theApp.m_gvInfo.bFilterGraphVisible);
		GetDlgItem(IDC_CHECK_MF)->EnableWindow(theApp.m_gvInfo.bFilterGraphVisible);

		// BaseGraph RAW
		m_viewGraph.SetElement(2);
		m_viewGraph.SetElementShow(theApp.m_gvInfo.bBaseGraphVisible);
		GetDlgItem(IDC_CHECK_B)->EnableWindow(theApp.m_gvInfo.bBaseGraphVisible);
		
		// BaseGraph Filtered
		m_viewGraph.SetElement(3);
		m_viewGraph.SetElementShow(theApp.m_gvInfo.bBaseFilterGraphVisible);
		GetDlgItem(IDC_CHECK_BF)->EnableWindow(theApp.m_gvInfo.bBaseFilterGraphVisible);
	}

	// Difference Graph
	if(m_Graph.m_pGraph->m_bUseDiff) {
		if(!m_Graph.m_pGraph->IsEmpty()) {
			m_viewGraph.SetElement(4);
			m_viewGraph.SetElementShow(theApp.m_gvInfo.bDiffGraphVisible);
		}
	}

	/*-------------------------------------------------------------------------
	 *	Cursor Visible
	 *-----------------------------------------------------------------------*/
	// Cursor 1
	m_viewGraph.SetCursor(0);
	m_viewGraph.SetCursorVisible(theApp.m_gvInfo.bCursor1Visible);
	m_viewGraph.SetAnnotation(0);
	m_viewGraph.SetAnnoVisible(theApp.m_gvInfo.bCursor1Visible);
	
	// Cursor 2
	m_viewGraph.SetCursor(1);
	m_viewGraph.SetCursorVisible(TRUE);
	m_viewGraph.SetAnnotation(1);
	m_viewGraph.SetAnnoVisible(TRUE);

	// Cursor 3
	m_viewGraph.SetCursor(3);
	m_viewGraph.SetCursorVisible(TRUE);

	// Cursor 4
	m_viewGraph.SetCursor(4);
	m_viewGraph.SetCursorVisible(TRUE);

	/*-------------------------------------------------------------------------
	 *	Track Mode
	 *-----------------------------------------------------------------------*/
	m_viewGraph.SetTrackMode(theApp.m_gvInfo.nTrackMode);
}

void CGMapViewerDlg::OnSelchangeComboCid() 
{
	UpdateData(TRUE);

	m_nCID = m_cmbChannelId.GetCurSel();
	m_strUrl.Format("http://www.wacon.co.kr/map/view.php?tid=%d&cid=%d&dist=%.1f", m_nTID, m_nCID, m_dDistant);
	TRACE("Selected Channel : 채널%d\n", m_nCID+1);
	UpdateData(FALSE);
}

void CGMapViewerDlg::OnSelchangeComboTid() 
{
	UpdateData(TRUE);
	m_nTID = m_cmbTdrID.GetCurSel();
	switch(m_nTID) {
	case 0:
		m_strTID = "소재공장";
		break;
	case 1:
		m_strTID = "버스공장";
		break;
	}
	m_strUrl.Format("http://www.wacon.co.kr/map/view.php?tid=%d&cid=%d&dist=%.1f", m_nTID, m_nCID, m_dDistant);
	UpdateData(FALSE);
}

void CGMapViewerDlg::OnButton2() 
{
	CString strRoadViewUrl;
	// 맵 뷰
	m_strUrl.Format("http://www.wacon.co.kr/map/view.php?tid=%d&cid=%d&dist=%.1f", m_nTID, m_nCID, m_dDistant);
    UpdateData(TRUE);
    m_viewMap.Navigate(m_strUrl, NULL, NULL, NULL, NULL);

	// 로드뷰
	strRoadViewUrl.Format("http://www.wacon.co.kr/map/road_view.php?tid=%d&cid=%d&dist=%.1f", m_nTID, m_nCID, m_dDistant);
    UpdateData(TRUE);
    m_viewRoad.Navigate(strRoadViewUrl, NULL, NULL, NULL, NULL);
}

void CGMapViewerDlg::OnChangeEditLength() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	m_strUrl.Format("http://www.wacon.co.kr/map/view.php?tid=%d&cid=%d&dist=%.1f", m_nTID, m_nCID, m_dDistant);
	UpdateData(TRUE);
}

void CGMapViewerDlg::OnUpdateEditLength() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.
	
	m_strUrl.Format("http://www.wacon.co.kr/map/view.php?tid=%d&cid=%d&dist=%.1f", m_nTID, m_nCID, m_dDistant);
	UpdateData(TRUE);	
}

// 로드뷰 보기
void CGMapViewerDlg::OnButtonRoadview() 
{
	CRect rcMap, rcRoad, rcGraph;

	m_AppInfo.vtSelectedView = VT_ROADVIEW;

	m_viewMap.GetWindowRect(&rcMap);
	ScreenToClient(rcMap);

	m_viewRoad.GetWindowRect(&rcRoad);
	ScreenToClient(rcRoad);

	// 로드뷰 크게
	rcRoad.right = rcMap.right - (VIEW_MIN_SIZE + VIEW_GAP_SIZE);
	m_viewRoad.MoveWindow(rcRoad);

	m_viewGraph.GetWindowRect(&rcGraph);
	ScreenToClient(rcGraph);

	// 그래프뷰 작게
	rcGraph.left = rcGraph.right - VIEW_MIN_SIZE;
	m_viewGraph.MoveWindow(rcGraph);
}

// WaveAnalyzer 보기
void CGMapViewerDlg::OnButtonWaveview() 
{
	CRect rcMap, rcRoad, rcGraph;
	
	m_AppInfo.vtSelectedView = VT_WAVEVIEW;

	m_viewMap.GetWindowRect(&rcMap);
	ScreenToClient(rcMap);
	
	m_viewRoad.GetWindowRect(&rcRoad);
	ScreenToClient(rcRoad);
	
	// 로드뷰 작게
	rcRoad.right = rcRoad.left + VIEW_MIN_SIZE;
	m_viewRoad.MoveWindow(rcRoad);
	
	m_viewGraph.GetWindowRect(&rcGraph);
	ScreenToClient(rcGraph);
	
	// 그래프뷰 크게
	rcGraph.left = rcRoad.right + VIEW_GAP_SIZE;
	m_viewGraph.MoveWindow(rcGraph);
}

void CGMapViewerDlg::OnButtonWaveFileOpen() 
{
	CString csPathname;
	int		nListLength	= 0;
    char	sDirName[MAX_PATH] = {0,};
    CString myExtensions( 
        // Descriptive name of file type            Extensions to use
        "Nanowave files (*.bin)"				"\1"			"*.bin"			"\1"
        "Nanowave files (*.dat)"				"\1"			"*.dat"			"\1"
		);
	
	strcpy(sDirName, theApp.m_gvInfo.csLastAccessDir);
	
    nListLength = myExtensions.GetLength() + 1;  // include terminating null in the count
    myExtensions.Replace( '\1', '\0' );				// embed nulls into the string
	
    CFileDialog_ReadOnly fileDialog( 
		TRUE,		    // TRUE = create a File Open dialog box
		this->m_hWnd, 	// handle to the owner window (NULL = no owner; non modal)
		sDirName, 	    // initial directory to open in
		"*.bin",		// initial filename to use
		myExtensions,	// list of allowed file extensions
		nListLength	    // total size of buffer used to hold file extension list
		);
	
	if(fileDialog.DoModal() == IDOK) {
		// 최근에 엑세스한 폴드 저장
		theApp.m_gvInfo.csLastAccessDir =  fileDialog.GetDirName();
		theApp.SaveConfig();
		
		LoadWavefileList(fileDialog.GetDirName());
		
		csPathname = fileDialog.GetPathName();
		OpenWaveFile(csPathname);
		m_strWaveFilePath = csPathname;
		UpdateData(FALSE);
		
		// File Index display
//		int nIndex = FileIndex(csPathname);
//		if(nIndex >= 0) {
//			m_MenuBar.m_csCurFilePos.Format("%d/%d", nIndex+1, m_nFileCnt);
//			m_MenuBar.UpdateData(FALSE);
//		}
	}	
}

int	CGMapViewerDlg::LoadWavefileList(CString csFoldname)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE	hFind;

	CString csFilename;
	CString csWaveFile;
	CString csAllWaveFiles;
	DWORD	dwError;

//	m_MenuBar.m_csCurFilePos.Format("0/0");
//	m_MenuBar.UpdateData(FALSE);

	m_WaveFilelist.RemoveAll();
//	m_listFile.DeleteAllItems();	// 화일리스트 삭제

	/*-------------------------------------------------------------------------
	 * DAT 화일 리스트 생성	
	 *-----------------------------------------------------------------------*/
	csAllWaveFiles.Format("%s\\*.dat", csFoldname);
	hFind = FindFirstFile(csAllWaveFiles, &FindFileData);
	if(hFind == INVALID_HANDLE_VALUE) {
//		TRACE("해당화일이 없음.\n");
	}
	else {
		// 첫번째 화일
		csWaveFile.Format("%s\\%s", csFoldname, FindFileData.cFileName);
		// Add wave file
		m_WaveFilelist.Add(csWaveFile);

		// 2 ~ n 번째 화일
		while(FindNextFile(hFind, &FindFileData) != 0) {
			csWaveFile.Format("%s\\%s", csFoldname, FindFileData.cFileName);
			// Add wave file
			m_WaveFilelist.Add(csWaveFile);
		}
		dwError = GetLastError();
		FindClose(hFind);
		if(dwError != ERROR_NO_MORE_FILES) {
			TRACE("FindNextFile error. Error Code : %u\n", dwError);
		}
	}

	csAllWaveFiles.Format("%s\\*.bin", csFoldname);
	/*-------------------------------------------------------------------------
	 * BIN 화일 리스트 생성	
	 *-----------------------------------------------------------------------*/
	hFind = FindFirstFile(csAllWaveFiles, &FindFileData);
	if(hFind == INVALID_HANDLE_VALUE) {
		TRACE("해당화일이 없음.\n");
	}
	else {
		// 첫번째 화일
		csWaveFile.Format("%s\\%s", csFoldname, FindFileData.cFileName);
		// Add wave file
		m_WaveFilelist.Add(csWaveFile);

		// 2 ~ n 번째 화일
		while(FindNextFile(hFind, &FindFileData) != 0) {
			csWaveFile.Format("%s\\%s", csFoldname, FindFileData.cFileName);
			// Add wave file
			m_WaveFilelist.Add(csWaveFile);
		}
		dwError = GetLastError();
		FindClose(hFind);
		if(dwError != ERROR_NO_MORE_FILES) {
			TRACE("FindNextFile error. Error Code : %u\n", dwError);
		}
	}


	// 화일의 수, 현재화일 인덱스 및 현재폴더 저장
	theApp.m_gvInfo.nCurFile	= 0;
	theApp.m_gvInfo.nFileCnt	= m_WaveFilelist.GetSize();
	
	// Menu bar에 화일수와 현재화일의 인덱스를 표시
//	m_MenuBar.m_csCurFilePos.Format("%d/%d", m_nCurFile+1, m_nFileCnt);
//	m_MenuBar.UpdateData(FALSE);

	// Add filelist
//	InsertFileList();

	return theApp.m_gvInfo.nFileCnt;
}

int CGMapViewerDlg::GetChannelNo(CString csPathname)
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

BOOL CGMapViewerDlg::OpenWaveFile(CString csWaveFile)
{
	double dResValue = 0.0;
	int		nChannelNo = 0;
	CString csMessage;
	EVENTSCAN_INFO infoEventScan;
	
	// Channel No
	theApp.m_gvInfo.nCurChannel = GetChannelNo(csWaveFile);
	// WAVE Filename
	m_Graph.measure.strPath = csWaveFile;
	
	if(m_Graph.m_pGraph) {
		if(!m_Graph.m_pGraph->IsEmpty()) {
			delete m_Graph.m_pGraph;
			m_Graph.m_pGraph = new CGraphData;
			m_Graph.m_pGraph->LoadCoefficient(theApp.m_csCoefPath);
		}
		m_Graph.m_pGraph->SetChannel(theApp.m_gvInfo.nCurChannel);
		m_Graph.m_pGraph->SetRepeatCount(theApp.m_gvInfo.nRepeatCnt);
		
		// 베이스 화일 패스
		if(theApp.m_gvInfo.bUseBase) {
			if(!m_Graph.m_pGraph->SetBasePath(theApp.m_gvInfo.csBasePath[theApp.m_gvInfo.nCurChannel])) {
				csMessage.Format("%s \n\r손상된 웨이브 화일입니다.", theApp.m_gvInfo.csBasePath[theApp.m_gvInfo.nCurChannel]);
				AfxMessageBox( csMessage, MB_OK|MB_ICONSTOP );
				return FALSE;
			}
		}
		TRACE("a.베이스 화일 : %s\n", theApp.m_gvInfo.csBasePath[theApp.m_gvInfo.nCurChannel]);
		
		// 측정 화일 패스
		if(!m_Graph.m_pGraph->SetDataPath(m_Graph.measure.strPath)) {
			csMessage.Format("%s \n\r손상된 웨이브 화일입니다.", m_Graph.measure.strPath);
			AfxMessageBox( csMessage, MB_OK|MB_ICONSTOP );
			return FALSE;
		}
		TRACE ("b. 이벤트 화일 : %s\n", m_Graph.measure.strPath);
		
		NANOWAVE_EX NanowaveEx;
		memset((void*)&NanowaveEx, 0x00, sizeof(NANOWAVE_EX));
		memset((void*)&infoEventScan, 0x00, sizeof(EVENTSCAN_INFO));
		
		if(theApp.GetResult(m_Graph.measure.strPath, (NANOWAVE_EX*)&NanowaveEx)) {
			if(NanowaveEx.wHead == 34049) {
				theApp.m_gvInfo.dResValue = ((LONG)(NanowaveEx.dResValue*100))/100.;
				infoEventScan.dResValue = theApp.m_gvInfo.dResValue;
				switch(NanowaveEx.wFlag) {
				case 0:
					m_uFindEventType = EVENT_NONE;
					TRACE("정상\n");
					break;
				case 1:
					m_uFindEventType = EVENT_SHORT;
					TRACE("누수\n");
					break;
				default:
					m_uFindEventType = EVENT_OPEN;
					TRACE("파손\n");
				}
			}
		}
		
		UpdateData(FALSE);
		if(m_Graph.m_pGraph->CalcDistance(m_uFindEventType, infoEventScan)) {
			double dMeasureDistance = m_Graph.m_pGraph->GetDistance();
			
			if(dMeasureDistance > 0.0) {
				// 측정거리
				TRACE("====================================================");
				TRACE (">>>>>> 채널%1d, 저항(%.1f), 보정거리 = %.1fm\n", theApp.m_gvInfo.nCurChannel+1, theApp.m_gvInfo.dResValue, dMeasureDistance);
				TRACE("====================================================");
			}
		}
		DisplayGraph();
		DisplayResult();	// 결과를 표시
		ApplyConfig();
	}
	
	return TRUE;
}

void CGMapViewerDlg::OnRadioEnone() 
{
	m_uFindEventType = EVENT_NONE;
}

void CGMapViewerDlg::OnRadioEshort() 
{
	m_uFindEventType = EVENT_SHORT;
}

void CGMapViewerDlg::OnRadioEopen() 
{
	m_uFindEventType = EVENT_OPEN;
}

int CGMapViewerDlg::FileIndex(CString csFilename)
{
	int		nIndex=0;
	CString csWaveFile;
	
	for(nIndex=0; nIndex<theApp.m_gvInfo.nFileCnt; nIndex++) {
		csWaveFile = m_WaveFilelist.GetAt(nIndex);
		if(csFilename.CompareNoCase(csWaveFile) == 0)
			break;
	}
	
	if(nIndex < theApp.m_gvInfo.nFileCnt) {
		theApp.m_gvInfo.nCurFile = nIndex;
		return nIndex;
	}
	return -1;
}

CString CGMapViewerDlg::GetMeasureFilename()
{
	CString csFilename;
	char szFoldername[MAX_PATH]={0,};
	char szFilename[MAX_PATH]={0,};
	
	if(m_Graph.measure.strPath.GetLength()) {
		theApp.GetFilename(m_Graph.measure.strPath, szFoldername, szFilename);
	}
	
	csFilename = (CString)szFilename;
	return csFilename;
}


/*-----------------------------------------------------------------------------
 * 함 수 명 : DisplayGraph()
 * 설    명 : Base, Measure 그래프를 그리고,
 *            이벤트 발생한 지점의 위치를 계산한다.
 * 작 성 자 : 김영규
 * 작성일자 : 2004.10.19
 * 변경사항 :
      1. 측정그래프만 보여줌  2004.10.19 김영규
 *---------------------------------------------------------------------------*/
void CGMapViewerDlg::DisplayGraph()
{
	CString csValue;
	double dVOP = 0.0;
	
	if(m_Graph.m_pGraph->IsEmpty()) return;

    // 채널번호의 검증	
	if(theApp.m_gvInfo.nCurChannel < 0 || theApp.m_gvInfo.nCurChannel > 3) return;

	/*-------------------------------------------------------------------------
	 *	채널별 측정데이타, 정보를 표시함.
	 *-----------------------------------------------------------------------*/
	dVOP			= m_Graph.m_pGraph->GetVOP();		// VOP(%)
	m_Graph.nTdrVOP		= (int)dVOP * 10;
	UpdateData(FALSE);
	
	m_Graph.nScaleSave	= m_Graph.m_pGraph->GetScale();		// Scale
	m_Graph.dThreshold	= m_Graph.m_pGraph->GetThreshold();	// Threshold

	// 탐색범위
	int nRangeStart	= m_Graph.m_pGraph->GetScanPos();
	int nRangeEnd	= m_Graph.m_pGraph->GetDataLength();

	csValue.Format("%d", nRangeStart);
	//GetDlgItem(IDC_STATIC_START_POS)->SetWindowText(csValue);

	// Cursor 위치를 탐색범위로 이동한다.
	theApp.m_gvInfo.nCurPos1	= nRangeStart;									// Cursor1 pos
	theApp.m_gvInfo.nCurPos2	= nRangeEnd;									// Cursor2 pos
	m_Graph.nDistance	= theApp.m_gvInfo.nCurPos2 - theApp.m_gvInfo.nCurPos1;						// Distance

	/*-------------------------------------------------------------------------
	 *	그래프를 그린다.
	 *  : 거리 환산을 위해서 Scale 과 VOP룰 필히 설정해야 함. 
	 *-----------------------------------------------------------------------*/ 
	m_viewGraph.SetScale(m_Graph.nScaleSave);
	m_viewGraph.SetVop(dVOP);
	
	DrawGraph();

	/*-------------------------------------------------------------------------
	 * 그래프 표시
	 *-----------------------------------------------------------------------*/
	// Base Graph - RAW
	m_viewGraph.SetElement(0);
	m_viewGraph.SetElementShow(theApp.m_gvInfo.bBaseGraphVisible);
	// Base Graph - Filtered
	m_viewGraph.SetElement(1);
	m_viewGraph.SetElementShow(theApp.m_gvInfo.bBaseFilterGraphVisible);

	if(m_Graph.m_pGraph->m_bBaseLoaded) {
		// Base Graph - RAW
		m_viewGraph.SetElement(2);
		m_viewGraph.SetElementShow(theApp.m_gvInfo.bBaseGraphVisible);
		// Base Graph - Filtered
		m_viewGraph.SetElement(3);
		m_viewGraph.SetElementShow(theApp.m_gvInfo.bBaseFilterGraphVisible);
		// Difference Graph
		m_viewGraph.SetElement(4);
		m_viewGraph.SetElementShow(theApp.m_gvInfo.bDiffGraphVisible);
	}
}


/*-----------------------------------------------------------------------------  
 * 함 수 명 : BOOL CWaveAnalysisDlg::DisplayResult()
 * 목    적 : 측정그래프를 분석한 결과를 표시한다.
 * 설    명 : 
 * 작 성 자 : 김영규
 * 작성일자 : 2004.10.19
 * 변경사항 :
 *---------------------------------------------------------------------------*/
BOOL CGMapViewerDlg::DisplayResult()
{
	int nPos=0;
	double dVOP = 0.0;
	CString csMessage;
	CString csValue;
	EVENTDETECT_INFO EventInfo;

	// 표시할 데이타가 없음.
	if(m_Graph.m_pGraph->IsEmpty()) return FALSE;

	/*-------------------------------------------------------------------------
	 * 측정 데이타 정보	
	 *-----------------------------------------------------------------------*/
//	csValue.Format("Gr%d", theApp.m_gvInfo.nCurChannel+1);
//	GetDlgItem(IDC_STATIC_GRAPH_NAME)->SetWindowText(csValue);				// Graph Name
//	GetDlgItem(IDC_STATIC_FILENAME)->SetWindowText(GetMeasureFilename());	// Filename

//	csValue.Format("%5.2f mV", m_Graph.m_pGraph->GetVolt());		
//	GetDlgItem(IDC_STATIC_VOLT)->SetWindowText(csValue);				// Volt

//	csValue.Format("%5.2f", m_Graph.m_pGraph->GetdBRL());			
//	GetDlgItem(IDC_STATIC_DBRL)->SetWindowText(csValue);				// dBRL

//	csValue.Format("%d", m_Graph.m_pGraph->GetScale());		
//	GetDlgItem(IDC_STATIC_SCALE)->SetWindowText(csValue);				// Scale

	/*-------------------------------------------------------------------------
	 * 이벤트 위치
	 *-----------------------------------------------------------------------*/
	memset((void*)&EventInfo, 0x00, EVENTDETECT_INFO_SIZE);

	// VOP는 사용자 설정값을 사용한다. 2005.6.14
	UpdateData(TRUE);

	m_Graph.m_pGraph->GetEventInfo(&EventInfo);
	ShowCursorInfo(1, m_Graph.m_pGraph->m_nPeakPoint);

	// Difference Avg
	switch(EventInfo.uEventMode) {
	case EVENT_OPEN:
		csValue.Format("EB %.1f/%.1f", m_Graph.m_pGraph->m_dPeakHeight, m_Graph.m_pGraph->m_dHeightThreshold);
		break;
	case EVENT_SHORT:
		csValue.Format("ES %.1f/%.1f", m_Graph.m_pGraph->m_dPeakHeight, m_Graph.m_pGraph->m_dHeightThreshold);
		break;
	default:
		csValue.Format("Nor %.1f/%.1f", m_Graph.m_pGraph->m_dPeakHeight, m_Graph.m_pGraph->m_dHeightThreshold);
	}

//etDlgItem(IDC_STATIC_EVENT)->SetWindowText(csValue);

	return TRUE;
}

void CGMapViewerDlg::ReDraw()
{
	UpdateData(TRUE);
	m_viewGraph.SetVop(m_Graph.nTdrVOP / 10.0);
	DrawGraph();		// 그래프를 그린다.
	DisplayResult();	// 결과를 표시한다.

}

void CGMapViewerDlg::DrawGraph()
{
	int		i		=0;
	int		nElementID =0;
	int		nLength = 0;
	int		nChannel =0;
	double	dValueX = 0;
	double	dValueY = 0;
	double	dDistance =0.;
	int		nDisplayGap=0;
	
	EVENTDETECT_INFO EventInfo;
	CString csMsg;
	CString csElementName;
	char szFoldername[MAX_PATH]={0,};
	char szFilename[MAX_PATH]={0,};

	if(m_Graph.m_pGraph->IsEmpty()) return;

	nChannel	= m_Graph.m_pGraph->GetChannelNo();			// 채널
	nLength		= m_Graph.m_pGraph->GetDataLength();		// 길이
	m_Graph.measure.pWaveData = m_Graph.m_pGraph->GetMeasureWaveData();	// 데이타

	if(m_Graph.m_pGraph->m_bBaseLoaded) {
		m_Graph.base.pWaveData = m_Graph.m_pGraph->m_BaseInfo.pWaveData;	
	}

	/*-------------------------------------------------------------------------
	 * 베이스 그래프와 측정그래프 간의 좌우 갭
	 *    Base : 500 px
	 * Measure : 501 px
	 *-----------------------------------------------------------------------*/
	nDisplayGap = m_Graph.m_pGraph->m_MeasureInfo.nStart - m_Graph.m_pGraph->m_BaseInfo.nStart;

	m_viewGraph.ClearGraph();
	m_viewGraph.SetFormatAxisLeft("%.1f");
	m_viewGraph.SetFormatAxisBottom("%.1f");

	/*-------------------------------------------------------------------------
	 *	Measure Graph - RAW
	 *-----------------------------------------------------------------------*/
	nElementID = 0;
	m_viewGraph.SetElement(nElementID);
	m_viewGraph.SetElementLineColor(RGB(50,205,50));
	m_viewGraph.SetElementIdentify(TRUE);
	m_viewGraph.SetElementPointSymbol(0);
	m_viewGraph.SetElementWidth(2);
	m_viewGraph.SetElementSolidPoint(TRUE);
    m_viewGraph.SetElementLinetype(6);
    
	csElementName.Format("%s(측정 원본)", GetMeasureFilename());
	m_viewGraph.SetElementName(csElementName);
	
	nLength = m_Graph.m_pGraph->GetDataLength();		// 길이

	if(theApp.m_gvInfo.bMeasureGraphVisible) {
		for (i = 0; i < nLength; i++) {
			dValueX = i;
			dValueY = m_Graph.m_pGraph->m_MeasureInfo.pRawData[i];

			if(dValueY > 0) 
				m_viewGraph.PlotXY(dValueX, dValueY, nElementID);			
		}
	}

	/*-------------------------------------------------------------------------
	 *	Measure Graph - Filterd
	 *-----------------------------------------------------------------------*/
	nElementID = 1;
	m_viewGraph.AddElement();
	m_viewGraph.SetElement(nElementID);
	m_viewGraph.SetElementLineColor(RGB(254,46,46));
	m_viewGraph.SetElementIdentify(TRUE);
	m_viewGraph.SetElementPointSymbol(0);
	m_viewGraph.SetElementWidth(2);
	m_viewGraph.SetElementSolidPoint(TRUE);
    m_viewGraph.SetElementLinetype(6);
    
	csElementName.Format("%s(측정 필터링)", GetMeasureFilename());
    m_viewGraph.SetElementName(csElementName);
	
	nLength =m_Graph.m_pGraph->m_MeasureInfo.nLength;

	if(theApp.m_gvInfo.bFilterGraphVisible) {
		
		for (i = 0; i < nLength; i++) {
			dValueX = i;
			dValueY = m_Graph.m_pGraph->m_MeasureInfo.pWaveData[i];

			if(dValueY > 0) {
				m_viewGraph.PlotXY(dValueX, dValueY, nElementID);
			}
		}
	}
	/*-------------------------------------------------------------------------
	 *	Base Graph - RAW
	 *-----------------------------------------------------------------------*/
	nElementID = 2;
	m_viewGraph.AddElement();
	if(m_Graph.m_pGraph->m_bBaseLoaded) {
		m_viewGraph.SetElement(nElementID);
		m_viewGraph.SetElementLineColor(RGB(0,0,255));
		m_viewGraph.SetElementIdentify(TRUE);
		m_viewGraph.SetElementPointSymbol(0);
		m_viewGraph.SetElementWidth(2);
		m_viewGraph.SetElementSolidPoint(TRUE);
		m_viewGraph.SetElementLinetype(6);
    
		if(nChannel >= 0) {
			theApp.GetFilename(theApp.m_gvInfo.csBasePath[nChannel], szFoldername, szFilename);
			csElementName.Format("%s(베이스 원본)", szFilename);
		}
		
		m_viewGraph.SetElementName(csElementName);
		
		nLength = m_Graph.m_pGraph->m_BaseInfo.nLength;

		if(theApp.m_gvInfo.bBaseGraphVisible) {
			for (i = 0; i < nLength; i++) {
				dValueX = i+ nDisplayGap;
				dValueY = m_Graph.m_pGraph->m_BaseInfo.pRawData[i];
				if(dValueY > 0) 
					m_viewGraph.PlotXY(dValueX, dValueY, nElementID);
			}
		}
	}
	
	/*-------------------------------------------------------------------------
	 *	Base Graph - Filtered
	 *-----------------------------------------------------------------------*/
	nElementID = 3;
	m_viewGraph.AddElement();

	if(m_Graph.m_pGraph->m_bBaseLoaded) {
		m_viewGraph.SetElement(nElementID);
		m_viewGraph.SetElementLineColor(RGB(0,64,255));
		m_viewGraph.SetElementIdentify(TRUE);
		m_viewGraph.SetElementPointSymbol(0);
		m_viewGraph.SetElementWidth(2);
		m_viewGraph.SetElementSolidPoint(TRUE);
		m_viewGraph.SetElementLinetype(6);
    
		if(nChannel >= 0) {
			theApp.GetFilename(theApp.m_gvInfo.csBasePath[nChannel], szFoldername, szFilename);
			csElementName.Format("%s(베이스 필터링)", szFilename);
		}
		
		m_viewGraph.SetElementName(csElementName);
		
		nLength = m_Graph.m_pGraph->m_BaseInfo.nLength;

		if(theApp.m_gvInfo.bBaseFilterGraphVisible) {
			for (i = 0; i < nLength; i++) {
				dValueX = i+ nDisplayGap;
				dValueY = m_Graph.m_pGraph->m_BaseInfo.pWaveData[i];

				if(dValueY > 0) 
					m_viewGraph.PlotXY(dValueX, dValueY, nElementID);
			}
		}
	}

	/*-------------------------------------------------------------------------
	 *	Difference Graph
	 *-----------------------------------------------------------------------*/
	nElementID = 4;
	m_viewGraph.AddElement();
	if(m_Graph.m_pGraph->m_bBaseLoaded && m_Graph.m_pGraph->m_bMeasureLoaded) {
		m_viewGraph.SetElement(nElementID);
		m_viewGraph.SetElementLineColor(RGB(255,215,0));
		m_viewGraph.SetElementIdentify(TRUE);
		m_viewGraph.SetElementPointSymbol(0);
		m_viewGraph.SetElementWidth(2);
		m_viewGraph.SetElementSolidPoint(TRUE);
		m_viewGraph.SetElementLinetype(6);
    
		csElementName.Format("Difference Graph");
		m_viewGraph.SetElementName(csElementName);
		nLength	= (m_Graph.m_pGraph->m_BaseInfo.nLength > m_Graph.m_pGraph->m_MeasureInfo.nLength) ? m_Graph.m_pGraph->m_MeasureInfo.nLength : m_Graph.m_pGraph->m_BaseInfo.nLength;

		// 기준값 구하기
		double dBaseValue=0;
		if(theApp.m_gvInfo.bDiffGraphVisible) {
			for (i = 0; i < nLength; i++) {
				if(m_Graph.measure.pWaveData[i] <= 0 || m_Graph.base.pWaveData[i] <= 0) break;				
				dValueX = i;
				if( 0 <= (i + nDisplayGap) && (i + nDisplayGap) < nLength) {
					dBaseValue = m_Graph.m_pGraph->m_MeasureInfo.pWaveData[i] - m_Graph.m_pGraph->m_BaseInfo.pWaveData[i+ nDisplayGap];
				}
			}
		}
		if(theApp.m_gvInfo.bDiffGraphVisible) {
			for (i = 16; i < nLength; i++) {
				if(m_Graph.measure.pWaveData[i] <= 0 || m_Graph.base.pWaveData[i] <= 0) break;
				
				dValueX = i;
				if( 0 <= (i + nDisplayGap) && (i + nDisplayGap) < nLength) {
					dValueY = m_Graph.m_pGraph->m_MeasureInfo.pWaveData[i] 
						    - m_Graph.m_pGraph->m_BaseInfo.pWaveData[i+ nDisplayGap] - dBaseValue;
					m_viewGraph.PlotXY(dValueX, dValueY, nElementID);
				}
			}
		}
	}

	/*-------------------------------------------------------------------------
	 *	단선-피크 그래프
	 *-----------------------------------------------------------------------*/
	nElementID = 5;
	m_viewGraph.AddElement();

	m_viewGraph.SetElement(nElementID);
	m_viewGraph.SetElementLineColor(RGB(255,215,0));
	m_viewGraph.SetElementIdentify(TRUE);
	m_viewGraph.SetElementPointSymbol(0);
	m_viewGraph.SetElementWidth(1);
	m_viewGraph.SetElementSolidPoint(TRUE);
	m_viewGraph.SetElementLinetype(6);
    
	csElementName.Format("단선-피크 그래프");
	m_viewGraph.SetElementName(csElementName);
	nLength	= (m_Graph.m_pGraph->m_BaseInfo.nLength > m_Graph.m_pGraph->m_MeasureInfo.nLength) ? m_Graph.m_pGraph->m_MeasureInfo.nLength : m_Graph.m_pGraph->m_BaseInfo.nLength;

	/*-------------------------------------------------------------------------
	 *	단락-피크 그래프
	 *-----------------------------------------------------------------------*/
	nElementID = 6;
	m_viewGraph.AddElement();

	m_viewGraph.SetElement(nElementID);
	m_viewGraph.SetElementLineColor(RGB(0,215,255));
	m_viewGraph.SetElementIdentify(TRUE);
	m_viewGraph.SetElementPointSymbol(0);
	m_viewGraph.SetElementWidth(1);
	m_viewGraph.SetElementSolidPoint(TRUE);
	m_viewGraph.SetElementLinetype(6);
    
	csElementName.Format("쇼트-피크 그래프");
	m_viewGraph.SetElementName(csElementName);
	nLength	= (m_Graph.m_pGraph->m_BaseInfo.nLength > m_Graph.m_pGraph->m_MeasureInfo.nLength) ? m_Graph.m_pGraph->m_MeasureInfo.nLength : m_Graph.m_pGraph->m_BaseInfo.nLength;

	memset((void*)&EventInfo, 0x00, sizeof(EVENTDETECT_INFO));
	m_Graph.m_pGraph->GetEventInfo(&EventInfo);

	/*-------------------------------------------------------------------------
	 *	Cursor & Annotation 표시
	 *-----------------------------------------------------------------------*/
	if(m_viewGraph.GetCursorCount() <= 0) {
		/*---------------------------------------------------------------------
		 * Start Cursor
		 *-------------------------------------------------------------------*/
		m_viewGraph.AddCursor();
		m_viewGraph.SetCursor(0);
		m_viewGraph.SetCursorColor(RGB(255,0,0));
		m_viewGraph.SetCursorX(theApp.m_gvInfo.nCurPos1);

		// Annotation 0
		m_viewGraph.AddAnnotation();
		m_viewGraph.SetAnnotation(0);
		m_viewGraph.SetAnnoLabelHorizontal(TRUE);
		m_viewGraph.SetAnnoLabelCaption("Cursor 1");

		/*---------------------------------------------------------------------
		 * Event Cursor	
		 *-------------------------------------------------------------------*/
		m_viewGraph.AddCursor();
		m_viewGraph.SetCursor(1);
		m_viewGraph.SetCursorColor(RGB(255,0,0));
		m_viewGraph.SetCursorX(EventInfo.nEvent);

		// Annotation 1
		m_viewGraph.AddAnnotation();
		m_viewGraph.SetAnnotation(1);
		m_viewGraph.SetAnnoLabelHorizontal(TRUE);
		m_viewGraph.SetAnnoLabelCaption("Cursor 2");

		/*---------------------------------------------------------------------
		 * 감시구간
		 *-------------------------------------------------------------------*/
		m_viewGraph.AddCursor();
		m_viewGraph.SetCursor(2);
		m_viewGraph.SetCursorColor(RGB(255,173,91));
//		m_viewGraph.SetCursorStyle(2);

		m_viewGraph.AddCursor();
		m_viewGraph.SetCursor(3);
		m_viewGraph.SetCursorColor(RGB(255,173,91));
//		m_viewGraph.SetCursorStyle(2);

		/*---------------------------------------------------------------------
		 * Peak Point
		 *-------------------------------------------------------------------*/
		m_viewGraph.AddCursor();
		m_viewGraph.SetCursor(4);
		m_viewGraph.SetCursorColor(RGB(0,0,255));
		m_viewGraph.SetCursorStyle(2);
	}

	// Track mode 설정.
	m_viewGraph.SetTrackMode(theApp.m_gvInfo.nTrackMode);

	// Grid 표시
	m_viewGraph.SetShowGrid(FALSE);

	if(theApp.m_gvInfo.bHoldZoom) {
		m_viewGraph.SetRange(theApp.m_gvInfo.dMinX, theApp.m_gvInfo.dMaxX, theApp.m_gvInfo.dMinY, theApp.m_gvInfo.dMaxY);
	}
	else {
		m_viewGraph.AutoRange();
	}
	ApplyConfig();
}

void CGMapViewerDlg::OnRadioTrack() 
{
	theApp.m_gvInfo.nTrackMode = TRACK;
	
	/*-------------------------------------------------------------------------
	 *	Apply Config
	 *-----------------------------------------------------------------------*/
	theApp.SaveConfig();
	MainWnd->ApplyConfig();	
}

void CGMapViewerDlg::OnRadioZoom() 
{
	theApp.m_gvInfo.nTrackMode = ZOOM;

	/*-------------------------------------------------------------------------
	 *	Apply Config
	 *-----------------------------------------------------------------------*/
	theApp.SaveConfig();
	MainWnd->ApplyConfig();	
}

void CGMapViewerDlg::OnRadioPan() 
{
	theApp.m_gvInfo.nTrackMode = PANXY;

	/*-------------------------------------------------------------------------
	 *	Apply Config
	 *-----------------------------------------------------------------------*/
	theApp.SaveConfig();
	MainWnd->ApplyConfig();	
}

void CGMapViewerDlg::OnRadioCursor() 
{
	theApp.m_gvInfo.nTrackMode = CURSOR;
	
	/*-------------------------------------------------------------------------
	 *	Apply Config
	 *-----------------------------------------------------------------------*/
	theApp.SaveConfig();
	MainWnd->ApplyConfig();	
}


void CGMapViewerDlg::OnButtonUp() 
{
	theApp.m_gvInfo.bZoomPressed = TRUE;
	theApp.m_gvInfo.uZoomDir = ZOOM_UP;
	
	DoZoom();
}

void CGMapViewerDlg::OnButtonDown() 
{
	theApp.m_gvInfo.bZoomPressed = TRUE;
	theApp.m_gvInfo.uZoomDir = ZOOM_DOWN;
	
	DoZoom();
}

void CGMapViewerDlg::OnButtonLeft() 
{
	theApp.m_gvInfo.bZoomPressed = TRUE;
	theApp.m_gvInfo.uZoomDir = ZOOM_LEFT;
	
	DoZoom();
}

void CGMapViewerDlg::OnButtonRight() 
{
	theApp.m_gvInfo.bZoomPressed = TRUE;
	theApp.m_gvInfo.uZoomDir = ZOOM_RIGHT;
	
	if(theApp.m_gvInfo.nTrackMode == ZOOM)	{
		DoZoom();
	}
	else if(theApp.m_gvInfo.nTrackMode == PANXY) {
		DoScroll();
	}
}


/*-----------------------------------------------------------------------------  
 * 함 수 명 : void CGMapViewerDlg::DoScroll()
 * 목    적 : 그래프 스크롤
 * 설    명 : 
 * 작 성 자 : 김영규
 * 작성일자 : 2005.03.02
 * 변경사항 :
 *---------------------------------------------------------------------------*/
void CGMapViewerDlg::DoScroll()
{
	// 그래프 줌인
	double dMinX=0.0;
	double dMaxX=0.0;
	double dMinY=0.0;
	double dMaxY=0.0;

	m_viewGraph.GetRange(&dMinX, &dMaxX, &dMinY, &dMaxY);

	double dDeltaX =  dMaxX / 100.0;
	double dDeltaY =  dMaxY / 100.0;

	switch(theApp.m_gvInfo.uZoomDir) {
	case ZOOM_LEFT:
		dMinX += dDeltaX;
		dMaxX += dDeltaX;
		break;
	case ZOOM_UP:
		dMinY -= dDeltaY;
		dMaxY -= dDeltaY;
		break;
	case ZOOM_RIGHT:
		dMinX -= dDeltaX;
		dMaxX -= dDeltaX;
		break;
	case ZOOM_DOWN:
		dMinY += dDeltaY;
		dMaxY += dDeltaY;
		break;
	}
	m_viewGraph.SetRange(dMinX, dMaxX, dMinY, dMaxY);

	if(theApp.m_gvInfo.bHoldZoom) {
		theApp.m_gvInfo.dMinX				= dMinX;
		theApp.m_gvInfo.dMaxX				= dMaxX;
		theApp.m_gvInfo.dMinY				= dMinY;
		theApp.m_gvInfo.dMaxY				= dMaxY;
	}
}

/*-----------------------------------------------------------------------------  
 * 함 수 명 : void CGMapViewerDlg::DoZoom()
 * 목    적 : 
 * 설    명 : 
 * 작 성 자 : 김영규
 * 작성일자 : 2005.03.02
 * 변경사항 :
 *---------------------------------------------------------------------------*/
void CGMapViewerDlg::DoZoom()
{
	// 그래프 줌인
	double dMinX=0.0;
	double dMaxX=0.0;
	double dMinY=0.0;
	double dMaxY=0.0;
	CString csValue;

	m_viewGraph.GetRange(&dMinX, &dMaxX, &dMinY, &dMaxY);
	
	double dRateX =  (dMaxX - dMinX) / 16.0;
	double dRateY =  (dMaxY - dMinY) / 16.0;

	switch(theApp.m_gvInfo.uZoomDir) {
	case ZOOM_LEFT:
		dMaxX += dRateX;
		break;
	case ZOOM_UP:
		if(dMaxY - dRateY > 0) {
			dMaxY -= dRateY;
		}
		break;
	case ZOOM_RIGHT:
		if(dMaxX - dRateX > 0) {
			dMaxX -= dRateX;
		}
		break;
	case ZOOM_DOWN:
		dMaxY += dRateY;
		break;
	}

//	csValue.Format("(x%.1f, x%.1f)", 1.0, 1.0);
//	GetDlgItem(IDC_STATIC_ZOOMRATE)->SetWindowText(csValue);

	m_viewGraph.SetRange(dMinX, dMaxX, dMinY, dMaxY);

	if(theApp.m_gvInfo.bHoldZoom) {
		theApp.m_gvInfo.dMinX				= dMinX;
		theApp.m_gvInfo.dMaxX				= dMaxX;
		theApp.m_gvInfo.dMinY				= dMinY;
		theApp.m_gvInfo.dMaxY				= dMaxY;
	}
}

void CGMapViewerDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// 최고점, 최저점 검색 & 설정
	if(point.x >= 10000) {
		if(m_Graph.m_pGraph->IsEmpty()) {
			return;
		}
		point.x -= 10000;

		/*---------------------------------------------------------------------
		 *	이벤트 위치 변경
		 *-------------------------------------------------------------------*/
		if(0 < point.x && point.x < m_Graph.m_pGraph->GetDataLength()) {
			UpdateData(TRUE);
			ShowCursorInfo(1, point.x);
			// 2007.5.7 김영규
			m_Graph.m_pGraph->ValidPeak(point.x, EVENT_OPEN);
		}
	}
	
	CDialog::OnLButtonDown(nFlags, point);
}

void CGMapViewerDlg::ShowCursorInfo(int nCursorNo, int nCursorPos)
{
	CString csValue;
	CString csMsg;
	int nXpos;
	int nYpos;
	int nScale=0;
	double dVOP=0.0;

	int nEvent = 0;
	int nEventPlus = 0;
	double dTemp=0;
	double dEventValue=0;
	double dEventPlusValue=0;
	double dDBRL=0.0;

	if(m_Graph.m_pGraph->IsEmpty()) return;

	EVENTDETECT_INFO EventInfo;
	memset((void*)&EventInfo, 0x00, EVENTDETECT_INFO_SIZE);
	m_Graph.m_pGraph->GetEventInfo(&EventInfo);
	
	// VOP는 사용자 설정값을 사용한다. 2005.6.14
	UpdateData(TRUE);
	dVOP = m_Graph.nTdrVOP / 10.0;
	
	nXpos = (int)nCursorPos;
	switch(theApp.m_gvInfo.nGraphSel) {
	case 0: // NONE
		nXpos = (nXpos >= m_Graph.m_pGraph->m_MeasureInfo.nLength) ? m_Graph.m_pGraph->m_MeasureInfo.nLength - 1: nXpos;
		nYpos = (int)m_Graph.m_pGraph->m_MeasureInfo.pWaveData[nXpos];
		break;
	case 1: // 베이스(RAW)
		nXpos = (nXpos >= m_Graph.m_pGraph->m_BaseInfo.nLength) ? m_Graph.m_pGraph->m_MeasureInfo.nLength - 1: nXpos;
		nYpos = (int)m_Graph.m_pGraph->m_BaseInfo.pRawData[nXpos];
		break;
	case 2: // 베이스(Filter)
		nXpos = (nXpos >= m_Graph.m_pGraph->m_BaseInfo.nLength) ? m_Graph.m_pGraph->m_MeasureInfo.nLength - 1: nXpos;
		nYpos = (int)m_Graph.m_pGraph->m_BaseInfo.pWaveData[nXpos];
		break;
	case 3: // 측정(RAW)
		nXpos = (nXpos >= m_Graph.m_pGraph->m_MeasureInfo.nLength) ? m_Graph.m_pGraph->m_MeasureInfo.nLength - 1: nXpos;
		nYpos = (int)m_Graph.m_pGraph->m_MeasureInfo.pRawData[nXpos];
		break;
	case 4: // 측정(Filter)
		nXpos = (nXpos >= m_Graph.m_pGraph->m_MeasureInfo.nLength) ? m_Graph.m_pGraph->m_MeasureInfo.nLength - 1: nXpos;
		nYpos = (int)m_Graph.m_pGraph->m_MeasureInfo.pWaveData[nXpos];
	}

	// Cursor 위치 변경
	m_viewGraph.SetCursor(nCursorNo);
	m_viewGraph.SetCursorX(nXpos);
	m_viewGraph.SetCursorY(nYpos);

	switch(nCursorNo) {
	case 0: // cursor 1
		theApp.m_gvInfo.nCurPos1 = nCursorPos;
		csMsg.Format("Cursor 1");
		break;
	case 1: // cursor 2 : 이벤트 지시 커서
		theApp.m_gvInfo.nCurPos2 = nCursorPos;
	    m_Graph.m_pGraph->DistanceCorrection(nXpos, m_uFindEventType);
		// x,y pos
		csValue.Format("%.1fm (x=%d y=%.0f)", m_Graph.m_pGraph->GetDistance(), nXpos, (float)nYpos);
//		SetDlgItemText(IDC_STATIC_LOC, csValue);
		break;
	case 2: // 감시구간 : 시작
		csMsg.Format("Scan Pos(%d)", nXpos);
		break;
	case 3: // 감시구간 : 끝
		csMsg.Format("Scan Pos(%d)", nXpos);
		break;
	}

	if(nCursorNo < 2) {		
		// Cursor의 위치 표시 갱신
		double dCursor1Meter=0.;
		double dCursor2Meter=0.;
		double dDistanceMeter=0.;

		// Annotation 위치 변경
		m_viewGraph.SetAnnotation(nCursorNo);
		m_viewGraph.SetAnnoLabelCaption(csMsg);

		m_viewGraph.SetAnnoLabelX(nXpos + 20); // +20
		m_viewGraph.SetAnnoLabelY(nYpos - 15); // -15

//		dCursor1Meter = ConvertToMeter(m_nCurPos1); // 미터단위로 환산
//		csMsg.Format(" %.1f m", dCursor1Meter);
//		GetDlgItem(IDC_STATIC_CURSOR1)->SetWindowText(csMsg);

//		dCursor2Meter = ConvertToMeter(m_nCurPos2); // 미터단위로 환산
		dDistanceMeter = m_Graph.m_pGraph->GetDistance();
//		csMsg.Format(" %.1f m", dDistanceMeter);
//		GetDlgItem(IDC_STATIC_CURSOR2)->SetWindowText(csMsg);

		// Distance 표시
		m_Graph.nDistance = (theApp.m_gvInfo.nCurPos2 - theApp.m_gvInfo.nCurPos1);
		csMsg.Format("%.1fm (x=%d y=%.0f)", dDistanceMeter, nXpos, (float)nYpos);
		SetDlgItemText(IDC_STATIC_LOC, csMsg);

		m_strUrl.Format("http://www.wacon.co.kr/map/view.php?tid=%d&cid=%d&dist=%.1f", m_nTID, m_nCID, dDistanceMeter);
		m_dDistant = dDistanceMeter;

		char buffer[256]={0,};
		CString strDateTime;

		COleDateTime dtDate=COleDateTime::GetCurrentTime();
		strDateTime.Format("%4d-%2d-%2d %2d:%2d:%2d", dtDate.GetYear(),
												dtDate.GetMonth(),
												dtDate.GetDay(),
												dtDate.GetHour(),
												dtDate.GetMinute(),
												dtDate.GetSecond());

		sprintf(buffer, "UPDATE t_event SET dist=%.1f, app_name='%s', rdate='%s'", dDistanceMeter, "gMapViewer ver 0.1", strDateTime);
		if(mysql_query(&m_mySQL, buffer))
		{
			TRACE("error : %s\n", mysql_error(&m_mySQL));
			InitMySQL();
		}

		UpdateData(FALSE);
	}
}

BOOL CGMapViewerDlg::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message == WM_KEYDOWN) {
		switch(pMsg->wParam) {
		case VK_ESCAPE:
			return TRUE;
			
			// Zoom 컨트롤
		case VK_LEFT:
			OnButtonLeft();
			break;
			
		case VK_UP:
			OnButtonUp();
			break;
			
		case VK_RIGHT:
			OnButtonRight();
			break;
			
		case VK_DOWN:
			OnButtonDown();
			break;
			
			// 화일 컨트롤
		case VK_PRIOR:
//			OnMenuPrev();
			break;
			
		case VK_NEXT:
//			OnMenuNext();
			break;
			
		case VK_HOME:
//			GoHome();
			break;
			
		case VK_END:
//			GoEnd();
			break;
			
		case VK_F5: // Reload file list
			theApp.m_gvInfo.nCurFile = 0;		// 현재화일 인덱스
			theApp.m_gvInfo.nFileCnt = 0;		// 화일수
			LoadWavefileList(theApp.m_gvInfo.csLastAccessDir);
			TRACE("%d개의 화일이 재로드 되었습니다.\n", theApp.m_gvInfo.nFileCnt);
			break;
			
		case VK_F8: // find Min peak
			TRACE("Finding MinPeak >> Mouse Point=%d\n", theApp.m_gvInfo.nCurPos2);
			FindPeak(theApp.m_gvInfo.nCurPos2, EVENT_SHORT);
			break;
			
		case VK_F9: // find max peak
			TRACE("Finding MaxPeak >> Mouse Point=%d\n", theApp.m_gvInfo.nCurPos2);
			FindPeak(theApp.m_gvInfo.nCurPos2, EVENT_OPEN);
			break;
		}
	}	
	return CDialog::PreTranslateMessage(pMsg);
}

void CGMapViewerDlg::FindPeak(int nPoint, EVENT_MODE uEventMode)
{
	double dMaxValue = -999999;
	double dMinValue = 999999;
	int nMaxPos = 0;
	int nMinPos = 0;
	double dValue = 0;
	int nIndex=0;
	
	int nStart=0;
	int nEnd=0;

	if(nPoint <= 0) return;
	
	switch(theApp.m_gvInfo.nGraphSel) {
	case 0: // NONE
		break;
	case 1: // 베이스(RAW)
		if(theApp.m_gvInfo.bBaseGraphVisible) {
			
			nStart	= 0 > nPoint-64 ? 0 : nPoint-64;
			nEnd	= m_Graph.m_pGraph->m_BaseInfo.nLength < nPoint+64 ? m_Graph.m_pGraph->m_BaseInfo.nLength : nPoint+64;

			for (nIndex=nStart; nIndex<nEnd; nIndex++)
			{
				dValue = m_Graph.m_pGraph->m_BaseInfo.pRawData[nIndex];
				if(dValue > dMaxValue) {
					dMaxValue = dValue;
					nMaxPos = nIndex;
				}
				if(dValue < dMinValue) {
					dMinValue = dValue;
					nMinPos = nIndex;
				}
			}
		}
		break;
	case 2: // 베이스(Filter)
		if(theApp.m_gvInfo.bBaseFilterGraphVisible) {

			nStart	= 0 > nPoint-64 ? 0 : nPoint-64;
			nEnd	= m_Graph.m_pGraph->m_BaseInfo.nLength < nPoint+64 ? m_Graph.m_pGraph->m_BaseInfo.nLength : nPoint+64;

			for (nIndex=nStart; nIndex<nEnd; nIndex++)
			{
				dValue = m_Graph.m_pGraph->m_BaseInfo.pWaveData[nIndex];
				if(dValue > dMaxValue) {
					dMaxValue = dValue;
					nMaxPos = nIndex;
				}
				if(dValue < dMinValue) {
					dMinValue = dValue;
					nMinPos = nIndex;
				}
			}
		}
		break;
	case 3: // 측정(RAW)
		if(theApp.m_gvInfo.bMeasureGraphVisible) {
			nStart	= 0 > nPoint-64 ? 0 : nPoint-64;
			nEnd	= m_Graph.m_pGraph->m_MeasureInfo.nLength < nPoint+64 ? m_Graph.m_pGraph->m_MeasureInfo.nLength : nPoint+64;

			for (nIndex=nStart; nIndex<nEnd; nIndex++)
			{
				dValue = m_Graph.m_pGraph->m_MeasureInfo.pRawData[nIndex];
				if(dValue > dMaxValue) {
					dMaxValue = dValue;
					nMaxPos = nIndex;
				}
				if(dValue < dMinValue) {
					dMinValue = dValue;
					nMinPos = nIndex;
				}
			}
		}
		break;
	case 4: // 측정(Filter)
		if(theApp.m_gvInfo.bFilterGraphVisible) {
			nStart	= 0 > nPoint-64 ? 0 : nPoint-64;
			nEnd	= m_Graph.m_pGraph->m_MeasureInfo.nLength < nPoint+64 ? m_Graph.m_pGraph->m_MeasureInfo.nLength : nPoint+64;

			for (nIndex=nStart; nIndex<nEnd; nIndex++)
			{
				dValue = m_Graph.m_pGraph->m_MeasureInfo.pWaveData[nIndex];
				if(dValue > dMaxValue) {
					dMaxValue = dValue;
					nMaxPos = nIndex;
				}
				if(dValue < dMinValue) {
					dMinValue = dValue;
					nMinPos = nIndex;
				}
			}
		}
		break;
	case 5: // 차(Diff)
		nStart	= 0 > nPoint-64 ? 0 : nPoint-64;
		nEnd	= m_Graph.m_pGraph->m_MeasureInfo.nLength < nPoint+64 ? m_Graph.m_pGraph->m_MeasureInfo.nLength : nPoint+64;
		
		for (nIndex=nStart; nIndex<nEnd; nIndex++)
		{
			dValue = m_Graph.m_pGraph->m_MeasureInfo.pRawData[nIndex] - m_Graph.m_pGraph->m_BaseInfo.pRawData[nIndex];
			if(dValue > dMaxValue) {
				dMaxValue = dValue;
				nMaxPos = nIndex;
			}
			if(dValue < dMinValue) {
				dMinValue = dValue;
				nMinPos = nIndex;
			}
		}
	}	

	switch(uEventMode)
	{
	case EVENT_OPEN:
		ShowCursorInfo(1, nMaxPos);
		break;
	default:
		ShowCursorInfo(1, nMinPos);
	    break;
	}
}

void CGMapViewerDlg::OnCheckB() 
{
	UpdateData(TRUE);
	theApp.m_gvInfo.bBaseGraphVisible =  m_bBaseView;

	theApp.m_gvInfo.nGraphSel = 1;
	m_viewGraph.SetElement(theApp.m_gvInfo.nGraphSel-1);
	m_viewGraph.SetElementShow(m_bBaseView);

	if(m_bBaseView) {
		TRACE("베이스그래프 ON\n");
	}
	else {
		TRACE("베이스그래프 OFF\n");
	}

}

void CGMapViewerDlg::OnCheckBf() 
{
	UpdateData(TRUE);
	theApp.m_gvInfo.bBaseFilterGraphVisible =  m_bBaseFilterView;

	theApp.m_gvInfo.nGraphSel = 2;
	m_viewGraph.SetElement(theApp.m_gvInfo.nGraphSel-1);
	m_viewGraph.SetElementShow(m_bBaseFilterView);
	if(m_bBaseFilterView) {
		TRACE("베이스필터 그래프 ON\n");
		ReDraw();
	}
	else {
		TRACE("베이스필터 그래프 OFF\n");
	}
}

void CGMapViewerDlg::OnCheckM() 
{
	UpdateData(TRUE);
	theApp.m_gvInfo.bMeasureGraphVisible =  m_bMeasureView;
	
	theApp.m_gvInfo.nGraphSel = 3;
	m_viewGraph.SetElement(theApp.m_gvInfo.nGraphSel-1);
	m_viewGraph.SetElementShow(m_bMeasureView);

	if(m_bMeasureView) {
		TRACE("측정그래프 ON\n");
		DrawGraph();
	}
	else {
		TRACE("측정그래프 OFF\n");
	}
}

void CGMapViewerDlg::OnCheckMf() 
{
	UpdateData(TRUE);
	theApp.m_gvInfo.bFilterGraphVisible =  m_bMeasureFilterView;

	theApp.m_gvInfo.nGraphSel = 4;
	m_viewGraph.SetElement(theApp.m_gvInfo.nGraphSel-1);
	m_viewGraph.SetElementShow(m_bMeasureFilterView);

	if(m_bMeasureFilterView) {
		TRACE("측정필터 그래프 ON\n");
		DrawGraph();
	}
	else {
		TRACE("측정필터 그래프 OFF\n");
	}
}

void CGMapViewerDlg::OnCheckD() 
{
	UpdateData(TRUE);
	theApp.m_gvInfo.bDiffGraphVisible =  m_bDiffView;

	theApp.m_gvInfo.nGraphSel = 5;
	m_viewGraph.SetElement(theApp.m_gvInfo.nGraphSel-1);
	m_viewGraph.SetElementShow(m_bDiffView);

	if(m_bDiffView) {
		TRACE("Diff그래프 ON\n");
		DrawGraph();
	}
	else {
		TRACE("Diff그래프 OFF\n");
	}
}


void CGMapViewerDlg::OnButtonRedraw() 
{
	ReDraw();
}

