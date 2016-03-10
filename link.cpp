#include "stdafx.h"
#include "helper.h"
#include "geometry.h"
#include "link.h"
#include "connector.h"
#include "graham.h"
#include "DebugItem.h"

extern CDebugItemList DebugItemList;

CLink::CLink() 
{
	m_Identifier = 0;
	m_MoveCount = 0;
	m_bSelected = false;
	m_bTempFixed = false;
	m_bActuator = false;
	m_bAlwaysManual = false;
	m_ActuatorStroke = 0;
	m_ActuatorCPM = 0;
	m_LineSize = 1;
	m_bSolid = 0;
	m_ActuatorExtension = 0;
	m_TempActuatorExtension = 0;
	m_pHull = 0;
	m_HullCount = 0;
	m_bGear = false;
	m_RotationAngle = 0.0;
	m_TempRotationAngle = 0.0;
	m_Color = RGB( 200, 200, 200 );
	m_bLocked = false;
}

CLink::CLink( const CLink &ExistingLink )
{
	m_bSelected = false;
	m_bTempFixed = ExistingLink.m_bTempFixed;
	m_Identifier = ExistingLink.m_Identifier;
	m_MoveCount = ExistingLink.m_MoveCount;
	m_LineSize = ExistingLink.m_LineSize;
	m_bSolid = ExistingLink.m_bSolid;
	m_ActuatorCPM = ExistingLink.m_ActuatorCPM;
	m_ActuatorStroke = ExistingLink.m_ActuatorStroke;
	m_ActuatorExtension = ExistingLink.m_ActuatorExtension;
	m_bAlwaysManual = ExistingLink.m_bAlwaysManual;
	m_RotationAngle = ExistingLink.m_RotationAngle;
	m_bGear = ExistingLink.m_bGear;
	m_bLocked = ExistingLink.m_bLocked;
	m_bMeasurementElement = ExistingLink.m_bMeasurementElement;
	m_bNoRotateWithAnchor = ExistingLink.m_bNoRotateWithAnchor;
	m_Layers = ExistingLink.m_Layers;
	m_Color = ExistingLink.m_Color;
	m_bActuator = ExistingLink.m_bActuator;
	m_bNoRotateWithAnchor = ExistingLink.m_bNoRotateWithAnchor;
	m_pHull = 0;
	m_HullCount = 0;

	POSITION Position = ExistingLink.m_Connectors.GetHeadPosition();
	while( Position != 0 )
	{
		CConnector *pConnector = ExistingLink.m_Connectors.GetNext( Position );
		if( pConnector == 0 )
			continue;
		m_Connectors.AddTail( pConnector );
	}
}

CLink::~CLink()
{
	if( m_pHull != 0 )
		delete [] m_pHull;
	m_pHull = 0;
	m_HullCount = 0;
}

void CLink::GetArea( const GearConnectionList &GearConnections, CFRect &Rect )
{
	CFRect AreaRect( DBL_MAX, -DBL_MAX, -DBL_MAX, DBL_MAX );
	int ConnectorCount = 0;
	
	POSITION Position = m_Connectors.GetHeadPosition();
	while( Position != 0 )
	{
		CConnector* pConnector = m_Connectors.GetNext( Position );
		if( pConnector == 0 )
			continue;
		CFRect TempRect;
		pConnector->GetArea( TempRect );
		
		AreaRect.left = min( AreaRect.left, TempRect.left );
		AreaRect.right = max( AreaRect.right, TempRect.right );
		AreaRect.top = max( AreaRect.top, TempRect.top );
		AreaRect.bottom = min( AreaRect.bottom, TempRect.bottom );
	}

	if( m_bGear )
	{
		CConnector *pConnector = GetConnector( 0 );
		double Radius = GetLargestGearRadius( GearConnections, 0 );
		AreaRect.left = min( AreaRect.left, pConnector->GetPoint().x - Radius );
		AreaRect.right = max( AreaRect.right, pConnector->GetPoint().x + Radius );
		AreaRect.top = max( AreaRect.top, pConnector->GetPoint().y + Radius );
		AreaRect.bottom = min( AreaRect.bottom, pConnector->GetPoint().y - Radius );
	}

	Rect = AreaRect;
}

