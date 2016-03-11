#include "stdafx.h"
#include "Linkage.h"
#include "io.h"
#include "MainFrm.h"
#include "mymfcribbongallery.h"
#include "examples_xml.h"
#include "samplegallery.h"
#include "AboutDialog.h"
#include "LinkageDoc.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_OFF_2007_AQUA, &CMainFrame::OnApplicationLook)
	ON_COMMAND(ID_FILE_PRINT, &CMainFrame::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CMainFrame::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CMainFrame::OnFilePrintPreview)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_PREVIEW, &CMainFrame::OnUpdateFilePrintPreview)
	ON_COMMAND(ID_HELP_USERGUIDE, &CMainFrame::OnHelpUserguide)
	ON_COMMAND(ID_HELP_ABOUT, &CMainFrame::OnHelpAbout)
	ON_COMMAND(ID_RIBBON_SAMPLE_GALLERY, &CMainFrame::OnSelectSample)
	ON_WM_DROPFILES()
END_MESSAGE_MAP()

// CMainFrame construction/destruction

class CValidatedString : public CString
{
	public: bool LoadString( UINT nID ) 
	{
		BOOL bNameValid = CString::LoadString( nID ); 
		ASSERT( bNameValid );
		return bNameValid != 0;
	}
};

CMainFrame::CMainFrame()
{
	m_pSampleGallery = 0;
	
	// TODO: add member initialization code here
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_OFF_2007_SILVER);
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	DragAcceptFiles( TRUE );

	// set the visual manager and style based on persisted value
	OnApplicationLook( theApp.m_nAppLook );

	m_wndRibbonBar.Create(this);
	InitializeRibbon();

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	EnableDocking( CBRS_ALIGN_ANY );

	m_wndStatusBar.AddElement( new CMFCRibbonStatusBarPane( ID_STATUSBAR_PANE1, "", TRUE, 0, "This is a sample of the type on this machine. This is filler to ensure that the pane is large enough for my text!" ), "" );

	// enable Visual Studio 2005 style docking window behavior
	CDockingManager::SetDockingMode(DT_SMART);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

static CMFCRibbonPanel* AddPanel( CMFCRibbonCategory *pCategory, int NameID, HICON hIcon = (HICON)0 )
{
	CValidatedString strTemp;
	if( NameID > 0 )
		strTemp.LoadString( NameID );
	return pCategory->AddPanel( strTemp, hIcon );
}

enum _ButtonDisplayMode { LARGE, LARGEONLY, SMALL, TEXT };

static void AddRibbonButton( CMFCRibbonPanel *pPanel, int NameID, UINT CommandID, int ImageOffset = -1, enum _ButtonDisplayMode Mode = TEXT )
{
	CValidatedString strTemp;
	if( NameID > 0 )
		strTemp.LoadString( NameID );
	pPanel->Add( new CMFCRibbonButton( CommandID, strTemp, Mode == TEXT || Mode == LARGEONLY ? -1 : ImageOffset, Mode == TEXT || Mode == SMALL ? -1 : ImageOffset) );
}

static void AddRibbonCheckbox( CMFCRibbonPanel *pPanel, int NameID, UINT CommandID )
{
	CString strTemp;
	if( NameID > 0 )
		strTemp.LoadString( NameID );
	pPanel->Add( new CMFCRibbonCheckBox( CommandID, strTemp ) );
}

static CMFCRibbonComboBox *AddRibbonCombobox( CMFCRibbonPanel *pPanel, int NameID, UINT CommandID, int Image )
{
	CString strTemp;
	if( NameID > 0 )
		strTemp.LoadString( NameID );
	CMFCRibbonComboBox *pNewCombobox = new CMFCRibbonComboBox( CommandID, 0, -1, strTemp, Image );
	pNewCombobox->EnableDropDownListResize( FALSE );
	pNewCombobox->SetWidth( 90 );
	pPanel->Add( pNewCombobox );
	return pNewCombobox;
}

/*static CMFCRibbonGallery* AddRibbonSampleGallery( CMFCRibbonCategory* pCategory, HICON hIcon = 0 )
{
	CMFCRibbonPanel* pPanelSamples = AddPanel( pCategory, IDS_RIBBON_OPENSAMPLE, hIcon );

	CString strTemp;
	strTemp.LoadString( IDS_RIBBON_SAMPLE_GALLERY );
	CMFCRibbonGallery *pSampleGallery = new CMFCRibbonGallery( ID_RIBBON_SAMPLE_GALLERY, strTemp, -1, 41, IDB_SAMPLE_GALLERY, 96 );
	pSampleGallery->SetIconsInRow( 6 );
	pSampleGallery->SetButtonMode();
	pSampleGallery->SelectElement( -1 );
	pPanelSamples->Add( pSampleGallery );

	CSampleGallery GalleryData;

	int Count = GalleryData.GetCount();
	for( int Counter = 0; Counter < Count; ++Counter )
	{
		CValidatedString strTemp;
		strTemp.LoadString( GalleryData.GetStringID( Counter ) );
		int EOL = strTemp.Find( '\n' );
		if( EOL >= 0 )
			strTemp.Truncate( EOL );
		pSampleGallery->SetItemToolTip( Counter, strTemp );
	}

	return pSampleGallery;
}*/

