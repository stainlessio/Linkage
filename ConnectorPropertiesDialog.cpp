// RPMDialog.cpp : implementation file
//

#include "stdafx.h"
#include <float.h>
#include "Linkage.h"
#include "ConnectorPropertiesDialog.h"

void AFXAPI DDX_MyDoubleText( CDataExchange* pDX, int nIDC, double& value, int Precision );
void AFXAPI _MyAfxTextFloatFormat(CDataExchange* pDX, int nIDC,
                                  void* pData, double value, int nSizeGcvt,
                                  int nSizeType);

// CConnectorPropertiesDialog dialog

IMPLEMENT_DYNAMIC(CConnectorPropertiesDialog, CMyDialog)

CConnectorPropertiesDialog::CConnectorPropertiesDialog( CWnd* pParent /*=NULL*/ )
	: CMyDialog( pParent, CConnectorPropertiesDialog::IDD )
	, m_RPM(0.0)
	, m_Property(0)
	, m_bDrawing(false)
	, m_bAlwaysManual(false)
	, m_xPosition(0)
	, m_yPosition(0)
	, m_Name(_T(""))
	, m_GalleryControl( ID_POPUP_GALLERY, IDB_INSERT_POPUP_GALLERY, 48 )
	, m_SlideRadius(0)
	, m_bSlideRadiusPath(FALSE)

, m_FastenedTo( _T( "" ) )
{
	m_bAnchor = false;
	m_bInput = false;
	m_bIsSlider = false;
	m_Color = RGB( 200, 200, 200 );
}

CConnectorPropertiesDialog::~CConnectorPropertiesDialog()
{
}

void CConnectorPropertiesDialog::DoDataExchange(CDataExchange* pDX)
{
	CMyDialog::DoDataExchange( pDX );
	DDX_Text( pDX, IDC_EDIT1, m_RPM );
	DDX_Control( pDX, IDC_EDIT1, m_RPMControl );
	DDX_Control( pDX, IDC_RADIO5, m_RotatingAnchorControl );
	DDX_Control( pDX, IDC_CHECK2, m_AlwaysManualCheckbox );
	DDX_Control( pDX, IDC_CHECK1, m_DrawCheckControl );
	DDX_Control( pDX, IDC_RADIO1, m_ConnectorControl );
	DDX_Text( pDX, IDC_EDIT3, m_Name );
	DDX_Control( pDX, IDC_CHECK4, m_SlideRadiusCheck );
	DDX_Check( pDX, IDC_CHECK4, m_bSlideRadiusPath );
	DDX_Control( pDX, IDC_EDIT7, m_SlideRadiusControl );
	DDX_MyDoubleText( pDX, IDC_EDIT7, m_SlideRadius, 4 );
	DDX_Control( pDX, IDC_SLIDERADIUSMINPROMPT, m_SlideRadiusMinPrompt );
	DDX_Control( pDX, IDC_SLIDRADIUSMINVALUE, m_SlideRadiusMinControl );
	DDX_MyDoubleText( pDX, IDC_SLIDRADIUSMINVALUE, m_MinimumSlideRadius, 4 );
	DDX_Control( pDX, IDC_EDIT2, m_xControl );
	DDX_Control( pDX, IDC_EDIT4, m_yControl );
	DDX_Control( pDX, IDC_FASTENEDTO, m_FastenedToControl );
	DDX_Text( pDX, IDC_FASTENEDTO, m_FastenedTo );
	DDX_Control( pDX, IDC_COLOR, m_ColorControl );

	int CheckValue;
	if( pDX->m_bSaveAndValidate )
	{
		DDX_Radio( pDX, IDC_RADIO1, m_Property );
		DDX_Check( pDX, IDC_CHECK1, CheckValue );
		m_bDrawing = CheckValue != 0;
		DDX_Check( pDX, IDC_CHECK2, CheckValue );
		m_bAlwaysManual = CheckValue != 0;

		m_bAnchor = m_Property == 1 || m_Property == 2;
		m_bInput = m_Property == 2;

		if( !m_bSlideRadiusPath )
			m_SlideRadius = 0;

		if( !m_bIsSlider )
		{
			DDX_MyDoubleText( pDX, IDC_EDIT2, m_xPosition, 4 );
			DDX_MyDoubleText( pDX, IDC_EDIT4, m_yPosition, 4 );
		}
	}
	else
	{
		DDX_MyDoubleText( pDX, IDC_EDIT2, m_xPosition, 4 );
		DDX_MyDoubleText( pDX, IDC_EDIT4, m_yPosition, 4 );

		CheckValue = m_bDrawing ? 1 : 0;
		DDX_Check( pDX, IDC_CHECK1, CheckValue );
		CheckValue = m_bAlwaysManual ? 1 : 0;
		DDX_Check( pDX, IDC_CHECK2, CheckValue );

		m_Property = 0;
		if( m_bAnchor && m_bInput )
			m_Property = 2;
		else if( m_bAnchor )
			m_Property = 1;
		else
			m_Property = 0;
		DDX_Radio( pDX, IDC_RADIO1, m_Property );

		m_SlideRadiusCheck.EnableWindow( m_bIsSlider );
		m_SlideRadiusCheck.SetCheck( m_bIsSlider && m_SlideRadius != 0 );
		m_SlideRadiusMinPrompt.EnableWindow( ( m_bIsSlider && m_SlideRadius != 0 ) ? TRUE : FALSE );
		m_SlideRadiusMinControl.EnableWindow( ( m_bIsSlider && m_SlideRadius != 0 ) ? TRUE : FALSE );
		m_xControl.EnableWindow( !m_bIsSlider );
		m_yControl.EnableWindow( !m_bIsSlider );
	}
	OnBnClickedRadio();
	OnBnClickedCheck4();

	if( pDX->m_bSaveAndValidate )
		m_Color = m_ColorControl.GetColor();
	else
		m_ColorControl.SetColor( m_Color );
}