void CLink::GetAdjustArea( const GearConnectionList &GearConnections, CFRect &Rect )
{
	CFRect AreaRect( DBL_MAX, -DBL_MAX, -DBL_MAX, DBL_MAX );
	int ConnectorCount = 0;
	
	POSITION Position = m_Connectors.GetHeadPosition();
	while( Position != 0 )
	{
		CConnector* pConnector = m_Connectors.GetNext( Position );
		if( pConnector == 0 )
			continue;
		CFRect TempRect;
		pConnector->GetAdjustArea( TempRect );
		
		AreaRect.left = min( AreaRect.left, TempRect.left );
		AreaRect.right = max( AreaRect.right, TempRect.right );
		AreaRect.top = max( AreaRect.top, TempRect.top );
		AreaRect.bottom = min( AreaRect.bottom, TempRect.bottom );
	}

	if( m_bGear )
	{
		CConnector *pConnector = GetConnector( 0 );
		double Radius = GetLargestGearRadius( GearConnections, 0 );
		AreaRect.left = min( AreaRect.left, pConnector->GetPoint().x - Radius );
		AreaRect.right = max( AreaRect.right, pConnector->GetPoint().x + Radius );
		AreaRect.top = max( AreaRect.top, pConnector->GetPoint().y + Radius );
		AreaRect.bottom = min( AreaRect.bottom, pConnector->GetPoint().y - Radius );
	}

	Rect = AreaRect;
}

void CLink::GetAveragePoint( const GearConnectionList &GearConnections, CFPoint &Point )
{
	Point = CFPoint( 0, 0 );
	int PointCount = 0;

	CFPoint* Points = GetHull( PointCount );
	for( int Counter = 0; Counter < PointCount; ++Counter )
		Point += Points[Counter];

	Point.x /= PointCount;
	Point.y /= PointCount;
}

void CLink::SelectAllConnectors( bool bSelected )
{
	POSITION Position = m_Connectors.GetHeadPosition();
	while( Position != 0 )
	{
		CConnector* pConnector = m_Connectors.GetNext( Position );
		if( pConnector != 0 )
			pConnector->Select( bSelected );
	}
	Select( bSelected );
}

bool CLink::PointOnLink( const GearConnectionList &GearConnections, CFPoint Point, double TestDistance )
{
	if( m_bGear )
	{
		double Radius = GetLargestGearRadius( GearConnections, 0 );
		double distance = Distance( Point, GetConnector( 0 )->GetPoint() );
		if( distance <= Radius )
			return true;
	}

	int Connectors = m_Connectors.GetCount();
	if( Connectors == 1 )
		return false;

	int PointCount = 0;
	CFPoint* Points = GetHull( PointCount );

	// Point within the polygon first. This doesn't allow for any grab distance outside of the polygon.
	if( IsPointInPoly( PointCount, Points, Point ) )
		return true;

	if( IsSolid() )
		TestDistance *= 1.5; // Just a guess - might help the user interface.

	// Point on one of the lines method next. This lets the user click outside of the polygon and still get a hit.

	CFPoint *pPreviousPoint = &Points[PointCount-1];
	
	bool bResult = false;
	
	for( int Counter = 0; Counter < PointCount; ++Counter )
	{
		double Distance = DistanceToLine( *pPreviousPoint, Points[Counter], CFPoint( Point ) );
		if( Distance < TestDistance )
		{
			double left = min( pPreviousPoint->x, Points[Counter].x ) - TestDistance;
			double top = max( pPreviousPoint->y, Points[Counter].y ) + TestDistance;
			double right = max( pPreviousPoint->x, Points[Counter].x ) + TestDistance;
			double bottom = min( pPreviousPoint->y, Points[Counter].y ) - TestDistance;
			if( Point.x >= left && Point.y >= bottom && Point.x <= right && Point.y <= top )
			{
				bResult = true;
				break;
			}
		}
		pPreviousPoint = &Points[Counter];
	}

	return bResult;
}

void CLink::Reset( void )
{
	m_ActuatorExtension = 0;
	m_TempActuatorExtension = 0;
	m_RotationAngle = 0.0; 
	m_TempRotationAngle = 0.0;
	SetPositionValid( true ); 

	POSITION Position = m_Connectors.GetHeadPosition();
	while( Position != 0 )
	{
		CConnector* pConnector = m_Connectors.GetNext( Position );
		if( pConnector != 0 )
			pConnector->Reset( false );
	}
	UpdateController();
}

