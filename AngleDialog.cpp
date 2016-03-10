// AngleDialog.cpp : implementation file
//

#include "stdafx.h"
#include "Linkage.h"
#include "AngleDialog.h"
#include "afxdialogex.h"


// CAngleDialog dialog

IMPLEMENT_DYNAMIC(CAngleDialog, CMyDialog)

CAngleDialog::CAngleDialog(CWnd* pParent /*=NULL*/)
	: CMyDialog( pParent, CAngleDialog::IDD )
	, m_Angle(0)
{

}

CAngleDialog::~CAngleDialog()
{
}

void CAngleDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_AngleTextControl);

	if( pDX->m_bSaveAndValidate )
		DDX_Text(pDX, IDC_EDIT1, m_Angle);
	else
	{
		CString Text;
		Text.Format( "%.4lf", m_Angle );
		m_AngleTextControl.SetWindowTextA( Text );
	}
}


BEGIN_MESSAGE_MAP(CAngleDialog, CMyDialog)
END_MESSAGE_MAP()


// CAngleDialog message handlers
