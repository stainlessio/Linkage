// PopupGallery.cpp : implementation file
//

#include "stdafx.h"
#include "Linkage.h"
#include "PopupGallery.h"
#include "math.h"

IMPLEMENT_DYNAMIC(CPopupGallery, CWnd)

class CPopupGalleryImplementation
{
	public:
	virtual ~CPopupGalleryImplementation()
	{
		if( m_pToolDescriptions != 0 )
			delete [] m_pToolDescriptions;
		if( m_pToolTitles != 0 )
			delete [] m_pToolTitles;
	}

	int m_CommandID;
	CMFCToolBarImages m_Images;
	int m_Columns;
	int m_Rows;
	CSize m_ItemSize;
	CWnd *m_pParent;
	bool m_bIsWindow;
	bool m_bTrackingMouseLeave;
	int m_SelectedItem;
	int m_Count;
	static const int ITEMBORDER = 4; 
	static const int BORDER = 2;
	static const int TEXT_BORDER = 10;
	CMFCToolTipCtrl m_ToolTip;
	CString *m_pToolDescriptions;
	CString *m_pToolTitles;
	CFont m_TitleFont;
	CFont m_DescriptionFont;

	void AnyButtonDown( UINT nFlags, CPoint point );
	void AnyButtonUp( CPopupGallery *pTheWindow, UINT nFlags, CPoint point );
	bool GetItemRect( int Index, CRect &Rect );
	int GetItemFromPoint( CPoint point );
	void PaintSelection( CDC *pDC, CRect Rect );
	void DrawGradient( CDC *pDC, int x1, int y1, int x2, int y2, COLORREF Start, COLORREF End, bool bVertical );
	void DrawGradient( CDC *pDC, CRect &Rect, COLORREF Start, COLORREF End, bool bVertical );
	void DrawLine( CDC *pDC, int x1, int y1, int x2, int y2, COLORREF Color );
	void DrawLine( CDC *pDC, int x1, int y1, int x2, int y2, COLORREF Start, COLORREF End );
	void GetGridSize( int &UseColumns, int &UseRows );
	void ComputeItemSize( void );
};

CPopupGallery::CPopupGallery( int CommandID, int ImageStripResourceID, int ImageWidth )
{	
	m_pImplementation = new CPopupGalleryImplementation;
	if( m_pImplementation == 0 )
		return;

	m_pImplementation->m_CommandID = CommandID;
	m_pImplementation->m_pParent = 0;
	m_pImplementation->m_bIsWindow = false;
	m_pImplementation->m_Columns = -1;
	m_pImplementation->m_Rows = 0;
	m_pImplementation->m_bTrackingMouseLeave = false;
	m_pImplementation->m_SelectedItem = -1;
	m_pImplementation->m_Count = 0;

	m_pImplementation->m_ToolTip.Create( this );
	m_pImplementation->m_ToolTip.Activate( TRUE );

	if( ImageStripResourceID == 0 )
		return;

	m_pImplementation->m_Images.Load( ImageStripResourceID, NULL, TRUE );

	BITMAP bmp;
	GetObject( m_pImplementation->m_Images.GetImageWell(), sizeof(BITMAP), &bmp );
	if( ImageWidth <= 0 )
		ImageWidth = bmp.bmHeight;
	m_pImplementation->m_Images.SetImageSize( CSize( ImageWidth, bmp.bmHeight ), TRUE );

	m_pImplementation->m_Count = m_pImplementation->m_Images.GetCount();

	m_pImplementation->m_pToolDescriptions = new CString[m_pImplementation->m_Count];
	m_pImplementation->m_pToolTitles = new CString[m_pImplementation->m_Count];

	m_pImplementation->m_TitleFont.CreateFont( -11, 0, 0, 0, FW_EXTRABOLD, 0, 0, 0, 0, 0, 0, ANTIALIASED_QUALITY, 0, "arial" );
	m_pImplementation->m_DescriptionFont.CreateFont( -11, 0, 0, 0, FW_NORMAL, 0, 0, 0, 0, 0, 0, ANTIALIASED_QUALITY, 0, "arial" );
}