bool CLink::RotateAround( CConnector* pConnector )
{
	#pragma message( "FIX ME! RotateAround() will allow stretching of links." )
	#pragma message( "Just make two anchors with two links that are connected and one of the links gets stretched!" )

	if( pConnector == 0 )
		return false;

	pConnector->SetTempFixed( true );
		
	CFPoint Offset;
	Offset.x = pConnector->GetTempPoint().x - pConnector->GetOriginalPoint().x;
	Offset.y = pConnector->GetTempPoint().y - pConnector->GetOriginalPoint().y;

	POSITION Position = m_Connectors.GetHeadPosition();
	while( Position != 0 )
	{
		CConnector* pUseConnector = m_Connectors.GetNext( Position );
		if( pUseConnector == 0 || pUseConnector == pConnector )
			continue;
			
		// Is this connector connected to something fixed? If so, we should
		// get an error instead of trying to move it. This will break 
		// locomotive drive wheel assemblies but that is acceptable at this
		// time.
		
		if( pConnector->IsInput() )
		{
			CList<CLink*,CLink*> *pList = pUseConnector->GetLinksList();
			POSITION Position2 = pList->GetHeadPosition();
			while( Position2 != 0 )
			{
				CLink *pLink = pList->GetNext( Position2 );
				if( pList == 0 || pLink == this )
					continue;

				POSITION Position3 = pLink->GetConnectorList()->GetHeadPosition();
				while( Position3 != 0 )
				{
					CConnector* pAnotherConnector = pLink->GetConnectorList()->GetNext( Position3 );
					if( pAnotherConnector == 0 || pAnotherConnector == pConnector || pAnotherConnector == pUseConnector )
						continue;
						
					// Any connectors of this link that are fixed/anchors will
					// make us unable to move pUseConnector.
					if( pAnotherConnector->IsFixed() || pAnotherConnector->IsTempFixed() )
					{
						pUseConnector->SetPositionValid( false );
						pUseConnector->SetTempFixed( false );
						pUseConnector->SetPositionValid( false );
						return false;
					}
				}
			}
		}
					
		CFPoint Temp = pUseConnector->GetOriginalPoint();

		if( IsActuator() )
		{
			CFLine Line( pConnector->GetOriginalPoint(), Temp );
			Line.SetDistance( GetActuatedConnectorDistance( pConnector, pUseConnector ) );
			Temp = Line.GetEnd();
		}

		Temp.x += Offset.x;
		Temp.y += Offset.y;
		pUseConnector->MovePoint( Temp );
		
		pUseConnector->RotateAround( pConnector->GetTempPoint(), pConnector->GetTempRotationAngle() );
		pUseConnector->SetTempFixed( true );
		++m_MoveCount;
	}

	// Save a rotation angle for this link.
	SetRotationAngle( pConnector->GetTempRotationAngle() );
	SetTempFixed( true );

	Position = m_FastenedElements.GetHeadPosition();
	while( Position != 0 )
	{
		CElementItem *pItem = m_FastenedElements.GetNext( Position );
		if( pItem == 0 )
			continue;

		CConnector *pMoveConnector = pItem->GetConnector();
		CLink *pMoveLink = pItem->GetLink();

		if( pMoveConnector != 0 || pMoveLink != 0 )
		{
			POSITION Position2 = pMoveLink == 0 ? 0 : pMoveLink->GetConnectorList()->GetHeadPosition();
			if( pMoveConnector == 0 )
				pMoveConnector = pMoveLink->GetConnectorList()->GetNext( Position2 );

			while( true )
			{
				if( pMoveConnector != pConnector )
				{
					CFPoint Temp = pMoveConnector->GetOriginalPoint();

					Temp.x += Offset.x;
					Temp.y += Offset.y;
					pMoveConnector->MovePoint( Temp );
		
					pMoveConnector->RotateAround( pConnector->GetTempPoint(), pConnector->GetTempRotationAngle() );
					pMoveConnector->SetTempFixed( true );
				}

				if( Position2 == 0 )
					break;

				pMoveConnector = pMoveLink->GetConnectorList()->GetNext( Position2 );
			}
		}

		if( pMoveLink != 0 )
		{
			pMoveLink->SetRotationAngle( pConnector->GetTempRotationAngle() );
			pMoveLink->SetTempFixed( true );
		}
	}

	return true;
}

