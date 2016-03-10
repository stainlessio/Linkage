#include "geometry.h"
#include "Renderer.h"
#include "connector.h"

CFPoint* GetHull( ConnectorList* pConnectors, int &ReturnedPointCount );
void DrawHull( CRenderer* pDC, CFPoint Offset, double Scale, ConnectorList* pConnectors, bool bFill, COLORREF Color );
