// ExportImageSettingsDialog.cpp : implementation file
//

#include "stdafx.h"
#include "linkage.h"
#include "linkageView.h"
#include "ExportImageSettingsDialog.h"
#include "afxdialogex.h"


// CExportImageSettingsDialog dialog

IMPLEMENT_DYNAMIC(CExportImageSettingsDialog, CDialog)

CExportImageSettingsDialog::CExportImageSettingsDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CExportImageSettingsDialog::IDD, pParent)
	, m_ResolutionSelection( 0 )
	, m_ScaleFactor( 3 )
	, m_Margin( 5 )
{
	m_pLinkageView = 0;
	m_bReadyToShowImage = false;
}

CExportImageSettingsDialog::~CExportImageSettingsDialog()
{
}

void CExportImageSettingsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange( pDX );

	DDX_Radio( pDX, IDC_RADIO1, m_ResolutionSelection );
	DDX_Control( pDX, IDC_SPIN1, m_SpinControl );
	DDX_Text( pDX, IDC_EDIT1, m_ScaleFactor );
	DDX_Control( pDX, IDC_PREVIEW, m_PreviewArea );
	DDX_Text( pDX, IDC_EDIT7, m_Margin );
	DDX_Control( pDX, IDC_SPIN2, m_MarginSpinControl );

	m_SpinControl.SetRange( 1, 50 );
	m_MarginSpinControl.SetRange( 0, 50 );
}


BEGIN_MESSAGE_MAP(CExportImageSettingsDialog, CDialog)
	ON_BN_CLICKED( IDC_RADIO1, &CExportImageSettingsDialog::OnBnClickedResolution )
	ON_BN_CLICKED( IDC_RADIO3, &CExportImageSettingsDialog::OnBnClickedResolution )
	ON_BN_CLICKED( IDC_RADIO2, &CExportImageSettingsDialog::OnBnClickedResolution )
	ON_BN_CLICKED( IDC_RADIO7, &CExportImageSettingsDialog::OnBnClickedResolution )
	ON_EN_CHANGE( IDC_EDIT1, &CExportImageSettingsDialog::OnEnChangeScaleFactor )
	ON_WM_PAINT()
	ON_BN_CLICKED( IDC_BUTTON1, &CExportImageSettingsDialog::OnCopy )
	ON_EN_CHANGE( IDC_EDIT7, &CExportImageSettingsDialog::OnEnChangeEdit7 )
END_MESSAGE_MAP()


// CExportImageSettingsDialog message handlers


void CExportImageSettingsDialog::OnBnClickedResolution()
{
	UpdateImage();
}


void CExportImageSettingsDialog::OnEnChangeScaleFactor()
{
	UpdateImage();
}

void CExportImageSettingsDialog::UpdateImage( void )
{
	if( !m_bReadyToShowImage )
		return;

	if( m_pLinkageView == 0 )
		return;

	CDataExchange DX( this, TRUE );
	DoDataExchange( &DX );

	CClientDC dc( this );

	int RenderWidth = GetResolutionWidth();
	int RenderHeight = GetResolutionHeight();
	double ResolutionScale = GetResolutionScale();
	double MarginFactor = GetMarginScale();

	if( RenderWidth > 0 && RenderHeight > 0 )
		m_pLinkageView->DisplayAsImage( &dc, m_DrawArea.left, m_DrawArea.top, m_DrawArea.Width(), m_DrawArea.Height(), RenderWidth, RenderHeight, ResolutionScale, MarginFactor, ::GetSysColor( COLOR_3DFACE ), true );
}

BOOL CExportImageSettingsDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	CRect Rect;
	m_PreviewArea.GetWindowRect( &Rect );
	ScreenToClient( &Rect );
	m_PreviewArea.ShowWindow( SW_HIDE );

	m_DrawArea = Rect;

	m_bReadyToShowImage = true;
	InvalidateRect( &m_DrawArea );

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CExportImageSettingsDialog::OnPaint()
{
	CPaintDC dc( this );
	UpdateImage();
}

int CExportImageSettingsDialog::GetResolutionWidth( void )
{
	switch( m_ResolutionSelection )
	{
		case 0:
			return 1920;
		case 1:
			return 960;
		case 2:
			return 640;
		case 3:
			return 100;
	}
	return 0;
}

int CExportImageSettingsDialog::GetResolutionHeight( void )
{
	switch( m_ResolutionSelection )
	{
		case 0:
			return 1200;
		case 1:
			return 600;
		case 2:
			return 400;
		case 3:
			return 100;
	}
	return 0;
}

double CExportImageSettingsDialog::GetResolutionScale( void )
{
	double UseAdjustment = 1.0 / ( 1 + ( ( m_ScaleFactor - 1 ) * .25 ) );
	if( UseAdjustment == 0 )
		UseAdjustment = 1;
	return UseAdjustment;
}

double CExportImageSettingsDialog::GetMarginScale( void )
{
	return m_Margin * .01;
}

void CExportImageSettingsDialog::OnCopy()
{
	if( !m_bReadyToShowImage )
		return;

	if( m_pLinkageView == 0 )
		return;

	CDataExchange DX( this, TRUE );
	DoDataExchange( &DX );

	int RenderWidth = GetResolutionWidth();
	int RenderHeight = GetResolutionHeight();
	double ResolutionScale = GetResolutionScale();
	double MarginFactor = GetMarginScale();

	if( RenderWidth == 0 || RenderHeight == 0 )
		return;

	CBitmap bitmap;
	CClientDC dc( this );
	CDC memDC;

	memDC.CreateCompatibleDC( &dc ); 
	bitmap.CreateCompatibleBitmap( &dc, RenderWidth, RenderHeight );
	CBitmap* pOldBitmap = memDC.SelectObject( &bitmap );

	m_pLinkageView->DisplayAsImage( &memDC, 0, 0, RenderWidth, RenderHeight, RenderWidth, RenderHeight, ResolutionScale, MarginFactor, ::GetSysColor( COLOR_3DFACE ), false );

	::OpenClipboard( ::AfxGetMainWnd()->GetSafeHwnd() );
	::EmptyClipboard() ;
	::SetClipboardData (CF_BITMAP, bitmap.GetSafeHandle() ) ;
	::CloseClipboard () ;
	bitmap.Detach();

	memDC.SelectObject( pOldBitmap );
}


void CExportImageSettingsDialog::OnEnChangeEdit7()
{
	UpdateImage();
}