bool CLink::FixAll( void )
{
	if( IsGear() )
	{
		SetRotationAngle( 0 );
		SetTempFixed( true );
		return true;
	}

	// There are at least two fixed connectors. Either move all of the others
	// to move the link into position or return false because some cannot be
	// moved to the proper location.
	
	if( GetFixedConnectorCount() < 2 )
		return false;
		
	CConnector *pPreviousFixed = 0;
	CConnector *pFixed1 = 0;
	CConnector *pFixed2 = 0;

	POSITION Position = m_Connectors.GetHeadPosition();
	CConnector *pBaseConnector = GetFixedConnector();
	while( Position != 0 )
	{
		CConnector* pConnector = m_Connectors.GetNext( Position );
		if( pConnector == 0 )
			continue;
		if( pConnector->IsFixed() )
		{
			pFixed1 = pConnector;
			if( pPreviousFixed != 0 )
			{
				// Check to make sure it's probably in the right place already.
				double d1 = Distance( pFixed1->GetOriginalPoint(), pPreviousFixed->GetOriginalPoint() );
				double d2 = Distance( pFixed1->GetPoint(), pPreviousFixed->GetPoint() );
				
				double Difference = fabs( d2 - d1 );
				if( Difference > ( d1 / 1000.0 ) )
					return false;
			}
			pFixed2 = pPreviousFixed;
			pPreviousFixed = pConnector;
		}
	}
	
	if( pFixed1 == 0 || pFixed2 == 0 )
		return false;
		
	// All of the fixed connectors are where they should be. Rotate the rest
	// to match.
	
	pFixed1->SetRotationAngle( GetAngle( pFixed1->GetTempPoint(), pFixed2->GetTempPoint(), pFixed1->GetOriginalPoint(), pFixed2->GetOriginalPoint() ) );
	if( !RotateAround( pFixed1 ) )
		return false;
		
	return true;
}
void CLink::InitializeForMove( void )
{
	POSITION Position = m_Connectors.GetHeadPosition();
	while( Position != 0 )
	{
		CConnector* pConnector = m_Connectors.GetNext( Position );
		if( pConnector == 0 )
			continue;
			
		pConnector->MovePoint( pConnector->GetPoint() );
		pConnector->SetPositionValid( true );
	}
}

int CLink::GetFixedConnectorCount( void )
{
	int Count = 0;
	
	POSITION Position = m_Connectors.GetHeadPosition();
	while( Position != 0 )
	{
		CConnector* pConnector = m_Connectors.GetNext( Position );
		if( pConnector == 0 )
			continue;
			
		if( pConnector->IsAnchor() || pConnector->IsTempFixed() )
			++Count;
	}
	return Count;
}

int CLink::GetInputConnectorCount( void )
{
	int Count = 0;
	
	POSITION Position = m_Connectors.GetHeadPosition();
	while( Position != 0 )
	{
		CConnector* pConnector = m_Connectors.GetNext( Position );
		if( pConnector == 0 )
			continue;
			
		if( pConnector->IsAnchor() && pConnector->IsInput() )
			++Count;
	}
	return Count;
}

CConnector * CLink::GetFixedConnector( void )
{
	int Count = 0;
	CConnector* pFixedConnector = 0;
	
	POSITION Position = m_Connectors.GetHeadPosition();
	while( Position != 0 )
	{
		CConnector* pConnector = m_Connectors.GetNext( Position );
		if( pConnector == 0 )
			continue;
			
		if( pConnector->IsAnchor() || pConnector->IsTempFixed() )
		{
			pFixedConnector = pConnector;
			++Count;
		}
	}
	return Count == 1 ? pFixedConnector : 0 ;
}

CConnector* CLink::GetCommonConnector( CLink *pLink1, CLink *pLink2 )
{
	if( pLink1 == 0 || pLink2 == 0)
		return 0;
		
	POSITION Position1 = pLink1->GetConnectorList()->GetHeadPosition();
	while( Position1 != 0 )
	{
		CConnector* pConnector1 = pLink1->GetConnectorList()->GetNext( Position1 );
		if( pConnector1 == 0 )
			continue;
			
		POSITION Position2 = pLink2->GetConnectorList()->GetHeadPosition();
		while( Position2 != 0 )
		{
			CConnector* pConnector2 = pLink2->GetConnectorList()->GetNext( Position2 );
			if( pConnector1 == pConnector2 )
				return pConnector1;
		}
	}
	return 0;
}

void CLink::RemoveConnector( CConnector* pConnector )
{
	if( pConnector == 0 )
	{
		m_Connectors.RemoveAll();
		m_ConnectedSliders.RemoveAll();
		m_FastenedElements.RemoveAll();
	}
	else
	{
		m_Connectors.Remove( pConnector );
		m_ConnectedSliders.Remove( pConnector );
		m_FastenedElements.Remove( (CElement*)pConnector );
	}
}

bool CLink::IsConnected( CConnector* pConnector )
{
	return m_Connectors.Find( pConnector ) != 0;
}

void CLink::AddConnector( class CConnector* pConnector )
{
	if( m_Connectors.Find( pConnector ) != 0 )
		return;
	m_Connectors.AddTail( pConnector ); 
}

