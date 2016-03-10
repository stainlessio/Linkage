#pragma once
#include "afxwin.h"
#include "MyDialog.h"
#include "ColorStatic.h"
#include "NullableColor.h"


// CPointPropertiesDialog dialog

class CPointPropertiesDialog : public CMyDialog
{
	DECLARE_DYNAMIC(CPointPropertiesDialog)

public:
	CPointPropertiesDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPointPropertiesDialog();

// Dialog Data
	enum { IDD = IDD_POINT_PROPERTIES };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	double m_RPM;
	virtual BOOL OnInitDialog();
	double m_xPosition;
	double m_yPosition;
	CString m_Name;
	CButton m_DrawCircleControl;
	BOOL m_bDrawCircle;
	afx_msg void OnBnClickedCheck1();
	CStatic m_RadiusPromptControl;
	CEdit m_RadiusControl;
	double m_Radius;
	CStatic m_FastenToControl;
	CString m_FastenTo;
	afx_msg void OnStnClickedColor();
	CColorStatic m_ColorControl;
	CNullableColor m_Color;
};
