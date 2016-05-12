#include "stdafx.h"
#include "math.h"
#include "geometry.h"

void CFPoint::SnapToLine( CFLine &Line, bool bToSegment, bool bStartToInfinity )
{
    CFPoint AP = *this - Line.GetStart();
    CFPoint AB = Line.GetEnd() - Line.GetStart();
    double ab2 = AB.x*AB.x + AB.y*AB.y;
    double ap_ab = AP.x*AB.x + AP.y*AB.y;
    double t = ap_ab / ab2;
    if( bToSegment || bStartToInfinity )
    {
         if( t < 0.0f )
			t = 0.0f;
         else if( !bStartToInfinity && t > 1.0f )
			t = 1.0f;
    }

	x = Line.m_Start.x + ( AB.x * t );
	y = Line.m_Start.y + ( AB.y * t );

	// Old code that had more computations than needed. Left where in case
	// there is a problem with the new code to set x and y above.

    //CFLine TempLine( CFPoint( 0, 0 ), AB );
    //TempLine.SetDistance( TempLine.GetDistance() * t );
    //*this = Line.GetStart() + TempLine.GetEnd();
}

void CFCircle::SetCircle( CFPoint Center, CFPoint RadiusPoint )
{
	double r = Distance( Center, RadiusPoint );
	SetCircle( Center, r );
}

bool CFCircle::CircleIntersection( CFCircle& OtherCircle, CFPoint* ReturnPoint0, CFPoint* ReturnPoint1 )
	{
	CFPoint p2;
	CFPoint p3;
	double d;		// distance between circles
	double a;		// distance to p2 from center of this circle
	double h;		// distance from P2 to p3

	d = sqrt( ( ( OtherCircle.x - x ) * ( OtherCircle.x - x ) ) + ( ( OtherCircle.y - y ) * ( OtherCircle.y - y ) ) );

	a = ( ( r * r ) - ( OtherCircle.r * OtherCircle.r ) + ( d * d ) ) / ( d + d );

	if( fabs( a ) > fabs( r ) )
	{
		if( fabs( a ) - r > .00005 )
			return false;

		// Fudge a bit to allow for mechanisms that are close to working but
		// that might have had some rounding issues cause malfunction.
		double sign = _copysign( 1, a );
		a = r * sign;
	}

	h = sqrt( ( r * r ) - ( a * a ) );

	p2.x = x + a * ( OtherCircle.x - x ) / d;
	p2.y = y + a * ( OtherCircle.y - y ) / d;

	p3.x = p2.x + h * ( OtherCircle.y - y ) / d;
	p3.y = p2.y - h * ( OtherCircle.x - x ) / d;

	*ReturnPoint0 = p3;

	p3.x = p2.x - h * ( OtherCircle.y - y ) / d;
	p3.y = p2.y + h * ( OtherCircle.x - x ) / d;

	*ReturnPoint1 = p3;

	return true;
	}

double DistanceToLine( CFPoint &End1, CFPoint &End2, CFPoint CheckPoint )
{
	double r_numerator = (CheckPoint.x-End1.x)*(End2.x-End1.x) + (CheckPoint.y-End1.y)*(End2.y-End1.y);
	double r_denomenator = (End2.x-End1.x)*(End2.x-End1.x) + (End2.y-End1.y)*(End2.y-End1.y);
	double r = r_numerator / r_denomenator;

    double px = End1.x + r*(End2.x-End1.x);
    double py = End1.y + r*(End2.y-End1.y);

    double s =  ((End1.y-CheckPoint.y)*(End2.x-End1.x)-(End1.x-CheckPoint.x)*(End2.y-End1.y) ) / r_denomenator;

	double DistanceToLine = fabs(s)*sqrt(r_denomenator);
	double DistanceToSegment = 0;

//
// (xx,yy) is the point on the lineSegment closest to (CheckPoint.x,CheckPoint.y)
//
	double xx = px;
	double yy = py;

	if ( (r >= 0) && (r <= 1) )
	{
		DistanceToSegment = DistanceToLine;
	}
	else
	{
		double dist1 = (CheckPoint.x-End1.x)*(CheckPoint.x-End1.x) + (CheckPoint.y-End1.y)*(CheckPoint.y-End1.y);
		double dist2 = (CheckPoint.x-End2.x)*(CheckPoint.x-End2.x) + (CheckPoint.y-End2.y)*(CheckPoint.y-End2.y);
		if (dist1 < dist2)
		{
			xx = End1.x;
			yy = End1.y;
			DistanceToSegment = sqrt(dist1);
		}
		else
		{
			xx = End2.x;
			yy = End2.y;
			DistanceToSegment = sqrt(dist2);
		}
	}

	return DistanceToSegment;
}

