#include "stdafx.h"
#include "geometry.h"
#include "link.h"
#include "connector.h"
#include "Renderer.h"

#define X	0
#define Y	1

#define DELETEFLAGCOORD 	-9999999999

CFPoint* pSortPoints; // Need a global for accessing in the sort function

int Graham( CFPoint* pPoints, CFPoint* pStack, int Count );
int Squash( CFPoint* pPoints, int Count );
int Compare( const void *tp1, const void *tp2 );
void MoveLowest( CFPoint* pPoints, int Count );

int AreaSign( CFPoint a, CFPoint b, CFPoint c );
bool Left( CFPoint a, CFPoint b, CFPoint c );

CFPoint* GetHull( ConnectorList* pConnectors, int &ReturnedPointCount, bool bUseOriginalPoints )
{
	POSITION Position;
	int Counter;
	int Count = pConnectors->GetCount();
	CFPoint* PointArray = new CFPoint[Count];
	if( PointArray == NULL )
		return 0;

	Position = pConnectors->GetHeadPosition();
	for( Counter = 0; Position != NULL && Counter < Count; Counter++ )
	{
		CConnector* pConnector = pConnectors->GetNext( Position );
		if( bUseOriginalPoints )
		{
			PointArray[Counter].x = pConnector->GetOriginalPoint().x;
			PointArray[Counter].y = pConnector->GetOriginalPoint().y;
		}
		else
		{
			PointArray[Counter].x = pConnector->GetPoint().x;
			PointArray[Counter].y = pConnector->GetPoint().y;
		}
	}

	/*
	 * With two or less points, there will be no points that are not part
	 * of the hull. Return the points as-is in this case. In the case of
	 * three points, they still need to be sorted to be in the expected
	 * direction relative to each other.
	 */

	if( Count <= 2 )
	{
		ReturnedPointCount = Count;
		return PointArray;
	}

	MoveLowest( PointArray, Count );

	pSortPoints = PointArray;

	qsort(
	  &PointArray[1],		/* pointer to 1st elem */
	  Count-1,					/* number of elems */
	  sizeof( CFPoint ),	/* size of each elem */
	  Compare				/* -1,0,+1 compare function */
	);

	Count = Squash( PointArray, Count );

	if( Count <= 1 )
	{
		ReturnedPointCount = Count;
		return PointArray;
	}

	CFPoint* PointStack = new CFPoint[Count];
	if( PointStack == NULL )
	{
		delete [] PointArray;
		return 0;
	}

	ReturnedPointCount = Graham( PointArray, PointStack, Count );

	delete [] PointArray;
	PointArray = 0;

	return PointStack;
}

/*---------------------------------------------------------------------
FindLowest finds the rightmost lowest point and swaps with 0-th.
The lowest point has the min y-coord, and amongst those, the
max x-coord: so it is rightmost among the lowest.
---------------------------------------------------------------------*/
void MoveLowest( CFPoint* pPoints, int Count )
	{
	int Counter;
	int Lowest = 0;
	CFPoint Temp;

	for( Counter = 1; Counter < Count; Counter++ )
		{
		if( (pPoints[Counter].y < pPoints[Lowest].y) ||
			( (pPoints[Counter].y == pPoints[Lowest].y) && (pPoints[Counter].x > pPoints[Lowest].x) ) )
			Lowest = Counter;
		}
	Temp = pPoints[0];
	pPoints[0] = pPoints[Lowest];
	pPoints[Lowest] = Temp;
	}

/*---------------------------------------------------------------------
Compare: returns -1,0,+1 if p1 < p2, =, or > respectively;
here "<" means smaller angle.  Follows the conventions of qsort.
---------------------------------------------------------------------*/
int Compare( const void* tpi, const void* tpj )
{
	int a;			  /* area */
	double x, y;		  /* projections of ri & rj in 1st quadrant */
	CFPoint* pi;
	CFPoint* pj;
	pi = (CFPoint*)tpi;
	pj = (CFPoint*)tpj;

	a = AreaSign( pSortPoints[0], *pi, *pj );
	if( a > 0 )
		return -1;
	else if( a < 0 )
		return 1;
	else
		{ /* Collinear with P[0] */
		x = fabs( pi->x -	pSortPoints[0].x ) - fabs( pj->x - pSortPoints[0].x );
		y = fabs( pi->y -	pSortPoints[0].y ) - fabs( pj->y - pSortPoints[0].y );

		if( (x < 0) || (y < 0) )
			{
			pi->x = DELETEFLAGCOORD;
			pi->y = DELETEFLAGCOORD;
			return -1;
			}
		else if( (x > 0) || (y > 0) )
			{
			pj->x = DELETEFLAGCOORD;
			pj->y = DELETEFLAGCOORD;
			return 1;
			}
		else
			{ /* points are coincident */
			pj->x = DELETEFLAGCOORD;
			pj->y = DELETEFLAGCOORD;
			return 0;
			}
		}
	}

int Graham( CFPoint* pPoints, CFPoint* pStack, int Count )
	{
	int Counter;
	int Top;

	// Initialize stack.
	// Note that top is the index of the top point, not the next location after the top point
	Top = -1;
	pStack[Top+1] = pPoints[0];
	Top++;
	pStack[Top+1] = pPoints[1];
	Top++;

	// Bottom two Links will never be removed.
	Counter = 2;

	while( Counter < Count )
		{
		if( Left( pStack[Top-1], pStack[Top], pPoints[Counter] ) )
			{
			pStack[Top+1] = pPoints[Counter];
			Top++;
			Counter++;
			}
		else
			Top--;
		}

	return Top + 1;
	}

/*---------------------------------------------------------------------
Squash removes all Links from P marked delete.
---------------------------------------------------------------------*/
int Squash( CFPoint* pPoints, int Count )
	{
	int Counter = 0;
	int j = 0;
	for( j = 0, Counter = 0; Counter < Count; )
		{
		if( pPoints[Counter].x != DELETEFLAGCOORD )
			{
			pPoints[j].x = pPoints[Counter].x;
			pPoints[j].y = pPoints[Counter].y;
			j++;
			}
		Counter++;
		}
	return j;
	}

/*---------------------------------------------------------------------
Returns twice the signed area of the triangle determined by a,b,c.
The area is positive if a,b,c are oriented ccw, negative if cw,
and zero if the points are collinear.
---------------------------------------------------------------------*/
double Area2( CFPoint a, CFPoint b, CFPoint c )
	{
	return (b.x - a.x) * (c.y - a.y) - (c.x - a.x) * (b.y - a.y);
	}

/*---------------------------------------------------------------------
Returns true iff c is strictly to the left of the directed
line through a to b.
---------------------------------------------------------------------*/
bool Left( CFPoint a, CFPoint b, CFPoint c )
	{
	return  Area2( a, b, c ) > 0;
	}

int AreaSign( CFPoint a, CFPoint b, CFPoint c )
	{
	double area2;

	area2 = ( b.x - a.x ) * ( c.y - a.y ) - ( c.x - a.x ) * ( b.y - a.y );

	/* The area should be an integer. */
	if		( area2 >  0.000000001 ) return  1;
	else if ( area2 < -0.000000001 ) return -1;
	else					 return  0;
	}

