#if !defined(AFX_NTGRAPH_H__62CB4278_EE3A_4954_9A9E_4DE800C8AEE9__INCLUDED_)
#define AFX_NTGRAPH_H__62CB4278_EE3A_4954_9A9E_4DE800C8AEE9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Machine generated IDispatch wrapper class(es) created by Microsoft Visual C++

// NOTE: Do not modify the contents of this file.  If this class is regenerated by
//  Microsoft Visual C++, your modifications will be overwritten.


// Dispatch interfaces referenced by this interface
class COleFont;
class CPicture;

/////////////////////////////////////////////////////////////////////////////
// CNTGraph wrapper class

class CNTGraph : public CWnd
{
protected:
	DECLARE_DYNCREATE(CNTGraph)
public:
	CLSID const& GetClsid()
	{
		static CLSID const clsid
			= { 0xc9fe01c2, 0x2746, 0x479b, { 0x96, 0xab, 0xe0, 0xbe, 0x99, 0x31, 0xb0, 0x18 } };
		return clsid;
	}
	virtual BOOL Create(LPCTSTR lpszClassName,
		LPCTSTR lpszWindowName, DWORD dwStyle,
		const RECT& rect,
		CWnd* pParentWnd, UINT nID,
		CCreateContext* pContext = NULL)
	{ return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID); }

    BOOL Create(LPCTSTR lpszWindowName, DWORD dwStyle,
		const RECT& rect, CWnd* pParentWnd, UINT nID,
		CFile* pPersist = NULL, BOOL bStorage = FALSE,
		BSTR bstrLicKey = NULL)
	{ return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID,
		pPersist, bStorage, bstrLicKey); }

// Attributes
public:
	short GetAppearance();
	void SetAppearance(short);
	CString GetCaption();
	void SetCaption(LPCTSTR);
	BOOL GetEnabled();
	void SetEnabled(BOOL);
	unsigned long GetAxisColor();
	void SetAxisColor(unsigned long);
	unsigned long GetGridColor();
	void SetGridColor(unsigned long);
	unsigned long GetLabelColor();
	void SetLabelColor(unsigned long);
	unsigned long GetCursorColor();
	void SetCursorColor(unsigned long);
	COleFont GetLabelFont();
	void SetLabelFont(LPDISPATCH);
	COleFont GetTickFont();
	void SetTickFont(LPDISPATCH);
	COleFont GetTitleFont();
	void SetTitleFont(LPDISPATCH);
	COleFont GetIdentFont();
	void SetIdentFont(LPDISPATCH);
	short GetXGridNumber();
	void SetXGridNumber(short);
	short GetYGridNumber();
	void SetYGridNumber(short);
	BOOL GetShowGrid();
	void SetShowGrid(BOOL);
	CString GetXLabel();
	void SetXLabel(LPCTSTR);
	CString GetYLabel();
	void SetYLabel(LPCTSTR);
	unsigned long GetElementLineColor();
	void SetElementLineColor(unsigned long);
	unsigned long GetElementPointColor();
	void SetElementPointColor(unsigned long);
	long GetElementLinetype();
	void SetElementLinetype(long);
	long GetElementWidth();
	void SetElementWidth(long);
	long GetElementPointSymbol();
	void SetElementPointSymbol(long);
	BOOL GetElementSolidPoint();
	void SetElementSolidPoint(BOOL);
	BOOL GetElementShow();
	void SetElementShow(BOOL);
	long GetTrackMode();
	void SetTrackMode(long);
	CString GetElementName();
	void SetElementName(LPCTSTR);
	BOOL GetElementIdentify();
	void SetElementIdentify(BOOL);
	BOOL GetXLog();
	void SetXLog(BOOL);
	BOOL GetYLog();
	void SetYLog(BOOL);
	CPicture GetControlFramePicture();
	void SetControlFramePicture(LPDISPATCH);
	CPicture GetPlotAreaPicture();
	void SetPlotAreaPicture(LPDISPATCH);
	unsigned long GetControlFrameColor();
	void SetControlFrameColor(unsigned long);
	unsigned long GetPlotAreaColor();
	void SetPlotAreaColor(unsigned long);
	long GetFrameStyle();
	void SetFrameStyle(long);
	CString GetAnnoLabelCaption();
	void SetAnnoLabelCaption(LPCTSTR);
	double GetAnnoLabelX();
	void SetAnnoLabelX(double);
	double GetAnnoLabelY();
	void SetAnnoLabelY(double);
	unsigned long GetAnnoLabelColor();
	void SetAnnoLabelColor(unsigned long);
	BOOL GetAnnoLabelHorizontal();
	void SetAnnoLabelHorizontal(BOOL);
	short GetAnnotation();
	void SetAnnotation(short);
	BOOL GetAnnoVisible();
	void SetAnnoVisible(BOOL);
	short GetAnnoCount();
	void SetAnnoCount(short);
	short GetElement();
	void SetElement(short);
	short GetElementCount();
	void SetElementCount(short);
	unsigned long GetAnnoLabelBkColor();
	void SetAnnoLabelBkColor(unsigned long);
	short GetCursorCount();
	void SetCursorCount(short);
	short GetCursor();
	void SetCursor(short);
	double GetCursorX();
	void SetCursorX(double);
	double GetCursorY();
	void SetCursorY(double);
	short GetCursorStyle();
	void SetCursorStyle(short);
	BOOL GetCursorVisible();
	void SetCursorVisible(BOOL);
	short GetCursorMode();
	void SetCursorMode(short);
	CString GetFormatAxisBottom();
	void SetFormatAxisBottom(LPCTSTR);
	CString GetFormatAxisLeft();
	void SetFormatAxisLeft(LPCTSTR);
	BOOL GetYTime();
	void SetYTime(BOOL);
	BOOL GetXTime();
	void SetXTime(BOOL);

// Operations
public:
	void SetRange(double xmin, double xmax, double ymin, double ymax);
	void AutoRange();
	void CopyToClipboard();
	void PrintGraph();
	void AddElement();
	void DeleteElement(short ElementID);
	void ClearGraph();
	double GetElementXValue(long index, short ElementID);
	void SetElementXValue(long index, short ElementID, double newValue);
	double GetElementYValue(long index, short ElementID);
	void SetElementYValue(long index, short ElementID, double newValue);
	void PlotXY(double X, double Y, short ElementID);
	void PlotY(double Y, short ElementID);
	void ShowProperties();
	void SaveAs(LPCTSTR szFilename);
	void AddAnnotation();
	void DeleteAnnotation(short annoID);
	void AddCursor();
	void DeleteCursor(short cursorID);
	void GetRange(double* pxmin, double* pxmax, double* pymin, double* pymax);
	void SetScale(short nScale);
	void SetVop(double dVop);
	void AboutBox();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NTGRAPH_H__62CB4278_EE3A_4954_9A9E_4DE800C8AEE9__INCLUDED_)
