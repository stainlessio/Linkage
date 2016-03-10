#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "MyDialog.h"
#include "NullableColor.h"
#include "ColorStatic.h"


// CLinkPropertiesDialog dialog

class CLinkPropertiesDialog : public CMyDialog
{
	DECLARE_DYNAMIC(CLinkPropertiesDialog)

public:
	CLinkPropertiesDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CLinkPropertiesDialog();

// Dialog Data
	enum { IDD = IDD_LINK_PROPERTIES };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	int m_LineSize;
	BOOL m_bSolid;
	CSpinButtonCtrl m_SpinControl;
	BOOL m_bActuator;
	int m_ConnectorCount;
	CButton m_ActuatorControl;
	CEdit m_ActuatorCPMControl;
	double m_ActuatorCPM;
	bool m_bFastened;

protected:

public:
	CButton m_AlwaysManualCheckbox;
	BOOL m_bAlwaysManual;
	afx_msg void OnBnClickedCheck2();
	int m_SelectedLinkCount;
	CString m_Name;
	CStatic m_NamePromptControl;
	CEdit m_NameControl;
	CEdit m_ThrowDistanceControl;
	double m_ThrowDistance;
	CStatic m_ThrowDistancePrompt;
	BOOL m_bIsGear;
	CStatic m_FastenedToControl;
	CString m_FastenedTo;
	CButton m_SolidControl;
	CNullableColor m_Color;
	afx_msg void OnStnClickedColor();
	CColorStatic m_ColorControl;
	CStatic m_ColorPromptControl;
	BOOL m_bLocked;
	CButton m_LockedControl;
};
