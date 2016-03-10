#pragma once
#include "afxwin.h"
#include "afxcmn.h"

class CLinkageView;

// CExportImageSettingsDialog dialog

class CExportImageSettingsDialog : public CDialog
{
	DECLARE_DYNAMIC(CExportImageSettingsDialog)

public:
	CExportImageSettingsDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CExportImageSettingsDialog();

// Dialog Data
	enum { IDD = IDD_EXPORTIMAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:

	CLinkageView *m_pLinkageView;

	int GetResolutionWidth( void );
	int GetResolutionHeight( void );
	double GetResolutionScale( void );
	double GetMarginScale( void );

private:
	afx_msg void OnBnClickedResolution();
	afx_msg void OnEnChangeScaleFactor();
	CSpinButtonCtrl m_SpinControl;
	CRect m_DrawArea;
	int m_ResolutionSelection;
	void UpdateImage( void );
	bool m_bReadyToShowImage;
	int m_ScaleFactor;
	CStatic m_PreviewArea;
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();

public:
	afx_msg void OnCopy();
	int m_Margin;
	CSpinButtonCtrl m_MarginSpinControl;
	afx_msg void OnEnChangeEdit7();
};