void CLink::SlideBetween( class CConnector *pSlider, class CConnector *pConnector1, class CConnector *pConnector2 )
{
	// the arguments are ignored at this time unless one of them is null.
	if( pConnector1 == 0 || pConnector2 == 0 )
		m_ConnectedSliders.Remove( pSlider );
	else
	{
		if( m_ConnectedSliders.Find( pSlider ) != 0 )
			return;
		m_ConnectedSliders.AddTail( pSlider ); 
	}
}

bool CLink::IsAllSelected( void )
{
	POSITION Position = m_Connectors.GetHeadPosition();
	while( Position != 0 )
	{
		CConnector* pCheckConnector = m_Connectors.GetNext( Position );
		if( pCheckConnector != 0 && !pCheckConnector->IsSelected() )
			return false;
	}
	return true;
}

bool CLink::IsAnySelected( void )
{
	POSITION Position = m_Connectors.GetHeadPosition();
	while( Position != 0 )
	{
		CConnector* pCheckConnector = m_Connectors.GetNext( Position );
		if( pCheckConnector != 0 && pCheckConnector->IsSelected() )
			return true;
	}
	return false;
}

int CLink::GetSelectedConnectorCount( void )
{
	int Count = 0;
	POSITION Position = m_Connectors.GetHeadPosition();
	while( Position != 0 )
	{
		CConnector* pCheckConnector = m_Connectors.GetNext( Position );
		if( pCheckConnector != 0 && pCheckConnector->IsSelected() )
			++Count;
	}
	return Count;
}

void CLink::FixAllConnectors( void )
{
}

bool CLink::IsSelected( bool bIsSelectedConnector )
{
	if( m_bSelected )
		return true;

	if( !bIsSelectedConnector )
		return false;

	// A kludge; a connector that has only one Link
	// that has only one connector and it's selected
	// is considered a selected Link (but not drawn
	// that way for the user).
	
	if( GetConnectorCount() == 1 && !IsGear() )
	{
		CConnector *pConnector = m_Connectors.GetHead();
		if( pConnector != 0 && pConnector->GetLinkCount() == 1
			&& pConnector->IsSelected() )
			return true;
	}

	return false;
}

int CLink::GetAnchorCount( void )
{
	int Count = 0;
	POSITION Position = m_Connectors.GetHeadPosition();
	while( Position != 0 )
	{
		CConnector* pCheckConnector = m_Connectors.GetNext( Position );
		if( pCheckConnector != 0 && pCheckConnector->IsAnchor() )
			++Count;
	}
	return Count;
}

CString CLink::GetIdentifierString( bool bDebugging )
{
	if( bDebugging )
	{
		CString String;
		String.Format( "L%d", GetIdentifier() );
		return String;
	}

	if( m_Name.IsEmpty() )
	{
		CString String;
		String.Format( "%d", GetIdentifier() + 1 );
		return String;
	}
	else
		return m_Name;
}


