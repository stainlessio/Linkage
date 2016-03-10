#pragma once
#include "afxwin.h"
#include "MyDialog.h"
#include "PopupGallery.h"
#include "NullableColor.h"
#include "ColorStatic.h"


// CConnectorPropertiesDialog dialog

class CConnectorPropertiesDialog : public CMyDialog
{
	DECLARE_DYNAMIC(CConnectorPropertiesDialog)

public:
	CConnectorPropertiesDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CConnectorPropertiesDialog();

// Dialog Data
	enum { IDD = IDD_CONNECTOR_PROPERTIES };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	double m_RPM;
	afx_msg void OnBnClickedRadio();
	int m_Property;
	bool m_bAnchor;
	bool m_bInput;
	CButton m_SimpleConnectorButton;
	CButton m_RotatingInputButton;
	CEdit m_RPMControl;
	bool m_bDrawing;
	virtual BOOL OnInitDialog();
	CButton m_RotatingAnchorControl;
	bool m_bAlwaysManual;
	CButton m_AlwaysManualCheckbox;
	double m_xPosition;
	double m_yPosition;
	CString m_Name;
	CPopupGallery m_GalleryControl;
	CButton m_DrawCheckControl;
	CButton m_ConnectorControl;
	CStatic m_SlideRadiusPrompt;
	CEdit m_SlideRadiusControl;
	double m_SlideRadius;
	CButton m_SlideRadiusCheck;
	BOOL m_bSlideRadiusPath;
	bool m_bIsSlider;
	double m_MinimumSlideRadius;
	afx_msg void OnBnClickedCheck4();
	CStatic m_SlideRadiusMinPrompt;
	CStatic m_SlideRadiusMinControl;
	CEdit m_xControl;
	CEdit m_yControl;
	BOOL m_bIsGear;
	CButton m_GearCheckboxControl;
	bool m_bGearAllowed;
	CStatic m_FastenedToControl;
	CString m_FastenedTo;
	CNullableColor m_Color;
	afx_msg void OnStnClickedColor();
	CColorStatic m_ColorControl;
};
