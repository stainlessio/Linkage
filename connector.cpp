#include "stdafx.h"
#include "geometry.h"
#include "link.h"
#include "connector.h"
#include "math.h"

static const char *ALPHABET = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

static CString GetAlphaIdentifier( int ID )
{
	if( ID < 52 )
		return CString( ALPHABET[ID] );

	CString TempString;
	TempString.Format( "%c%d", ALPHABET[ID%52], ID / 52 );
	return TempString;
}

void CConnector::Reset( void )
{
	m_Point.x = 0;
	m_Point.y = 0;
	m_OriginalPoint.x = 0;
	m_OriginalPoint.y = 0;
	m_TempPoint.x = 0;
	m_TempPoint.y = 0;
	m_RotationAngle = 0.0;
	m_TempRotationAngle = 0.0;
	m_Identifier = 0;
	m_bInput = false;
	m_bAnchor = false;
	m_bTempFixed = false;
	m_bDrawingConnector = false;
	m_bAlwaysManual = false;
	m_RPM = 0;
	m_StartPoint = 0;
	m_PointCount = 0;
	m_DrawingPointCounter = 9999;
	m_pSlideLimits[0] = 0;
	m_pSlideLimits[1] = 0;
	m_DrawCircleRadius = 0;
	m_OriginalDrawCircleRadius = 0;
	m_SlideRadius = 0;
	m_OriginalSlideRadius = 0;
	m_Color = RGB( 200, 200, 200 );
}

CConnector::CConnector()
{
	Reset();
}

CString CConnector::GetIdentifierString( bool bDebugging )
{
	if( bDebugging )
	{
		CString String;
		String.Format( "C%d", GetIdentifier() );
		return String;
	}

	if( m_Name.IsEmpty() )
		return GetAlphaIdentifier( GetIdentifier() );
	else
		return m_Name;
}

void CConnector::Select( bool bSelected )
{
	m_bSelected = bSelected;
}

bool CConnector::IsLinkSelected( void )
{
	POSITION Position = m_Links.GetHeadPosition();
	for( int Counter = 0; Position != NULL; ++Counter )
	{
		CLink* pLink = m_Links.GetNext( Position );
		if( pLink == 0 )
			continue;
		if( pLink->IsSelected() )
			return true;
	}
	return false;
}

bool CConnector::PointOnConnector( CFPoint Point, double TestDistance )
{
	double Distance = m_Point.DistanceToPoint( Point );

	return Distance <= TestDistance;
}

void CConnector::AddLink( class CLink* pLink )
{
	if( m_Links.Find( pLink ) == 0 )
		m_Links.AddTail( pLink );
}

CLink* CConnector::GetLink( int Index )
{
	POSITION Position = m_Links.GetHeadPosition();
	for( int Counter = 0; Position != NULL; ++Counter )
	{
		CLink* pLink = m_Links.GetNext( Position );
		if( Counter == Index )
			return pLink;
	}
	return 0;
}

int CConnector::GetSelectedLinkCount( void )
{
	int Count = 0;
	POSITION Position = m_Links.GetHeadPosition();
	for( int Counter = 0; Position != NULL; ++Counter )
	{
		CLink* pLink = m_Links.GetNext( Position );
		if( pLink == 0 || !pLink->IsSelected() )
			continue;
		++Count;
	}
	return Count;
}

void CConnector::RotateAround( CFPoint& Point, double Angle )
{
	SetTempFixed( true );

	m_TempPoint.RotateAround( Point, Angle );

/*	Angle *= 0.0174532925; // Convert to radians.

	double x = m_TempPoint.x - Point.x;
	double y = m_TempPoint.y - Point.y;

	double Cosine = cos( Angle );
	double Sine = sin( Angle );

	double x1 = x * Cosine - y * Sine;
	double y1 = x * Sine + y * Cosine;

	m_TempPoint.x = x1 + Point.x;
	m_TempPoint.y = y1 + Point.y;
	*/
}

