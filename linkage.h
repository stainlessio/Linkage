// Linkage.h : main header file for the Linkage application
//

#if !defined(AFX_Linkage_H__5264C550_7E3B_4583_A5CD_BDAA527599CF__INCLUDED_)
#define AFX_Linkage_H__5264C550_7E3B_4583_A5CD_BDAA527599CF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CLinkageApp:
// See Linkage.cpp for the implementation of this class
//

class CLinkageApp : public CWinAppEx
{
public:
	CLinkageApp();

// Implementation
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLinkageApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	void SetPrinterOrientation( int OrientationMode );

// Implementation
	//{{AFX_MSG(CLinkageApp)
	afx_msg void OnAppAbout();
	afx_msg void OnFilePrintSetup() { CWinAppEx::OnFilePrintSetup(); }
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

void SetStatusText( const char *pText = 0 );
void SetStatusText( UINT ResourceID);
void ShowPrintPreviewCategory( bool bShow = true );


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

extern CLinkageApp theApp;

#endif // !defined(AFX_Linkage_H__5264C550_7E3B_4583_A5CD_BDAA527599CF__INCLUDED_)
