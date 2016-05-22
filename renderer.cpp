#include "stdafx.h"
#include "Renderer.h"
#include "DXFFile.h"

#if defined( LINKAGE_USE_DIRECT2D )
#pragma comment( lib, "d2d1" )
#pragma comment( lib, "Dwrite" )
#include <d2d1.h>
#include <dwrite.h>
#endif

static int ExpandPolygonCornerx( CFPoint &Point, CFPoint &PreviousPoint, CFPoint &NextPoint, double Distance, CFPoint &NewPoint1, CFPoint &NewPoint2 )
{
	// Returns two points that are the proper distance from the original and
	// are the start and end points for an arc to round the corner. The order
	// of the two points is correct for the Windows CDC Arc function.

	double Angle = GetAngle( Point, PreviousPoint, NextPoint );

	bool bChangeDirection = ( Angle > 180 );
	// FORCE TO FALSE. THE HULL SHOULD ALWAYS BE SORTED SO THERE SHOULD BE NO DIRECTION CHANGE!
	bChangeDirection = false;

	CFLine Line1( PreviousPoint, Point );
	CFLine Perp1;
	Line1.PerpendicularLine( Perp1, bChangeDirection ? 1 : -1 );

	CFLine Line2( NextPoint, Point );
	CFLine Perp2;
	Line2.PerpendicularLine( Perp2, bChangeDirection ? -1 : 1 );

	Perp1.SetDistance( Distance );
	Perp2.SetDistance( Distance );

	NewPoint1 = Perp1.GetEnd();
	NewPoint2 = Perp2.GetEnd();

//	if( fabs( NewPoint1.x - NewPoint2.x ) < 2 && fabs( NewPoint1.y - NewPoint2.y ) < 2 )
//		return 0;

	return bChangeDirection ? 1 : -1;
}

class CRendererImplementation
{
	public:

	CFPoint m_ScrollPosition;
	double m_Scale;
	double m_DPIScale;

	CRendererImplementation()
	{
		m_ScrollPosition.SetPoint( 0, 0 );
		m_Scale = 1.0;
		m_DPIScale = 1.0;
	}

	virtual ~CRendererImplementation() {}

	double Scale( double &length )
	{
		length *= m_Scale * m_DPIScale;
		return length;
	}

	void Scale( double &x, double &y )
	{
		x *= m_Scale * m_DPIScale;
		y *= m_Scale * m_DPIScale;
		if( m_Scale > 1 )
		{
			/*
			 * This adjustment lines up the pixels from drawing operations in
			 * a way that looks better when shrunk down. This give the anti-aliased
			 * images, videos, etc., a slightly better look to them.
			 */
			x += (int)( m_Scale / 2 );
			y += (int)( m_Scale / 2 );
		}
		x -= m_ScrollPosition.x;
		y -= m_ScrollPosition.y;
	}

	void Unscale( double &x, double &y )
	{
		x += m_ScrollPosition.x;
		y += m_ScrollPosition.y;
		x /= m_Scale * m_DPIScale;
		y /= m_Scale * m_DPIScale;
	}

	CFPoint Unscale( CPoint PixelPoint )
	{
		CFPoint NewPoint = PixelPoint;
		Unscale( NewPoint.x, NewPoint.y );
		return NewPoint;
	}

	CFRect Unscale( CRect PixelRect )
	{
		CFRect NewRect( PixelRect.left, PixelRect.top, PixelRect.right, PixelRect.bottom );
		Unscale( NewRect.left, NewRect.top );
		Unscale( NewRect.right, NewRect.bottom );
		return NewRect;
	}

	CFPoint Scale( CFPoint Point )
	{
		Scale( Point.x, Point.y );
		CFPoint NewPoint( Point.x, Point.y );
		return NewPoint;
	}

	CFRect Scale( CFRect Rect )
	{
		Scale( Rect.left, Rect.top );
		Scale( Rect.right, Rect.bottom );
		CFRect NewRect( Rect.left, Rect.top, Rect.right, Rect.bottom );
		return NewRect;
	}

	virtual int GetYOrientation( void ) = 0;
	virtual void SaveDXF( const char *pFileName ) = 0;
	virtual bool IsPrinting( void ) = 0;
	virtual int GetColorCount( void ) = 0;
	virtual CDC *GetDC( void ) = 0;
	virtual HDC GetSafeHdc( void ) = 0;
	virtual void Attach( HDC hDeviceContext ) = 0;
	virtual void Detach( void ) = 0;
	virtual void SetAttribDC( HDC hDeviceContext ) = 0;
	virtual int SetBkMode( int nBkMode ) = 0;
	virtual COLORREF SetBkColor( COLORREF crColor ) = 0;
	virtual CGdiObject* SelectStockObject( int nIndex ) = 0;
	virtual COLORREF SetTextColor( COLORREF crColor ) = 0;
	virtual COLORREF GetTextColor( void ) = 0;
	virtual void FillRect( LPCRECT lpRect, CBrush* pBrush ) = 0;
	virtual unsigned int SetTextAlign( unsigned int nFlags ) = 0;
	virtual bool PatBlt( int x, int y, int nWidth, int nHeight, DWORD dwRop ) = 0;
	virtual bool CreateCompatibleDC( CDC *pDC, CRect *pRect = 0 ) = 0;
	virtual int SetArcDirection(int Direction ) = 0;
	virtual bool FillRgn( CFPoint *pPoints, int Count, CBrush* pBrush ) = 0;
	virtual CFPoint MoveTo( double x, double y ) = 0;
	virtual CFPoint LineTo( double x, double y ) = 0;
	CFPoint MoveTo( CFPoint Point ) { return MoveTo( Point.x, Point.y ); }
	CFPoint LineTo( CFPoint Point ) { return LineTo( Point.x, Point.y ); }
	virtual CFPoint DrawLine( CFLine Line ) = 0;
	virtual CFPoint DrawLine( CFPoint Point1, CFPoint Point2 ) = 0;
	virtual bool TextOut( double x, double y, const CString& str ) = 0;
	virtual bool Circle( CFCircle &Circle ) = 0;
	virtual bool Arc( CFArc &TheArc ) = 0;
	virtual bool Arc( double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4, bool bDrawTo ) = 0;
	virtual bool Arc( double x, double y, double r, double x3, double y3, double x4, double y4, bool bDrawTo ) = 0;
	virtual bool Pie( double x, double y, double r, double x3, double y3, double x4, double y4, bool bDrawTo ) = 0;
	virtual bool DrawRect( double RotationAngle, double x1, double y1, double x2, double y2 ) = 0;
	virtual bool DrawRect( double x1, double y1, double x2, double y2 ) = 0;
	virtual bool DrawRect( const CFRect &Rect ) = 0;
	virtual void FillRect( CFRect* pRect, CBrush* pBrush ) = 0;
	//virtual COLORREF SetPixel( double x, double y, COLORREF crColor ) = 0;
	virtual void BeginDraw( void ) {}
	virtual void EndDraw( void ) {}

	virtual int ExpandPolygonCorner( CFPoint &Point, CFPoint &PreviousPoint, CFPoint &NextPoint, double Distance, CFPoint &NewPoint1, CFPoint &NewPoint2 )
	{
		// Returns two points that are the proper distance from the original and
		// are the start and end points for an arc to round the corner. The order
		// of the two points is correct for the Windows CDC Arc function.

		double Angle = GetAngle( Point, PreviousPoint, NextPoint );

		bool bChangeDirection = ( Angle > 180 );
		// FORCE TO FALSE. THE HULL SHOULD ALWAYS BE SORTED SO THERE SHOULD BE NO DIRECTION CHANGE!
		bChangeDirection = false;

		CFLine Line1( PreviousPoint, Point );
		CFLine Perp1;
		Line1.PerpendicularLine( Perp1, bChangeDirection ? 1 : -1 );

		CFLine Line2( NextPoint, Point );
		CFLine Perp2;
		Line2.PerpendicularLine( Perp2, bChangeDirection ? -1 : 1 );

		Perp1.SetDistance( Distance );
		Perp2.SetDistance( Distance );

		NewPoint1 = Perp1.GetEnd();
		NewPoint2 = Perp2.GetEnd();

		if( fabs( NewPoint1.x - NewPoint2.x ) < 2 && fabs( NewPoint1.y - NewPoint2.y ) < 2 )
			return 0;

		return bChangeDirection ? 1 : -1;
	}

	virtual void LinkageDrawExpandedPolygon( CFPoint *pPoints, double *pLineRadii, int PointCount, COLORREF Color, bool bFill, double ExpansionDistance )
	{
		// WARNING, THIS CODE ASSUMES A 100% CONVEX POLYGON. IT WILL NOT
		// WORK PROPERLY ON A POLYGON WITH ANY CONCAVE-NESS.

		if( PointCount <= 1 )
			return;

		CFPoint *pRegionPoints = 0;
		COLORREF NewColor;
		CFPoint FirstPoint1;
		CFPoint FirstPoint2;
		int FillPoint = 0;

		double Red = GetRValue( Color ) / 255.0;
		double Green = GetGValue( Color ) / 255.0;
		double Blue = GetBValue( Color ) / 255.0;
		if( Red < 1.0 )
			Red += ( 1.0 - Red ) * .965;
		if( Green < 1.0 )
			Green += ( 1.0 - Green ) * .965;
		if( Blue < 1.0 )
			Blue += ( 1.0 - Blue ) * .965;

		NewColor = RGB( (int)( Red * 255 ), (int)( Green * 255 ), (int)( Blue * 255 ) );
		CBrush Brush;
		Brush.CreateSolidBrush( NewColor );
		CPen Pen( PS_SOLID, (int)min( 1.0 * m_Scale * m_DPIScale, 1.0 ), NewColor );

		int ArcDirection = 0;

		if( ExpansionDistance == 0 )
		{
			FirstPoint1 = pPoints[0];
			FirstPoint2 = pPoints[0];
			ArcDirection = 0;
		}
		else
		{
			ArcDirection = ExpandPolygonCorner( pPoints[0], pPoints[PointCount-1], pPoints[1], ExpansionDistance, FirstPoint1, FirstPoint2 );
			SetArcDirection( ArcDirection == 1 ? AD_COUNTERCLOCKWISE : AD_CLOCKWISE );
		}
		int FirstDirection = ArcDirection;

		CPen *pOldPen = 0;
		CBrush *pOldBrush = 0;

		if( bFill )
		{
			pOldPen = SelectObject( &Pen );
			pOldBrush = SelectObject( &Brush );

			pRegionPoints = new CFPoint[PointCount*2];
			if( pRegionPoints == 0 )
				return;

			Scale( FirstPoint1.x, FirstPoint1.y );
			pRegionPoints[FillPoint++].SetPoint( (int)FirstPoint1.x, (int)FirstPoint1.y );
			if( ArcDirection != 0 )
			{
				Scale( FirstPoint2.x, FirstPoint2.y );
				pRegionPoints[FillPoint++].SetPoint( (int)FirstPoint2.x, (int)FirstPoint2.y );
			}
		}
		else
		{
			if( ArcDirection == 0 )
				MoveTo( FirstPoint1 );
			else
			{
				CPen *pOldPen = (CPen*)SelectStockObject( NULL_PEN );
				MoveTo( FirstPoint2 );
				SelectObject( pOldPen );
			}
		}

		for( int Counter = 1; Counter < PointCount; ++Counter )
		{
			CFPoint *pNextPoint = 0;
			if( Counter == PointCount-1 )
				pNextPoint = &pPoints[0];
			else
				pNextPoint = &pPoints[Counter+1];
			CFPoint TempPoint1;
			CFPoint TempPoint2;
			if( ExpansionDistance == 0 )
			{
				TempPoint1 = pPoints[Counter];
				TempPoint2 = pPoints[Counter];
				ArcDirection = 0;
			}
			else
			{
				ArcDirection = ExpandPolygonCorner( pPoints[Counter], pPoints[Counter-1], *pNextPoint, ExpansionDistance, TempPoint1, TempPoint2 );
				SetArcDirection( ArcDirection == 1 ? AD_COUNTERCLOCKWISE : AD_CLOCKWISE );
			}

			if( bFill )
			{
				if( ArcDirection != 0 )
					Pie( pPoints[Counter].x, pPoints[Counter].y, ExpansionDistance, TempPoint1.x, TempPoint1.y, TempPoint1.x, TempPoint1.y, true );
				Scale( TempPoint1.x, TempPoint1.y );
				pRegionPoints[FillPoint++].SetPoint( (int)TempPoint1.x, (int)TempPoint1.y );
				int BackPoints = 1;
				if( ArcDirection != 0 )
				{
					++BackPoints;
					Scale( TempPoint2.x, TempPoint2.y );
					pRegionPoints[FillPoint++].SetPoint( (int)TempPoint2.x, (int)TempPoint2.y );
				}

				if( 0 && pLineRadii != 0 && pLineRadii[Counter-1] != 0 && ExpansionDistance == 0 )
				{
					double ChordHalf = Distance( pRegionPoints[FillPoint-2], pRegionPoints[FillPoint-3] ) / 2;
					double ToCenter = sqrt( pLineRadii[Counter-1] * pLineRadii[Counter-1] / ChordHalf * ChordHalf );
					CFLine Line( pRegionPoints[FillPoint-BackPoints], pRegionPoints[FillPoint-BackPoints-1] );
					CFLine Perp;
					Line.PerpendicularLine( Perp, 0 );
					Line.SetDistance( ChordHalf );
					Perp -= Line.GetEnd() - Line.GetStart();
					Perp.SetDistance( ToCenter );

					CPen Derf( PS_SOLID, 6, RGB( 255, 0, 0 ) );
					CPen *pDerf = SelectObject( &Derf );
					DrawLine( Perp );

					Pie( pRegionPoints[FillPoint-BackPoints].x, pRegionPoints[FillPoint-BackPoints].y, 10, pRegionPoints[FillPoint-BackPoints].x+10, pRegionPoints[FillPoint-BackPoints].y, pRegionPoints[FillPoint-BackPoints].x+10, pRegionPoints[FillPoint-BackPoints].y, false );
					Pie( pRegionPoints[FillPoint-BackPoints-1].x, pRegionPoints[FillPoint-BackPoints-1].y, 10, pRegionPoints[FillPoint-BackPoints-1].x+10, pRegionPoints[FillPoint-BackPoints-1].y, pRegionPoints[FillPoint-BackPoints-1].x+10, pRegionPoints[FillPoint-BackPoints-1].y, false );



					//Pie( Line.GetEnd().x, Line.GetEnd().y, pLineRadii[Counter-1], pRegionPoints[FillPoint-2].x, pRegionPoints[FillPoint-2].y, pRegionPoints[FillPoint-3].x, pRegionPoints[FillPoint-3].y, false );
					SelectObject( pDerf );
				}
			}
			else
			{
				if( ArcDirection == 0 )
					LineTo( TempPoint1.x, TempPoint1.y );
				else
				{
					Arc( pPoints[Counter].x, pPoints[Counter].y, ExpansionDistance, TempPoint1.x, TempPoint1.y, TempPoint2.x, TempPoint2.y, true );
					MoveTo( TempPoint2.x, TempPoint2.y );
				}
			}
		}

		if( bFill )
		{
			if( ExpansionDistance > 0 )
				Pie( pPoints[0].x, pPoints[0].y, ExpansionDistance, FirstPoint1.x, FirstPoint1.y, FirstPoint1.x, FirstPoint1.y, true );

			FillRgn( pRegionPoints, FillPoint, &Brush );
			
			delete [] pRegionPoints;

			SelectObject( pOldPen );
			SelectObject( pOldBrush );
		}
		else
		{
			if( FirstDirection == 0 )
				LineTo( FirstPoint1.x, FirstPoint1.y );
			else
			{
				// Draw a second time to get the line to go to it properly.
				SetArcDirection( FirstDirection == 1 ? AD_COUNTERCLOCKWISE : AD_CLOCKWISE );
				Arc( pPoints[0].x, pPoints[0].y, ExpansionDistance, FirstPoint1.x, FirstPoint1.y, FirstPoint2.x, FirstPoint2.y, true );

				// Get back to the exact end point of this arc.
				LineTo( FirstPoint2 );
			}
		}
		SetArcDirection( AD_COUNTERCLOCKWISE );
	}

