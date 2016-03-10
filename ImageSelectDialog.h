#pragma once


// CImageSelectDialog dialog

class CImageSelectDialog : public CDialog
{
	DECLARE_DYNAMIC(CImageSelectDialog)

public:
	CImageSelectDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CImageSelectDialog();

// Dialog Data
	enum { IDD = IDD_SELECT_IMAGE_OPTION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	
private:
	class CImageSelectDialogImplementation *m_pImpelementation;
	
public:
	virtual BOOL OnInitDialog();
};