static void AddPrintButton( CMFCRibbonPanel *pPanel )
{
	CValidatedString strTemp;
	strTemp.LoadString(IDS_RIBBON_PRINT);
	CMFCRibbonButton* pBtnPrint = new CMFCRibbonButton(ID_FILE_PRINT, strTemp, 6, 6);
	pBtnPrint->SetKeys(_T("p"), _T("w"));
	strTemp.LoadString(IDS_RIBBON_PRINT_LABEL);
	pBtnPrint->AddSubItem(new CMFCRibbonLabel(strTemp));
	strTemp.LoadString(IDS_RIBBON_PRINT_QUICK);
	pBtnPrint->AddSubItem(new CMFCRibbonButton(ID_FILE_PRINT_DIRECT, strTemp, 7, 7, TRUE));
	strTemp.LoadString(IDS_RIBBON_PRINT_PREVIEW);
	pBtnPrint->AddSubItem(new CMFCRibbonButton(ID_FILE_PRINT_PREVIEW, strTemp, 8, 8, TRUE));
	strTemp.LoadString(IDS_RIBBON_PRINT_SETUP);
	pBtnPrint->AddSubItem(new CMFCRibbonButton(ID_FILE_PRINT_SETUP, strTemp, 11, 11, TRUE));
	pPanel->Add(pBtnPrint);
}

static void AddExportButton( CMFCRibbonPanel *pPanel )
{
	CValidatedString strTemp;
	strTemp.LoadString(IDS_RIBBON_EXPORT);

	static CMenu Dummy;

	CMFCRibbonButton* pButtonExport = new CMFCRibbonButton( 0, strTemp, 76, 76 );

	Dummy.CreatePopupMenu();
	pButtonExport->SetMenu( Dummy.GetSafeHmenu() );

	//AddRibbonButton( pMainPanel, IDS_RIBBON_SAVEVIDEO, ID_FILE_SAVEVIDEO, 5, LARGE );
	//AddRibbonButton( pMainPanel, IDS_RIBBON_SAVEIMAGE, ID_FILE_SAVEIMAGE, 37, LARGE );


	strTemp.LoadString(IDS_RIBBON_EXPORT_LABEL);
	pButtonExport->AddSubItem(new CMFCRibbonLabel(strTemp));

	strTemp.LoadString(IDS_RIBBON_SAVEVIDEO);
	pButtonExport->AddSubItem(new CMFCRibbonButton(ID_FILE_SAVEVIDEO, strTemp, 5, 5, TRUE));
	strTemp.LoadString(IDS_RIBBON_SAVEIMAGE);
	pButtonExport->AddSubItem(new CMFCRibbonButton(ID_FILE_SAVEIMAGE, strTemp, 37, 37, TRUE));
	strTemp.LoadString(IDS_RIBBON_SAVEDXF);
	pButtonExport->AddSubItem(new CMFCRibbonButton(ID_FILE_SAVEDXF, strTemp, 77, 77, TRUE));
	strTemp.LoadString(IDS_RIBBON_SAVEMOTION);
	pButtonExport->AddSubItem(new CMFCRibbonButton( ID_FILE_SAVEMOTION, strTemp, 83, 83, TRUE));
	pPanel->Add(pButtonExport);
}

//void CMainFrame::CreateSamplePanel( CMFCRibbonCategory* pCategory )
//{
//	m_pSampleGallery = AddRibbonSampleGallery( pCategory, m_PanelImages.ExtractIcon(29) );
//}

void CMainFrame::CreateClipboardPanel( CMFCRibbonCategory* pCategory )
{
	CMFCRibbonPanel* pPanelClipboard = AddPanel( pCategory, IDS_RIBBON_CLIPBOARD, m_PanelImages.ExtractIcon(30) );

	AddRibbonButton( pPanelClipboard, IDS_RIBBON_PASTE, ID_EDIT_PASTE, 14, LARGE );
	AddRibbonButton( pPanelClipboard, IDS_RIBBON_CUT, ID_EDIT_CUT, 21, SMALL );
	AddRibbonButton( pPanelClipboard, IDS_RIBBON_COPY, ID_EDIT_COPY, 22, SMALL );
	AddRibbonButton( pPanelClipboard, IDS_RIBBON_SELECTALL, ID_EDIT_SELECT_ALL, 44, SMALL );
}

