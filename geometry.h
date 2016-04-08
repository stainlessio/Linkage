#if !defined( _GEOMETRY_H_ )
#define _GEOMETRY_H_

#include "math.h"

#define RADIANS_TO_DEGREES_BEST_FOR_THIS_CODE 57.295779513082320876798154814105
#define DEGREES_TO_RADIANS_BEST_FOR_THIS_CODE 0.0174532925199432957692369076848

static void swap( double& a, double& b )
{
    double temp = a;
    a = b;
    b = temp;
}

#if 0
inline double fsqrt (double y) 
{
	double x, z, tempf;
	unsigned long *tfptr = ((unsigned long *)&tempf) + 1;

	tempf = y;
	*tfptr = (0xbfcdd90a - *tfptr)>>1; /* estimate of 1/sqrt(y) */
	x =  tempf;
	z =  y*0.5;                        /* hoist out the “/2”    */
	x = (1.5*x) - (x*x)*(x*z);         /* iteration formula     */
	x = (1.5*x) - (x*x)*(x*z);
	x = (1.5*x) - (x*x)*(x*z);
	x = (1.5*x) - (x*x)*(x*z);
//	x = (1.5*x) - (x*x)*(x*z);
	return x*y;
}
#endif

class CFPoint
{
	public:
	_inline CFPoint( double xIn = 0, double yIn = 0 ) { x = xIn; y = yIn; };
	_inline CFPoint( const CFPoint &Point ) { x = Point.x; y = Point.y; };
	_inline CFPoint( const CPoint& Point ) { x = Point.x; y = Point.y; };
	_inline double DistanceToPoint( const CFPoint Point ) { return sqrt( ( (Point.x - x ) * ( Point.x - x ) ) + ( ( Point.y - y ) * ( Point.y - y ) ) ); }
	_inline double DistanceToPoint( double x, double y ) { return sqrt( ( ( x - this->x ) * ( x -this-> x ) ) + ( ( y - this->y ) * ( y -this-> y ) ) ); }
	_inline CFPoint operator-( const CFPoint &Right )
		{ return CFPoint( x - Right.x, y - Right.y ); }
	_inline CFPoint operator+( const CFPoint &Right )
		{ return CFPoint( x + Right.x, y + Right.y ); }
	_inline void operator-=( const CFPoint &Right )
		{ x -= Right.x; y -= Right.y; }
	_inline bool operator==( const CFPoint &Right ) const
		{ return x == Right.x && y == Right.y; }
	_inline bool operator!=( const CFPoint &Right ) const
		{ return x != Right.x || y != Right.y; }
	_inline void operator-=( double Distance )
		{ x -= Distance; y -= Distance; }
	_inline void operator+=( const CFPoint &Right )
		{ x += Right.x; y += Right.y; }
	_inline void operator+=( const int &Right )
		{ x += Right; y += Right; }
	_inline void operator=( const CFPoint &Point )
		{ x = Point.x; y = Point.y; }
	_inline void SetPoint( double x, double y ) 
		{ this->x = x; this->y = y; }
	_inline void SetPoint( CPoint point ) 
		{ this->x = point.x; this->y = point.y; }
		
	bool IsInsideOf( class CFRect &Rect );
	
	void SnapToLine( class CFLine &Line, bool bToSegment, bool bStartToInfinity = false );
	CFPoint SnapToArc( class CFArc &TheArc );

	double GetDistance( const CFPoint &Point ) const
		{ return (double)sqrt( ( ( Point.x - x )  * ( Point.x - x ) ) + ( ( Point.y - y ) * ( Point.y - y ) ) ); }
		
	CFPoint MidPoint( CFPoint &OtherPoint, double ScaleDistance );
	
	void RotateAround( const CFPoint &OtherPoint, double Angle );
	
	void SetPoint( CFPoint &OtherPoint, double Distance, double Angle );

	double x;
	double y;
};

