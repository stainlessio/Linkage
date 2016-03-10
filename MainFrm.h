// This MFC Samples source code demonstrates using MFC Microsoft Office Fluent User Interface 
// (the "Fluent UI") and is provided only as referential material to supplement the 
// Microsoft Foundation Classes Reference and related electronic documentation 
// included with the MFC C++ library software.  
// License terms to copy, use or distribute the Fluent UI are available separately.  
// To learn more about our Fluent UI licensing program, please visit 
// http://msdn.microsoft.com/officeui.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// MainFrm.h : interface of the CMainFrame class
//

#pragma once
#include "Resource.h"

//class COutlookBar : public CMFCOutlookBar
//{
//	virtual BOOL AllowShowOnPaneMenu() const { return TRUE; }
//	virtual void GetPaneName(CString& strName) const { BOOL bNameValid = strName.LoadString(IDS_OUTLOOKBAR); ASSERT(bNameValid); }
//};

class CMyMFCRibbonBar : public CMFCRibbonBar
{
	public:
	void SetPrintPreviewMode( BOOL bSet = 1 ) { CMFCRibbonBar::SetPrintPreviewMode( bSet ); }
};

class CMainFrame : public CFrameWndEx
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	CMFCRibbonStatusBar  m_wndStatusBar;
	CMyMFCRibbonBar     m_wndRibbonBar;

protected:  // control bar embedded members
	CMFCRibbonApplicationButton m_MainButton;
	CMFCToolBarImages m_PanelImages;
	CImageList m_PanelImagesList;

private:
	void CreateMainCategory( void );
	void CreateHomeCategory( void );
	void CreatePrintingCategory( void );
	void CreateQuickAccessCommands( void );
	void CreateSamplePanel( CMFCRibbonCategory* pCategory );
	void CreateClipboardPanel( CMFCRibbonCategory* pCategory );
	void CreateViewPanel( CMFCRibbonCategory* pCategory );
	void CreateDimensionsPanel( CMFCRibbonCategory* pCategory );
	void CreateInsertPanel( CMFCRibbonCategory* pCategory );
	void CreateElementPanel( CMFCRibbonCategory* pCategory );
	void CreateAlignPanel( CMFCRibbonCategory* pCategory );
	void CreateSimulationPanel( CMFCRibbonCategory* pCategory );
	void CreatePrintPanel( CMFCRibbonCategory* pCategory );
	void CreateSettingsPanel( CMFCRibbonCategory* pCategory );
	void CreatePrintOptionsPanel( CMFCRibbonCategory* pCategory );
	void CreateHelpButtons( void );
	void InitializeRibbon();
	int FindFocusedOutlookWnd(CMFCOutlookBarTabCtrl** ppOutlookWnd);
	void ConfigureDocumentationMenu( CMenu *pMenu );
	void AppendMenuItem( CMenu &Menu, UINT StringID, UINT ID, int BitmapIndex );
	void AppendMenuItem( CMFCRibbonButton *pButton, UINT StringID, UINT ID, int BitmapIndex );
	void AppendMenuCheck( CMFCRibbonButton *pButton, UINT StringID, UINT ID );

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnFilePrint();
	afx_msg void OnFilePrintPreview();
	afx_msg void OnUpdateFilePrintPreview(CCmdUI* pCmdUI);
	afx_msg void OnHelpUserguide();
	afx_msg void OnHelpAbout();
	afx_msg void OnSelectSample();
	DECLARE_MESSAGE_MAP()

	CMFCOutlookBarTabCtrl* FindOutlookParent(CWnd* pWnd);
	CMFCOutlookBarTabCtrl* m_pCurrOutlookWnd;
	CMFCOutlookBarPane*    m_pCurrOutlookPage;
	CMFCRibbonGallery*	m_pSampleGallery;
public:
	afx_msg void OnDropFiles(HDROP hDropInfo);
};