	//virtual void LinkageDrawExpandedPolygon( CFPoint *pPoints, double *pLineRadii, int PointCount, COLORREF Color, bool bFill, double ExpansionDistance ) = 0;
	virtual CPen* SelectObject( CPen* pPen ) = 0;
	virtual CFont* SelectObject( CFont* pFont, double FontHeight ) = 0;
	virtual CBitmap* SelectObject( CBitmap* pBitmap ) = 0;
	virtual int SelectObject( CRgn* pRgn ) = 0;
	virtual CBrush* SelectObject( CBrush *pBrush ) = 0;
	virtual CFPoint GetTextExtent( const char *pString, int Count ) = 0;

	int GetPenSize( int UnscaledSize )
	{
		return (int)max( UnscaledSize * m_Scale * m_DPIScale, 1.0 );
	}

	virtual bool DrawArrow( CFPoint FromPoint, CFPoint ToPoint, double Width, double Length ) = 0;

	virtual void SaveToFile( const char *pFileName ) = 0;
};

class CGDIRenderer : public CRendererImplementation
{
	public:

	CList<CPen*,CPen*> m_CreatedPens;
	CDC *m_pDC;
	bool m_bIsPrinting;
	bool m_bCreatedFont;
	LOGFONT m_ScaledLogFont;
	CFont m_ScaledFont;
	CPen m_ScaledPen;

	CGDIRenderer( bool bIsPrinting )
	{
		m_bIsPrinting = bIsPrinting;
		m_ScrollPosition.SetPoint( 0, 0 );
		m_Scale = 1.0;
		m_bCreatedFont = false;
		m_pDC = 0;
		memset( &m_ScaledLogFont, 0, sizeof( m_ScaledLogFont ) );
	}

	virtual ~CGDIRenderer()
	{
		POSITION Position = m_CreatedPens.GetHeadPosition();
		while( Position )
		{
			CPen *pPen = m_CreatedPens.GetNext( Position );
			if( pPen != 0 )
				delete pPen;
		}
		m_CreatedPens.RemoveAll();
		if( m_pDC != 0 )
			delete m_pDC;
	}

	int GetYOrientation( void )
	{
		return -1;
	}

	void SaveDXF( const char *pFileName ) {}

	CFPoint MoveTo( CFPoint Point ) { return MoveTo( Point.x, Point.y ); }
	CFPoint LineTo( CFPoint Point ) { return LineTo( Point.x, Point.y ); }

	unsigned int SetTextAlign( unsigned int nFlags )
	{
		if( m_pDC == 0 )
			return 0;

		return m_pDC->SetTextAlign( nFlags );
	}

	virtual void FillRect( LPCRECT lpRect, CBrush* pBrush )
	{
		if( m_pDC == 0 )
			return;

		m_pDC->FillRect( lpRect, pBrush );
	}

	COLORREF GetTextColor( void )
	{
		if( m_pDC == 0 )
			return RGB( 0, 0, 0 );

		return m_pDC->GetTextColor();
	}

	COLORREF SetTextColor( COLORREF crColor )
	{
		if( m_pDC == 0 )
			return RGB( 0, 0, 0 );

		return m_pDC->SetTextColor( crColor );
	}

	CGdiObject* SelectStockObject( int nIndex )
	{
		if( m_pDC == 0 )
			return 0;

		return m_pDC->SelectStockObject( nIndex );
	}

	bool IsPrinting( void )
	{
		return m_bIsPrinting;
	}

	int GetColorCount( void )
	{
		if( m_pDC != 0 )
			return 0;

		return m_pDC->GetDeviceCaps( NUMCOLORS );
	}

	void Attach( HDC hDeviceContext )
	{
		if( m_pDC != 0 )
			return;

		m_pDC = new CDC;
		m_pDC->Attach( hDeviceContext );
	}

	void Detach( void )
	{
		if( m_pDC == 0 )
			return;

		m_pDC->Detach();
	}

	void SetAttribDC( HDC hDeviceContext )
	{
		if( m_pDC == 0 )
			return;

		m_pDC->SetAttribDC( hDeviceContext );
	}

	virtual HDC GetSafeHdc( void )
	{
		if( m_pDC == 0 )
			return (HDC)INVALID_HANDLE_VALUE;

		return m_pDC->GetSafeHdc();
	}

	virtual CDC *GetDC( void )
	{
		return m_pDC;
	}

	virtual int SetBkMode( int nBkMode )
	{
		if( m_pDC == 0 )
			return 0;

		return m_pDC->SetBkMode( nBkMode );
	}

	virtual COLORREF SetBkColor( COLORREF crColor )
	{
		if( m_pDC == 0 )
			return RGB( 0, 0, 0 );

		return m_pDC->SetBkColor( crColor );
	}

	bool PatBlt( int x, int y, int nWidth, int nHeight, DWORD dwRop )
	{
		if( m_pDC == 0 )
			return false;

		return m_pDC->PatBlt( x, y, nWidth, nHeight, dwRop ) != 0;
	}

	bool CreateCompatibleDC( CDC *pDC, CRect *pRect )
	{
		m_pDC = new CDC;
		if( m_pDC == 0 )
			return false;
		return m_pDC->CreateCompatibleDC( pDC ) != 0;
	}

	int SetArcDirection( int Direction )
	{
		return m_pDC->SetArcDirection( Direction );
	}

	bool FillRgn( CFPoint *pPoints, int Count, CBrush* pBrush )
	{
		CRgn Region;
		CPoint *pIntegerPoints = new CPoint[Count];
		if( pIntegerPoints == 0 )
			return false;
		for( int Index = 0; Index < Count; ++Index )
			pIntegerPoints[Index].SetPoint( (int)pPoints[Index].x, (int)pPoints[Index].y );
		Region.CreatePolygonRgn( pIntegerPoints, Count, ALTERNATE );
		delete [] pIntegerPoints;
		return m_pDC->FillRgn( &Region, pBrush ) != 0;
	}

	void SaveToFile( const char *pFileName ) {}

	CFPoint MoveTo( double x, double y )
	{
		Scale( x, y );
		CFPoint Result = m_pDC->MoveTo( (int)x, (int)y );
		Unscale( Result.x, Result.y );
		return Result;
	}

	CFPoint LineTo( double x, double y )
	{
		Scale( x, y );

		CFPoint Result = m_pDC->LineTo( (int)x, (int)y );
		CPen* pPen = m_pDC->GetCurrentPen();
		if( pPen != 0 )
		{
			LOGPEN LogPen;
			if( pPen->GetLogPen( &LogPen ) != 0 )
			{
				COLORREF PenColor = LogPen.lopnColor;
				m_pDC->SetPixel( (int)x, (int)y, PenColor );
			}
		}
		Unscale( Result.x, Result.y );

		return Result;
	}

	CFPoint DrawLine( CFLine Line )
	{
		MoveTo( Line.GetStart() );
		return LineTo( Line.GetEnd() );
	}

	CFPoint DrawLine( CFPoint Point1, CFPoint Point2 )
	{
		return DrawLine( CFLine( Point1, Point2 ) );
	}

	bool TextOut( double x, double y, const CString& str )
	{
		Scale( x, y );
		return m_pDC->TextOut( (int)x, (int)y, str ) != 0;
	}

	bool Circle( CFCircle &Circle )
	{
		double x1 = Circle.x - Circle.r;
		double y1 = Circle.y - Circle.r;
		double x2 = Circle.x + Circle.r;
		double y2 = Circle.y + Circle.r;

		Scale( x1, y1 );
		Scale( x2, y2 );

		return m_pDC->Ellipse( (int)( x1 ), (int)( y1 ), (int)( x2 ) + 1, (int)( y2 ) + 1 ) != 0;
	}

	bool Arc( CFArc &TheArc )
	{
		return Arc( TheArc.x, TheArc.y, TheArc.r, TheArc.m_Start.x, TheArc.m_Start.y, TheArc.m_End.x, TheArc.m_End.y );
	}

	bool Arc( double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4, bool bDrawTo = false )
	{
		Scale( x1, y1 );
		Scale( x2, y2 );
		Scale( x3, y3 );
		Scale( x4, y4 );

		if( bDrawTo )
			return m_pDC->ArcTo( (int)x1, (int)y1, (int)x2+1, (int)y2+1, (int)x3, (int)y3, (int)x4, (int)y4 ) != 0;
		else
			return m_pDC->Arc( (int)x1, (int)y1, (int)x2+1, (int)y2+1, (int)x3, (int)y3, (int)x4, (int)y4 ) != 0;
	}

	bool Arc( double x, double y, double r, double x3, double y3, double x4, double y4, bool bDrawTo = false )
	{
		double x2 = x + r;
		double y2 = y + r;
		x -= r;
		y -= r;
		return Arc( x, y, x2, y2, x3, y3, x4, y4, bDrawTo );
	}

