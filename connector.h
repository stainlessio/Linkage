#if !defined( _Linkage_CONNECTOR_H_ )
#define _Linkage_CONNECTOR_H_

#include "Element.h"
#include "geometry.h"
//#include "Renderer.h"

class ConnectorListOperation
{
	public:
	virtual bool operator()( class CConnector *pConnector, bool bFirst, bool bLast ) { return true; } // Continue to iterate.
};

class ConnectorList : public CList< class CConnector*, class CConnector* >
{
	public:
	
	ConnectorList() {}
	~ConnectorList() {}
	
	bool Iterate( ConnectorListOperation &Operation );
	bool Remove( class CConnector *pConnector );
	class CConnector * Find( class CConnector *pConnector );
};

class CConnector : public CElement
{
public:
	CConnector();
	CConnector( const CConnector &ExistingConnector );
	virtual ~CConnector() {}

	virtual bool IsLink( void ) { return false; }
	virtual bool IsConnector( void ) { return true; }

	void Reset( void );

	void Reset( bool bClearMotionPath );
	void SetPoint( CFPoint& Point ) { m_Point = Point; m_OriginalPoint = Point; }
	void SetPoint( double x, double y ) { m_Point.x = x; m_Point.y = y; m_OriginalPoint = m_Point; }
	void SetIntermediatePoint( CFPoint &Point ) { m_Point = Point; }
	void MovePoint( CFPoint& Point ) { m_TempPoint = Point; }
	void MovePoint( double x, double y ) { m_TempPoint.x = x; m_TempPoint.y = y; }
	void MakePermanent( void );
	void SetRPM( double Value ) { m_RPM = Value; }
	void SetTempFixed( bool bSet ) { m_bTempFixed = bSet; }
	void SetRotationAngle( double Value ) { m_TempRotationAngle = Value; }
	void IncrementRotationAngle( double Change );
	void RotateAround( CFPoint& Point, double Angle );
	void SetAsDrawing( bool bSet ) { m_bDrawingConnector = bSet; }
	void SetAsInput( bool bSet ) { m_bInput = bSet; }
	void SetAsAnchor( bool bSet ) { m_bAnchor = bSet; }
	void SetAlwaysManual( bool bSet ) { m_bAlwaysManual = bSet; }
	void Select( bool bSelected );
	CList< class CLink*, class CLink* >* GetLinkList( void ) { return &m_Links; }
	void AddMotionPoint( void );
	
	void SetIntermediateDrawCircleRadius( double Radius ) { m_DrawCircleRadius = fabs( Radius ); }
	void SetDrawCircleRadius( double Radius ) { m_DrawCircleRadius = fabs( Radius ); m_OriginalDrawCircleRadius = fabs( Radius ); }
	double GetDrawCircleRadius( void ) { return m_DrawCircleRadius; }
	double GetOriginalDrawCircleRadius( void ) { return m_OriginalDrawCircleRadius; }
	
	void SetIntermediateSlideRadius( double Radius ) { m_SlideRadius = Radius; }
	void SetSlideRadius( double Radius ) { m_SlideRadius = Radius; m_OriginalSlideRadius = Radius; }
	double GetSlideRadius( void );
	double GetOriginalSlideRadius( void );

	bool GetSliderArc( CFArc &TheArc, bool bGetOriginal = false );

	bool IsLinkSelected( void );
	bool IsAnchor( void ) { return m_bAnchor; }
	bool IsInput( void ) { return m_bInput; }
	bool IsSlider( void ) { return m_pSlideLimits[0] != 0 && m_pSlideLimits[1] != 0; }
	bool IsDrawing( void ) { return m_bDrawingConnector; }
	bool IsTempFixed( void ) { return m_bTempFixed; }
	bool IsFixed( void ) { return m_bTempFixed | m_bAnchor | m_bInput; }
	bool IsAlwaysManual( void ) { return m_bAlwaysManual; }
	int GetSelectedLinkCount( void );
	bool GetSlideLimits( class CConnector *&pConnector1, class CConnector *&pConnector2 ) { pConnector1 = m_pSlideLimits[0]; pConnector2 = m_pSlideLimits[1]; return IsSlider(); }
	bool GetSlideLimits( CFPoint &Point1, CFPoint &Point2 );
	CLink* GetLink( int Index );
	CFPoint GetPoint( void ) { return m_Point; }
	CFPoint GetPreviousPoint( void ) { return m_PreviousPoint; }
	CFPoint GetTempPoint( void );
	CFPoint GetOriginalPoint( void ) { return m_OriginalPoint; }
	double GetRPM( void ) { return m_RPM; }
	CString GetIdentifierString( bool bDebugging );
	int GetLinkCount( void ) { return m_Links.GetCount(); }
	bool PointOnConnector( CFPoint Point, double TestDistance );
	double GetRotationAngle( void ) { return m_RotationAngle; }
	double GetTempRotationAngle( void ) { return m_TempRotationAngle; }
	void GetArea( CFRect &Rect );
	void GetAdjustArea( CFRect &Rect );
	CFPoint * GetMotionPath( int &StartPoint, int &PointCount, int &MaxPoint );
	bool HasLink( CLink *pLink );
	bool IsAlone( void );
	
	void SlideBetween( class CConnector *pConnector1 = 0, class CConnector *pConnector2 = 0 );
	
	CLink * GetSharingLink( CConnector *pOtherConnector );
	bool IsSharingLink( CConnector *pOtherConnector );
	
	void AddLink( class CLink* pLink );
	void RemoveAllLinks( void ) { RemoveLink( 0 ); }
	void RemoveLink( CLink *pLink );

private:
	CFPoint m_Point;
	CFPoint m_PreviousPoint;
	double m_RotationAngle; // In degrees, not radians!
	CFPoint m_TempPoint;
	double m_TempRotationAngle; // In degrees, not radians!
	CList< class CLink*, class CLink* > m_Links;
	class CConnector * m_pSlideLimits[2];
	CFPoint m_OriginalPoint;
	double m_RPM;
	bool m_bInput;
	bool m_bAnchor;
	bool m_bTempFixed;
	bool m_bDrawingConnector;
	bool m_bAlwaysManual;
	double m_DrawCircleRadius;
	double m_OriginalDrawCircleRadius;
	double m_SlideRadius;
	double m_OriginalSlideRadius;

	static const int MAX_DRAWING_POINTS = 600;
	CFPoint m_DrawingPoints[MAX_DRAWING_POINTS];
	int m_StartPoint;
	int m_PointCount;
	int m_DrawingPointCounter;
};

struct ConnectorDistance
{
	CConnector *m_pConnector;
	double m_Distance;
};

struct CompareConnectorDistance 
{
	bool operator ()( ConnectorDistance &lhs, ConnectorDistance &rhs) 
	{
		return lhs.m_Distance < rhs.m_Distance; 
	}
};

#endif

