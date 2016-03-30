#pragma once


// CControlWindow

class CControlWindow : public CWnd
{
	DECLARE_DYNAMIC(CControlWindow)

public:
	CControlWindow();
	virtual ~CControlWindow();

	void Clear( void );
	void AddControl( int ControlID, const char *pDescription, int CommandID, bool bCentered = false, double InitialPosition = 0.0 );
	void UpdateControl( int ControlID, double Position );

	int GetDesiredHeight( void );

	double GetPosition( int ControlID ); // value from 0.0 to 1.0 with 1.0 being 100%.

	int GetControlCount( void );
	int GetControlId( int Index );
	double GetControlPosition( int Index );

private:
	class CControlWindowImplementation *m_pImplementation;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


