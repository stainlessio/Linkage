#include "stdafx.h"
#include "ColorStatic.h"

IMPLEMENT_DYNAMIC( CColorStatic, CStatic )

BEGIN_MESSAGE_MAP(CColorStatic, CStatic)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
END_MESSAGE_MAP()