CPopupGallery::~CPopupGallery()
{
	if( m_pImplementation != 0 )
		delete m_pImplementation;
	m_pImplementation = 0;
}

int CPopupGallery::GetSelection( void )
{
	if( m_pImplementation == 0 )
		return -1;
	return m_pImplementation->m_SelectedItem;
}

void CPopupGallery::SetColumns( int Columns )
{
	if( m_pImplementation == 0 )
		return;

	if( Columns < 1 )
		Columns = 1;

	m_pImplementation->m_Columns = Columns;
	m_pImplementation->m_Rows = 0;
}

void CPopupGallery::SetRows( int Rows )
{
	if( m_pImplementation == 0 )
		return;

	if( Rows < 1 )
		Rows = 1;

	m_pImplementation->m_Columns = 0;
	m_pImplementation->m_Rows = Rows;
}

void CPopupGallery::SetTooltip( int Index, UINT ResourceID )
{
	CString String;
	String.LoadString( AfxGetInstanceHandle(), ResourceID );
	SetTooltip( Index, String );
}

void CPopupGallery::SetTooltip( int Index, const char *pString )
{
	if( m_pImplementation == 0 )
		return;

	CString Description = pString;
	CString Title;
	const char *pNewline = strchr( pString, '\n' );
	if( pNewline != 0 )
	{
		Description.Truncate( pNewline - pString );
		Title = pNewline + 1;
	}
	
	if( Index < m_pImplementation->m_Count )
	{
		m_pImplementation->m_pToolTitles[Index] = Title;
		m_pImplementation->m_pToolDescriptions[Index] = Description;
	}
}

void CPopupGalleryImplementation::GetGridSize( int &UseColumns, int &UseRows )
{
	UseColumns = 0;
	UseRows = 0;

	if( m_Rows > 0 )
	{
		UseRows = min( m_Rows, m_Count );
		UseColumns = ( m_Count + UseRows - 1 ) / UseRows;
	}
	else
	{
		if( m_Columns <= 0 )
		{
			double Temp = sqrt( (double)m_Count );
			UseColumns = (int)floor( Temp + 1 );
		}
		else
			UseColumns = min( m_Columns, m_Count );

		UseColumns = min( UseColumns, m_Count );
		UseRows = ( m_Count + UseColumns - 1 ) / UseColumns;
	}
}

void CPopupGallery::ShowGallery( CWnd *pParentWnd, int xScreen, int yScreen, bool bAddToExisting )
{
	if( m_pImplementation == 0 )
		return;

	m_pImplementation->ComputeItemSize();

	m_pImplementation->m_bTrackingMouseLeave = false;
	m_pImplementation->m_SelectedItem = -1;

	m_pImplementation->m_pParent = pParentWnd;

	if( !CreateEx( WS_EX_TOOLWINDOW, AfxRegisterWndClass( CS_DROPSHADOW | CS_HREDRAW | CS_OWNDC | CS_SAVEBITS | CS_VREDRAW, 0, 0 ), "Popup Gallery", WS_POPUP, CRect( 0, 0, 10, 10 ), 0, 0 ) )
		return;

	CMFCToolTipInfo params;
	params.m_bVislManagerTheme = TRUE;
	params.m_bBoldLabel = TRUE;
	params.m_bDrawDescription = TRUE;
	params.m_bDrawSeparator = TRUE;
	m_pImplementation->m_ToolTip.SetParams( &params );
	m_pImplementation->m_ToolTip.SetDelayTime( 600 );

	m_pImplementation->m_bIsWindow = true;

	int UseColumns = 0;
	int UseRows = 0;

	m_pImplementation->GetGridSize( UseColumns, UseRows );

	CRect Rect;
	m_pImplementation->GetItemRect( 0, Rect );
	int cx = ( m_pImplementation->BORDER * 2 ) + ( UseColumns * Rect.Width() );
	int cy = ( m_pImplementation->BORDER * 2 ) + ( UseRows * Rect.Height() );

	// This class handles the non-client area so adjust for it here.
	cx += 4;
	cy += 4;

	CRect WorkArea;
	SystemParametersInfo( SPI_GETWORKAREA, 0, &WorkArea, 0 );

	CPoint Point( xScreen, yScreen );

	HMONITOR hMonitor = MonitorFromPoint( Point, MONITOR_DEFAULTTONEAREST );
	if( hMonitor != INVALID_HANDLE_VALUE )
	{
		MONITORINFO MonitorInfo;
		MonitorInfo.cbSize = sizeof ( MONITORINFO );
		if( GetMonitorInfo( hMonitor, &MonitorInfo ) )
			WorkArea = MonitorInfo.rcWork;
	}

	if( xScreen + cx + 3 >= WorkArea.right )
		xScreen -= xScreen + cx + 3 - WorkArea.right;
	if( yScreen + cy + 3 >= WorkArea.bottom )
		yScreen -= yScreen + cy + 3 - WorkArea.bottom;
	if( xScreen < WorkArea.left )
		xScreen = WorkArea.left;
	if( yScreen < WorkArea.top )
		yScreen = WorkArea.top;

	SetWindowPos( 0, xScreen, yScreen, cx, cy, SWP_NOZORDER );

	for( int Index = 0; Index < m_pImplementation->m_Count; ++Index )
	{
		m_pImplementation->GetItemRect( Index, Rect );
		m_pImplementation->m_ToolTip.AddTool( this, m_pImplementation->m_pToolTitles[Index], &Rect, 1000 + Index );
	}

	ShowWindow( SW_RESTORE );
}

