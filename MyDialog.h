#pragma once
#include "afxwin.h"


// CMyDialog dialog

class CMyDialog : public CDialog
{
	DECLARE_DYNAMIC(CMyDialog)

public:
	CMyDialog( CWnd* pParent, unsigned int ID );
	virtual ~CMyDialog();

	virtual BOOL OnInitDialog();

	CString m_Label;

protected:

	int m_DarkBottomSize;

	void AFXAPI DDX_MyDoubleText( CDataExchange* pDX, int nIDC, double& value, int Precision );
	BOOL AFXAPI MyAfxSimpleFloatParse(LPCTSTR lpszText, double& d);
	void AFXAPI MyAfxTextFloatFormat(CDataExchange* pDX, int nIDC, void* pData, double value, int nSizeGcvt, int nSizeType);

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	DECLARE_MESSAGE_MAP()

};