	bool Pie( double x, double y, double r, double x3, double y3, double x4, double y4, bool bDrawTo )
	{
		double x2 = x + r;
		double y2 = y + r;
		x -= r;
		y -= r;

		Scale( x, y );
		Scale( x2, y2 );
		Scale( x3, y3 );
		Scale( x4, y4 );

		return m_pDC->Pie( (int)x, (int)y, (int)x2+1, (int)y2+1, (int)x3, (int)y3, (int)x4, (int)y4 ) != 0;
	}

	bool DrawRect( double RotationAngle, double x1, double y1, double x2, double y2 )
	{
		double Deviation = RotationAngle;
		while( Deviation > 89 )
			Deviation -= 90;
		if( fabs( Deviation ) < .1 )
	//	if( RotationAngle == 0 || RotationAngle == 90 || RotationAngle == 180 || RotationAngle == 270 )
			return DrawRect( x1, y1, x2, y2 );

		CFPoint Center( ( x1 + x2 ) / 2.0, ( y1 + y2 ) / 2.0 );

		// The addition of .1 causes the display to look better when rounding to
		// individual pixels. Without this, the rectange appears to rotate sooner
		// than expected in comparison to other things.

		CFPoint Point1( x1 + .1, y1 + .1 );
		CFPoint Point2( x2 + .1, y1 + .1 );
		CFPoint Point3( x2 + .1, y2 + .1 );
		CFPoint Point4( x1 + .1, y2 + .1 );

		Point1.RotateAround( Center, RotationAngle );
		Point2.RotateAround( Center, RotationAngle );
		Point3.RotateAround( Center, RotationAngle );
		Point4.RotateAround( Center, RotationAngle );

		MoveTo( Point1 );
		LineTo( Point2 );
		LineTo( Point3 );
		LineTo( Point4 );
		LineTo( Point1 );

		return TRUE;
	}

	bool DrawRect( double x1, double y1, double x2, double y2 )
	{
		Scale( x1, y1 );
		Scale( x2, y2 );
		CBrush *pOldBrush = (CBrush*)m_pDC->SelectStockObject( NULL_BRUSH );
		bool bResult =  m_pDC->Rectangle( (int)x1, (int)y1, (int)x2+1, (int)y2+1 ) != 0;
		m_pDC->SelectObject( pOldBrush );
		return bResult;
	}

	bool DrawRect( const CFRect &Rect )
	{
		CFRect NewRect = Scale( Rect );
		CBrush *pOldBrush = (CBrush*)m_pDC->SelectStockObject( NULL_BRUSH );
		bool bResult =  m_pDC->Rectangle( (int)NewRect.left, (int)NewRect.top, (int)NewRect.right+1, (int)NewRect.bottom+1 ) != 0;
		m_pDC->SelectObject( pOldBrush );
		return bResult;
	}

	void FillRect( CFRect* pRect, CBrush* pBrush )
	{
		CFRect UseRect = *pRect;
		Scale( UseRect.left, UseRect.top );
		Scale( UseRect.right, UseRect.bottom );
		UseRect.right++;
		UseRect.bottom++;
		CRect PixelRect( (int)UseRect.left, (int)UseRect.top, (int)UseRect.right, (int)UseRect.bottom );
		m_pDC->FillRect( &PixelRect, pBrush );
	}

	//COLORREF SetPixel( double x, double y, COLORREF crColor )
	//{
	//	Scale( x, y );
	//	return m_pDC->SetPixel( (int)x, (int)y, crColor );
	//}

	CPen* SelectObject( CPen* pPen )
	{
		if( m_Scale <= 1 )
			return m_pDC->SelectObject( pPen );

		EXTLOGPEN LogPen;
		if( pPen == 0 || pPen->GetExtLogPen( &LogPen ) == 0 )
			return m_pDC->SelectObject( pPen );
		else
		{
			/*
			 * Check for a pen that we created.
			 */

			POSITION Position = m_CreatedPens.GetHeadPosition();
			while( Position )
			{
				if( pPen == m_CreatedPens.GetNext( Position ) )
					return m_pDC->SelectObject( pPen );
			}

			/*
			 * Check for non-solid pens and don't scale their width since
			 * larger pens cannot be non-solid at this time.
			 */
			if( LogPen.elpPenStyle != PS_SOLID )
			{
				double Viewport = m_pDC->GetViewportExt().cx;
				double Window = m_pDC->GetWindowExt().cx;
				double ExtraScale = Window / Viewport;

				LOGBRUSH LogBrush;
				LogBrush.lbColor = LogPen.elpColor;
				LogBrush.lbStyle = PS_SOLID;

				const DWORD DashStyle[2] = { (DWORD)( 4 * m_Scale * m_DPIScale ), (DWORD)( 4 * m_Scale * m_DPIScale ) };

				CPen *pPen = new CPen( PS_GEOMETRIC | PS_USERSTYLE, (int)( LogPen.elpWidth * m_Scale * m_DPIScale ), &LogBrush, 2, DashStyle );

				m_CreatedPens.AddTail( pPen );

				return m_pDC->SelectObject( pPen );
			}

			LogPen.elpWidth = (int)( LogPen.elpWidth * m_Scale * m_DPIScale );

			CPen *pPen = new CPen( LogPen.elpPenStyle, LogPen.elpWidth, LogPen.elpColor );
			if( pPen == 0 )
				return m_pDC->SelectObject( pPen );

			m_CreatedPens.AddTail( pPen );

			return m_pDC->SelectObject( pPen );
		}
		return 0;
	}

	CFont* SelectObject( CFont* pFont, double FontHeight )
	{
		// Using GDI rendering so ignore font height.

		if( m_Scale <= 1 )
			return m_pDC->SelectObject( pFont );

		LOGFONT LogFont;
		if( pFont->GetLogFont( &LogFont ) == 0 )
			return m_pDC->SelectObject( pFont );
		else
		{
			if( !m_bCreatedFont || memcmp( &LogFont, &m_ScaledLogFont, sizeof( LOGFONT ) ) != 0 )
			{
				m_ScaledFont.DeleteObject();
				LogFont.lfHeight = (int)( LogFont.lfHeight * m_Scale * m_DPIScale );
				LogFont.lfWidth = (int)( LogFont.lfWidth * m_Scale * m_DPIScale );
				m_ScaledFont.CreateFontIndirect( &LogFont );
				m_bCreatedFont = true;
				memcpy( &m_ScaledLogFont, &LogFont, sizeof( LOGFONT ) );
			}
			return m_pDC->SelectObject( &m_ScaledFont );
		}
	}

	CBitmap* SelectObject( CBitmap* pBitmap )
		{ return m_pDC->SelectObject( pBitmap ); }

	int SelectObject( CRgn* pRgn )
		{ return m_pDC->SelectObject( pRgn ); }

	CBrush* SelectObject( CBrush *pBrush )
		{ return m_pDC->SelectObject( pBrush ); }

	CFPoint GetTextExtent( const char *pString, int Count )
	{
		CSize Size = m_pDC->GetTextExtent( pString, Count );
		CFPoint Result( Size.cx, Size.cy );
		// Only adjust the scaling and not the position.
		Result.x /= m_Scale * m_DPIScale;
		Result.y /= m_Scale * m_DPIScale;
		return Result;
	}

	int GetPenSize( int UnscaledSize )
	{
		return max( (int)( UnscaledSize * m_Scale * m_DPIScale ), 1 );
	}

	bool DrawArrow( CFPoint FromPoint, CFPoint ToPoint, double Width, double Length )
	{
		CPen* pPen = m_pDC->GetCurrentPen();
		LOGPEN LogPen;
		if( pPen->GetLogPen( &LogPen ) == 0 )
			return FALSE;
		COLORREF PenColor = LogPen.lopnColor;
		double Red = GetRValue( PenColor ) / 255.0;
		double Green = GetGValue( PenColor ) / 255.0;
		double Blue = GetBValue( PenColor ) / 255.0;

		CFLine Line( ToPoint, FromPoint );
		Line.SetDistance( Length );

		if( m_Scale <= 1.0 )
		{
			// Darken the color of the pen for the fake arrow.
			Red *= 0.3;
			Green *= 0.3;
			Blue *= 0.3;

			COLORREF NewColor = RGB( (int)( Red * 255 ), (int)( Green * 255 ), (int)( Blue * 255 ) );
			CPen Pen( LogPen.lopnStyle, LogPen.lopnWidth.x, NewColor );

			CPen *pOldPen = (CPen*)m_pDC->SelectObject( &Pen );

			MoveTo( Line.GetStart() );
			LineTo( Line.GetEnd() );

			m_pDC->SelectObject( pOldPen );
		}
		else
		{
			CBrush Brush;
			Red *= 0.85;
			Green *= 0.85;
			Blue *= 0.85;
			COLORREF NewColor = RGB( (int)( Red * 255.0 ), (int)( Green * 255.0 ), (int)( Blue * 255.0 ) );
			Brush.CreateSolidBrush( NewColor );

			CFLine CrossLine;
			Line.PerpendicularLine( CrossLine, Width / 2, 1 );
			CrossLine.ReverseDirection();
			CrossLine.SetDistance( Width );
			CPen *pOldPen = (CPen*)m_pDC->SelectStockObject( NULL_PEN );

			POINT Points[3];
			Points[0].x = (int)Scale( CrossLine.GetStart() ).x;
			Points[0].y = (int)Scale( CrossLine.GetStart() ).y;
			Points[1].x = (int)Scale( CrossLine.GetEnd() ).x;
			Points[1].y = (int)Scale( CrossLine.GetEnd() ).y;
			Points[2].x = (int)Scale( ToPoint ).x;
			Points[2].y = (int)Scale( ToPoint ).y;

			CRgn Region;
			Region.CreatePolygonRgn( Points, 3, ALTERNATE );
			m_pDC->FillRgn( &Region, &Brush );
			m_pDC->SelectObject( pOldPen );
		}

		return TRUE;
	}
};

class CNullRenderer : public CRendererImplementation
{
	public:

	//CList<CPen*,CPen*> m_CreatedPens;
	//CDC *m_pDC;
	//bool m_bIsPrinting;
	//bool m_bCreatedFont;
	//LOGFONT m_ScaledLogFont;
	//CFont m_ScaledFont;
	//CPen m_ScaledPen;

	CNullRenderer( bool bIsPrinting )
	{
	}

	virtual ~CNullRenderer()
	{
	}

	int GetYOrientation( void )
	{
		return -1;
	}

	void SaveDXF( const char *pFileName ) {}

	CFPoint MoveTo( CFPoint Point ) { return MoveTo( Point.x, Point.y ); }
	CFPoint LineTo( CFPoint Point ) { return LineTo( Point.x, Point.y ); }

	unsigned int SetTextAlign( unsigned int nFlags )
	{
		return 0;
	}

	virtual void FillRect( LPCRECT lpRect, CBrush* pBrush )
	{
	}

	COLORREF GetTextColor( void )
	{
		return 0;
	}

	COLORREF SetTextColor( COLORREF crColor )
	{
		return 0;
	}

	CGdiObject* SelectStockObject( int nIndex )
	{
		return 0;
	}

	bool IsPrinting( void )
	{
		return false;
	}

	int GetColorCount( void )
	{
		return 0;
	}

	void Attach( HDC hDeviceContext )
	{
	}

	void Detach( void )
	{
	}

	void SetAttribDC( HDC hDeviceContext )
	{
	}

	virtual HDC GetSafeHdc( void )
	{
		return (HDC)INVALID_HANDLE_VALUE;
	}

	virtual CDC *GetDC( void )
	{
		return 0;
	}

	virtual int SetBkMode( int nBkMode )
	{
		return 0;
	}

	virtual COLORREF SetBkColor( COLORREF crColor )
	{
		return 0;
	}

	bool PatBlt( int x, int y, int nWidth, int nHeight, DWORD dwRop )
	{
		return false;
	}

	bool CreateCompatibleDC( CDC *pDC, CRect *pRect )
	{
		return false;
	}

	int SetArcDirection( int Direction )
	{
		return 0;
	}

	bool FillRgn( CFPoint *pPoints, int Count, CBrush* pBrush )
	{
		return false;
	}

	void SaveToFile( const char *pFileName ) {}

	CFPoint MoveTo( double x, double y )
	{
		return CFPoint();
	}

	CFPoint LineTo( double x, double y )
	{
		return CFPoint();
	}

	CFPoint DrawLine( CFLine Line )
	{
		return CFPoint();
	}

	CFPoint DrawLine( CFPoint Point1, CFPoint Point2 )
	{
		return CFPoint();
	}

	bool TextOut( double x, double y, const CString& str )
	{
		return false;
	}

	bool Circle( CFCircle &Circle )
	{
		return false;
	}

	bool Arc( CFArc &TheArc )
	{
		return false;
	}

	bool Arc( double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4, bool bDrawTo = false )
	{
		return false;
	}