class CFLine
{
	public:
	CFLine() { SetLine( 0, 0, 0, 0 ); }
	CFLine( const double x1, const double y1, const double x2, const double y2 )	{ SetLine( x1, y1, x2, y2 ); }
	CFLine( const CFPoint &Point1, const CFPoint &Point2 )	{ SetLine( Point1.x, Point1.y, Point2.x, Point2.y ); }
	CFLine( const CFLine &Line ) { SetLine( Line ); }
	void SetLine( const double x1, const double y1, const double x2, const double y2 )
		{ m_Start.x = x1; m_Start.y = y1; m_End.x = x2; m_End.y = y2; }
	void SetLine( const CFPoint &Point1, const CFPoint &Point2 )
		{ m_Start.x = Point1.x; m_Start.y = Point1.y; m_End.x = Point2.x; m_End.y = Point2.y; }
	void SetLine( const CFLine &Line )
		{ m_Start.x = Line[0].x; m_Start.y = Line[0].y; m_End.x = Line[1].x; m_End.y = Line[1].y; }
	void SetDistance( const double Distance );
	
	double GetDistance( void ) const
		{ return (double)sqrt( ( ( m_End.x - m_Start.x )  * ( m_End.x - m_Start.x ) ) + ( ( m_End.y - m_Start.y ) * ( m_End.y - m_Start.y ) ) ); }

	double GetAngle( void )
		{ return RADIANS_TO_DEGREES_BEST_FOR_THIS_CODE * (double)atan2( m_End.y - m_Start.y, m_End.x - m_Start.x ); }

	void GetParallelLine( CFLine &NewLine, double Offset );
	
	void ReverseDirection( void ) { CFPoint Temp = m_End; m_End = m_Start; m_Start = Temp; }

	const CFPoint &operator[]( int Index ) const { return ( Index == 0 ? m_Start : m_End); }
	CFPoint &operator[]( int Index ) { return ( Index == 0 ? m_Start : m_End); }

	// These operators cannot take references to points (or pointers to
	// points) because we might be using the points of this line to alter
	// this same line and things would get very messed up.
	void operator-=( const CFPoint &Right )
		{ SetLine( m_Start.x - Right.x, m_Start.y - Right.y, m_End.x - Right.x, m_End.y - Right.y ); }
	void operator+=( const CFPoint &Right )
		{ SetLine( m_Start.x + Right.x, m_Start.y + Right.y, m_End.x + Right.x, m_End.y + Right.y ); }

	void PerpendicularPoint( CFPoint &Perp, int Direction = 1 ) const;
	void PerpendicularLine( CFLine &Perp, int Direction = 1 ) const;
	void PerpendicularLine( CFLine &Perp, double length, int Direction = 1 ) const;

	void MoveEnds( double MoveStartDistance, double MoveEndDistance );

	CFPoint MidPoint( double ScaleDistance );
	
	CFPoint GetStart( void ) { return m_Start; }
	CFPoint GetEnd( void ) { return m_End; }

	CFPoint m_Start;
	CFPoint m_End;
};

class CFCircle
{
	public:
	_inline CFCircle( CFPoint Center, double rIn = 0 ) { SetCircle( Center, rIn ); }
	CFCircle( CFPoint Center, CFPoint RadiusPoint ) { SetCircle( Center, RadiusPoint ); }
	_inline CFCircle( double xIn = 0, double yIn = 0, double rIn = 0 ) { SetCircle( xIn, yIn, rIn ); }

	void SetCircle( CFPoint Center, double rIn = 0 ) { x = Center.x; y = Center.y; r = rIn; }
	void SetCircle( CFPoint Center, CFPoint RadiusPoint );
	void SetCircle( double xIn = 0, double yIn = 0, double rIn = 0 ) { x = xIn; y = yIn; r = rIn; }

	CFPoint GetCenter( void ) const { return CFPoint( x, y ); }

	double x;
	double y;
	double r;	// radius

	bool CircleIntersection( CFCircle& OtherCircle, CFPoint* ReturnPoint0, CFPoint* ReturnPoint1 );
};

class CFArc : public CFCircle
{
	public:
	CFArc() {}
	CFArc( CFPoint &Center, double Radius, CFPoint &Start, CFPoint &End ) { SetArc( Center, Radius, Start, End ); }
	CFArc( double x, double y, double Radius, CFPoint &Start, CFPoint &End ) { SetArc( CFPoint( x, y ), Radius, Start, End ); }
	void SetArc( CFPoint &Center, double Radius, CFPoint &Start, CFPoint &End ) { x = Center.x; y = Center.y; r = Radius; m_Start = Start; m_End = End; }

	CFPoint GetStart( void ) { return m_Start; }
	CFPoint GetEnd( void ) { return m_End; }

