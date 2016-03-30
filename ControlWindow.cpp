// ControlWindow.cpp : implementation file
//

#include "stdafx.h"
#include "Linkage.h"
#include "ControlWindow.h"

static const int HANDLESIZE = 10;
static const int CONTROLHEIGHT = 18;
static const int BORDER = 6;
static const int MAXCONTROLWIDTH = 600;

class CControlWindowControl
{
	public:
	CControlWindowControl()
	{
		m_ControlID = 0;
		m_CommandID = 0;
		m_Position = 0.0;
		m_bCentered = false;
	}

	int m_ControlID;
	int m_CommandID;
	CString m_Description;
	double m_Position; // 0.0 to 1.0 or -1.0 to 1.0 if centered.
	bool m_bCentered; // true if the zero point is at the center of the control.
};


class CControlWindowImplementation
{
	public:
	CControlWindowImplementation()
	{
		m_pCaptureControl = 0;
		m_ControlCount = 0;
		m_ControlWidth = MAXCONTROLWIDTH;
		m_ControlHeight = CONTROLHEIGHT;
		m_Border = BORDER;
		m_HandleSize = HANDLESIZE;
		m_MaxTextWidth = 0;
		m_CaptureControl = -1;
		m_Font.CreateFont( -11, 0, 0, 0, FW_NORMAL, 0, 0, 0, 
							DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, 
							ANTIALIASED_QUALITY | CLEARTYPE_QUALITY, VARIABLE_PITCH | FF_SWISS,
							"arial" );

	}
	~CControlWindowImplementation() {}

	static const int m_MAXCONTROLS = 15;
	int m_ControlCount;

	CControlWindowControl m_Controls[m_MAXCONTROLS];
	
	CControlWindowControl *m_pCaptureControl;

	int m_ControlWidth;
	int m_ControlHeight;
	int m_Border;
	int m_HandleSize;
	int m_MaxTextWidth;
	CFont m_Font;
	CPoint m_CaptureOffset;
	int m_CaptureControl;

	int GetDesiredHeight( void )
	{
		return ( BORDER * 2 ) + ( m_ControlCount * CONTROLHEIGHT );
	}

	bool GetCaptureRange( CWnd *pWnd, int ControlIndex, CRect &Rect )
	{
		if( pWnd == 0 )
			return false;

		CRect ClientRect;
		pWnd->GetClientRect( &ClientRect );

		SetSizingValues( pWnd, ClientRect.Width(), ClientRect.Height() );

		Rect.left = ClientRect.left + ( ClientRect.Width() / 2 ) - ( ( m_ControlWidth + m_MaxTextWidth ) / 2 );
		Rect.left += m_MaxTextWidth;
		Rect.right = Rect.left + m_ControlWidth;

		Rect.top = m_Border + ( m_ControlHeight / 2 ) + ( m_ControlHeight * ControlIndex )	- ( m_HandleSize / 2 );
		Rect.bottom = Rect.top + m_HandleSize;

		return true;
	}

	void SetSizingValues( CWnd *pWnd, int cx, int cy )
	{
		int DesiredHeight = GetDesiredHeight();

		double ScaleFactor = (double)cy / (double)DesiredHeight;
		if( ScaleFactor > 1.0 )
			ScaleFactor = 1.0;

		m_ControlHeight = (int)( CONTROLHEIGHT * ScaleFactor );
		m_Border = (int)( BORDER * ScaleFactor );
		m_HandleSize = (int)( HANDLESIZE * ScaleFactor );

		m_MaxTextWidth = 0;
		if( pWnd != 0 )
		{
			CClientDC dc( pWnd );
			dc.SelectObject( &m_Font );
			for( int Counter = 0; Counter < m_ControlCount && Counter < m_MAXCONTROLS; ++Counter )
			{
				int cx = dc.GetTextExtent( m_Controls[Counter].m_Description ).cx;
				if( cx > m_MaxTextWidth )
					m_MaxTextWidth = cx;
			}
		}

		if( m_MaxTextWidth > 0 )
			m_MaxTextWidth += BORDER * 2;

		m_ControlWidth = cx - ( BORDER * 2 ) - m_MaxTextWidth;
		if( m_ControlWidth > MAXCONTROLWIDTH )
			m_ControlWidth  = MAXCONTROLWIDTH;
	}
};

// CControlWindow

IMPLEMENT_DYNAMIC(CControlWindow, CWnd)

CControlWindow::CControlWindow()
{
	m_pImplementation = new CControlWindowImplementation;
}

CControlWindow::~CControlWindow()
{
	if( m_pImplementation != 0 )
		delete m_pImplementation;
	m_pImplementation = 0;
}


BEGIN_MESSAGE_MAP(CControlWindow, CWnd)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()



// CControlWindow message handlers