	bool Arc( double x, double y, double r, double x3, double y3, double x4, double y4, bool bDrawTo = false )
	{
		return false;
	}

	bool Pie( double x, double y, double r, double x3, double y3, double x4, double y4, bool bDrawTo )
	{
		return false;
	}

	bool DrawRect( double RotationAngle, double x1, double y1, double x2, double y2 )
	{
		return false;
	}

	bool DrawRect( double x1, double y1, double x2, double y2 )
	{
		return false;
	}

	bool DrawRect( const CFRect &Rect )
	{
		return false;
	}

	void FillRect( CFRect* pRect, CBrush* pBrush )
	{
	}

	//COLORREF SetPixel( double x, double y, COLORREF crColor )
	//{
	//	return 0;
	//}

	CPen* SelectObject( CPen* pPen )
	{
		return 0;
	}

	CFont* SelectObject( CFont* pFont, double FontHeight )
	{
		return 0;
	}

	CBitmap* SelectObject( CBitmap* pBitmap )
		{ return 0; }

	int SelectObject( CRgn* pRgn )
		{ return 0; }

	CBrush* SelectObject( CBrush *pBrush )
		{ return 0; }

	CFPoint GetTextExtent( const char *pString, int Count )
	{
		return CFPoint();
	}

	int GetPenSize( int UnscaledSize )
	{
		return 0;
	}

	bool DrawArrow( CFPoint FromPoint, CFPoint ToPoint, double Width, double Length )
	{
		return false;
	}
};

class CDXFRenderer : public CRendererImplementation
{
	public:

	CList<CPen*,CPen*> m_CreatedPens;
	CDC *m_pDC; // Used to store attributes like pens and fonts but not used for drawing.
	bool m_bCreatedFont;
	LOGFONT m_ScaledLogFont;
	CFont m_ScaledFont;
	CPen m_ScaledPen;
	CFPoint m_CurrentPosition;
	double m_FontHeight;
	int m_ArcDirection;

	CDXFFile m_DXFFile;

	CDXFRenderer()
	{
		// Use the screen device context to keep track of stuff for now.
		m_pDC = new CDC;
		m_pDC->Attach( ::GetDC( 0 ) );

		m_ScrollPosition.SetPoint( 0, 0 );
		m_Scale = 1.0;
		m_bCreatedFont = false;
		memset( &m_ScaledLogFont, 0, sizeof( m_ScaledLogFont ) );
		m_CurrentPosition = CFPoint( 0, 0 );
		m_FontHeight = 0;
		m_ArcDirection = AD_COUNTERCLOCKWISE;
	}

	virtual ~CDXFRenderer()
	{
		POSITION Position = m_CreatedPens.GetHeadPosition();
		while( Position )
		{
			CPen *pPen = m_CreatedPens.GetNext( Position );
			if( pPen != 0 )
				delete pPen;
		}
		m_CreatedPens.RemoveAll();
		if( m_pDC != 0 )
			delete m_pDC;
	}

	int GetYOrientation( void )
	{
		return 1;
	}

	void SaveDXF( const char *pFileName )
	{
		m_DXFFile.DXF_Save( pFileName );
	}

	CFPoint MoveTo( CFPoint Point ) { return MoveTo( Point.x, Point.y ); }
	CFPoint LineTo( CFPoint Point ) { return LineTo( Point.x, Point.y ); }

	unsigned int SetTextAlign( unsigned int nFlags )
	{
		if( m_pDC == 0 )
			return 0;

		return m_pDC->SetTextAlign( nFlags );
	}

	virtual void FillRect( LPCRECT lpRect, CBrush* pBrush )
	{
		if( m_pDC == 0 )
			return;

		// m_pDC->FillRect( lpRect, pBrush );
	}

	COLORREF GetTextColor( void )
	{
		if( m_pDC == 0 )
			return RGB( 0, 0, 0 );

		return m_pDC->GetTextColor();
	}

	COLORREF SetTextColor( COLORREF crColor )
	{
		if( m_pDC == 0 )
			return RGB( 0, 0, 0 );

		return m_pDC->SetTextColor( crColor );
	}

	CGdiObject* SelectStockObject( int nIndex )
	{
		if( m_pDC == 0 )
			return 0;

		return m_pDC->SelectStockObject( nIndex );
	}

	bool IsPrinting( void )
	{
		return false;
	}

	int GetColorCount( void )
	{
		if( m_pDC != 0 )
			return 0;

		return m_pDC->GetDeviceCaps( NUMCOLORS );
	}

	void Attach( HDC hDeviceContext ) {}
	void Detach( void ) {}

	void SetAttribDC( HDC hDeviceContext )
	{
		if( m_pDC == 0 )
			return;

		m_pDC->SetAttribDC( hDeviceContext );
	}

	virtual HDC GetSafeHdc( void )
	{
		if( m_pDC == 0 )
			return (HDC)INVALID_HANDLE_VALUE;

		return m_pDC->GetSafeHdc();
	}

	virtual CDC *GetDC( void )
	{
		return m_pDC;
	}

	virtual int SetBkMode( int nBkMode )
	{
		if( m_pDC == 0 )
			return 0;

		return m_pDC->SetBkMode( nBkMode );
	}

	virtual COLORREF SetBkColor( COLORREF crColor )
	{
		if( m_pDC == 0 )
			return RGB( 0, 0, 0 );

		return m_pDC->SetBkColor( crColor );
	}

	bool PatBlt( int x, int y, int nWidth, int nHeight, DWORD dwRop )
	{
		return false;
	}

	bool CreateCompatibleDC( CDC *pDC, CRect *pRect )
	{
		return false;
	}

	int SetArcDirection( int Direction )
	{
		int Old = m_ArcDirection;
		m_ArcDirection = Direction;
		return Old;
	}

	bool FillRgn( CFPoint *pPoints, int Count, CBrush* pBrush )
	{
		return false;
		//return m_pDC->FillRgn( pRgn, pBrush ) != 0;
	}

	void SaveToFile( const char *pFileName ) {}

	CFPoint MoveTo( double x, double y )
	{
		Scale( x, y );
		m_CurrentPosition = CFPoint( x, y );
		return m_CurrentPosition;
	}

	CFPoint LineTo( double x, double y )
	{
		Scale( x, y );

		m_DXFFile.DXF_Line( m_CurrentPosition.x, m_CurrentPosition.y, 0, x, y, 0 );
		m_CurrentPosition = CFPoint( x, y );

		return m_CurrentPosition;
	}

	CFPoint DrawLine( CFLine Line )
	{
		MoveTo( Line.GetStart() );
		return LineTo( Line.GetEnd() );
	}

	CFPoint DrawLine( CFPoint Point1, CFPoint Point2 )
	{
		return DrawLine( CFLine( Point1, Point2 ) );
	}

	bool TextOut( double x, double y, const CString& str )
	{
		Scale( x, y );

		unsigned int Alignment = m_pDC->GetTextAlign();
		// DXF text is different from Windows GDI text so some adjustments are made
		// to the coordinates to get things that look like we expect.
		if( ( Alignment & ( TA_BOTTOM | TA_BASELINE ) ) == 0 ) // MS used 0 for TA_TOP! How crappy is that for doing this test?
		{
			x += m_FontHeight * 0.05;
			y -= m_FontHeight * 0.66;
		}
		else
		{
			x += m_FontHeight * 0.05;
			if( ( Alignment & TA_BASELINE ) == TA_BASELINE )
				y += m_FontHeight * 0.25;
			else
				y += m_FontHeight * 0.35;
		}

		m_DXFFile.DXF_Text( x, y, 0, m_FontHeight * 0.6, (const char*)str, ( Alignment & TA_CENTER ) != 0 );
		return true;
	}

	bool Circle( CFCircle &Circle )
	{
		double x = Circle.x;
		double y = Circle.y;
		double r = Circle.r;

		Scale( x, y );
		Scale( r );

		m_DXFFile.DXF_Circle( x, y, 0, r );

		return true;
	}

	bool Arc( CFArc &TheArc )
	{
		return Arc( TheArc.x, TheArc.y, TheArc.r, TheArc.m_Start.x, TheArc.m_Start.y, TheArc.m_End.x, TheArc.m_End.y );
	}

	bool Arc( double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4, bool bDrawTo = false )
	{
		if( bDrawTo )
			LineTo( CFPoint( x3, y3 ) );

		Scale( x1, y1 );
		Scale( x2, y2 );
		Scale( x3, y3 );
		Scale( x4, y4 );

		if( x3 == x4 && y3 == y4 )
			m_DXFFile.DXF_Circle( ( x1 + x2 ) / 2, ( y1 + y2 ) / 2, 0, ( x2 - x1 ) / 2 );
		else
		{
			double StartAngle = GetAngle( CFPoint( ( x1 + x2 ) / 2, ( y1 + y2 ) / 2 ), CFPoint( x3, y3 ) );
			double EndAngle = GetAngle( CFPoint( ( x1 + x2 ) / 2, ( y1 + y2 ) / 2 ), CFPoint( x4, y4 ) );
			m_DXFFile.DXF_Arc( ( x1 + x2 ) / 2, ( y1 + y2 ) / 2, 0, ( x2 - x1 ) / 2, EndAngle, StartAngle );
		}

		if( bDrawTo )
			m_CurrentPosition = CFPoint( x4, y4 );

		return true;
	}

	bool Arc( double x, double y, double r, double x3, double y3, double x4, double y4, bool bDrawTo = false )
	{
		double x2 = x + r;
		double y2 = y + r;
		x -= r;
		y -= r;
		return Arc( x, y, x2, y2, x3, y3, x4, y4, bDrawTo );
	}

	bool Pie( double x, double y, double r, double x3, double y3, double x4, double y4, bool bDrawTo )
	{
		Scale( x, y );
		Scale( r );
		Scale( x3, y3 );
		Scale( x4, y4 );

		double Angle = GetAngle( CFPoint( x, y ), CFPoint( x3, y3 ), CFPoint( x4, y4 ) );
		m_DXFFile.DXF_Arc( x, y, 0, r, Angle, Angle );

		return true;
	}

	bool DrawRect( double RotationAngle, double x1, double y1, double x2, double y2 )
	{
		// Negate the rotation for DXF files.
		double Deviation = RotationAngle;
		while( Deviation > 89 )
			Deviation -= 90;
		if( fabs( Deviation ) < .1 )
			return DrawRect( x1, y1, x2, y2 );

		CFPoint Center( ( x1 + x2 ) / 2.0, ( y1 + y2 ) / 2.0 );

		// The addition of .1 causes the display to look better when rounding to
		// individual pixels. Without this, the rectange appears to rotate sooner
		// than expected in comparison to other things.

		CFPoint Point1( x1, y1 );
		CFPoint Point2( x2, y1 );
		CFPoint Point3( x2, y2 );
		CFPoint Point4( x1, y2 );

		Point1.RotateAround( Center, -RotationAngle );
		Point2.RotateAround( Center, -RotationAngle );
		Point3.RotateAround( Center, -RotationAngle );
		Point4.RotateAround( Center, -RotationAngle );

		double CoordinateArray[8];

		CoordinateArray[0] = Point1.x;
		CoordinateArray[1] = Point1.y;
		CoordinateArray[2] = Point2.x;
		CoordinateArray[3] = Point2.y;
		CoordinateArray[4] = Point3.x;
		CoordinateArray[5] = Point3.y;
		CoordinateArray[6] = Point4.x;
		CoordinateArray[7] = Point4.y;

		m_DXFFile.DXF_Polyline( 4, CoordinateArray, true );
		return TRUE;
	}

	bool DrawRect( double x1, double y1, double x2, double y2 )
	{
		Scale( x1, y1 );
		Scale( x2, y2 );

		double CoordinateArray[8];

		CoordinateArray[0] = x1;
		CoordinateArray[1] = y1;
		CoordinateArray[2] = x2;
		CoordinateArray[3] = y1;
		CoordinateArray[4] = x2;
		CoordinateArray[5] = y2;
		CoordinateArray[6] = x1;
		CoordinateArray[7] = y2;

		m_DXFFile.DXF_Polyline( 4, CoordinateArray, true );

		return true;
	}

	bool DrawRect( const CFRect &Rect )
	{
		return DrawRect( Rect.left, Rect.top, Rect.right, Rect.bottom );
	}

	void FillRect( CFRect* pRect, CBrush* pBrush )
	{
		DrawRect( pRect->left, pRect->top, pRect->right, pRect->bottom );
	}

	//COLORREF SetPixel( double x, double y, COLORREF crColor )
	//{
	//	return RGB( 0, 0, 0 );
	//}