void CPopupGallery::HideGallery( void )
{
	if( m_pImplementation == 0 )
		return;

	AfxGetMainWnd()->EnableWindow( TRUE );

	ShowWindow( SW_HIDE );
	m_pImplementation->m_bIsWindow = false;
	DestroyWindow();
}

BEGIN_MESSAGE_MAP(CPopupGallery, CWnd)
	ON_WM_PAINT()
	ON_WM_KILLFOCUS()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_CHAR()
	ON_WM_NCCALCSIZE()
	ON_WM_NCPAINT()
END_MESSAGE_MAP()

bool CPopupGalleryImplementation::GetItemRect( int Index, CRect &Rect )
{	
	if( Index >= m_Count )
		return false;

	int UseColumns = 0;
	int UseRows = 0;
	GetGridSize( UseColumns, UseRows );

	int Row = Index / UseColumns;
	int Column = Index % UseColumns;

	int cx = m_ItemSize.cx + ( ITEMBORDER * 2 );
	int cy = m_ItemSize.cy + ( ITEMBORDER * 2 );

	int x = BORDER + ( Column * cx );
	int y = BORDER + ( Row * cy );

	Rect.SetRect( x, y, x + cx, y + cy );

	return true;		
}


_inline void COLORREF_TO_COLOR16( COLORREF r, TRIVERTEX &v ) { v.Red = ( (int)GetRValue( r ) ) << 8; v.Green = ( (int)GetGValue( r ) ) << 8; v.Blue = ( (int)GetBValue( r ) ) << 8; v.Alpha = 0; }

void CPopupGalleryImplementation::DrawGradient( CDC *pDC, int x1, int y1, int x2, int y2, COLORREF Start, COLORREF End, bool bVertical )
{
	CRect Rect( x1, y1, x2, y2 );
	DrawGradient( pDC, Rect, Start, End, bVertical );
}

void CPopupGalleryImplementation::ComputeItemSize( void )
{
	m_ItemSize.cx = m_Images.GetImageSize().cx;
	m_ItemSize.cy = m_Images.GetImageSize().cy;

	if( m_Columns == 1 )
	{
		CWindowDC dc( 0 );
		int TextWidth = 0;

		for( int Index = 0; Index < m_Count; ++Index )
		{
			dc.SelectObject( &m_TitleFont );
			TextWidth = max( TextWidth, dc.GetTextExtent( m_pToolTitles[Index] ).cx );
			dc.SelectObject( &m_DescriptionFont );
			TextWidth = max( TextWidth, dc.GetTextExtent( m_pToolDescriptions[Index] ).cx );
		}

		if( TextWidth > 0 )
			m_ItemSize.cx += TextWidth + ( TEXT_BORDER * 2 );
	}
}

