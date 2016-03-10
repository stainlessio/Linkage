// LinkPropertiesDialog.cpp : implementation file
//

#include "stdafx.h"
#include "Linkage.h"
#include "LinkPropertiesDialog.h"


// CLinkPropertiesDialog dialog

IMPLEMENT_DYNAMIC(CLinkPropertiesDialog, CMyDialog)

CLinkPropertiesDialog::CLinkPropertiesDialog(CWnd* pParent /*=NULL*/)
	: CMyDialog( pParent, CLinkPropertiesDialog::IDD )
	, m_LineSize(1)
	, m_bSolid( FALSE )
	, m_bActuator(FALSE)
	, m_ActuatorCPM(0)
	, m_bAlwaysManual( FALSE )
	, m_SelectedLinkCount(0)
	, m_Name(_T(""))
	, m_ThrowDistance(0)
	, m_bIsGear( FALSE )
	, m_FastenedTo( _T( "" ) )
	, m_bLocked(FALSE)
{
	m_Color = RGB( 200, 200, 200 );
}

CLinkPropertiesDialog::~CLinkPropertiesDialog()
{
}

void CLinkPropertiesDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_LineSize);
	DDV_MinMaxInt(pDX, m_LineSize, 1, 4);
	DDX_Control(pDX, IDC_SPIN1, m_SpinControl);
	DDX_Check(pDX, IDC_CHECK1, m_bSolid);
	DDX_Check(pDX, IDC_CHECK2, m_bActuator);
	DDX_Control(pDX, IDC_CHECK2, m_ActuatorControl);
	DDX_Control(pDX, IDC_EDIT2, m_ActuatorCPMControl);
	DDX_Text(pDX, IDC_EDIT2, m_ActuatorCPM);
	DDX_Control(pDX, IDC_CHECK3, m_AlwaysManualCheckbox);
	DDX_Check(pDX, IDC_CHECK3, m_bAlwaysManual);
	DDX_Text(pDX, IDC_EDIT3, m_Name);
	DDX_Control(pDX, IDC_NAMEPROMPT, m_NamePromptControl);
	DDX_Control(pDX, IDC_EDIT3, m_NameControl);
	DDX_Control(pDX, IDC_EDIT5, m_ThrowDistanceControl);
	DDX_MyDoubleText(pDX, IDC_EDIT5, m_ThrowDistance, 4);
	DDV_MinMaxDouble(pDX, m_ThrowDistance, 0.0, DBL_MAX);
	DDX_Control(pDX, IDC_THROWDISTANCEPROMPT, m_ThrowDistancePrompt);
	DDX_Check(pDX, IDC_CHECK5, m_bIsGear);
	DDX_Control(pDX, IDC_FASTENEDTO, m_FastenedToControl);
	DDX_Text(pDX, IDC_FASTENEDTO, m_FastenedTo);
	DDX_Control(pDX, IDC_CHECK1, m_SolidControl);
	DDX_Control(pDX, IDC_COLORPROMPT, m_ColorPromptControl);
	DDX_Control(pDX, IDC_COLOR, m_ColorControl);
	DDX_Check(pDX, IDC_LOCKED, m_bLocked);
	DDX_Control(pDX, IDC_LOCKED, m_LockedControl);

	m_SpinControl.SetRange(1, 4);

	if (m_ConnectorCount == 2)
	{
		m_ActuatorControl.EnableWindow(TRUE);
		m_ActuatorCPMControl.EnableWindow(TRUE);
	}
	else
	{
		m_ActuatorControl.EnableWindow(FALSE);
		m_ActuatorCPMControl.EnableWindow(FALSE);
		m_ActuatorControl.SetCheck(0);
	}

	if (m_SelectedLinkCount > 1)
	{
		m_AlwaysManualCheckbox.EnableWindow(FALSE);
		m_ActuatorControl.EnableWindow(FALSE);
		m_ActuatorCPMControl.EnableWindow(FALSE);
		m_NamePromptControl.EnableWindow(FALSE);
		m_NameControl.EnableWindow(FALSE);
		m_ColorPromptControl.EnableWindow(FALSE);
		m_ColorControl.EnableWindow(FALSE);
		m_LockedControl.EnableWindow(FALSE);
	}

	if (m_bIsGear)
	{
		m_SolidControl.EnableWindow(FALSE);
		if (m_bFastened)
		{
			m_ColorPromptControl.EnableWindow(FALSE);
			m_ColorControl.EnableWindow(FALSE);
		}
	}

	if (pDX->m_bSaveAndValidate)
		m_Color = m_ColorControl.GetColor();
	else
		m_ColorControl.SetColor(m_Color);

	OnBnClickedCheck2();
}


BEGIN_MESSAGE_MAP(CLinkPropertiesDialog, CMyDialog)
	ON_BN_CLICKED(IDC_CHECK2, &CLinkPropertiesDialog::OnBnClickedCheck2)
	ON_STN_CLICKED( IDC_COLOR, &CLinkPropertiesDialog::OnStnClickedColor )
END_MESSAGE_MAP()


// CLinkPropertiesDialog message handlers




void CLinkPropertiesDialog::OnBnClickedCheck2()
{
	m_AlwaysManualCheckbox.EnableWindow( m_ActuatorControl.GetCheck() != 0 ? TRUE : FALSE );
	m_ActuatorCPMControl.EnableWindow( m_ActuatorControl.GetCheck() != 0 ? TRUE : FALSE );
	m_ThrowDistancePrompt.EnableWindow( m_ActuatorControl.GetCheck() != 0 ? TRUE : FALSE );
	m_ThrowDistanceControl.EnableWindow( m_ActuatorControl.GetCheck() != 0 ? TRUE : FALSE );
}


void CLinkPropertiesDialog::OnStnClickedColor()
{
	CColorDialog dlg;
	if( dlg.DoModal() == IDOK )
	{
		m_Color = dlg.GetColor();
		m_ColorControl.SetColor( m_Color ); 
	}
}