	virtual int ExpandPolygonCorner( CFPoint &Point, CFPoint &PreviousPoint, CFPoint &NextPoint, double Distance, CFPoint &NewPoint1, CFPoint &NewPoint2 )
	{
		double Angle = GetAngle( Point, PreviousPoint, NextPoint );

		bool bChangeDirection = false;

		CFLine Line1( PreviousPoint, Point );
		CFLine Perp1;
		Line1.PerpendicularLine( Perp1, bChangeDirection ? -1 : 1 );

		CFLine Line2( NextPoint, Point );
		CFLine Perp2;
		Line2.PerpendicularLine( Perp2, bChangeDirection ? 1 : -1 );

		Perp1.SetDistance( Distance );
		Perp2.SetDistance( Distance );

		NewPoint1 = Perp1.GetEnd();
		NewPoint2 = Perp2.GetEnd();

		return bChangeDirection ? 1 : -1;
	}

	CPen* SelectObject( CPen* pPen )
	{
		if( m_Scale <= 1 )
			return m_pDC->SelectObject( pPen );

		EXTLOGPEN LogPen;
		if( pPen == 0 || pPen->GetExtLogPen( &LogPen ) == 0 )
			return m_pDC->SelectObject( pPen );
		else
		{
			/*
			 * Check for a pen that we created.
			 */

			POSITION Position = m_CreatedPens.GetHeadPosition();
			while( Position )
			{
				if( pPen == m_CreatedPens.GetNext( Position ) )
					return m_pDC->SelectObject( pPen );
			}

			/*
			 * Check for non-solid pens and don't scale their width since
			 * larger pens cannot be non-solid at this time.
			 */
			if( LogPen.elpPenStyle != PS_SOLID )
			{
				double Viewport = m_pDC->GetViewportExt().cx;
				double Window = m_pDC->GetWindowExt().cx;
				double ExtraScale = Window / Viewport;

				LOGBRUSH LogBrush;
				LogBrush.lbColor = LogPen.elpColor;
				LogBrush.lbStyle = PS_SOLID;

				const DWORD DashStyle[2] = { (DWORD)( 4 * m_Scale * m_DPIScale ), (DWORD)( 4 * m_Scale * m_DPIScale ) };

				CPen *pPen = new CPen( PS_GEOMETRIC | PS_USERSTYLE, (int)( LogPen.elpWidth * m_Scale * m_DPIScale ), &LogBrush, 2, DashStyle );

				m_CreatedPens.AddTail( pPen );

				return m_pDC->SelectObject( pPen );
			}

			LogPen.elpWidth = (int)( LogPen.elpWidth * m_Scale * m_DPIScale );

			CPen *pPen = new CPen( LogPen.elpPenStyle, LogPen.elpWidth, LogPen.elpColor );
			if( pPen == 0 )
				return m_pDC->SelectObject( pPen );

			m_CreatedPens.AddTail( pPen );

			return m_pDC->SelectObject( pPen );
		}
		return 0;
	}

	CFont* SelectObject( CFont* pFont, double FontHeight )
	{
		// Not using GDI renderer so ignore pFont.
		m_FontHeight = FontHeight * m_Scale * m_DPIScale;
		return 0;
	}

	CBitmap* SelectObject( CBitmap* pBitmap )
		{ return m_pDC->SelectObject( pBitmap ); }

	int SelectObject( CRgn* pRgn )
		{ return m_pDC->SelectObject( pRgn ); }

	CBrush* SelectObject( CBrush *pBrush )
		{ return m_pDC->SelectObject( pBrush ); }

	CFPoint GetTextExtent( const char *pString, int Count )
	{
		CSize Size = m_pDC->GetTextExtent( pString, Count );
		CFPoint Result( Size.cx, Size.cy );
		// Only adjust the scaling and not the position.
		Result.x /= m_Scale * m_DPIScale;
		Result.y /= m_Scale * m_DPIScale;
		return Result;
	}

	int GetPenSize( int UnscaledSize )
	{
		return max( (int)( UnscaledSize * m_Scale * m_DPIScale ), 1 );
	}

	bool DrawArrow( CFPoint FromPoint, CFPoint ToPoint, double Width, double Length )
	{
		CPen* pPen = m_pDC->GetCurrentPen();
		LOGPEN LogPen;
		if( pPen->GetLogPen( &LogPen ) == 0 )
			return FALSE;
		COLORREF PenColor = LogPen.lopnColor;
		double Red = GetRValue( PenColor ) / 255.0;
		double Green = GetGValue( PenColor ) / 255.0;
		double Blue = GetBValue( PenColor ) / 255.0;

		Width = Scale( Width );
		Length = Scale( Length );

		CFLine Line( ToPoint, FromPoint );
		Line.SetDistance( Length );

		CBrush Brush;
		Red *= 0.85;
		Green *= 0.85;
		Blue *= 0.85;
		COLORREF NewColor = RGB( (int)( Red * 255.0 ), (int)( Green * 255.0 ), (int)( Blue * 255.0 ) );
		Brush.CreateSolidBrush( NewColor );

		CFLine CrossLine;
		Line.PerpendicularLine( CrossLine, Width / 2, 1 );
		CrossLine.ReverseDirection();
		CrossLine.SetDistance( Width );
		CPen *pOldPen = (CPen*)m_pDC->SelectStockObject( NULL_PEN );

		DrawLine( CrossLine.GetStart(), CrossLine.GetEnd() );
		DrawLine( CrossLine.GetEnd(), ToPoint );
		DrawLine( ToPoint, CrossLine.GetStart() );

//		CRgn Region;
//		Region.CreatePolygonRgn( Points, 3, ALTERNATE );
//		m_pDC->FillRgn( &Region, &Brush );
//		m_pDC->SelectObject( pOldPen );

		return TRUE;
	}
};
#if defined( LINKAGE_USE_DIRECT2D )

class CD2DRenderer : public CRendererImplementation
{
	private:

	static ID2D1Factory* GetD2D1Factory( void )
	{
		static ID2D1Factory* pD2DFactory = 0;
		if( pD2DFactory == 0 )
		{
			HRESULT hr = D2D1CreateFactory( D2D1_FACTORY_TYPE_SINGLE_THREADED, &pD2DFactory );
		}
		return pD2DFactory;
	}

	static IDWriteFactory* GetDWriteFactory( void )
	{
		static IDWriteFactory *pDWriteFactor = 0;
		if( pDWriteFactor == 0 )
		{
			HRESULT hr = DWriteCreateFactory( DWRITE_FACTORY_TYPE_SHARED, __uuidof(pDWriteFactor), reinterpret_cast<IUnknown **>(&pDWriteFactor) );
		}
		return pDWriteFactor;
	}

	CList<CPen*,CPen*> m_CreatedPens;
	ID2D1StrokeStyle *m_pSolidLineStroke = 0;
	int m_ArcDirection;
	CDC *m_pDC;
	bool m_bIsPrinting;
	bool m_bCreatedFont;
	LOGFONT m_ScaledLogFont;
	CFont m_ScaledFont;
	CPen m_ScaledPen;
	CFPoint m_CurrentPosition;
	ID2D1DCRenderTarget * m_pRenderTarget;
	CRect m_Rect;
	IDWriteTextFormat* m_pTextFormat;
	double m_FontHeight;
	unsigned int m_TextAlign;
	CString m_FontName;

	public:

	CD2DRenderer( bool bIsPrinting )
	{
		m_bIsPrinting = bIsPrinting;
		m_ScrollPosition.SetPoint( 0, 0 );
		m_Scale = 1.0;
		m_bCreatedFont = false;
		m_pDC = 0;
		m_pRenderTarget = 0;
		m_pTextFormat = 0;
		m_FontHeight = 0;
		m_TextAlign = TA_LEFT | TA_BOTTOM;
		m_ArcDirection = AD_COUNTERCLOCKWISE;
		memset( &m_ScaledLogFont, 0, sizeof( m_ScaledLogFont ) );

		ID2D1Factory* pD2D1Factory = GetD2D1Factory();

		pD2D1Factory->CreateStrokeStyle( D2D1::StrokeStyleProperties( D2D1_CAP_STYLE_SQUARE, D2D1_CAP_STYLE_SQUARE), 0, 0, &m_pSolidLineStroke );
	}

	virtual ~CD2DRenderer()
	{
		POSITION Position = m_CreatedPens.GetHeadPosition();
		while( Position )
		{
			CPen *pPen = m_CreatedPens.GetNext( Position );
			if( pPen != 0 )
				delete pPen;
		}
		m_CreatedPens.RemoveAll();
		if( m_pDC != 0 )
			delete m_pDC;

		m_pSolidLineStroke->Release();
		m_pRenderTarget->Release();
	}

	virtual void BeginDraw( void ) 
	{
		if( m_pRenderTarget == 0 )
			return;
		m_pRenderTarget->BeginDraw();
	}

	virtual void EndDraw( void )
	{
		if( m_pRenderTarget == 0 )
			return;
		m_pRenderTarget->EndDraw();
	}

	int GetYOrientation( void )
	{
		return -1;
	}

	void SaveDXF( const char *pFileName ) {}

	CFPoint MoveTo( CFPoint Point ) { return MoveTo( Point.x, Point.y ); }
	CFPoint LineTo( CFPoint Point ) { return LineTo( Point.x, Point.y ); }

	unsigned int SetTextAlign( unsigned int nFlags )
	{
		m_TextAlign = nFlags;
		return 0;
	}

	virtual void FillRect( LPCRECT lpRect, CBrush* pBrush )
	{
		if( m_pDC == 0 )
			return;

		m_pDC->FillRect( lpRect, pBrush );
	}

	COLORREF GetTextColor( void )
	{
		if( m_pDC == 0 )
			return RGB( 0, 0, 0 );

		return m_pDC->GetTextColor();
	}

	COLORREF SetTextColor( COLORREF crColor )
	{
		if( m_pDC == 0 )
			return RGB( 0, 0, 0 );

		return m_pDC->SetTextColor( crColor );
	}

	CGdiObject* SelectStockObject( int nIndex )
	{
		if( m_pDC == 0 )
			return 0;

		return m_pDC->SelectStockObject( nIndex );
	}

	bool IsPrinting( void )
	{
		return m_bIsPrinting;
	}

	int GetColorCount( void )
	{
		if( m_pDC != 0 )
			return 0;

		return m_pDC->GetDeviceCaps( NUMCOLORS );
	}

	void Attach( HDC hDeviceContext )
	{
		if( m_pDC != 0 )
			return;

		m_pDC = new CDC;
		m_pDC->Attach( hDeviceContext );
	}

	void Detach( void )
	{
		if( m_pDC == 0 )
			return;

		m_pDC->Detach();
	}

	void SetAttribDC( HDC hDeviceContext )
	{
		if( m_pDC == 0 )
			return;

		m_pDC->SetAttribDC( hDeviceContext );
	}

	virtual HDC GetSafeHdc( void )
	{
		if( m_pDC == 0 )
			return (HDC)INVALID_HANDLE_VALUE;

		return m_pDC->GetSafeHdc();
	}

	virtual CDC *GetDC( void )
	{
		return m_pDC;
	}

	virtual int SetBkMode( int nBkMode )
	{
		if( m_pDC == 0 )
			return 0;

		return m_pDC->SetBkMode( nBkMode );
	}

	virtual COLORREF SetBkColor( COLORREF crColor )
	{
		if( m_pDC == 0 )
			return RGB( 0, 0, 0 );

		return m_pDC->SetBkColor( crColor );
	}

	bool PatBlt( int x, int y, int nWidth, int nHeight, DWORD dwRop )
	{
		if( m_pDC == 0 )
			return false;

		return m_pDC->PatBlt( x, y, nWidth, nHeight, dwRop ) != 0;
	}

	bool CreateCompatibleDC( CDC *pDC, CRect *pRect )
	{
		m_pDC = new CDC;
		if( m_pDC == 0 )
			return false;
		bool bResult = m_pDC->CreateCompatibleDC( pDC ) != 0;

		_ASSERT( pRect != 0 );

		if( !bResult || pRect == 0 )
			return false;


		int PPI = m_pDC->GetDeviceCaps( LOGPIXELSX );
		m_DPIScale = (double)PPI / 96.0;

		m_Rect.SetRect( pRect->left, pRect->top, pRect->right, pRect->bottom );

		return true;
	}

	int SetArcDirection( int Direction )
	{
		int Old = m_ArcDirection;
		m_ArcDirection = Direction;
		return Old;
	}