bool CLink::CanOnlySlide( CConnector** pLimit1, CConnector** pLimit2, CConnector** pSlider1, CConnector** pSlider2, bool *pbSlidersOnLink )
{
	/*
	 * Find two sliding connectors that both slide on the same link.
	 * Start with sliders that are on links connected to this link
	 * then switch to testing the connectos on this link that are
	 * sliders. In either case, if we find two sliders that slide on
	 * this link or this link slides on another, return true with some
	 * infomation about the sliders. Note that sliders on another link 
	 * must be fixed but sliders on this link require that the other link
	 * be fixed.
	 *
	 * THE CASE WHERE NEITHER LINK IS FIXED CANNOT BE HANDLED BY THIS CODE.
	 * THERE IS A VALID CASE BUT IT REQUIRES ONE FIXED CONNECTOR ON EACH LINK
	 * AND BOTH GET ROTATED TO LINE UP THE SLIDERS ON THE SLIDE LIMIT LINK.
	 */

	ConnectorList *pConnectorList = &m_ConnectedSliders;
	for( int Try = 0; Try < 2; ++Try, pConnectorList = &m_Connectors )
	{
		// First test to see if there are two sliding connectors that slide on this link.
		POSITION Position = pConnectorList->GetHeadPosition();
		while( Position != 0 )
		{
			CConnector* pCheckSlider1 = pConnectorList->GetNext( Position );
			if( pCheckSlider1 == 0 || !pCheckSlider1->IsSlider() )
				continue;

			if( Try == 0 )
			{
				if( !pCheckSlider1->IsFixed() )
					continue;
			}
			
			CConnector* pLimit1_1 = 0;
			CConnector* pLimit1_2 = 0;
			if( !pCheckSlider1->GetSlideLimits( pLimit1_1, pLimit1_2 ) )
				continue;

			if( Try == 1 )
			{
				if( !pLimit1_1->IsFixed() || !pLimit1_2->IsFixed() )
					continue;
			}

			POSITION Position2 = pConnectorList->GetHeadPosition();
			while( Position2 != 0 )
			{
				CConnector* pCheckSlider2 = pConnectorList->GetNext( Position2 );
				if( pCheckSlider2 == 0 || !pCheckSlider1->IsSlider() || pCheckSlider2 == pCheckSlider1 )
					continue;

				if( Try == 0 )
				{
					if( !pCheckSlider2->IsFixed() )
						continue;
				}

				CConnector* pLimit2_1 = 0;
				CConnector* pLimit2_2 = 0;
				if( !pCheckSlider2->GetSlideLimits( pLimit2_1, pLimit2_2 ) )
					continue;
				
				if( Try == 1 )
				{
					if( !pLimit2_1->IsFixed() || !pLimit2_2->IsFixed() )
						continue;
				}
				// If the sliders have the same limits then this link must slide
				// through them and the connectors of the sliding link can only be moved
				// along a line segment.
				if( ( pLimit1_1 == pLimit2_1 && pLimit1_2 == pLimit2_2 )
					|| ( pLimit1_1 == pLimit2_2 && pLimit1_2 == pLimit2_1 ) )
				{
					if( pLimit1 != 0 )
						*pLimit1 = pLimit1_1;
					if( pLimit2 != 0 )
						*pLimit2 = pLimit1_2;
					double Distance1 = Distance( pCheckSlider1->GetOriginalPoint(), pLimit1_1->GetOriginalPoint() );
					double Distance2 = Distance( pCheckSlider1->GetOriginalPoint(), pLimit1_2->GetOriginalPoint() );
					if( Distance1 < Distance2 )
					{
						if( pSlider1 != 0 )
							*pSlider1 = pCheckSlider1;
						if( pSlider2 != 0 )
							*pSlider2 = pCheckSlider2;
					}
					else
					{
						if( pSlider1 != 0 )
							*pSlider1 = pCheckSlider2;
						if( pSlider2 != 0 )
							*pSlider2 = pCheckSlider1;
					}

					if( pbSlidersOnLink != 0 )
						*pbSlidersOnLink = Try == 1;

					return true;
				}
			}
		}
	}

	return false;
}

void CLink::SetActuator( bool bActuator )
{
	// Set as an actuator if there are 2 connectors or zero. Zero connectors
	// might exist if the link is being read from a file or undo buffer.
	if( GetConnectorCount() != 2 && GetConnectorCount() != 0 )
	{
		m_bActuator = false;
		return;
	}
	
	bool bNewActuator = bActuator && !m_bActuator;
	m_bActuator = bActuator; 

	if( bNewActuator )
	{

		POSITION Position = m_Connectors.GetHeadPosition();
		if( Position == 0 )
			return;
		CConnector* pConnector1 = m_Connectors.GetNext( Position );
		if( Position == 0 )
			return;
		CConnector* pConnector2 = m_Connectors.GetNext( Position );

		m_StrokeConnector.SetLayers( GetLayers() );

		m_StrokeConnector.AddLink( this );

		CFLine Line( pConnector1->GetPoint(), pConnector2->GetPoint() );
		m_ActuatorCPM = 15;
		SetStroke( Line.GetDistance() / 2 );
	}
}

bool CLink::GetStrokePoint( CFPoint &Point )
{
	Point.SetPoint( 0, 0 );

	if( !m_bActuator || GetConnectorCount() != 2 )
		return false;

	POSITION Position = m_Connectors.GetHeadPosition();
	if( Position == 0 )
		return false;

	CConnector* pConnector1 = m_Connectors.GetNext( Position );
	if( Position == 0 )
		return false;

	CConnector* pConnector2 = m_Connectors.GetNext( Position );

	CFLine Line( pConnector1->GetPoint(), pConnector2->GetPoint() );

	double Distance = Line.GetDistance();
	if( m_ActuatorStroke < Distance )
		Distance = m_ActuatorStroke;

//	Line.SetDistance( Distance );
	Line.SetDistance( m_ActuatorStroke );
	Point = Line.GetEnd();

	return true;
}

void CLink::IncrementExtension( double Increment )
{
	SetExtension( m_TempActuatorExtension + Increment );
}

