#if !defined(AFX_WEBBROWSER_H__DF9A03AA_E35A_4C2F_A4C5_3A894861B672__INCLUDED_)
#define AFX_WEBBROWSER_H__DF9A03AA_E35A_4C2F_A4C5_3A894861B672__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WebBrowser.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWebBrowser html view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif
#include <afxhtml.h>

class CWebBrowser : public CHtmlView
{
protected:
	CWebBrowser();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CWebBrowser)

// html Data
public:
	//{{AFX_DATA(CWebBrowser)
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Attributes
public:
	CString m_strUrl;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWebBrowser)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CWebBrowser();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CWebBrowser)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WEBBROWSER_H__DF9A03AA_E35A_4C2F_A4C5_3A894861B672__INCLUDED_)
