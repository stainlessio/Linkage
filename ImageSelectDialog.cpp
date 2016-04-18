// ImageSelectDialog.cpp : implementation file
//

#include "stdafx.h"
#include "Linkage.h"
#include "ImageSelectDialog.h"

// CImageSelectDialog dialog

class CImageSelectDialogImplementation
{
	public:
	CImageSelectDialogImplementation()
	{
		m_SelectionArea.SetRect( 0, 0, 0, 0 );
	}
	~CImageSelectDialogImplementation() {}

	CRect m_SelectionArea;

	// need selection number, bitmap pairs storage, height adjustment value, ...
};

IMPLEMENT_DYNAMIC(CImageSelectDialog, CDialog)

CImageSelectDialog::CImageSelectDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CImageSelectDialog::IDD, pParent)
{
	m_pImpelementation = new CImageSelectDialogImplementation;
}

CImageSelectDialog::~CImageSelectDialog()
{
	if( m_pImpelementation != 0 )
	{
		delete m_pImpelementation;
		m_pImpelementation = 0;
	}
}

void CImageSelectDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CImageSelectDialog, CDialog)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

// CImageSelectDialog message handlers

BOOL CImageSelectDialog::OnEraseBkgnd(CDC* pDC)
{
	BOOL bResult = CDialog::OnEraseBkgnd( pDC );

	if( m_pImpelementation == 0 )
		return bResult;

	CRect ClientRect;
	GetClientRect( &ClientRect );

	int Width = max( ClientRect.Width(), m_pImpelementation->m_SelectionArea.Width() );

	pDC->PatBlt( ClientRect.left, ClientRect.top, Width, m_pImpelementation->m_SelectionArea.Height(), WHITENESS );

	return bResult;
}

BOOL CImageSelectDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	if( m_pImpelementation == 0 )
		return TRUE;

	/*
	 * Adjust the size of the window to allow for all of the bitmaps
	 * to be drawn fully with appropriate spacing.
	 */

	m_pImpelementation->m_SelectionArea.SetRect( 0, 0, 150, 150 );

	static const int BUTTONMARGIN = 6;
	static const int BUTTONMARGIN2 = 12;

	CRect ButtonRect( 0, 0, 0, 0 );
	int xMin = 0;
	int yMin = 0;

	CWnd *pCancelButton = GetDlgItem( IDCANCEL );
	if( pCancelButton != 0 )
	{
		pCancelButton->GetWindowRect( &ButtonRect );
		xMin = ButtonRect.Width() + BUTTONMARGIN2;
		yMin = ButtonRect.Height() + BUTTONMARGIN2;
	}

	CRect ClientRect;
	GetClientRect( &ClientRect );
	CRect NewClientRect( 0, 0, 0, 0 );

	NewClientRect.right = max( m_pImpelementation->m_SelectionArea.Width(), xMin );
	NewClientRect.bottom = max( m_pImpelementation->m_SelectionArea.Height() + ButtonRect.Height() + BUTTONMARGIN2, yMin );

	int dx = NewClientRect.Width() - ClientRect.Width();
	int dy = NewClientRect.Height() - ClientRect.Height();

	CRect WindowRect;
	GetWindowRect( &WindowRect );

	SetWindowPos( 0, 0, 0, WindowRect.Width() + dx, WindowRect.Height() + dy, SWP_NOMOVE | SWP_NOZORDER | SWP_NOREDRAW );
	GetClientRect( &ClientRect );

	if( pCancelButton != 0 )
		pCancelButton->SetWindowPos( 0, NewClientRect.right - ( ButtonRect.Width() + BUTTONMARGIN ), NewClientRect.bottom - ( ButtonRect.Height() + BUTTONMARGIN ), ButtonRect.Width(), ButtonRect.Height(), SWP_NOZORDER | SWP_NOREDRAW );

	CenterWindow();

	return TRUE;
}
