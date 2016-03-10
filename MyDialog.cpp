#include "stdafx.h"
#include "mydialog.h"

static const int DARKBOTTOM = 41;

// CConnectorPropertiesDialog dialog

IMPLEMENT_DYNAMIC(CMyDialog, CDialog)

CMyDialog::CMyDialog( CWnd* pParent, unsigned int ID )
	: CDialog( ID, pParent )
{
}

CMyDialog::~CMyDialog()
{
}

BOOL CMyDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	if( !m_Label.IsEmpty() )
		SetWindowText( m_Label );
	
	return TRUE;
}

HBRUSH CMyDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	static CBrush Brush( RGB( 255, 255, 255 ) );
	static CBrush DarkBrush( RGB( 240, 240, 240 ) );

	CRect Rect;
	CRect ClientRect;
	GetClientRect( &ClientRect );
	pWnd->GetWindowRect( &Rect );
	ScreenToClient( &Rect );
	if( Rect.top >= ClientRect.bottom - DARKBOTTOM )
		return (HBRUSH)DarkBrush.GetSafeHandle();

	return (HBRUSH)Brush.GetSafeHandle();
}

BOOL CMyDialog::OnEraseBkgnd(CDC* pDC)
{
	CRect ClientRect;
	GetClientRect( &ClientRect );

	pDC->PatBlt( ClientRect.left, ClientRect.top, ClientRect.right, ClientRect.bottom - DARKBOTTOM, WHITENESS );

	CBrush Brush( RGB( 240, 240, 240 ) );
	CRect Rect( ClientRect.left, ClientRect.bottom - DARKBOTTOM, ClientRect.right, ClientRect.bottom );

	pDC->FillRect( &Rect, &Brush );

	CPen Pen( PS_SOLID, 1, RGB( 223, 223, 223 ) );
	CPen *pOldPen = pDC->SelectObject( &Pen );
	pDC->MoveTo( ClientRect.left, ClientRect.bottom - DARKBOTTOM );
	pDC->LineTo( ClientRect.right, ClientRect.bottom - DARKBOTTOM );
	pDC->SelectObject( pOldPen );

	return TRUE;
}

void AFXAPI CMyDialog::DDX_MyDoubleText( CDataExchange* pDX, int nIDC, double& value, int Precision )
{
    MyAfxTextFloatFormat(pDX, nIDC, &value, value, Precision, DBL_DIG);
}

BOOL AFXAPI CMyDialog::MyAfxSimpleFloatParse(LPCTSTR lpszText, double& d)
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

void AFXAPI CMyDialog::MyAfxTextFloatFormat(CDataExchange* pDX, int nIDC,
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


BEGIN_MESSAGE_MAP(CMyDialog, CDialog)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

