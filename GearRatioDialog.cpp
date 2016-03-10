// GearRatioDialog.cpp : implementation file
//

#include "stdafx.h"
#include "linkage.h"
#include "GearRatioDialog.h"
#include "afxdialogex.h"


// CGearRatioDialog dialog

IMPLEMENT_DYNAMIC(CGearRatioDialog, CMyDialog)

CGearRatioDialog::CGearRatioDialog(CWnd* pParent /*=NULL*/)
	: CMyDialog( pParent, CGearRatioDialog::IDD)
	, m_Gear1Size( 0 )
	, m_Gear2Size( 0 )
, m_Link1Name( _T( "" ) )
, m_Link2Name( _T( "" ) )
, m_GearChainSelection( 0 )
, m_bUseRadiusValues( FALSE )
{

}

CGearRatioDialog::~CGearRatioDialog()
{
}

void CGearRatioDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange( pDX );
	DDX_Text( pDX, IDC_EDIT1, m_Gear1Size );
	DDX_Text( pDX, IDC_EDIT7, m_Gear2Size );
	DDX_Text( pDX, IDC_NAME1, m_Link1Name );
	DDX_Text( pDX, IDC_NAME2, m_Link2Name );
	DDX_Radio( pDX, IDC_RADIO1, m_GearChainSelection );
	DDX_Check( pDX, IDC_CHECK1, m_bUseRadiusValues );
	DDX_Control( pDX, IDC_RADIO1, m_GearRadioControl );
	DDX_Control( pDX, IDC_CHECK1, m_UseRadiiControl );

	OnBnClickedRadio();
}


BEGIN_MESSAGE_MAP(CGearRatioDialog, CMyDialog)
	ON_BN_CLICKED( IDC_RADIO1, &CGearRatioDialog::OnBnClickedRadio )
	ON_BN_CLICKED( IDC_RADIO2, &CGearRatioDialog::OnBnClickedRadio )
END_MESSAGE_MAP()


// CGearRatioDialog message handlers


void CGearRatioDialog::OnBnClickedRadio()
{
	m_UseRadiiControl.EnableWindow( m_GearRadioControl.GetCheck() == BST_CHECKED ? FALSE : TRUE );
}