void CPopupGalleryImplementation::DrawGradient( CDC *pDC, CRect &Rect, COLORREF Start, COLORREF End, bool bVertical )
{
	TRIVERTEX vert[2];
	vert[0].x = Rect.left;
	vert[0].y = Rect.top;
	vert[1].x = Rect.right;
	vert[1].y = Rect.bottom;

	COLORREF_TO_COLOR16( Start, vert[0] );
	COLORREF_TO_COLOR16( End, vert[1] );

	GRADIENT_RECT gRect;

	gRect.UpperLeft = 0;
	gRect.LowerRight = 1;

	pDC->GradientFill( vert, 2, &gRect, 1, bVertical ? GRADIENT_FILL_RECT_V : GRADIENT_FILL_RECT_H );
}

void CPopupGalleryImplementation::DrawLine( CDC *pDC, int x1, int y1, int x2, int y2, COLORREF Color )
{
	CPen Pen( PS_SOLID, 1, Color );
	CPen *pOldPen = pDC->SelectObject( &Pen );
	pDC->MoveTo( x1, y1 );
	pDC->LineTo( x2, y2 );
	pDC->SelectObject( pOldPen );
}

void CPopupGalleryImplementation::DrawLine( CDC *pDC, int x1, int y1, int x2, int y2, COLORREF Start, COLORREF End )
{
	CRect Rect( x1, y1, x2 + 1, y2 + 1 );
	DrawGradient( pDC, Rect, Start, End, x1 == x2 );
}

void CPopupGalleryImplementation::PaintSelection( CDC *pDC, CRect Rect )
{
	DrawLine( pDC, Rect.left + 1, Rect.top, Rect.right - 1, Rect.top, RGB( 221, 207, 155 ) );
	DrawLine( pDC, Rect.left + 1, Rect.bottom - 1, Rect.right - 1, Rect.bottom - 1, RGB( 219, 211, 184 ) );
	CRect Rect2( Rect );
	Rect.top++;
	Rect.bottom--;
	int Middle = Rect.Height() / 2;
	DrawGradient( pDC, Rect.left, Rect.top, Rect.left + 1, Rect.top + Middle, RGB( 220, 206, 154 ), RGB( 192, 167, 118 ), true );
	DrawGradient( pDC, Rect.left, Rect.top + Rect.Height() / 2, Rect.left + 1, Rect.bottom, RGB( 192, 167, 118 ), RGB( 220, 206, 154 ), true );
	DrawGradient( pDC, Rect.right, Rect.top, Rect.right - 1, Rect.top + Middle, RGB( 220, 206, 154 ), RGB( 192, 167, 118 ), true );
	DrawGradient( pDC, Rect.right, Rect.top + Rect.Height() / 2, Rect.right - 1, Rect.bottom, RGB( 192, 167, 118 ), RGB( 220, 206, 154 ), true );
	Rect.left++;
	Rect.right--;
	DrawLine( pDC, Rect.left + 1, Rect.top, Rect.right - 1, Rect.top, RGB( 255, 255, 247 ) );
	DrawGradient( pDC, Rect.left, Rect.top + 1, Rect.left + 1, Rect.bottom - 1, RGB( 255, 255, 247 ), RGB( 255, 245, 91 ), true );
	DrawGradient( pDC, Rect.right - 1, Rect.top + 1, Rect.right, Rect.bottom - 1, RGB( 255, 255, 247 ), RGB( 255, 245, 91 ), true );
	DrawLine( pDC, Rect.left + 1, Rect.top, Rect.right - 1, Rect.top, RGB( 255, 255, 247 ) );
	DrawGradient( pDC, Rect.left, Rect.bottom - 1, Rect.left + ( Rect.Width() / 2 ), Rect.bottom, RGB( 255, 245, 91 ), RGB( 255, 255, 247 ), false );
	DrawGradient( pDC, Rect.left + ( Rect.Width() / 2 ), Rect.bottom - 1, Rect.right, Rect.bottom, RGB( 255, 255, 247 ), RGB( 255, 245, 91 ), false );
	pDC->SetPixel( Rect.left, Rect.top, RGB( 234, 226, 168 ) );
	pDC->SetPixel( Rect.right - 1, Rect.top, RGB( 234, 226, 168 ) );
	pDC->SetPixel( Rect.left, Rect.bottom - 1, RGB( 238, 225, 186 ) );
	pDC->SetPixel( Rect.right - 1, Rect.bottom - 1, RGB( 238, 225, 186 ) );
	Rect.InflateRect( -1, -1 );
	Middle = Rect.Height() * 12 / 32;
	DrawLine( pDC, Rect.left, Rect.top, Rect.right, Rect.top, RGB( 255, 245, 220 ) );
	DrawLine( pDC, Rect.left, Rect.top + 1, Rect.right, Rect.top + 1, RGB( 255, 245, 220 ) );
	DrawGradient( pDC, Rect.left, Rect.top, Rect.left + 2, Rect.top + Middle, RGB( 255, 245, 220 ), RGB( 255, 231, 145 ), true );
	DrawGradient( pDC, Rect.left, Rect.top + Middle, Rect.left + 2, Rect.bottom, RGB( 255, 214, 75 ), RGB( 255, 232, 150 ), true );
	DrawGradient( pDC, Rect.right - 2, Rect.top, Rect.right, Rect.top + Middle, RGB( 255, 245, 220 ), RGB( 255, 231, 145 ), true );
	DrawGradient( pDC, Rect.right - 2, Rect.top + Middle, Rect.right, Rect.bottom, RGB( 255, 214, 75 ), RGB( 255, 232, 150 ), true );
	DrawGradient( pDC, Rect.left, Rect.bottom - 2, Rect.left + ( Rect.Width() / 2 ), Rect.bottom, RGB( 255, 232, 150 ), RGB( 255, 235, 174 ), false );
	DrawGradient( pDC, Rect.left + ( Rect.Width() / 2 ), Rect.bottom - 2, Rect.right, Rect.bottom, RGB( 255, 235, 174 ), RGB( 255, 232, 150 ), false );
}

