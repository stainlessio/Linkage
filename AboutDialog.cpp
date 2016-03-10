// AboutDialog.cpp : implementation file
//

#include "stdafx.h"
#include "Linkage.h"
#include "AboutDialog.h"
#include "afxdialogex.h"


// CAboutDialog dialog

IMPLEMENT_DYNAMIC(CAboutDialog, CMyDialog)

CAboutDialog::CAboutDialog(CWnd* pParent /*=NULL*/)
	: CMyDialog( pParent, CAboutDialog::IDD )
{
	m_BigFont.CreateFont( -12, 0, 0, 0, FW_EXTRABOLD, 0, 0, 0, 0, 0, 0, ANTIALIASED_QUALITY, 0, "arial" );
}

CAboutDialog::~CAboutDialog()
{
}

static CString GetVersionInfo( void )
{
	CString Result = "0.0.0.0";

	HMODULE hLib = AfxGetResourceHandle();
  
	HRSRC hVersion = FindResource( hLib, MAKEINTRESOURCE( VS_VERSION_INFO ), RT_VERSION );
	if( hVersion == 0 )
		return Result;

	HGLOBAL hGlobal = LoadResource( hLib, hVersion ); 
	if ( hGlobal == 0 )
		return Result;
		
	LPVOID versionInfo  = LockResource( hGlobal );  
	if( versionInfo == 0 )
		return Result;

	UINT uLen;
	VS_FIXEDFILEINFO *lpFfi;

	VerQueryValue( versionInfo , "\\", (LPVOID*)&lpFfi , &uLen );

	DWORD dwFileVersionMS = lpFfi->dwFileVersionMS;
	DWORD dwFileVersionLS = lpFfi->dwFileVersionLS;

	UnlockResource( hGlobal );  
	FreeResource( hGlobal );  

	DWORD dwLeftMost     = HIWORD(dwFileVersionMS);
	DWORD dwSecondLeft   = LOWORD(dwFileVersionMS);
	DWORD dwSecondRight  = HIWORD(dwFileVersionLS);
	DWORD dwRightMost    = LOWORD(dwFileVersionLS);

	if( dwSecondRight > 0 )
		Result.Format( "%d.%d.%d" , dwLeftMost, dwSecondLeft, dwSecondRight, dwRightMost);
	else
		Result.Format( "%d.%d" , dwLeftMost, dwSecondLeft, dwSecondRight, dwRightMost);

	return Result;
}

void CAboutDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TITLE, m_TitleControl);
	DDX_Control(pDX, IDC_VERSION, m_VersionControl);
	m_TitleControl.SetFont( &m_BigFont );

	CString VersionNumber = GetVersionInfo();
	CString Version = "Version " + VersionNumber;
	m_VersionControl.SetWindowText( Version );

}

BEGIN_MESSAGE_MAP(CAboutDialog, CMyDialog)
END_MESSAGE_MAP()


