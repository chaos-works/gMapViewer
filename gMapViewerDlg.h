// gMapViewerDlg.h : header file
//
//{{AFX_INCLUDES()
#include "webbrowser2.h"
#include "ntgraph.h"
//}}AFX_INCLUDES

#if !defined(AFX_GMAPVIEWERDLG_H__066B7305_7481_4157_9F79_44BB514C2146__INCLUDED_)
#define AFX_GMAPVIEWERDLG_H__066B7305_7481_4157_9F79_44BB514C2146__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "../common/FileDialog_ReadOnly.h"
#include "GraphData.h"
#include "define_value.h"
#include "../Common/BtnST.h"

#include "mysql.h"

/////////////////////////////////////////////////////////////////////////////
// CGMapViewerDlg dialog

class CGMapViewerDlg : public CDialog
{
// Construction
public:
	CGMapViewerDlg(CWnd* pParent = NULL);	// standard constructor

public:
	int m_nTID;
	int m_nCID;
	CString m_strTID;
	APP_INFO m_AppInfo;
	GRAPH m_Graph;
	EVENT_MODE		m_uFindEventType;	// 탐색이벤트 타입

	CStringArray	m_WaveFilelist;		// 화일리스트

	MYSQL m_mySQL;

public:
	void DisplayVersion();
	void InitControl();
	void InitGraph();
	BOOL InitMySQL();
	void DelMySQL();

	int	LoadWavefileList(CString csFoldname);
	BOOL OpenWaveFile(CString);
	int FileIndex(CString csFilename);
	int GetChannelNo(CString csPathname);
	void DisplayGraph();
	void DrawGraph();
	CString GetMeasureFilename();
	void DoZoom();
	void DoScroll();
	void ShowCursorInfo(int nCursorNo, int nCursorPos);
	BOOL DisplayResult();
	void ApplyConfig();
	void ReDraw();
	
	void OnButtonUp();
	void OnButtonDown();
	void OnButtonLeft();
	void OnButtonRight();
	void FindPeak(int nPoint, EVENT_MODE uEventMode);


// Dialog Data
	//{{AFX_DATA(CGMapViewerDlg)
	enum { IDD = IDD_GMAPVIEWER_DIALOG };
	CButtonST	m_cbRedraw;
	CButtonST	m_cbConfig;
	CButtonST	m_cbOpenWaveFile;
	CComboBox	m_cmbTdrID;
	CComboBox	m_cmbChannelId;
	CString	m_strUrl;
	CString	m_strOutput;
	double	m_dDistant;
	CNTGraph	m_viewGraph;
	CWebBrowser2	m_viewRoad;
	CWebBrowser2	m_viewMap;
	CString	m_strWaveFilePath;
	BOOL	m_bBaseFilterView;
	BOOL	m_bDiffView;
	BOOL	m_bMeasureView;
	BOOL	m_bMeasureFilterView;
	BOOL	m_bBaseView;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGMapViewerDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	CWebBrowser2* m_pBrowser;

	// Generated message map functions
	//{{AFX_MSG(CGMapViewerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOK();
	afx_msg void OnButton2();
	afx_msg void OnDocumentCompleteExplorer1(LPDISPATCH pDisp, VARIANT FAR* URL);
	afx_msg void OnButtonViewpoint();
	afx_msg void OnSelchangeComboCid();
	afx_msg void OnSelchangeComboTid();
	afx_msg void OnChangeEditLength();
	afx_msg void OnUpdateEditLength();
	afx_msg void OnButtonRoadview();
	afx_msg void OnButtonWaveview();
	afx_msg void OnButtonWaveFileOpen();
	afx_msg void OnRadioTrack();
	afx_msg void OnRadioZoom();
	afx_msg void OnRadioPan();
	afx_msg void OnRadioCursor();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnCheckB();
	afx_msg void OnCheckBf();
	afx_msg void OnCheckM();
	afx_msg void OnCheckMf();
	afx_msg void OnCheckD();
	afx_msg void OnDestroy();
	afx_msg void OnButtonRedraw();
	afx_msg void OnRadioEnone();
	afx_msg void OnRadioEshort();
	afx_msg void OnRadioEopen();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

HRESULT GetLibraryVersion(LPDWORD pdwMajor, LPDWORD pdwMinor, LPDWORD pdwBuild);
#endif // !defined(AFX_GMAPVIEWERDLG_H__066B7305_7481_4157_9F79_44BB514C2146__INCLUDED_)