void CPopupGallery::OnPaint()
{
	CPaintDC dc(this);

	if( m_pImplementation == 0 )
		return;

	CRect ClientRect;
	GetClientRect( &ClientRect );

	CRect ClippingRect;
	dc.GetClipBox( &ClippingRect );

	CDC MemoryDC;

	MemoryDC.CreateCompatibleDC( &dc );
	CBitmap Bitmap;
	Bitmap.CreateCompatibleBitmap( &dc, ClientRect.Width(), ClientRect.Height() );
	MemoryDC.SelectObject( &Bitmap );
	CBrush Background( RGB( 240, 240, 240 ) );
	MemoryDC.FillRect( &ClippingRect, &Background );

	int UseColumns = 0;
	int UseRows = 0;
	m_pImplementation->GetGridSize( UseColumns, UseRows );

	MemoryDC.SetTextColor( RGB( 40, 40, 40 ) );

	int Index = 0;
	for( int Row = 0; Row < UseRows; ++Row )
	{
		for( int Column = 0; Column < UseColumns; ++Column, ++Index )
		{
			if( Index >= m_pImplementation->m_Count )
				continue;

			CRect Rect;
			m_pImplementation->GetItemRect( Index, Rect );

			if( Rect.left < ClippingRect.right && Rect.top < ClippingRect.bottom
			    && Rect.right >= ClippingRect.left && Rect.bottom >= ClippingRect.top )
			{
				if( Index == m_pImplementation->m_SelectedItem )
					m_pImplementation->PaintSelection( &MemoryDC, &Rect );

				Rect.InflateRect( -m_pImplementation->ITEMBORDER, -m_pImplementation->ITEMBORDER );

				if( UseColumns == 1 )
					MemoryDC.PatBlt( Rect.left, Rect.top, Rect.Width(), Rect.Height(), WHITENESS );

				m_pImplementation->m_Images.DrawEx( &MemoryDC, Rect, Index );

				if( UseColumns == 1 )
				{
					MemoryDC.SelectObject( &m_pImplementation->m_TitleFont );
					MemoryDC.SetTextAlign( TA_BASELINE | TA_LEFT );
					MemoryDC.TextOut( Rect.left + Rect.Height() + m_pImplementation->TEXT_BORDER, Rect.top + ( Rect.Height() / 2 ) - 3,  m_pImplementation->m_pToolTitles[Index] );
					MemoryDC.SelectObject( &m_pImplementation->m_DescriptionFont );
					MemoryDC.SetTextAlign( TA_TOP | TA_LEFT );
					MemoryDC.TextOut( Rect.left + Rect.Height() + m_pImplementation->TEXT_BORDER, Rect.top + ( Rect.Height() / 2 ) - 3,  m_pImplementation->m_pToolDescriptions[Index] );
				}
			}
		}
	}

	dc.BitBlt( 0, 0, ClientRect.Width(), ClientRect.Height(), &MemoryDC, 0, 0, SRCCOPY );
}

