#pragma once

#include "NullableColor.h"

class CElementItem
{
	public:

	CElementItem() { m_ElementType = NONE; pElement = 0; }
	CElementItem( class CLink *pLink ) { m_ElementType = LINK; pElement = (class CElement*)pLink; }
	CElementItem( class CConnector *pConnector ) { m_ElementType = CONNECTOR; pElement = (class CElement*)pConnector; }

	class CLink *GetLink( void ) { return m_ElementType == LINK ? (CLink*)pElement : 0; }
	class CConnector *GetConnector( void ) { return m_ElementType == CONNECTOR ? (CConnector*)pElement : 0; }
	class CElement *GetElement( void ) { return pElement; }

	private:

	enum { CONNECTOR, LINK, NONE } m_ElementType;
	class CElement *pElement;
};

class ElementList : public CList< class CElementItem*, class CElementItem* >
{
	public:
	
	ElementList() {}
	~ElementList() {}
	
	bool Remove( class CElement *pElement )
	{
		POSITION Position = GetHeadPosition();
		while( Position != 0 )
		{
			POSITION RemovePosition = Position;
			CElementItem* pCheckItem = GetNext( Position );
			if( pCheckItem == 0 )
				continue;
			
			if( pCheckItem->GetElement() == pElement )
			{
				RemoveAt( RemovePosition );
				return true;
			}
		}
		return false;
	}
};

class CElement
{
	public:

	CElement(void);
	CElement( const CElement &ExistingElement );
	virtual ~CElement(void);

	void SetName( const char *pName ) { m_Name = pName; }
	CString &GetName( void ) { return m_Name; }

	bool IsSelected( void ) { return m_bSelected; }
	void Select( bool bSelected ) { m_bSelected = bSelected; }

	void SetIdentifier( int Value ) { m_Identifier = Value; }
	int GetIdentifier( void ) { return m_Identifier; }
	virtual CString GetIdentifierString( bool bDebugging ) = 0;

	void SetLayers( unsigned int Layers ) { m_Layers = Layers; }
	unsigned int GetLayers( void ) { return m_Layers; }
	bool IsOnLayers( int Layers ) { return ( m_Layers & Layers ) != 0; }
	void SetMeasurementElement( bool bSet ) { m_bMeasurementElement = bSet; }
	bool IsMeasurementElement( void ) { return m_bMeasurementElement; }

	void SetPositionValid( bool bSet ) { m_bPositionValid = bSet; }
	bool IsPositionValid( void ) { return m_bPositionValid; }

	CNullableColor GetColor( void ) { return m_Color; }
	void SetColor( CNullableColor Color ) { m_Color = Color; }

	CElementItem * GetFastenedTo( void ) { return m_pFastenedTo.GetElement() == 0 ? 0 : &m_pFastenedTo; }
	CLink * GetFastenedToLink( void ) { return m_pFastenedTo.GetLink() == 0 ? 0 : m_pFastenedTo.GetLink(); }
	CConnector * GetFastenedToConnector( void ) { return m_pFastenedTo.GetConnector() == 0 ? 0 : m_pFastenedTo.GetConnector(); }
	void UnfastenTo( void );
	void FastenTo( CLink *pFastenLink ) { m_pFastenedTo = pFastenLink; }
	void FastenTo( CConnector *pFastenConnector ) { m_pFastenedTo = pFastenConnector; }

	ElementList* GetFastenedElementList( void ) { return &m_FastenedElements; }
	void AddFastenConnector( class CConnector *pConnector );
	void AddFastenLink( class CLink *pLink );
	void RemoveFastenElement( class CElement *pElement );

	protected:

	int m_Identifier;
	bool m_bSelected;
	unsigned int m_Layers;
	CString m_Name;
	bool m_bMeasurementElement;
	bool m_bPositionValid; // True until there is some sort of binding problem
	CNullableColor m_Color;
	CElementItem m_pFastenedTo;
	ElementList m_FastenedElements;
};