void CMainFrame::CreateViewPanel( CMFCRibbonCategory* pCategory )
{
	CMFCRibbonPanel* pPanelView = AddPanel( pCategory, IDS_RIBBON_VIEW, m_PanelImages.ExtractIcon(31) );
	AddRibbonButton( pPanelView, IDS_RIBBON_ZOOMFIT, ID_VIEW_ZOOMFIT, 20, SMALL );
	AddRibbonButton( pPanelView, IDS_RIBBON_VIEW_ZOOMIN, ID_VIEW_ZOOMIN, 16, SMALL );
	AddRibbonButton( pPanelView, IDS_RIBBON_VIEW_ZOOMOUT, ID_VIEW_ZOOMOUT, 17, SMALL );
	
	AddRibbonCheckbox( pPanelView, IDS_RIBBON_VIEWDRAWING, ID_VIEW_DRAWING );
	AddRibbonCheckbox( pPanelView, IDS_RIBBON_VIEWMECHANISM, ID_VIEW_MECHANISM );

	//AddRibbonCheckbox( pPanelView, IDS_RIBBON_LABELS, ID_VIEW_LABELS );
	//AddRibbonCheckbox( pPanelView, IDS_RIBBON_ANGLES, ID_VIEW_ANGLES );
	//AddRibbonCheckbox( pPanelView, IDS_RIBBON_VIEW_ANICROP, ID_VIEW_ANICROP );
	//AddRibbonCheckbox( pPanelView, IDS_RIBBON_DIMENSIONS, ID_VIEW_DIMENSIONS );


	CMFCRibbonButton* pDetailsButton = new CMFCRibbonButton( ID_ALIGN_DETAILSBUTTON, "Details", 69 );

	AppendMenuItem( pDetailsButton, IDS_RIBBON_LABELS, ID_VIEW_LABELS, 10 );
	AppendMenuItem( pDetailsButton, IDS_RIBBON_ANGLES, ID_VIEW_ANGLES, 12 );
	AppendMenuItem( pDetailsButton, IDS_RIBBON_VIEW_ANICROP, ID_VIEW_ANICROP, 13 );
	AppendMenuItem( pDetailsButton, IDS_RIBBON_DIMENSIONS, ID_VIEW_DIMENSIONS, 15 );
	AppendMenuItem( pDetailsButton, IDS_RIBBON_VIEW_LARGEFONT, ID_VIEW_LARGEFONT, 78 );
	AppendMenuItem( pDetailsButton, IDS_RIBBON_SOLIDLINKS, ID_VIEW_SOLIDLINKS, 75 );
	AppendMenuItem( pDetailsButton, IDS_RIBBON_VIEW_GRID, ID_VIEW_GRID, 85 );
//	AppendMenuItem( pDetailsButton, IDS_RIBBON_VIEW_PARTS, ID_VIEW_PARTS, 86 );
	AppendMenuItem( pDetailsButton, IDS_RIBBON_VIEW_DEBUG, ID_VIEW_DEBUG, 81 );

	// The menu is only needed in order to make the button work properly.
	// Without the menu, clicking on the icon in the button will not display
	// the menu but clicking the text will.
	static CMenu Menu;
	Menu.CreatePopupMenu();
	pDetailsButton->SetMenu( Menu.GetSafeHmenu() );

	pPanelView->Add( pDetailsButton );
}

void CMainFrame::CreateDimensionsPanel( CMFCRibbonCategory* pCategory )
{
	CMFCRibbonPanel* pPanelDimensions = AddPanel( pCategory, IDS_RIBBON_DIM, m_PanelImages.ExtractIcon(32) );
	CMFCRibbonComboBox *pUnitsBox = AddRibbonCombobox( pPanelDimensions, 0, ID_VIEW_UNITS, 52 );
	if( pUnitsBox != 0 )
	{
		pUnitsBox->AddItem( CLinkageDoc::GetUnitsString( CLinkageDoc::MM ), CLinkageDoc::MM );
		pUnitsBox->AddItem( CLinkageDoc::GetUnitsString( CLinkageDoc::INCH ), CLinkageDoc::INCH );
	}

	CMFCRibbonEdit *pEdit = new CMFCRibbonEdit( ID_VIEW_COORDINATES, 96, 0, 53 );
	pEdit->SetWidth( 106 );
	pPanelDimensions->Add( pEdit );

	AddRibbonButton( pPanelDimensions, IDS_RIBBON_SET_RATIO, ID_EDIT_SET_RATIO, 79, SMALL );


}

void CMainFrame::CreatePrintPanel( CMFCRibbonCategory* pCategory )
{
	CMFCRibbonPanel* pPanePrint = AddPanel( pCategory, IDS_RIBBON_PRINT, m_PanelImages.ExtractIcon(33) );

	AddRibbonButton( pPanePrint, IDS_RIBBON_PRINT_QUICK, ID_FILE_PRINT_DIRECT, 7, LARGE );
	AddRibbonButton( pPanePrint, IDS_RIBBON_PRINT_PREVIEW, ID_FILE_PRINT_PREVIEW, 8, LARGE );
}

