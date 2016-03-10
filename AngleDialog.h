#pragma once

#include "afxwin.h"
#include "MyDialog.h"

// CAngleDialog dialog

class CAngleDialog : public CMyDialog
{
	DECLARE_DYNAMIC(CAngleDialog)

public:
	CAngleDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAngleDialog();

// Dialog Data
	enum { IDD = IDD_ANGLE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	double m_Angle;
	CEdit m_AngleTextControl;
};
