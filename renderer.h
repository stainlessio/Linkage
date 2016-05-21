#if !defined( _Linkage_Renderer_H_ )
#define _Linkage_Renderer_H_

#include "stdafx.h"
#include "geometry.h"
#include "DXFFile.h"

class CRenderer// : public CDC
{
	public:

	enum _RenderDestination { NULL_RENDERER, WINDOWS_PRINTER_GDI, WINDOWS_GDI, WINDOWS_GDIPLUS, DXF_FILE, WINDOWS_D2D };
	
	CRenderer( enum _RenderDestination RendererDestination );
	virtual ~CRenderer();

	void BeginDraw( void );
	void EndDraw( void );

	//CDC *GetScreenDevice( void );

	void Attach( HDC hDeviceContext );
	void Detach( void );
	void SetAttribDC( HDC hDeviceContext );

	//void DrawToDXF( void );
	void SaveDXF( const char *pFileName );
		
	void SetOffset( CFPoint &Offset );
	void SetOffset( double dx, double dy );
	void SetScale( double Scale );

	int GetYOrientation( void );
	//bool IsPlotter( void );

	double GetScale( void );
	CFPoint GetOffset( void );

	int GetPenSize( int UnscaledSize );
	
	CFPoint DrawLine( CFLine Line );
	CFPoint DrawLine( CFPoint Point1, CFPoint Point2 );
	CFPoint MoveTo( double x, double y );
	CFPoint LineTo( double x, double y );
	CFPoint MoveTo( CFPoint Point ) { return MoveTo( Point.x, Point.y ); }
	CFPoint LineTo( CFPoint Point ) { return LineTo( Point.x, Point.y ); }
	
	bool TextOut( double x, double y, const CString& str );

	int GetColorCount( void );

	CDC *GetDC( void );
	HDC GetSafeHdc( void );

	int SetBkMode( int nBkMode );
	COLORREF SetBkColor( COLORREF crColor );
	CGdiObject* SelectStockObject( int nIndex );	
	COLORREF SetTextColor( COLORREF crColor );
	COLORREF GetTextColor( void );
	//void FillRect( LPCRECT lpRect, CBrush* pBrush );
	unsigned int SetTextAlign( unsigned int nFlags );
	bool PatBlt( int x, int y, int nWidth, int nHeight, DWORD dwRop );
	bool Arc( double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4, bool bDrawTo = false );
	bool Arc( double x, double y, double r, double x3, double y3, double x4, double y4, bool bDrawTo = false );
	bool Pie( double x, double y, double r, double x3, double y3, double x4, double y4, bool bDrawTo = false );
	bool Arc( CFArc &TheArc );
	bool Circle( CFCircle &Circle );
	//COLORREF SetPixel( double x, double y, COLORREF crColor );
	//COLORREF SetPixel( CFPoint Point, COLORREF crColor ) { return SetPixel( Point.x, Point.y, crColor ); }
	bool CreateCompatibleDC( CDC *pDC, CRect *prect = 0 );

	bool DrawArrow( CFPoint FromPoint, CFPoint ToPoint, double Width, double Length );
	
	bool DrawRect( const CFRect &Rect );
	bool DrawRect( double x1, double y1, double x2, double y2 );
	bool DrawRect( double RotationAngle, double x1, double y1, double x2, double y2 );
	
	CPen* SelectObject( CPen* pPen );
	virtual CFont* SelectObject( CFont* pFont, double FontHeight );
	CBitmap* SelectObject( CBitmap* pBitmap );
	CBrush* SelectObject( CBrush* pBrush );
	int SelectObject( CRgn* pRgn );
	
	void FillRect( CFRect* pRect, CBrush* pBrush );
	void LinkageDrawExpandedPolygon( CFPoint *pPoints, double *pLineRadii, int PointCount, COLORREF Color, bool bFill, double ExpansionDistance );
	
	//CFPoint ScalePoint( CPoint PixelPoint );
	//CFRect ScaleRect( CRect PixelRect );
	//CPoint UnscalePoint( CFPoint Point );
	//CRect UnscaleRect( CFRect Rect );
	
	CFPoint GetTextExtent( const char *pString, int Count );

	bool IsPrinting( void );

	private:

	class CRendererImplementation *m_pImplementation;
};

#endif