	double GetStartAngle( void )
		{ return RADIANS_TO_DEGREES_BEST_FOR_THIS_CODE * (double)atan2( m_Start.y - y, m_Start.x - x ); }
	double GetEndAngle( void )
		{ return RADIANS_TO_DEGREES_BEST_FOR_THIS_CODE * (double)atan2( m_End.y - y, m_End.x - x ); }

	bool PointOnArc( CFPoint &Point );

	CFPoint m_Start;
	CFPoint m_End;
};

class CFRect
{
	public:
	_inline CFRect( double left = 0, double top = 0, double right = 0, double bottom = 0 )
		{ this->top = top; this->left = left; this->bottom = bottom; this->right = right; }
	_inline CFRect( CRect Rect )
		{ this->top = Rect.top; this->left = Rect.left; this->bottom = Rect.bottom; this->right = Rect.right; }
	_inline CFRect( CFPoint &TopLeft, CFPoint &BottomRight )
		{ this->top = TopLeft.y; this->left = TopLeft.x; this->bottom = BottomRight.y; this->right = BottomRight.x; }
	_inline void SetRect( double x1, double y1, double x2, double y2 )
		{ top = y1; bottom = y2; left = x1; right = x2; }
	_inline void InflateRect( double x, double y ) { top -= y; left -= x; bottom += y; right += x; }
		
	bool IsOverlapped( CFRect &Rect );
	
	bool IsInsideOf( CFRect &Rect );
	bool PointInRect( CFPoint Point );
	CFPoint GetCenter( void ) { return CFPoint( ( left + right ) / 2, ( top + bottom ) / 2 ); }
	
	double Width( void ) { return right - left; }
	double Height( void ) { return bottom - top; }

	CFPoint TopLeft( void ) { return CFPoint( left, top ); }
	CFPoint BottomRight( void ) { return CFPoint( right, bottom ); }

	void Normalize( void )
	{
		if( top > bottom )
			swap( top, bottom );
		if( left > right )
			swap( left, right );
	}
	
	double top;
	double left;
	double bottom;
	double right;
};

class CFArea : public CFRect
{
	public:
	_inline CFArea( CFRect &Rect )
		{ this->top = Rect.top; this->left = Rect.left; this->bottom = Rect.bottom; this->right = Rect.right; }
	_inline CFArea( double left = NAN, double top = NAN, double right = NAN, double bottom = NAN )
		{ this->top = top; this->left = left; this->bottom = bottom; this->right = right; }
	_inline CFArea( CRect Rect )
		{ this->top = Rect.top; this->left = Rect.left; this->bottom = Rect.bottom; this->right = Rect.right; }
	_inline CFArea( CFPoint &TopLeft, CFPoint &BottomRight )
		{ this->top = TopLeft.y; this->left = TopLeft.x; this->bottom = BottomRight.y; this->right = BottomRight.x; }

	_inline bool IsValid( void ) { return !isnan( left ); }

	CFRect GetRect( void ) { return IsValid() ? CFRect( left, top, right, bottom ): CFRect(); }

	void operator+=( const CFPoint &Point )
	{
		if( !IsValid() )
		{
			top = Point.y;
			bottom = Point.y;
			left = Point.x;
			right = Point.x;
			return;
		}
		top = min( top, Point.y );
		bottom = max( bottom, Point.y );
		left = min( left, Point.x );
		right = max( right, Point.x );
	}

	void operator+=( const CFRect &Rect )
	{ 
		if( !IsValid() )
		{
			top = Rect.top;
			bottom = Rect.bottom;
			left = Rect.left;
			right = Rect.right;
			return;
		}
		top = min( top, Rect.top );
		top = min( top, Rect.bottom );
		bottom = max( bottom, Rect.top );
		bottom = max( bottom, Rect.bottom );
		left = min( left, Rect.left );
		left = min( left, Rect.right );
		right = max( right, Rect.left );
		right = max( right, Rect.right );
	}

	void operator+=( const CFArea &Area )
	{ 
		if( isnan( Area.left ) )
			return;
		if( !IsValid() )
		{
			top = Area.top;
			bottom = Area.bottom;
			left = Area.left;
			right = Area.right;
			Normalize();
			return;
		}
		top = min( top, Area.top );
		top = min( top, Area.bottom );
		bottom = max( bottom, Area.top );
		bottom = max( bottom, Area.bottom );
		left = min( left, Area.left );
		left = min( left, Area.right );
		right = max( right, Area.left );
		right = max( right, Area.right );
	}