bool CFRect::IsOverlapped( CFRect &Rect )
{
	return !( Rect.top > bottom || Rect.bottom < top || Rect.left > right || Rect.right < left );
}

bool CFRect::IsInsideOf( CFRect &Rect )
{
	return ( top >= Rect.top && top <= Rect.bottom
		     && bottom >= Rect.top && bottom <= Rect.bottom
		     && left >= Rect.left && left <= Rect.right
		     && right >= Rect.left && right <= Rect.right );
}

bool CFPoint::IsInsideOf( class CFRect &Rect )
{
	return ( y >= Rect.top && y <= Rect.bottom
		     && x >= Rect.left && x <= Rect.right );
}

CFPoint CFPoint::MidPoint( CFPoint &Point, double ScaleDistance )
{
	CFLine Line( *this, Point );
	Line.SetDistance( Line.GetDistance() * ScaleDistance );
	return Line.GetEnd();
}

CFPoint CFLine::MidPoint( double ScaleDistance )
{
	CFLine Line( m_Start, m_End );
	Line.SetDistance( Line.GetDistance() * ScaleDistance );
	return Line.GetEnd();
}

void Intersects( const CFLine &Line, const CFCircle &Circle, CFPoint &HitPoint1, CFPoint &HitPoint2, bool &bHit1, bool &bHit2, bool bBeforeOkay, bool bAfterOkay )
{
	double drsq;
	double D;

	double dx = Line[1].x - Line[0].x;
	double dy = Line[1].y - Line[0].y;

	bHit1 = true;
	bHit2 = true;

	drsq = dx * dx + dy * dy;
	D = ( Line[0].x - Circle.x ) * dy - ( Line[0].y - Circle.y ) * dx;

	if( ( ( Circle.r*Circle.r*drsq ) - ( D * D ) ) < 0 )
	{
		bHit1 = false;
		bHit2 = false;
		return;
	}
	else
	{
		double det = sqrt( Circle.r * Circle.r * drsq - ( D * D ) );
		HitPoint1.x = Circle.x + ( D * dy + dx * det) / drsq;
		HitPoint1.y = Circle.y + ( (-D) * dx + dy * det) / drsq;
		HitPoint2.x = Circle.x + ( D * dy - dx * det) / drsq;
		HitPoint2.y = Circle.y + ( (-D) * dx - dy * det) / drsq;

		double d = Line[0].x+dx;
		double d2 = Line[0].y+dy;

		double xsign = _copysign( 1, Line[1].x-Line[0].x );
		double ysign = _copysign( 1, Line[1].y-Line[0].y );

		if( d - Line[1].x < -0.00005 || d - Line[1].x > 0.00005 )
		{
			bHit1 = false;
			bHit2 = false;
			return;
		}

		// If the line is dead vertical or horizontal then the hit point MUST be
		// on that same x or y coordinate. Rounding of IEEE numbers can be off
		// sometimes and then the intersection is incorrectly rejected.
		if( Line[0].x == Line[1].x )
		{
			HitPoint1.x = Line[0].x;
			HitPoint2.x = Line[0].x;
		}
		if( Line[0].y == Line[1].y )
		{
			HitPoint1.y = Line[0].y;
			HitPoint2.y = Line[0].y;
		}

		// Detect being at the beginning or end of the line and adjust the hit point.
		// This is to account for more of those pesky rounding issues that cause
		// intersections that theoretically should be on the end of the line to be
		// past the end by a small amount.
		double Fudge = 10000.0;

		// Test if points are before or after the line segment.
		if( !bBeforeOkay )
		{
			if( _copysign( 1, HitPoint1.x - Line[0].x ) != xsign )
			{
				if( floor( HitPoint1.x * Fudge ) != floor( Line[0].x * Fudge ) )
					bHit1 = false;
			}
			if( _copysign( 1, HitPoint1.y - Line[0].y ) != ysign )
			{
				if( floor( HitPoint1.y * Fudge ) != floor( Line[0].y * Fudge ) )
					bHit1 = false;
			}
			if( _copysign( 1, HitPoint2.x - Line[0].x) != xsign )
			{
				if( floor( HitPoint2.x * Fudge ) != floor( Line[0].x * Fudge ) )
					bHit2 = false;
			}
			if( _copysign( 1, HitPoint2.y - Line[0].y) != ysign )
			{
				if( floor( HitPoint2.y * Fudge ) != floor( Line[0].y * Fudge ) )
					bHit2 = false;
			}
		}
		if( !bAfterOkay )
		{
			if( _copysign( 1, Line[1].x - HitPoint1.x  ) != xsign )
			{
				if( floor( HitPoint1.x * Fudge ) != floor( Line[1].x * Fudge ) )
					bHit1 = false;
			}
			if( _copysign( 1, Line[1].y - HitPoint1.y ) != ysign )
			{
				if( floor( HitPoint1.y * Fudge ) != floor( Line[1].y * Fudge ) )
					bHit1 = false;
			}
			if( _copysign( 1, Line[1].x - HitPoint2.x ) != xsign )
			{
				if( floor( HitPoint2.x * Fudge ) != floor( Line[1].x * Fudge ) )
					bHit2 = false;
			}
			if( _copysign( 1, Line[1].y - HitPoint2.y ) != ysign )
			{
				if( floor( HitPoint2.y * Fudge ) != floor( Line[1].y * Fudge ) )
					bHit2 = false;
			}
		}
	}
}

