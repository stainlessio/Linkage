#include "stdafx.h"
#include "Simulator.h"
#include "linkageDoc.h"
#include "DebugItem.h"

extern CDebugItemList DebugItemList;

#define SWAP(x,y) do \
   { unsigned char swap_temp[sizeof(x) == sizeof(y) ? (signed)sizeof(x) : -1]; \
     memcpy(swap_temp,&y,sizeof(x)); \
     memcpy(&y,&x,       sizeof(x)); \
     memcpy(&x,swap_temp,sizeof(x)); \
    } while(0)

class CSimulatorImplementation
{
	public:

	const int INTERMEDIATE_STEPS = 2;
	long m_SimulationStep;
	bool m_bSimulationValid;

	CSimulatorImplementation()
	{
		m_bSimulationValid = false;
		Reset();
	}

	~CSimulatorImplementation()
	{
	}

	bool IsSimulationValid( void ) { return m_bSimulationValid; }

	bool Reset( void )
	{
		m_SimulationStep = 0;
		return true;
	}

	int CommonDivisor( int *Values, int Count )
	{
		for(;;)
		{
			/*
			 * If any value is one then simply return 1 since there can be
			 * no smaller value.
			 */
			for( int Counter = 0; Counter < Count; ++Counter )
			{
				if( Values[Counter] == 1 )
					return 1;
			}
		
			/*
			 * Find out if all values except for one of them are zero. Return
			 * the one non-zero value.
			 */
			int NonzeroValue = 0;
			int NonzeroCount = 0;
			for( int Counter = 0; Counter < Count; ++Counter )
			{
				if( Values[Counter] == 0 )
					continue;
				NonzeroValue = Values[Counter];
				++NonzeroCount;
			
			}
			if( NonzeroCount == 1 )
				return NonzeroValue;
			
			/*
			 * Find the lowest value and then change all of the other values
			 * to be the remainder of the value divided by the lowest value.
			 */
			int LowestLocation = -1;
			int LowestValue = 999999999;
			for( int Counter = 0; Counter < Count; ++Counter )
			{
				if( Values[Counter] < LowestValue )
				{
					LowestLocation = Counter;
					LowestValue = Values[Counter];
				}
			}
			for( int Counter = 0; Counter < Count; ++Counter )
			{
				if( Counter == LowestLocation )
					continue;
				Values[Counter] %= LowestValue;
	//			if( Values[Counter] == 0 )
	//				Values[Counter] = 1;
			}
		
			/*
			 * The new values will get processed at the top of the loop. This
			 * computation will continue until one of the conditions up there
			 * is met and we return something. The loop does not have a maximum
			 * loop count because the compuations are guanranteed to have an 
			 * effect, to the best of my knowledge.
			 */
		}
	}

	int GetSimulationSteps( CLinkageDoc *pDoc )
	{
		int InputCount = 0;
		POSITION Position = pDoc->GetConnectorList()->GetHeadPosition();
		int Count = 0;
		CConnector *pLastInput = 0;
		while( Position != NULL )
		{
			CConnector* pConnector = pDoc->GetConnectorList()->GetNext( Position );
			if( pConnector == 0 )
				continue;
			if( pConnector->IsInput() )
				++InputCount;
		}
	
		Position = pDoc->GetLinkList()->GetHeadPosition();
		while( Position != NULL )
		{
			CLink* pLink = pDoc->GetLinkList()->GetNext( Position );
			if( pLink == 0 )
				continue;
			if( pLink->IsActuator() )
				++InputCount;
		}

		if( InputCount == 0 )
			return 1;
		
		int *Values = new int [InputCount];
		if( Values == 0 )
			return 0;
	
		Position = pDoc->GetConnectorList()->GetHeadPosition();
		int Index = 0;
		while( Position != NULL )
		{
			CConnector* pConnector = pDoc->GetConnectorList()->GetNext( Position );
			if( pConnector == 0 )
				continue;
			if( pConnector->IsInput() )
				Values[Index++] = (int)( fabs( pConnector->GetRPM() ) + .9999 );
		}

		Position = pDoc->GetLinkList()->GetHeadPosition();
		while( Position != NULL )
		{
			CLink* pLink = pDoc->GetLinkList()->GetNext( Position );
			if( pLink == 0 )
				continue;
			if( pLink->IsActuator() )
				Values[Index++] = (int)( fabs( pLink->GetCPM() ) + .9999 );
		}

		/*
		 * Convert RPM values to simulation steps to get a single rotation.
		 */

		//for( Index = 0; Index < InputCount; ++Index )
		//	Values[Index] = ( 30 * INTERMEDIATE_STEPS * 60 ) / Values[Index];


		/*
		 * Figure out a common divisor that can be used
		 * to reduce the necessary rotations to do a complete
		 * cycle of the entire mechanism.
		 */
	 
		int Divisor = CommonDivisor( Values, InputCount );
	
		/*
		 * We now assume all RPM values are integers and divide one minute
		 * of simulation by the common divisor. This gives the smallest
		 * number of simulation steps needed to get all inputs back to their 
		 * starting locations at the same time. Fractional RPM will not
		 * work since more than one minute would be needed and the steps might
		 * be way too large to simulate in a reasonable amount of time.
		 */
		return 1800 / Divisor;
	}

	bool SimulateStep( CLinkageDoc *pDoc, int StepNumber, bool bAbsoluteStepNumber, int* pInputID, double *pInputPositions, int InputCount, bool bNoMultiStep )
	{
		if( pDoc == 0 )
			return false;

		m_bSimulationValid = true;

		ConnectorList *pConnectors = pDoc->GetConnectorList();
		LinkList *pLinks = pDoc->GetLinkList();

		if( pConnectors == 0 || pLinks == 0 )
			return false;

		if( pConnectors->GetCount() == 0 || pLinks->GetCount() == 0 )
			return false;

		int StepsToMove = StepNumber;
		if( bAbsoluteStepNumber )
			StepsToMove = StepNumber - m_SimulationStep;
		else if( InputCount > 0 && !bNoMultiStep )
			StepsToMove = 150;

		DWORD TickCount = GetTickCount();
		static const int MAX_TIME_TO_SIMULATE = 250;

		int Direction = StepsToMove > 0 ? +1 : -1;

		bool bResult = true;

		int StartStep = m_SimulationStep;

		while( StepsToMove != 0 && GetTickCount() < TickCount + MAX_TIME_TO_SIMULATE )
		{
			m_SimulationStep += Direction;
			StepsToMove -= Direction;

			double TotalManualControlDistance = 0;

			int Counter = 0;

			/*
			 * The m_SimulationStep value has already been changed to the next step. Do calculations and simulation
			 * for all of the partial steps up to and including the m_SimulationStep step number.
			 */
			for( int IntermediateStep = INTERMEDIATE_STEPS - 1; IntermediateStep >= 0 && bResult; --IntermediateStep )
			{
				POSITION Position = pConnectors->GetHeadPosition();
				while( Position != 0 )
				{
					CConnector* pConnector = pConnectors->GetNext( Position );
					if( pConnector == 0 )
						continue;
					pConnector->SetTempFixed( false );
					// If this is a rotation input connector then rotate it now (the temp rotation value).
					if( !pConnector->IsInput() )
						continue;

					// Find the input control for this connector.
					for( Counter = 0; Counter < InputCount && pInputID[Counter] != 10000 + pConnector->GetIdentifier(); )
						++Counter;
					if( Counter == InputCount )
					{
						/*
						 * There is no manual input control so rotate this connector
						 * automatically.
						 */

						if( !pConnector->IsAlwaysManual() )
							pConnector->SetRotationAngle( ( ( m_SimulationStep * INTERMEDIATE_STEPS ) - ( IntermediateStep * Direction ) ) * ( -( pConnector->GetRPM() * 0.2 ) / INTERMEDIATE_STEPS ) );
					}
					else
					{
						double DesiredAngle = 0;
						if( pInputPositions[Counter] != 0 )
							DesiredAngle = 360 - ( 360.0 * pInputPositions[Counter] );
						double Difference = DesiredAngle - pConnector->GetRotationAngle();
						if( Difference > 180.0 )
							Difference -= 360.0;
						if( Difference > 2 )
							Difference = 2;
						else if( Difference < -2 )
							Difference = -2;
						Difference /= INTERMEDIATE_STEPS;
						if( Difference != 0 )
						{
							pConnector->IncrementRotationAngle( Difference );
							TotalManualControlDistance += Difference;
						}
					}
				}
	
				Position = pLinks->GetHeadPosition();
				while( Position != 0 )
				{
					CLink *pLink = pLinks->GetNext( Position );
					if( pLink == 0  )
						continue;
					pLink->SetTempFixed( false );
					pLink->InitializeForMove();

					if( !pLink->IsActuator() )
						continue;

					// Find the input control for this connector.
					for( Counter = 0; Counter < InputCount && pInputID[Counter] != pLink->GetIdentifier(); )
						++Counter;
					if( Counter == InputCount )
					{
						/*
						 * There is no input control so extend this link automatically.
						 */
						if( !pLink->IsAlwaysManual() )
						{
							double Distance = pLink->GetStroke() * fabs( pLink->GetCPM() ) / 900.0;
							pLink->SetExtension( ( ( m_SimulationStep * INTERMEDIATE_STEPS ) - ( IntermediateStep * Direction ) ) * ( Distance / INTERMEDIATE_STEPS ));
						}
					}
					else
					{
						// This is a bit of a kludge. The extension distance will be
						// positive or nagative depending on the actuator being a push
						// or a pull at the start of its movement. On the other hand,
						// the increment should be positive in either case and a positive
						// increment actually causes the extension distance to move
						// negative when the CPM is negative. There is no simple fix
						// and something else would need to be changed to alter this
						// need for a kludge.

						double DesiredExtension = 0;
						if( pInputPositions[Counter] != 0 )
							DesiredExtension = pLink->GetStroke() * pInputPositions[Counter];
						double RealExtension = pLink->GetExtendedDistance();
						if( pLink->GetCPM() < 0 )
							RealExtension = pLink->GetStroke() + RealExtension;
						double ActuatorDifference = DesiredExtension - RealExtension;
						double MaxDistance = pLink->GetStroke() * fabs( pLink->GetCPM() ) / 900.0;
						if( ActuatorDifference > MaxDistance )
							ActuatorDifference = MaxDistance;
						else if( ActuatorDifference < -MaxDistance )
							ActuatorDifference = -MaxDistance;
						if( pLink->GetCPM() < 0 )
							ActuatorDifference = -ActuatorDifference;
						ActuatorDifference /= INTERMEDIATE_STEPS;
						if( ActuatorDifference != 0 )
						{
							pLink->IncrementExtension( ActuatorDifference );
							TotalManualControlDistance += ActuatorDifference;
						}
					}
				}

				bResult = MoveSimulation( pDoc );
			}

			// Undo the step if the simulation failed.
			if( !bResult )
			{
				m_SimulationStep = StartStep;
				break;
			}

			if( InputCount > 1 && TotalManualControlDistance == 0 )
				break;
		}

		return bResult;
	}

