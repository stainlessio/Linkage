#pragma once


// CGearRatioDialog dialog

#pragma once

#include "mydialog.h"
#include "afxwin.h"

class CGearRatioDialog : public CMyDialog
{
	DECLARE_DYNAMIC(CGearRatioDialog)

public:
	CGearRatioDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CGearRatioDialog();

// Dialog Data
	enum { IDD = IDD_GEAR_RATIO_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	double m_Gear1Size;
	double m_Gear2Size;
	CString m_Link1Name;
	CString m_Link2Name;
	int m_GearChainSelection;
	BOOL m_bUseRadiusValues;
	afx_msg void OnBnClickedRadio();
	CButton m_GearRadioControl;
	CButton m_UseRadiiControl;
};
