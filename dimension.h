#pragma once

#include "connector.h"

class CDimension
{
	public:

	CDimension(void);
	CDimension( CConnector *pConnector1, CConnector *pConnector2, CConnector *pConnector3 = 0 );
	~CDimension(void);

	bool Contains( CConnector *pConnector );

	void SetOffset( double Offset ) { m_Offset = Offset; }
	double GetOffset( void ) { return m_Offset; }

	private:
	int m_ConnectorCount;
	static const int MAX_CONNECTORS = 3;
	CConnector *m_pConnectors[MAX_CONNECTORS];
	double m_Offset;
};