void CFPoint::RotateAround( const CFPoint &OtherPoint, double Angle )
{
	Angle *= DEGREES_TO_RADIANS_BEST_FOR_THIS_CODE; // Convert to radians.

	double x1 = x - OtherPoint.x;
	double y1 = y - OtherPoint.y;

	double Cosine = cos( Angle );
	double Sine = sin( Angle );

	double x2 = x1 * Cosine - y1 * Sine;
	double y2 = x1 * Sine + y1 * Cosine;

	x = x2 + OtherPoint.x;
	y = y2 + OtherPoint.y;
}

void CFPoint::SetPoint( CFPoint &OtherPoint, double Distance, double Angle )
{
	Angle *= DEGREES_TO_RADIANS_BEST_FOR_THIS_CODE; // Convert to radians.

	x = OtherPoint.x + ( Distance * cos( Angle ) );
	y = OtherPoint.y + ( Distance * sin( Angle ) );

//	double Cosine = cos( Angle );
//	double Sine = sin( Angle );
//
//	double x2 = x1 * Cosine - y1 * Sine;
//	double y2 = x1 * Sine + y1 * Cosine;
//
//	x = x2 + OtherPoint.x;
//	y = y2 + OtherPoint.y;
}

bool Intersects( const CFLine &Line1, const CFLine &Line2, CFPoint &intersection, int *pDirection, bool *pbOnBothSegments, bool *pbOnFirstSegment )
{
	return Intersects( Line1[0], Line1[1], Line2[0], Line2[1], intersection, pDirection, pbOnBothSegments, pbOnFirstSegment );
}