	void operator+=( const CFLine &Line )
	{ 
		if( !IsValid() )
		{
			top = Line.m_Start.y;
			bottom = Line.m_End.y;
			left = Line.m_Start.x;
			right = Line.m_End.x;
			Normalize();
			return;
		}
		*this += Line.m_Start;
		*this += Line.m_End;
	}
};


static inline double Distance( CFPoint Point1, CFPoint Point2 )
	{ return sqrt( fabs( ( Point1.x - Point2.x ) * ( Point1.x - Point2.x ) ) + fabs( ( Point1.y - Point2.y ) * ( Point1.y - Point2.y ) ) ); }

static inline double GetAngle( CFPoint Point1, CFPoint Point2 )
{
	double Angle = RADIANS_TO_DEGREES_BEST_FOR_THIS_CODE * atan2( Point2.y - Point1.y, Point2.x - Point1.x );
	if( Angle < 0 )
		Angle += 360;
	return Angle;
}

static inline double setprecision(double x, int prec)
{
    return ceil( x * pow( 10, (double)prec ) - .4999999999999) / pow(10,(double)prec );
}

static inline double GetAngle( CFPoint CenterPoint, CFPoint Point1, CFPoint Point2 )
{
	double Angle1 = atan2( Point1.y - CenterPoint.y, Point1.x - CenterPoint.x ) * RADIANS_TO_DEGREES_BEST_FOR_THIS_CODE;
	double Temp = atan2( Point1.y - CenterPoint.y, Point1.x - CenterPoint.x );
	double Angle2 = atan2( Point2.y - CenterPoint.y, Point2.x - CenterPoint.x ) * RADIANS_TO_DEGREES_BEST_FOR_THIS_CODE; 
	if( Angle1 < -180 ) //if( Angle1 < 0 )
		Angle1 += 360;
	if( Angle2 < -180 ) //if( Angle2 < 0 )
		Angle2 += 360;
	double Angle = Angle1 - Angle2;
	//if( Angle1 < Angle2 )
	//	Angle += 360;
	return Angle;
}

static inline double GetAngle( CFPoint Point1, CFPoint Point2, CFPoint Point3, CFPoint Point4 )
{
	double Angle1 = atan2( Point2.y - Point1.y, Point2.x - Point1.x ) * RADIANS_TO_DEGREES_BEST_FOR_THIS_CODE;
	double Angle2 = atan2( Point4.y - Point3.y, Point4.x - Point3.x ) * RADIANS_TO_DEGREES_BEST_FOR_THIS_CODE;
	if( Angle1 < -180 ) //if( Angle1 < 0 )
		Angle1 += 360;
	if( Angle2 < -180 ) //if( Angle2 < 0 )
		Angle2 += 360;
	double Angle = Angle1 - Angle2;
	//if( Angle1 < Angle2 )
	//	Angle += 360;
	return Angle;
}

double DistanceToLine( CFPoint &End1, CFPoint &End2, CFPoint CheckPoint );

void Intersects( const CFLine &Line, const CFCircle &Circle, CFPoint &HitPoint1, CFPoint &HitPoint2, bool &bHit1, bool &bHit2, bool bBeforeOkay, bool bAfterOkay );
bool Intersects( const CFPoint &line1Point1, const CFPoint &line1Point2, const CFPoint &line2Point1, const CFPoint &line2Point2, CFPoint &intersection, int *pDirection = NULL, bool *pbOnBothSegments = NULL, bool *pbOnFirstSegment = NULL );
bool Intersects( const CFLine &Line1, const CFLine &Line2, CFPoint &intersection, int *pDirection = NULL, bool *pbOnBothSegments = NULL, bool *pbOnFirstSegment = NULL );

double DistanceAlongLine( CFLine &Line, CFPoint &Point );

bool IsPointInPoly( int PointCount, const CFPoint *pPoints, const CFPoint &TestPoint );

double FindRadius( double ChordLength, double DistanceFromChord );

bool GetTangents( const CFCircle &Circle1, const CFCircle &Circle2, CFLine &Result1, CFLine &Result2 );

double GetClosestAngle( double OldAngle, double NewAngle );

#endif





