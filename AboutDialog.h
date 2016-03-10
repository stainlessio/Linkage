#pragma once

#include "mydialog.h"
#include "afxwin.h"

// CAboutDialog dialog

class CAboutDialog : public CMyDialog
{
	DECLARE_DYNAMIC(CAboutDialog)

public:
	CAboutDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAboutDialog();

// Dialog Data
	enum { IDD = IDD_ABOUT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	CFont m_BigFont;
public:
	CStatic m_TitleControl;
	CStatic m_VersionControl;
};