void CConnector::IncrementRotationAngle( double Change )
{
	double Angle = GetRotationAngle();
	Angle += Change;
//	if( Angle < 0.0 )
//		Angle += 360.0;
//	else if( Angle >= 360.0 )
//		Angle -= 360.0;
	SetRotationAngle( Angle );
}

void CConnector::GetArea( CFRect &Rect )
{
	CFPoint Point = GetPoint();
	Rect.left = Point.x - m_DrawCircleRadius;
	Rect.top = Point.y + m_DrawCircleRadius;
	Rect.right = Point.x + m_DrawCircleRadius;
	Rect.bottom = Point.y - m_DrawCircleRadius;
}

void CConnector::GetAdjustArea( CFRect &Rect )
{
	CFPoint Point = GetPoint();
	Rect.left = Point.x - m_DrawCircleRadius;
	Rect.top = Point.y + m_DrawCircleRadius;
	Rect.right = Point.x + m_DrawCircleRadius;
	Rect.bottom = Point.y - m_DrawCircleRadius;
}

void CConnector::RemoveLink( CLink *pLink )
{
	POSITION Position = m_Links.GetHeadPosition();
	while( Position != 0 )
	{
		POSITION RemovePosition = Position;
		CLink *pCheckLink = m_Links.GetNext( Position );
		if( pCheckLink != 0 && ( pLink == 0 || pCheckLink == pLink ) )
		{
			m_Links.RemoveAt( RemovePosition );
			pCheckLink->SetActuator( pCheckLink->IsActuator() && pCheckLink->GetConnectorCount() == 2 );
		}
	}
}

CFPoint * CConnector::GetMotionPath( int &StartPoint, int &PointCount, int &MaxPoint )
{
	if( m_PointCount < 2 )
		return 0;

	StartPoint = m_StartPoint;
	PointCount = m_PointCount;
	MaxPoint = MAX_DRAWING_POINTS - 1;
	return m_DrawingPoints;
}

void CConnector::AddMotionPoint( void )
{
	if( !m_bDrawingConnector )
		return;

	++m_DrawingPointCounter;
	if( m_DrawingPointCounter < 3 )
		return;

	m_DrawingPointCounter = 0;

	CFPoint Point = GetPoint();

	if( m_PointCount < MAX_DRAWING_POINTS )
	{
		if( m_PointCount > 0 && m_DrawingPoints[m_PointCount] == Point )
			return;
		m_DrawingPoints[m_PointCount] = Point;
		++m_PointCount;
	}
	else
	{
		int PreviousPoint = m_StartPoint == 0 ? MAX_DRAWING_POINTS - 1 : m_StartPoint - 1;
		if( m_DrawingPoints[PreviousPoint] == Point )
			return;
		m_DrawingPoints[m_StartPoint] = Point;
		++m_StartPoint;
		if( m_StartPoint == MAX_DRAWING_POINTS )
			m_StartPoint = 0;
	}
}

void CConnector::Reset( bool bClearMotionPath )
{
	m_Point = m_OriginalPoint;
	m_PreviousPoint = m_OriginalPoint;
	m_RotationAngle = 0.0;
	m_TempRotationAngle = 0.0;
	SetPositionValid( true );
	if( bClearMotionPath )
	{
		m_StartPoint = 0;
		m_PointCount = 0;
		m_DrawingPointCounter = 9999;
	}
}

void CConnector::MakePermanent( void )
{
	m_PreviousPoint = m_Point;
	m_Point = m_TempPoint;
	m_RotationAngle = m_TempRotationAngle;
	m_OriginalDrawCircleRadius = m_DrawCircleRadius;
	m_OriginalSlideRadius = m_SlideRadius;
}

