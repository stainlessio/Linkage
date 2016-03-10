//////////////////////////////////////////////////////////////////////
// ColorCursor.h	: interface for the CColorCursor class.
// Author		    : Jiju George T (NeST)
//////////////////////////////////////////////////////////////////////

#if !defined(_COLORCURSOR_H)
#define _COLORCURSOR_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CColorCursor  
{
public:
	static void GetMaskBitmaps(HBITMAP hSourceBitmap,COLORREF clrTransparent,
							   HBITMAP &hAndMaskBitmap,HBITMAP &hXorMaskBitmap);
	static HCURSOR CreateCursorFromBitmap(HBITMAP hSourceBitmap,COLORREF clrTransparent,
										  DWORD   xHotspot,DWORD   yHotspot);
	CColorCursor();
	virtual ~CColorCursor();

};

#endif // !defined(_COLORCURSOR_H)