	bool FillRgn( CFPoint *pPoints, int Count, CBrush* pBrush )
	{
		if( Count <= 0 || pPoints == 0 )
			return false;

		LOGBRUSH LogBrush;
		if( pBrush->GetLogBrush( &LogBrush ) == 0 )
			return FALSE;
		COLORREF BrushColor = LogBrush.lbColor;
		float Red = GetRValue( BrushColor ) / 255.0f;
		float Green = GetGValue( BrushColor ) / 255.0f;
		float Blue = GetBValue( BrushColor ) / 255.0f;

		D2D1_COLOR_F UseColor;
		UseColor.r = Red;
		UseColor.g = Green;
		UseColor.b = Blue;
		UseColor.a = 1;  // Opaque.

		CComPtr<ID2D1SolidColorBrush> pTempBrush = NULL;
		m_pRenderTarget->CreateSolidColorBrush( UseColor, &pTempBrush );

		ID2D1PathGeometry *pRegionGeometry = 0;
		ID2D1Factory* pD2D1Factory = GetD2D1Factory();
		pD2D1Factory->CreatePathGeometry( &pRegionGeometry ) ;

	    ID2D1GeometrySink *pSink = NULL;
		pRegionGeometry->Open(&pSink);
        pSink->SetFillMode( D2D1_FILL_MODE_WINDING );

        pSink->BeginFigure( D2D1::Point2F( (float)pPoints[0].x, (float)pPoints[0].y ), D2D1_FIGURE_BEGIN_FILLED );

		for( int Index = 1; Index < Count; ++Index )
			pSink->AddLine( D2D1::Point2F( (float)pPoints[Index].x, (float)pPoints[Index].y ) );

		pSink->EndFigure( D2D1_FIGURE_END_CLOSED );
		pSink->Close();

		m_pRenderTarget->FillGeometry( pRegionGeometry, pTempBrush );

		pSink->Release();
		pRegionGeometry->Release();

		return true;
	}

	void SaveToFile( const char *pFileName ) {}

	CFPoint MoveTo( double x, double y )
	{
		CFPoint Old = m_CurrentPosition;
		Scale( x, y );
		m_CurrentPosition.x = x;
		m_CurrentPosition.y = y;
		return Old;
	}

	CFPoint LineTo( double x, double y )
	{
		double x1 = x;
		double y1 = y;
		Scale( x1, y1 );

		CPen* pPen = m_pDC->GetCurrentPen();
		LOGPEN LogPen;
		if( pPen->GetLogPen( &LogPen ) == 0 )
			return FALSE;
		COLORREF PenColor = LogPen.lopnColor;
		float Red = GetRValue( PenColor ) / 255.0f;
		float Green = GetGValue( PenColor ) / 255.0f;
		float Blue = GetBValue( PenColor ) / 255.0f;

		D2D1_COLOR_F UseColor;
		UseColor.r = Red;
		UseColor.g = Green;
		UseColor.b = Blue;
		UseColor.a = 1;  // Opaque.

		CComPtr<ID2D1SolidColorBrush> pTempBrush = NULL;

		m_pRenderTarget->CreateSolidColorBrush( UseColor, &pTempBrush );

		m_pRenderTarget->DrawLine(
        D2D1::Point2F( (float)m_CurrentPosition.x, (float)m_CurrentPosition.y ),
        D2D1::Point2F( (float)x1, (float)y1 ),
        pTempBrush,
        (float)( LogPen.lopnWidth.x * m_DPIScale ),
		m_pSolidLineStroke );

		m_CurrentPosition.SetPoint( x1, y1 );

		return CFPoint( x, y );
	}

	CFPoint DrawLine( CFLine Line )
	{
		MoveTo( Line.GetStart() );
		return LineTo( Line.GetEnd() );
	}

	CFPoint DrawLine( CFPoint Point1, CFPoint Point2 )
	{
		MoveTo( Point1 );
		return LineTo( Point2 );
	}

	bool TextOut( double x, double y, const CString& str )
	{
		Scale( x, y );

		COLORREF Color = m_pDC->GetTextColor();
		float Red = GetRValue( Color ) / 255.0f;
		float Green = GetGValue( Color ) / 255.0f;
		float Blue = GetBValue( Color ) / 255.0f;

		D2D1_COLOR_F UseColor;
		UseColor.r = Red;
		UseColor.g = Green;
		UseColor.b = Blue;
		UseColor.a = 1;  // Opaque.

		CComPtr<ID2D1SolidColorBrush> pTempBrush = NULL;
		m_pRenderTarget->CreateSolidColorBrush( UseColor, &pTempBrush );

		// Convert to a wchar_t* from CStringA
		const size_t newsizea = (str.GetLength() + 1);
		size_t convertedCharsa = 0;
		wchar_t *wcstring = new wchar_t[newsizea];
		mbstowcs_s( &convertedCharsa, wcstring, newsizea, str, _TRUNCATE);

		CFRect Rect( x, y, x, y );

		DWRITE_TEXT_ALIGNMENT DWAlign = DWRITE_TEXT_ALIGNMENT_CENTER;
		DWRITE_PARAGRAPH_ALIGNMENT DAPAlign = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;

		const unsigned int HORIZONTAL_MASK = TA_LEFT | TA_RIGHT | TA_CENTER;
		const unsigned int VERTICAL_MASK = TA_TOP | TA_BOTTOM | TA_BASELINE;
		
		if( ( m_TextAlign & HORIZONTAL_MASK ) == TA_LEFT )
		{
			DWAlign = DWRITE_TEXT_ALIGNMENT_LEADING;
			Rect.right += 1000;
		}
		else if( ( m_TextAlign & HORIZONTAL_MASK ) == TA_CENTER )
		{
			DWAlign = DWRITE_TEXT_ALIGNMENT_CENTER;
			Rect.right += 500;
			Rect.left -= 500;
		}
		else if( ( m_TextAlign & HORIZONTAL_MASK ) == TA_RIGHT )
		{
			DWAlign = DWRITE_TEXT_ALIGNMENT_TRAILING;
			Rect.left -= 1000;
		}

		if( ( m_TextAlign & VERTICAL_MASK ) == TA_TOP )
		{
			DAPAlign = DWRITE_PARAGRAPH_ALIGNMENT_NEAR;
			Rect.bottom += 1000;
		}
		else if( ( m_TextAlign & VERTICAL_MASK ) == TA_BASELINE )
		{
			DAPAlign = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
			Rect.top -= 500;
			Rect.bottom += 500;
		}
		else if( ( m_TextAlign & VERTICAL_MASK ) == TA_BOTTOM )
		{
			DAPAlign = DWRITE_PARAGRAPH_ALIGNMENT_FAR;
			Rect.top -= 1000;
		}

		m_pTextFormat->SetTextAlignment( DWAlign );
		m_pTextFormat->SetParagraphAlignment( DAPAlign );

		D2D1_RECT_F layoutRect = D2D1::RectF( (float)Rect.left, (float)Rect.top, (float)Rect.right, (float)Rect.bottom );

		D2D1_MATRIX_3X2_F Transform;
		if( 0 )
		{
			m_pRenderTarget->GetTransform( &Transform );

			// set a xxx degree rotation around the (100,100);
			m_pRenderTarget->SetTransform( D2D1::Matrix3x2F::Rotation( 45, D2D1::Point2F((float)x,(float)y))); 
		}

		m_pRenderTarget->DrawText( 
			wcstring,        // The string to render.
			convertedCharsa,    // The string's length.
			m_pTextFormat,    // The text format.
			&layoutRect,       // The region of the window where the text will be rendered.
			pTempBrush     // The brush used to draw the text.
			);

		if( 0 )
			m_pRenderTarget->SetTransform( Transform); 

		return true;
	}

	bool Circle( CFCircle &Circle )
	{
		double x1 = Circle.x - Circle.r;
		double y1 = Circle.y - Circle.r;
		double x2 = Circle.x + Circle.r;
		double y2 = Circle.y + Circle.r;

		return Arc( x1, y1, x2, y2, y1, x1, y1, x1, false );
	}

	bool Arc( CFArc &TheArc )
	{
		return Arc( TheArc.x, TheArc.y, TheArc.r, TheArc.m_Start.x, TheArc.m_Start.y, TheArc.m_End.x, TheArc.m_End.y );
	}

	bool Arc( double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4, bool bDrawTo = false )
	{
		if( bDrawTo )
			LineTo( x3, y3 );

		Scale( x1, y1 );
		Scale( x2, y2 );
		Scale( x3, y3 );
		Scale( x4, y4 );

		// Convert the box-type arc drawing of GDI into the start/end info for the Direct2D ArcSegment.
		CFPoint Center( ( x2 + x1 ) / 2, ( y2 + y1 ) / 2 );
		double Radius = fabs( x2 - x1 ) / 2;
		double StartAngle = GetAngle( Center, CFPoint( x3, y3 ) );
		double EndAngle = GetAngle( Center, CFPoint( x4, y4 ) );

		CPen* pPen = m_pDC->GetCurrentPen();
		LOGPEN LogPen;
		if( pPen->GetLogPen( &LogPen ) == 0 )
			return FALSE;
		COLORREF PenColor = LogPen.lopnColor;
		float Red = GetRValue( PenColor ) / 255.0f;
		float Green = GetGValue( PenColor ) / 255.0f;
		float Blue = GetBValue( PenColor ) / 255.0f;

		D2D1_COLOR_F UseColor;
		UseColor.r = Red;
		UseColor.g = Green;
		UseColor.b = Blue;
		UseColor.a = 1;  // Opaque.

		CComPtr<ID2D1SolidColorBrush> pTempBrush = NULL;
		m_pRenderTarget->CreateSolidColorBrush( UseColor, &pTempBrush );

		ID2D1PathGeometry *pCircleGeometry = 0;
		ID2D1Factory* pD2D1Factory = GetD2D1Factory();
		pD2D1Factory->CreatePathGeometry( &pCircleGeometry ) ;

	    ID2D1GeometrySink *pSink = NULL;
		pCircleGeometry->Open(&pSink);
        pSink->SetFillMode( D2D1_FILL_MODE_WINDING );

		if( StartAngle == EndAngle )
		{
	        pSink->BeginFigure( D2D1::Point2F( (float)( Center.x - Radius ), (float)Center.y ), D2D1_FIGURE_BEGIN_HOLLOW );

			// Add the top half circle
			pSink->AddArc(
				D2D1::ArcSegment(
				D2D1::Point2F( (float)( Center.x + Radius ), (float)Center.y ), // end point of the top half circle, also the start point of the bottom half circle
				D2D1::SizeF((float)Radius, (float)Radius), // radius
				0.0f, // rotation angle
				D2D1_SWEEP_DIRECTION_CLOCKWISE,
				D2D1_ARC_SIZE_SMALL
				));            

			// Add the bottom half circle
			pSink->AddArc(
				D2D1::ArcSegment(
				D2D1::Point2F( (float)( Center.x - Radius ), (float)Center.y ), // end point of the bottom half circle
				D2D1::SizeF((float)Radius, (float)Radius),   // radius of the bottom half circle, same as previous one.
				0.0f, // rotation angle
				D2D1_SWEEP_DIRECTION_CLOCKWISE,
				D2D1_ARC_SIZE_SMALL
				));       

			pSink->EndFigure( D2D1_FIGURE_END_CLOSED );
		}
		else
		{
			D2D1_SWEEP_DIRECTION SweepDirection = m_ArcDirection == AD_CLOCKWISE ? D2D1_SWEEP_DIRECTION_CLOCKWISE : D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE;

			double TotalAngle = 0;
			if( m_ArcDirection == AD_CLOCKWISE )
			{
				if( StartAngle > EndAngle )
					TotalAngle = ( 360 - StartAngle ) + EndAngle;
				else
					TotalAngle = EndAngle - StartAngle;
			}
			else
			{
				if( StartAngle > EndAngle )
					TotalAngle = StartAngle - EndAngle;
				else
					TotalAngle = ( 360 - EndAngle ) + StartAngle;
			}


	        pSink->BeginFigure( D2D1::Point2F( (float)x3, (float)y3 ), D2D1_FIGURE_BEGIN_HOLLOW );
			pSink->AddArc(
				D2D1::ArcSegment(
				D2D1::Point2F( (float)x4, (float)y4 ), // end point of the top half circle, also the start point of the bottom half circle
				D2D1::SizeF((float)Radius, (float)Radius), // radius
				0.0f, // rotation angle
				SweepDirection,
				TotalAngle >= 180 ? D2D1_ARC_SIZE_LARGE : D2D1_ARC_SIZE_SMALL
				));            

			pSink->EndFigure( D2D1_FIGURE_END_OPEN );
		}

		pSink->Close();

		m_pRenderTarget->DrawGeometry( pCircleGeometry, pTempBrush, (float)( LogPen.lopnWidth.x * m_DPIScale ), m_pSolidLineStroke );

		pSink->Release();
		pCircleGeometry->Release();

		m_CurrentPosition.SetPoint( x4, y4 );

		return true;
	}

