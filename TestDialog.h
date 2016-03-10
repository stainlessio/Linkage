#pragma once


// CTestDialog dialog

class CTestDialog : public CDialog
{
	DECLARE_DYNAMIC(CTestDialog)

public:
	CTestDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTestDialog();

// Dialog Data
	enum { IDD = IDD_DIALOG1 };

	CMFCRibbonBar     m_wndRibbonBar;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
};