void CLink::SetExtension( double Value )
{
	m_TempActuatorExtension = Value; 
	
	// The extension is used to calculate the position of the actuator
	// where one complete cycle is twice the throw distsance. Because
	// of this, this code can subtract the throw distance times two
	// to keep the extension value from ever overflowing. By keeping a
	// single extension value for the movement of the actuator, there
	// is no running error in the numbers and the results should be
	// very accurate.
	//
	// The caller might have a running error in the increment.

	while( m_TempActuatorExtension > m_ActuatorStroke * 2 )
		m_TempActuatorExtension -= m_ActuatorStroke * 2;
	while( m_TempActuatorExtension < 0 )
		m_TempActuatorExtension += m_ActuatorStroke * 2;
}

double CLink::GetExtendedDistance( void )
{
	double UseDistance = fabs( m_TempActuatorExtension );
	if( UseDistance > m_ActuatorStroke )
		UseDistance = m_ActuatorStroke - ( UseDistance - m_ActuatorStroke );
	else
		UseDistance = m_TempActuatorExtension;
	return UseDistance * ( m_ActuatorCPM >= 0 ? 1 : -1 );
}

double CLink::GetActuatedConnectorDistance( CConnector *pConnector1, CConnector* pConnector2 )
{
	CLink *pLink = pConnector1->GetSharingLink( pConnector2 );
	if( !m_bActuator || pLink == 0 )
		return Distance( pConnector1->GetOriginalPoint(), pConnector2->GetOriginalPoint() );

	return Distance( pConnector1->GetOriginalPoint(), pConnector2->GetOriginalPoint() ) + GetExtendedDistance();
}

void CLink::SetStroke( double Stroke )
{
	m_ActuatorStroke = Stroke; 
	m_ActuatorExtension = 0;
	m_TempActuatorExtension = 0;

	UpdateController();
}

void CLink::UpdateFromController( void )
{
	// The controller moved.
	if( !m_bActuator )
		return;

	int Count = GetConnectorCount();
	if( Count != 2 )
		return;
	POSITION Position = m_Connectors.FindIndex( 0 );
	if( Position == 0 )
		return;
	CConnector *pConnector1 = m_Connectors.GetAt( Position );
	if( pConnector1 == 0 )
		return;

	CFLine Line( pConnector1->GetPoint(), m_StrokeConnector.GetPoint() );

	SetStroke( Line.GetDistance() );
}

void CLink::UpdateController( void )
{
	if( !IsActuator() )
		return;

	// Always keep the throw connector sliding between the two ends of the link.
	POSITION Position = m_Connectors.GetHeadPosition();
	if( Position == 0 )
		return;

	CConnector* pConnector1 = m_Connectors.GetNext( Position );
	if( Position == 0 )
		return;

	CConnector* pConnector2 = m_Connectors.GetNext( Position );

	m_StrokeConnector.SlideBetween( pConnector1, pConnector2 );

	CFPoint Point;
	GetStrokePoint( Point );
	m_StrokeConnector.MovePoint( Point );
	m_StrokeConnector.SetPoint( Point );
}

void CLink::MakePermanent( void )
{
	m_ActuatorExtension = m_TempActuatorExtension;
	m_RotationAngle = m_TempRotationAngle;
}


CFPoint *CLink::ComputeHull( int *Count )
{
	if( m_pHull != 0 )
		delete [] m_pHull;

	m_pHull = ::GetHull( &m_Connectors, m_HullCount );
	if( m_pHull == 0 )
		m_HullCount = 0;
	if( Count != 0 )
		*Count = m_HullCount < 0 ? 0 : m_HullCount;

	return m_pHull;
}

CFPoint *CLink::GetHull( int &Count )
{
	if( m_pHull == 0 )
		ComputeHull( 0 );
	Count = m_HullCount;
	return m_pHull;
}

CConnector *CLink::GetConnector( int Index )
{
	int Counter = 0;
	POSITION Position = m_Connectors.GetHeadPosition();
	while( Position != 0 )
	{
		CConnector* pConnector = m_Connectors.GetNext( Position );
		if( pConnector == 0 )
			continue;
		if( Counter == Index )
			return pConnector;
		++Counter;
	}
	return 0;
}

CConnector *CLink::GetConnectedSlider( int Index )
{
	int Counter = 0;
	POSITION Position = m_ConnectedSliders.GetHeadPosition();
	while( Position != 0 )
	{
		CConnector* pConnector = m_ConnectedSliders.GetNext( Position );
		if( pConnector == 0 )
			continue;
		if( Counter == Index )
			return pConnector;
		++Counter;
	}
	return 0;
}