CConnector::CConnector( const CConnector &ExistingConnector ) : CElement( ExistingConnector )
{
	Reset();
	m_Point = ExistingConnector.m_OriginalPoint;
	m_RotationAngle = ExistingConnector.m_RotationAngle;
	m_TempPoint = ExistingConnector.m_OriginalPoint;
	m_TempRotationAngle = ExistingConnector.m_TempRotationAngle;
	m_OriginalPoint = ExistingConnector.m_OriginalPoint;
	m_Identifier = ExistingConnector.m_Identifier;
	m_RPM = ExistingConnector.m_RPM;
	m_bInput = ExistingConnector.m_bInput;
	m_bAnchor = ExistingConnector.m_bAnchor;
	m_bSelected = ExistingConnector.m_bSelected;
	m_bTempFixed = ExistingConnector.m_bTempFixed;
	m_bDrawingConnector = ExistingConnector.m_bDrawingConnector;
	m_Layers = ExistingConnector.m_Layers;
	m_PreviousPoint = ExistingConnector.m_PreviousPoint;
	m_RotationAngle = ExistingConnector.m_RotationAngle;
	m_OriginalDrawCircleRadius = ExistingConnector.m_OriginalDrawCircleRadius;
	m_OriginalSlideRadius = ExistingConnector.m_OriginalSlideRadius;
//	m_pSlideLimits[0] = ExistingConnector.m_pSlideLimits[0];
//	m_pSlideLimits[1] = ExistingConnector.m_pSlideLimits[1];

	m_StartPoint = 0;
	m_PointCount = 0;
	m_DrawingPointCounter = 0;

//	POSITION Position = ExistingConnector.m_Links.GetHeadPosition();
//	while( Position != 0 )
//	{
//		CLink *pLink = ExistingConnector.m_Links.GetNext( Position );
//		if( pLink == 0 )
//			continue;
//		m_Links.AddTail( pLink );
//	}
}

void CConnector::SlideBetween( class CConnector *pConnector1, class CConnector *pConnector2 )
{
	if( pConnector2 == 0 )
		pConnector1 = 0;

	bool bSet = pConnector1 != 0;
	CLink *pLink = 0;

	if( bSet )
	{
		pLink = pConnector1->GetSharingLink( pConnector2 );
		// The two connectors must share a link.
		if( pLink == 0 )
			return;
	}
	else if( m_pSlideLimits[0] != 0 && m_pSlideLimits[1] != 0 )
		pLink = m_pSlideLimits[0]->GetSharingLink( m_pSlideLimits[1] );

	if( pLink != 0 )
		pLink->SlideBetween( this, pConnector1, pConnector2 );

	m_pSlideLimits[0] = pConnector1;
	m_pSlideLimits[1] = pConnector2;
}

bool ConnectorList::Iterate( ConnectorListOperation &Operation )
{
	POSITION Position = GetHeadPosition();
	bool bFirst = true;
	while( Position != 0 )
	{
		CConnector* pConnector = GetNext( Position );
		if( pConnector == 0 )
			continue;
		if( !Operation( pConnector, bFirst, pConnector == 0 ) )
			return false;
		bFirst = false;
	}
	return true;
}

CLink * CConnector::GetSharingLink( CConnector *pOtherConnector )
{
	POSITION Position = m_Links.GetHeadPosition();
	for( int Counter = 0; Position != NULL; ++Counter )
	{
		CLink* pLink = m_Links.GetNext( Position );
		if( pLink == 0 )
			continue;
		if( pOtherConnector->HasLink( pLink ) )
			return pLink;
	}

	return 0;
}

bool CConnector::IsSharingLink( CConnector *pOtherConnector )
{
	if( GetSharingLink( pOtherConnector ) != 0 )
		return true;

	return IsAnchor() && pOtherConnector->IsAnchor();
}

bool CConnector::HasLink( CLink *pLink )
{
	POSITION Position = m_Links.GetHeadPosition();
	while( Position != 0 )
	{
		POSITION RemovePosition = Position;
		CLink *pCheckLink = m_Links.GetNext( Position );
		if( pCheckLink != 0 && pLink == pCheckLink )
			return true;
	}
	return false;
}

