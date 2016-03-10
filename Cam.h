#pragma once

#include "geometry.h"

struct _CamPathElement
{
	enum _type { LINE, ARC };

	enum _type m_Type;
	CFPoint m_Start;
	CFPoint m_End;
	CFPoint m_Center;
	// Arc is always clockwise from start to end.
};

class CCam
{	
	public:

	CCam( void );
	virtual ~CCam( void );

	virtual bool GetDefinitionValue( int Index, double &Value ) = 0;
	virtual bool SetDefinitionValue( int Index, double Value ) = 0;

	virtual void GetCamType( CString &TypeName ) = 0;

	static CCam* CreateCamObject( const char *pTypeName );

	double GetAngle( void ) { return m_RotationAngle; }
	void SetAngle( double Angle ) { m_RotationAngle = Angle; }

	struct _CamPathElement *GetDrawingPath( int &Count );

	protected:

	double m_RotationAngle;
	struct _CamPathElement *m_pDrawingPath;
	int m_DrawingPathCount;

	virtual void ComputeDrawingPath( void ) = 0;
};

class CSingleNoseCam : public CCam
{
	public:

	CSingleNoseCam( void );
	virtual ~CSingleNoseCam( void ) {}

	virtual void GetCamType( CString &TypeName )
	{
		TypeName = "singlenose";
	}

	virtual bool GetDefinitionValue( int Index, double &Value );
	virtual bool SetDefinitionValue( int Index, double Value );

	private:

	double m_BaseRadius;	// Radius of the base circle.
	double m_NoseLength;	// Nose from centerline to end of nose, not center of nose.
	double m_FlankRadius;	// Radius of large flank curve.
	double m_FlankAngle;	// Angle where flank curve leaves the base citrcle.
	double m_NoseRadius;	// Radius of the nose circle.

	virtual void ComputeDrawingPath( void );
};

class CBezierCam : public CCam
{
	public:

	CBezierCam( void );
	virtual ~CBezierCam( void ) {}

	virtual void GetCamType( CString &TypeName )
	{
		TypeName = "bezier";
	}

	virtual bool GetDefinitionValue( int Index, double &Value );
	virtual bool SetDefinitionValue( int Index, double Value );

	private:

	CFPoint *m_pPoints;		// Points for the poly-bezier path.
	int m_PointCount;

	virtual void ComputeDrawingPath( void );
};

