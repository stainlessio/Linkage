#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "MyDialog.h"


// CLinkPropertiesDialog dialog

class CRecordDialog : public CMyDialog
{
	DECLARE_DYNAMIC(CRecordDialog)

public:
	CRecordDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CRecordDialog();

// Dialog Data
	enum { IDD = IDD_START_RECORD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_EncoderName;
	CComboBox m_EncoderComboBox;
	CComboBox m_QualityControl;
	int m_QualitySelection;
	BOOL m_bUseStartTime;
	BOOL m_bUseRecordTime;
	BOOL m_bUseEndTime;
	CString m_StartTime;
	CString m_RecordTime;
	CString m_EndTime;
	afx_msg void OnBnClickedButton1();
};
