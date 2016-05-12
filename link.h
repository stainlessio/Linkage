#if !defined( _Linkage_LINK_H_ )
#define _Linkage_LINK_H_

#include "Element.h"
#include "connector.h"
#include <list>


class LinkList : public CList< class CLink*, class CLink* >
{
	public:
	
	LinkList() {}
	~LinkList() {}
	
	bool Remove( class CLink *pLink );
	bool Contains( CLink *pLink )
	{
		POSITION Position = GetHeadPosition();
		while( Position != 0 )
		{
			CLink *pTest = GetNext( Position );
			if( pTest == pLink )
				return true;
		}
		return false;
	}
};

class CGearConnection
{
	public:
	enum ConnectionType { GEARS, CHAIN };

	CGearConnection()
	{
		m_pGear1 = 0;
		m_pGear2 = 0;
		m_pDriveGear = 0;
		m_Gear1Size = 0.0;
		m_Gear2Size = 0.0;
		m_ConnectionType = GEARS;
		m_bUseSizeAsRadius = false;
	}

	CLink *m_pGear1;
	CLink *m_pGear2;
	CLink *m_pDriveGear;
	double m_Gear1Size;
	double m_Gear2Size;
	ConnectionType m_ConnectionType;
	double Ratio( void ) { return m_Gear2Size / m_Gear1Size; }
	bool m_bUseSizeAsRadius;
};

typedef CList< class CGearConnection*, class CGearConnection* > GearConnectionList;

class CLink : public CElement
{
	public:
	
	CLink();
	CLink( const CLink &ExistingLink );
	virtual ~CLink();

	virtual bool IsLink( void ) { return true; }
	virtual bool IsConnector( void ) { return false; }


	CString GetIdentifierString( bool bDebugging );
	ConnectorList* GetConnectorList( void ) { return &m_Connectors; }
	void AddConnector( class CConnector* pConnector );

	void SelectAllConnectors( bool bSelected );
	void SetTempFixed( bool bSet ) { m_bTempFixed = bSet; }
	void SetRotationAngle( double Value ) { m_TempRotationAngle = Value; ++m_MoveCount; }
	double GetRotationAngle( void ) { return m_RotationAngle; }
	double GetTempRotationAngle( void ) { return m_TempRotationAngle; }

	bool IsGearAnchored( void );

	void FixAllConnectors( void );
	void Reset( void );

	bool IsTempFixed( void ) { return m_bTempFixed; }
	bool IsFixed( void ) { return m_bTempFixed; }

	double GetLength( void );

	bool IsSelected( bool bIsSelectedConnector = true );
	bool IsAllSelected( void );
	bool IsAnySelected( void );
	bool IsConnected( CConnector* pConnector );
	int GetSelectedConnectorCount( void );
	void GetArea( const GearConnectionList &GearConnections, CFRect &Rect );
	void GetAdjustArea( const GearConnectionList &GearConnections, CFRect &Rect );
	void GetAveragePoint( const GearConnectionList &GearConnections, CFPoint &Point );
	bool PointOnLink( const GearConnectionList &GearConnections, CFPoint Point, double TestDistance, double SolidLinkExpandion );
	int GetConnectorCount( void ) { return m_Connectors.GetCount(); }
	int GetAnchorCount( void );
	bool CanOnlySlide( CConnector** pLimit1 = 0, CConnector** pLimit2 = 0, CConnector** pSlider1 = 0, CConnector** pSlider2 = 0, bool *pbSlidersOnLink = 0 );
	CFPoint *ComputeHull( int *Count = 0, bool bUseOriginalPoints = false );
	CFPoint *GetHull( int &Count, bool bUseOriginalPoints = false );

	void MakePermanent( void );

	double GetTempActuatorExtension( void ) { return m_TempActuatorExtension; }

