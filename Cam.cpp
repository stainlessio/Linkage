#include "stdafx.h"
#include "Cam.h"

CCam::CCam( void )
{
	m_RotationAngle = 0;
	m_pDrawingPath = 0;
	m_DrawingPathCount = 0;
}

CCam* CCam::CreateCamObject( const char *pTypeName )
{
	CString TypeName = pTypeName;
	if( TypeName == "singlenose" )
		return new CSingleNoseCam;

	return 0;
}

struct _CamPathElement *CCam::GetDrawingPath( int &Count )
{
	if( m_pDrawingPath == 0 )
		ComputeDrawingPath();
	Count = m_DrawingPathCount;
	return m_pDrawingPath;
}

CCam::~CCam( void )
{
	if( m_pDrawingPath != 0 )
		delete [] m_pDrawingPath;
}

CSingleNoseCam::CSingleNoseCam( void )
{
	m_BaseRadius = 0;
	m_NoseLength = 0;
	m_FlankRadius = 0;
	m_FlankAngle = 0;
	m_NoseRadius = 0;
}

bool CSingleNoseCam::GetDefinitionValue( int Index, double &Value )
{
	switch( Index )
	{
		case 0: Value = m_BaseRadius; return true;
		case 1: Value = m_NoseLength; return true;
		case 2: Value = m_FlankRadius; return true;
		case 3: Value = m_FlankAngle; return true;
		case 4: Value = m_NoseRadius; return true;
		default: Value = 0.0; return false;
	}
}

bool CSingleNoseCam::SetDefinitionValue( int Index, double Value )
{
	switch( Index )
	{
		case 0: m_BaseRadius = Value; return true;
		case 1: m_NoseLength = Value; return true;
		case 2: m_FlankRadius = Value; return true;
		case 3: m_FlankAngle = Value; return true;
		case 4: m_NoseRadius = Value; return true;
		default: return false;
	}
}

void CSingleNoseCam::ComputeDrawingPath( void )
{
	m_pDrawingPath = 0;
	m_DrawingPathCount = 0;
}

CBezierCam::CBezierCam( void )
{
	m_pPoints = 0;
	m_PointCount = 0;
}

bool CBezierCam::GetDefinitionValue( int Index, double &Value )
{
	if( Index >= m_PointCount || m_pPoints == 0 )
		return false;

	// Callers access x and y values of the points as separate definition indexes.
	int ArrayIndex = Index / 2;

	if( Index & 0x01 )
		Value = m_pPoints[ArrayIndex].y;
	else
		Value = m_pPoints[ArrayIndex].x;
	return true;
}

bool CBezierCam::SetDefinitionValue( int Index, double Value )
{
	// m_pPoints is ALWAYS allocated at a size that is on a 10 entry boundary.
	// Count on this to know when to allocate a new buffer.

	static const int ARRAY_COUNT_ALIGNMENT = 10;

	int CurrentArrayCount = ( ( m_PointCount + 9 ) / 10 ) * 10;
	if( m_pPoints == 0 )
		CurrentArrayCount = 0;

	// Callers access x and y values of the points as separate definition indexes.
	int ArrayIndex = Index / 2;

	int NewArrayCount = max( CurrentArrayCount, ( ( ArrayIndex + 10 ) / 10 ) * 10 );

	if( NewArrayCount != CurrentArrayCount )
	{
		CFPoint *pNewPoints = new CFPoint[NewArrayCount];
		if( pNewPoints == 0 )
			return false;
		int Counter = 0;
		for( ; Counter < CurrentArrayCount; ++Counter )
			pNewPoints[Counter] = m_pPoints[Counter];

		delete [] m_pPoints;
		m_pPoints = pNewPoints;
	}

	if( Index & 0x01 )
		m_pPoints[ArrayIndex].y = Value;
	else
		m_pPoints[ArrayIndex].x = Value;

	return true;
}

void CBezierCam::ComputeDrawingPath( void )
{
	m_pDrawingPath = 0;
	m_DrawingPathCount = 0;
}
