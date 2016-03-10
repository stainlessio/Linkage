#pragma once

class CColorStatic : public CStatic
{
	DECLARE_DYNAMIC(CColorStatic)

	public:
	CColorStatic (COLORREF backgroundColor = 0, COLORREF textColor = 0)
	{
		m_Color = RGB( 200, 200, 200 );
	}

	public:

	// Operations
	public:
	void SetColor( COLORREF Color ) { m_Color = Color; InvalidateRect( 0 ); }
	COLORREF GetColor( void ) { return m_Color; }

	public:
	virtual ~CColorStatic() {}

	protected:
	COLORREF m_Color;

	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnEraseBkgnd(CDC* pDC) { return TRUE; }
	afx_msg void OnPaint( void )
	{
		CPaintDC dc( this );
		CBrush Brush;
		COLORREF Color = m_Color;
		if( !IsWindowEnabled() )
			Color = RGB( 220, 220, 220 );
		Brush.CreateSolidBrush( Color );
		CRect Rect;
		GetClientRect( &Rect );
		dc.FillRect( &Rect, &Brush );
		CBrush Border;
		Border.CreateSolidBrush( RGB( 170, 170, 170 ) );
		dc.FrameRect( &Rect, &Border );
	}
};