void CMainFrame::CreateSettingsPanel( CMFCRibbonCategory* pCategory )
{
	CMFCRibbonPanel* pPaneSettings = AddPanel( pCategory, IDS_RIBBON_SETTINGS, m_PanelImages.ExtractIcon(33) );

//	AddRibbonCheckbox( pPaneSettings, IDS_RIBBON_LABELS, ID_VIEW_LABELS );
//	AddRibbonCheckbox( pPaneSettings, IDS_RIBBON_ANGLES, ID_VIEW_ANGLES );
//	AddRibbonCheckbox( pPaneSettings, IDS_RIBBON_VIEW_ANICROP, ID_VIEW_ANICROP );
//	AddRibbonCheckbox( pPaneSettings, IDS_RIBBON_DIMENSIONS, ID_VIEW_DIMENSIONS );
	AddRibbonCheckbox( pPaneSettings, IDS_RIBBON_SOLIDLINKS, ID_VIEW_SOLIDLINKS );
}

void CMainFrame::CreatePrintOptionsPanel( CMFCRibbonCategory* pCategory )
{
	CMFCRibbonPanel* pPanePrintOptions = AddPanel( pCategory, IDS_RIBBON_PRINTOPTIONS, m_PanelImages.ExtractIcon(33) );

	AddRibbonButton( pPanePrintOptions, IDS_RIBBON_PRINT_SETUP, ID_FILE_PRINT_SETUP, 11, LARGE );
	AddRibbonCheckbox( pPanePrintOptions, IDS_RIBBON_PRINTFULL, ID_FILE_PRINTFULL );
}



void CMainFrame::CreateInsertPanel( CMFCRibbonCategory* pCategory )
{
	CMFCRibbonPanel* pPaneInsert = AddPanel( pCategory, IDS_RIBBON_INSERT, m_PanelImages.ExtractIcon(33) );

	CValidatedString strTemp;
	strTemp.LoadString( IDS_RIBBON_INSERTBUTTON );

	CMFCRibbonButton* pInsertButton = new CMFCRibbonButton( ID_EDIT_INSERTBUTTON, strTemp, 28, 28 );

	AppendMenuItem( pInsertButton, IDS_RIBBON_INSERT_CONNECTOR, ID_INSERT_CONNECTOR, 29 );
	AppendMenuItem( pInsertButton, IDS_RIBBON_INSERT_ANCHOR, ID_INSERT_ANCHOR, 27 );
	AppendMenuItem( pInsertButton, IDS_RIBBON_INSERT_LINK2, ID_INSERT_LINK2, 28 );
	AppendMenuItem( pInsertButton, IDS_RIBBON_INSERT_ANCHORLINK, ID_INSERT_ANCHORLINK, 27 );
	AppendMenuItem( pInsertButton, IDS_RIBBON_INSERT_INPUT, ID_INSERT_INPUT, 26 );
	AppendMenuItem( pInsertButton, IDS_RIBBON_INSERT_ACTUATOR, ID_INSERT_ACTUATOR, 49 );
	AppendMenuItem( pInsertButton, IDS_RIBBON_INSERT_LINK3, ID_INSERT_LINK3, 31 );
	AppendMenuItem( pInsertButton, IDS_RIBBON_INSERT_LINK4, ID_INSERT_LINK4, 43 );

	AppendMenuItem( pInsertButton, IDS_RIBBON_INSERT_POINT, ID_INSERT_POINT, 42 );
	AppendMenuItem( pInsertButton, IDS_RIBBON_INSERT_LINE, ID_INSERT_LINE, 45 );
	AppendMenuItem( pInsertButton, IDS_RIBBON_INSERTMEASUREMENT, ID_INSERT_MEASUREMENT, 74 );
	AppendMenuItem( pInsertButton, IDS_RIBBON_INSERTGEAR, ID_INSERT_GEAR, 82 );

	// The menu is only needed in order to make the button work properly.
	// Without the menu, clicking on the icon in the button will not display
	// the menu but clicking tyhe text will.
	static CMenu Menu;
	Menu.CreatePopupMenu();
	pInsertButton->SetMenu( Menu.GetSafeHmenu() );

	pInsertButton->SetDescription( "" );

	pPaneInsert->Add( pInsertButton );
}

void CMainFrame::CreateElementPanel( CMFCRibbonCategory* pCategory )
{
	CMFCRibbonPanel* pPaneAction = AddPanel( pCategory, IDS_RIBBON_ACTION, m_PanelImages.ExtractIcon(34) );
	AddRibbonButton( pPaneAction, IDS_RIBBON_ADDCONNECTOR, ID_EDIT_ADDCONNECTOR, 73, SMALL );
	AddRibbonButton( pPaneAction, IDS_RIBBON_CONNECT, ID_EDIT_CONNECT, 32, SMALL );
	AddRibbonButton( pPaneAction, IDS_RIBBON_JOIN, ID_EDIT_JOIN, 34, SMALL );
	AddRibbonButton( pPaneAction, IDS_RIBBON_COMBINE, ID_EDIT_COMBINE, 35, SMALL );
	AddRibbonButton( pPaneAction, IDS_RIBBON_SLIDE, ID_EDIT_SLIDE, 36, SMALL );
	AddRibbonButton( pPaneAction, IDS_RIBBON_SPLIT, ID_EDIT_SPLIT, 38, SMALL );
	AddRibbonButton( pPaneAction, IDS_RIBBON_FASTEN, ID_EDIT_FASTEN, 33, SMALL );
	AddRibbonButton( pPaneAction, IDS_RIBBON_UNFASTEN, ID_EDIT_UNFASTEN, 80, SMALL );
	AddRibbonButton( pPaneAction, IDS_RIBBON_LOCK, ID_EDIT_LOCK, 84, SMALL );
	AddRibbonButton( pPaneAction, IDS_RIBBON_PROPERTIES_PROPERTIES, ID_PROPERTIES_PROPERTIES, 4, LARGE );
}

