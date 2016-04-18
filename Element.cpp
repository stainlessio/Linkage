#include "stdafx.h"
#include "Element.h"

CElement::CElement(void)
{
	m_bSelected = false;
	m_Layers = 0;
	m_bMeasurementElement = false;
	m_bPositionValid = true;
}

CElement::~CElement(void)
{
}

CElement::CElement( const CElement &ExistingElement )
{
	m_Identifier = -1; // Should never copy and ID but some value needs to be set here.
	m_bSelected = ExistingElement.m_bSelected;
	m_Layers = ExistingElement.m_Layers;
	m_Name = ExistingElement.m_Name;
	m_bMeasurementElement = ExistingElement.m_bMeasurementElement;
	m_bPositionValid = ExistingElement.m_bPositionValid;
	m_Color = ExistingElement.m_Color;
	// The fastening is not copied because that seems just wrong - someother element can't be fastened to more than one element at a time.
}

void CElement::AddFastenConnector( class CConnector *pConnector )
{
	m_FastenedElements.AddTail( new CElementItem( pConnector ) );
}

void CElement::AddFastenLink( class CLink *pLink )
{
	m_FastenedElements.AddTail( new CElementItem( pLink ) );
}

void CElement::RemoveFastenElement( class CElement *pElement )
{
	POSITION Position = m_FastenedElements.GetHeadPosition();
	while( Position != 0 )
	{
		POSITION DeletePosition = Position;
		CElementItem *pItem = m_FastenedElements.GetNext( Position );
		if( pItem == 0  )
			continue;

		if( pItem->GetElement() == pElement || pElement == 0 )
		{
			m_FastenedElements.RemoveAt( DeletePosition );
			break;
		}
	}
}

void CElement::UnfastenTo( void )
{
	CElementItem *pItem = GetFastenedTo();
	if( pItem != 0 && pItem->GetElement() != 0 )
		pItem->GetElement()->RemoveFastenElement( this );
	m_pFastenedTo = CElementItem();
}