	void InitializeForMove( void );
	void ResetMoveCount( void ) { m_MoveCount = 0; }
	int GetMoveCount( void ) { return m_MoveCount; }
	void IncrementMoveCount( int Increment = +1 ) { m_MoveCount += Increment; }
	
	int GetLineSize( void ) { return m_LineSize; }
	void SetLineSize( int Size ) { m_LineSize = Size <= 0 ? 1 : Size; }
	
	bool IsSolid( void ) { return m_bSolid; }
	void SetSolid( bool bSolid ) { m_bSolid = bSolid; }
	
	bool IsLocked( void ) { return m_bLocked; }
	void SetLocked( bool bLocked ) { m_bLocked = bLocked; }

	bool IsGear( void ) { return m_bGear; }
	void SetGear( bool bSet ) { m_bGear = bSet; }
	CConnector *GetGearConnector( void ) const { return m_bGear ? GetConnector( 0 ) : 0; }
	virtual bool GetGearRadii( const GearConnectionList &ConnectionList, std::list<double> &RadiusList ) const;
	bool GetGearsRadius( CGearConnection *pGearConnection, double &Radius1, double &Radius2 );
	double GetLargestGearRadius( const GearConnectionList &ConnectionList, CConnector **ppGearConnector );
	bool IsActuator( void ) { return m_bActuator; }
	void SetActuator( bool bActuator );
	void SetAlwaysManual( bool bSet ) { m_bAlwaysManual = bSet; }
	bool IsAlwaysManual( void ) { return m_bAlwaysManual; }
	double GetStroke( void ) { return m_ActuatorStroke; }
	bool GetStrokePoint( CFPoint &Point );
	void SetStroke( double Stroke );
	double GetCPM( void ) { return m_ActuatorCPM; }
	void SetCPM( double CPM ) { m_ActuatorCPM = CPM; }
	void IncrementExtension( double Increment );
	void SetExtension( double Value );
	double GetExtendedDistance( void );
	CConnector *GetStrokeConnector( int Index ) { Index = 0; if( m_bActuator ) return &m_StrokeConnector; else return 0; }
	void UpdateFromController( void );
	void UpdateController( void );
	CConnector *GetConnector( int Index ) const;
	int GetConnectedSliderCount( void ) { return m_ConnectedSliders.GetCount(); }
	CConnector *GetConnectedSlider( int Index );

	// GetActuatedConnectorDistance will return the distance between the original
	// locations of the connectors except for when one is being moved with 
	// an actuator in which case the actuator extension is added to the distance.
	double GetActuatedConnectorDistance( CConnector *pConnector1, CConnector* pConnector2 );

	void RemoveConnector( CConnector* pConnector );
	void RemoveAllConnectors( void ) { RemoveConnector( 0 ); }

	void SlideBetween( class CConnector *pSlider = 0, class CConnector *pConnector1 = 0, class CConnector *pConnector2 = 0 );

	int GetFixedConnectorCount( void );
	CConnector* GetFixedConnector( void );
	bool RotateAround( CConnector* pConnector );
	bool FixAll( void );
	static CConnector* GetCommonConnector( CLink *pLink1, CLink *pLink2 );

	private:
	
	ConnectorList m_Connectors;
	ConnectorList m_ConnectedSliders;	// Sliders that slide between other connectors on this link, not sliders that are part of this link. This is only used for the simulation and nothing else.
	
	bool m_bGear;
	bool m_bTempFixed;
	double m_RotationAngle;
	double m_TempRotationAngle;			
	int m_MoveCount;
	int m_LineSize;
	bool m_bNoRotateWithAnchor;
	bool m_bSolid;
	bool m_bActuator;
	double m_ActuatorStroke;
	double m_ActuatorCPM;
	double m_TempActuatorExtension;
	double m_ActuatorExtension;
	bool m_bAlwaysManual;
	CFPoint *m_pHull;
	int m_HullCount;
	bool m_bLocked;

	CConnector m_StrokeConnector;
	
	int GetInputConnectorCount( void );
};

#endif