bool CConnector::IsAlone( void )
{
	if( m_Links.GetCount() > 1 )
		return false;

	POSITION Position = m_Links.GetHeadPosition();
	if( Position != 0 )
	{
		CLink *pCheckLink = m_Links.GetNext( Position );
		if( pCheckLink->GetConnectorCount() == 1 )
			return true;
	}
	return false;
}

bool ConnectorList::Remove( class CConnector *pConnector )
{
	POSITION Position = GetHeadPosition();
	while( Position != 0 )
	{
		POSITION RemovePosition = Position;
		CConnector* pCheckConnector = GetNext( Position );
		if( pCheckConnector != 0 && ( pConnector == 0 || pCheckConnector == pConnector  ) )
		{
			RemoveAt( RemovePosition );
			return true;
		}
	}
	return false;
}

class CConnector * ConnectorList::Find( class CConnector *pConnector )
{
	POSITION Position = GetHeadPosition();
	while( Position != 0 )
	{
		POSITION RemovePosition = Position;
		CConnector* pCheckConnector = GetNext( Position );
		if( pCheckConnector != 0 && ( pConnector == 0 || pCheckConnector == pConnector  ) )
			return pConnector;
	}
	return 0;
}

bool CConnector::GetSlideLimits( CFPoint &Point1, CFPoint &Point2 )
{
	if(  m_pSlideLimits[0] != 0 )
		Point1 = m_pSlideLimits[0]->GetPoint();
	if(  m_pSlideLimits[1] != 0 )
		Point2 = m_pSlideLimits[1]->GetPoint();
	return IsSlider() && m_pSlideLimits[0] != 0;
}

CFPoint CConnector::GetTempPoint( void )
{
	return IsAnchor() ? m_OriginalPoint : m_TempPoint;
}

bool CConnector::GetSliderArc( CFArc &TheArc,bool bGetOriginal )
{
	if( GetSlideRadius() == 0 )
		return false;

	CConnector *pLimit1;
	CConnector *pLimit2;

	if( !GetSlideLimits( pLimit1, pLimit2 ) )
		return false;

	CFPoint Point1 = bGetOriginal ? pLimit1->GetOriginalPoint() : pLimit1->GetPoint();
	CFPoint Point2 = bGetOriginal ? pLimit2->GetOriginalPoint() : pLimit2->GetPoint();

	CFLine Line( Point1, Point2 );
	CFLine Perpendicular;
	double aLength = Line.GetDistance() / 2;
	double Radius = GetSlideRadius();
	double bLength = sqrt( ( Radius * Radius ) - ( aLength * aLength ) );
	Line.SetDistance( aLength );
	Line.PerpendicularLine( Perpendicular, Radius > 0 ? bLength : -bLength );

	if( Radius > 0 )
		TheArc.SetArc( Perpendicular.GetEnd(), Radius, Point1, Point2 );
	else
		TheArc.SetArc( Perpendicular.GetEnd(), Radius, Point2, Point1 );

	return true;
}

double CConnector::GetSlideRadius( void )
{
	if( m_SlideRadius == 0 )
		return 0;

	double sign = _copysign( 1, m_SlideRadius );

	CConnector *pLimit1;
	CConnector *pLimit2;
	if( !GetSlideLimits( pLimit1, pLimit2 ) )
		return 0;
	return sign * max( fabs( m_SlideRadius ), ( Distance( pLimit1->GetPoint(), pLimit2->GetPoint() ) / 2 ) + 0.0009 );
}

double CConnector::GetOriginalSlideRadius( void )
{
	if( m_OriginalSlideRadius == 0 )
		return 0;

	double sign = _copysign( 1, m_SlideRadius );

	CConnector *pLimit1;
	CConnector *pLimit2;
	if( !GetSlideLimits( pLimit1, pLimit2 ) )
		return 0;
	return sign * max( fabs( m_OriginalSlideRadius ), ( Distance( pLimit1->GetOriginalPoint(), pLimit2->GetOriginalPoint() ) / 2 ) + 0.0009 );
}