void CControlWindow::OnLButtonDown(UINT nFlags, CPoint point)
{
	if( m_pImplementation == 0 )
		return;

	SetCapture();

	CRect Rect;
	for( int Counter = 0; Counter < m_pImplementation->m_ControlCount && Counter < m_pImplementation->m_MAXCONTROLS; ++Counter )
	{
		if( m_pImplementation->GetCaptureRange( this, Counter, Rect ) )
		{
			if( point.y >= Rect.top && point.y <= Rect.bottom )
			{
				int Location;
				if( m_pImplementation->m_Controls[Counter].m_bCentered)
					Location = Rect.left + ( Rect.Width() / 2 ) + (int)( ( Rect.Width() / 2 ) * m_pImplementation->m_Controls[Counter].m_Position );
				else
					Location = Rect.left + (int)( Rect.Width() * m_pImplementation->m_Controls[Counter].m_Position );
				if( point.x >= Location - m_pImplementation->m_HandleSize && point.x <= Location + m_pImplementation->m_HandleSize )
				{
					m_pImplementation->m_CaptureControl = Counter;
					m_pImplementation->m_CaptureOffset.SetPoint( Location - point.x, 0 );
					return;
				}
			}
		}
	}
}

void CControlWindow::OnLButtonUp(UINT nFlags, CPoint point)
{
	if( m_pImplementation == 0 )
		return;
	m_pImplementation->m_CaptureControl = -1;
	ReleaseCapture();
}


void CControlWindow::OnMouseMove(UINT nFlags, CPoint point)
{
	if( m_pImplementation == 0 )
		return;
	if( m_pImplementation->m_CaptureControl < 0 )
		return;

	CRect Rect;
	if( !m_pImplementation->GetCaptureRange( this, m_pImplementation->m_CaptureControl, Rect ) )
		return;

	double NewPosition;
	if( m_pImplementation->m_Controls[m_pImplementation->m_CaptureControl].m_bCentered )
	{
		int NewLocation = point.x + m_pImplementation->m_CaptureOffset.x - ( Rect.left + ( Rect.Width() / 2 ) );
		NewPosition = (double)NewLocation / (double)( Rect.Width() / 2 );
		if( NewPosition < -1.0 )
			NewPosition = -1.0;
	}
	else
	{
		int NewLocation = point.x + m_pImplementation->m_CaptureOffset.x - Rect.left;
		NewPosition = (double)NewLocation / (double)Rect.Width();
		if( NewPosition < 0.0 )
			NewPosition = 0.0;
	}
	if( NewPosition > 1.0 )
		NewPosition = 1.0;

	m_pImplementation->m_Controls[m_pImplementation->m_CaptureControl].m_Position = NewPosition;
	InvalidateRect( 0 );

	CWnd *pParent = GetParent();
	if( pParent == 0 )
		return;

	LONG MyID = GetWindowLong( GetSafeHwnd(), GWL_ID );

	WPARAM wParam = MAKEWPARAM( MyID, 0 );
	LPARAM lParam = (LPARAM)GetSafeHwnd();

	pParent->SendMessage( WM_COMMAND, wParam, lParam );
}

