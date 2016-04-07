#include "geometry.h"
#include "Renderer.h"
#include "connector.h"

CFPoint* GetHull( ConnectorList* pConnectors, int &ReturnedPointCount, bool bUseOriginalPoints = false );
void DrawHull( CRenderer* pDC, CFPoint Offset, double Scale, ConnectorList* pConnectors, bool bFill, COLORREF Color );