	bool ValidateMovement( CLinkageDoc *pDoc )
	{
		if( pDoc == 0 )
			return false;

		ConnectorList *pConnectors = pDoc->GetConnectorList();
		LinkList *pLinks = pDoc->GetLinkList();

		if( pConnectors == 0 || pLinks == 0 )
			return false;

		if( pConnectors->GetCount() == 0 || pLinks->GetCount() == 0 )
			return false;

		POSITION Position = pConnectors->GetHeadPosition();
		while( Position != NULL )
		{
			CConnector* pCheckConnector = pConnectors->GetNext( Position );
			if( pCheckConnector == 0 || !pCheckConnector->IsOnLayers( CLinkageDoc::MECHANISMLAYERS ) )
				continue;
			if( !pCheckConnector->IsPositionValid() )
				return false;
		}
	
		bool bResult = true;
		Position = pConnectors->GetHeadPosition();
		while( Position != NULL )
		{
			CConnector* pCheckConnector = pConnectors->GetNext( Position );
			if( pCheckConnector == 0 || !pCheckConnector->IsOnLayers( CLinkageDoc::MECHANISMLAYERS ) )
				continue;
			
			if( !pCheckConnector->IsFixed() )
			{
				pCheckConnector->SetPositionValid( false );
				bResult = false;
			}
		}
		Position = pLinks->GetHeadPosition();
		while( Position != 0 )
		{
			CLink *pLink = pLinks->GetNext( Position );
			if( pLink == 0 || !pLink->IsGear() )
				continue;

			if( !pLink->IsTempFixed() )
			{
				pLink->SetPositionValid( false );
				bResult = false;
			}
		}
	
		return bResult;
	}