void CMainFrame::AppendMenuItem( CMenu &Menu, UINT StringID, UINT ID, int BitmapIndex )
{
	CValidatedString strTemp;
	strTemp.LoadStringA( StringID );
	Menu.AppendMenu( MF_STRING, ID, strTemp );
}

void CMainFrame::AppendMenuItem( CMFCRibbonButton *pButton, UINT StringID, UINT ID, int BitmapIndex )
{
	CValidatedString strTemp;
	strTemp.LoadString( StringID );

	pButton->AddSubItem( new CMFCRibbonButton( ID, strTemp, BitmapIndex ) );
}

class MyMenuCheck : public CMFCRibbonCheckBox
{
	public:
	MyMenuCheck( UINT ID, const char *pText ) : CMFCRibbonCheckBox( ID, pText ) {}
	
	virtual void OnDraw( CDC* pDC )
	{
		m_szMargin;
		CMFCRibbonCheckBox::OnDraw( pDC );
	}

};

void CMainFrame::AppendMenuCheck( CMFCRibbonButton *pButton, UINT StringID, UINT ID )
{
	CValidatedString strTemp;
	strTemp.LoadString( StringID );

	CMFCRibbonCheckBox * pTemp = new MyMenuCheck( ID, strTemp );

	pButton->AddSubItem( pTemp );
}

void CMainFrame::CreateAlignPanel( CMFCRibbonCategory* pCategory )
{
	CMFCRibbonPanel* pPaneAlign = AddPanel( pCategory, IDS_RIBBON_ALIGN, m_PanelImages.ExtractIcon(35) );

	AddRibbonButton( pPaneAlign, IDS_RIBBON_UNDO, ID_EDIT_UNDO, 19, LARGE );


	AddRibbonCheckbox( pPaneAlign, IDS_RIBBON_EDITDRAWING, ID_EDIT_DRAWING );
	AddRibbonCheckbox( pPaneAlign, IDS_RIBBON_EDITMECHANISM, ID_EDIT_MECHANISM );

	CMFCRibbonButton* pSnapButton = new CMFCRibbonButton( ID_ALIGN_SNAPBUTTON, "Snap   ", 68 );

	AppendMenuItem( pSnapButton, IDS_RIBBON_SNAP, ID_EDIT_SNAP, 47 );
	AppendMenuItem( pSnapButton, IDS_RIBBON_GRIDSNAP, ID_EDIT_GRIDSNAP, 46 );
	AppendMenuItem( pSnapButton, IDS_RIBBON_AUTOJOIN, ID_EDIT_AUTOJOIN, 48 );

	// The menu is only needed in order to make the button work properly.
	// Without the menu, clicking on the icon in the button will not display
	// the menu but clicking the text will.
	static CMenu Menu;
	Menu.CreatePopupMenu();
	pSnapButton->SetMenu( Menu.GetSafeHmenu() );

	pPaneAlign->Add( pSnapButton );

	CValidatedString strTemp;
	strTemp.LoadString( IDS_RIBBON_ALIGNBUTTON );

	CMFCRibbonButton* pAlignButton = new CMFCRibbonButton( ID_ALIGN_ALIGNBUTTON, strTemp, 58, 58 );

	AppendMenuItem( pAlignButton, IDS_RIBBON_SETANGLE, ID_ALIGN_SETANGLE, 66 );
	AppendMenuItem( pAlignButton, IDS_RIBBON_RIGHTANGLE, ID_ALIGN_RIGHTANGLE, 54 );
	AppendMenuItem( pAlignButton, IDS_RIBBON_RECTANGLE, ID_ALIGN_RECTANGLE, 55 );
	AppendMenuItem( pAlignButton, IDS_RIBBON_PARALLELOGRAM, ID_ALIGN_PARALLELOGRAM, 56 );
	AppendMenuItem( pAlignButton, IDS_RIBBON_HORIZONTAL, ID_ALIGN_HORIZONTAL, 57 );
	AppendMenuItem( pAlignButton, IDS_RIBBON_VERTICAL, ID_ALIGN_VERTICAL, 58 );
	AppendMenuItem( pAlignButton, IDS_RIBBON_LINEUP, ID_ALIGN_LINEUP, 59 );
	AppendMenuItem( pAlignButton, IDS_RIBBON_EVENSPACE, ID_ALIGN_EVENSPACE, 65 );
	AppendMenuItem( pAlignButton, IDS_RIBBON_FLIPH, ID_ALIGN_FLIPH, 60 );
	AppendMenuItem( pAlignButton, IDS_RIBBON_FLIPV, ID_ALIGN_FLIPV, 61 );

	// The menu is only needed in order to make the button work properly.
	// Without the menu, clicking on the icon in the button will not display
	// the menu but clicking the text will.
	static CMenu Menu2;
	Menu2.CreatePopupMenu();
	pAlignButton->SetMenu( Menu2.GetSafeHmenu() );

	pPaneAlign->Add( pAlignButton );
}

