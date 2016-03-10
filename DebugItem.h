#pragma once

#include "geometry.h"

class CDebugItem
{
	public:
	CDebugItem( CFPoint Point ) 
	{ 
		m_Type = DEBUG_OBJECT_POINT;
		m_Point = Point;
	}
	CDebugItem( CFLine Line ) 
	{ 
		m_Type = DEBUG_OBJECT_LINE;
		m_Line = Line;
	}
	CDebugItem( CFCircle Circle ) 
	{ 
		m_Type = DEBUG_OBJECT_CIRCLE;
		m_Circle = Circle;
	}
	CDebugItem( CFArc Arc ) 
	{ 
		m_Type = DEBUG_OBJECT_ARC;
		m_Arc = Arc;
	}
	~CDebugItem() {}

	enum _Type { DEBUG_OBJECT_POINT, DEBUG_OBJECT_LINE, DEBUG_OBJECT_CIRCLE, DEBUG_OBJECT_ARC } m_Type;
	CFPoint m_Point;
	CFLine m_Line;
	CFCircle m_Circle;
	CFArc m_Arc;
};

class CDebugItemList : public CList< class CDebugItem*, class CDebugItem* >
{
	public:
	
	CDebugItemList() {}
	~CDebugItemList() {}

	CDebugItem *Pop( void ) { return GetHeadPosition() == 0 ? 0 : RemoveHead(); }
	void Push( CDebugItem *pItem ) { AddHead( pItem ); }

	void Clear( void )
	{
		while( true )
		{
			CDebugItem *pDebugItem = Pop();
			if( pDebugItem == 0 )
				break;
			delete pDebugItem;
		}
	}
};