void CControlWindow::OnPaint()
{
	CPaintDC dc(this);
	if( m_pImplementation == 0 )
		return;

	CRect Rect;
	GetClientRect( &Rect );

    CDC MemoryDC;
	MemoryDC.CreateCompatibleDC( &dc );
	MemoryDC.SelectObject( &m_pImplementation->m_Font );
	MemoryDC.SetTextAlign( TA_TOP | TA_RIGHT );
	CBitmap Bitmap;
    Bitmap.CreateCompatibleBitmap( &dc, Rect.Width(), Rect.Height() );
	MemoryDC.SelectObject( &Bitmap );
	MemoryDC.PatBlt( 0, 0, Rect.Width(), Rect.Height(), WHITENESS );

	m_pImplementation->SetSizingValues( this, Rect.Width(), Rect.Height() );

	CPen BlackPen( PS_SOLID, 1, RGB( 0, 0, 0 ) );
	CBrush BlackBrush( RGB( 0, 0, 0 ) );
	CBrush WhiteBrush( RGB( 255, 255, 255 ) );

	int x = Rect.left + ( Rect.Width() / 2 ) - ( ( m_pImplementation->m_ControlWidth + m_pImplementation->m_MaxTextWidth ) / 2 );
	int x2 = x + m_pImplementation->m_MaxTextWidth;
	x = x2 - ( BORDER * 2 );
	int y = m_pImplementation->m_Border + ( m_pImplementation->m_ControlHeight / 2 );
	int HalfSize = m_pImplementation->m_HandleSize / 2;
	int Extra = 2;

	for( int Counter = 0; Counter < m_pImplementation->m_ControlCount && Counter < m_pImplementation->m_MAXCONTROLS; ++Counter )
	{
		MemoryDC.TextOut( x, y - HalfSize - 1, m_pImplementation->m_Controls[Counter].m_Description );

		MemoryDC.SelectObject( &BlackPen );
		MemoryDC.MoveTo( x2, y );
		MemoryDC.LineTo( x2 + m_pImplementation->m_ControlWidth, y );
		MemoryDC.MoveTo( x2, y - HalfSize - Extra );
		MemoryDC.LineTo( x2, y + HalfSize + Extra + 1 );
		MemoryDC.MoveTo( x2 + m_pImplementation->m_ControlWidth, y - HalfSize - Extra );
		MemoryDC.LineTo( x2 + m_pImplementation->m_ControlWidth, y + HalfSize + Extra + 1 );

		int xTemp;
		if( m_pImplementation->m_Controls[Counter].m_bCentered )
			xTemp = x2 + ( m_pImplementation->m_ControlWidth / 2 ) + (int)( ( m_pImplementation->m_ControlWidth / 2 ) * m_pImplementation->m_Controls[Counter].m_Position );
		else
			xTemp = x2 + (int)( m_pImplementation->m_ControlWidth * m_pImplementation->m_Controls[Counter].m_Position );
		int Offset = HalfSize;

		CRect Box( xTemp - Offset, y - Offset, xTemp + Offset + 1, y + Offset + 1 );
		MemoryDC.FrameRect( &Box, &BlackBrush );
		Box.InflateRect( -1, -1 );
		MemoryDC.FrameRect( &Box, &BlackBrush );
		Box.InflateRect( -1, -1 );
		MemoryDC.FillRect( &Box, &WhiteBrush );

		y += m_pImplementation->m_ControlHeight;
	}

	dc.BitBlt( 0, 0, Rect.Width(), Rect.Height(), &MemoryDC, 0, 0, SRCCOPY );
}

int CControlWindow::GetControlCount( void )
{
	if( m_pImplementation == 0 )
		return 0;
	return m_pImplementation->m_ControlCount;
}

int CControlWindow::GetControlId( int Index )
{
	if( m_pImplementation == 0 )
		return 0;
	
	return m_pImplementation->m_Controls[Index].m_ControlID;
}

double CControlWindow::GetControlPosition( int Index )
{
	if( m_pImplementation == 0 )
		return 0.0;

	return m_pImplementation->m_Controls[Index].m_Position;
}

void CControlWindow::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	if( m_pImplementation == 0 )
		return;

	m_pImplementation->SetSizingValues( this, cx, cy );
}

int CControlWindow::GetDesiredHeight( void )
{
	if( m_pImplementation == 0 )
		return 0;
	
	return m_pImplementation->GetDesiredHeight();
}

double CControlWindow::GetPosition( int ControlID )
{
	if( m_pImplementation == 0 )
		return 0.0;

	for( int Counter = 0; Counter < m_pImplementation->m_ControlCount && Counter < m_pImplementation->m_MAXCONTROLS; ++Counter )
	{
		if( m_pImplementation->m_Controls[Counter].m_ControlID == ControlID )
			return m_pImplementation->m_Controls[Counter].m_Position;
	}
	
	return 0.0;
}

void CControlWindow::UpdateControl( int ControlID, double Position )
{
	if( m_pImplementation == 0 )
		return;

	for( int Counter = 0; Counter < m_pImplementation->m_ControlCount && Counter < m_pImplementation->m_MAXCONTROLS; ++Counter )
	{
		if( m_pImplementation->m_Controls[Counter].m_ControlID == ControlID )
		{
			m_pImplementation->m_Controls[Counter].m_Position = Position;
			InvalidateRect( 0 );
			return;
		}
	}
}

void CControlWindow::AddControl( int ControlID, const char *pDescription, int CommandID, bool bCentered, double InitialPosition )
{
	if( m_pImplementation == 0 )
		return;

	if( m_pImplementation->m_ControlCount >= m_pImplementation->m_MAXCONTROLS )
		return;

	m_pImplementation->m_Controls[m_pImplementation->m_ControlCount].m_CommandID = CommandID;
	m_pImplementation->m_Controls[m_pImplementation->m_ControlCount].m_Description = pDescription;
	m_pImplementation->m_Controls[m_pImplementation->m_ControlCount].m_ControlID = ControlID;
	m_pImplementation->m_Controls[m_pImplementation->m_ControlCount].m_Position = InitialPosition;
	m_pImplementation->m_Controls[m_pImplementation->m_ControlCount].m_bCentered = bCentered;

	m_pImplementation->m_ControlCount++;
}


BOOL CControlWindow::OnEraseBkgnd( CDC* pDC )
{
	return FALSE;
}

void CControlWindow::Clear( void )
{
	if( m_pImplementation == 0 )
		return;

	m_pImplementation->m_ControlCount = 0;
}