void CPopupGallery::OnKillFocus(CWnd* pNewWnd)
{
	CWnd::OnKillFocus( pNewWnd );

	HideGallery();
}

void CPopupGalleryImplementation::AnyButtonDown( UINT nFlags, CPoint point )
{
}

void CPopupGalleryImplementation::AnyButtonUp( CPopupGallery *pTheWindow, UINT nFlags, CPoint point )
{
	m_SelectedItem = GetItemFromPoint( point );
	if( m_pParent == 0 || m_SelectedItem >= m_Count )
	{
		m_SelectedItem = -1;
		pTheWindow->HideGallery();
		return;
	}

	m_pParent->PostMessage( WM_COMMAND, m_CommandID );
	pTheWindow->HideGallery();
}

int CPopupGalleryImplementation::GetItemFromPoint( CPoint point )
{
	int cx = m_ItemSize.cx + ( ITEMBORDER * 2 );
	int cy = m_ItemSize.cy + ( ITEMBORDER * 2 );

	point.x -= BORDER;
	point.y -= BORDER;

	if( point.x < 0 || point.y < 0 )
		return -1;

	int Column = point.x / cx;
	int Row = point.y / cy;

	int UseColumns = 0;
	int UseRows = 0;
	GetGridSize( UseColumns, UseRows );

	if( Column >= UseColumns || Row >= UseRows )
		return -1;

	return ( Row * UseColumns ) + Column;
}

void CPopupGallery::OnMouseMove(UINT nFlags, CPoint point)
{
	if( m_pImplementation == 0 )
	{
		CWnd::OnMouseMove( nFlags, point );
		return;
	}
	
	if( !m_pImplementation->m_bTrackingMouseLeave )
	{
		m_pImplementation->m_bTrackingMouseLeave = true;
		
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(TRACKMOUSEEVENT);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = this->m_hWnd;
		TrackMouseEvent( &tme );
	}

	int SelectedItem = m_pImplementation->GetItemFromPoint( point );
	if( SelectedItem != m_pImplementation->m_SelectedItem )
	{
		CRect Rect;
		if( m_pImplementation->m_SelectedItem >= 0 )
		{
			m_pImplementation->GetItemRect( m_pImplementation->m_SelectedItem, Rect );
			InvalidateRect( &Rect );
		}
		m_pImplementation->m_SelectedItem = SelectedItem;
		if( m_pImplementation->m_SelectedItem >= 0 )
		{
			m_pImplementation->GetItemRect( m_pImplementation->m_SelectedItem, Rect );
			InvalidateRect( &Rect );
		}
	}

	if( SelectedItem >= 0 && SelectedItem < m_pImplementation->m_Count )
		m_pImplementation->m_ToolTip.SetDescription( m_pImplementation->m_pToolDescriptions[SelectedItem] );

	CWnd::OnMouseMove( nFlags, point );
}


void CPopupGallery::OnMouseLeave()
{
	if( m_pImplementation == 0 )
		return;

	m_pImplementation->m_bTrackingMouseLeave = false;

	CRect Rect;
	if( m_pImplementation->m_SelectedItem >= 0 )
	{
		m_pImplementation->GetItemRect( m_pImplementation->m_SelectedItem, Rect );
		InvalidateRect( &Rect );
	}
	m_pImplementation->m_SelectedItem = -1;
}


void CPopupGallery::OnLButtonDown(UINT nFlags, CPoint point)
{
	if( m_pImplementation == 0 )
		return;

	m_pImplementation->AnyButtonDown(nFlags, point);
}