bool Intersects( const CFPoint &line1Point1, const CFPoint &line1Point2, const CFPoint &line2Point1, const CFPoint &line2Point2, CFPoint &intersection, int *pDirection, bool *pbOnBothSegments, bool *pbOnFirstSegment )
{
	// Based on the 2d line intersection method from "comp.graphics.algorithms
	// Frequently Asked Questions"

	/*
	(Ay-Cy)(Dx-Cx)-(Ax-Cx)(Dy-Cy)
	r = ----------------------------- (eqn 1)
	(Bx-Ax)(Dy-Cy)-(By-Ay)(Dx-Cx)
	*/

	double q =	( line1Point1.y - line2Point1.y ) * (line2Point2.x - line2Point1.x) -
				(line1Point1.x - line2Point1.x) * (line2Point2.y - line2Point1.y);

	double d =	(line1Point2.x - line1Point1.x) * (line2Point2.y - line2Point1.y) -
				(line1Point2.y - line1Point1.y) * (line2Point2.x - line2Point1.x);

	if( d == 0 ) // parallel lines so no intersection anywhere in space (in curved space, maybe, but not here in Euclidian space.)
		return false;

	double r = q / d;

	// Direction is something added to this to make it more useful but has nothing to do with the real
	// line intersection algorithm. In fact, it may make no sense at all if the line segments are
	// not passed in the way they were when this was added! Also, the s value below might be useful for
	// a similar functionality.

	if( pDirection != NULL )
	{
		*pDirection = r >= 0 ? 1 : 0;
	}

	/*
	(Ay-Cy)(Bx-Ax)-(Ax-Cx)(By-Ay)
	s = ----------------------------- (eqn 2)
	(Bx-Ax)(Dy-Cy)-(By-Ay)(Dx-Cx)
	*/

	q = (line1Point1.y - line2Point1.y) * (line1Point2.x - line1Point1.x) -
		(line1Point1.x - line2Point1.x) * (line1Point2.y - line1Point1.y);
	double s = q / d;

	/*
	If r>1, P is located on extension of AB
	If r<0, P is located on extension of BA
	If s>1, P is located on extension of CD
	If s<0, P is located on extension of DC

	The above basically checks if the intersection is located at an	extrapolated
	point outside of the line segments. To ensure the intersection is
	only within	the line segments then the above must all be false, ie r between 0
	and 1 and s between 0 and 1.
	*/

	if( pbOnBothSegments != NULL )
	{
		if( r < 0 || r > 1 || s < 0 || s > 1 )
		{
			*pbOnBothSegments = false;
			if( pbOnFirstSegment != NULL )
			{
				if( s < 0 || s > 1 )
					*pbOnFirstSegment = false;
				else
					*pbOnFirstSegment = true;
			}
		}
		else
		{
			*pbOnBothSegments = true;
			if( pbOnFirstSegment != NULL )
				*pbOnFirstSegment = true;
		}
	}
	if( pbOnFirstSegment != NULL )
	{
		if( r < 0 || r > 1 )
			*pbOnFirstSegment = false;
		else
			*pbOnFirstSegment = true;
	}

	/*
	Px=Ax+r(Bx-Ax)
	Py=Ay+r(By-Ay)
	*/

	intersection.x = line1Point1.x + ( r * (line1Point2.x - line1Point1.x));
	intersection.y = line1Point1.y + ( r * (line1Point2.y - line1Point1.y));

	return true;
}

void CFLine::PerpendicularPoint( CFPoint &Perp, const int Direction ) const
{
	double dx = m_End.x - m_Start.x;
	double dy = m_End.y - m_Start.y;
	if( Direction == 1 )
	{
		Perp.x = m_End.x - dy;
		Perp.y = m_End.y + dx;
	}
	else
	{
		Perp.x = m_End.x + dy;
		Perp.y = m_End.y - dx;
	}
}

void CFLine::PerpendicularLine( CFLine &Perp, const int Direction ) const
{
	CFPoint Point = m_End;
	PerpendicularPoint( Perp[1], Direction );
	Perp[0] = Point;
}

void CFLine::PerpendicularLine( CFLine &Perp, double Length, const int Direction ) const
{
	CFPoint Point = m_End;
	PerpendicularPoint( Perp[1], Direction );
	Perp[0] = Point;
	Perp.SetDistance( Length );
}

void CFLine::GetParallelLine( CFLine &NewLine, double Offset )
{
	if( Offset == 0 )
	{
		NewLine = *this;
		return;
	}

	double Length = GetDistance();

	double dx = ( m_End.x - m_Start.x ) * fabs( Offset ) / Length;
	double dy = ( m_End.y - m_Start.y ) * fabs( Offset ) / Length;

	if( Offset > 0 )
		dy = -dy;
	else
		dx = -dx;

	NewLine.m_Start.x = m_Start.x + dy;
	NewLine.m_Start.y = m_Start.y + dx;
	NewLine.m_End.x = m_End.x + dy;
	NewLine.m_End.y = m_End.y + dx;
}

