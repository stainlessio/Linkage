#pragma once
#include "linkageDoc.h"

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

