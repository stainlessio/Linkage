#pragma once


// CPopupGallery

class CPopupGallery : public CWnd
{
	DECLARE_DYNAMIC(CPopupGallery)

public:
	CPopupGallery( int CommandID, int ImageStripResourceID, int ImageWidth = 0 );
	virtual ~CPopupGallery();

public:
	void SetColumns( int Columns = INT_MAX ); // INT_MAX means one row with as many columns as images.
	void SetRows( int Rows = INT_MAX ); // INT_MAX means one column with as many rows as images.
	void ShowGallery( CWnd *pParentWnd, int xScreen, int yScreen, bool bAddToExisting );
	void HideGallery( void );
	int GetSelection( void );
	void SetTooltip( int Index, const char *pString );
	void SetTooltip( int Index, UINT ResourceID );

protected:

	class CPopupGalleryImplementation *m_pImplementation;

	BOOL PreTranslateMessage( MSG* pMsg );

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
	afx_msg void OnNcPaint();
};


