#include "stdafx.h"
#include "dimension.h"


CDimension::CDimension(void)
{
	m_Offset = 0;
	m_ConnectorCount = 0;
}

CDimension::CDimension( CConnector *pConnector1, CConnector *pConnector2, CConnector *pConnector3 )
{
	m_pConnectors[0] = pConnector1;
	m_pConnectors[1] = pConnector2;
	if( pConnector3 == 0 )
		m_ConnectorCount = 2;
	else
	{
		m_pConnectors[2] = pConnector3;
		m_ConnectorCount = 3;
	}
}

CDimension::~CDimension(void)
{
}

bool CDimension::Contains( CConnector *pConnector )
{
	for( int Counter = 0; Counter < m_ConnectorCount; ++Counter )
	{
		if( m_pConnectors[Counter] == pConnector )
			return true;
	}
	return false;
}