void CPopupGallery::OnLButtonUp(UINT nFlags, CPoint point)
{
	if( m_pImplementation == 0 )
		return;

	m_pImplementation->AnyButtonUp( this, nFlags, point );
}


void CPopupGallery::OnRButtonDown(UINT nFlags, CPoint point)
{
	if( m_pImplementation == 0 )
		return;

	m_pImplementation->AnyButtonDown(nFlags, point);
}


void CPopupGallery::OnRButtonUp(UINT nFlags, CPoint point)
{
	if( m_pImplementation == 0 )
		return;

	m_pImplementation->AnyButtonUp( this, nFlags, point );
}


void CPopupGallery::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if( nChar == VK_ESCAPE )
		HideGallery();
}


void CPopupGallery::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
	InflateRect( &lpncsp->rgrc[0], -2, -2 );
}


void CPopupGallery::OnNcPaint()
{
	CWindowDC dc( this );

	CRect Rect;
	GetWindowRect( &Rect );
	Rect.OffsetRect( -Rect.left, -Rect.top );
	
	CBrush Medium( RGB( 180, 180, 180 ) );
	CPen Dark( PS_SOLID, 1, RGB( 100, 100, 100 ) );
	CPen Light( PS_SOLID, 1, RGB( 244, 247, 252 ) );

	--Rect.right;
	--Rect.bottom;

	dc.FrameRect( &Rect, &Medium );

	dc.SelectObject( &Light );
	dc.MoveTo( Rect.left + 1, Rect.top + 1 );
	dc.LineTo( Rect.right - 1, Rect.top + 1 );
	dc.MoveTo( Rect.left + 1, Rect.top + 1 );
	dc.LineTo( Rect.left + 1, Rect.bottom - 1 );

	dc.SelectObject( &Dark );
	dc.MoveTo( Rect.right, Rect.top );
	dc.LineTo( Rect.right, Rect.bottom + 1 );
	dc.MoveTo( Rect.left, Rect.bottom );
	dc.LineTo( Rect.right + 1, Rect.bottom );
}

#if 0
INT_PTR CPopupGallery::OnToolHitTest(CPoint point, TOOLINFO* pTI) const
{
	if( pTI != NULL )
	{
		pTI->hwnd = m_hWnd;
		pTI->uId = (UINT_PTR)GetSafeHwnd();
		pTI->uFlags |= TTF_ALWAYSTIP; // | TTF_IDISHWND;
		pTI->hinst = AfxGetInstanceHandle();
		pTI->lpszText = "derfus";
	}

	return 0;

#if 0
	// find child window which hits the point
	// (don't use WindowFromPoint, because it ignores disabled windows)
	HWND hWndChild = _AfxTopChildWindowFromPoint(m_hWnd, point);
	if (hWndChild != NULL)
	{
		// return positive hit if control ID isn't -1
		INT_PTR nHit = _AfxGetDlgCtrlID(hWndChild);

		// hits against child windows always center the tip
		if (pTI != NULL && pTI->cbSize >= sizeof(AFX_OLDTOOLINFO))
		{
			// setup the TOOLINFO structure
			pTI->hwnd = m_hWnd;
			pTI->uId = (UINT_PTR)hWndChild;
			pTI->uFlags |= TTF_IDISHWND;
			pTI->lpszText = LPSTR_TEXTCALLBACK;

			// set TTF_NOTBUTTON and TTF_CENTERTIP if it isn't a button
			if (!(::SendMessage(hWndChild, WM_GETDLGCODE, 0, 0) & DLGC_BUTTON))
				pTI->uFlags |= TTF_NOTBUTTON|TTF_CENTERTIP;
		}
		return nHit;
	}
	return -1;  // not found
#endif
}
#endif

BOOL CPopupGallery::PreTranslateMessage( MSG* pMsg )
{
 	if( m_pImplementation == 0 )
		return CWnd::PreTranslateMessage(pMsg);

  	switch (pMsg->message)
	{
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
		case WM_MOUSEMOVE:
			m_pImplementation->m_ToolTip.RelayEvent(pMsg);
			break;
	}

	return CWnd::PreTranslateMessage(pMsg);
}