class MyCMFCRibbonButtonsGroup : public CMFCRibbonButtonsGroup
{
	public:

	MyCMFCRibbonButtonsGroup( int yOffset = 0 ) : CMFCRibbonButtonsGroup() 
	{
		m_SpecialOffsetForMe = yOffset;
	}

	virtual BOOL IsAlignByColumn() const { return TRUE; }

	int m_SpecialOffsetForMe;

	virtual void OnAfterChangeRect( CDC* pDC )
	{
		/*
		 * After calling the parent class function, adjust the rectangles
		 * of the buttons to get them to display better. This is a kludge
		 * and there may be code in the ribbon classes that doesn't deal
		 * wwell with this adjustment. A parent window might not know the
		 * right location of these buttons after this change it made.
		 */

		CMFCRibbonButtonsGroup::OnAfterChangeRect( pDC );

		for (int i = 0; i < m_arButtons.GetSize(); i++)
		{
			CMFCRibbonBaseElement* pButton = m_arButtons [i];
			ASSERT_VALID(pButton);

			CRect rect = pButton->GetRect();

			if (rect.IsRectEmpty())
				continue;

			rect.OffsetRect( 0, m_SpecialOffsetForMe );

			pButton->SetRect( rect );
		}

		// The overall rect for this button group needs to get changed too.
		CRect rect;
		rect = GetRect();
		rect.OffsetRect( 0, m_SpecialOffsetForMe );
		SetRect( rect );
	}
};

void CMainFrame::CreateSimulationPanel( CMFCRibbonCategory* pCategory )
{
	CMFCRibbonPanel* pPanelMechanism = AddPanel( pCategory, IDS_RIBBON_SIMULATION, m_PanelImages.ExtractIcon(36) );
	AddRibbonButton( pPanelMechanism, IDS_RIBBON_RUN, ID_SIMULATION_RUN, 23, LARGE );
	AddRibbonButton( pPanelMechanism, IDS_RIBBON_STOP, ID_SIMULATION_STOP, 24, LARGE );
	AddRibbonButton( pPanelMechanism, IDS_RIBBON_PIN, ID_SIMULATION_PIN, 63, LARGE );
	
	MyCMFCRibbonButtonsGroup *pGroup = new MyCMFCRibbonButtonsGroup( 6 );
	pGroup->AddButton( new CMFCRibbonButton( ID_SIMULATION_SIMULATE, "", 25, -1 ) );
	pGroup->AddButton( new CMFCRibbonButton( ID_SIMULATE_INTERACTIVE, "", 50, -1 ) );
	pGroup->AddButton( new CMFCRibbonButton( ID_SIMULATE_MANUAL, "", 51, -1 ) );
	pPanelMechanism->Add( pGroup );

	MyCMFCRibbonButtonsGroup *pGroup2 = new MyCMFCRibbonButtonsGroup( 10 );
	pGroup2->AddButton( new CMFCRibbonButton( ID_SIMULATE_BACKWARD, "", 71, -1 ) );
	pGroup2->AddButton( new CMFCRibbonButton( ID_SIMULATION_PAUSE, "", 70, -1 ) );
	pGroup2->AddButton( new CMFCRibbonButton( ID_SIMULATE_FORWARD, "", 72, -1 ) );
	pPanelMechanism->Add( pGroup2 );
}