	#if 0 // Dont need it since the link knows how to rotate around things on its own.
	bool RotateAround( CLink *pLink, CConnector* pConnector )
	{
		#pragma message( "FIX ME! RotateAround() will allow stretching of links." )
		#pragma message( "Just make two anchors with two links that are connected and one of the links gets stretched!" )

		if( pConnector == 0 )
			return false;

		pConnector->SetTempFixed( true );
		
		CFPoint Offset;
		Offset.x = pConnector->GetTempPoint().x - pConnector->GetOriginalPoint().x;
		Offset.y = pConnector->GetTempPoint().y - pConnector->GetOriginalPoint().y;

		POSITION Position = pLink->GetConnectorList()->GetHeadPosition();
		while( Position != 0 )
		{
			CConnector* pUseConnector = pLink->GetConnectorList()->GetNext( Position );
			if( pUseConnector == 0 || pUseConnector == pConnector )
				continue;
			
			// Is this connector connected to something fixed? If so, we should
			// get an error instead of trying to move it. This will break 
			// locomotive drive wheel assemblies but that is acceptable at this
			// time.
		
			if( pConnector->IsInput() )
			{
				CList<CLink*,CLink*> *pList = pUseConnector->GetLinkList();
				POSITION Position2 = pList->GetHeadPosition();
				while( Position2 != 0 )
				{
					CLink *pTestLink = pList->GetNext( Position2 );
					if( pList == 0 || pTestLink == pLink )
						continue;

					POSITION Position3 = pTestLink->GetConnectorList()->GetHeadPosition();
					while( Position3 != 0 )
					{
						CConnector* pAnotherConnector = pTestLink->GetConnectorList()->GetNext( Position3 );
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

			if( pLink->IsActuator() )
			{
				CFLine Line( pConnector->GetOriginalPoint(), Temp );
				Line.SetDistance( pLink->GetActuatedConnectorDistance( pConnector, pUseConnector ) );
				Temp = Line.GetEnd();
			}

			Temp.x += Offset.x;
			Temp.y += Offset.y;
			pUseConnector->MovePoint( Temp );
		
			pUseConnector->RotateAround( pConnector->GetTempPoint(), pConnector->GetTempRotationAngle() );
			pUseConnector->SetTempFixed( true );
			pLink->IncrementMoveCount();
		}

		// Save a rotation angle for this link.
		pLink->SetRotationAngle( pConnector->GetTempRotationAngle() );
		pLink->SetTempFixed( true );

		Position = pLink->GetFastenedElementList()->GetHeadPosition();
		while( Position != 0 )
		{
			CElementItem *pItem = pLink->GetFastenedElementList()->GetNext( Position );
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
	#endif

	bool FindGearsToMatch( CLink *pLink, GearConnectionList &GearConnections )
	{
		if( !pLink->IsFixed() && !pLink->IsTempFixed() )
			return false;

		int FoundCount = 0;

		POSITION Position = GearConnections.GetHeadPosition();
		while( Position != 0 )
		{
			CGearConnection *pConnection = GearConnections.GetNext( Position );
			if( pConnection == 0 || pConnection->m_pGear1 == 0 || pConnection->m_pGear2 == 0 )
				continue;

			if( pConnection->m_pGear1 == pLink || pConnection->m_pGear2 == pLink )
			{
				if( pConnection->m_pGear1 == pLink && pConnection->m_pGear2->IsFixed() )
					continue;
				else if( pConnection->m_pGear2 == pLink && pConnection->m_pGear1->IsFixed() )
					continue;

				// The other gear can be rotated if it's connector is fixed.
				CLink *pOtherGear = pConnection->m_pGear1 == pLink ? pConnection->m_pGear2 : pConnection->m_pGear1;
				CConnector *pConnector = pLink->GetConnector( 0 );
				CConnector *pOtherConnector = pOtherGear->GetConnector( 0 );
				if( pConnector == 0 || pOtherConnector == 0 || pOtherGear->IsFixed() || !pOtherConnector->IsFixed() )
					continue;

				// Find the link that connects these two gears. It may be the ground if they both reside on anchors.
				CLink *pConnectionLink = pConnector->GetSharingLink( pOtherConnector );
				if( pConnectionLink == 0 )
				{
					if( !pConnector->IsAnchor() || !pOtherConnector->IsAnchor() )
						continue;
				}

				double GearAngle = pLink->GetRotationAngle();
				double LinkAngle = pConnectionLink == 0 ? 0 : -pConnectionLink->GetRotationAngle();
				double UseAngle = GearAngle + LinkAngle;

				if( pConnection->m_ConnectionType == pConnection->CHAIN )
					UseAngle = -UseAngle;
			
				if( pOtherGear == pConnection->m_pGear1 )
					UseAngle *= pConnection->Ratio();
				else
					UseAngle /= pConnection->Ratio();

				UseAngle += LinkAngle;
				pOtherConnector->SetRotationAngle( -UseAngle );

				pOtherGear->RotateAround( pOtherConnector );
				++FoundCount;
				pLink->IncrementMoveCount();

				CElementItem *pFastenedTo = pOtherGear->GetFastenedTo();
				if( pFastenedTo != 0 )
				{
					// Gears are unusual because they can move the element that they are fastened to, not just be moved by that element.
					// But it only happens when a gear is fastened to a link, not to a connector.
					if( pFastenedTo->GetLink() != 0 )
						pFastenedTo->GetLink()->RotateAround( pOtherConnector );
				}
			}
		}

		return FoundCount > 0;
	}

	bool CheckForMovement( CLinkageDoc *pDoc, CLink *pLink )
	{
		GearConnectionList *pGearConnections = pDoc->GetGearConnections();

		/*
		 * Check for movement for A SPECIFIC LINK.
		 *
		 * The function will move Links that have an input connector. It
		 * will also find links that can be moved for some reason or another
		 * and move them in to their preoper new location if possible.
		 */
	 
		int FixedCount = pLink->GetFixedConnectorCount();

		bool bResult = true;

		if( FixedCount != pLink->GetConnectorCount() || !pLink->IsFixed() || pLink->IsGear() )
		{
			CConnector *pFixed = pLink->GetFixedConnector();
			if( pFixed != 0 && pFixed->IsInput() )
			{
				if( !pLink->IsFixed() )
				{
					if( pLink->IsGearAnchored() )
						bResult = pLink->FixAll();
					else
						bResult = pLink->RotateAround( pFixed );
				}

				if( pLink->IsGear() )
					bResult = FindGearsToMatch( pLink, *pGearConnections );
			}
			else if( pLink->IsGear() )
			{
				if( pLink->IsGearAnchored() && !pLink->IsFixed() )
					bResult = pLink->FixAll();
				bResult = FindGearsToMatch( pLink, *pGearConnections );
			}
			else if( FixedCount <= 1 )
			{
				bResult = FindLinksToMatch( pLink );
				if( !bResult )
					bResult = FindSlideLinksToMatch( pLink, pDoc->GetLinkList() );
				if( !bResult )
					bResult = FindLinkTriangleMatch( pLink );
			}
			else
				bResult = pLink->FixAll();
		}

		return bResult;
	}

	bool MoveSimulation( CLinkageDoc *pDoc )
	{
		if( pDoc == 0 )
			return false;

		ConnectorList *pConnectors = pDoc->GetConnectorList();
		LinkList *pLinks = pDoc->GetLinkList();

		if( pConnectors == 0 || pLinks == 0 )
			return false;

		if( pConnectors->GetCount() == 0 || pLinks->GetCount() == 0 )
			return false;

		POSITION Position;
		for(;;)
		{
			int MoveCount = 0;
	
			Position = pLinks->GetHeadPosition();
			while( Position != 0 )
			{
				CLink *pLink = pLinks->GetNext( Position );
				if( pLink == 0 || !pLink->IsOnLayers( CLinkageDoc::MECHANISMLAYERS ) )
					continue;
				pLink->ResetMoveCount();
				CheckForMovement( pDoc, pLink );
				MoveCount += pLink->GetMoveCount();
			}
			if( MoveCount == 0 )
				break;
		}

		if( !ValidateMovement( pDoc ) )
		{
			// Add motion points up to the current position.
			Position = pConnectors->GetHeadPosition();
			while( Position != 0 )
			{
				CConnector* pConnector = pConnectors->GetNext( Position );
				if( pConnector == 0 )
					continue;
				pConnector->AddMotionPoint();
			}

			m_bSimulationValid = false;
			return false;
		}
		
		Position = pConnectors->GetHeadPosition();
		while( Position != 0 )
		{
			CConnector* pConnector = pConnectors->GetNext( Position );
			if( pConnector == 0 )
				continue;
			pConnector->AddMotionPoint();
			pConnector->MakePermanent();
		}

		Position = pLinks->GetHeadPosition();
		while( Position != 0 )
		{
			CLink *pLink = pLinks->GetNext( Position );
			if( pLink == 0 )
				continue;
			pLink->MakePermanent();
			pLink->UpdateController();
		}

		return true;
	}

	bool JoinToSlideLinkSpecial( CLink *pLink, CConnector *pFixedConnection, CConnector *pCommonConnector, CLink *pOtherLink )
	{
		/*
		 * Rotate the "this" Link and slide the other link so that they are still
		 * connected. The "this" Link has only one connection that is in a new
		 * temp location, pFixedConnection, and the other Link needs to have two
		 * fixed sliding connectors that it must slide through.
		 *
		 * The "this" Link will not have proper temp locations for all
		 * connectors yet, only the fixed one. It is in an odd screwed up state
		 * at the moment but will be fixed shortly.
		 *
		 * SPECIAL CASE: If this link IS pOtherLink link then we are only trying
		 * to slide this link to a new position based on slider pCommonConnector
		 * ( which is a slider but not one of the sliders that we are sliding ON).
		 * The common connector has limits that are fixed and we can move this 
		 * link to a new position without changing any other link.
		 *
		 * This code is for the special case where the other link slides on a 
		 * curved slider path. The sliders must have the exact same slide radius
		 * for this to work properly.
		 */

		if( pCommonConnector == 0 )
			return false;

		bool bOtherLinkOnly = ( pLink == pOtherLink && pFixedConnection == 0 );

		CConnector *pLimit1 = 0;	 
		CConnector *pLimit2 = 0;	 
		CConnector *pSlider1 = 0;	 
		CConnector *pSlider2 = 0;	 
		CConnector *pActualSlider1 = 0;	 
		CConnector *pActualSlider2 = 0;	 
		bool bOtherLinkHasSliders;
	
		// Double check that it can slide and get the limits for that slide.
		if( !pOtherLink->CanOnlySlide( &pLimit1, &pLimit2, &pSlider1, &pSlider2, &bOtherLinkHasSliders ) )
			return false;

		pActualSlider1 = pSlider1;
		pActualSlider2 = pSlider2;

		// The sliding connectors MUST use the exact same slide radius.
		if( pSlider1->GetSlideRadius() != pSlider2->GetSlideRadius() )
			return false;

		/*
		 * Check to make sure that we are not trying to position this link using a
		 * common connector that is also one of the sliders that is on the "track"
		 * because the slider is then limiting the movement, not defining it.
		 */
		if( pCommonConnector == pSlider1 || pCommonConnector == pSlider2 )
			return false;

		CFArc LinkArc;
		if( !pSlider1->GetSliderArc( LinkArc, true ) )
			return false;

	//		DebugItemList.AddTail( new CDebugItem( LinkArc ) );

		if( bOtherLinkHasSliders )
		{
			SWAP( pSlider1, pLimit1 );
			SWAP( pSlider2, pLimit2 );
		}

		/*
		 * Create a circle that represents the valid positions of the common connector
		 * based on the sliding connectors.
		 */

		CFCircle CommonCircle = LinkArc;
		CommonCircle.r = Distance( CommonCircle.GetCenter(), pCommonConnector->GetPoint() );

 		CFPoint Intersect;
 		CFPoint Intersect2;
 		bool bHit = false;
 		bool bHit2 = false;

		if( bOtherLinkOnly )
		{
			CConnector *pTempLimit1 = 0;	 
			CConnector *pTempLimit2 = 0;	 
			if( !pCommonConnector->GetSlideLimits( pTempLimit1, pTempLimit2 ) )
				return false;

			if( pCommonConnector->GetSlideRadius() == 0 )
			{
				bool bOnSegments = false;
				Intersects( CFLine( pTempLimit1->GetTempPoint(), pTempLimit2->GetTempPoint() ), 
								 CommonCircle, Intersect, Intersect2, bHit, bHit2, false, false );
			}
			else
			{
				// This is the arc that the common connector slides on for this link, not for the sliding pOtherLink.
				CFArc TheArc;
				if( !pCommonConnector->GetSliderArc( TheArc, false ) )
					return false;

				bHit = bHit2 = CommonCircle.CircleIntersection( TheArc, &Intersect, &Intersect2 );
			}
		}
		else
		{
			if( pFixedConnection == 0 )
				return false;

			double r = Distance( pFixedConnection->GetOriginalPoint(), pCommonConnector->GetOriginalPoint() );
			r = pLink->GetActuatedConnectorDistance( pFixedConnection, pCommonConnector );
 			CFCircle Circle( pFixedConnection->GetTempPoint(), r );

			bHit = bHit2 = CommonCircle.CircleIntersection( Circle, &Intersect, &Intersect2 );
 		}
	
		if( !bHit && !bHit2 )
			return false;
		
		if( !bHit2 )
			Intersect2 = Intersect;
		else if( !bHit )
			Intersect = Intersect2;

		double d1 = Distance( pCommonConnector->GetTempPoint(), Intersect );
		double d2 = Distance( pCommonConnector->GetTempPoint(), Intersect2 );
	
		if( d2 < d1 )
			Intersect = Intersect2;

		if( !bOtherLinkOnly )
		{
			double TempAngle = GetAngle( pFixedConnection->GetTempPoint(), Intersect, pFixedConnection->GetOriginalPoint(), pCommonConnector->GetOriginalPoint() );
		
			pFixedConnection->SetRotationAngle( TempAngle );
			if( !pLink->RotateAround( pFixedConnection ) )
				return false;

			pLink->IncrementMoveCount( -1 ); // Don't count the common connector twice.
		}

		// Calculate the difference between the origibal center of the slider arc and the new center of the slider arc.
		CFArc OriginalArc;
		if( !pSlider1->GetSliderArc( OriginalArc, true ) )
			return false;

		CFPoint Offset = LinkArc.GetCenter() - OriginalArc.GetCenter();

		// Caluclate the new unrotated location of the common connector and get the angle to the new location.

		CFPoint OrignalPoint = pCommonConnector->GetOriginalPoint();
		OrignalPoint += Offset;
		double Angle = GetAngle( LinkArc.GetCenter(), OrignalPoint, Intersect );

		// Cheat and move the common connector to the exact intersection location
		// in case the math caused it to be off a tiny bit. It shouldn't happen
		// but it looks like it when running some simulations.
	
		pCommonConnector->MovePoint( Intersect );

		/*
		 * Now move and rotate the other link so that it passes through its
		 * sliders and meets with the new common connector location. The angle
		 * of rotation is the original angle we figured out earlier in the process
		 * and the rotation happens around the common connector.
		 */
		
		pCommonConnector->SetRotationAngle( -Angle );
		if( !pOtherLink->RotateAround( pCommonConnector ) )
			return false;

		/*
		 * Do bounds checking of the sliding connectors on the sliding arc.
		 * This is easier to do now than earlier. Use the new arc location.
		 */

		if( !pActualSlider1->GetSliderArc( LinkArc ) )
		{
			pLink->ResetMoveCount();
			return false;
		}
		CFArc LinkArc2;
		if( !pActualSlider2->GetSliderArc( LinkArc2 ) )
		{
			pLink->ResetMoveCount();
			return false;
		}

		if( !LinkArc.PointOnArc( pActualSlider1->GetPoint() ) )
		{
			pSlider1->SetPositionValid( false );
			pLink->ResetMoveCount();
			return false;
		}

		if( !LinkArc2.PointOnArc( pActualSlider2->GetPoint() ) )
		{
			pSlider2->SetPositionValid( false );
			pLink->ResetMoveCount();
			return false;
		}


		return true;
	}

	bool JoinToSlideLink( CLink *pLink, CConnector *pFixedConnection, CConnector *pCommonConnector, CLink *pOtherLink )
	{
		/*
		 * Rotate the "this" Link and slide the other link so that they are still
		 * connected. The "this" Link has only one connection that is in a new
		 * temp location, pFixedConnection, and the other Link needs to have two
		 * fixed sliding connectors that it must slide through.
		 *
		 * The "this" Link will not have proper temp locations for all
		 * connectors yet, only the fixed one. It is in an odd screwed up state
		 * at the moment but will be fixed shortly.
		 *
		 * SPECIAL CASE: If this link IS pOtherLink link then we are only trying
		 * to slide this link to a new position based on slider pCommonConnector
		 * ( which is a slider but not one of the sliders that we are sliding ON).
		 * The common connector has limits that are fixed and we can move this 
		 * link to a new position without changing any other link.
		 */

		if( pCommonConnector == 0 )
			return false;
		
		bool bOtherLinkOnly = ( pLink == pOtherLink && pFixedConnection == 0 );

		CConnector *pLimit1 = 0;	 
		CConnector *pLimit2 = 0;	 
		CConnector *pSlider1 = 0;	 
		CConnector *pSlider2 = 0;	 
		bool bOtherLinkHasSliders;
	
		// Double check that it can slide and get the limits for that slide.
		if( !pOtherLink->CanOnlySlide( &pLimit1, &pLimit2, &pSlider1, &pSlider2, &bOtherLinkHasSliders ) )
			return false;

		if( pSlider1->GetSlideRadius() != pSlider2->GetSlideRadius() )
			return false;

		/*
		 * Test for the case where the path of the slide is not linear and handle
		 * it in another function.
		 */

		if( pSlider1->GetSlideRadius() != 0 )
			return JoinToSlideLinkSpecial( pLink, pFixedConnection, pCommonConnector, pOtherLink );

		/*
		 * IMPORTANT...
		 * bOtherLinkHasSliders tells us if the other link is the link with the
		 * sliders or if the other link is the link with the limits. This changes
		 * how the valid range of motion for the common connector is calculated.
		 */

		/*
		 * Check to make sure that we are not trying to position this link using a
		 * common connector that is also one of the sliders that is on the "track"
		 * because the slider is then limiting the movement, not defining it.
		 */
		if( pCommonConnector == pSlider1 || pCommonConnector == pSlider2 )
			return false;


		if( bOtherLinkHasSliders )
		{
			SWAP( pSlider1, pLimit1 );
			SWAP( pSlider2, pLimit2 );
		}

		/*
		 * Take the angle and distance from pLimit1 to the common connector. Add
		 * the slider change angle. Get a point that is offset from the slider
		 * originally closest to pLimit2 (yes, pLimit2) using the new angle and
		 * distance. This is one end of a segment that limits the location of the
		 * common connector.
		 *
		 * When the sliders are on the other link, the sliders and limits are swapped
		 * but the code works the same otherwise.
		 */
		bool bSlidersAreMoving = pSlider1->HasLink( pLink );

		double Angle = GetAngle( pSlider1->GetPoint(), pSlider2->GetPoint(), pSlider1->GetOriginalPoint(), pSlider2->GetOriginalPoint() );
		double TempAngle = GetAngle( pLimit1->GetOriginalPoint(), pCommonConnector->GetOriginalPoint() ) + Angle;
		double TempDistance = Distance( pLimit1->GetOriginalPoint(), pCommonConnector->GetOriginalPoint() );
		CFPoint EndPoint1; EndPoint1.SetPoint( pSlider1->GetPoint(), TempDistance, TempAngle );
		TempAngle = GetAngle( pLimit2->GetOriginalPoint(), pCommonConnector->GetOriginalPoint() ) + Angle;
		TempDistance = Distance( pLimit2->GetOriginalPoint(), pCommonConnector->GetOriginalPoint() );
		CFPoint EndPoint2; EndPoint2.SetPoint( pSlider2->GetPoint(), TempDistance, TempAngle );
	
		/*
		 * There is now a segment that is the limit for the common connector.
		 * Find where we can rotate the current link so that it intersects
		 * that segment. Abort if it misses the segment.
		 *	
		 * Move the current link and the other link to meet at the new common
		 * connector. The change in angle is the same angle change needed here
		 * to rotate the other link before offsetting it to the new common 
		 * connector location.
		 *
		 * SPECIAL CASE: If the limits of the slider pCommonConnector are fixed
		 * then the common connector location will be defined by a line 
		 * intersection and not a circle intersection.
		 */
	 
 		CFPoint Intersect;
 		CFPoint Intersect2;
 		bool bHit = false;
 		bool bHit2 = false;

 		CFLine LimitLine( EndPoint1, EndPoint2 );

		if( bOtherLinkOnly )
		{
			CConnector *pTempLimit1 = 0;	 
			CConnector *pTempLimit2 = 0;	 
			if( !pCommonConnector->GetSlideLimits( pTempLimit1, pTempLimit2 ) )
				return false;

			if( pCommonConnector->GetSlideRadius() == 0 )
			{
			
				bool bOnSegments = false;
				if( !Intersects( pTempLimit1->GetTempPoint(), pTempLimit2->GetTempPoint(), 
								 EndPoint1, EndPoint2, Intersect, 0, &bOnSegments ) 
					|| !bOnSegments )
				{
					return false;
				}
 				bHit = true;
			}
			else
			{
				CFArc TheArc;
				if( !pCommonConnector->GetSliderArc( TheArc, false ) )
					return false;

				Intersects( LimitLine, TheArc, Intersect, Intersect2, bHit, bHit2, false, false );

				if( !bHit && !bHit2 )
					return false;

				if( !TheArc.PointOnArc( Intersect2 ) )
				{
					Intersect2 = Intersect;
					if( !TheArc.PointOnArc( Intersect ) )
						return false;
				}
			}
		}
		else
		{
			if( pFixedConnection == 0 )
				return false;
			
			double r = Distance( pFixedConnection->GetOriginalPoint(), pCommonConnector->GetOriginalPoint() );
			r = pLink->GetActuatedConnectorDistance( pFixedConnection, pCommonConnector );
 			CFCircle Circle( pFixedConnection->GetTempPoint(), r );
			Intersects( LimitLine, Circle, Intersect, Intersect2, bHit, bHit2, false, false );
 		}
	
		if( !bHit && !bHit2 )
			return false;
		
		if( !bHit2 )
			Intersect2 = Intersect;
		else if( !bHit )
			Intersect = Intersect2;

		double d1 = Distance( pCommonConnector->GetTempPoint(), Intersect );
		double d2 = Distance( pCommonConnector->GetTempPoint(), Intersect2 );
	
		if( d2 < d1 )
			Intersect = Intersect2;
		
		if( !bOtherLinkOnly )
		{
			double TempAngle = GetAngle( pFixedConnection->GetTempPoint(), Intersect, pFixedConnection->GetOriginalPoint(), pCommonConnector->GetOriginalPoint() );
		
			pFixedConnection->SetRotationAngle( TempAngle );
			if( !pLink->RotateAround( pFixedConnection ) )
				return false;

			pLink->IncrementMoveCount( -1 ); // Don't count the common connector twice.
		}
			
		// Cheat and move the common connector to the exact intersection location
		// in case the math caused it to be off a tiny bit. It shouldn't happen
		// but it looks like it when running some simulations.
	
		pCommonConnector->MovePoint( Intersect );
		
		/*
		 * Now move and rotate the other link so that it passes through its
		 * sliders and meets with the new common connector location. The angle
		 * of rotation is the original angle we figured out earlier in the process
		 * and the rotation happens around the common connector.
		 */
		
		pCommonConnector->SetRotationAngle( Angle );
		if( !pOtherLink->RotateAround( pCommonConnector ) )
			return false;

		return true;
	}

	bool SlideToLink( CLink* pLink, CConnector *pFixedConnection, CConnector *pSlider, CConnector *pLimit1, CConnector *pLimit2 )
	{
		/*
		 * Rotate the "this" Link so that the slider it on the segment between 
		 * the limit connectors. The other link is left unmodified AT THIS TIME.
		 * The slide path may be an arc so move and rotate the slider to be on the arc.
		 *
		 * The "this" Link will not have proper temp locations for all
		 * connectors yet, only the fixed one. It is in an odd screwed up state
		 * at the moment but will be fixed shortly.
		 */
	 
		double r;   // = Distance( pFixedConnection->GetOriginalPoint(), pSlider->GetOriginalPoint() );
		r = pLink->GetActuatedConnectorDistance( pFixedConnection, pSlider );
 		CFCircle Circle( pFixedConnection->GetTempPoint(), r );

		CFPoint Intersect;
		CFPoint Intersect2;

		if( pSlider->GetSlideRadius() == 0 )
		{
 			CFLine Line( pLimit1->GetTempPoint(), pLimit2->GetTempPoint() );
 	
 			bool bHit = false;
 			bool bHit2 = false;
			Intersects( Line, Circle, Intersect, Intersect2, bHit, bHit2, false, false );

			if( !bHit && !bHit2 )
			{
	//			DebugItemList.AddTail( new CDebugItem( Line ) );
	//			DebugItemList.AddTail( new CDebugItem( Circle ) );
				return false;
			}
		
			if( !bHit2 )
				Intersect2 = Intersect;
			else if( !bHit )
				Intersect = Intersect2;
		}
		else
		{
			CFArc TheArc;
			if( !pSlider->GetSliderArc( TheArc ) )
				return false;

			if( !TheArc.CircleIntersection( Circle, &Intersect, &Intersect2 ) )
				return false;

			if( !TheArc.PointOnArc( Intersect2 ) )
			{
				Intersect2 = Intersect;
				if( !TheArc.PointOnArc( Intersect ) )
					return false;
			}
		}

			
		// Try to give the point momentum by determining where it would be if 
		// it moved from a previous point through it's current point to some 
		// new point. Use that new point as the basis for selecting the new 
		// location.
	
		CFLine TestLine( pSlider->GetPreviousPoint(), pSlider->GetPoint() );
		TestLine.SetDistance( TestLine.GetDistance() + 1 );
		CFPoint SuggestedPoint = TestLine.GetEnd();
		
		double d1 = Distance( SuggestedPoint /*pCommonConnector->GetPoint()*/, Intersect );
		double d2 = Distance( SuggestedPoint /*pCommonConnector->GetPoint()*/, Intersect2 );
	
		if( d2 < d1 )
			Intersect = Intersect2;
		
		pFixedConnection->SetRotationAngle( GetAngle( pFixedConnection->GetTempPoint(), Intersect, pFixedConnection->GetOriginalPoint(), pSlider->GetOriginalPoint() ) );
		if( !pLink->RotateAround( pFixedConnection ) )
			return false;

		// Stretch or compress the link just a tiny bit to ensure that the end of it
		// is actually at the intersection point. Otherwise there are some numeric
		// issues that cause the picture of the mechanism to look a tiny bit off.

		pSlider->MovePoint( Intersect );
		
		return true;
	}
	
	bool SlideToSlider( CLink *pLink, CConnector *pTargetConnector, CConnector *pTargetLimit1, CConnector *pTargetLimit2 )
	{
		if( pLink->IsFixed() )
			return false;

		/*
		 * Slide the link so that it's slide limits line up through the fixed sliding connector.
		 */

		CConnector *pLimit1 = 0;	 
		CConnector *pLimit2 = 0;	 
		CConnector *pSlider1 = 0;	 
		CConnector *pSlider2 = 0;	 
		CConnector *pActualSlider1 = 0;	 
		CConnector *pActualSlider2 = 0;	 
		bool bOtherLinkHasSliders;
	
		// Double check that it can slide and get the limits for that slide.
		if( !pLink->CanOnlySlide( &pLimit1, &pLimit2, &pSlider1, &pSlider2, &bOtherLinkHasSliders ) )
			return false;

		if( !bOtherLinkHasSliders )
		{
			SWAP( pSlider1, pLimit1 );
			SWAP( pSlider2, pLimit2 );
		}

		// Get a line through the connector slider limits in their original location.
		CFLine ConnectedLine( pTargetLimit1->GetOriginalPoint(), pTargetLimit2->GetOriginalPoint() );

		// Get a line through the link slide limits in their original location.
		CFLine SlideLine( pLimit1->GetOriginalPoint(), pLimit2->GetOriginalPoint() );

		// Their intersection.
		CFPoint Intersect;
		if( !Intersects( ConnectedLine, SlideLine, Intersect ) )
			return false;

		// Save the distance form the intersection to one of the sliding connectors on this link.
		double IntersectToSlider = Distance( Intersect, pSlider1->GetOriginalPoint() );
		if( !bOtherLinkHasSliders )
			IntersectToSlider = -IntersectToSlider;

		// Get a line through the new location of the link slide limits.
		CFLine NewSlideLine( pLimit1->GetPoint(), pLimit2->GetPoint() );

		DebugItemList.AddTail( new CDebugItem( ConnectedLine ) );
		DebugItemList.AddTail( new CDebugItem( NewSlideLine ) );

		// Get the change in angle from the original to the new slide limit line.
		double ChangeAngle = NewSlideLine.GetAngle() - SlideLine.GetAngle();

		// Figure out the angle between the two lines at the intersection point. Use the connected slider location for one point and one of the slide limits for the other.
		// double SlideToSlideAngle = GetAngle( Intersect, pTargetConnector->GetOriginalPoint(), pLimit1->GetOriginalPoint() );

		// Move the connected slider limit line to go through its new point.
		ConnectedLine -= ConnectedLine.GetStart();
		ConnectedLine += pTargetConnector->GetPoint();

		// Change the connected slider limit line to be the proper new angle.
		ConnectedLine.m_End.RotateAround( ConnectedLine.m_Start, ChangeAngle );

		DebugItemList.AddTail( new CDebugItem( ConnectedLine ) );

		// Get the intersection of the connected slider limits and the link slider limits.
		if( !Intersects( ConnectedLine, NewSlideLine, Intersect ) )
			return false;

		DebugItemList.AddTail( new CDebugItem( Intersect ) );

		// Find the location of the slider on the link based on its distance from the intersection and being on the new limit line.
		// Use the NewSlideLine to measure this because it is already at the proper angle.
		NewSlideLine -= NewSlideLine.m_Start;
		NewSlideLine += Intersect;
		NewSlideLine.SetDistance( IntersectToSlider );
		DebugItemList.AddTail( new CDebugItem( NewSlideLine ) );

		pSlider1->SetRotationAngle( ChangeAngle );
		pSlider1->MovePoint( NewSlideLine.m_End );
		pLink->RotateAround( pSlider1 );



		/////// JUST FOR DEBUGGING
		pLink->SetTempFixed( true );

		
		/*




		If not on the line, return false

		Hmmm, figure out how far to slide the link to get it to the sliding connector

		*/

#if 0

		/*
		 * Rotate the "this" Link so that the slider it on the segment between 
		 * the limit connectors. The other link is fixed and is not changed.
		 *
		 * The "this" Link will not have proper temp locations for all
		 * connectors yet, only the fixed one. It is in an odd screwed up state
		 * at the moment but will be fixed shortly.
		 */
	 
 		CFCircle Circle( pFixedConnection->GetTempPoint(), pSlider->GetTempPoint() );
		Circle.r = Distance( pSlider->GetTempPoint(), pFixedConnection->GetTempPoint() );

		CFPoint Offset = pFixedConnection->GetTempPoint() - pFixedConnection->GetOriginalPoint();

 		CFPoint Intersect;
 		CFPoint Intersect2;

		if( pSlider->GetSlideRadius() == 0 )
		{
 			CConnector *pLimit1 = 0;
 			CConnector *pLimit2 = 0;
 			pSlider->GetSlideLimits( pLimit1, pLimit2 );
 	
 			// The limits need to be offset by the offset of the fixed connector
 			// before they can be used. Offset the limit line to accomplish this.
 	
 			CFLine LimitLine( pLimit1->GetOriginalPoint(), pLimit2->GetOriginalPoint() );
 			LimitLine += Offset;

 			bool bHit = false;
 			bool bHit2 = false;

			Intersects( LimitLine, Circle, Intersect, Intersect2, bHit, bHit2, false, false );
	
			if( !bHit && !bHit2 )
				return false;
		
			if( !bHit2 )
				Intersect2 = Intersect;
			else if( !bHit )
				Intersect = Intersect2;
		}
		else
		{
			CFArc TheArc;
			if( !pSlider->GetSliderArc( TheArc, true ) )
				return false;

			TheArc.x += Offset.x;
			TheArc.y += Offset.y;

			if( !TheArc.CircleIntersection( Circle, &Intersect, &Intersect2 ) )
				return false;

			if( !TheArc.PointOnArc( Intersect2 ) )
			{
				Intersect2 = Intersect;
				if( !TheArc.PointOnArc( Intersect ) )
					return false;
			}
		}

		// No momentum for this calculation.
		// The intersection point is the point of intersection if the slider were 
		// in it's original location. it is not the new slider point.
		
		double d1 = Distance( pSlider->GetTempPoint(), Intersect );
		double d2 = Distance( pSlider->GetTempPoint(), Intersect2 );
	
		if( d2 < d1 )
			Intersect = Intersect2;
		
		double Angle = GetAngle( pFixedConnection->GetTempPoint(), pSlider->GetTempPoint(), Intersect );
		Angle = GetClosestAngle( pFixedConnection->GetRotationAngle(), Angle );
		pFixedConnection->SetRotationAngle( Angle );
		if( !pLink->RotateAround( pFixedConnection ) )
			return false;

#endif

		return true;
	}

	bool MoveToSlider( CLink *pLink, CConnector *pFixedConnection, CConnector *pSlider )
	{
		/*
		 * Rotate the "this" Link so that the slider it on the segment between 
		 * the limit connectors. The other link is fixed and is not changed.
		 *
		 * The "this" Link will not have proper temp locations for all
		 * connectors yet, only the fixed one. It is in an odd screwed up state
		 * at the moment but will be fixed shortly.
		 */
	 
 		CFCircle Circle( pFixedConnection->GetTempPoint(), pSlider->GetTempPoint() );
		Circle.r = Distance( pSlider->GetTempPoint(), pFixedConnection->GetTempPoint() );

		CFPoint Offset = pFixedConnection->GetTempPoint() - pFixedConnection->GetOriginalPoint();

 		CFPoint Intersect;
 		CFPoint Intersect2;

		if( pSlider->GetSlideRadius() == 0 )
		{
 			CConnector *pLimit1 = 0;
 			CConnector *pLimit2 = 0;
 			pSlider->GetSlideLimits( pLimit1, pLimit2 );
 	
 			// The limits need to be offset by the offset of the fixed connector
 			// before they can be used. Offset the limit line to accomplish this.
 	
 			CFLine LimitLine( pLimit1->GetOriginalPoint(), pLimit2->GetOriginalPoint() );
 			LimitLine += Offset;

 			bool bHit = false;
 			bool bHit2 = false;

			Intersects( LimitLine, Circle, Intersect, Intersect2, bHit, bHit2, false, false );
	
			if( !bHit && !bHit2 )
				return false;
		
			if( !bHit2 )
				Intersect2 = Intersect;
			else if( !bHit )
				Intersect = Intersect2;
		}
		else
		{
			CFArc TheArc;
			if( !pSlider->GetSliderArc( TheArc, true ) )
				return false;

			TheArc.x += Offset.x;
			TheArc.y += Offset.y;

			if( !TheArc.CircleIntersection( Circle, &Intersect, &Intersect2 ) )
				return false;

			if( !TheArc.PointOnArc( Intersect2 ) )
			{
				Intersect2 = Intersect;
				if( !TheArc.PointOnArc( Intersect ) )
					return false;
			}
		}

		// No momentum for this calculation.
		// The intersection point is the point of intersection if the slider were 
		// in it's original location. it is not the new slider point.
		
		double d1 = Distance( pSlider->GetTempPoint(), Intersect );
		double d2 = Distance( pSlider->GetTempPoint(), Intersect2 );
	
		if( d2 < d1 )
			Intersect = Intersect2;
		
		double Angle = GetAngle( pFixedConnection->GetTempPoint(), pSlider->GetTempPoint(), Intersect );
		Angle = GetClosestAngle( pFixedConnection->GetRotationAngle(), Angle );
		pFixedConnection->SetRotationAngle( Angle );
		if( !pLink->RotateAround( pFixedConnection ) )
			return false;

		return true;
	}

	bool JoinToLink( CLink *pLink, CConnector *pFixedConnection, CConnector *pCommonConnector, CLink *pOtherToRotate )
	{
		/*
		 * Rotate the pLink and the other Link so that they are still
		 * connected. The pLink has only one connection that is in a new
		 * temp location, pFixedConnection, and the other Link needs to have a
		 * fixed connector of some sort too (that is not an input!).
		 *
		 * The pLink will not have proper temp locations for all
		 * connectors yet, only the fixed one. It is in an odd screwed up state
		 * at the moment but will be fixed shortly.
		 */
	 
		CConnector *pOtherFixedConnector = pOtherToRotate->GetFixedConnector();
		if( pOtherFixedConnector == 0 )
		{
			if( pOtherToRotate->GetFixedConnectorCount() == 0 )
				return true; // Maybe it can be dealt with later in processing by another chain of Links.
			else
				return false;
		}
		if( pOtherFixedConnector->IsInput() )
			return false;
	
	//	CConnector* pCommonConnector = GetCommonConnector( this, pOtherToRotate );
	//	if( pCommonConnector == 0 )
	//		return false;

		// "this" link could be connected to something that can't move. Check to make
		// sure but don't otherwise move anything else. Check all Links connected to
		// this other than the pOtherToRotate Link and if any of them have fixed connectors,
		// return immediately.
		for( POSITION Position = pLink->GetConnectorList()->GetHeadPosition(); Position != 0; )
		{
			CConnector* pTestConnector = pLink->GetConnectorList()->GetNext( Position );
			if( pTestConnector == 0 || pTestConnector == pFixedConnection || pTestConnector == pFixedConnection )
				continue;
			for( POSITION Position2 = pTestConnector->GetLinkList()->GetHeadPosition(); Position2 != 0; )
			{
				CLink *pTestLink = pTestConnector->GetLinkList()->GetNext( Position2 );
				if( pTestLink == 0 || pTestLink == pLink || pTestLink == pOtherToRotate )
					continue;
				if( pTestLink->GetFixedConnectorCount() != 0 )
				{
					pCommonConnector->SetPositionValid( false );
					return false;
				}
			}
		}
		
		// pOtherToRotate could be connected to something that can't move. Check to make
		// sure but don't otherwise move anything else. Check all Links connected to
		// pOtherToRotate other than this Link and if any of them have fixed connectors,
		// return immediately.
		for( POSITION Position = pOtherToRotate->GetConnectorList()->GetHeadPosition(); Position != 0; )
		{
			CConnector* pTestConnector =  pOtherToRotate->GetConnectorList()->GetNext( Position );
			if( pTestConnector == 0 || pTestConnector == pOtherFixedConnector || pTestConnector == pFixedConnection )
				continue;
			for( POSITION Position2 = pTestConnector->GetLinkList()->GetHeadPosition(); Position2 != 0; )
			{
				CLink *pTestLink = pTestConnector->GetLinkList()->GetNext( Position2 );
				if( pTestLink == 0 || pTestLink == pLink || pTestLink == pOtherToRotate )
					continue;
				if( pTestLink->GetFixedConnectorCount() != 0 )
				{
					pCommonConnector->SetPositionValid( false );
					return false;
				}
			}
		}

		double r1 = Distance( pFixedConnection->GetOriginalPoint(), pCommonConnector->GetOriginalPoint() );
		double r2 = Distance( pOtherFixedConnector->GetOriginalPoint(), pCommonConnector->GetOriginalPoint() );

		r1 = pLink->GetActuatedConnectorDistance( pFixedConnection, pCommonConnector );
		r2 = pOtherToRotate->GetActuatedConnectorDistance( pOtherFixedConnector, pCommonConnector );
	
		CFCircle Circle1( pFixedConnection->GetTempPoint(), r1 );
		CFCircle Circle2( pOtherFixedConnector->GetTempPoint(), r2 );
	
		CFPoint Intersect;
		CFPoint Intersect2;
	
		if( !Circle1.CircleIntersection( Circle2, &Intersect, &Intersect2 ) )
			return false;
		
		// Try to give the point momentum by determining where it would be if 
		// it moved from a previous point through it's current point to some 
		// new point. Use that new point as the basis for selecting the new 
		// location.
	
		CFLine Line( pCommonConnector->GetPreviousPoint(), pCommonConnector->GetPoint() );
		Line.SetDistance( Line.GetDistance() * 2 );
		CFPoint SuggestedPoint = Line.GetEnd();
		
		double d1 = Distance( SuggestedPoint /*pCommonConnector->GetPoint()*/, Intersect );
		double d2 = Distance( SuggestedPoint /*pCommonConnector->GetPoint()*/, Intersect2 );
	
		if( d2 < d1 )
			Intersect = Intersect2;
		
		double Angle = GetAngle( pFixedConnection->GetTempPoint(), Intersect, pFixedConnection->GetOriginalPoint(), pCommonConnector->GetOriginalPoint() );
		Angle = GetClosestAngle( pFixedConnection->GetRotationAngle(), Angle );
		pFixedConnection->SetRotationAngle( Angle );
		if( !pLink->RotateAround( pFixedConnection ) )
			return false;
		pCommonConnector->SetTempFixed( false ); // needed so it can be rotated again.
		pLink->IncrementMoveCount( -1 ); // Don't count that one twice.
		Angle = GetAngle( pOtherFixedConnector->GetTempPoint(), Intersect, pOtherFixedConnector->GetOriginalPoint(), pCommonConnector->GetOriginalPoint() );
		Angle = GetClosestAngle( pOtherFixedConnector->GetRotationAngle(), Angle );
		pOtherFixedConnector->SetRotationAngle( Angle );
		if( !pOtherToRotate->RotateAround( pOtherFixedConnector ) )
			return false;
		
		return true;
	}

	#if 0

	bool JoinToLink( CLink *pLink, CConnector *pFixedConnection, CConnector *pCommonConnector, CLink *pOtherToRotate )
	{
		/*
		 * Rotate the pLink and the other Link so that they are still
		 * connected. The pLink has only one connection that is in a new
		 * temp location, pFixedConnection, and the other Link needs to have a
		 * fixed connector of some sort too (that is not an input!).
		 *
		 * The pLink will not have proper temp locations for all
		 * connectors yet, only the fixed one. It is in an odd screwed up state
		 * at the moment but will be fixed shortly.
		 */
	 
		CConnector *pOtherFixedConnector = pOtherToRotate->GetFixedConnector();
		if( pOtherFixedConnector == 0 )
		{
			if( pOtherToRotate->GetFixedConnectorCount() == 0 )
				return true; // Maybe it can be dealt with later in processing by another chain of Links.
			else
				return false;
		}
		if( pOtherFixedConnector->IsInput() )
			return false;
	
	//	CConnector* pCommonConnector = GetCommonConnector( this, pOtherToRotate );
	//	if( pCommonConnector == 0 )
	//		return false;

		// "this" link could be connected to something that can't move. Check to make
		// sure but don't otherwise move anything else. Check all Links connected to
		// this other than the pOtherToRotate Link and if any of them have fixed connectors,
		// return immediately.
		for( POSITION Position = pLink->GetConnectorList()->GetHeadPosition(); Position != 0; )
		{
			CConnector* pTestConnector = pLink->GetConnectorList()->GetNext( Position );
			if( pTestConnector == 0 || pTestConnector == pFixedConnection || pTestConnector == pFixedConnection )
				continue;
			for( POSITION Position2 = pTestConnector->GetLinkList()->GetHeadPosition(); Position2 != 0; )
			{
				CLink *pTestLink = pTestConnector->GetLinkList()->GetNext( Position2 );
				if( pTestLink == 0 || pTestLink == pLink || pTestLink == pOtherToRotate )
					continue;
				if( pTestLink->GetFixedConnectorCount() != 0 )
				{
					pCommonConnector->SetPositionValid( false );
					return false;
				}
			}
		}
		
		// pOtherToRotate could be connected to something that can't move. Check to make
		// sure but don't otherwise move anything else. Check all Links connected to
		// pOtherToRotate other than this Link and if any of them have fixed connectors,
		// return immediately.
		for( POSITION Position = pOtherToRotate->GetConnectorList()->GetHeadPosition(); Position != 0; )
		{
			CConnector* pTestConnector =  pOtherToRotate->GetConnectorList()->GetNext( Position );
			if( pTestConnector == 0 || pTestConnector == pOtherFixedConnector || pTestConnector == pFixedConnection )
				continue;
			for( POSITION Position2 = pTestConnector->GetLinkList()->GetHeadPosition(); Position2 != 0; )
			{
				CLink *pTestLink = pTestConnector->GetLinkList()->GetNext( Position2 );
				if( pTestLink == 0 || pTestLink == pLink || pTestLink == pOtherToRotate )
					continue;
				if( pTestLink->GetFixedConnectorCount() != 0 )
				{
					pCommonConnector->SetPositionValid( false );
					return false;
				}
			}
		}

		double r1 = Distance( pFixedConnection->GetOriginalPoint(), pCommonConnector->GetOriginalPoint() );
		double r2 = Distance( pOtherFixedConnector->GetOriginalPoint(), pCommonConnector->GetOriginalPoint() );

		r1 = pLink->GetActuatedConnectorDistance( pFixedConnection, pCommonConnector );
		r2 = pOtherToRotate->GetActuatedConnectorDistance( pOtherFixedConnector, pCommonConnector );
	
		CFCircle Circle1( pFixedConnection->GetTempPoint(), r1 );
		CFCircle Circle2( pOtherFixedConnector->GetTempPoint(), r2 );
	
		CFPoint Intersect;
		CFPoint Intersect2;
	
		if( !Circle1.CircleIntersection( Circle2, &Intersect, &Intersect2 ) )
			return false;
		
		// Try to give the point momentum by determining where it would be if 
		// it moved from a previous point through it's current point to some 
		// new point. Use that new point as the basis for selecting the new 
		// location.
	
		CFLine Line( pCommonConnector->GetPreviousPoint(), pCommonConnector->GetPoint() );
		Line.SetDistance( Line.GetDistance() * 2 );
		CFPoint SuggestedPoint = Line.GetEnd();
		
		double d1 = Distance( SuggestedPoint /*pCommonConnector->GetPoint()*/, Intersect );
		double d2 = Distance( SuggestedPoint /*pCommonConnector->GetPoint()*/, Intersect2 );
	
		if( d2 < d1 )
			Intersect = Intersect2;
		
		double Angle = GetAngle( pFixedConnection->GetTempPoint(), Intersect, pFixedConnection->GetOriginalPoint(), pCommonConnector->GetOriginalPoint() );
		Angle = GetClosestAngle( pFixedConnection->GetRotationAngle(), Angle );
		pFixedConnection->SetRotationAngle( Angle );
		if( !pLink->RotateAround( pFixedConnection ) )
			return false;
		pCommonConnector->SetTempFixed( false ); // needed so it can be rotated again.
		pLink->IncrementMoveCount( -1 ); // Don't count that one twice.
		Angle = GetAngle( pOtherFixedConnector->GetTempPoint(), Intersect, pOtherFixedConnector->GetOriginalPoint(), pCommonConnector->GetOriginalPoint() );
		Angle = GetClosestAngle( pOtherFixedConnector->GetRotationAngle(), Angle );
		pOtherFixedConnector->SetRotationAngle( Angle );
		if( !pOtherToRotate->RotateAround( pOtherFixedConnector ) )
			return false;
		
		return true;
	}
	#endif

	int GetLinkTriangles( CLink *pLink, CList<LinkList*> &Triangles )
	{
		// return a list of link triangles that contain the pLink parameter.
		int Count = 0;

		CConnector *pBaseConnector = pLink->GetFixedConnector();

		/*
		 * Examine every combination of two links that are connected to pLink. See if any of
		 * them connect to each other. If so, that is a link triangle.
		 */

		// get a list of all links connected to pLink.
		LinkList ConnectedLinks;
		POSITION Position = pLink->GetConnectorList()->GetHeadPosition();
		while( Position != 0 )
		{
			CConnector *pConnector = pLink->GetConnectorList()->GetNext( Position );
			if( pConnector == 0 || pConnector->GetLinkCount() <= 1 )
				continue;
			POSITION Position2 = pConnector->GetLinkList()->GetHeadPosition();
			while( Position2 != 0 )
			{
				CLink *pSaveLink = pConnector->GetLinkList()->GetNext( Position2 );
				if( pSaveLink == 0 || pSaveLink == pLink )
					continue;
				ConnectedLinks.AddTail( pSaveLink );
			}
		}

		// Look at each pair of links to see if they connect to each other.
		Position = ConnectedLinks.GetHeadPosition();
		while( Position != 0 )
		{
			CLink *pFirstLink = ConnectedLinks.GetNext( Position );
			if( pFirstLink == 0 || pFirstLink == pLink )
				continue;
			POSITION Position2 = Position;
			while( Position2 != 0 )
			{
				CLink *pSecondLink = ConnectedLinks.GetNext( Position2 );
				if( pSecondLink == 0 )
					continue;
				CConnector *pNewConnector = CLink::GetCommonConnector( pFirstLink, pSecondLink );
				CConnector *pTestConnector = CLink::GetCommonConnector( pLink, pFirstLink );
				if( pNewConnector != 0 && pNewConnector != pTestConnector )
				{
					if( pNewConnector->IsFixed() )
						continue;
					// Found a triangle!
					LinkList *pLinkList = new LinkList;
					pLinkList->AddTail( pLink );
					pLinkList->AddTail( pFirstLink );
					pLinkList->AddTail( pSecondLink );
					Triangles.AddTail( pLinkList );
					++Count;
				}
			}
		}

		return Count;
	}

	bool JoinToLink( LinkList *pLinkList, ConnectorList *pConnectors, CConnector *pFixedConnection, CConnector *pCommonConnector, CLink *pOtherToRotate )
	{
		/*
		 * Rotate the list of links and the other Link so that they are still
		 * connected. The link list has only one connection that is in a new
		 * temp location, pFixedConnection, and the other Link needs to have a
		 * fixed connector of some sort too (that is not an input!).
		 *
		 * The link list will not have proper temp locations for all
		 * connectors yet, only the fixed one. It is in an odd screwed up state
		 * at the moment but will be fixed shortly.
		 *
		 * Also adjust positions of connectors to account for linear actuators in the triangle.

		 * Also account for the other link being a link triangle! Oh crap, this is getting complicated!
		 */
	 
		CConnector *pOtherFixedConnector = pOtherToRotate->GetFixedConnector();
		if( pOtherFixedConnector == 0 )
		{
			if( pOtherToRotate->GetFixedConnectorCount() == 0 )
				return true; // Maybe it can be dealt with later in processing by another chain of Links.
			else
				return false;
		}
		if( pOtherFixedConnector->IsInput() )
			return false;
	
	//	CConnector* pCommonConnector = GetCommonConnector( this, pOtherToRotate );
	//	if( pCommonConnector == 0 )
	//		return false;

		// "this" link could be connected to something that can't move. Check to make
		// sure but don't otherwise move anything else. Check all Links connected to
		// this other than the pOtherToRotate Link and if any of them have fixed connectors,
		// return immediately.
		for( POSITION Position = pConnectors->GetHeadPosition(); Position != 0; )
		{
			CConnector* pTestConnector = pConnectors->GetNext( Position );
			if( pTestConnector == 0 || pTestConnector == pFixedConnection || pTestConnector == pFixedConnection )
				continue;
			for( POSITION Position2 = pTestConnector->GetLinkList()->GetHeadPosition(); Position2 != 0; )
			{
				CLink *pTestLink = pTestConnector->GetLinkList()->GetNext( Position2 );
				if( pTestLink == 0 || pTestLink == pOtherToRotate || pLinkList->Contains( pTestLink ) )
					continue;
				if( pTestLink->GetFixedConnectorCount() != 0 )
				{
					pCommonConnector->SetPositionValid( false );
					return false;
				}
			}
		}
		
		// pOtherToRotate could be connected to something that can't move. Check to make
		// sure but don't otherwise move anything else. Check all Links connected to
		// pOtherToRotate other than this Link and if any of them have fixed connectors,
		// return immediately.
		for( POSITION Position = pOtherToRotate->GetConnectorList()->GetHeadPosition(); Position != 0; )
		{
			CConnector* pTestConnector =  pOtherToRotate->GetConnectorList()->GetNext( Position );
			if( pTestConnector == 0 || pTestConnector == pOtherFixedConnector || pTestConnector == pFixedConnection )
				continue;
			for( POSITION Position2 = pTestConnector->GetLinkList()->GetHeadPosition(); Position2 != 0; )
			{
				CLink *pTestLink = pTestConnector->GetLinkList()->GetNext( Position2 );
				if( pTestLink == 0 || pTestLink == pOtherToRotate || pLinkList->Contains( pTestLink ) )
					continue;
				if( pTestLink->GetFixedConnectorCount() != 0 )
				{
					pCommonConnector->SetPositionValid( false );
					return false;
				}
			}
		}

		double r1 = Distance( pFixedConnection->GetOriginalPoint(), pCommonConnector->GetOriginalPoint() );
		double r2 = Distance( pOtherFixedConnector->GetOriginalPoint(), pCommonConnector->GetOriginalPoint() );

		// r1 = pLink->GetActuatedConnectorDistance( pFixedConnection, pCommonConnector );
		r1 = Distance( pFixedConnection->GetOriginalPoint(), pCommonConnector->GetOriginalPoint() );
		r2 = pOtherToRotate->GetActuatedConnectorDistance( pOtherFixedConnector, pCommonConnector );
	
		CFCircle Circle1( pFixedConnection->GetTempPoint(), r1 );
		CFCircle Circle2( pOtherFixedConnector->GetTempPoint(), r2 );
	
		CFPoint Intersect;
		CFPoint Intersect2;
	
		if( !Circle1.CircleIntersection( Circle2, &Intersect, &Intersect2 ) )
			return false;
		
		// Try to give the point momentum by determining where it would be if 
		// it moved from a previous point through it's current point to some 
		// new point. Use that new point as the basis for selecting the new 
		// location.
	
		CFLine Line( pCommonConnector->GetPreviousPoint(), pCommonConnector->GetPoint() );
		Line.SetDistance( Line.GetDistance() * 2 );
		CFPoint SuggestedPoint = Line.GetEnd();
		
		double d1 = Distance( SuggestedPoint /*pCommonConnector->GetPoint()*/, Intersect );
		double d2 = Distance( SuggestedPoint /*pCommonConnector->GetPoint()*/, Intersect2 );
	
		if( d2 < d1 )
			Intersect = Intersect2;
		
		double Angle = GetAngle( pFixedConnection->GetTempPoint(), Intersect, pFixedConnection->GetOriginalPoint(), pCommonConnector->GetOriginalPoint() );
		Angle = GetClosestAngle( pFixedConnection->GetRotationAngle(), Angle );
		pFixedConnection->SetRotationAngle( Angle );

		for( POSITION Position = pLinkList->GetHeadPosition(); Position != 0; )
		{ 
			CLink *pLink = pLinkList->GetNext( Position );
			if( pLink == 0 )
				continue;
			if( !pLink->RotateAround( pFixedConnection ) )
				return false;
			pLink->IncrementMoveCount( -1 ); // Don't count that one twice.
		}

		pCommonConnector->SetTempFixed( false ); // needed so it can be rotated again.
		Angle = GetAngle( pOtherFixedConnector->GetTempPoint(), Intersect, pOtherFixedConnector->GetOriginalPoint(), pCommonConnector->GetOriginalPoint() );
		Angle = GetClosestAngle( pOtherFixedConnector->GetRotationAngle(), Angle );
		pOtherFixedConnector->SetRotationAngle( Angle );
		if( !pOtherToRotate->RotateAround( pOtherFixedConnector ) )
			return false;
		
		return true;
	}

	int GetLinkTriangleConnectors( LinkList *pLinkList, ConnectorList &Connectors, int MaxFixedCount )
	{
		Connectors.RemoveAll();
		POSITION Position = pLinkList->GetHeadPosition();
		while( Position != 0 )
		{
			CLink *pLink = pLinkList->GetNext( Position );
			if( pLink == 0 )
				continue;
			POSITION Position2 = pLink->GetConnectorList()->GetHeadPosition();
			while( Position2 != 0 )
			{
				CConnector *pConnector = pLink->GetConnectorList()->GetNext( Position2 );
				if( pConnector == 0 )
					continue;
				POSITION Position3 = Connectors.GetHeadPosition();
				while( Position3 != 0 )
				{
					CConnector *pExistingConnector = Connectors.GetNext( Position3 );
					if( pConnector == pExistingConnector )
					{
						pConnector = 0;
						break;
					}
				}
				if( pConnector == 0 )
					continue;
				Connectors.AddTail( pConnector );
			}
		}
		return Connectors.GetCount();
	}

	bool FindLinkTriangleMatch( CLink *pLink )
	{
		/*
		 * A link triangle is a set of three links that can be treated like a single link.
		 * there is also the possibility of one of the links being an actuator, which is tricky,
		 * but the concept is the same. If the current link is not part of a link triangle,
		 * nothing else is done here.
		 * 
		 * Find another link that is connected to the current link triangle and can be
		 * used as a basis for moving the link and teh triangle into their new postitions.
		 *
		 * The possible configurations are (AND SOME MIGHT NOT BE HANDLED HERE BUT ARE LISTED FOR REFERNCE WHILE WRITING THE CODE)
		 *   A. Other link has a fixed connector and the triangle and link can be 
		 *      moved and rotated to connect to each other.
		 *   B. Other link has a sliding connector that is now fixed and the
		 *      current link can be moved and rotated to match the slider.
		 *   C. Other link has limits for one of the current link sliders and the
		 *      current link can be moved and rotated to match those limits.
		 *   !D. Other link has two sliders that are not fixed and it and the 
		 *      current link can both be moved and rotated to connect properly.
		 *   E. Other link can slide through two fixed sliders and the 
		 *      current both links can be moved and the current
		 *      link rotated to meet properly.
		 *   F. Found a link connected to this one. The other link has a slider
		 *		onto this link and the other link also slides on something. That
		 *		other link has at least three sliders and it needs to be slid into
		 *		place based on it's slider connection to this link.
		 *   G. connected to other link with a slider but it has no limits. The
		 *		other link should be able to rotate into position as if it was
		 *		not a link with a slider.
		 *
		 */

		CConnector *pBaseConnector = pLink->GetFixedConnector();

		CList<LinkList*> Triangles;
		int Count = GetLinkTriangles( pLink, Triangles );

		if( Count == 0 )
			return false;

		POSITION TrianglePosition = Triangles.GetHeadPosition();
		while( TrianglePosition != 0 )
		{
			LinkList *pLinks = Triangles.GetNext( TrianglePosition );
			if( pLinks == 0 )
				continue;
			ConnectorList Connectors;
			int ConnectorCount = GetLinkTriangleConnectors( pLinks, Connectors, 1 );

			if( ConnectorCount < 3 )
				continue;

			CConnector *pLimit1 = 0;	 
			CConnector *pLimit2 = 0;	 
			CConnector *pSlider1 = 0;	 
			CConnector *pSlider2 = 0;	 

			int SliderCount = 0;

			POSITION Position = Connectors.GetHeadPosition();
			while( Position != 0 )
			{
				CConnector* pConnector = Connectors.GetNext( Position );
				if( pConnector == 0 || pConnector->IsFixed() )
					continue;
			
				if( pConnector->IsSlider() )
				{
					// Got a slider. See if it is limited by fixed limit connectors
					// of some other link. Move and rotate to fit betwen the limits.
			
					CConnector *pLimit1;
					CConnector *pLimit2;
					if( !pConnector->GetSlideLimits( pLimit1, pLimit2 ) )
						continue;
				
					if( pLimit1->IsFixed() && pLimit2->IsFixed() )
					{
						if( pBaseConnector == 0 )
						{
//							if( pLink->CanOnlySlide() )
//							{
//								// Case F.
//								return JoinToSlideLink( pLink, 0, pConnector, pLink );
//							}
						}
						else
						{
							// Case C.
//							return SlideToLink( pLink, pBaseConnector, pConnector, pLimit1, pLimit2 );
						}

						// The slider is limited so don't do additional tests
						// below this.
						continue;
					}
				}

				// Check all Links connected to this connector until we find one 
				// that has a single fixed connector. Move and rotate both to
				// meet with each other properly. Check the connected links for
				// one that can slide but not rotate to meet the current link.
			
				POSITION Position2 = pConnector->GetLinkList()->GetHeadPosition();
				while( Position2 != 0 && pBaseConnector != 0 )
				{
					CLink *pOtherLink = pConnector->GetLinkList()->GetNext( Position2 );
					if( pOtherLink == 0 || pOtherLink == pLink )
						continue;
					
					CConnector *pOtherFixedConnector = pOtherLink->GetFixedConnector();
					if( pOtherFixedConnector != 0 && !pOtherFixedConnector->IsInput() && pBaseConnector != pOtherFixedConnector )
					{
						// Case A.
						return JoinToLink( pLinks, &Connectors, pBaseConnector, pConnector, pOtherLink );
					}
					
					if( pOtherLink->CanOnlySlide() )
					{
						// Case E.
//						return JoinToSlideLink( pLink, pBaseConnector, pConnector, pOtherLink );
					}
				}
			}
	
			// Look at the sliders whose limits are part of the current link. If
			// the slider is fixed then the current link can be moved and rotated
			// to meet it.
	
			//pBaseConnector = pLink->GetFixedConnector();
			//if( pBaseConnector != 0 )
			//{
				for( int Index = 0; Index < pLink->GetConnectedSliderCount(); ++Index )
				{
					pLink->GetConnectedSlider( Index );
					CConnector* pSlider = pLink->GetConnectedSlider( Index );
					if( pSlider == 0 )
						continue;
			
					if( pSlider->IsFixed() )
					{
						// Case B.
						// Got a fixed slider. Make the current link align with it.
//						return MoveToSlider( pLink, pBaseConnector, pSlider );
					}
				}	
			//}
		}

		if( Triangles.GetCount() > 0 )
		{
			POSITION Position = Triangles.GetHeadPosition();
			while( Position != 0 )
			{
				LinkList* pLinks = Triangles.GetNext( Position );
				if( pLinks != 0 )
					delete pLinks;
			}
		}

		return false;
	}

	bool FindLinksToMatch( CLink *pLink )
	{
		/*
		 * Find another link that is connected to the current link and can be
		 * used as a basis for moving the two links into their new postitions.
		 *
		 * The possible configurations are:
		 *   A. Other link has a fixed connector and the two links can be moved
		 *      and rotated to connect to each other.
		 *   B. Other link has a sliding connector that is now fixed and the
		 *      current link can be moved and rotated to match the slider.
		 *   C. Other link has limits for one of the current link sliders and the
		 *      current link can be moved and rotated to match those limits.
		 *   !D. Other link has two sliders that are not fixed and it and the 
		 *      current link can both be moved and rotated to connect properly.
		 *   E. Other link can slide through two fixed sliders and the 
		 *      current both links can be moved and the current
		 *      link rotated to meet properly.
		 *   F. Found a link connected to this one. The other link has a slider
		 *		onto this link and the other link also slides on something. That
		 *		other link has at least three sliders and it needs to be slid into
		 *		place based on it's slider connection to this link.
		 *   G. connected to other link with a slider but it has no limits. The
		 *		other link should be able to rotate into position as if it was
		 *		not a link with a slider.
		 *
		 */

		CConnector *pLimit1 = 0;	 
		CConnector *pLimit2 = 0;	 
		CConnector *pSlider1 = 0;	 
		CConnector *pSlider2 = 0;	 

		int SliderCount = 0;

		ConnectorList *pConnectors = pLink->GetConnectorList();

		CConnector *pBaseConnector = pLink->GetFixedConnector();
	
		POSITION Position = pConnectors->GetHeadPosition();
		while( Position != 0 )
		{
			CConnector* pConnector = pConnectors->GetNext( Position );
			if( pConnector == 0 || pConnector->IsFixed() )
				continue;
			
			if( pConnector->IsSlider() )
			{
				// Got a slider. See if it is limited by fixed limit connectors
				// of some other link. Move and rotate to fit betwen the limits.
			
				CConnector *pLimit1;
				CConnector *pLimit2;
				if( !pConnector->GetSlideLimits( pLimit1, pLimit2 ) )
					continue;
				
				if( pLimit1->IsFixed() && pLimit2->IsFixed() )
				{
					if( pBaseConnector == 0 )
					{
						if( pLink->CanOnlySlide() )
						{
							// Case F.
							return JoinToSlideLink( pLink, 0, pConnector, pLink );
						}
					}
					else
					{
						// Case C.
						return SlideToLink( pLink, pBaseConnector, pConnector, pLimit1, pLimit2 );
					}

					// The slider is limited so don't do additional tests
					// below this.
					continue;
				}
			}

			// Check all Links connected to this connector until we find one 
			// that has a single fixed connector. Move and rotate both to
			// meet with each other properly. Check the connected links for
			// one that can slide but not rotate to meet the current link.
			
			POSITION Position2 = pConnector->GetLinkList()->GetHeadPosition();
			while( Position2 != 0 && pBaseConnector != 0 )
			{
				CLink *pOtherLink = pConnector->GetLinkList()->GetNext( Position2 );
				if( pOtherLink == 0 || pOtherLink == pLink )
					continue;
					
				CConnector *pOtherFixedConnector = pOtherLink->GetFixedConnector();
				if( pOtherFixedConnector != 0 && !pOtherFixedConnector->IsInput() && pBaseConnector != pOtherFixedConnector )
				{
					// Case A.
					return JoinToLink( pLink, pBaseConnector, pConnector, pOtherLink );
				}
					
				if( pOtherLink->CanOnlySlide() )
				{
					// Case E.
					return JoinToSlideLink( pLink, pBaseConnector, pConnector, pOtherLink );
				}
			}
		}
	
		// Look at the sliders whose limits are part of the current link. If
		// the slider is fixed then the current link can be moved and rotated
		// to meet it.
	
		//pBaseConnector = pLink->GetFixedConnector();
		if( pBaseConnector != 0 )
		{
			for( int Index = 0; Index < pLink->GetConnectedSliderCount(); ++Index )
			{
				pLink->GetConnectedSlider( Index );
				CConnector* pSlider = pLink->GetConnectedSlider( Index );
				if( pSlider == 0 )
					continue;
			
				if( pSlider->IsFixed() )
				{
					// Case B.
					// Got a fixed slider. Make the current link align with it.
					return MoveToSlider( pLink, pBaseConnector, pSlider );
				}
			}	
		}

		return false;
	}

	bool FindSlideLinksToMatch( CLink *pLink, LinkList *pLinkList )
	{
		/*
		 * Look for another link that has a sliding connection to the current
		 * link and the current link can only slide.
		 */

		CConnector *pLimit1 = 0;	 
		CConnector *pLimit2 = 0;	 
		CConnector *pSlider1 = 0;	 
		CConnector *pSlider2 = 0;	 

		int SliderCount = 0;

		POSITION Position = pLinkList->GetHeadPosition();
		while( Position != 0 )
		{
			CLink *pOtherLink = pLinkList->GetNext( Position );
			if( pOtherLink == 0 || pOtherLink == pLink )
				continue;

			POSITION Position2 = pOtherLink->GetConnectorList()->GetHeadPosition();
			while( Position2 != 0 )
			{
				CConnector* pConnector = pOtherLink->GetConnectorList()->GetNext( Position2 );
				if( pConnector == 0 || !pConnector->IsFixed() || !pConnector->IsSlider() )
					continue;
			
				CConnector *pLimit1;
				CConnector *pLimit2;
				if( !pConnector->GetSlideLimits( pLimit1, pLimit2 ) )
					continue;

				if( !pLink->IsConnected( pLimit1 ) )
					continue;

				return SlideToSlider( pLink, pConnector, pLimit1, pLimit2 );
			}	
		}

		return false;
	}
};

CSimulator::CSimulator()
{
	m_pImplementation = new CSimulatorImplementation;
}

CSimulator::~CSimulator()
{
	if( m_pImplementation != 0 )
		delete m_pImplementation;
	m_pImplementation = 0;
}

bool CSimulator::Reset( void )
{
	if( m_pImplementation == 0 )
		return false;
	return m_pImplementation->Reset();
}

bool CSimulator::SimulateStep( CLinkageDoc *pDoc, int StepNumber, bool bAbsoluteStepNumber, int* pInputID, double *pInputPositions, 
                               int InputCount, bool bNoMultiStep )
{
	if( m_pImplementation == 0 )
		return false;
	return m_pImplementation->SimulateStep( pDoc, StepNumber, bAbsoluteStepNumber, pInputID, pInputPositions, InputCount, bNoMultiStep );
}

int CSimulator::GetSimulationSteps( CLinkageDoc *pDoc )
{
	if( m_pImplementation == 0 )
		return false;
	return m_pImplementation->GetSimulationSteps( pDoc );
}

bool CSimulator::IsSimulationValid( void )
{
	if( m_pImplementation == 0 )
		return false;
	return m_pImplementation->IsSimulationValid();
}
