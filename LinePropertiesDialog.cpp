// LinkPropertiesDialog.cpp : implementation file
//

#include "stdafx.h"
#include "Linkage.h"
#include "LinePropertiesDialog.h"

// CLinePropertiesDialog dialog

IMPLEMENT_DYNAMIC(CLinePropertiesDialog, CMyDialog)

CLinePropertiesDialog::CLinePropertiesDialog(CWnd* pParent /*=NULL*/)
	: CMyDialog( pParent, CLinePropertiesDialog::IDD )
	, m_LineSize(1)
	, m_SelectedLinkCount(0)
	, m_Name(_T(""))
	, m_bMeasurementLine(FALSE)
	, m_FastenTo( _T( "" ) )
{
	m_Color = RGB( 200, 200, 200 );
}

CLinePropertiesDialog::~CLinePropertiesDialog()
{
}

void CLinePropertiesDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange( pDX );
	DDX_Text( pDX, IDC_EDIT1, m_LineSize );
	DDV_MinMaxInt( pDX, m_LineSize, 1, 4 );
	DDX_Control( pDX, IDC_SPIN1, m_SpinControl );
	DDX_Text( pDX, IDC_EDIT3, m_Name );
	DDX_Control( pDX, IDC_NAMEPROMPT, m_NamePromptControl );
	DDX_Control( pDX, IDC_EDIT3, m_NameControl );
	DDX_Check( pDX, IDC_CHECK1, m_bMeasurementLine );
	DDX_Control( pDX, IDC_CHECK1, m_MeasurementLineControl );
	DDX_Control( pDX, IDC_RPMPROMPT, m_LineSizeControl );
	DDX_Control( pDX, IDC_EDIT1, m_LineSizeInputControl );

	m_SpinControl.SetRange( 1, 4 );

	if( m_SelectedLinkCount > 1 )
	{
		m_NamePromptControl.EnableWindow( FALSE );
		m_NameControl.EnableWindow( FALSE );
		m_MeasurementLineControl.EnableWindow( FALSE );
	}
	DDX_Control( pDX, IDC_FASTENEDTO, m_FastenToControl );
	DDX_Text( pDX, IDC_FASTENEDTO, m_FastenTo );
	DDX_Control( pDX, IDC_COLOR, m_ColorControl );

	if( pDX->m_bSaveAndValidate )
		m_Color = m_ColorControl.GetColor();
	else
		m_ColorControl.SetColor( m_Color );
}


BEGIN_MESSAGE_MAP(CLinePropertiesDialog, CMyDialog)
	ON_BN_CLICKED(IDC_CHECK1, &CLinePropertiesDialog::OnBnClickedCheck1)
	ON_STN_CLICKED( IDC_COLOR, &CLinePropertiesDialog::OnStnClickedColor )
END_MESSAGE_MAP()


// CLinePropertiesDialog message handlers



void CLinePropertiesDialog::OnBnClickedCheck1()
{
	bool bUnchecked = m_MeasurementLineControl.GetCheck() == BST_UNCHECKED;
	m_LineSizeControl.EnableWindow( bUnchecked );
	m_LineSizeInputControl.EnableWindow( bUnchecked );
	m_SpinControl.EnableWindow( bUnchecked );
}


void CLinePropertiesDialog::OnStnClickedColor()
{
	CColorDialog dlg;
	if( dlg.DoModal() == IDOK )
	{
		m_Color = dlg.GetColor();
		m_ColorControl.SetColor( m_Color ); 
	}
}
