// gMapViewer.h : main header file for the GMAPVIEWER application
//

#if !defined(AFX_GMAPVIEWER_H__C5441B98_CBA9_4674_A04B_ECD7039011BB__INCLUDED_)
#define AFX_GMAPVIEWER_H__C5441B98_CBA9_4674_A04B_ECD7039011BB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "Ini.h"

#include "GraphData.h"
#include "define_value.h"

/////////////////////////////////////////////////////////////////////////////
// CGMapViewerApp:
// See gMapViewer.cpp for the implementation of this class
//

class CGMapViewerApp : public CWinApp
{
public:
	CGMapViewerApp();

public:
	CIni	m_ini;				// ini file
	CIni	m_coef;				// coef file
    CString m_csSettingPath;	// 설정화일
    CString     m_csExecPath;
	CString 	m_csFolderPath;				// 배치 측정된 결과 저장폴더
	CString		m_csCoefPath;
	GRAPH_VIEW m_gvInfo;
private:
    HANDLE      m_hInstanceMutex;
     
// Function
public:
	void	LoadConfig();
	void	SaveConfig();
	BOOL GetResult(CString csFilename, NANOWAVE_EX* pNanowaveEx);
	BOOL GetFilename(CString csPathname, char* spFolderName, char* spFilename);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGMapViewerApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CGMapViewerApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern CGMapViewerApp theApp;

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GMAPVIEWER_H__C5441B98_CBA9_4674_A04B_ECD7039011BB__INCLUDED_)