	bool Arc( double x, double y, double r, double x3, double y3, double x4, double y4, bool bDrawTo = false )
	{
		double x2 = x + r;
		double y2 = y + r;
		x -= r;
		y -= r;
		return Arc( x, y, x2, y2, x3, y3, x4, y4, bDrawTo );
	}

	bool Pie( double x, double y, double r, double x3, double y3, double x4, double y4, bool bDrawTo )
	{
		if( bDrawTo )
			LineTo( x3, y3 );

		Scale( x, y );
		Scale( r );
		Scale( x3, y3 );
		Scale( x4, y4 );

		// Convert the box-type arc drawing of GDI into the start/end info for the Direct2D ArcSegment.
		CFPoint Center( x, y );
		double Radius = r;
		double StartAngle = GetAngle( Center, CFPoint( x3, y3 ) );
		double EndAngle = GetAngle( Center, CFPoint( x4, y4 ) );

		CBrush *pBrush = m_pDC->GetCurrentBrush();
		LOGBRUSH LogBrush;
		if( pBrush->GetLogBrush( &LogBrush ) == 0 )
			return FALSE;
		COLORREF BrushColor = LogBrush.lbColor;
		float Red = GetRValue( BrushColor ) / 255.0f;
		float Green = GetGValue( BrushColor ) / 255.0f;
		float Blue = GetBValue( BrushColor ) / 255.0f;

		D2D1_COLOR_F UseColor;
		UseColor.r = Red;
		UseColor.g = Green;
		UseColor.b = Blue;
		UseColor.a = 1;  // Opaque.

		CComPtr<ID2D1SolidColorBrush> pTempBrush = NULL;
		m_pRenderTarget->CreateSolidColorBrush( UseColor, &pTempBrush );

		ID2D1PathGeometry *pCircleGeometry = 0;
		ID2D1Factory* pD2D1Factory = GetD2D1Factory();
		pD2D1Factory->CreatePathGeometry( &pCircleGeometry ) ;

	    ID2D1GeometrySink *pSink = NULL;
		pCircleGeometry->Open(&pSink);
        pSink->SetFillMode( D2D1_FILL_MODE_WINDING );

		if( StartAngle == EndAngle )
		{
	        pSink->BeginFigure( D2D1::Point2F( (float)( Center.x - Radius ), (float)Center.y ), D2D1_FIGURE_BEGIN_FILLED );

			// Add the top half circle
			pSink->AddArc(
				D2D1::ArcSegment(
				D2D1::Point2F( (float)( Center.x + Radius ), (float)Center.y ), // end point of the top half circle, also the start point of the bottom half circle
				D2D1::SizeF((float)Radius, (float)Radius), // radius
				0.0f, // rotation angle
				D2D1_SWEEP_DIRECTION_CLOCKWISE,
				D2D1_ARC_SIZE_SMALL
				));            

			// Add the bottom half circle
			pSink->AddArc(
				D2D1::ArcSegment(
				D2D1::Point2F( (float)( Center.x - Radius ), (float)Center.y ), // end point of the bottom half circle
				D2D1::SizeF((float)Radius, (float)Radius),   // radius of the bottom half circle, same as previous one.
				0.0f, // rotation angle
				D2D1_SWEEP_DIRECTION_CLOCKWISE,
				D2D1_ARC_SIZE_SMALL
				));       

			pSink->EndFigure( D2D1_FIGURE_END_CLOSED );
		}
		else
		{
			D2D1_SWEEP_DIRECTION SweepDirection = m_ArcDirection == AD_CLOCKWISE ? D2D1_SWEEP_DIRECTION_CLOCKWISE : D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE;

			double TotalAngle = 0;
			if( m_ArcDirection == AD_CLOCKWISE )
			{
				if( StartAngle > EndAngle )
					TotalAngle = ( 360 - StartAngle ) + EndAngle;
				else
					TotalAngle = EndAngle - StartAngle;
			}
			else
			{
				if( StartAngle > EndAngle )
					TotalAngle = StartAngle - EndAngle;
				else
					TotalAngle = ( 360 - EndAngle ) + StartAngle;
			}


	        pSink->BeginFigure( D2D1::Point2F( (float)x3, (float)y3 ), D2D1_FIGURE_BEGIN_FILLED );
			pSink->AddArc(
				D2D1::ArcSegment(
				D2D1::Point2F( (float)x4, (float)y4 ), // end point of the top half circle, also the start point of the bottom half circle
				D2D1::SizeF((float)Radius, (float)Radius), // radius
				0.0f, // rotation angle
				SweepDirection,
				TotalAngle >= 180 ? D2D1_ARC_SIZE_LARGE : D2D1_ARC_SIZE_SMALL
				));            

			pSink->EndFigure( D2D1_FIGURE_END_CLOSED );
		}

		pSink->Close();

		m_pRenderTarget->FillGeometry( pCircleGeometry, pTempBrush );

		pSink->Release();
		pCircleGeometry->Release();

		m_CurrentPosition.SetPoint( x4, y4 );

		return true;
	}

	bool DrawRect( double RotationAngle, double x1, double y1, double x2, double y2 )
	{
		double Deviation = RotationAngle;
		while( Deviation > 89 )
			Deviation -= 90;
		if( fabs( Deviation ) < .1 )
	//	if( RotationAngle == 0 || RotationAngle == 90 || RotationAngle == 180 || RotationAngle == 270 )
			return DrawRect( x1, y1, x2, y2 );

		CFPoint Center( ( x1 + x2 ) / 2.0, ( y1 + y2 ) / 2.0 );

		// The addition of .1 causes the display to look better when rounding to
		// individual pixels. Without this, the rectange appears to rotate sooner
		// than expected in comparison to other things.

		CFPoint Point1( x1 + .1, y1 + .1 );
		CFPoint Point2( x2 + .1, y1 + .1 );
		CFPoint Point3( x2 + .1, y2 + .1 );
		CFPoint Point4( x1 + .1, y2 + .1 );

		Point1.RotateAround( Center, RotationAngle );
		Point2.RotateAround( Center, RotationAngle );
		Point3.RotateAround( Center, RotationAngle );
		Point4.RotateAround( Center, RotationAngle );

		MoveTo( Point1 );
		LineTo( Point2 );
		LineTo( Point3 );
		LineTo( Point4 );
		LineTo( Point1 );

		return TRUE;
	}

	bool DrawRect( double x1, double y1, double x2, double y2 )
	{
		//Scale( x1, y1 );
		//Scale( x2, y2 );

		MoveTo( CFPoint( x1, y1 ) );
		LineTo( CFPoint( x2, y1 ) );
		LineTo( CFPoint( x2, y2 ) );
		LineTo( CFPoint( x1, y2 ) );
		LineTo( CFPoint( x1, y1 ) );
		return true;
	}

	bool DrawRect( const CFRect &Rect )
	{
		CFRect NewRect = Rect;
		MoveTo( CFPoint( NewRect.left, NewRect.top ) );
		LineTo( CFPoint( NewRect.right, NewRect.top ) );
		LineTo( CFPoint( NewRect.right, NewRect.bottom ) );
		LineTo( CFPoint( NewRect.left, NewRect.bottom ) );
		LineTo( CFPoint( NewRect.left, NewRect.top ) );
		return true;
	}

	void FillRect( CFRect* pRect, CBrush* pBrush )
	{
		*pRect = Scale( *pRect );

		CFPoint Points[4];
		Points[0].x = pRect->left;
		Points[0].y = pRect->top;
		Points[1].x = pRect->right;
		Points[1].y = pRect->top;
		Points[2].x = pRect->right;
		Points[2].y = pRect->bottom;
		Points[3].x = pRect->left;
		Points[3].y = pRect->bottom;

		FillRgn( Points, 4, pBrush );
	}

	CPen* SelectObject( CPen* pPen )
	{
		if( m_Scale <= 1 )
			return m_pDC->SelectObject( pPen );

		EXTLOGPEN LogPen;
		if( pPen == 0 || pPen->GetExtLogPen( &LogPen ) == 0 )
			return m_pDC->SelectObject( pPen );
		else
		{
			/*
			 * Check for a pen that we created.
			 */

			POSITION Position = m_CreatedPens.GetHeadPosition();
			while( Position )
			{
				if( pPen == m_CreatedPens.GetNext( Position ) )
					return m_pDC->SelectObject( pPen );
			}

			/*
			 * Check for non-solid pens and don't scale their width since
			 * larger pens cannot be non-solid at this time.
			 */
			if( LogPen.elpPenStyle != PS_SOLID )
			{
				double Viewport = m_pDC->GetViewportExt().cx;
				double Window = m_pDC->GetWindowExt().cx;
				double ExtraScale = Window / Viewport;

				LOGBRUSH LogBrush;
				LogBrush.lbColor = LogPen.elpColor;
				LogBrush.lbStyle = PS_SOLID;

				const DWORD DashStyle[2] = { (DWORD)( 4 * m_Scale * m_DPIScale ), (DWORD)( 4 * m_Scale * m_DPIScale ) };

				CPen *pPen = new CPen( PS_GEOMETRIC | PS_USERSTYLE, (int)( LogPen.elpWidth * m_Scale * m_DPIScale ), &LogBrush, 2, DashStyle );

				m_CreatedPens.AddTail( pPen );

				return m_pDC->SelectObject( pPen );
			}

			LogPen.elpWidth = (int)( LogPen.elpWidth * m_Scale * m_DPIScale );

			CPen *pPen = new CPen( LogPen.elpPenStyle, LogPen.elpWidth, LogPen.elpColor );
			if( pPen == 0 )
				return m_pDC->SelectObject( pPen );

			m_CreatedPens.AddTail( pPen );

			return m_pDC->SelectObject( pPen );
		}
		return 0;
	}

	CFont* SelectObject( CFont* pFont, double FontHeight )
	{
		if( pFont == 0 )
			return 0;

		LOGFONT LogFont;
		if( pFont->GetLogFont( &LogFont ) == 0 )
			return 0;

		if( m_pTextFormat != 0 && m_FontHeight == FontHeight && m_FontName == LogFont.lfFaceName )
			return 0; // OK, we already have font stuff ready to go.

		if( m_pTextFormat != 0 )
			m_pTextFormat->Release();

		m_pTextFormat = 0;

		CStringW FontName( LogFont.lfFaceName );

		IDWriteFactory *pDWriteFactory = GetDWriteFactory();

		pDWriteFactory->CreateTextFormat(
			(const wchar_t*)FontName,
			0, 
			DWRITE_FONT_WEIGHT_REGULAR,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			(float)( FontHeight * m_DPIScale ),
			L"en-us",
			&m_pTextFormat
			);

		m_FontHeight = FontHeight;
		m_FontName = LogFont.lfFaceName;

		return 0;
	}

	CBitmap* SelectObject( CBitmap* pBitmap )
	{
		// Create the Direct2D render target here so that the device context has a bitmap to draw into.
		// Maybe this can be done earlier but there may be a requirement to have the bitmap selected before
		// doing the Direct2D stuff.

		CBitmap *pOldBitmap = m_pDC->SelectObject( pBitmap );

		ID2D1Factory* pD2D1Factory = GetD2D1Factory();
		if( pD2D1Factory == 0 )
			return pOldBitmap;

		D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
			D2D1_RENDER_TARGET_TYPE_DEFAULT,
			D2D1::PixelFormat(
				DXGI_FORMAT_B8G8R8A8_UNORM,
				D2D1_ALPHA_MODE_IGNORE),
			0,
			0,
			D2D1_RENDER_TARGET_USAGE_NONE,
			D2D1_FEATURE_LEVEL_DEFAULT
			);

		HRESULT hr = pD2D1Factory->CreateDCRenderTarget( &props, &m_pRenderTarget );

		hr = m_pRenderTarget->BindDC( m_pDC->GetSafeHdc(), &m_Rect );
		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

