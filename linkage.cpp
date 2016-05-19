// Linkage.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "Linkage.h"

#include "MainFrm.h"
#include "geometry.h"
#include "connector.h"
#include "link.h"
#include "LinkageDoc.h"
#include "LinkageView.h"
#include "Winspool.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////
// Custome doc template to allow a user to open the same file that is already open.

class CMyDocTemplate : public CSingleDocTemplate
{
       DECLARE_DYNAMIC(CMyDocTemplate)

// Constructors
public:
    CMyDocTemplate(UINT nIDResource,  CRuntimeClass* pDocClass,
              CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass)
			  : CSingleDocTemplate(nIDResource, pDocClass, pFrameClass, pViewClass) {}
    virtual ~CMyDocTemplate() {}

// Overrides
public:
    CDocTemplate::Confidence MatchDocType(LPCTSTR _lpszPathName, CDocument*& _rpDocMatch)
	{
		Confidence con ;

		con = CDocTemplate::MatchDocType( _lpszPathName, _rpDocMatch) ;
		if( con == CDocTemplate::yesAlreadyOpen )
		{
			LPCTSTR lpszDot = ::PathFindExtension(_lpszPathName);
			CString DummyFileName = "__A_ZZZ___.___._____.__99999____ABCDEFGHIJK____._____..."; // No one will ever try using this name... I hope.
			if( lpszDot != NULL )
				DummyFileName += lpszDot;

			return CDocTemplate::MatchDocType( DummyFileName, _rpDocMatch) ;
		}

		return con;
	}
};

IMPLEMENT_DYNAMIC(CMyDocTemplate, CSingleDocTemplate)

/////////////////////////////////////////////////////////////////////////////
// CLinkageApp

BEGIN_MESSAGE_MAP(CLinkageApp, CWinAppEx)
	//{{AFX_MSG_MAP(CLinkageApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinAppEx::OnFileOpen)
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLinkageApp construction

CLinkageApp::CLinkageApp()
{
	m_bHiColorIcons = TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CLinkageApp object

CLinkageApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CLinkageApp initialization

BOOL CLinkageApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

    if (!AfxOleInit())
    {
        AfxMessageBox("Ole Initialization Failed");
        return FALSE;
    }

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("rectorsquid"));
	LoadStdProfileSettings( 8 );  // Load standard INI file options

	// Stop stupid MFC action to overwrite my accelerators with those in
	// a registery setting.
	WriteProfileString( "Workspace\\Keyboard-0" , "Accelerators", NULL );

	InitContextMenuManager();
	InitShellManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CMyDocTemplate* pDocTemplate;
	pDocTemplate = new CMyDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CLinkageDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CLinkageView));
	AddDocTemplate(pDocTemplate);

	// Enable DDE Execute open
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);

	#if defined( LINKAGE_USE_DIRECT2D )
		EnableD2DSupport();
	#endif

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;

	// Process the empt shell command to get a frame window before
	// tryimg to open an actual document.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// NBow parse the command line.
	ParseCommandLine(cmdInfo);

	// process the actual command line if it is not a "file new" command.

	if( cmdInfo.m_nShellCommand != CCommandLineInfo::FileNew )
	{
		// Dispatch commands specified on the command line.  Will return FALSE if
		// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
		if (!ProcessShellCommand(cmdInfo))
			return FALSE;
	}

	// The one and only window has been initialized, so show and update it
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand
	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();
	return TRUE;
}

void CLinkageApp::SetPrinterOrientation( int OrientationMode )
{
	// Get default printer settings.
	PRINTDLG   pd;

	pd.lStructSize = (DWORD) sizeof(PRINTDLG);
	if( GetPrinterDeviceDefaults(&pd) )
	{
		// Lock memory handle.
		DEVMODE FAR* pDevMode = (DEVMODE FAR*)::GlobalLock( m_hDevMode );
		LPDEVNAMES lpDevNames;
		LPTSTR lpszDriverName, lpszDeviceName, lpszPortName;
		HANDLE hPrinter;

		if( pDevMode )
		{
			pDevMode->dmOrientation = OrientationMode;

			// Unlock memory handle.
			lpDevNames = (LPDEVNAMES)GlobalLock(pd.hDevNames);
			lpszDriverName = (LPTSTR )lpDevNames + lpDevNames->wDriverOffset;
			lpszDeviceName = (LPTSTR )lpDevNames + lpDevNames->wDeviceOffset;
			lpszPortName   = (LPTSTR )lpDevNames + lpDevNames->wOutputOffset;

			OpenPrinter( lpszDeviceName, &hPrinter, NULL );
			DocumentProperties( NULL,hPrinter,lpszDeviceName,pDevMode,
							   pDevMode, DM_IN_BUFFER|DM_OUT_BUFFER );

			// Sync the pDevMode.
			// See SDK help for DocumentProperties for more info.
			ClosePrinter(hPrinter);
			::GlobalUnlock( m_hDevNames );
			::GlobalUnlock(m_hDevMode);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CLinkageApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CLinkageApp message handlers

void CLinkageApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
}

void CLinkageApp::LoadCustomState()
{
}

void CLinkageApp::SaveCustomState()
{
}
