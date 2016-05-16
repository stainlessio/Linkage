#pragma once
#include "linkageDoc.h"

#if 0
typedef enum  { None, Bind, Flop } SimulatorErrorType; 

class CSimulatorError
{
	public:
	CSimulatorError( CConnector *pBadConnector, SimulatorErrorType Error )
	{
		m_BadConectors.AddTail( pBadConnector );
		m_Error = Error;
	}
	CSimulatorError( CLink *pBadLink, SimulatorErrorType Error )
	{
		m_BadLinks.AddTail( pBadLink );
		m_Error = Error;
	}
	ConnectorList m_BadConectors;
	LinkList m_BadLinks;
	SimulatorErrorType m_Error;
};

#endif

class CSimulator
{
public:
	CSimulator();
	~CSimulator();

	bool SimulateStep( CLinkageDoc *pDoc, int StepNumber, bool bAbsoluteStepNumber, int* pInputID, double *pInputPositions, int InputCount, bool bNoMultiStep );
	bool Reset( void );
	int GetSimulationSteps( CLinkageDoc *pDoc );
	bool IsSimulationValid( void );

private:
	class CSimulatorImplementation *m_pImplementation;
};