bool CLink::GetGearsRadius( CGearConnection *pGearConnection, double &Radius1, double &Radius2 )
{
	CConnector *pConnector1 = GetGearConnector();
	if( pConnector1 == 0 )
		return false;

	CLink *pOtherLink = pGearConnection->m_pGear1 == this ? pGearConnection->m_pGear2 : pGearConnection->m_pGear1;

	CConnector *pConnector2 = pOtherLink->GetGearConnector();
	if( pConnector2 == 0 )
		return false;

	if( pGearConnection->m_bUseSizeAsRadius && pGearConnection->m_ConnectionType == CGearConnection::CHAIN )
	{
		Radius1 = pGearConnection->m_Gear1Size;
		Radius2 = pGearConnection->m_Gear2Size;
		return true;
	}

	double distance = Distance( pConnector1->GetPoint(), pConnector2->GetPoint() );

	if( pGearConnection->m_pGear2 == this )
		distance *= pGearConnection->Ratio();

	double Radius = distance / ( pGearConnection->Ratio() + 1 );

	Radius1 = Radius;
	Radius2 = distance - Radius;

	if( pGearConnection->m_ConnectionType == pGearConnection->CHAIN )
	{
		Radius1 /= 2;
		Radius2 /= 2;
	}
	return true;
}

bool CompareDoubles( double &First, double &Second )
{
	return First < Second;
}

bool CLink::GetGearRadii( const GearConnectionList &ConnectionList, std::list<double> &RadiusList )
{
	/*
	 * The gear connector of this link could mesh with multiple other gears. It's
	 * not really a single gear, but a set of one or more gears all on the same spindle.
	 * return all of the sizes for all of the connected gears.
	 */

	RadiusList.clear();

	CConnector *pConnector1 = GetGearConnector();
	if( pConnector1 == 0 )
		return false;

	POSITION Position = ConnectionList.GetHeadPosition();
	while( Position != 0 )
	{
		POSITION CurrentPosition = Position;
		CGearConnection *pConnection = ConnectionList.GetNext( Position );
		if( pConnection == 0 )
			continue;
			
		if( pConnection->m_pGear1 != this && pConnection->m_pGear2 != this )
			continue;

		CLink *pOtherLink = pConnection->m_pGear1 == this ? pConnection->m_pGear2 : pConnection->m_pGear1;
		CConnector *pConnector2 = pOtherLink->GetGearConnector();
		if( pConnector2 == 0 )
			continue;

		if( pConnection->m_bUseSizeAsRadius && pConnection->m_ConnectionType == pConnection->CHAIN )
		{
			RadiusList.push_back( pConnection->m_pGear1 == this ? pConnection->m_Gear1Size : pConnection->m_Gear2Size );
			continue;
		}

		double distance = Distance( pConnector1->GetPoint(), pConnector2->GetPoint() );

		if( pConnection->m_pGear2 == this )
			distance *= pConnection->Ratio();

		double Radius = distance / ( pConnection->Ratio() + 1 );

		if( pConnection->m_ConnectionType == pConnection->CHAIN )
			Radius /= 2;

		RadiusList.push_back( Radius );
	}

	if( RadiusList.size() == 0 )
	{
		// Give it some default radius, but what?
		RadiusList.push_back( 30.0 );
	}

	RadiusList.sort( CompareDoubles );

	return RadiusList.size() > 0;
}

double CLink::GetLargestGearRadius( const GearConnectionList &ConnectionList, CConnector **ppGearConnector )
{
	CConnector *pGearConnector = GetGearConnector();
	if( ppGearConnector != 0 )
		*ppGearConnector = pGearConnector;
	if( pGearConnector == 0 )
		return 0.0;

	std::list<double> RadiusList;
	GetGearRadii( ConnectionList, RadiusList );

	double Radius = 0.0;
	for( std::list<double>::iterator it = RadiusList.begin(); it != RadiusList.end(); ++it )
		Radius = max( *it, Radius );
	return Radius;
}

bool LinkList::Remove( class CLink *pLink )
{
	POSITION Position = GetHeadPosition();
	while( Position != 0 )
	{
		POSITION RemovePosition = Position;
		CLink* pCheckLink = GetNext( Position );
		if( pCheckLink != 0 && ( pLink == 0 || pCheckLink == pLink  ) )
		{
			RemoveAt( RemovePosition );
			return true;
		}
	}
	return false;
}

bool CLink::IsGearAnchored( void )
{
	return IsGear() && GetFastenedTo() != 0 && GetFastenedTo()->GetConnector() != 0 && GetFastenedTo()->GetConnector()->IsAnchor();
}