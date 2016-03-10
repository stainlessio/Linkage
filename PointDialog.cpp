// RPMDialog.cpp : implementation file
//

#include "stdafx.h"
#include <float.h>
#include "Linkage.h"
#include "PointDialog.h"

void AFXAPI DDX_MyDoubleText( CDataExchange* pDX, int nIDC, double& value, int Precision );
void AFXAPI _MyAfxTextFloatFormat(CDataExchange* pDX, int nIDC,
                                  void* pData, double value, int nSizeGcvt,
                                  int nSizeType);

// CPointPropertiesDialog dialog

IMPLEMENT_DYNAMIC(CPointPropertiesDialog, CMyDialog)

CPointPropertiesDialog::CPointPropertiesDialog( CWnd* pParent /*=NULL*/ )
	: CMyDialog( pParent, CPointPropertiesDialog::IDD )
	, m_xPosition(0)
	, m_yPosition(0)
	, m_Name(_T(""))
	, m_bDrawCircle(FALSE)
	, m_Radius(0)
, m_FastenTo( _T( "" ) )
{
	m_Color = RGB( 200, 200, 200 );
}

CPointPropertiesDialog::~CPointPropertiesDialog()
{
}

void CPointPropertiesDialog::DoDataExchange(CDataExchange* pDX)
{
	CMyDialog::DoDataExchange( pDX );
	DDX_MyDoubleText( pDX, IDC_EDIT2, m_xPosition, 4 );
	DDX_MyDoubleText( pDX, IDC_EDIT4, m_yPosition, 4 );
	DDX_Text( pDX, IDC_EDIT3, m_Name );
	DDX_Control( pDX, IDC_CHECK1, m_DrawCircleControl );
	DDX_Check( pDX, IDC_CHECK1, m_bDrawCircle );
	DDX_Control( pDX, IDC_RADIUSPROMPT, m_RadiusPromptControl );
	DDX_Control( pDX, IDC_EDIT6, m_RadiusControl );
	DDX_MyDoubleText( pDX, IDC_EDIT6, m_Radius, 4 );

	OnBnClickedCheck1();
	DDX_Control( pDX, IDC_FASTENEDTO, m_FastenToControl );
	DDX_Text( pDX, IDC_FASTENEDTO, m_FastenTo );

	DDX_Control( pDX, IDC_COLOR, m_ColorControl );

	if( pDX->m_bSaveAndValidate )
		m_Color = m_ColorControl.GetColor();
	else
		m_ColorControl.SetColor( m_Color );
}


BEGIN_MESSAGE_MAP(CPointPropertiesDialog, CMyDialog)
	ON_BN_CLICKED(IDC_CHECK1, &CPointPropertiesDialog::OnBnClickedCheck1)
	ON_STN_CLICKED( IDC_COLOR, &CPointPropertiesDialog::OnStnClickedColor )
END_MESSAGE_MAP()


// CPointPropertiesDialog message handlers

BOOL CPointPropertiesDialog::OnInitDialog()
{
	CMyDialog::OnInitDialog();
	
	return TRUE;
}

static void AFXAPI DDX_MyDoubleText( CDataExchange* pDX, int nIDC, double& value, int Precision )
{
    _MyAfxTextFloatFormat(pDX, nIDC, &value, value, Precision, DBL_DIG);
}

static BOOL AFXAPI MyAfxSimpleFloatParse(LPCTSTR lpszText, double& d)
{
	ASSERT(lpszText != NULL);
	while (*lpszText == ' ' || *lpszText == '\t')
		lpszText++;

	TCHAR chFirst = lpszText[0];
	d = _tcstod(lpszText, (LPTSTR*)&lpszText);
	if (d == 0.0 && chFirst != '0')
		return FALSE;   // could not convert
	while (*lpszText == ' ' || *lpszText == '\t')
		lpszText++;

	if (*lpszText != '\0')
		return FALSE;   // not terminated properly

	return TRUE;
}

static void AFXAPI _MyAfxTextFloatFormat(CDataExchange* pDX, int nIDC,
                                  void* pData, double value, int nSizeGcvt,
                                  int nSizeType)
{
    ASSERT(pData != NULL);

    HWND hWndCtrl = pDX->PrepareEditCtrl(nIDC);

        // Make sure your buffer is big enough. Strings returned by
        // _stprintf() using the "f" specifier tend to be longer
        // than those returned using the "g" specifier.
    TCHAR szBuffer[64];

    if (pDX->m_bSaveAndValidate)
    {
        ::GetWindowText(hWndCtrl, szBuffer, _countof(szBuffer));
        double d;
        if (!MyAfxSimpleFloatParse(szBuffer, d))
        {
            AfxMessageBox(AFX_IDP_PARSE_REAL);
            pDX->Fail();            // throws exception
        }
        if (nSizeType == FLT_DIG)
            *((float*)pData) = (float)d;
        else
            *((double*)pData) = d;
    }
    else
    {
        sprintf_s( szBuffer, sizeof( szBuffer ), "%.*f", nSizeGcvt, value );
        AfxSetWindowText( hWndCtrl, szBuffer );
    }
}

void CPointPropertiesDialog::OnBnClickedCheck1()
{
	m_RadiusPromptControl.EnableWindow( m_DrawCircleControl.GetCheck() != 0 ? TRUE : FALSE );
	m_RadiusControl.EnableWindow( m_DrawCircleControl.GetCheck() != 0 ? TRUE : FALSE );
}

void CPointPropertiesDialog::OnStnClickedColor()
{
	CColorDialog dlg;
	if( dlg.DoModal() == IDOK )
	{
		m_Color = dlg.GetColor();
		m_ColorControl.SetColor( m_Color ); 
	}
}