double DistanceAlongLine( CFLine &Line, CFPoint &Point )
{
    CFPoint AP = Point - Line.GetStart();
    CFPoint AB = Line.GetEnd() - Line.GetStart();
    double ab2 = AB.x*AB.x + AB.y*AB.y;
    double ap_ab = AP.x*AB.x + AP.y*AB.y;
    double t = ap_ab / ab2;

    CFLine TempLine( CFPoint( 0, 0 ), AB );

	double Distance = TempLine.GetDistance() * t;

	return Distance;
}

void CFLine::MoveEnds( double MoveStartDistance, double MoveEndDistance )
{
	if( m_End.x == m_Start.x && m_End.y == m_Start.y )
		return;

	double Length = sqrt( ( ( m_End.x - m_Start.x )  * ( m_End.x - m_Start.x ) ) + ( ( m_End.y - m_Start.y ) * ( m_End.y - m_Start.y ) ) );

	double ScaleFactor = MoveStartDistance / Length;
	m_Start.x = m_Start.x + ( ( m_End.x - m_Start.x ) * ScaleFactor );
	m_Start.y = m_Start.y + ( ( m_End.y - m_Start.y ) * ScaleFactor );

	ScaleFactor = MoveEndDistance / Length;
	m_End.x = m_End.x + ( ( m_End.x - m_Start.x ) * ScaleFactor );
	m_End.y = m_End.y + ( ( m_End.y - m_Start.y ) * ScaleFactor );
}

void CFLine::MoveEndsFromStart( double MoveStartDistance, double MoveEndDistance )
{
	if( m_End.x == m_Start.x && m_End.y == m_Start.y )
		return;

	double Length = sqrt( ( ( m_End.x - m_Start.x )  * ( m_End.x - m_Start.x ) ) + ( ( m_End.y - m_Start.y ) * ( m_End.y - m_Start.y ) ) );

	double ScaleFactor = MoveStartDistance / Length;
	CFPoint NewStart;
	NewStart.x = m_Start.x + ( ( m_End.x - m_Start.x ) * ScaleFactor );
	NewStart.y = m_Start.y + ( ( m_End.y - m_Start.y ) * ScaleFactor );

	ScaleFactor = MoveEndDistance / Length;
	CFPoint NewEnd;
	NewEnd.x = m_Start.x + ( ( m_End.x - m_Start.x ) * ScaleFactor );
	NewEnd.y = m_Start.y + ( ( m_End.y - m_Start.y ) * ScaleFactor );

	m_Start = NewStart;
	m_End = NewEnd;
}

void CFLine::SetDistance( const double Distance )
{
	if( m_End.x == m_Start.x && m_End.y == m_Start.y )
		return;

	double ScaleFactor = Distance / ( (double)sqrt( ( ( m_End.x - m_Start.x )  * ( m_End.x - m_Start.x ) ) + ( ( m_End.y - m_Start.y ) * ( m_End.y - m_Start.y ) ) ) );
	m_End.x = m_Start.x + ( ( m_End.x - m_Start.x ) * ScaleFactor );
	m_End.y = m_Start.y + ( ( m_End.y - m_Start.y ) * ScaleFactor );
}

bool IsPointInPoly( int PointCount, const CFPoint *pPoints, const CFPoint &TestPoint )
{
	int i;
	int j = PointCount - 1;
	int oddNodes = 0;

	for( i = 0; i < PointCount; ++i )
	{
		if ((pPoints[i].y< TestPoint.y && pPoints[j].y>=TestPoint.y
		||   pPoints[j].y< TestPoint.y && pPoints[i].y>=TestPoint.y)
		&&  (pPoints[i].x<=TestPoint.x || pPoints[j].x<=TestPoint.x))
		{
			oddNodes^=(pPoints[i].x+(TestPoint.y-pPoints[i].y)/(pPoints[j].y-pPoints[i].y)*(pPoints[j].x-pPoints[i].x)<TestPoint.x);
		}
		j=i;
	}

	return oddNodes != 0;
}

bool CFRect::PointInRect( CFPoint Point )
{
	return Point.x >= left && Point.y >= top && Point.x < right && Point.y < bottom;
}