void CMainFrame::CreateMainCategory( void )
{
	CValidatedString strTemp;
	m_MainButton.SetImage( IDB_MAIN );
	m_MainButton.SetText( "File\nf" );
	m_MainButton.SetToolTipText( strTemp );

	m_wndRibbonBar.SetApplicationButton (&m_MainButton, CSize (64, 32) );

	CMFCRibbonMainPanel* pMainPanel = m_wndRibbonBar.AddMainCategory(strTemp, IDB_FILESMALL, IDB_FILELARGE, CSize( 16, 16 ), CSize( 32, 32 ) );

	AddRibbonButton( pMainPanel, IDS_RIBBON_NEW, ID_FILE_NEW, 0, LARGE );
	AddRibbonButton( pMainPanel, IDS_RIBBON_OPEN, ID_FILE_OPEN, 1, LARGE );

	strTemp.LoadString( IDS_MENU_SAMPLE_GALLERY );
	CMFCRibbonGallery *pSampleGallery = new CMFCRibbonGallery( ID_RIBBON_SAMPLE_GALLERY, strTemp, -1, 41, IDB_SAMPLE_GALLERY, 96 );
	m_pSampleGallery = pSampleGallery;

	pSampleGallery->SetIconsInRow( 6 );
	pSampleGallery->SetButtonMode();
	pSampleGallery->SelectItem( -1 );

	pMainPanel->Add( pSampleGallery );

	CSampleGallery GalleryData;
	int Count = GalleryData.GetCount();
	for( int Counter = 0; Counter < Count; ++Counter )
	{
		CValidatedString strTemp;
		strTemp.LoadString( GalleryData.GetStringID( Counter ) );
		int EOL = strTemp.Find( '\n' );
		if( EOL >= 0 )
			strTemp.Truncate( EOL );
		pSampleGallery->SetItemToolTip( Counter, strTemp );
	}

	AddRibbonButton( pMainPanel, IDS_RIBBON_SAVE, ID_FILE_SAVE, 2, LARGE );
	AddRibbonButton( pMainPanel, IDS_RIBBON_SAVEAS, ID_FILE_SAVE_AS, 3, LARGE );
	
	AddExportButton( pMainPanel );

	//AddRibbonButton( pMainPanel, IDS_RIBBON_SAVEVIDEO, ID_FILE_SAVEVIDEO, 5, LARGE );
	//AddRibbonButton( pMainPanel, IDS_RIBBON_SAVEIMAGE, ID_FILE_SAVEIMAGE, 37, LARGE );



	pMainPanel->Add(new CMFCRibbonSeparator(TRUE));
	AddPrintButton( pMainPanel );

	pMainPanel->Add(new CMFCRibbonSeparator(TRUE));

	AddRibbonButton( pMainPanel, IDS_RIBBON_CLOSE, ID_FILE_CLOSE, 9, LARGE );

	strTemp.LoadString(IDS_RIBBON_RECENT_DOCS);
	pMainPanel->AddRecentFilesList(strTemp);
}

void CMainFrame::CreateHelpButtons( void )
{
	CValidatedString strTemp;

	// ABout button.
	CMFCRibbonButton* pAboutButton = new CMFCRibbonButton( ID_HELP_ABOUT, 0, m_PanelImages.ExtractIcon(1), FALSE );
	strTemp.LoadString( ID_HELP_ABOUT_TIP );
	pAboutButton->SetToolTipText(strTemp);
	strTemp.LoadString( ID_HELP_ABOUT_DESCRIPTION );
	pAboutButton->SetDescription(strTemp);
	m_wndRibbonBar.AddToTabs( pAboutButton );

	// Help button.
	CMFCRibbonButton* pHelpButton = new CMFCRibbonButton( ID_HELP_USERGUIDE, 0, m_PanelImages.ExtractIcon(0), FALSE );
	strTemp.LoadString( ID_HELP_USERGUIDE_TIP );
	pHelpButton->SetToolTipText(strTemp);
	strTemp.LoadString( ID_HELP_USERGUIDE_DESCRIPTION );
	pHelpButton->SetDescription(strTemp);
	m_wndRibbonBar.AddToTabs( pHelpButton );
}

static CMFCRibbonCategory *CreateCategory( CMyMFCRibbonBar *pRibbonBar, UINT StringId )
{
	CValidatedString strTemp;
	strTemp.LoadString(StringId);
	return pRibbonBar->AddCategory(strTemp, IDB_FILESMALL, IDB_FILELARGE, CSize( 16, 16 ), CSize( 32, 32 ));
}

void CMainFrame::CreateQuickAccessCommands( void )
{
	CList<UINT, UINT> lstQATCmds;

	lstQATCmds.AddTail(ID_FILE_NEW);
	lstQATCmds.AddTail(ID_FILE_OPEN);
	lstQATCmds.AddTail(ID_FILE_SAVE);
	lstQATCmds.AddTail(ID_FILE_PRINT_DIRECT);

	m_wndRibbonBar.SetQuickAccessCommands(lstQATCmds);
}

void CMainFrame::CreatePrintingCategory( void )
{
	CMFCRibbonCategory* pCategoryPrinting = CreateCategory( &m_wndRibbonBar, IDS_RIBBON_PRINTING );
	CreatePrintPanel( pCategoryPrinting );
	CreatePrintOptionsPanel( pCategoryPrinting );
}

void CMainFrame::CreateHomeCategory( void )
{
	CMFCRibbonCategory* pCategoryHome = CreateCategory( &m_wndRibbonBar, IDS_RIBBON_HOME );
	CreateClipboardPanel( pCategoryHome );
	CreateViewPanel( pCategoryHome );
	CreateDimensionsPanel( pCategoryHome );
	CreateInsertPanel( pCategoryHome );
	CreateElementPanel( pCategoryHome );
	CreateAlignPanel( pCategoryHome );
	CreateSimulationPanel( pCategoryHome );
}