BEGIN_MESSAGE_MAP(CConnectorPropertiesDialog, CMyDialog)
	ON_BN_CLICKED(IDC_RADIO1, &CConnectorPropertiesDialog::OnBnClickedRadio)
	ON_BN_CLICKED(IDC_RADIO2, &CConnectorPropertiesDialog::OnBnClickedRadio)
	ON_BN_CLICKED(IDC_RADIO3, &CConnectorPropertiesDialog::OnBnClickedRadio)
	ON_BN_CLICKED(IDC_RADIO4, &CConnectorPropertiesDialog::OnBnClickedRadio)
	ON_BN_CLICKED(IDC_RADIO5, &CConnectorPropertiesDialog::OnBnClickedRadio)
	ON_BN_CLICKED(IDC_CHECK4, &CConnectorPropertiesDialog::OnBnClickedCheck4)
	ON_STN_CLICKED( IDC_COLOR, &CConnectorPropertiesDialog::OnStnClickedColor )
END_MESSAGE_MAP()


// CConnectorPropertiesDialog message handlers

void CConnectorPropertiesDialog::OnBnClickedRadio()
{
	m_RPMControl.EnableWindow( m_RotatingAnchorControl.GetCheck() != 0 ? TRUE : FALSE );
	m_AlwaysManualCheckbox.EnableWindow( m_RotatingAnchorControl.GetCheck() != 0 ? TRUE : FALSE );
	m_DrawCheckControl.EnableWindow( m_ConnectorControl.GetCheck() != 0 ? TRUE : FALSE );
}

BOOL CConnectorPropertiesDialog::OnInitDialog()
{
	CMyDialog::OnInitDialog();
	
	return TRUE;
}



void CConnectorPropertiesDialog::OnBnClickedCheck4()
{
	m_SlideRadiusControl.EnableWindow( ( m_bIsSlider && m_SlideRadiusCheck.GetCheck() != 0 ) ? TRUE : FALSE );
}

void CConnectorPropertiesDialog::OnStnClickedColor()
{
	CColorDialog dlg;
	if( dlg.DoModal() == IDOK )
	{
		m_Color = dlg.GetColor();
		m_ColorControl.SetColor( m_Color ); 
	}
}