double FindRadius( double ChordLength, double DistanceFromChord )
{
	return ( ( ( DistanceFromChord * DistanceFromChord ) + ( ( ChordLength * ChordLength ) * 0.25 ) ) ) / ( DistanceFromChord * 2 );
}

CFPoint CFPoint::SnapToArc( CFArc &TheArc )
{
	CFLine AngleLine( TheArc.x, TheArc.y, x, y );
	AngleLine.SetDistance( fabs( TheArc.r ) );
	CFPoint Point = AngleLine.GetEnd();

	if( TheArc.GetStart() != TheArc.GetEnd() && !TheArc.PointOnArc( Point ) )
	{
		double d1 = Distance( TheArc.GetStart(), Point );
		double d2 = Distance( TheArc.GetEnd(), Point );
		if( d1 <= d2 )
			Point = TheArc.GetStart();
		else
			Point = TheArc.GetEnd();
	}
	*this = Point;
	return Point;
}

bool CFArc::PointOnArc( CFPoint &Point )
{
	double Distance = ::Distance( CFPoint( x, y ), Point );
	//if( fabs( Distance - fabs( r ) ) > 0.1 ) // Not worrying about distnce so much right now.
	//	return false;

	double StartAngle = GetAngle( CFPoint( x, y ), m_Start );
	double EndAngle = GetAngle( CFPoint( x, y ), m_End );
	double Angle = GetAngle( CFPoint( x, y ), Point );
	if( EndAngle < StartAngle )
		EndAngle += 360.0;
	if( Angle < StartAngle )
		Angle += 360;
	return Angle >= StartAngle && Angle <= EndAngle;
}

bool GetTangents( const CFCircle &Circle1, const CFCircle &Circle2, CFLine &Result1, CFLine &Result2 )
{
	if( Circle1.r > Circle2.r )
		return GetTangents( Circle2, Circle1, Result1, Result2 );
	else if( Circle1.r == Circle2.r )
	{
		CFLine Line( Circle1.GetCenter(), Circle2.GetCenter() );
		Result1 = Line; Result1.ReverseDirection();
		Result2 = Line; Result2.ReverseDirection();
		CFLine Perp;
		Line.PerpendicularLine( Perp, 1 );
		Perp.SetDistance( Circle2.r );
		Result1 -= Perp.GetEnd() - Circle2.GetCenter();
		Result2 += Perp.GetEnd() - Circle2.GetCenter();
		return true;
	}

	// Circle1 is smaller than circle2 as per the above tests.

	CFCircle Temp = Circle2;
	Temp.r -= Circle1.r;

	CFLine Line( Circle2.GetCenter(), Circle1.GetCenter() );
	double DistanceValue = Line.GetDistance() / 2;
	Line.SetDistance( DistanceValue );
	CFCircle Intersector( Line.GetEnd(), Line.GetDistance() );

	CFPoint Point1;
	CFPoint Point2;
	if( !Intersector.CircleIntersection( Temp, &Point1, &Point2 ) )
		return false;

	Result1.SetLine( Point1, Circle1.GetCenter() );
	Result2.SetLine( Point2, Circle1.GetCenter() );

	Line.SetLine( Circle2.GetCenter(), Point1 );
	Line.SetDistance( Circle2.r );

	Result1 += Line.GetEnd() - Point1;

	Line.SetLine( Circle2.GetCenter(), Point2 );
	Line.SetDistance( Circle2.r );

	Result2 += Line.GetEnd() - Point2;

	return true;
}

double GetClosestAngle( double OldAngle, double NewAngle )
{
	/*
	 * NewAngle could be about 360 degrees off due to code doing normalization. Figure out how
	 * to get the angle as close to the old angle as possible by adding or subtracting 360.
	 */

	double AsIs = fabs( NewAngle - OldAngle );
	double WithAdd = fabs( ( NewAngle + 360 ) - OldAngle );
	if( WithAdd < AsIs )
		return NewAngle + 360;
	double WithSubtract = fabs( ( NewAngle - 360 ) - OldAngle );
	if( WithSubtract < AsIs )
		return NewAngle - 360;
	return NewAngle;
}