void CMainFrame::InitializeRibbon()
{
	m_wndRibbonBar.SetWindows7Look( TRUE );
	m_wndRibbonBar.EnablePrintPreview( 1 );

	// Load panel images:
	m_PanelImages.SetImageSize(CSize(16, 16));
	m_PanelImages.Load(IDB_BUTTONS);

	m_PanelImagesList.Create( IDB_BUTTONS, 16, 1, ILC_COLOR32 );

	CreateMainCategory();
	CreateHomeCategory();
	CreatePrintingCategory();
	CreateQuickAccessCommands();
	CreateHelpButtons();

	// strTemp.LoadString(IDS_RIBBON_CONFIG);
	// CMFCRibbonCategory* pCategoryConfig = m_wndRibbonBar.AddCategory(strTemp, IDB_FILESMALL, IDB_FILELARGE, CSize( 16, 16 ), CSize( 32, 32 ));
	// strTemp.LoadString(IDS_RIBBON_SETTINGS);
	// CMFCRibbonCategory* pCategoryDetails = m_wndRibbonBar.AddCategory(strTemp, IDB_FILESMALL, IDB_FILELARGE, CSize( 16, 16 ), CSize( 32, 32 ));

}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}
#endif //_DEBUG

class CMFCVisualManagerWindows7Dave : public CMFCVisualManagerWindows7
{
	DECLARE_DYNAMIC( CMFCVisualManagerWindows7Dave )
};

IMPLEMENT_DYNAMIC( CMFCVisualManagerWindows7Dave, CMFCVisualManagerWindows7 )


// CMainFrame message handlers

void CMainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

#if 0

	theApp.m_nAppLook = id;

	switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		break;

	case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		break;

	case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	default:
		switch (theApp.m_nAppLook)
		{
		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
			break;
		}

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
	}

#endif

	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));

	RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnFilePrint()
{
	if (IsPrintPreview())
	{
		PostMessage(WM_COMMAND, AFX_ID_PREVIEW_PRINT);
	}
}

void CMainFrame::OnFilePrintPreview()
{
	if (IsPrintPreview())
	{
		//PostMessage(WM_COMMAND, AFX_ID_PREVIEW_CLOSE);  // force Print Preview mode closed
	}
}

void CMainFrame::OnUpdateFilePrintPreview(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(IsPrintPreview());
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers


void CMainFrame::OnHelpUserguide()
{
	HINSTANCE hResult = 0;
	static const char *DOC_FILE = ".\\Linkage.pdf";
	if( _access( DOC_FILE, 0 ) == 0 )
		hResult = ShellExecute( this->GetSafeHwnd(), "open", DOC_FILE, 0, 0, SW_SHOWNORMAL );
		
	if( hResult == 0 || hResult == INVALID_HANDLE_VALUE )
		AfxMessageBox( "Unable to open the documentation file.", MB_ICONEXCLAMATION | MB_OK );
}

void CMainFrame::OnHelpAbout()
{
	CAboutDialog dlg;
	dlg.DoModal();
}

void CMainFrame::ConfigureDocumentationMenu( CMenu *pMenu )
{
	if( pMenu == 0 || _access( ".\\Linkage.pdf", 0 ) == 0 )
		return;
		
	// Remove the documentation menu item since there is no document.
		
	int Index;
	for( Index = pMenu->GetMenuItemCount() - 1; Index >= 0; --Index )
	{
		if( pMenu->GetMenuItemID( Index ) == ID_HELP_USERGUIDE )
			break;
	}
	if( Index < 0 )
		return; // never found the placeholder.

	pMenu->DeleteMenu( Index, MF_BYPOSITION );
	if( pMenu->GetMenuItemID( Index ) == 0 ) // Separator?
		pMenu->DeleteMenu( Index, MF_BYPOSITION );
}

void SetStatusText( const char *pText )
{
	CMainFrame* pFrame= (CMainFrame*)AfxGetMainWnd();
	pFrame->m_wndStatusBar.GetElement( 0 )->SetText( pText == 0 ? "" : pText );
	pFrame->m_wndStatusBar.Invalidate( true );
}

void SetStatusText( UINT ResourceID)
{
	CString String;
	BOOL bNameValid = String.LoadString( ResourceID );
	SetStatusText( String );
}

void ShowPrintPreviewCategory( bool bShow )
{
	CMainFrame* pFrame= (CMainFrame*)AfxGetMainWnd();
	pFrame->m_wndRibbonBar.SetPrintPreviewMode( bShow ? 1 : 0 );
}

void CMainFrame::OnSelectSample()
{
	int Selection = m_pSampleGallery->GetLastSelectedItem( ID_RIBBON_SAMPLE_GALLERY );

	m_pSampleGallery->SelectItem( -1 );

	CSampleGallery GalleryData;
	int Count = GalleryData.GetCount();
	if( Selection >= Count )
		return;

	PostMessage( WM_COMMAND, GalleryData.GetCommandID( Selection ) );
}


void CMainFrame::OnDropFiles(HDROP hDropInfo)
{
	CFrameWndEx::OnDropFiles(hDropInfo);
}