		return pOldBitmap; 
	}

	int SelectObject( CRgn* pRgn )
		{ return m_pDC->SelectObject( pRgn ); }

	CBrush* SelectObject( CBrush *pBrush )
		{ return m_pDC->SelectObject( pBrush ); }

	CFPoint GetTextExtent( const char *pString, int Count )
	{
		CSize Size = m_pDC->GetTextExtent( pString, Count );
		CFPoint Result( Size.cx, Size.cy );
		// Only adjust the scaling and not the position.
		Result.x /= m_Scale * m_DPIScale;
		Result.y /= m_Scale * m_DPIScale;
		return Result;
	}

	int GetPenSize( int UnscaledSize )
	{
		return max( (int)( UnscaledSize * m_Scale * m_DPIScale ), 1 );
	}

	bool DrawArrow( CFPoint FromPoint, CFPoint ToPoint, double Width, double Length )
	{
		CPen* pPen = m_pDC->GetCurrentPen();
		LOGPEN LogPen;
		if( pPen->GetLogPen( &LogPen ) == 0 )
			return FALSE;
		COLORREF PenColor = LogPen.lopnColor;
		double Red = GetRValue( PenColor ) / 255.0;
		double Green = GetGValue( PenColor ) / 255.0;
		double Blue = GetBValue( PenColor ) / 255.0;

		CFLine Line( ToPoint, FromPoint );
		Line.SetDistance( Length );

		CBrush Brush;
		Red *= 0.80;
		Green *= 0.80;
		Blue *= 0.80;
		COLORREF NewColor = RGB( (int)( Red * 255.0 ), (int)( Green * 255.0 ), (int)( Blue * 255.0 ) );
		Brush.CreateSolidBrush( NewColor );

		CFLine CrossLine;
		Line.PerpendicularLine( CrossLine, Width / 2, 1 );
		CrossLine.ReverseDirection();
		CrossLine.SetDistance( Width );
		CPen *pOldPen = (CPen*)m_pDC->SelectStockObject( NULL_PEN );

		CFPoint Points[3];
		Points[0].x = Scale( CrossLine.GetStart() ).x;
		Points[0].y = Scale( CrossLine.GetStart() ).y;
		Points[1].x = Scale( CrossLine.GetEnd() ).x;
		Points[1].y = Scale( CrossLine.GetEnd() ).y;
		Points[2].x = Scale( ToPoint ).x;
		Points[2].y = Scale( ToPoint ).y;

		FillRgn( Points, 3, &Brush );
		m_pDC->SelectObject( pOldPen );

		return TRUE;
	}
};

#endif

///////////////////////////////////////////////////////////////////////////////////
// The external CRenderer class used outside of this module!
///////////////////////////////////////////////////////////////////////////////////

CRenderer::~CRenderer()
{
	if( m_pImplementation != 0 )
		delete m_pImplementation;
	m_pImplementation = 0;
}

CFPoint CRenderer::MoveTo( double x, double y )
{
	if( m_pImplementation == 0 )
		return CFPoint( 0, 0 );

	return m_pImplementation->MoveTo( x, y );
}

CFPoint CRenderer::LineTo( double x, double y )
{
	if( m_pImplementation == 0 )
		return CFPoint( 0, 0 );

	return m_pImplementation->LineTo( x, y );
}

CFPoint CRenderer::DrawLine( CFLine Line )
{
	if( m_pImplementation == 0 )
		return CFPoint( 0, 0 );

	return m_pImplementation->DrawLine( Line );
}

CFPoint CRenderer::DrawLine( CFPoint Point1, CFPoint Point2 )
{
	if( m_pImplementation == 0 )
		return CFPoint( 0, 0 );

	return m_pImplementation->DrawLine( Point1, Point2 );
}

bool CRenderer::TextOut( double x, double y, const CString& str )
{
	if( m_pImplementation == 0 )
		return false;

	return m_pImplementation->TextOut( x, y, str );
}

bool CRenderer::Circle( CFCircle &Circle )
{
	if( m_pImplementation == 0 )
		return false;

	return m_pImplementation->Circle( Circle );
}

bool CRenderer::Arc( CFArc &TheArc )
{
	if( m_pImplementation == 0 )
		return false;

	return m_pImplementation->Arc( TheArc );
}

bool CRenderer::Arc( double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4, bool bDrawTo )
{
	if( m_pImplementation == 0 )
		return false;

	return m_pImplementation->Arc( x1, y1, x2, y2, x3, y3, x4, y4, bDrawTo );
}

bool CRenderer::Arc( double x, double y, double r, double x3, double y3, double x4, double y4, bool bDrawTo )
{
	if( m_pImplementation == 0 )
		return false;

	return m_pImplementation->Arc( x, y, r, x3, y3, x4, y4, bDrawTo );
}

bool CRenderer::Pie( double x, double y, double r, double x3, double y3, double x4, double y4, bool bDrawTo )
{
	if( m_pImplementation == 0 )
		return false;

	return m_pImplementation->Pie( x, y, r, x3, y3, x4, y4, bDrawTo );
}

bool CRenderer::DrawRect( double RotationAngle, double x1, double y1, double x2, double y2 )
{
	if( m_pImplementation == 0 )
		return false;

	return m_pImplementation->DrawRect( RotationAngle, x1, y1, x2, y2 );
}

bool CRenderer::DrawRect( double x1, double y1, double x2, double y2 )
{
	if( m_pImplementation == 0 )
		return false;

	return m_pImplementation->DrawRect( x1, y1, x2, y2 );
}

bool CRenderer::DrawRect( const CFRect &Rect )
{
	if( m_pImplementation == 0 )
		return false;

	return m_pImplementation->DrawRect( Rect );
}

void CRenderer::FillRect( CFRect* pRect, CBrush* pBrush )
{
	if( m_pImplementation == 0 )
		return;

	return m_pImplementation->FillRect( pRect, pBrush );
}

//COLORREF CRenderer::SetPixel( double x, double y, COLORREF crColor )
//{
//	if( m_pImplementation == 0 )
//		return RGB( 0, 0, 0 );
//	return m_pImplementation->SetPixel( x, y, crColor );
//}

void CRenderer::LinkageDrawExpandedPolygon( CFPoint *pPoints, double *pLineRadii, int PointCount, COLORREF Color, bool bFill, double ExpansionDistance )
{
	if( m_pImplementation == 0 )
		return;

	m_pImplementation->LinkageDrawExpandedPolygon( pPoints, pLineRadii, PointCount, Color, bFill, ExpansionDistance );
}

CPen* CRenderer::SelectObject( CPen* pPen )
{
	if( m_pImplementation == 0 )
		return 0;

	return m_pImplementation->SelectObject( pPen );
}

CFont* CRenderer::SelectObject( CFont* pFont, double FontHeight )
{
	if( m_pImplementation == 0 )
		return 0;

	return m_pImplementation->SelectObject( pFont, FontHeight );
}

CBitmap* CRenderer::SelectObject( CBitmap* pBitmap )
{
	if( m_pImplementation == 0 )
		return 0;

	return m_pImplementation->SelectObject( pBitmap );
}

int CRenderer::SelectObject( CRgn* pRgn )
{
	if( m_pImplementation == 0 )
		return 0;

	return m_pImplementation->SelectObject( pRgn );
}

CBrush* CRenderer::SelectObject( CBrush *pBrush )
{
	if( m_pImplementation == 0 )
		return 0;

	return m_pImplementation->SelectObject( pBrush );
}

CFPoint CRenderer::GetTextExtent( const char *pString, int Count )
{
	if( m_pImplementation == 0 )
		return CFPoint( 0, 0 );

	return m_pImplementation->GetTextExtent( pString, Count );
}

int CRenderer::GetPenSize( int UnscaledSize )
{
	if( m_pImplementation == 0 )
		return 0;

	return m_pImplementation->GetPenSize( UnscaledSize );
}

bool CRenderer::DrawArrow( CFPoint FromPoint, CFPoint ToPoint, double Width, double Length )
{
	if( m_pImplementation == 0 )
		return false;

	return m_pImplementation->DrawArrow( FromPoint, ToPoint, Width, Length );
}

//void CRenderer::DrawToDXF( void )
//{
//	m_pDXFFile = new CDXFFile;
//}

//void CRenderer::SaveDXF( const char *pFileName )
//{
//	if( m_pDXFFile == 0 )
//		return;
//
//	m_pDXFFile->DXF_Save( pFileName );
//
//	delete m_pDXFFile;
//	m_pDXFFile = 0;
//}

bool CRenderer::IsPrinting( void )
{
	if( m_pImplementation == 0 )
		return false;

	return m_pImplementation->IsPrinting();
}

void CRenderer::SetOffset( CFPoint &Offset )
{
	if( m_pImplementation == 0 )
		return;

	m_pImplementation->m_ScrollPosition = Offset;
}

void CRenderer::SetOffset( double dx, double dy )
{
	if( m_pImplementation == 0 )
		return;

	m_pImplementation->m_ScrollPosition.x = dx;
	m_pImplementation->m_ScrollPosition.y = dy;
}

void CRenderer::SetScale( double Scale )
{
	if( m_pImplementation == 0 )
		return;

	m_pImplementation->m_Scale = Scale;
}

double CRenderer::GetScale( void )
{
	if( m_pImplementation == 0 )
		return 1;

	return m_pImplementation->m_Scale;
}

CFPoint CRenderer::GetOffset( void )
{
	if( m_pImplementation == 0 )
		return CPoint( 0, 0 );

	return m_pImplementation->m_ScrollPosition;
}

CDC * CRenderer::GetDC( void )
{
	if( m_pImplementation == 0 )
		return 0;

	return m_pImplementation->GetDC();
}

bool CRenderer::PatBlt( int x, int y, int nWidth, int nHeight, DWORD dwRop )
{
	if( m_pImplementation == 0 )
		return false;

	return m_pImplementation->PatBlt( x, y, nWidth, nHeight, dwRop );
}

bool CRenderer::CreateCompatibleDC( CDC *pDC, CRect *pRect )
{
	if( m_pImplementation == 0 )
		return false;

	return m_pImplementation->CreateCompatibleDC( pDC, pRect );
}

void CRenderer::SetAttribDC( HDC hDeviceContext )
{
	if( m_pImplementation == 0 )
		return;

	m_pImplementation->SetAttribDC( hDeviceContext );
}

void CRenderer::Attach( HDC hDeviceContext )
{
	if( m_pImplementation == 0 )
		return;

	m_pImplementation->Attach( hDeviceContext );
}

void CRenderer::Detach( void )
{
	if( m_pImplementation == 0 )
		return;

	m_pImplementation->Detach();
}

unsigned int CRenderer::SetTextAlign( unsigned int nFlags )
{
	if( m_pImplementation == 0 )
		return 0;

	return m_pImplementation->SetTextAlign( nFlags );
}

CGdiObject* CRenderer::SelectStockObject( int nIndex )
{
	if( m_pImplementation == 0 )
		return 0;

	return m_pImplementation->SelectStockObject( nIndex );
}

COLORREF CRenderer::SetTextColor( COLORREF crColor )
{
	if( m_pImplementation == 0 )
		return RGB( 0, 0, 0 );

	return m_pImplementation->SetTextColor( crColor );
}

COLORREF CRenderer::GetTextColor( void )
{
	if( m_pImplementation == 0 )
		return RGB( 0, 0, 0 );

	return m_pImplementation->GetTextColor();
}

int CRenderer::SetBkMode( int nBkMode )
{
	if( m_pImplementation == 0 )
		return 0;

	return m_pImplementation->SetBkMode( nBkMode );
}

COLORREF CRenderer::SetBkColor( COLORREF crColor )
{
	if( m_pImplementation == 0 )
		return RGB( 0, 0, 0 );

	return m_pImplementation->SetBkColor( crColor );
}

HDC CRenderer::GetSafeHdc( void )
{
	if( m_pImplementation == 0 )
		return (HDC)INVALID_HANDLE_VALUE;

	return m_pImplementation->GetSafeHdc();
}

int CRenderer::GetColorCount( void )
{
	if( m_pImplementation == 0 )
		return 0;

	return m_pImplementation->GetColorCount();
}

void CRenderer::SaveDXF( const char *pFileName )
{
	if( m_pImplementation == 0 )
		return;

	m_pImplementation->SaveDXF( pFileName );
}

int CRenderer::GetYOrientation( void )
{
	if( m_pImplementation == 0 )
		return -1;

	return m_pImplementation->GetYOrientation();
}

void CRenderer::BeginDraw( void )
{
	if( m_pImplementation == 0 )
		return;

	m_pImplementation->BeginDraw();
}

void CRenderer::EndDraw( void )
{
	if( m_pImplementation == 0 )
		return;

	m_pImplementation->EndDraw();
}

CRenderer::CRenderer( enum _RenderDestination RendererDestination )
{
	m_pImplementation = 0;

	switch( RendererDestination )
	{
		case WINDOWS_PRINTER_GDI:
			m_pImplementation = new CGDIRenderer( true );
			break;
		case WINDOWS_GDI:
			m_pImplementation = new CGDIRenderer( false );
			break;
		case DXF_FILE:
			m_pImplementation = new CDXFRenderer();
			break;
		case WINDOWS_D2D:
			#if defined( LINKAGE_USE_DIRECT2D )
				m_pImplementation = new CD2DRenderer( false );
			#endif
			break;
		case NULL_RENDERER:
		default:
			m_pImplementation = new CNullRenderer( false );
	}
}
