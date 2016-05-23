// LinkageView.cpp : implementation of the CLinkageView class
//

#include "stdafx.h"
#include "Linkage.h"
#include "mainfrm.h"
#include "geometry.h"
#include "link.h"
#include "connector.h"
#include "LinkageDoc.h"
#include "LinkageView.h"
#include "mmsystem.h"
#include "ConnectorPropertiesDialog.h"
#include "Renderer.h"
#include "graham.h"
#include "imageselectdialog.h"
#include "float.h"
#include "linkpropertiesdialog.h"
#include "testdialog.h"
#include "DebugItem.h"
#include "RecordDialog.h"
#include "AngleDialog.h"
#include <stdio.h>
#include <string.h>
#include "gdiplus.h"
#include "SampleGallery.h"
#include "PointDialog.h"
#include "LinePropertiesDialog.h"
#include "DXFFile.h"
#include "GearRatioDialog.h"
#include "ExportImageSettingsDialog.h"

#include <algorithm>
#include <vector>

using namespace std;

using namespace Gdiplus;

CDebugItemList DebugItemList;

static const double ZOOM_AMOUNT = 1.3;

static const double LINK_INSERT_PIXELS = 40;

static const int GRAB_DISTANCE = 6;

static const int FRAMES_PER_SECONDS = 30;

static const int ANIMATIONWIDTH = 960;
static const int ANIMATIONHEIGHT = 540;

static const int OFFSET_INCREMENT = 25;
static const int RADIUS_MEASUREMENT_OFFSET = 9;

static COLORREF COLOR_ADJUSTMENTKNOBS = RGB( 0, 0, 0 );
static COLORREF COLOR_BLACK = RGB( 0, 0, 0 );
static COLORREF COLOR_NONADJUSTMENTKNOBS = RGB( 170, 170, 170 );
static COLORREF COLOR_ALIGNMENTHINT = RGB( 170, 220, 170 );
static COLORREF COLOR_SNAPHINT = RGB( 50, 170, 240 );
static COLORREF COLOR_SUPERHIGHLIGHT = RGB( 255, 255, 192 );
static COLORREF COLOR_SUPERHIGHLIGHT2 = RGB( 255, 255, 128 );
static COLORREF COLOR_TEXT = RGB( 24, 24, 24 );
static COLORREF COLOR_CHAIN = RGB( 50, 50, 50 );
static COLORREF COLOR_STRETCHDOTS = RGB( 220, 220, 220 );
static COLORREF COLOR_MOTIONPATH = RGB( 60, 60, 60 );
static COLORREF COLOR_DRAWINGLIGHT = RGB( 200, 200, 200 );
static COLORREF COLOR_DRAWINGDARK = RGB( 70, 70, 70 );
static COLORREF COLOR_GRID = RGB( 200, 240, 240 );
static COLORREF COLOR_GROUND = RGB( 100, 100, 100 );
static COLORREF COLOR_MINORSELECTION = RGB( 170, 170, 170 );
static COLORREF COLOR_LASTSELECTION = RGB( 196, 96, 96 );

static const int CONNECTORRADIUS = 5;
static const int CONNECTORTRIANGLE = 6;
static const int ANCHORBASE = 4;

#ifdef _DEBUG
//#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const int PIXEL_SNAP_DISTANCE = 6;

wchar_t* ToUnicode( const char *pString )
{
	size_t origsize = strlen( pString ) + 1;
    size_t convertedChars = 0;
    wchar_t *wcstring = new wchar_t[origsize];
    mbstowcs_s( &convertedChars, wcstring, origsize, pString, _TRUNCATE );
	return wcstring;
}

static int GetLongestHullDimensionLine( CLink *pLink, int &BestEndPoint, CFPoint *pHullPoints, int HullPointCount, CConnector **pStartConnector );

class CTempLink : public CLink
{
	public:
	CTempLink() : CLink(), OriginalLink( *this ) {}
	CTempLink( const CLink &ExistingLink ) : CLink( ExistingLink ), OriginalLink( ExistingLink ) {}

	const CLink &OriginalLink; // Can't set it from a reference

	virtual bool GetGearRadii( const GearConnectionList &ConnectionList, std::list<double> &RadiusList ) const
	{
		return OriginalLink.GetGearRadii( ConnectionList, RadiusList );
	}
};

/////////////////////////////////////////////////////////////////////////////
// CLinkageView

IMPLEMENT_DYNCREATE(CLinkageView, CView)

BEGIN_MESSAGE_MAP(CLinkageView, CView)
	//{{AFX_MSG_MAP(CLinkageView)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	//ON_MESSAGE(WM_GESTURE, OnGesture)
	ON_COMMAND(ID_MECHANISM_RESET, OnMechanismReset)
	ON_COMMAND(ID_MECHANISM_SIMULATE, &CLinkageView::OnMechanismSimulate)
	ON_COMMAND(ID_CONTROL_WINDOW, &CLinkageView::OnControlWindow)
	ON_UPDATE_COMMAND_UI(ID_MECHANISM_SIMULATE, &CLinkageView::OnUpdateButtonRun)
	ON_COMMAND(ID_SIMULATION_SIMULATE, &CLinkageView::OnMechanismQuicksim)
	ON_UPDATE_COMMAND_UI(ID_SIMULATION_SIMULATE, &CLinkageView::OnUpdateButtonRun)

	ON_COMMAND(ID_SIMULATE_INTERACTIVE, &CLinkageView::OnSimulateInteractive)
	ON_UPDATE_COMMAND_UI(ID_SIMULATE_INTERACTIVE, &CLinkageView::OnUpdateButtonRun)

	ON_COMMAND(ID_SIMULATION_PAUSE, &CLinkageView::OnSimulatePause)
	ON_UPDATE_COMMAND_UI(ID_SIMULATION_PAUSE, &CLinkageView::OnUpdateSimulatePause)
	ON_COMMAND(ID_SIMULATE_FORWARD, &CLinkageView::OnSimulateForward)
	ON_UPDATE_COMMAND_UI(ID_SIMULATE_FORWARD, &CLinkageView::OnUpdateSimulateForwardBackward)
	ON_COMMAND(ID_SIMULATE_BACKWARD, &CLinkageView::OnSimulateBackward)
	ON_UPDATE_COMMAND_UI(ID_SIMULATE_BACKWARD, &CLinkageView::OnUpdateSimulateForwardBackward)

	ON_COMMAND(ID_SIMULATE_FORWARD_BIG, &CLinkageView::OnSimulateForwardBig)
	ON_UPDATE_COMMAND_UI(ID_SIMULATE_FORWARD_BIG, &CLinkageView::OnUpdateSimulateForwardBackward)
	ON_COMMAND(ID_SIMULATE_BACKWARD_BIG, &CLinkageView::OnSimulateBackwardBig)
	ON_UPDATE_COMMAND_UI(ID_SIMULATE_BACKWARD_BIG, &CLinkageView::OnUpdateSimulateForwardBackward)

	ON_COMMAND(ID_SIMULATE_MANUAL, &CLinkageView::OnSimulateManual)
	ON_UPDATE_COMMAND_UI(ID_SIMULATE_MANUAL, &CLinkageView::OnUpdateButtonRun)
	ON_WM_TIMER()
	ON_UPDATE_COMMAND_UI(ID_MECHANISM_RESET, &CLinkageView::OnUpdateMechanismReset)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COMBINE, &CLinkageView::OnUpdateEditCombine)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CONNECT, &CLinkageView::OnUpdateEditConnect)
	ON_COMMAND(ID_EDIT_COMBINE, &CLinkageView::OnEditCombine)
	ON_COMMAND(ID_EDIT_CONNECT, &CLinkageView::OnEditConnect)
	ON_COMMAND(ID_EDIT_FASTEN, &CLinkageView::OnEditFasten)
	ON_UPDATE_COMMAND_UI(ID_EDIT_FASTEN, &CLinkageView::OnUpdateEditFasten)
	ON_COMMAND(ID_EDIT_UNFASTEN, &CLinkageView::OnEditUnfasten)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNFASTEN, &CLinkageView::OnUpdateEditUnfasten)

	ON_COMMAND(ID_EDIT_SET_RATIO, &CLinkageView::OnEditSetRatio)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SET_RATIO, &CLinkageView::OnUpdateEditSetRatio)
	ON_COMMAND(ID_EDIT_MAKEANCHOR, &CLinkageView::OnEditmakeAnchor)
	ON_UPDATE_COMMAND_UI(ID_EDIT_MAKEANCHOR, &CLinkageView::OnUpdateEditmakeAnchor)

	ON_UPDATE_COMMAND_UI(ID_EDIT_DELETESELECTED, &CLinkageView::OnUpdateEditSelected)
	ON_COMMAND(ID_EDIT_DELETESELECTED, &CLinkageView::OnEditDeleteselected)
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_UPDATE_COMMAND_UI(ID_SIMULATION_RUN, &CLinkageView::OnUpdateButtonRun)
	ON_COMMAND(ID_SIMULATION_RUN, &CLinkageView::OnButtonRun)
	ON_UPDATE_COMMAND_UI(ID_SIMULATION_STOP, &CLinkageView::OnUpdateButtonStop)
	ON_COMMAND(ID_SIMULATION_STOP, &CLinkageView::OnButtonStop)
	ON_UPDATE_COMMAND_UI(ID_SIMULATION_PIN, &CLinkageView::OnUpdateButtonPin)
	ON_COMMAND(ID_SIMULATION_PIN, &CLinkageView::OnButtonPin)

	ON_UPDATE_COMMAND_UI(ID_ALIGN_ALIGNBUTTON, &CLinkageView::OnUpdateAlignButton)
	ON_UPDATE_COMMAND_UI(ID_ALIGN_SNAPBUTTON, &CLinkageView::OnUpdateNotSimulating)
	ON_UPDATE_COMMAND_UI(ID_ALIGN_DETAILSBUTTON, &CLinkageView::OnUpdateNotSimulating)
	ON_UPDATE_COMMAND_UI(ID_EDIT_INSERTBUTTON, &CLinkageView::OnUpdateEdit)

	ON_COMMAND( ID_FILE_PRINTFULL, OnPrintFull )
	ON_UPDATE_COMMAND_UI( ID_FILE_PRINTFULL, OnUpdatePrintFull )

	ON_COMMAND( ID_FILE_PRINT_SETUP, OnFilePrintSetup )
	ON_COMMAND(ID_INSERT_CONNECTOR, (AFX_PMSG)&CLinkageView::OnInsertConnector)
	ON_COMMAND(ID_INSERT_POINT,(AFX_PMSG)&CLinkageView::OnInsertPoint)
	ON_COMMAND(ID_INSERT_LINE,(AFX_PMSG)&CLinkageView::OnInsertLine)
	ON_COMMAND(ID_INSERT_MEASUREMENT,(AFX_PMSG)&CLinkageView::OnInsertMeasurement)
	ON_COMMAND(ID_INSERT_GEAR,(AFX_PMSG)&CLinkageView::OnInsertGear)
	ON_COMMAND(ID_INSERT_LINK2, (AFX_PMSG)&CLinkageView::OnInsertDouble)
	ON_COMMAND(ID_INSERT_LINK3, (AFX_PMSG)&CLinkageView::OnInsertTriple)
	ON_COMMAND(ID_INSERT_LINK4, (AFX_PMSG)&CLinkageView::OnInsertQuad)
	ON_COMMAND(ID_INSERT_ANCHOR, (AFX_PMSG)&CLinkageView::OnInsertAnchor)
	ON_COMMAND(ID_INSERT_ANCHORLINK, (AFX_PMSG)&CLinkageView::OnInsertAnchorLink)
	ON_COMMAND(ID_INSERT_INPUT, (AFX_PMSG)&CLinkageView::OnInsertRotatinganchor)
	ON_UPDATE_COMMAND_UI(ID_INSERT_POINT, &CLinkageView::OnUpdateEdit)
	ON_UPDATE_COMMAND_UI(ID_INSERT_LINE, &CLinkageView::OnUpdateEdit)
	ON_UPDATE_COMMAND_UI(ID_INSERT_CONNECTOR, &CLinkageView::OnUpdateEdit)
	ON_UPDATE_COMMAND_UI(ID_INSERT_LINK2, &CLinkageView::OnUpdateEdit)
	ON_UPDATE_COMMAND_UI(ID_INSERT_LINK3, &CLinkageView::OnUpdateEdit)
	ON_UPDATE_COMMAND_UI(ID_INSERT_LINK4, &CLinkageView::OnUpdateEdit)
	ON_UPDATE_COMMAND_UI(ID_INSERT_ACTUATOR, &CLinkageView::OnUpdateEdit)
	ON_UPDATE_COMMAND_UI(ID_INSERT_ANCHOR, &CLinkageView::OnUpdateEdit)
	ON_UPDATE_COMMAND_UI(ID_INSERT_ANCHORLINK, &CLinkageView::OnUpdateEdit)
	ON_UPDATE_COMMAND_UI(ID_INSERT_INPUT, &CLinkageView::OnUpdateEdit)
	ON_COMMAND(ID_INSERT_LINK, (AFX_PMSG)&CLinkageView::OnInsertLink)
	ON_UPDATE_COMMAND_UI(ID_PROPERTIES_PROPERTIES, &CLinkageView::OnUpdateEditProperties)
	ON_COMMAND(ID_PROPERTIES_PROPERTIES, &CLinkageView::OnEditProperties)
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(ID_EDIT_JOIN, &CLinkageView::OnEditJoin)
	ON_UPDATE_COMMAND_UI(ID_EDIT_JOIN, &CLinkageView::OnUpdateEditJoin)
	ON_COMMAND(ID_EDIT_LOCK, &CLinkageView::OnEditLock)
	ON_UPDATE_COMMAND_UI(ID_EDIT_LOCK, &CLinkageView::OnUpdateEditLock)
	ON_COMMAND(ID_EDIT_SELECT_ALL, &CLinkageView::OnEditSelectall)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SELECT_ALL, &CLinkageView::OnUpdateSelectall)

	ON_COMMAND(ID_EDIT_SPLIT, &CLinkageView::OnEditSplit)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SPLIT, &CLinkageView::OnUpdateEditSplit)

	ON_COMMAND(ID_EDIT_ADDCONNECTOR, &CLinkageView::OnAddConnector)
	ON_UPDATE_COMMAND_UI(ID_EDIT_ADDCONNECTOR, &CLinkageView::OnUpdateAddConnector)

	ON_COMMAND(ID_ALIGN_SETANGLE, &CLinkageView::OnAlignSetAngle)
	ON_UPDATE_COMMAND_UI(ID_ALIGN_SETANGLE, &CLinkageView::OnUpdateAlignSetAngle)
	ON_COMMAND(ID_ALIGN_RIGHTANGLE, &CLinkageView::OnAlignRightAngle)
	ON_UPDATE_COMMAND_UI(ID_ALIGN_RIGHTANGLE, &CLinkageView::OnUpdateAlignRightAngle)
	ON_COMMAND(ID_ALIGN_RECTANGLE, &CLinkageView::OnAlignRectangle)
	ON_UPDATE_COMMAND_UI(ID_ALIGN_RECTANGLE, &CLinkageView::OnUpdateAlignRectangle)
	ON_COMMAND(ID_ALIGN_PARALLELOGRAM, &CLinkageView::OnAlignParallelogram)
	ON_UPDATE_COMMAND_UI(ID_ALIGN_PARALLELOGRAM, &CLinkageView::OnUpdateAlignParallelogram)

	ON_COMMAND(ID_ALIGN_HORIZONTAL, &CLinkageView::OnAlignHorizontal)
	ON_UPDATE_COMMAND_UI(ID_ALIGN_HORIZONTAL, &CLinkageView::OnUpdateAlignHorizontal)
	ON_COMMAND(ID_ALIGN_VERTICAL, &CLinkageView::OnAlignVertical)
	ON_UPDATE_COMMAND_UI(ID_ALIGN_VERTICAL, &CLinkageView::OnUpdateAlignVertical)
	ON_COMMAND(ID_ALIGN_LINEUP, &CLinkageView::OnAlignLineup)
	ON_UPDATE_COMMAND_UI(ID_ALIGN_LINEUP, &CLinkageView::OnUpdateAlignLineup)
	ON_COMMAND(ID_ALIGN_EVENSPACE, &CLinkageView::OnAlignEvenSpace)
	ON_UPDATE_COMMAND_UI(ID_ALIGN_EVENSPACE, &CLinkageView::OnUpdateAlignEvenSpace)

	ON_COMMAND(ID_ALIGN_FLIPH, &CLinkageView::OnFlipHorizontal)
	ON_UPDATE_COMMAND_UI(ID_ALIGN_FLIPH, &CLinkageView::OnUpdateFlipHorizontal)
	ON_COMMAND(ID_ALIGN_FLIPV, &CLinkageView::OnFlipVertical)
	ON_UPDATE_COMMAND_UI(ID_ALIGN_FLIPV, &CLinkageView::OnUpdateFlipVertical)

	ON_COMMAND(ID_VIEW_LABELS, &CLinkageView::OnViewLabels)
	ON_UPDATE_COMMAND_UI(ID_VIEW_LABELS, &CLinkageView::OnUpdateViewLabels)
	ON_COMMAND(ID_VIEW_ANGLES, &CLinkageView::OnViewAngles)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ANGLES, &CLinkageView::OnUpdateViewAngles)
	ON_COMMAND(ID_EDIT_SNAP, &CLinkageView::OnEditSnap)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SNAP, &CLinkageView::OnUpdateEditSnap)
	ON_COMMAND(ID_EDIT_GRIDSNAP, &CLinkageView::OnEditGridSnap)
	ON_UPDATE_COMMAND_UI(ID_EDIT_GRIDSNAP, &CLinkageView::OnUpdateEditGridSnap)

	ON_COMMAND(ID_EDIT_AUTOJOIN, &CLinkageView::OnEditAutoJoin)
	ON_UPDATE_COMMAND_UI(ID_EDIT_AUTOJOIN, &CLinkageView::OnUpdateEditAutoJoin)

	ON_COMMAND(ID_VIEW_DATA, &CLinkageView::OnViewData)
	ON_UPDATE_COMMAND_UI(ID_VIEW_DATA, &CLinkageView::OnUpdateViewData)
	ON_COMMAND(ID_VIEW_DIMENSIONS, &CLinkageView::OnViewDimensions)
	ON_UPDATE_COMMAND_UI(ID_VIEW_DIMENSIONS, &CLinkageView::OnUpdateViewDimensions)
	ON_COMMAND(ID_VIEW_GROUNDDIMENSIONS, &CLinkageView::OnViewGroundDimensions)
	ON_UPDATE_COMMAND_UI(ID_VIEW_GROUNDDIMENSIONS, &CLinkageView::OnUpdateViewGroundDimensions)

	ON_COMMAND(ID_VIEW_SOLIDLINKS, &CLinkageView::OnViewSolidLinks)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SOLIDLINKS, &CLinkageView::OnUpdateViewSolidLinks)

	ON_COMMAND(ID_VIEW_DRAWING, &CLinkageView::OnViewDrawing)
	ON_UPDATE_COMMAND_UI(ID_VIEW_DRAWING, &CLinkageView::OnUpdateViewDrawing)
	ON_COMMAND(ID_VIEW_MECHANISM, &CLinkageView::OnViewMechanism)
	ON_UPDATE_COMMAND_UI(ID_VIEW_MECHANISM, &CLinkageView::OnUpdateViewMechanism)

	ON_COMMAND(ID_EDIT_DRAWING, &CLinkageView::OnEditDrawing)
	ON_UPDATE_COMMAND_UI(ID_EDIT_DRAWING, &CLinkageView::OnUpdateEditDrawing)
	ON_COMMAND(ID_EDIT_MECHANISM, &CLinkageView::OnEditMechanism)
	ON_UPDATE_COMMAND_UI(ID_EDIT_MECHANISM, &CLinkageView::OnUpdateEditMechanism)

	ON_COMMAND(ID_VIEW_DEBUG, &CLinkageView::OnViewDebug)
	ON_UPDATE_COMMAND_UI(ID_VIEW_DEBUG, &CLinkageView::OnUpdateViewDebug)
	ON_COMMAND(ID_VIEW_ANICROP, &CLinkageView::OnViewAnicrop)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ANICROP, &CLinkageView::OnUpdateViewAnicrop)
	ON_COMMAND(ID_VIEW_LARGEFONT, &CLinkageView::OnViewLargeFont)
	ON_UPDATE_COMMAND_UI(ID_VIEW_LARGEFONT, &CLinkageView::OnUpdateViewLargeFont)

	ON_COMMAND(ID_VIEW_GRID, &CLinkageView::OnViewGrid)
	ON_UPDATE_COMMAND_UI(ID_VIEW_GRID, &CLinkageView::OnUpdateViewGrid)
	ON_COMMAND(ID_VIEW_PARTS, &CLinkageView::OnViewParts)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PARTS, &CLinkageView::OnUpdateViewParts)

	ON_COMMAND(ID_EDIT_CUT, &CLinkageView::OnEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, &CLinkageView::OnUpdateEditSelected)
	ON_COMMAND(ID_EDIT_COPY, &CLinkageView::OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, &CLinkageView::OnUpdateEditSelected)
	ON_COMMAND(ID_EDIT_PASTE, &CLinkageView::OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, &CLinkageView::OnUpdateEditPaste)

	ON_COMMAND(ID_VIEW_BOLD, &CLinkageView::OnViewBold)
	ON_UPDATE_COMMAND_UI(ID_VIEW_BOLD, &CLinkageView::OnUpdateViewBold)

	ON_COMMAND( ID_VIEW_UNITS, &CLinkageView::OnViewUnits )
	ON_COMMAND( ID_VIEW_COORDINATES, &CLinkageView::OnViewCoordinates )

	ON_UPDATE_COMMAND_UI(ID_VIEW_UNITS, &CLinkageView::OnUpdateViewUnits)
	ON_UPDATE_COMMAND_UI(ID_VIEW_COORDINATES, &CLinkageView::OnUpdateViewCoordinates)

	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEWHEEL()
	ON_COMMAND(ID_VIEW_ZOOMIN, &CLinkageView::OnViewZoomin)
	ON_COMMAND(ID_VIEW_ZOOMOUT, &CLinkageView::OnViewZoomout)
	ON_COMMAND(ID_VIEW_ZOOMFIT, &CLinkageView::OnMenuZoomfit)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOMIN, &CLinkageView::OnUpdateNotSimulating)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOMOUT, &CLinkageView::OnUpdateNotSimulating)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOMFIT, &CLinkageView::OnUpdateNotSimulating)
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_SAVEVIDEO, &OnFileSaveVideo)
	ON_COMMAND(ID_FILE_SAVEIMAGE, &OnFileSaveImage)
	ON_COMMAND(ID_FILE_SAVEDXF, &OnFileSaveDXF)
	ON_COMMAND(ID_FILE_SAVEMOTION, &OnFileSaveMotion)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVEMOTION, &CLinkageView::OnUpdateFileSaveMotion)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CLinkageView::OnFilePrintPreview)
	ON_COMMAND(ID_SELECT_NEXT,OnSelectNext)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, &CLinkageView::OnUpdateEditUndo)
	ON_COMMAND(ID_EDIT_UNDO, &CLinkageView::OnEditUndo)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_COMMAND(ID_EDIT_SLIDE, &CLinkageView::OnEditSlide)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SLIDE, &CLinkageView::OnUpdateEditSlide)
	ON_COMMAND(ID_INSERT_SLIDER, (AFX_PMSG)&CLinkageView::OnInsertlinkSlider)
	ON_UPDATE_COMMAND_UI(ID_INSERT_SLIDER, &CLinkageView::OnUpdateEdit)
	//ON_UPDATE_COMMAND_UI( ID_RIBBON_SAMPLE_GALLERY, &CLinkageView::OnUpdateEdit)
	ON_COMMAND(ID_INSERT_ACTUATOR, (AFX_PMSG)&CLinkageView::OnInsertActuator)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_COMMAND(ID_POPUP_GALLERY, &CLinkageView::OnPopupGallery)
	ON_WM_KEYUP()
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	ON_COMMAND_RANGE( ID_SAMPLE_SIMPLE, ID_SAMPLE_UNUSED20, OnSelectSample )
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static const int SMALL_FONT_SIZE = 9;
static const int MEDIUM_FONT_SIZE = 13;

/////////////////////////////////////////////////////////////////////////////
// CLinkageView construction/destruction

CLinkageView::CLinkageView()
{
	m_MouseAction = ACTION_NONE;
	m_bSimulating = false;
	m_bAllowEdit = true;
	m_SimulationControl = AUTO;
	m_bShowLabels = true;
	m_bShowAngles = true;
	m_bSnapOn = true;
	m_bGridSnap = false;
	m_bAutoJoin = false;
	m_bShowDimensions = false;
	m_bShowGroundDimensions = true;
	m_bNewLinksSolid = false;
	m_xGrid = 20.0;
	m_yGrid = 20.0;
	m_bShowData = false;
	m_bShowDebug = false;
	m_bShowBold = false;
	m_bShowAnicrop = false;
	m_bShowLargeFont = false;
	m_bShowGrid = false;
	m_ScreenZoom = 1;
	m_ScrollPosition.SetPoint( 0, 0 );
	m_ScreenScrollPosition.SetPoint( 0, 0 );
	m_DragStartPoint.SetPoint( 0, 0 );
	m_StretchingControl = AC_TOP_LEFT;
	m_VisibleAdjustment = ADJUSTMENT_NONE;
	m_bChangeAdjusters = false;
	m_bMouseMovedEnough = false;
	m_bSuperHighlight = false;
	m_YUpDirection = -1;

	m_bRequestAbort = false;
	m_bProcessingVideoThread = false;
	m_pVideoBitmapQueue = 0;
	m_hVideoFrameEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
	m_VideoStartFrames = 0;
	m_VideoRecordFrames = 0;
	m_VideoEndFrames = 0;
	m_bUseVideoCounters = false;
	m_bShowSelection = true;

	m_SelectedEditLayers = CLinkageDoc::ALLLAYERS;
	m_SelectedViewLayers = CLinkageDoc::ALLLAYERS;

	m_pAvi = 0;
	m_bRecordingVideo = false;
	m_RecordQuality = 0;

	m_pPopupGallery = new CPopupGallery( ID_POPUP_GALLERY, IDB_INSERT_POPUP_GALLERY, 48 );
	if( m_pPopupGallery != 0 )
	{
		m_pPopupGallery->SetRows( 2 );
		m_pPopupGallery->SetTooltip( 0, ID_INSERT_CONNECTOR );
		m_pPopupGallery->SetTooltip( 1, ID_INSERT_ANCHOR );
		m_pPopupGallery->SetTooltip( 2, ID_INSERT_LINK2 );
		m_pPopupGallery->SetTooltip( 3, ID_INSERT_ANCHORLINK );
		m_pPopupGallery->SetTooltip( 4, ID_INSERT_INPUT );
		m_pPopupGallery->SetTooltip( 5, ID_INSERT_ACTUATOR );
		m_pPopupGallery->SetTooltip( 6, ID_INSERT_LINK3 );
		m_pPopupGallery->SetTooltip( 7, ID_INSERT_LINK4 );
		m_pPopupGallery->SetTooltip( 8, ID_INSERT_POINT );
		m_pPopupGallery->SetTooltip( 9, ID_INSERT_LINE );
		m_pPopupGallery->SetTooltip( 10, ID_INSERT_MEASUREMENT );
		m_pPopupGallery->SetTooltip( 11, ID_INSERT_GEAR );
	}

	m_SmallFont.CreateFont( -SMALL_FONT_SIZE, 0, 0, 0, FW_LIGHT, 0, 0, 0,
							DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
							ANTIALIASED_QUALITY | CLEARTYPE_QUALITY, VARIABLE_PITCH | FF_SWISS,
							"arial" );

	m_MediumFont.CreateFont( -MEDIUM_FONT_SIZE, 0, 0, 0, FW_NORMAL, 0, 0, 0,
							DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
							ANTIALIASED_QUALITY | CLEARTYPE_QUALITY, VARIABLE_PITCH | FF_SWISS,
							"arial" );

	CWinApp *pApp = AfxGetApp();
	if( pApp != 0 )
	{
		m_bShowLabels = pApp->GetProfileInt( "Settings", "Showlabels", 1 ) != 0;
		m_bShowAngles = pApp->GetProfileInt( "Settings", "Showangles", 1 ) != 0;
		m_bShowDebug = pApp->GetProfileInt( "Settings", "Showdebug", 0 ) != 0;
		m_bShowBold = pApp->GetProfileInt( "Settings", "Showdata", 0 ) != 0;
		m_bShowData = pApp->GetProfileInt( "Settings", "ShowBold", 0 ) != 0;
		m_bSnapOn = pApp->GetProfileInt( "Settings", "Snapon", 1 ) != 0;
		m_bGridSnap = pApp->GetProfileInt( "Settings", "Gridsnap", 1 ) != 0;
		m_bAutoJoin = pApp->GetProfileIntA( "Settings", "AutoJoin", 1 ) != 0;
		m_bNewLinksSolid = pApp->GetProfileInt( "Settings", "Newlinkssolid", 0 ) != 0;
		m_bShowAnicrop = pApp->GetProfileInt( "Settings", "Showanicrop", 0 ) != 0;
		m_bShowLargeFont = pApp->GetProfileInt( "Settings", "Showlargefont", 0 ) != 0;
		m_bPrintFullSize = pApp->GetProfileInt( "Settings", "PrintFullSize", 0 ) != 0;
		m_bShowGrid = pApp->GetProfileInt( "Settings", "ShowGrid", 0 ) != 0;
		m_bShowParts = pApp->GetProfileInt( "Settings", "ShowParts", 0 ) != 0;
		m_bAllowEdit = !m_bShowParts;
		GetSetGroundDimensionVisbility( false );
		m_bShowDimensions = pApp->GetProfileInt( "Settings", "Showdimensions", 0 ) != 0;
		m_bShowGroundDimensions = pApp->GetProfileInt( "Settings", "Showgrounddimensions", 1 ) != 0;
		m_SelectedEditLayers = (unsigned int)pApp->GetProfileInt( "Settings", "EditLayers", 0xFFFFFFFF );
		m_SelectedViewLayers = (unsigned int)pApp->GetProfileInt( "Settings", "ViewLayers", 0xFFFFFFFF );

		m_Rotate0 = pApp->LoadIcon( IDI_ICON5 );
		m_Rotate1 = pApp->LoadIcon( IDI_ICON1 );
		m_Rotate2 = pApp->LoadIcon( IDI_ICON2 );
		m_Rotate3 = pApp->LoadIcon( IDI_ICON3 );
		m_Rotate4 = pApp->LoadIcon( IDI_ICON4 );
	}

	SetupFont();

	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup( &m_gdiplusToken, &gdiplusStartupInput, 0 );

	timeBeginPeriod( 1 );
	m_TimerID = 0;
}

void CLinkageView::GetSetGroundDimensionVisbility( bool bSave )
{
	CWinApp *pApp = AfxGetApp();
	if( pApp == 0 )
		return;

	CString ShowDimensionsName = m_bShowParts ? "PartsShowdimensions" : "Showdimensions";
	CString ShowGroundDimensionsName = m_bShowParts ? "PartsShowgrounddimensions" : "Showgrounddimensions";

	// Not sure if I want to have separate dimension settings for the parts list while not having other settings be separate - it might be confusing to users.
	// Just use the one set of settings for now - and think about this for a while.
	ShowDimensionsName = "Showdimensions";
	ShowGroundDimensionsName = "Showgrounddimensions";

	if( bSave )
	{
		pApp->WriteProfileInt( "Settings", ShowDimensionsName, m_bShowDimensions ? 1 : 0  );
		pApp->WriteProfileInt( "Settings", ShowGroundDimensionsName, m_bShowGroundDimensions ? 1 : 0  );
	}
	else
	{
		m_bShowDimensions = pApp->GetProfileInt( "Settings", ShowDimensionsName, 0 ) != 0;
		m_bShowGroundDimensions = pApp->GetProfileInt( "Settings", ShowGroundDimensionsName, 1 ) != 0;
	}
}

void CLinkageView::SetupFont( void )
{
	if( m_bShowLargeFont )
	{
		m_pUsingFont = &m_MediumFont;
		m_UsingFontHeight = MEDIUM_FONT_SIZE;
	}
	else
	{
		m_pUsingFont = &m_SmallFont;
		m_UsingFontHeight = SMALL_FONT_SIZE;
	}
}

void CLinkageView::SaveSettings( void )
{
	CWinApp *pApp = AfxGetApp();
	if( pApp == 0 )
		return;

	pApp->WriteProfileInt( "Settings", "Showlabels", m_bShowLabels ? 1 : 0  );
	pApp->WriteProfileInt( "Settings", "Showangles", m_bShowAngles ? 1 : 0  );
	pApp->WriteProfileInt( "Settings", "Showdebug", m_bShowDebug ? 1 : 0  );
	pApp->WriteProfileInt( "Settings", "ShowBold", m_bShowBold ? 1 : 0  );
	pApp->WriteProfileInt( "Settings", "Showdata", m_bShowData ? 1 : 0  );
	pApp->WriteProfileInt( "Settings", "Snapon", m_bSnapOn ? 1 : 0  );
	pApp->WriteProfileInt( "Settings", "Gridsnap", m_bGridSnap ? 1 : 0  );
	pApp->WriteProfileInt( "Settings", "AutoJoin", m_bAutoJoin ? 1 : 0 );
	pApp->WriteProfileInt( "Settings", "Newlinkssolid", m_bNewLinksSolid ? 1 : 0  );
	pApp->WriteProfileInt( "Settings", "Showanicrop", m_bShowAnicrop ? 1 : 0  );
	pApp->WriteProfileInt( "Settings", "Showlargefont", m_bShowLargeFont ? 1 : 0  );
	pApp->WriteProfileInt( "Settings", "ShowGrid", m_bShowGrid ? 1 : 0  );
	pApp->WriteProfileInt( "Settings", "ShowParts", m_bShowParts ? 1 : 0  );
	pApp->WriteProfileInt( "Settings", "PrintFullSize", m_bPrintFullSize ? 1 : 0 );
	GetSetGroundDimensionVisbility( true );
	pApp->WriteProfileInt( "Settings", "EditLayers", (int)m_SelectedEditLayers  );
	pApp->WriteProfileInt( "Settings", "ViewLayers", (int)m_SelectedViewLayers );
}

CLinkageView::~CLinkageView()
{
	if( m_pPopupGallery != 0 )
		delete m_pPopupGallery;
	m_pPopupGallery = 0;

	SaveSettings();

	m_bSimulating = false;
	if( m_TimerID != 0 )
		timeKillEvent( m_TimerID );
	m_TimerID = 0;

	m_bRequestAbort = true;
	SetEvent( m_hVideoFrameEvent );
	DWORD Ticks = GetTickCount();
	static const int VIDEO_KILL_TIMEOUT = 2000;
	while( m_bProcessingVideoThread && GetTickCount() < Ticks + VIDEO_KILL_TIMEOUT )
		Sleep( 100 );

	CloseHandle( m_hVideoFrameEvent );

	if( m_pAvi != 0 )
		delete m_pAvi;

   GdiplusShutdown( m_gdiplusToken );
}

BOOL CLinkageView::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style |= WS_HSCROLL | WS_VSCROLL;
	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CLinkageView diagnostics

#ifdef _DEBUG
void CLinkageView::AssertValid() const
{
	CView::AssertValid();
}

void CLinkageView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CLinkageDoc* CLinkageView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CLinkageDoc)));
	return (CLinkageDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CLinkageView drawing

void CLinkageView::GetAdjustmentControlRect( AdjustmentControl Control, CRect &Rect )
{
	CFRect FRect;
	GetAdjustmentControlRect( Control, FRect );
	Rect.SetRect( (int)FRect.left, (int)FRect.top, (int)FRect.right, (int)FRect.bottom );
}

void CLinkageView::GetAdjustmentControlRect( AdjustmentControl Control, CFRect &Rect )
{
	CFRect PixelRect = Scale( m_SelectionContainerRect );
	PixelRect.InflateRect( 4, 4 );

	if( m_VisibleAdjustment == ADJUSTMENT_ROTATE )
	{
		switch( Control )
		{
			case AC_ROTATE:
			{
				//CPoint PixelPoint = TempDC.UnscalePoint( m_SelectionRotatePoint );
				CFPoint PixelPoint = Scale( m_SelectionRotatePoint );
				Rect.SetRect( PixelPoint.x - 7, PixelPoint.y - 7, PixelPoint.x + 7, PixelPoint.y + 7 );
				break;
			}
			case AC_TOP_LEFT:
				Rect.SetRect( PixelRect.left - 11, PixelRect.top - 11, PixelRect.left, PixelRect.top );
				break;
			case AC_TOP_RIGHT:
				Rect.SetRect( PixelRect.right, PixelRect.top - 11, PixelRect.right + 11, PixelRect.top );
				break;
			case AC_BOTTOM_RIGHT:
				Rect.SetRect( PixelRect.right, PixelRect.bottom, PixelRect.right + 11, PixelRect.bottom + 11 );
				break;
			case AC_BOTTOM_LEFT:
				Rect.SetRect( PixelRect.left - 11, PixelRect.bottom, PixelRect.left, PixelRect.bottom + 11 );
				break;
			default: return;
		}
	}
	else if( m_VisibleAdjustment == ADJUSTMENT_STRETCH )
	{
		switch( Control )
		{
			case AC_TOP_LEFT:
				Rect.SetRect( PixelRect.left - 6, PixelRect.top - 6, PixelRect.left, PixelRect.top );
				break;
			case AC_TOP_RIGHT:
				Rect.SetRect( PixelRect.right, PixelRect.top - 6, PixelRect.right + 6, PixelRect.top );
				break;
			case AC_BOTTOM_RIGHT:
				Rect.SetRect( PixelRect.right, PixelRect.bottom, PixelRect.right + 6, PixelRect.bottom + 6 );
				break;
			case AC_BOTTOM_LEFT:
				Rect.SetRect( PixelRect.left - 6, PixelRect.bottom, PixelRect.left, PixelRect.bottom + 6 );
				break;
			case AC_TOP:
				Rect.SetRect( ( ( PixelRect.left + PixelRect.right ) / 2 ) - 3,
							  PixelRect.top - 6,
							  ( ( PixelRect.left + PixelRect.right ) / 2 ) + 3,
							  PixelRect.top );
				break;
			case AC_RIGHT:
				Rect.SetRect( PixelRect.right,
							  ( ( PixelRect.top + PixelRect.bottom ) / 2 ) - 3,
							  PixelRect.right + 6,
							  ( ( PixelRect.top + PixelRect.bottom ) / 2 ) + 3 );
				break;
			case AC_BOTTOM:
				Rect.SetRect( ( ( PixelRect.left + PixelRect.right ) / 2 ) - 3,
							  PixelRect.bottom,
							  ( ( PixelRect.left + PixelRect.right ) / 2 ) + 3,
							  PixelRect.bottom + 6 );
				break;
			case AC_LEFT:
				Rect.SetRect( PixelRect.left - 6,
							  ( ( PixelRect.top + PixelRect.bottom ) / 2 ) - 3,
							  PixelRect.left,
							  ( ( PixelRect.top + PixelRect.bottom ) / 2 ) + 3 );
				break;
			default: return;
		}
	}
}

void CLinkageView::DrawAdjustmentControls( CRenderer *pRenderer )
{
	if( !m_bAllowEdit )
		return;

	CDC *pDC = pRenderer->GetDC();

	if( m_VisibleAdjustment == ADJUSTMENT_ROTATE )
	{
		if( m_MouseAction != ACTION_NONE && m_MouseAction != ACTION_RECENTER /*&& m_MouseAction != ACTION_ROTATE*/ )
			return;

		CFRect Rect;
		GetAdjustmentControlRect( AC_ROTATE, Rect );
		::DrawIconEx( pDC->GetSafeHdc(), (int)Rect.left, (int)Rect.top, m_Rotate0, 32, 32, 0, 0, DI_NORMAL );
		// pDC->DrawIcon( (int)Rect.left, (int)Rect.top, m_Rotate0 );

		if( m_MouseAction != ACTION_NONE )
			return;

		GetAdjustmentControlRect( AC_TOP_LEFT, Rect );
		::DrawIconEx(pDC->GetSafeHdc(), (int)Rect.left, (int)Rect.top, m_Rotate1, 32, 32, 0, 0, DI_NORMAL);
		//pDC->DrawIcon( (int)Rect.left, (int)Rect.top, m_Rotate1 );
		GetAdjustmentControlRect( AC_TOP_RIGHT, Rect );
		::DrawIconEx(pDC->GetSafeHdc(), (int)Rect.left, (int)Rect.top, m_Rotate2, 32, 32, 0, 0, DI_NORMAL);
		//pDC->DrawIcon( (int)Rect.left, (int)Rect.top, m_Rotate2 );
		GetAdjustmentControlRect( AC_BOTTOM_RIGHT, Rect );
		::DrawIconEx(pDC->GetSafeHdc(), (int)Rect.left, (int)Rect.top, m_Rotate3, 32, 32, 0, 0, DI_NORMAL);
		//pDC->DrawIcon( (int)Rect.left, (int)Rect.top, m_Rotate3 );
		GetAdjustmentControlRect( AC_BOTTOM_LEFT, Rect );
		::DrawIconEx(pDC->GetSafeHdc(), (int)Rect.left, (int)Rect.top, m_Rotate4, 32, 32, 0, 0, DI_NORMAL);
		//pDC->DrawIcon( (int)Rect.left, (int)Rect.top, m_Rotate4 );
	}
	else if( m_VisibleAdjustment == ADJUSTMENT_STRETCH )
	{
		if( m_MouseAction != ACTION_NONE )
			return;

		bool bVertical = fabs( m_SelectionContainerRect.Height() ) > 0;
		bool bHorizontal = fabs( m_SelectionContainerRect.Width() ) > 0;
		bool bBoth = bHorizontal && bVertical;

		CRect Rect;

		CBrush Brush( m_bAllowEdit ? COLOR_ADJUSTMENTKNOBS : COLOR_NONADJUSTMENTKNOBS );
		GetAdjustmentControlRect( AC_TOP_LEFT, Rect );
		Rect.right++; Rect.bottom++;
		if( bBoth ) pDC->FillRect( &Rect, &Brush );
		GetAdjustmentControlRect( AC_TOP_RIGHT, Rect );
		Rect.right++; Rect.bottom++;
		if( bBoth ) pDC->FillRect( &Rect, &Brush );
		GetAdjustmentControlRect( AC_BOTTOM_RIGHT, Rect );
		Rect.right++; Rect.bottom++;
		if( bBoth ) pDC->FillRect( &Rect, &Brush );
		GetAdjustmentControlRect( AC_BOTTOM_LEFT, Rect );
		Rect.right++; Rect.bottom++;
		if( bBoth ) pDC->FillRect( &Rect, &Brush );
		GetAdjustmentControlRect( AC_TOP, Rect );
		Rect.right++; Rect.bottom++;
		if( bVertical ) pDC->FillRect( &Rect, &Brush );
		GetAdjustmentControlRect( AC_RIGHT, Rect );
		Rect.right++; Rect.bottom++;
		if( bHorizontal ) pDC->FillRect( &Rect, &Brush );
		GetAdjustmentControlRect( AC_BOTTOM, Rect );
		Rect.right++; Rect.bottom++;
		if( bVertical ) pDC->FillRect( &Rect, &Brush );
		GetAdjustmentControlRect( AC_LEFT, Rect );
		Rect.right++; Rect.bottom++;
		if( bHorizontal ) pDC->FillRect( &Rect, &Brush );
	}
}

int CLinkageView::GetPrintPageCount( CDC *pDC, CPrintInfo *pPrintInfo, bool bPrintActualSize )
{
	if( !bPrintActualSize )
		return 1;

	int cx = pDC->GetDeviceCaps(HORZRES);
	int cy = pDC->GetDeviceCaps(VERTRES);

	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	static const double MyLogicalPixels = CLinkageDoc::GetBaseUnitsPerInch();

	int PixelsPerInch = pDC->GetDeviceCaps( LOGPIXELSX );
	double ScaleFactor = MyLogicalPixels / PixelsPerInch;

	CRect DrawingRect;
	DrawingRect.SetRect( 0, 0, (int)( cx * ScaleFactor ), (int)( cy * ScaleFactor ) );

	CFRect Area = GetDocumentArea();
	Area.Normalize();
	CFRect PixelRect = Area;

	//PixelRect = Scale( PixelRect );

	int WidthInPages = (int)( ( ( PixelRect.Width() / ScaleFactor ) / cx ) + 0.999999999999999999 );
	int HeightInPages = (int)( ( ( PixelRect.Height() / ScaleFactor ) / cy ) + 0.999999999999999999 );

	return WidthInPages * HeightInPages;
}

CRect CLinkageView::PrepareRenderer( CRenderer &Renderer, CRect *pDrawRect, CBitmap *pBitmap, CDC *pDC, double ScalingValue, bool bScaleToFit, double MarginScale, double UnscaledUnitSize, bool bForScreen, bool bAntiAlias, bool bActualSize, int PageNumber )
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	bool bEmpty = pDoc->IsEmpty();
	pDoc = 0; // Don't use this anymore after this.

	m_YUpDirection = ( Renderer.GetYOrientation() < 0 ) ? -1 : 1;
	m_BaseUnscaledUnit = UnscaledUnitSize;
	m_ConnectorRadius = CONNECTORRADIUS * UnscaledUnitSize;
	m_ConnectorTriangle = CONNECTORTRIANGLE * UnscaledUnitSize;
	m_ArrowBase = ANCHORBASE * UnscaledUnitSize;

	if( pDrawRect != 0 )
		m_DrawingRect = *pDrawRect;

	m_bShowSelection = bForScreen;

	m_Zoom = 1.0;
	m_ScrollPosition.SetPoint( 0, 0 );

    if( Renderer.IsPrinting() )
    {
		ASSERT( pDC != 0 );

		m_bShowSelection = false;

		m_Zoom = 1;
		m_ScrollPosition.SetPoint( 0, 0 );
		Renderer.Attach( pDC->GetSafeHdc() );
		Renderer.SetAttribDC( pDC->m_hAttribDC );

		int cx = pDC->GetDeviceCaps(HORZRES);
		int cy = pDC->GetDeviceCaps(VERTRES);

		/*
		 * There is a bug in the CRenderer code that make pen handling fail
		 * at unexpected times. The CRenderer code would try to adjust the
		 * pen width for a specified pen to account for the scaling but
		 * the arrow head drawing code makes this fail intermittently.
		 * Because of all of that, the only way to get proper printing
		 * is to use the window and viewport feature and not use the scaling
		 * feature of CRenderer.
		 */

		static const double MyLogicalPixels = CLinkageDoc::GetBaseUnitsPerInch();

		int PixelsPerInch = pDC->GetDeviceCaps( LOGPIXELSX );
		double ScaleFactor = MyLogicalPixels / PixelsPerInch;

		/*
		 * The detail scale allows the drawing functions to draw to a much larger area and the window/viewport
		 * scaling combined with the device context scaling makes it the correct size for the page. This has
		 * the advantage of allowing a higher resolution to be drawn to than the document 96 DPI while still
		 * making fonts, lines, arrowheads, all look correct on the page.
		 */
		static double DETAILSCALE = 8.0;

		if( pDrawRect == 0 )
			m_DrawingRect.SetRect( 0, 0, (int)( cx * ScaleFactor ), (int)( cy * ScaleFactor ) );

		Renderer.SetScale( DETAILSCALE );

		CFRect Area = GetDocumentArea();
		Area.Normalize();
		CFRect PixelRect = Area;

		pDC->SetMapMode( MM_ISOTROPIC );
		pDC->SetWindowExt( (int)( m_DrawingRect.Width() * DETAILSCALE ), (int)( m_DrawingRect.Height() * DETAILSCALE ) );
		pDC->SetViewportExt( cx, cy );

		/*
		 * Mechanisms are drawn actual size. The view zoom lets the user do this but printing
		 * does not use the screen Unscale. Shrink the drawing to fit on the page if it is too big.
		 */
		double ExtraScaling = 1.0;
		// Get a usable area that has an extra 1/2 margin around it.
		double xUsable = m_DrawingRect.Width() - MyLogicalPixels;
		double yUsable = m_DrawingRect.Height() - MyLogicalPixels;
		if( !bActualSize && ( PixelRect.Width() > xUsable || PixelRect.Height() > yUsable ) )
			ExtraScaling = min( yUsable / PixelRect.Height(), xUsable / PixelRect.Width() );

		if( ExtraScaling < 1.0 )
			m_Zoom = ExtraScaling;
		PixelRect = Scale( PixelRect );

		if( bActualSize )
		{
			int WidthInPages = (int)( ( ( PixelRect.Width() / ScaleFactor ) / cx ) + 0.999999999999999999 );
			int HeightInPages = (int)( ( ( PixelRect.Height() / ScaleFactor ) / cy ) + 0.999999999999999999 );

			int HorizontalPage = PageNumber % WidthInPages;
			int VerticalPage = PageNumber / WidthInPages;

			m_ScrollPosition.x = PixelRect.left + ( m_DrawingRect.Width() * HorizontalPage );
			m_ScrollPosition.y = PixelRect.top + ( m_DrawingRect.Height() * VerticalPage );
		}
		else
		{
			m_ScrollPosition.x = (int)( PixelRect.left + ( PixelRect.Width() / 2 ) - ( m_DrawingRect.Width() / 2 ) );
			m_ScrollPosition.y = (int)( PixelRect.top + ( PixelRect.Height() / 2 ) - ( m_DrawingRect.Height() / 2 ) );
		}
	}
	else
	{
		if( pDrawRect == 0 )
			GetClientRect( &m_DrawingRect );

		if( pBitmap != 0 && pDC != 0 )
		{
			Renderer.CreateCompatibleDC( pDC, &m_DrawingRect );
			pBitmap->CreateCompatibleBitmap( pDC, (int)( m_DrawingRect.Width() * ScalingValue ), (int)( m_DrawingRect.Height() * ScalingValue ) );
			Renderer.SelectObject( pBitmap );
			Renderer.PatBlt( 0, 0, (int)( m_DrawingRect.Width() * ScalingValue ), (int)( m_DrawingRect.Height() * ScalingValue ), WHITENESS );
		}

		Renderer.SetOffset( 0, 0 );
		Renderer.SetScale( ScalingValue );

		if( bScaleToFit )
		{
			if( bEmpty )
			{
				m_Zoom = 1;
				m_ScrollPosition.x = 0;
				m_ScrollPosition.y = 0;
				return m_DrawingRect;
			}

			int cx = m_DrawingRect.Width();
			int cy = m_DrawingRect.Height();

			int xMargin = (int)( cx * MarginScale );
			int yMargin = (int)( cy * MarginScale );

			// Some margin because the dimensions are not done well and we can't predict them just yet.
			cx -= xMargin;
			cy -= yMargin;

			CFRect Area = GetDocumentArea();
			Area.Normalize();

			double xScaleChange = cx / Area.Width();
			double yScaleChange = cy / Area.Height();

			double ScaleValue = min( xScaleChange, yScaleChange );

			// But screen zoom needs to one of the "standard" zoom
			// levels that a user would see if they just scrolled in and out.
// 			m_Zoom = pow( ZOOM_AMOUNT, floor( log( ScaleValue ) / log( ZOOM_AMOUNT ) ) );

			m_Zoom = ScaleValue;

			//m_ConnectorRadius

			CFRect NewArea = GetDocumentArea( false );
			NewArea.Normalize();

			// 15 times is enought to get the chage ratio up to .99995 during testing.
			for( int Test = 0; Test < 15; ++Test )
			{
				// Zoom out to make room for dimensions. This is not exact because the zoom affects the dimension line sizes and positions.
				CFRect BigArea = GetDocumentArea( m_bShowDimensions );
				BigArea.Normalize();

				BigArea.left -= Unscale( 1 );
				BigArea.right += Unscale( 2 );
				BigArea.top -= Unscale( 1 );
				BigArea.bottom += Unscale( 2 );

				BigArea.left -= Unscale( m_ConnectorRadius );
				BigArea.right += Unscale( m_ConnectorRadius );
				BigArea.top -= Unscale( m_ConnectorRadius );
				BigArea.bottom += Unscale( m_ConnectorRadius );

				double xRatio = NewArea.Width() / BigArea.Width();
				double yRatio = NewArea.Height() / BigArea.Height();
				double Ratio = max( xRatio, yRatio );

				m_Zoom *= Ratio;

				NewArea = BigArea;
			}

			Area = GetDocumentArea( m_bShowDimensions );
			Area.Normalize();

			// Center the mechanism.
			m_ScrollPosition.x = m_Zoom * ( Area.left + Area.Width() / 2 );
			m_ScrollPosition.y = m_Zoom * -( Area.top + Area.Height() / 2 );
		}
		else if( bForScreen )
		{
			m_Zoom = m_ScreenZoom;
			m_ScrollPosition = m_ScreenScrollPosition;

			//int cx = m_DrawingRect.Width();
			//int cy = m_DrawingRect.Height();
			//pDC->SetMapMode(MM_ANISOTROPIC);
			//pDC->SetWindowExt((int)(m_DrawingRect.Width()), (int)(m_DrawingRect.Height()));
			//pDC->SetViewportExt((int)( cx * 2 ), (int)(cy*2));
		}
	}

	return m_DrawingRect;
}

class CConnectorTextData : public ConnectorListOperation
{
	public:
	CString &m_Text;
	bool m_bShowDebug;
	CConnectorTextData( CString &Text, bool bShowDebug ) : m_Text( Text ), m_bShowDebug( bShowDebug )
	{
	}

	bool operator()( class CConnector *pConnector, bool bFirst, bool bLast )
	{
		CString TempString;
		if( m_bShowDebug )
			TempString.Format( "      Connector %s [%d]: %.4lf, %.4lf\n",
							   (const char*)pConnector->GetIdentifierString( m_bShowDebug ), pConnector->GetIdentifier(),
							   pConnector->GetPoint().x, pConnector->GetPoint().y );
		else
			TempString.Format( "      Connector %s: %.4lf, %.4lf\n",
							   (const char*)pConnector->GetIdentifierString( m_bShowDebug ),
							   pConnector->GetPoint().x, pConnector->GetPoint().y );
		m_Text += TempString;
		return true;
	}
};

class CConnectorDistanceData : public ConnectorListOperation
{
	public:
	CString &m_Text;
	CConnector *m_pLastConnector;
	CConnectorDistanceData( CString &Text, CConnector *pLastConnector ) : m_Text( Text ) { m_pLastConnector = pLastConnector; }
	bool operator()( class CConnector *pConnector, bool bFirst, bool bLast )
	{
		if( m_pLastConnector != 0 )
		{
			double Dist = Distance( m_pLastConnector->GetPoint(), pConnector->GetPoint() );
			CString TempString;
			TempString.Format( "      Distance %s%s %.4lf\n",
							   (const char*)m_pLastConnector->GetIdentifierString( false ), (const char*)pConnector->GetIdentifierString( false ), Dist );
			m_Text += TempString;
		}
		m_pLastConnector = pConnector;
		return true;
	}
};

void CLinkageView::DrawData( CRenderer *pRenderer )
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	/*
	 * This data information is never scaled or offset.
	 */
	CFont *pOriginalFont = pRenderer->SelectObject( m_pUsingFont, UnscaledUnits( m_UsingFontHeight ) );

	pRenderer->SetTextAlign( TA_TOP | TA_LEFT );

	static const int x = 10;
	static const int x2 = 30;
	double y = 10;
	double dy = pRenderer->GetTextExtent( "X", 1 ).y - 1;

	CString Text;

	LinkList* pLinkList = pDoc->GetLinkList();
	POSITION Position = pLinkList->GetHeadPosition();
	while( Position != NULL )
	{
		CLink* pLink = pLinkList->GetNext( Position );
		if( pLink == NULL )
			continue;

		CString TempString;
		if( m_bShowDebug )
			TempString.Format( "Link %s [%d]\n", (const char*)pLink->GetIdentifierString( m_bShowDebug ), pLink->GetIdentifier() );
		else
			TempString.Format( "Link %s\n", (const char*)pLink->GetIdentifierString( m_bShowDebug ) );
		Text += TempString;

		CConnectorTextData FormatText( Text, m_bShowDebug );
		pLink->GetConnectorList()->Iterate( FormatText );

		CConnectorDistanceData FormatDistance( Text, pLink->GetConnectorList()->GetCount() <= 2 ? 0 : pLink->GetConnectorList()->GetTail() );
		pLink->GetConnectorList()->Iterate( FormatDistance );
	}

	char *pStart = Text.GetBuffer();
	for(;;)
	{
		char *pEnd = strrchr( pStart, '\n' );
		if( pEnd != 0 )
			*pEnd = '\0';
		pRenderer->TextOut( x, y, pStart );
		y += dy;
		if( pEnd == 0 )
			break;
		pStart = pEnd + 1;
	}
	Text.ReleaseBuffer( 0 );

	y += dy;

	/*
	 * Objects selected before we changed things need to be selected back
	 * in the device context before we reset the scale to non-zero.
	 * If we did this after the scale is set, the object would get scaled
	 * more than once.
	 */
	pRenderer->SelectObject( pOriginalFont, UnscaledUnits( m_UsingFontHeight ) );
}

void CLinkageView::DrawAlignmentLines( CRenderer *pRenderer )
{
	if( m_MouseAction != ACTION_NONE )
		return;

	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if( pDoc->GetAlignConnectorCount() < 2 )
		return;

	int PointCount = pDoc->GetAlignConnectorCount();

	COLORREF Color = COLOR_ALIGNMENTHINT;
	CPen DotPen( PS_DOT, 1, Color );
	CPen * pOldPen = pRenderer->SelectObject( &DotPen );

	CFPoint Points[4];

	CConnector *pConnector = (CConnector*)pDoc->GetSelectedConnector( 0 );
	if( pConnector == 0 )
		return;
	Points[0] = pConnector->GetPoint();
	pRenderer->MoveTo( Scale( Points[0] ) );

	// Draw only a single line if there are too many points for a rectangle/parallelogram.
	if( PointCount > 4 )
	{
		for( int Counter = 1; Counter < PointCount; ++Counter )
		{
			pConnector = (CConnector*)pDoc->GetSelectedConnector( Counter );
			if( pConnector == 0 )
				return;
			Points[1] = pConnector->GetPoint();
		}

		pRenderer->LineTo( Scale( Points[1] ) );

		return;
	}

	for( int Counter = 1; Counter < PointCount; ++Counter )
	{
		pConnector = (CConnector*)pDoc->GetSelectedConnector( Counter );
		if( pConnector == 0 )
			return;
		Points[Counter] = pConnector->GetPoint();
		pRenderer->LineTo( Scale( Points[Counter] ) );
	}
	// Finish drawing the polygon when it is a rectangle.
	if( PointCount == 4 )
		pRenderer->LineTo( Scale( Points[0] ) );

	double Angle = GetAngle( Points[1], Points[0], Points[2] );

	if( PointCount == 3 ) // && fabs( Angle ) >= 0.5 && fabs( Angle ) < 359.5 )
	{
		CFPoint TempPoint;
		TempPoint = Scale( Points[0] );
		Points[0].SetPoint( CPoint( (int)TempPoint.x, (int)TempPoint.y ) );
		TempPoint = Scale( Points[1] );
		Points[1].SetPoint( CPoint( (int)TempPoint.x, (int)TempPoint.y ) );
		TempPoint = Scale( Points[2] );
		Points[2].SetPoint( CPoint( (int)TempPoint.x, (int)TempPoint.y ) );

		double ArcRadius = 20;
		CFLine Line( Points[1], Points[0] );
		Line.SetDistance( ArcRadius );
		Points[0] = Line[1];

		Line.SetLine( Points[1], Points[2] );
		Line.SetDistance( ArcRadius );
		Points[2] = Line[1];

		CPen Pen( PS_SOLID, 1, Color );

		pRenderer->SelectObject( &Pen );

		if( Angle <= 180 )
			pRenderer->Arc( Points[1].x, Points[1].y, ArcRadius, Points[2].x, Points[2].y, Points[0].x, Points[0].y );
		else
			pRenderer->Arc( Points[1].x, Points[1].y, ArcRadius, Points[2].x, Points[2].y, Points[0].x, Points[0].y );
			//pRenderer->Arc( Points[1].x, Points[1].y, ArcRadius, Points[0].x, Points[0].y, Points[2].x, Points[2].y );
	}

	pRenderer->SelectObject( pOldPen );
}

void CLinkageView::DrawSnapLines( CRenderer *pRenderer )
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CFLine SnapLine1;
	CFLine SnapLine2;
	pDoc->GetSnapLines( SnapLine1, SnapLine2 );

	if( SnapLine1.GetStart() == SnapLine1.GetEnd()
		&& SnapLine2.GetStart() == SnapLine2.GetEnd() )
		return;

	COLORREF Color = COLOR_SNAPHINT;

	CPen DotPen( PS_DOT, 1, Color );

	CPen * pOldPen = pRenderer->SelectObject( &DotPen );

	CRect ClientRect;
	GetClientRect( &ClientRect );
	//CFRect Viewport = pRenderer->ScaleRect( ClientRect );
	CFRect Viewport = Unscale( ClientRect );

	if( SnapLine1.m_Start.x == -99999 )
		SnapLine1.m_Start.x = Viewport.left;
	if( SnapLine1.m_Start.y == -99999 )
		SnapLine1.m_Start.y = Viewport.top;
	if( SnapLine1.m_End.x == -99999 )
		SnapLine1.m_End.x = Viewport.left;
	if( SnapLine1.m_End.y == -99999 )
		SnapLine1.m_End.y = Viewport.top;

	if( SnapLine1.GetStart() != SnapLine1.GetEnd() )
	{
		pRenderer->DrawLine( Scale( SnapLine1 ) );
		//pRenderer->MoveTo( Scale( SnapLine1.GetStart() ) );
		//pRenderer->LineTo( Scale( SnapLine1.GetEnd() ) );
	}
	if( SnapLine2.GetStart() != SnapLine2.GetEnd() )
	{
		pRenderer->DrawLine( Scale( SnapLine2 ) );
		//pRenderer->MoveTo( Scale( SnapLine2.GetStart() ) );
		//pRenderer->LineTo( Scale( SnapLine2.GetEnd() ) );
	}

	pRenderer->SelectObject( pOldPen );
}

CFRect CLinkageView::GetDocumentAdjustArea( bool bSelectedOnly )
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CFRect SelectRect;
	pDoc->GetDocumentAdjustArea( SelectRect, bSelectedOnly );

	// Dimensions are not included because they are not part of ahything that can be selected.
	return SelectRect;
}

CFRect CLinkageView::GetDocumentArea( bool bWithDimensions, bool bSelectedOnly )
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CFRect Rect;
	pDoc->GetDocumentArea( Rect, bSelectedOnly );

//	if( !bWithDimensions )
//		return Rect;

	CFArea DocumentArea = Rect;

	/*
	 * The following code is almost useful. It does not work for exporting images because
	 * the scale of the image is determined after the document area is obtained, but the
	 * code below uses the scale to figure out a new document area. In other words, the
	 * code cannot use the document area to pick a scale if the scale is needed to get the
	 * document area!
	 *
	 * An alternative might be to use the document area without the dimension lines and then
	 * increase the image size to allow room for the dimensions to fit. Then the image export
	 * might be able to use this, as would the printing process.
	 */

	DocumentArea.Normalize();

	CRenderer NullRenderer( CRenderer::NULL_RENDERER );
	if( m_bShowParts )
	{
		bool bTemp = m_bShowDimensions;
//		m_bShowDimensions = bWithDimensions;
		DocumentArea = DrawPartsList( &NullRenderer );
//		m_bShowDimensions = bTemp;d
	}
	else
	{
		if( !bSelectedOnly )
		{
			double SolidLinkCornerRadius = Unscale( CONNECTORRADIUS + 3 );
			DocumentArea.InflateRect( SolidLinkCornerRadius, SolidLinkCornerRadius );

			LinkList* pLinkList = pDoc->GetLinkList();
			POSITION Position = pLinkList->GetHeadPosition();
			while( Position != NULL )
			{
				CLink* pLink = pLinkList->GetNext( Position );
				if( pLink != 0 && !pLink->IsMeasurementElement() )
				{
					pLink->ComputeHull();
					DocumentArea += DrawDimensions( &NullRenderer, pDoc->GetGearConnections(), m_SelectedViewLayers, pLink, true, true );
				}
			}

			ConnectorList* pConnectors = pDoc->GetConnectorList();
			Position = pConnectors->GetHeadPosition();
			while( Position != NULL )
			{
				CConnector* pConnector = pConnectors->GetNext( Position );
				if( pConnector != 0 )
					DocumentArea += DrawDimensions( &NullRenderer, m_SelectedViewLayers, pConnector, true, true );
			}

			DocumentArea += DrawGroundDimensions( &NullRenderer, pDoc, m_SelectedViewLayers, 0, true, true );
		}
	}
	DocumentArea.InflateRect( Unscale( m_ConnectorRadius * 2 ), Unscale( m_ConnectorRadius * 2 ) );
	return DocumentArea.GetRect();
}

CFArea CLinkageView::DoDraw( CRenderer* pRenderer )
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CFArea Area;

	if( m_bShowDebug )
	{
		// As drawn
		CFRect Derfus = GetDocumentArea( true ) ;
		CPen aPen( PS_SOLID, 1, RGB( 90, 255, 90 ) );
		CPen *pOldPen = pRenderer->SelectObject( &aPen );
		pRenderer->DrawRect( Scale( Derfus ) );
		pRenderer->SelectObject( pOldPen );

		// Original document
		pDoc->GetDocumentArea( Derfus );
		CPen xxxPen( PS_SOLID, 1, RGB( 0, 180, 0 ) );
		pOldPen = pRenderer->SelectObject( &xxxPen );
		pRenderer->DrawRect( Scale( Derfus ) );
		pRenderer->SelectObject( pOldPen );
	}

	if( m_bShowParts )
		Area = DrawPartsList( pRenderer );
	else
		Area = DrawMechanism( pRenderer );

	return Area;
}

double roundUp( double number, double fixedBase )
{
    if (fixedBase != 0 && number != 0)
	{
        double sign = number > 0 ? 1 : -1;
        number *= sign;
        number /= fixedBase;
        int fixedPoint = (int) ceil(number);
        number = fixedPoint * fixedBase;
        number *= sign;
    }
    return number;
}

double roundDown( double number, double fixedBase )
{
    if (fixedBase != 0 && number != 0)
	{
        double sign = number > 0 ? 1 : -1;
        number *= sign;
        number /= fixedBase;
        int fixedPoint = (int) floor(number);
        number = fixedPoint * fixedBase;
        number *= sign;
    }
    return number;
}

void CLinkageView::DrawGrid( CRenderer* pRenderer )
{
	if( pRenderer->IsPrinting() )
		return;

	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	double GapDistance = CalculateDefaultGrid();

#if 0
	double GapDistance = Unscale( 20 ) * pDoc->GetUnitScale();

	// Get the next 1, 5, 10, 50, 100 type of number that is above the
	// distance we got using a base pixel length. This will be the value
	// to use for the snap.
	int Test = (int)( GapDistance * 1000 );
	int Adjustment = -4;
	while( Test != 0 )
	{
		++Adjustment;
		Test /= 10;
	}
	double NewValue = GapDistance / pow( 10.0, Adjustment );
	NewValue += .9999;
	NewValue = (int)( NewValue );
	if( NewValue > 5.0 )
		NewValue = 10;
	else if( NewValue > 1.0 )
		NewValue = 5;
	GapDistance = NewValue * pow( 10.0, Adjustment );
	if( GapDistance < 0.01 )
		GapDistance = 0.01;
#endif

	GapDistance /= pDoc->GetUnitScale();

	CFPoint DrawStartPoint = Unscale( CFPoint( m_DrawingRect.left, m_DrawingRect.top ) );
	CFPoint DrawEndPoint = Unscale( CFPoint( m_DrawingRect.right, m_DrawingRect.bottom ) );
	CFPoint GridStartPoint = DrawStartPoint;

	GridStartPoint.x = roundDown( GridStartPoint.x, GapDistance );
	GridStartPoint.y = roundDown( GridStartPoint.y, GapDistance );

	COLORREF Color = COLOR_GRID;
	CPen Pen( PS_SOLID, 1, Color );
	CPen *pOldPen = pRenderer->SelectObject( &Pen );

	for( double Step = GridStartPoint.x; Step < DrawEndPoint.x; Step += GapDistance )
	{
		pRenderer->MoveTo( Scale( CFPoint( Step, DrawStartPoint.y ) ) );
		pRenderer->LineTo( Scale( CFPoint( Step, DrawEndPoint.y ) ) );
	}

	// Y always goes from high values up to low values down.
	for( double Step = GridStartPoint.y; Step > DrawEndPoint.y; Step -= GapDistance )
	{
		pRenderer->MoveTo( Scale( CFPoint( DrawStartPoint.x, Step ) ) );
		pRenderer->LineTo( Scale( CFPoint( DrawEndPoint.x, Step ) ) );
	}

	pRenderer->SelectObject( pOldPen );
}

void CLinkageView::ClearDebugItems( void )
{
	while( true )
	{
		CDebugItem *pDebugItem = DebugItemList.Pop();
		if( pDebugItem == 0 )
			break;
	}
}

void CLinkageView::DrawDebugItems( CRenderer *pRenderer )
{
	CPen Pen( PS_SOLID, 1, RGB( 255, 0, 0 ) );
	CPen *pOldPen = pRenderer->SelectObject( &Pen );

	pRenderer->SetTextAlign( TA_LEFT | TA_TOP );
	pRenderer->SetTextColor( COLOR_TEXT );
	CString Label;
	CFPoint LabelPoint;
	int Count = 0;
	POSITION Position = DebugItemList.GetHeadPosition();
	while( Position != 0 )
	{
		CDebugItem *pDebugItem = DebugItemList.GetNext( Position );
		if( pDebugItem == 0 )
			break;

		Label.Format( "DEBUG %d", Count );

		if( pDebugItem->m_Type == pDebugItem->DEBUG_OBJECT_POINT )
		{
			CFPoint Point = pDebugItem->m_Point;
			LabelPoint.SetPoint( Point.x + ( m_ConnectorRadius * 2 ), Point.y - ( m_ConnectorRadius * 2 ) );
			Point = Scale( Point );
			pRenderer->Arc( Point.x - m_ConnectorRadius * 2, Point.y - m_ConnectorRadius * 2, Point.x + m_ConnectorRadius * 2, Point.y + m_ConnectorRadius * 2,
				            Point.x, Point.y - m_ConnectorRadius * 2, Point.x, Point.y - m_ConnectorRadius * 2 );
		}
		else if( pDebugItem->m_Type == pDebugItem->DEBUG_OBJECT_LINE )
		{
			CFLine Line = pDebugItem->m_Line;
			pRenderer->DrawLine( Scale( Line ) );
			LabelPoint.SetPoint( Line.m_Start.x + ( m_ConnectorRadius * 2 ), Line.m_Start.y - ( m_ConnectorRadius * 2 ) );
		}
		else if( pDebugItem->m_Type == pDebugItem->DEBUG_OBJECT_CIRCLE )
		{
			CFCircle Circle = pDebugItem->m_Circle;
			LabelPoint.SetPoint( Circle.x + ( m_ConnectorRadius * 2 ), Circle.y - ( m_ConnectorRadius * 2 ) );
			Circle = Scale( Circle );
			CBrush *pSaveBrush = (CBrush*)pRenderer->SelectStockObject( NULL_BRUSH );
			pRenderer->Circle( Circle );
			pRenderer->SelectObject( pSaveBrush );
		}
		else if( pDebugItem->m_Type == pDebugItem->DEBUG_OBJECT_ARC )
		{
			CFArc Arc = pDebugItem->m_Arc;
			LabelPoint.SetPoint( Arc.x + ( m_ConnectorRadius * 2 ), Arc.y - ( m_ConnectorRadius * 2 ) );
			Arc = Scale( Arc );
			CBrush *pSaveBrush = (CBrush*)pRenderer->SelectStockObject( NULL_BRUSH );
			pRenderer->Arc( Arc );
			pRenderer->SelectObject( pSaveBrush );
		}
		LabelPoint = Scale( LabelPoint );
		pRenderer->TextOut(LabelPoint.x, LabelPoint.y, Label );
		++Count;
	}

	pRenderer->SelectObject( pOldPen );
}

CFArea CLinkageView::DrawMechanism( CRenderer* pRenderer )
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if( m_bShowGrid )
		DrawGrid( pRenderer );

	if( m_bShowSelection && m_bSuperHighlight && pDoc->IsAnySelected() )
	{
		CFRect SelectRect = GetDocumentArea( false, true );

		CFRect RealRect = Scale( SelectRect );
		RealRect.InflateRect( 3, 3 );

		bool bMoreHighlight = RealRect.Width() < 30 || RealRect.Height() < 30;
		RealRect.InflateRect( 2, 2 );
		double Inflate = max( 10 - RealRect.Width(), 10 - RealRect.Height() );
		if( Inflate > 0 )
			RealRect.InflateRect( Inflate, Inflate );

		CBrush Brush( bMoreHighlight ? COLOR_SUPERHIGHLIGHT2 : COLOR_SUPERHIGHLIGHT );
		pRenderer->FillRect( &RealRect, &Brush );
	}

	pRenderer->SelectObject( m_pUsingFont, UnscaledUnits( m_UsingFontHeight ) );
	pRenderer->SetTextColor( COLOR_TEXT );

	POSITION Position = 0;

	LinkList* pLinkList = pDoc->GetLinkList();
	Position = pLinkList->GetHeadPosition();
	while( Position != NULL )
	{
		CLink* pLink = pLinkList->GetNext( Position );
		if( pLink != 0 )
			pLink->ComputeHull();
	}

	ConnectorList* pConnectors = pDoc->GetConnectorList();
	Position = pConnectors->GetHeadPosition();
	while( Position != NULL )
	{
		CConnector* pConnector = pConnectors->GetNext( Position );
		if( pConnector == 0 )
			continue;
		DebugDrawConnector( pRenderer, m_SelectedViewLayers, pConnector, m_bShowLabels );
	}

	static const int Steps = 2;
	static const int LayersOnStep[Steps] = { CLinkageDoc::DRAWINGLAYER, CLinkageDoc::MECHANISMLAYER };

	for( int Step = 0; Step < 2; ++Step )
	{
		int StepLayers = LayersOnStep[Step];
		Position = pLinkList->GetHeadPosition();
		while( Position != NULL )
		{
			CLink* pLink = pLinkList->GetNext( Position );
			if( pLink != 0  && ( pLink->GetLayers() & StepLayers ) != 0 )
				DrawLink( pRenderer, pDoc->GetGearConnections(), m_SelectedViewLayers, pLink, false, false, true );
		}
	}

	Position = pLinkList->GetHeadPosition();
	while( Position != NULL )
	{
		CLink* pLink = pLinkList->GetNext( Position );
		if( pLink != 0  )
			DebugDrawLink( pRenderer, m_SelectedViewLayers, pLink, false, true, true );
	}

	for( int Step = 0; Step < 2; ++Step )
	{
		int StepLayers = LayersOnStep[Step];

		Position = pLinkList->GetHeadPosition();
		while( Position != NULL )
		{
			CLink* pLink = pLinkList->GetNext( Position );
			if( pLink != 0 && ( pLink->GetLayers() & StepLayers ) != 0 )
				DrawLink( pRenderer, pDoc->GetGearConnections(), m_SelectedViewLayers, pLink, m_bShowLabels, true, false );
		}

		if( m_MouseAction == ACTION_STRETCH && Step == 1 )
		{
			/*
			 * Draw a box just for stretching that shows the stretch extents.
			 */

			CLinkageDoc* pDoc = GetDocument();
			ASSERT_VALID(pDoc);
			CFRect Rect = GetDocumentAdjustArea( true );
			if( fabs( Rect.Height() ) > 0 && fabs( Rect.Width() ) > 0 )
			{
				CPen Pen( PS_DOT, 1, COLOR_STRETCHDOTS );
				CPen *pOldPen = pRenderer->SelectObject( &Pen );
				pRenderer->DrawRect( Scale( Rect ) );
				pRenderer->SelectObject( pOldPen );
			}
		}

		if( Step == 1 )
		{
			Position = pConnectors->GetHeadPosition();
			while( Position != NULL )
			{
				CConnector* pConnector = pConnectors->GetNext( Position );
				if( pConnector == 0 )
					continue;

				DrawMotionPath( pRenderer, pConnector );
			}
		}

		Position = pConnectors->GetHeadPosition();
		while( Position != NULL )
		{
			CConnector* pConnector = pConnectors->GetNext( Position );
			if( pConnector != 0 && ( pConnector->GetLayers() & StepLayers ) != 0 )
				DrawSliderTrack( pRenderer, m_SelectedViewLayers, pConnector );
		}

		Position = pLinkList->GetHeadPosition();
		while( Position != NULL )
		{
			CLink* pLink = pLinkList->GetNext( Position );
			if( pLink != 0 && ( pLink->GetLayers() & StepLayers ) != 0 )
				DrawLink( pRenderer, pDoc->GetGearConnections(), m_SelectedViewLayers, pLink, m_bShowLabels, false, false );
		}

		Position = pConnectors->GetHeadPosition();
		while( Position != NULL )
		{
			CConnector* pConnector = pConnectors->GetNext( Position );
			if( pConnector != 0 && ( pConnector->GetLayers() & StepLayers ) != 0 )
				DrawConnector( pRenderer, m_SelectedViewLayers, pConnector, m_bShowLabels );
		}
	}

	Position = pDoc->GetGearConnections()->GetHeadPosition();
	while( Position != 0 )
	{
		CGearConnection *pGearConnection = pDoc->GetGearConnections()->GetNext( Position );
		if( pGearConnection == 0 )
			continue;

		if( pGearConnection->m_ConnectionType == pGearConnection->CHAIN )
			DrawChain( pRenderer, m_SelectedViewLayers, pGearConnection );
	}

	Position = pLinkList->GetHeadPosition();
	while( Position != NULL )
	{
		CLink* pLink = pLinkList->GetNext( Position );
		if( pLink != 0 )
		{
			CConnector *pConnector = pLink->GetStrokeConnector( 0 );
			if( pConnector != 0 )
				DrawConnector( pRenderer, m_SelectedViewLayers, pConnector, m_bShowLabels, false, false, false, true );
		}
	}

	DrawStackedConnectors( pRenderer, m_SelectedViewLayers );

	Position = pLinkList->GetHeadPosition();
	while( Position != NULL )
	{
		CLink* pLink = pLinkList->GetNext( Position );
		if( pLink != 0 && !pLink->IsMeasurementElement() )
			DrawDimensions( pRenderer, pDoc->GetGearConnections(), m_SelectedViewLayers, pLink, true, true );
	}

	Position = pConnectors->GetHeadPosition();
	while( Position != NULL )
	{
		CConnector* pConnector = pConnectors->GetNext( Position );
		if( pConnector != 0 )
			DrawDimensions( pRenderer, m_SelectedViewLayers, pConnector, true, true );
	}

	DrawGroundDimensions( pRenderer, pDoc, m_SelectedViewLayers, 0, true, true );

	DrawSnapLines( pRenderer );

	if( m_bShowAngles )
		DrawAlignmentLines( pRenderer );

	if( m_bShowDebug )
		DrawDebugItems( pRenderer );

	if( m_bShowSelection && pDoc->IsSelectionAdjustable() )
		DrawAdjustmentControls( pRenderer );

	if( m_bShowData && m_bShowSelection )
		DrawData( pRenderer );

	if( m_bShowSelection && m_MouseAction == ACTION_SELECT )
	{
		CPen Pen( PS_SOLID, 1, RGB( 128, 128, 255 ) );
		CPen *pOldPen = pRenderer->SelectObject( &Pen );
		pRenderer->DrawRect( m_SelectRect );
		pRenderer->SelectObject( pOldPen );
	}

	return CFArea();
}

void CLinkageView::MovePartsLinkToOrigin( CLink *pPartsLink, CFPoint Origin, GearConnectionList *pGearConnections, bool bRotateToOptimal )
{
	if( pPartsLink->GetConnectorCount() == 1 || pPartsLink->IsGear() && pGearConnections != 0 )
	{
		std::list<double> RadiusList;
		pPartsLink->GetGearRadii( *pGearConnections, RadiusList );
		if( !RadiusList.empty() )
		{
			double LargestRadius = RadiusList.back();
			CConnector *pConnector = pPartsLink->GetConnector( 0 );
			if( pConnector != 0 )
				pConnector->SetPoint( CFPoint( Origin.x + LargestRadius, Origin.y - LargestRadius ) );
		}
		return;
	}

	int HullCount = 0;
	CFPoint *pPoints = pPartsLink->GetHull( HullCount );
	int BestStartPoint = -1;
	int BestEndPoint = -1;

	CConnector *pStartConnector = 0;
	BestStartPoint = GetLongestHullDimensionLine( pPartsLink, BestEndPoint, pPoints, HullCount, &pStartConnector );
	if( BestStartPoint < 0 )
		return;

	POSITION Position = pPartsLink->GetConnectorList()->GetHeadPosition();
	while( Position != 0 )
	{
		CConnector *pConnector = pPartsLink->GetConnectorList()->GetNext( Position );
		if( pConnector == 0 )
			continue;

		if( pConnector->GetPoint() == pPoints[BestStartPoint] )
		{
			pStartConnector = pConnector;
			break;
		}
	}

	CFLine OrientationLine( pPoints[BestStartPoint], pPoints[BestEndPoint] );
	double Angle = 0;
	if( bRotateToOptimal )
		Angle = GetAngle( pPoints[BestStartPoint], pPoints[BestEndPoint] );

	CFPoint AdditionalOffset;
	Position = pPartsLink->GetConnectorList()->GetHeadPosition();
	while( Position != 0 )
	{
		CConnector *pConnector = pPartsLink->GetConnectorList()->GetNext( Position );
		if( pConnector == 0 || pConnector == pStartConnector )
			continue;

		CFPoint Point = pConnector->GetPoint();
		Point.RotateAround( pPoints[BestStartPoint], -Angle );
		pConnector->SetPoint( Point );
		if( Point.x - pPoints[BestStartPoint].x < AdditionalOffset.x )
			AdditionalOffset.x = Point.x - pPoints[BestStartPoint].x;
		if( Point.y - pPoints[BestStartPoint].y > AdditionalOffset.y )
			AdditionalOffset.y = Point.y - pPoints[BestStartPoint].y;
	}

	Position = pPartsLink->GetConnectorList()->GetHeadPosition();
	while( Position != 0 )
	{
		CConnector *pConnector = pPartsLink->GetConnectorList()->GetNext( Position );
		if( pConnector == 0 || pConnector == pStartConnector )
			continue;

		CFPoint Point = pConnector->GetPoint();
		Point -= pPoints[BestStartPoint];
		Point += Origin;
		Point -= AdditionalOffset;
		pConnector->SetPoint( Point );
	}
	Origin -= AdditionalOffset;
	pStartConnector->SetPoint( Origin );
}

CTempLink* CLinkageView::GetTemporaryGroundLink( LinkList *pDocLinks, ConnectorList *pDocConnectors, CFPoint PartOrigin )
{
	int Anchors = 0;
	POSITION Position = pDocConnectors->GetHeadPosition();
	while( Position != 0 )
	{
		CConnector *pConnector = pDocConnectors->GetNext( Position );
		if( pConnector == 0 || !pConnector->IsAnchor() )
			continue;

		++Anchors;
	}

	if( Anchors == 0 ) // Empty or non-functioning mechanism.
		return 0;

	#if 0 // Always include the ground link.
	Position = pDocLinks->GetHeadPosition();
	while( Position != 0 )
	{
		CLink *pLink = pDocLinks->GetNext( Position );
		if( pLink == 0  )
			continue;

		int FoundAnchors = pLink->GetAnchorCount();
		if( FoundAnchors > 0 && FoundAnchors == Anchors )
			return 0; // The total anchor count all on this link so there is no need for a ground link.
	}
	#endif

	// Create the ground link.

	CTempLink *pPartsLink = new CTempLink();
	pPartsLink->SetColor( COLOR_GROUND );
	pPartsLink->SetLocked( false );
	pPartsLink->SetIdentifier( -1 );
	pPartsLink->SetName( "Ground" );
	pPartsLink->SetLayers( CLinkageDoc::MECHANISMLAYER );

	Position = pDocConnectors->GetHeadPosition();
	while( Position != 0 )
	{
		CConnector *pConnector = pDocConnectors->GetNext( Position );
		if( pConnector == 0 || !pConnector->IsAnchor() )
			continue;

		CConnector *pPartsConnector = new CConnector( *pConnector );
		pPartsConnector->SetColor( pPartsLink->GetColor() );
		pPartsConnector->SetAsAnchor( false );
		pPartsConnector->SetAsInput( false );
		pPartsConnector->SetAsDrawing( false );
		pPartsLink->AddConnector( pPartsConnector );
	}

	MovePartsLinkToOrigin( pPartsLink, PartOrigin, 0, false );

	return pPartsLink;
}

CTempLink* CLinkageView::GetTemporaryPartsLink( CLink *pLink, CFPoint PartOrigin, GearConnectionList *pGearConnections )
{
	CTempLink *pPartsLink = new CTempLink( *pLink );
	// MUST REMOVE THE COPIED CONNECTORS HERE BECAUSE THEY ARE JUST POINTER COPIES!
	pPartsLink->RemoveAllConnectors();
	pPartsLink->SetLocked( false );

	POSITION Position = pLink->GetConnectorList()->GetHeadPosition();
	while( Position != 0 )
	{
		CConnector *pConnector = pLink->GetConnectorList()->GetNext( Position );
		if( pConnector == 0 )
			continue;

		CConnector *pPartsConnector = new CConnector( *pConnector );
		pPartsConnector->SetColor( pPartsLink->GetColor() );
		pPartsConnector->SetAsAnchor( false );
		pPartsConnector->SetAsInput( false );
		pPartsConnector->SetAsDrawing( false );
		pPartsLink->AddConnector( pPartsConnector );
	}

	MovePartsLinkToOrigin( pPartsLink, PartOrigin, pGearConnections );

	return pPartsLink;
}

CFArea CLinkageView::DrawPartsList( CRenderer* pRenderer )
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// get the original document area, not the one for the parts list!
	CFRect Area;
	pDoc->GetDocumentArea( Area );

	if( m_bShowGrid )
		DrawGrid( pRenderer );

	pRenderer->SelectObject( m_pUsingFont, UnscaledUnits( m_UsingFontHeight ) );
	pRenderer->SetTextColor( COLOR_TEXT );

	CFPoint StartPoint = Area.TopLeft();
	CFArea DocumentArea;
	double LastPartHeight = 0;
	double yOffset = 0.0;

	GearConnectionList *pGearConnections = pDoc->GetGearConnections();

	POSITION Position = pDoc->GetLinkList()->GetHeadPosition();
	bool bGroundLink = false;
	for( ; !bGroundLink; ( Position == 0 ? 0 : pDoc->GetLinkList()->GetNext( Position ) ) )
	{
		CLink *pPartsLink = 0;
		if( Position == 0 )
		{
			if( !m_bShowGroundDimensions )
				break;
			/*
			 * Generate a ground link for this last pass through the data.
			 */

			bGroundLink = true;
			pPartsLink = GetTemporaryGroundLink( pDoc->GetLinkList(), pDoc->GetConnectorList(), CFPoint( StartPoint.x, StartPoint.y - yOffset - LastPartHeight ) );
		}
		else
		{
			CLink *pLink = pDoc->GetLinkList()->GetAt( Position );
			if( pLink == 0 || ( pLink->GetLayers() & CLinkageDoc::DRAWINGLAYER ) != 0 )
				continue;

			if( pLink->GetConnectorCount() <= 1 && !pLink->IsGear() )
				continue;

			pPartsLink = GetTemporaryPartsLink( pLink, CFPoint( StartPoint.x, StartPoint.y - yOffset - LastPartHeight ), pGearConnections );
		}

		if( pPartsLink == 0 )
			continue;

		yOffset += LastPartHeight;

		ConnectorList* pConnectors = pPartsLink->GetConnectorList();

		DrawLink( pRenderer, pGearConnections, m_SelectedViewLayers, pPartsLink, false, false, true );
		DrawLink( pRenderer, pGearConnections, m_SelectedViewLayers, pPartsLink, m_bShowLabels, true, false );

		//POSITION Position2 = pConnectors->GetHeadPosition();
		//while( Position2 != NULL )
		//{
		//	CConnector* pConnector = pConnectors->GetNext( Position2 );
		//	if( pConnector != 0 )
		//		DrawSliderTrack( pRenderer, m_SelectedViewLayers, pConnector );
		//}

		DrawLink( pRenderer, pGearConnections, m_SelectedViewLayers, pPartsLink, m_bShowLabels, false, false );
		POSITION Position2 = pConnectors->GetHeadPosition();
		while( Position2 != NULL )
		{
			CConnector* pConnector = pConnectors->GetNext( Position2 );
			if( pConnector != 0 )
			{
				DrawConnector( pRenderer, m_SelectedViewLayers, pConnector, m_bShowLabels );
				// DrawConnector( pRenderer, m_SelectedViewLayers, pConnector, m_bShowLabels, false, false, false, true );
			}
		}

		CFArea PartArea;
		pPartsLink->GetArea( *pGearConnections, PartArea );

		if( bGroundLink )
			PartArea += DrawGroundDimensions( pRenderer, pDoc, m_SelectedViewLayers, pPartsLink, true, true );
		else
			PartArea += DrawDimensions( pRenderer, pGearConnections, m_SelectedViewLayers, pPartsLink, true, true );
		Position2 = pConnectors->GetHeadPosition();
		while( Position2 != NULL )
		{
			CConnector* pConnector = pConnectors->GetNext( Position2 );
			if( pConnector != 0 )
			PartArea += DrawDimensions( pRenderer, m_SelectedViewLayers, pConnector, true, true );
		}

		DocumentArea += PartArea;

		LastPartHeight = abs( PartArea.Height() ) + Unscale( OFFSET_INCREMENT );

		ASSERT( pPartsLink != 0 );
		delete pPartsLink;
	}

	/*
	 * Create a ground link if no other single link holds all of the ground connectors.
	 */

	return DocumentArea;
}

void CLinkageView::DrawMotionPath( CRenderer *pRenderer, CConnector *pConnector )
{
	// Draw the drawing stuff before the Links and connectors
	int DrawPoint = 0;
	int PointCount = 0;
	int MaxPoints = 0;

	pRenderer->SetBkMode( TRANSPARENT );
	CFPoint *pPoints = pConnector->GetMotionPath( DrawPoint, PointCount, MaxPoints );
	if( pPoints != 0 && PointCount > 1 )
	{
		COLORREF Color = COLOR_MOTIONPATH;
		CPen Pen( PS_SOLID, 1, Color );
		CPen *pOldPen = pRenderer->SelectObject( &Pen );

		pRenderer->MoveTo( Scale( CFPoint( pPoints[DrawPoint].x, pPoints[DrawPoint].y ) ) );
		++DrawPoint;
		for( int Counter = 1; Counter < PointCount; ++Counter )
		{
			if( DrawPoint > MaxPoints )
				DrawPoint = 0;
			pRenderer->LineTo( Scale( CFPoint( pPoints[DrawPoint].x, pPoints[DrawPoint].y ) ) );
			++DrawPoint;
		}
		if( m_bSimulating )
			pRenderer->LineTo( Scale( pConnector->GetPoint() ) );
	}
}

BOOL StretchBltPlus(
  HDC hdcDest,
  int nXOriginDest,
  int nYOriginDest,
  int nWidthDest,
  int nHeightDest,
  HDC hdcSrc,
  int nXOriginSrc,
  int nYOriginSrc,
  int nWidthSrc,
  int nHeightSrc,
  DWORD dwRop
)
{
	  Graphics dest(hdcDest);
	  Graphics src(hdcSrc);
	  HBITMAP hBmp = (HBITMAP)GetCurrentObject( hdcSrc, OBJ_BITMAP );
	  Image *imx = Bitmap::FromHBITMAP(hBmp,NULL);
	  RectF r( (Gdiplus::REAL)nXOriginDest, (Gdiplus::REAL)nYOriginDest, (Gdiplus::REAL)nWidthDest, (Gdiplus::REAL)nHeightDest );
	  dest.SetInterpolationMode( InterpolationModeHighQuality );
	  dest.DrawImage( imx, r, (Gdiplus::REAL)nXOriginSrc, (Gdiplus::REAL)nYOriginSrc, (Gdiplus::REAL)nWidthSrc, (Gdiplus::REAL)nHeightSrc, UnitPixel );
	  delete imx;
	  return TRUE;
}

void CLinkageView::OnPrint(CDC* pDC, CPrintInfo* pPrintInfo)
{
	ASSERT_VALID(pDC);

	// Override and set printing variables based on page number
	OnDraw(pDC, pPrintInfo);                    // Call Draw
}

void CLinkageView::OnDraw( CDC* pDC )
{
		OnDraw( pDC, 0 );

/*
TEST CODE FOR BITMAP DRAWING FROM A FILE...

CImage image;
image.Load(_T("C:\\Users\\David\\Documents\\image.png")); // just change extension to load jpg
CBitmap bitmap;
bitmap.Attach(image.Detach());

CDC MemDC;
MemDC.CreateCompatibleDC( pDC );
MemDC.SelectObject( &bitmap );

pDC->BitBlt( 0, 0, 300, 300, &MemDC, 0, 0, SRCCOPY );
*/
}

void CLinkageView::OnDraw( CDC* pDC, CPrintInfo *pPrintInfo )
{
   	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	#if defined( LINKAGE_USE_DIRECT2D )
		CRenderer Renderer( pDC->IsPrinting() ? CRenderer::WINDOWS_PRINTER_GDI : CRenderer::WINDOWS_D2D );
	#else
		CRenderer Renderer( pDC->IsPrinting() ? CRenderer::WINDOWS_PRINTER_GDI : CRenderer::WINDOWS_GDI );
	#endif
    CBitmap Bitmap;

	/*
	 * Forcing a scale that is not 1.0 will result in slow framerates during editing
	 * and simulation. The forced scale value is really only intended to be used
	 * when saving a video animation of the mechanism.
	 */

    PrepareRenderer( Renderer, 0, &Bitmap, pDC, 1.0, false, 0.0, 1.0, !pDC->IsPrinting(), false, m_bPrintFullSize, pPrintInfo == 0 ? 0 : pPrintInfo->m_nCurPage - 1 );

	Renderer.BeginDraw();

	DoDraw( &Renderer );

	if( pDC->IsPrinting() )
	{
		/*
		 * Printing is done using the print DC directly. There is no
		 * copy operating and no video creation so this there is
		 * no further action to take in this function.
		 */
		Renderer.Detach();
		return;
	}

	if( m_bRecordingVideo && m_RecordQuality == 0 )
	{
		/*
		 * Copy the image used for the window directory to the video
		 * file as-is. No extra draw operation is needed for a
		 * standard quality video. The image will lose data if the
		 * window on the screen does not contain the entire video
		 * capture area.
		 */

		Renderer.EndDraw();
		SaveVideoFrame( &Renderer, m_DrawingRect );
		Renderer.BeginDraw();
	}

	if( m_bShowAnicrop )
		DrawAnicrop( &Renderer );

	DrawRuler( &Renderer );

	Renderer.EndDraw();


	/*
	 * Copy the image to the window.
	 */

	if( Renderer.GetScale() == 1.0 )
		pDC->BitBlt( 0, 0, m_DrawingRect.Width(), m_DrawingRect.Height(), Renderer.GetDC(), 0, 0, SRCCOPY );
	else
		StretchBltPlus( pDC->GetSafeHdc(),
		                0, 0, m_DrawingRect.Width(), m_DrawingRect.Height(),
		                Renderer.GetSafeHdc(),
						0, 0, (int)( m_DrawingRect.Width() * Renderer.GetScale() ), (int)( m_DrawingRect.Height() * Renderer.GetScale() ),
						SRCCOPY );

	if( m_bRecordingVideo && m_RecordQuality > 0 )
	{
		/*
		 * Redraw the entire image at a scaled size to get a high quality anti-aliased
		 * image in the video. Convert to 1080p.
		 */

		CRenderer VideoRenderer( CRenderer::WINDOWS_GDI );
		CBitmap VideoBitmap;

		CRect VideoRect( 0, 0, ANIMATIONWIDTH, ANIMATIONHEIGHT );

		PrepareRenderer( VideoRenderer, &VideoRect, &VideoBitmap, pDC, 4.0, false, 0.0, 1.0, true, true, false, 0 );

		// Adjust the offset to get the image properly positioned within the video area
		// since drawing is done of the video area only.
		int xAnimation = ( m_DrawingRect.Width() / 2 ) - ( ( ANIMATIONWIDTH + 1 ) / 2 );
		int yAnimatiom = ( m_DrawingRect.Height() / 2 ) - ( ( ANIMATIONHEIGHT + 1 ) / 2 );
		m_ScrollPosition.x += xAnimation;
		m_ScrollPosition.y += yAnimatiom;

		DoDraw( &VideoRenderer );

		SaveVideoFrame( &VideoRenderer, VideoRect );
	}
}

void CLinkageView::DrawAnicrop( CRenderer *pRenderer )
{
	if( !m_bShowAnicrop )
		return;

	int UseWidth = ( ANIMATIONWIDTH + 1 );
	int UseHeight = ( ANIMATIONHEIGHT + 1 );

	int x = ( m_DrawingRect.Width() / 2 ) - ( UseWidth / 2 );
	int y = ( m_DrawingRect.Height() / 2 ) - ( UseHeight / 2 );

	CPen Blue( PS_SOLID, 1, RGB( 196, 196, 255 ) );
	CPen *pOldPen = pRenderer->SelectObject( &Blue );
	pRenderer->DrawRect( x, y, x+UseWidth, y+UseHeight );

	UseWidth = ( ANIMATIONWIDTH + 1 ) / 2;
	UseHeight = ( ANIMATIONHEIGHT + 1 ) / 2;

	x = ( m_DrawingRect.Width() / 2 ) - ( UseWidth / 2 );
	y = ( m_DrawingRect.Height() / 2 ) - ( UseHeight / 2 );

	CPen BlueDots( PS_DOT, 1, RGB( 196, 196, 255 ) );
	pRenderer->SelectObject( &BlueDots );
	//pRenderer->DrawRect( x, y, x+UseWidth, y+UseHeight );

	pRenderer->SelectObject( pOldPen );
}

void CLinkageView::DrawRuler( CRenderer *pRenderer )
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if( !m_bShowDimensions || pDoc->GetConnectorList()->GetCount() > 0 )
		return;

	CPen Pen( PS_SOLID, 1, RGB( 196, 196, 196 ) );
	CPen *pOldPen = pRenderer->SelectObject( &Pen );
	CFont *pOriginalFont = pRenderer->SelectObject( m_pUsingFont, UnscaledUnits( m_UsingFontHeight ) );
	COLORREF OldFontColor = pRenderer->GetTextColor();
	pRenderer->SetTextColor( RGB( 96, 96, 96 ) );

	static const int SCALE_LINE_GAPLENGTH = 50;
	static const int SCALE_LINE_LENGTH = 500;
	static const int SCALE_LINE_SEGMENTS = 3;
	static const int SCALE_LINE_TICK_HEIGHT = 10;

	double DocScale = pDoc->GetUnitScale();

	double GapDistance = Unscale( SCALE_LINE_GAPLENGTH ) * DocScale;

	// Get the next 1, 5, 10, 50, 100 type of number that is above the
	// distance we got using a base pixel length. This will be the value
	// to use for the measurement line tick marks.
	int Test = (int)( GapDistance * 1000 );
	int Adjustment = -4;
	while( Test != 0 )
	{
		++Adjustment;
		Test /= 10;
	}
	double NewValue = GapDistance / pow( 10.0, Adjustment );
	NewValue += .9999;
	NewValue = (int)( NewValue );
	if( NewValue > 5.0 )
		NewValue = 10;
	else if( NewValue > 1.0 )
		NewValue = 5;
	GapDistance = NewValue * pow( 10.0, Adjustment );

	double PixelWidth = Scale( GapDistance / DocScale );
	int Sections = (int)( m_DrawingRect.Width() / 2 / PixelWidth );
	--Sections;
	PixelWidth *= Sections;

	for( int Counter = 0; Counter <= Sections; ++Counter )
	{
		CFLine Line( Counter * GapDistance / DocScale, 0, ( Counter + 1 ) * GapDistance / DocScale, 0 );
		Line = Scale( Line );

		CFLine Tick1( Line.GetStart(), CFPoint( Line.GetStart().x, Line.GetStart().y + SCALE_LINE_TICK_HEIGHT ) );

		pRenderer->DrawLine( Tick1 );

		if( Counter < Sections )
		{
			CFLine Tick2( Line.GetStart(), CFPoint( Line.GetStart().x, Line.GetStart().y + SCALE_LINE_TICK_HEIGHT ) );
			pRenderer->DrawLine( Tick2 );
			pRenderer->DrawLine( Line );
		}

		CString Measure;
		Measure.Format( "%.4lf", GapDistance * Counter );
		pRenderer->SetTextAlign( TA_CENTER | TA_TOP );
		pRenderer->TextOut( Line.GetStart().x, Line.GetStart().y + UnscaledUnits( SCALE_LINE_TICK_HEIGHT + 1 ), Measure );
	}

	pRenderer->SelectObject( pOldPen );
	pRenderer->SelectObject( pOriginalFont, UnscaledUnits( m_UsingFontHeight ) );
	pRenderer->SetTextColor( OldFontColor );
}

bool CLinkageView::SelectVideoBox( UINT nFlags, CPoint Point )
{
	if( m_VisibleAdjustment == ADJUSTMENT_NONE || m_MouseAction != ACTION_NONE )
		return false;

	CFRect Rect( 0, 0, 0, 0 );
	m_MouseAction = ACTION_NONE;

	CFPoint FPoint = Point;

	/*
	 * The line of code below gets screwed up by the compiler if the option
	 * is set to allow any appropriate function to be inline. Only functions
	 * marked as _inline shuold be allowed to avoid this compiler bug.
	 */
	GetAdjustmentControlRect( AC_ROTATE, Rect );
	Rect.InflateRect( 2, 2 );
	if( Rect.PointInRect( FPoint ) )
		m_MouseAction = ACTION_RECENTER;

	CFRect AdjustmentRect = Scale( m_SelectionContainerRect );

	MouseAction DoMouseAction = ACTION_NONE;
	if( m_VisibleAdjustment == ADJUSTMENT_ROTATE )
		DoMouseAction = ACTION_ROTATE;
	else
		DoMouseAction = ACTION_STRETCH;

	bool bVertical = fabs( m_SelectionContainerRect.Height() ) > 0;
	bool bHorizontal = fabs( m_SelectionContainerRect.Width() ) > 0;
	bool bBoth = bHorizontal && bVertical;

	GetAdjustmentControlRect( AC_TOP_LEFT, Rect );
	Rect.InflateRect( 2, 2 );
	if( bBoth && Rect.PointInRect( FPoint ) )
	{
		m_StretchingControl = AC_TOP_LEFT;
		m_DragOffset = FPoint - AdjustmentRect.TopLeft();
		m_MouseAction = DoMouseAction;
	}
	GetAdjustmentControlRect( AC_TOP_RIGHT, Rect );
	Rect.InflateRect( 2, 2 );
	if( bBoth && Rect.PointInRect( FPoint ) )
	{
		m_StretchingControl = AC_TOP_RIGHT;
		m_DragOffset = FPoint - CFPoint( AdjustmentRect.right, AdjustmentRect.top );
		m_MouseAction = DoMouseAction;
	}
	GetAdjustmentControlRect( AC_BOTTOM_RIGHT, Rect );
	Rect.InflateRect( 2, 2 );
	if( bBoth && Rect.PointInRect( FPoint ) )
	{
		m_StretchingControl = AC_BOTTOM_RIGHT;
		m_DragOffset = FPoint - AdjustmentRect.BottomRight();
		m_MouseAction = DoMouseAction;
	}
	GetAdjustmentControlRect( AC_BOTTOM_LEFT, Rect );
	Rect.InflateRect( 2, 2 );
	if( bBoth && Rect.PointInRect( FPoint ) )
	{
		m_StretchingControl = AC_BOTTOM_LEFT;
		m_DragOffset = FPoint - CFPoint( AdjustmentRect.left, AdjustmentRect.bottom );
		m_MouseAction = DoMouseAction;
	}

	if( m_VisibleAdjustment == ADJUSTMENT_STRETCH )
	{
		GetAdjustmentControlRect( AC_TOP, Rect );
		Rect.InflateRect( 2, 2 );
		if( bVertical && Rect.PointInRect( FPoint ) )
		{
			m_StretchingControl = AC_TOP;
			m_DragOffset = FPoint - CFPoint( ( AdjustmentRect.left + AdjustmentRect.right ) / 2, AdjustmentRect.top );
			m_MouseAction = ACTION_STRETCH;
		}
		GetAdjustmentControlRect( AC_RIGHT, Rect );
		Rect.InflateRect( 2, 2 );
		if( bHorizontal && Rect.PointInRect( FPoint ) )
		{
			m_StretchingControl = AC_RIGHT;
			m_DragOffset = FPoint - CFPoint( AdjustmentRect.right, ( AdjustmentRect.top + AdjustmentRect.bottom ) / 2 );
			m_MouseAction = ACTION_STRETCH;
		}
		GetAdjustmentControlRect( AC_BOTTOM, Rect );
		Rect.InflateRect( 2, 2 );
		if( bVertical && Rect.PointInRect( FPoint ) )
		{
			m_StretchingControl = AC_BOTTOM;
			m_DragOffset = FPoint - CFPoint( ( AdjustmentRect.left + AdjustmentRect.right ) / 2, AdjustmentRect.bottom );
			m_MouseAction = ACTION_STRETCH;
		}
		GetAdjustmentControlRect( AC_LEFT, Rect );
		Rect.InflateRect( 2, 2 );
		if( bHorizontal && Rect.PointInRect( FPoint ) )
		{
			m_StretchingControl = AC_LEFT;
			m_DragOffset = FPoint - CFPoint( AdjustmentRect.left, ( AdjustmentRect.top + AdjustmentRect.bottom ) / 2 );
			m_MouseAction = ACTION_STRETCH;
		}
	}

	if( m_MouseAction == ACTION_NONE )
		return false;
	else
	{
		InvalidateRect( 0 );
		return true;
	}
}

bool CLinkageView::GrabAdjustmentControl( CFPoint Point, CFPoint GrabPoint, _AdjustmentControl CheckControl, MouseAction DoMouseAction, _AdjustmentControl *pSelectControl, CFPoint *pDragOffset, MouseAction *pMouseAction )
{
	CFRect Rect( 0, 0, 0, 0 );
	GetAdjustmentControlRect( CheckControl, Rect );
	Rect.InflateRect( 2, 2 );
	if( Rect.PointInRect( Point ) )
	{
		*pSelectControl = CheckControl;
		*pDragOffset = Point - GrabPoint;
		*pMouseAction = DoMouseAction;
		return true;
	}
	return false;
}

CFPoint CLinkageView::AdjustClientAreaPoint( CPoint point )
{
	#if defined( LINKAGE_USE_DIRECT2D )
		return CFPoint( point.x / m_DPIScale, point.y / m_DPIScale );
	#else
		return CFPoint( point.x, point.y );
	#endif
}

bool CLinkageView::SelectAdjustmentControl( UINT nFlags, CFPoint Point )
{
	if( m_VisibleAdjustment == ADJUSTMENT_NONE || m_MouseAction != ACTION_NONE )
		return false;

	CFRect Rect( 0, 0, 0, 0 );
	m_MouseAction = ACTION_NONE;

	CFPoint FPoint = Point;

	/*
	 * The line of code below gets screwed up by the compiler if the option
	 * is set to allow any appropriate function to be inline. Only functions
	 * marked as _inline should be allowed to avoid this compiler bug.
	 */
	GetAdjustmentControlRect( AC_ROTATE, Rect );
	Rect.InflateRect( 2, 2 );
	if( Rect.PointInRect( FPoint ) )
		m_MouseAction = ACTION_RECENTER;

	CFRect AdjustmentRect = Scale( m_SelectionContainerRect );

	MouseAction DoMouseAction = ACTION_NONE;
	if( m_VisibleAdjustment == ADJUSTMENT_ROTATE )
		DoMouseAction = ACTION_ROTATE;
	else
		DoMouseAction = ACTION_STRETCH;

	bool bVertical = fabs( m_SelectionContainerRect.Height() ) > 0;
	bool bHorizontal = fabs( m_SelectionContainerRect.Width() ) > 0;

	if( bHorizontal && bVertical )
	{
		GrabAdjustmentControl( FPoint, AdjustmentRect.TopLeft(), AC_TOP_LEFT, DoMouseAction, &m_StretchingControl, &m_DragOffset, &m_MouseAction );
		GrabAdjustmentControl( FPoint, CFPoint( AdjustmentRect.right, AdjustmentRect.top ), AC_TOP_RIGHT, DoMouseAction, &m_StretchingControl, &m_DragOffset, &m_MouseAction );
		GrabAdjustmentControl( FPoint, AdjustmentRect.BottomRight(), AC_BOTTOM_RIGHT, DoMouseAction, &m_StretchingControl, &m_DragOffset, &m_MouseAction );
		GrabAdjustmentControl( FPoint,  CFPoint( AdjustmentRect.left, AdjustmentRect.bottom ), AC_BOTTOM_LEFT, DoMouseAction, &m_StretchingControl, &m_DragOffset, &m_MouseAction );
	}

	if( m_VisibleAdjustment == ADJUSTMENT_STRETCH )
	{
		if( bVertical )
		{
			GrabAdjustmentControl( FPoint, CFPoint( ( AdjustmentRect.left + AdjustmentRect.right ) / 2 ), AC_TOP, ACTION_STRETCH, &m_StretchingControl, &m_DragOffset, &m_MouseAction );
			GrabAdjustmentControl( FPoint, CFPoint( ( AdjustmentRect.left + AdjustmentRect.right ) / 2, AdjustmentRect.bottom ), AC_BOTTOM, ACTION_STRETCH, &m_StretchingControl, &m_DragOffset, &m_MouseAction );
		}
		if( bHorizontal )
		{
			GrabAdjustmentControl( FPoint, CFPoint( AdjustmentRect.right, ( AdjustmentRect.top + AdjustmentRect.bottom ) / 2 ), AC_RIGHT, ACTION_STRETCH, &m_StretchingControl, &m_DragOffset, &m_MouseAction );
			GrabAdjustmentControl( FPoint, CFPoint( AdjustmentRect.left, ( AdjustmentRect.top + AdjustmentRect.bottom ) / 2 ), AC_LEFT, ACTION_STRETCH, &m_StretchingControl, &m_DragOffset, &m_MouseAction );
		}
	}

	if( m_MouseAction == ACTION_NONE )
		return false;
	else
	{
		InvalidateRect( 0 );
		return true;
	}
}

void CLinkageView::MarkSelection( bool bSelectionChanged )
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if( pDoc->IsSelectionAdjustable() )
	{
		if( bSelectionChanged )
		{
			m_bChangeAdjusters = false;
			m_VisibleAdjustment = ADJUSTMENT_STRETCH;
			m_SelectionContainerRect = GetDocumentArea( false, true );
			m_SelectionAdjustmentRect = GetDocumentAdjustArea( true );
			m_SelectionRotatePoint.SetPoint( ( m_SelectionContainerRect.left + m_SelectionContainerRect.right ) / 2,
												( m_SelectionContainerRect.top + m_SelectionContainerRect.bottom ) / 2 );
		}
		else
			m_bChangeAdjusters = true;
	}
	else
		m_VisibleAdjustment = ADJUSTMENT_NONE;
}

bool CLinkageView::FindDocumentItem( CFPoint Point, CLink *&pFoundLink, CConnector *&pFoundConnector )
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CFPoint AdjustedPoint = Unscale( Point );

	double GrabDistance = Unscale( GRAB_DISTANCE );

	return pDoc->FindElement( AdjustedPoint, GrabDistance, 0, pFoundLink, pFoundConnector );
}

bool CLinkageView::SelectDocumentItem( UINT nFlags, CFPoint point )
{
	if( m_MouseAction != ACTION_NONE )
		return false;

	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	bool bMultiSelect = ( nFlags & ( MK_CONTROL | MK_SHIFT ) ) != 0;

	CFPoint AdjustedPoint = Unscale( point );

	double GrabDistance = Unscale( GRAB_DISTANCE );

	bool bSelectionChanged = false;
	m_MouseAction = pDoc->SelectElement( AdjustedPoint, GrabDistance, 0, bMultiSelect, bSelectionChanged ) ? ACTION_DRAG : ACTION_NONE;

	MarkSelection( bSelectionChanged );
	ShowSelectedElementCoordinates();

	if( m_MouseAction == ACTION_NONE )
		return false;
	else
	{
		InvalidateRect( 0 );
		return true;
	}
}

bool CLinkageView::DragSelectionBox( UINT nFlags, CFPoint point )
{
	if( m_MouseAction != ACTION_NONE )
		return false;

	/*
	 * Assume nothing else is under the mouse cursor at this point
	 * in the processing. There would be some mouse action set if
	 * something else has been selected before noe.
	 */

	m_MouseAction = ACTION_SELECT;
	m_SelectRect.SetRect( point.x, point.y, point.x, point.y );
	InvalidateRect( 0 );

	return true;
}

void CLinkageView::OnLButtonDown(UINT nFlags, CPoint MousePoint)
{
	CFPoint point = AdjustClientAreaPoint( MousePoint );

	SetCapture();

	if( m_bSimulating || !m_bAllowEdit )
		return;

	m_PreviousDragPoint = point;
	m_DragStartPoint = point;
	m_DragOffset.SetPoint( 0, 0 );
	m_bMouseMovedEnough	= false;
	m_MouseAction = ACTION_NONE;
	m_bSuperHighlight = 0;

	CFPoint DocumentPoint = Unscale( point );
	SetLocationAsStatus( DocumentPoint );

	if( m_bAllowEdit && SelectAdjustmentControl( nFlags, point ) )
		return;

	if( SelectDocumentItem( nFlags, point ) )
		return;

	if( DragSelectionBox( nFlags, point ) )
		return;
}

void CLinkageView::OnButtonUp(UINT nFlags, CPoint MousePoint)
{
	CFPoint point = AdjustClientAreaPoint( MousePoint );

	ReleaseCapture();

    if( m_bChangeAdjusters )
		OnMouseEndChangeAdjusters( nFlags, point );

	switch( m_MouseAction )
	{
		case ACTION_SELECT: OnMouseEndSelect( nFlags, point ); break;
		case ACTION_DRAG: OnMouseEndDrag( nFlags, point ); break;
		case ACTION_PAN: OnMouseEndPan( nFlags, point ); break;
		case ACTION_ROTATE: OnMouseEndRotate( nFlags, point ); break;
		case ACTION_RECENTER: OnMouseEndRecenter( nFlags, point ); break;
		case ACTION_STRETCH: OnMouseEndStretch( nFlags, point ); break;
		default: break;
	}
	m_MouseAction = ACTION_NONE;

	CFPoint DocumentPoint = Unscale( point );
	//SetLocationAsStatus( DocumentPoint );

	SetScrollExtents();
}

void CLinkageView::OnLButtonUp(UINT nFlags, CPoint point)
{
	OnButtonUp( nFlags, point );
}

void CLinkageView::OnMouseMoveSelect(UINT nFlags, CFPoint point)
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	m_SelectRect.SetRect( m_SelectRect.TopLeft(), point );
	InvalidateRect( 0 );
}

double CLinkageView::CalculateDefaultGrid( void )
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	double GapDistance = Unscale( 20 ) * pDoc->GetUnitScale();

	// Get the next 1, 5, 10, 50, 100 type of number that is above the
	// distance we got using a base pixel length. This will be the value
	// to use for the snap.
	int Test = (int)( GapDistance * 1000 );
	int Adjustment = -4;
	while( Test != 0 )
	{
		++Adjustment;
		Test /= 10;
	}
	double NewValue = GapDistance / pow( 10.0, Adjustment );
	NewValue += .9999;
	NewValue = (int)( NewValue );
	if( NewValue > 5.0 )
		NewValue = 10;
	else if( NewValue > 1.0 )
		NewValue = 5;
	GapDistance = NewValue * pow( 10.0, Adjustment );
	if( GapDistance < 0.01 )
		GapDistance = 0.01;

	return GapDistance;
}

void CLinkageView::OnMouseMoveDrag(UINT nFlags, CFPoint point)
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CFPoint AdjustedPoint = Unscale( point );

	bool bNoSnap = ( nFlags & ( MK_CONTROL | MK_SHIFT ) ) != 0;

	CFPoint ReferencePoint = AdjustedPoint;

	double GapDistance = CalculateDefaultGrid();

#if 0

	double GapDistance = Unscale( 20 ) * pDoc->GetUnitScale();

	// Get the next 1, 5, 10, 50, 100 type of number that is above the
	// distance we got using a base pixel length. This will be the value
	// to use for the snap.
	int Test = (int)( GapDistance * 1000 );
	int Adjustment = -4;
	while( Test != 0 )
	{
		++Adjustment;
		Test /= 10;
	}
	double NewValue = GapDistance / pow( 10.0, Adjustment );
	NewValue += .9999;
	NewValue = (int)( NewValue );
	if( NewValue > 5.0 )
		NewValue = 10;
	else if( NewValue > 1.0 )
		NewValue = 5;
	GapDistance = NewValue * pow( 10.0, Adjustment );
	if( GapDistance < 0.01 )
		GapDistance = 0.01;
#endif

	if( m_bAllowEdit )
	{
		if( pDoc->MoveSelected( AdjustedPoint, m_bSnapOn && !bNoSnap, m_bGridSnap && !bNoSnap,  GapDistance / pDoc->GetUnitScale(),  GapDistance / pDoc->GetUnitScale(), Unscale( PIXEL_SNAP_DISTANCE ), ReferencePoint ) )
		InvalidateRect( 0 );
	}

	SetLocationAsStatus( ReferencePoint );
}

void CLinkageView::OnMouseMovePan(UINT nFlags, CFPoint point)
{
	m_ScreenScrollPosition.x -= point.x - m_PreviousDragPoint.x;
	m_ScreenScrollPosition.y -= point.y - m_PreviousDragPoint.y;
	InvalidateRect( 0 );
}

void CLinkageView::OnMouseMoveRotate(UINT nFlags, CFPoint point)
{
	CFPoint DragStartPoint = Unscale( m_DragStartPoint );
	CFPoint CurrentPoint = Unscale( point );

	double Angle1 = GetAngle( m_SelectionRotatePoint, DragStartPoint );
	double Angle2 = GetAngle( m_SelectionRotatePoint, CurrentPoint );
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if( pDoc->RotateSelected( m_SelectionRotatePoint, Angle2 - Angle1 ) )
		InvalidateRect( 0 );
}

class CSnapConnector : public ConnectorListOperation
{
	public:
	CConnector *m_pSnapConnector;
	double m_Distance;
	CFPoint m_Point;
	double m_SnapDistance;
	CSnapConnector( const CFPoint &Point, double SnapDistance ) { m_Distance = DBL_MAX; m_pSnapConnector = 0; m_Point = Point; m_SnapDistance = SnapDistance; }
	bool operator()( class CConnector *pConnector, bool bFirst, bool bLast )
	{
		double TestDistance = m_Point.DistanceToPoint( pConnector->GetPoint() );
		if( TestDistance < m_Distance && TestDistance <= m_SnapDistance )
		{
			m_Distance = TestDistance;
			m_pSnapConnector = pConnector;
		}
		return true;
	}
	double GetDistance( void ) { return m_Distance; }
	CConnector *GetConnector( void ) { return m_pSnapConnector; }
};

void CLinkageView::OnMouseMoveRecenter(UINT nFlags, CFPoint point)
{
	// Snap to connectors

	CFPoint AdjustedPoint = Unscale( point );

	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CSnapConnector Snap( AdjustedPoint, Unscale( PIXEL_SNAP_DISTANCE ) );
	pDoc->GetConnectorList()->Iterate( Snap );

	CConnector* pSnapConnector = Snap.GetConnector();

	if( pSnapConnector != 0 )
		m_SelectionRotatePoint = pSnapConnector->GetPoint();
	else
		m_SelectionRotatePoint = AdjustedPoint;

	InvalidateRect( 0 );
}

void CLinkageView::OnMouseMoveStretch(UINT nFlags, CFPoint point, bool bEndStretch )
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	CFPoint AdjustedPoint = Unscale( point );
	CLinkageDoc::_Direction Direction = CLinkageDoc::DIAGONAL;

	CFRect NewRect = m_SelectionAdjustmentRect;
	switch( m_StretchingControl )
	{
		case AC_TOP: NewRect.top = AdjustedPoint.y; Direction = CLinkageDoc::VERTICAL; break;
		case AC_RIGHT: NewRect.right = AdjustedPoint.x; Direction = CLinkageDoc::HORIZONTAL; break;
		case AC_BOTTOM: NewRect.bottom = AdjustedPoint.y; Direction = CLinkageDoc::VERTICAL; break;
		case AC_LEFT: NewRect.left = AdjustedPoint.x; Direction = CLinkageDoc::HORIZONTAL; break;

		case AC_TOP_LEFT:
		case AC_TOP_RIGHT:
		case AC_BOTTOM_LEFT:
		case AC_BOTTOM_RIGHT:
		{
			Direction = CLinkageDoc::DIAGONAL;
			double NewHeight;
			double NewWidth;
			switch( m_StretchingControl )
			{
				case AC_TOP_LEFT:
					NewRect.left = AdjustedPoint.x;
					NewRect.top = AdjustedPoint.y;
					NewHeight = m_SelectionAdjustmentRect.Width() == 0 ? 0 : m_SelectionAdjustmentRect.Height() * NewRect.Width() / m_SelectionAdjustmentRect.Width();
					NewWidth = m_SelectionAdjustmentRect.Height() == 0 ? 0 : m_SelectionAdjustmentRect.Width() * NewRect.Height() / m_SelectionAdjustmentRect.Height();

					if( fabs( NewWidth ) > fabs( NewRect.Width() ) )
						NewRect.left = NewRect.right - NewWidth;
					else
						NewRect.top = NewRect.bottom - NewHeight;
					break;
				case AC_TOP_RIGHT:
					NewRect.right = AdjustedPoint.x;
					NewRect.top = AdjustedPoint.y;
					NewHeight = m_SelectionAdjustmentRect.Width() == 0 ? 0 : m_SelectionAdjustmentRect.Height() * NewRect.Width() / m_SelectionAdjustmentRect.Width();
					NewWidth = m_SelectionAdjustmentRect.Height() == 0 ? 0 : m_SelectionAdjustmentRect.Width() * NewRect.Height() / m_SelectionAdjustmentRect.Height();

					if( fabs( NewWidth ) > fabs( NewRect.Width() ) )
						NewRect.right = NewRect.left + NewWidth;
					else
						NewRect.top = NewRect.bottom - NewHeight;
					break;
				case AC_BOTTOM_LEFT:
					NewRect.left = AdjustedPoint.x;
					NewRect.bottom = AdjustedPoint.y;
					NewHeight = m_SelectionAdjustmentRect.Width() == 0 ? 0 : m_SelectionAdjustmentRect.Height() * NewRect.Width() / m_SelectionAdjustmentRect.Width();
					NewWidth = m_SelectionAdjustmentRect.Height() == 0 ? 0 : m_SelectionAdjustmentRect.Width() * NewRect.Height() / m_SelectionAdjustmentRect.Height();

					if( fabs( NewWidth ) > fabs( NewRect.Width() ) )
						NewRect.left = NewRect.right - NewWidth;
					else
						NewRect.bottom = NewRect.top + NewHeight;
					break;
				case AC_BOTTOM_RIGHT:
					NewRect.right = AdjustedPoint.x;
					NewRect.bottom = AdjustedPoint.y;
					NewHeight = m_SelectionAdjustmentRect.Width() == 0 ? 0 : m_SelectionAdjustmentRect.Height() * NewRect.Width() / m_SelectionAdjustmentRect.Width();
					NewWidth = m_SelectionAdjustmentRect.Height() == 0 ? 0 : m_SelectionAdjustmentRect.Width() * NewRect.Height() / m_SelectionAdjustmentRect.Height();

					if( fabs( NewWidth ) > fabs( NewRect.Width() ) )
						NewRect.right = NewRect.left + NewWidth;
					else
						NewRect.bottom = NewRect.top + NewHeight;
					break;
			}
			break;
		}
	}

	if( !pDoc->StretchSelected( m_SelectionAdjustmentRect, NewRect, Direction ) )
		return;

	if( bEndStretch )
	{
		CFPoint Scale( NewRect.Width() / m_SelectionAdjustmentRect.Width(), NewRect.Height() / m_SelectionAdjustmentRect.Height() );

		CFPoint OldPoint = m_SelectionRotatePoint;
		CFPoint NewPoint;
		m_SelectionRotatePoint.x = NewRect.left + ( ( OldPoint.x - m_SelectionAdjustmentRect.left ) * Scale.x );
		m_SelectionRotatePoint.y = NewRect.top + ( ( OldPoint.y - m_SelectionAdjustmentRect.top ) * Scale.y );
	}
	InvalidateRect( 0 );
}

void CLinkageView::OnMouseEndChangeAdjusters(UINT nFlags, CFPoint point)
{
	m_bChangeAdjusters = false;
	if( m_bAllowEdit )
	{
		if( m_VisibleAdjustment == ADJUSTMENT_STRETCH )
			m_VisibleAdjustment = ADJUSTMENT_ROTATE;
		else if( m_VisibleAdjustment == ADJUSTMENT_ROTATE )
			m_VisibleAdjustment = ADJUSTMENT_STRETCH;
	}
}

void CLinkageView::OnMouseEndDrag(UINT nFlags, CFPoint point)
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	pDoc->FinishChangeSelected();

	int TotalSelected = pDoc->GetSelectedLinkCount( false ) + pDoc->GetSelectedConnectorCount();
	if( TotalSelected > 0 )
	{
		CFPoint TempPoint( m_SelectionContainerRect.left, m_SelectionContainerRect.top );
		m_SelectionRotatePoint -= TempPoint;
		m_SelectionContainerRect = GetDocumentArea( false, true );
		TempPoint.SetPoint( m_SelectionContainerRect.left, m_SelectionContainerRect.top );
		m_SelectionRotatePoint += TempPoint;

		ShowSelectedElementStatus();
	}

	if( m_bSnapOn && m_bAutoJoin &&
	    pDoc->GetSelectedLinkCount( true ) == 0 && pDoc->GetSelectedConnectorCount() == 1 )
	{
		pDoc->AutoJoinSelected();
	}

	m_SelectionContainerRect = GetDocumentArea( false, true );
	m_SelectionAdjustmentRect = GetDocumentAdjustArea( true );
	//ShowSelectedElementCoordinates();

	InvalidateRect( 0 );
}

void CLinkageView::ShowSelectedElementStatus( void )
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CElement *pElement = 0;

	if( pDoc->GetSelectedConnectorCount() == 1 )
	{
		pElement = pDoc->GetSelectedConnector( 0 );
	}
	else if( pDoc->GetSelectedLinkCount( false ) == 1 )
	{
		pElement = pDoc->GetSelectedLink( 0, false );
	}
	else
		return;

	if( pElement == 0 )
		return;

	CString String;
	if( pElement->IsLink() )
		String = "Link ";
	else if( pElement->IsConnector() )
		String = "Connector ";

	String += pElement->GetIdentifierString( m_bShowDebug );

	SetStatusText( String );
}

void CLinkageView::OnMouseEndSelect(UINT nFlags, CFPoint point)
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	/*
	 * Normalize the rectangle for selection. Other code might not handle
	 * it otherwise.
	 */

	if( m_SelectRect.left > m_SelectRect.right )
	{
		double Temp = m_SelectRect.left;
		m_SelectRect.left = m_SelectRect.right;
		m_SelectRect.right = Temp;
	}
	if( m_SelectRect.top > m_SelectRect.bottom )
	{
		double Temp = m_SelectRect.top;
		m_SelectRect.top = m_SelectRect.bottom;
		m_SelectRect.bottom = Temp;
	}

	CFRect Rect = Unscale( m_SelectRect );

	m_bChangeAdjusters = false;
	bool bMultiSelect = ( nFlags & ( MK_CONTROL | MK_SHIFT ) ) != 0;

	bool bSelectionChanged = false;
	m_MouseAction = pDoc->SelectElement( Rect, 0, bMultiSelect, bSelectionChanged ) ? ACTION_DRAG : ACTION_NONE;

    if( pDoc->IsSelectionAdjustable() )
    {
		if( bSelectionChanged )
		{
			m_VisibleAdjustment = ADJUSTMENT_STRETCH;
			m_SelectionContainerRect = GetDocumentArea( false, true );
			m_SelectionAdjustmentRect = GetDocumentAdjustArea( true );
			m_SelectionRotatePoint.SetPoint( ( m_SelectionContainerRect.left + m_SelectionContainerRect.right ) / 2,
											 ( m_SelectionContainerRect.top + m_SelectionContainerRect.bottom ) / 2 );
		}
	}
	else
		m_VisibleAdjustment = ADJUSTMENT_NONE;

	ShowSelectedElementStatus();

	ShowSelectedElementCoordinates();
	InvalidateRect( 0 );
}

void CLinkageView::OnMouseEndPan(UINT nFlags, CFPoint point)
{
	InvalidateRect( 0 );
}

void CLinkageView::OnMouseEndRotate(UINT nFlags, CFPoint point)
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->FinishChangeSelected();
	m_SelectionContainerRect = GetDocumentArea( false, true );
	m_SelectionAdjustmentRect = GetDocumentAdjustArea( true );
	InvalidateRect( 0 );
}

void CLinkageView::OnMouseEndRecenter(UINT nFlags, CFPoint point)
{
	InvalidateRect( 0 );
}

void CLinkageView::OnMouseEndStretch(UINT nFlags, CFPoint point)
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->FinishChangeSelected();
	m_SelectionContainerRect = GetDocumentArea( false, true );
	m_SelectionAdjustmentRect = GetDocumentAdjustArea( true );
	InvalidateRect( 0 );
}

void CLinkageView::SetLocationAsStatus( CFPoint &Point )
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CString Location;
	double DocumentScale = pDoc->GetUnitScale();

	Location.Format( "%.4f,%.4f", Point.x * DocumentScale, Point.y * DocumentScale );
	SetStatusText( Location );
}

void CLinkageView::OnMouseMove(UINT nFlags, CPoint MousePoint)
{
	CFPoint point = AdjustClientAreaPoint( MousePoint );

	if( m_PreviousDragPoint == point )
		return;

	if( m_bSimulating )
		return;

	if( !m_bMouseMovedEnough )
	{
		if( abs( point.x - m_DragStartPoint.x ) < 2
			&& abs( point.y - m_DragStartPoint.y ) < 2 )
			return;

		m_bMouseMovedEnough = true;

		if( m_MouseAction == ACTION_DRAG
			|| m_MouseAction == ACTION_ROTATE
			|| m_MouseAction == ACTION_STRETCH )
		{
			CLinkageDoc* pDoc = GetDocument();
			ASSERT_VALID(pDoc);
			pDoc->StartChangeSelected();
		}
	}

	if( m_MouseAction != ACTION_DRAG )
	{
		CFPoint DocumentPoint = Unscale( point );
		SetLocationAsStatus( DocumentPoint );
	}

	if( m_MouseAction == ACTION_NONE )
		return;

	switch( m_MouseAction )
	{
		case ACTION_SELECT: OnMouseMoveSelect( nFlags, point ); break;
		case ACTION_DRAG: OnMouseMoveDrag( nFlags, point ); break;
		case ACTION_PAN: OnMouseMovePan( nFlags, point ); break;
		case ACTION_ROTATE: OnMouseMoveRotate( nFlags, point ); break;
		case ACTION_RECENTER: OnMouseMoveRecenter( nFlags, point ); break;
		case ACTION_STRETCH: OnMouseMoveStretch( nFlags, point, false ); break;
		default: break;
	}

	m_bChangeAdjusters = false;

	m_PreviousDragPoint = point;
}

void CLinkageView::OnMechanismReset()
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	pDoc->Reset( false );
	m_Simulator.Reset();
	UpdateForDocumentChange();
}

DWORD TickCount;

void CALLBACK TimeProc( UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2 )
{
	TickCount = GetTickCount();
	CLinkageView* pView = (CLinkageView*)dwUser;
	pView->SendMessage( WM_TIMER, 0, 0 );
}

bool CLinkageView::InitializeVideoFile( const char *pVideoFile, const char *pCompressorString )
{
	CString FCC = "CVID";
	const char *pFCC = strchr( pCompressorString, '(' );
	if( pFCC != 0 )
	{
		FCC.Truncate( 0 );
		FCC.Append( pFCC+1, 4 );
	}

	m_pAvi = new CAviFile( pVideoFile, mmioFOURCC( FCC[0], FCC[1], FCC[2], FCC[3] ), 30 );

	return true;
}

void CLinkageView::SaveVideoFrame( CRenderer *pRenderer, CRect &CaptureRect )
{
	if( m_pAvi == 0 )
		return;

	bool bHDScaling = m_RecordQuality > 0;

	double ScaleFactor = pRenderer->GetScale();

	int Width = bHDScaling ? 1920 : ANIMATIONWIDTH;
	int Height = bHDScaling ? 1080 : ANIMATIONHEIGHT;

	CDIBSection *pVideoBitmap = new CDIBSection;
	if( pVideoBitmap == 0 )
		return;

	if( !pVideoBitmap->CreateDibSection( pRenderer->GetDC(), Width, Height ) )
		return;

	if( !bHDScaling && ( CaptureRect.Width() < Width || CaptureRect.Height() < Height ) )
		pVideoBitmap->m_DC.PatBlt( 0, 0, (int)( Width * ScaleFactor ), (int)( Height * ScaleFactor ), WHITENESS );

	int x = bHDScaling ? 0 : ( CaptureRect.Width() / 2 ) - ( Width / 2 );
	int y = bHDScaling ? 0 : ( CaptureRect.Height() / 2 ) - ( Height / 2 );

	if( ScaleFactor == 1.0 && !bHDScaling )
		pVideoBitmap->m_DC.BitBlt( 0, 0, Width, Height, pRenderer->GetDC(), x, y, SRCCOPY );
	else
	{
		StretchBltPlus( pVideoBitmap->m_DC.GetSafeHdc(),
		                0, 0, Width, Height,
		                pRenderer->GetSafeHdc(),
						(int)( x * ScaleFactor ), (int)( y * ScaleFactor ),
						(int)( ANIMATIONWIDTH * ScaleFactor ), (int)( ANIMATIONHEIGHT * ScaleFactor ),
						SRCCOPY );
	}

	DWORD Ticks = GetTickCount();
	while( m_pVideoBitmapQueue != 0 && GetTickCount() < Ticks + 2000 )
		Sleep( 1 );

	if( m_pVideoBitmapQueue == 0 && StartVideoThread() )
	{
		m_pVideoBitmapQueue = pVideoBitmap;
		SetEvent( m_hVideoFrameEvent );
	}
	else
		delete pVideoBitmap;
}

void CLinkageView::VideoThreadFunction( void )
{
	CoInitialize( 0 );

	while( !m_bRequestAbort )
	{
		if( WaitForSingleObject( m_hVideoFrameEvent, 500 ) != WAIT_TIMEOUT )
		{
			if( m_pVideoBitmapQueue != 0 )
			{
				m_pAvi->AppendNewFrame( m_pVideoBitmapQueue, 1 );
				delete m_pVideoBitmapQueue;
				m_pVideoBitmapQueue = 0;
			}
		}
	}

	CoUninitialize();

	m_bProcessingVideoThread = false;
}

unsigned int __stdcall VideoThreadFunction( void* pInfo )
{
	if( pInfo == 0 )
		return 0;

	CLinkageView *pView = (CLinkageView*)pInfo;
	pView->VideoThreadFunction();

	return 0;
}

bool CLinkageView::StartVideoThread( void )
{
	if( m_bProcessingVideoThread )
		return true;

	m_bProcessingVideoThread = true;
	uintptr_t ThreadHandle;
	ThreadHandle = _beginthreadex( NULL, 0, ::VideoThreadFunction, this, 0, NULL );
	if( ThreadHandle == 0 )
	{
		m_bProcessingVideoThread = false;
		return false;
	}
	CloseHandle( (HANDLE)ThreadHandle );
	return true;
}

void CLinkageView::CloseVideoFile( void )
{
	if( !m_bRecordingVideo )
		return;

	m_bRecordingVideo = false;

	DWORD Ticks = GetTickCount();
	while( m_pVideoBitmapQueue != 0 && GetTickCount() < Ticks + 60000 )
		Sleep( 1 );

	if( m_pAvi != 0 )
	{
		delete m_pAvi;
		m_pAvi = 0;
	}
}

bool CLinkageView::AnyAlwaysManual( void )
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	ConnectorList* pConnectors = pDoc->GetConnectorList();
	POSITION Position = pConnectors->GetHeadPosition();
	while( Position != NULL )
	{
		CConnector* pConnector = pConnectors->GetNext( Position );
		if( pConnector != 0 && pConnector->IsInput() && pConnector->IsAlwaysManual() )
			return true;
	}

	LinkList* pLinkList = pDoc->GetLinkList();
	Position = pLinkList->GetHeadPosition();
	while( Position != NULL )
	{
		CLink* pLink = pLinkList->GetNext( Position );
		if( pLink != 0 && pLink->IsActuator() && pLink->IsAlwaysManual() )
			return true;
	}

	return false;
}

void CLinkageView::StartMechanismSimulate( enum _SimulationControl SimulationControl )
{
	SetFocus();

	if( m_bSimulating )
		return;

	if( m_TimerID != 0 )
		timeKillEvent( m_TimerID );

	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	pDoc->SelectElement();
	InvalidateRect( 0 );
	m_MouseAction = ACTION_NONE;

	DebugItemList.Clear();

	m_bWasValid = true;
	if( m_bRecordingVideo )
		SetStatusText( "Recording simulation video." );
	else
		SetStatusText( "Running simulation." );

	pDoc->Reset( true );

	m_Simulator.Reset();
	m_bSimulating = true;
	m_SimulationControl = SimulationControl;
	m_SimulationSteps = 0;
	SetScrollExtents( false );
	m_ControlWindow.ShowWindow( m_ControlWindow.GetControlCount() > 0 ? SW_SHOWNORMAL : SW_HIDE );
	m_TimerID = timeSetEvent( 33, 1, TimeProc, (DWORD_PTR)this, 0 );
}

void CLinkageView::StopMechanismSimulate( bool KeepCurrentPositions )
{
	if( !m_bSimulating )
		return;

	CloseVideoFile();

	ClearDebugItems();

	if( m_TimerID != 0 )
		timeKillEvent( m_TimerID );

	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	pDoc->SelectElement();
	InvalidateRect( 0 );
	m_MouseAction = ACTION_NONE;

	SetStatusText();
	m_bSimulating = false;
	SetScrollExtents();
	m_ControlWindow.ShowWindow( SW_HIDE );

	pDoc->Reset( false, KeepCurrentPositions );
	m_Simulator.Reset();

	UpdateForDocumentChange();
}

void CLinkageView::OnMechanismSimulate()
{
	ConfigureControlWindow( AUTO );
	StartMechanismSimulate( AUTO );
}

void CLinkageView::ConfigureControlWindow( enum _SimulationControl SimulationControl )
{
	m_ControlWindow.Clear();

	if( SimulationControl == GLOBAL )
		m_ControlWindow.AddControl( -1, "Mechanism", -1 );

	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	ConnectorList* pConnectors = pDoc->GetConnectorList();
	POSITION Position = pConnectors->GetHeadPosition();
	while( Position != NULL )
	{
		CConnector* pConnector = pConnectors->GetNext( Position );
		if( pConnector == 0 )
			continue;
		if( !pConnector->IsInput() )
			continue;
		if( SimulationControl == INDIVIDUAL || pConnector->IsAlwaysManual() )
		{
			CString String;
			String.Format( "Connector %s", (const char*)pConnector->GetIdentifierString( m_bShowDebug ) );
			m_ControlWindow.AddControl( 10000 + pConnector->GetIdentifier(), String, 10000 + pConnector->GetIdentifier(), true );
		}
	}

	LinkList* pLinkList = pDoc->GetLinkList();
	Position = pLinkList->GetHeadPosition();
	while( Position != NULL )
	{
		CLink* pLink = pLinkList->GetNext( Position );
		if( pLink == 0 )
			continue;
		if( !pLink->IsActuator() )
			continue;
		if( SimulationControl == INDIVIDUAL || pLink->IsAlwaysManual() )
		{
			CString String;
			String.Format( "Link %s", pLink->GetIdentifierString( m_bShowDebug ) );
			m_ControlWindow.AddControl( pLink->GetIdentifier(), String, pLink->GetIdentifier(), false, pLink->GetCPM() >= 0 ? 0.0 : 1.0 );
		}
	}

	int Height = m_ControlWindow.GetDesiredHeight();
	if( Height > m_DrawingRect.Height() )
		Height = m_DrawingRect.Height();
	m_ControlWindow.SetWindowPos( 0, 0, m_DrawingRect.Height() - Height, m_DrawingRect.Width(), m_DrawingRect.Height(), SWP_NOZORDER );
}

void CLinkageView::OnControlWindow()
{
}

void CLinkageView::OnUpdateMechanismQuicksim(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( !m_bSimulating && m_bAllowEdit );
}

void CLinkageView::OnUpdateMechanismSimulate(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( !m_bSimulating && m_bAllowEdit );
}

void CLinkageView::StepSimulation( enum _SimulationControl SimulationControl )
{
	int ControlCount = m_ControlWindow.GetControlCount();
	if( ( SimulationControl == GLOBAL || SimulationControl == INDIVIDUAL ) && ControlCount == 0 )
		return;

	int *pControlIDs = 0;
	double *pPositions = 0;
	if( ControlCount > 0 )
	{
		pControlIDs = new int[ControlCount];
		if( pControlIDs == 0 )
			return;
		pPositions = new double[ControlCount];
		if( pPositions == 0 )
		{
			delete [] pControlIDs;
			return;
		}
		for( int Index = 0; Index < ControlCount; ++Index )
		{
			pControlIDs[Index] = m_ControlWindow.GetControlId( Index );
			pPositions[Index] = m_ControlWindow.GetControlPosition( Index );
		}
	}

	CLinkageDoc* pDoc = GetDocument();

	DWORD SimulationTimeLimit = 100;
	DWORD SimulationStartTicks = GetTickCount();

	bool bSetToAbsoluteStep = false;

	if( SimulationControl == AUTO )
		m_SimulationSteps = 1;
	else if( SimulationControl == GLOBAL )
	{
		static const int MAX_MANUAL_STEPS = 800;
		int Steps = min( m_Simulator.GetSimulationSteps( pDoc ), MAX_MANUAL_STEPS );
		double Position = m_ControlWindow.GetControlPosition( 0 );
		m_SimulationSteps = (int)( Steps * Position );
		bSetToAbsoluteStep = true;
	}
	else if( SimulationControl == INDIVIDUAL )
	{
//		if( ControlCount > 0 )
//			m_SimulationSteps = m_Simulator.GetUnfinishedSimulationSteps();
	}

	//if( SimulationControl != INDIVIDUAL )
	//	ControlCount = 0;

	if( m_SimulationSteps != 0 || SimulationControl == INDIVIDUAL || bSetToAbsoluteStep )
	{
		ClearDebugItems();
		m_Simulator.SimulateStep( pDoc, m_SimulationSteps, bSetToAbsoluteStep, pControlIDs, pPositions, ControlCount, AnyAlwaysManual() && SimulationControl != INDIVIDUAL );
	}

	m_SimulationSteps = 0;

	if( pControlIDs != 0 )
		delete [] pControlIDs;
	if( pPositions != 0 )
		delete [] pPositions;
}

void CLinkageView::OnTimer(UINT_PTR nIDEvent)
{
	if( !m_bSimulating )
		return;

	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// Step the simuation as long as the video is not getting extra pause frames before or after.
	if( !m_bRecordingVideo || !m_bUseVideoCounters || ( m_VideoStartFrames == 0 && m_VideoRecordFrames != 0 ) )
		StepSimulation( m_SimulationControl );

	// Redraw without invalidation. This makes things a little more exact.
	//InvalidateRect( 0 );

	CClientDC dc( this );
	OnDraw( &dc );

	if( m_bRecordingVideo && m_pAvi != 0 && m_pAvi->GetLastError() != 0 )
	{
		CString AviError = m_pAvi->GetLastErrorMessage();
		StopMechanismSimulate();
		AfxMessageBox( AviError, MB_ICONEXCLAMATION | MB_OK );
		return;
	}

	if( !m_Simulator.IsSimulationValid() && m_bWasValid )
	{
		SetStatusText( "Invalid mechanism. Simulation paused." );
		m_bWasValid = false;
	}

	TickCount = GetTickCount() - TickCount;
	if( TickCount > 15 )
		TickCount = 15;

	if( m_bRecordingVideo )
	{
		TickCount = 0;
		if( m_bUseVideoCounters )
		{
			CString Status;
			int TotalFrames = m_VideoStartFrames + m_VideoRecordFrames + m_VideoEndFrames;
			double Seconds = (double)TotalFrames / 30.0;
			Status.Format( "Recording simulation video. %.1lf more second%s of video to go.", Seconds, Seconds == 1.0 ? "" : "s" );

			if( m_VideoStartFrames > 0 )
				--m_VideoStartFrames;
			else if( m_VideoRecordFrames > 0 )
				--m_VideoRecordFrames;
			else if( m_VideoEndFrames > 0 )
				--m_VideoEndFrames;

			if( !Status.IsEmpty() )
				SetStatusText( Status );

			if( m_VideoStartFrames == 0 && m_VideoRecordFrames == 0 && m_VideoEndFrames == 0 )
				StopMechanismSimulate();
		}
	}

	if( !m_bSimulating )
		return;

	m_TimerID = timeSetEvent( 33 - TickCount, 1, TimeProc, (DWORD_PTR)this, 0 );
}

void CLinkageView::OnUpdateMechanismReset(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( !m_bSimulating && m_bAllowEdit );
}

void CLinkageView::OnUpdateAlignButton(CCmdUI *pCmdUI)
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	int Selected = pDoc->GetSelectedConnectorCount();
	if( pDoc->GetSelectedLinkCount( true ) > 0 )
		Selected += 2;

	bool bEnable = pDoc->GetAlignConnectorCount() > 1
				   || Selected > 1
				   || pDoc->IsSelectionTriangle()
				   || pDoc->IsSelectionRectangle();

	pCmdUI->Enable( !m_bSimulating && bEnable && m_bAllowEdit );
}

void CLinkageView::OnUpdateEdit(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( !m_bSimulating && m_bAllowEdit );
}

void CLinkageView::OnUpdateNotSimulating(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( !m_bSimulating );
}

void CLinkageView::OnUpdateEditSelected(CCmdUI *pCmdUI)
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	int Selected = pDoc->GetSelectedConnectorCount() + pDoc->GetSelectedLinkCount( false );
	pCmdUI->Enable( !m_bSimulating && Selected != 0  && m_bAllowEdit );
}

void CLinkageView::OnUpdateEditConnect(CCmdUI *pCmdUI)
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pCmdUI->Enable( !m_bSimulating && pDoc->IsSelectionConnectable() && m_bAllowEdit );
}

void CLinkageView::OnUpdateEditFasten(CCmdUI *pCmdUI)
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pCmdUI->Enable( !m_bSimulating && pDoc->IsSelectionFastenable() && m_bAllowEdit );
}

void CLinkageView::OnUpdateEditUnfasten(CCmdUI *pCmdUI)
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pCmdUI->Enable( !m_bSimulating && pDoc->IsSelectionUnfastenable() && m_bAllowEdit );
}

void CLinkageView::OnUpdateEditSetRatio(CCmdUI *pCmdUI)
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pCmdUI->Enable( !m_bSimulating && pDoc->IsSelectionMeshableGears() && m_bAllowEdit );
}

void CLinkageView::OnUpdateEditJoin(CCmdUI *pCmdUI)
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pCmdUI->Enable( !m_bSimulating && pDoc->IsSelectionJoinable() && m_bAllowEdit );
}

void CLinkageView::OnUpdateEditLock(CCmdUI *pCmdUI)
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pCmdUI->Enable( !m_bSimulating && pDoc->IsSelectionLockable() && m_bAllowEdit );
}

void CLinkageView::OnUpdateEditCombine(CCmdUI *pCmdUI)
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pCmdUI->Enable( !m_bSimulating && pDoc->IsSelectionCombinable() && m_bAllowEdit );
}

void CLinkageView::OnEditCombine()
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->CombineSelected();
	InvalidateRect( 0 );
}

afx_msg void CLinkageView::OnEditmakeAnchor()
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->MakeAnchorSelected();
	InvalidateRect( 0 );
}

afx_msg void CLinkageView::OnUpdateEditmakeAnchor(CCmdUI *pCmdUI)
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pCmdUI->Enable( !m_bSimulating && pDoc->IsSelectionMakeAnchor() && m_bAllowEdit );
}

void CLinkageView::OnEditConnect()
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->ConnectSelected();
	InvalidateRect( 0 );
}

void CLinkageView::OnEditFasten()
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->FastenSelected();
	InvalidateRect( 0 );
}

void CLinkageView::OnEditUnfasten()
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->UnfastenSelected();
	InvalidateRect( 0 );
}

void CLinkageView::OnEditSetRatio()
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if( !pDoc->IsSelectionMeshableGears() )
		return;

	CGearRatioDialog dlg;

	// We know that there an be only two links selected at this time.
	CLink *pLink1 = pDoc->GetSelectedLink( 0, false );
	CLink *pLink2 = pDoc->GetSelectedLink( 1, false );

	CGearConnection *pConnection = pDoc->GetGearRatio( pLink1, pLink2, &dlg.m_Gear1Size, &dlg.m_Gear2Size );

	if( pConnection == 0 )
	{
		dlg.m_Gear1Size = 1.0;
		dlg.m_Gear2Size = 1.0;
		dlg.m_GearChainSelection = 0;
		dlg.m_bUseRadiusValues = false;
	}
	else
	{
		dlg.m_GearChainSelection = pConnection->m_ConnectionType == CGearConnection::GEARS ? 0 : 1;
		dlg.m_bUseRadiusValues = pConnection->m_bUseSizeAsRadius;
	}

	dlg.m_Link1Name = pLink1->GetIdentifierString( m_bShowDebug );
	dlg.m_Link2Name = pLink2->GetIdentifierString( m_bShowDebug );

	if( dlg.DoModal() == IDOK )
	{
		if( pDoc->SetGearRatio( pLink1, pLink2, dlg.m_Gear1Size, dlg.m_Gear2Size, dlg.m_bUseRadiusValues && dlg.m_GearChainSelection == 1, dlg.m_GearChainSelection == 0 ? CGearConnection::GEARS : CGearConnection::CHAIN ) )
			UpdateForDocumentChange();
	}
	InvalidateRect( 0 );
}

void CLinkageView::OnEditDeleteselected()
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->DeleteSelected();
	UpdateForDocumentChange();
}

BOOL CLinkageView::OnEraseBkgnd(CDC* pDC)
{
	return 0;
}

void CLinkageView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	SetScrollExtents();

	int Height = m_ControlWindow.GetDesiredHeight();
	if( Height > cy )
		Height = cy;
	m_ControlWindow.SetWindowPos( 0, 0, cy - Height, cx, cy, SWP_NOZORDER );

	GetClientRect( &m_DrawingRect );
}

void CLinkageView::OnUpdateButtonRun(CCmdUI *pCmdUI)
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pCmdUI->Enable( ( !m_bSimulating || m_SimulationControl == STEP ) && !pDoc->IsEmpty() && m_bAllowEdit );
}

void CLinkageView::OnUpdateButtonStop(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( m_bSimulating );
}

void CLinkageView::OnButtonRun()
{
	if( m_bSimulating )
	{
		if( m_SimulationControl == STEP )
			m_SimulationControl = AUTO;
	}
	else
	{
		ConfigureControlWindow( AUTO );
		StartMechanismSimulate( AUTO );
	}
}

void CLinkageView::StopSimulation()
{
	StopMechanismSimulate();
}

void CLinkageView::OnButtonStop()
{
	StopMechanismSimulate();
}

void CLinkageView::OnButtonPin()
{
	StopMechanismSimulate( true );
}

void CLinkageView::OnUpdateButtonPin(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( m_bSimulating );
}

CFPoint CLinkageView::GetDocumentViewCenter( void )
{
	CFPoint Point( ( m_DrawingRect.left + m_DrawingRect.right ) / 2, ( m_DrawingRect.top + m_DrawingRect.bottom ) / 2 );

	return Unscale( Point );
}

void CLinkageView::InsertPoint( CFPoint *pPoint )
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->InsertConnector( CLinkageDoc::DRAWINGLAYER, Unscale( LINK_INSERT_PIXELS ), pPoint == 0 ? GetDocumentViewCenter() : *pPoint, pPoint != 0 );
	m_bSuperHighlight = true;

	UpdateForDocumentChange();
}

void CLinkageView::InsertLine( CFPoint *pPoint )
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->InsertLink( CLinkageDoc::DRAWINGLAYER, Unscale( LINK_INSERT_PIXELS ), pPoint == 0 ? GetDocumentViewCenter() : *pPoint, pPoint != 0, 2, false );
	m_bSuperHighlight = true;

	UpdateForDocumentChange();
}

void CLinkageView::InsertMeasurement( CFPoint *pPoint )
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->InsertMeasurement( CLinkageDoc::DRAWINGLAYER, Unscale( LINK_INSERT_PIXELS ), pPoint == 0 ? GetDocumentViewCenter() : *pPoint, pPoint != 0 );
	m_bSuperHighlight = true;

	UpdateForDocumentChange();
}

void CLinkageView::InsertGear( CFPoint *pPoint )
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->InsertGear( CLinkageDoc::MECHANISMLAYER, Unscale( LINK_INSERT_PIXELS ), pPoint == 0 ? GetDocumentViewCenter() : *pPoint, pPoint != 0 );
	m_bSuperHighlight = true;

	UpdateForDocumentChange();
}

void CLinkageView::InsertConnector( CFPoint *pPoint )
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->InsertConnector( CLinkageDoc::MECHANISMLAYER, Unscale( LINK_INSERT_PIXELS ), pPoint == 0 ? GetDocumentViewCenter() : *pPoint, pPoint != 0 );
	m_bSuperHighlight = true;

	UpdateForDocumentChange();
}

void CLinkageView::InsertAnchor( CFPoint *pPoint )
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->InsertAnchor( CLinkageDoc::MECHANISMLAYER, Unscale( LINK_INSERT_PIXELS ), pPoint == 0 ? GetDocumentViewCenter() : *pPoint, pPoint != 0, m_bNewLinksSolid );
	m_bSuperHighlight = true;

	UpdateForDocumentChange();
}

void CLinkageView::InsertAnchorLink( CFPoint *pPoint )
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->InsertAnchorLink(  CLinkageDoc::MECHANISMLAYER, Unscale( LINK_INSERT_PIXELS ), pPoint == 0 ? GetDocumentViewCenter() : *pPoint, pPoint != 0, m_bNewLinksSolid );
	m_bSuperHighlight = true;

	UpdateForDocumentChange();
}

void CLinkageView::InsertRotatinganchor( CFPoint *pPoint )
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->InsertRotateAnchor(  CLinkageDoc::MECHANISMLAYER, Unscale( LINK_INSERT_PIXELS ), pPoint == 0 ? GetDocumentViewCenter() : *pPoint, pPoint != 0, m_bNewLinksSolid );
	m_bSuperHighlight = true;

	UpdateForDocumentChange();
}

void CLinkageView::InsertLink( CFPoint *pPoint )
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->InsertLink( CLinkageDoc::MECHANISMLAYER, Unscale( LINK_INSERT_PIXELS ), pPoint == 0 ? GetDocumentViewCenter() : *pPoint, pPoint != 0, 2, m_bNewLinksSolid );
	m_bSuperHighlight = true;

	UpdateForDocumentChange();
}

void CLinkageView::OnUpdateEditSplit(CCmdUI *pCmdUI)
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pCmdUI->Enable( !m_bSimulating && pDoc->IsSelectionSplittable() && m_bAllowEdit );
}

void CLinkageView::OnUpdateEditProperties(CCmdUI *pCmdUI)
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if( pDoc->GetSelectedLinkCount( true ) == 1  )
		pCmdUI->Enable( true );
	else if( pDoc->GetSelectedLinkCount( false ) == 1 && pDoc->GetSelectedConnectorCount() == 0 )
		pCmdUI->Enable( true );
	else if( pDoc->GetSelectedLinkCount( true ) > 1 )
		pCmdUI->Enable( true );
	else if( pDoc->GetSelectedConnectorCount() == 1 )
		pCmdUI->Enable( true );
	else
		pCmdUI->Enable( false );
}

class CSelectedConnector : public ConnectorListOperation
{
	public:
	CConnector *m_pSelectedConnector;
	CSelectedConnector() { m_pSelectedConnector = 0; }
	bool operator()( class CConnector *pConnector, bool bFirst, bool bLast )
	{
		if( pConnector->IsSelected() )
		{
			m_pSelectedConnector = pConnector;
			return false;
		}
		return true;
	}
	CConnector *GetConnector( void ) { return m_pSelectedConnector; }
};

void CLinkageView::EditProperties( CConnector *pConnector, CLink *pLink, bool bSelectedLinks )
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if( pLink != 0 || bSelectedLinks )
	{
		int Layers = 0;
		int Count = 0;
		if( bSelectedLinks )
		{
			LinkList* pLinkList = pDoc->GetLinkList();
			POSITION Position = pLinkList->GetHeadPosition();
			while( Position != NULL )
			{
				pLink = pLinkList->GetNext( Position );
				if( pLink != 0 && pLink->IsSelected() )
				{
					Layers |= pLink->GetLayers();
					++Count;
				}
			}
		}
		else
		{
			Layers = pLink->GetLayers();
			Count = 1;
		}
		if( pLink != 0 )
		{
			bool bResult = false;
			if( Layers & CLinkageDoc::DRAWINGLAYER )
				bResult = LineProperties( Count > 1 ? 0 : pLink );
			else
				bResult = LinkProperties( Count > 1 ? 0 : pLink );
			if( bResult )
			{
				pDoc->SetModifiedFlag( true );
				SetScrollExtents();
				InvalidateRect( 0 );
			}
			return;
		}
	}

	if( pConnector != 0 )
	{
		bool bResult = false;
		if( pConnector->GetLayers() & CLinkageDoc::DRAWINGLAYER )
			bResult = PointProperties( pConnector );
		else
			bResult = ConnectorProperties( pConnector );
		if( bResult )
		{
			pDoc->SetModifiedFlag( true );
			SetScrollExtents();
			InvalidateRect( 0 );
		}
		return;
	}
}

void CLinkageView::OnEditProperties()
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if( pDoc->GetSelectedLinkCount( true ) == 1  )
		EditProperties( 0, pDoc->GetSelectedLink( 0, true ), false );
	else if( pDoc->GetSelectedLinkCount( false ) == 1 && pDoc->GetSelectedConnectorCount() == 0 )
		EditProperties( 0, pDoc->GetSelectedLink( 0, false ), false );
	else if( pDoc->GetSelectedLinkCount( true ) > 1 )
		EditProperties( 0, 0, true );
	else if( pDoc->GetSelectedConnectorCount() == 1 )
		EditProperties( pDoc->GetSelectedConnector( 0 ), 0, false );
}

void CLinkageView::OnLButtonDblClk(UINT nFlags, CPoint MousePoint)
{
	CFPoint point = AdjustClientAreaPoint( MousePoint );

	if( m_VisibleAdjustment == ADJUSTMENT_ROTATE )
	{
		// Must have been selected before double click - go back to
		// stretch mode.
		m_VisibleAdjustment = ADJUSTMENT_STRETCH;
		InvalidateRect( 0 );
		UpdateWindow();
	}
	OnEditProperties();
}

void CLinkageView::OnEditJoin()
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->JoinSelected( true );
	InvalidateRect( 0 );
}

void CLinkageView::OnEditLock()
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->LockSelected();
	InvalidateRect( 0 );
}

void CLinkageView::OnEditSelectall()
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->SelectAll();
	int TotalSelected = pDoc->GetSelectedLinkCount( false ) + pDoc->GetSelectedConnectorCount();
	if( TotalSelected > 0 )
	{
		m_SelectionContainerRect = GetDocumentArea( false, true );
		m_SelectionAdjustmentRect = GetDocumentAdjustArea( true );
		m_SelectionRotatePoint.SetPoint( ( m_SelectionContainerRect.left + m_SelectionContainerRect.right ) / 2,
		                                 ( m_SelectionContainerRect.top + m_SelectionContainerRect.bottom ) / 2 );
		m_VisibleAdjustment = ADJUSTMENT_STRETCH;
	}
	InvalidateRect( 0 );
}

void CLinkageView::OnUpdateSelectall( CCmdUI *pCmdUI )
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pCmdUI->Enable( m_bAllowEdit && !m_bSimulating && !pDoc->IsEmpty() );
}

void CLinkageView::OnAlignHorizontal()
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->AlignSelected( pDoc->HORIZONTAL );
	UpdateForDocumentChange();
}

void CLinkageView::OnUpdateAlignHorizontal( CCmdUI *pCmdUI )
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pCmdUI->Enable( !m_bSimulating && pDoc->GetAlignConnectorCount() > 1 && m_bAllowEdit );
}

void CLinkageView::OnFlipHorizontal()
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->AlignSelected( pDoc->FLIPHORIZONTAL );
	UpdateForDocumentChange();
}

void CLinkageView::OnUpdateFlipHorizontal( CCmdUI *pCmdUI )
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	int Selected = pDoc->GetSelectedConnectorCount();
	if( pDoc->GetSelectedLinkCount( true ) > 0 )
		Selected += 2;
	pCmdUI->Enable( !m_bSimulating && Selected > 1 && m_bAllowEdit );
}

void CLinkageView::OnFlipVertical()
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->AlignSelected( pDoc->FLIPVERTICAL );
	UpdateForDocumentChange();
}

void CLinkageView::OnUpdateFlipVertical( CCmdUI *pCmdUI )
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	int Selected = pDoc->GetSelectedConnectorCount();
	if( pDoc->GetSelectedLinkCount( true ) > 0 )
		Selected += 2;
	pCmdUI->Enable( !m_bSimulating && Selected > 1 && m_bAllowEdit );
}

void CLinkageView::OnAlignVertical()
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->AlignSelected( pDoc->VERTICAL );
	UpdateForDocumentChange();
}

void CLinkageView::OnUpdateAlignVertical( CCmdUI *pCmdUI )
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pCmdUI->Enable( !m_bSimulating && pDoc->GetAlignConnectorCount() > 1 && m_bAllowEdit );
}

void CLinkageView::OnAlignLineup()
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->AlignSelected( pDoc->INLINE );
	UpdateForDocumentChange();
}

void CLinkageView::OnUpdateAlignLineup( CCmdUI *pCmdUI )
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pCmdUI->Enable( pDoc->GetAlignConnectorCount() > 2 && m_bAllowEdit );
}

void CLinkageView::OnAlignEvenSpace()
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->AlignSelected( pDoc->INLINESPACED );
	UpdateForDocumentChange();
}

void CLinkageView::OnUpdateAlignEvenSpace( CCmdUI *pCmdUI )
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pCmdUI->Enable( pDoc->GetAlignConnectorCount() > 2 && m_bAllowEdit );
}

void CLinkageView::OnAlignRightAngle()
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->MakeRightAngleSelected();
	UpdateForDocumentChange();
}

void CLinkageView::OnUpdateAlignRightAngle( CCmdUI *pCmdUI )
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	CConnector *pThird = pDoc->GetSelectedConnector( 2 );
	pCmdUI->Enable( !m_bSimulating && pDoc->IsSelectionTriangle() && pThird != 0 && !pThird->IsSlider() && m_bAllowEdit );
}

void CLinkageView::OnAlignSetAngle()
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if( !pDoc->IsSelectionTriangle() )
		return;

	CConnector *pConnector0 = (CConnector*)pDoc->GetSelectedConnector( 0 );
	CConnector *pConnector1 = (CConnector*)pDoc->GetSelectedConnector( 1 );
	CConnector *pConnector2 = (CConnector*)pDoc->GetSelectedConnector( 2 );

	if( pConnector2 != 0 && pConnector2->IsSlider() )
		return;

	double Angle = GetAngle( pConnector1->GetOriginalPoint(), pConnector0->GetOriginalPoint(), pConnector2->GetOriginalPoint() );
	if( Angle > 180 )
		Angle = 360 - Angle;

	CAngleDialog dlg;
	dlg.m_Angle = Angle;

	if( dlg.DoModal() )
	{
		pDoc->MakeSelectedAtAngle( dlg.m_Angle );
		UpdateForDocumentChange();
	}
}

void CLinkageView::OnUpdateAlignSetAngle( CCmdUI *pCmdUI )
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	CConnector *pThird = pDoc->GetSelectedConnector( 2 );
	pCmdUI->Enable( !m_bSimulating && pDoc->IsSelectionTriangle() && pThird != 0 && !pThird->IsSlider() && m_bAllowEdit );
}

void CLinkageView::OnAddConnector()
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if( pDoc->AddConnectorToSelected( Scale( GRAB_DISTANCE * 4 ) ) )
		UpdateForDocumentChange();
}

void CLinkageView::OnUpdateAddConnector( CCmdUI *pCmdUI )
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pCmdUI->Enable( pDoc->CanAddConnector() && m_bAllowEdit );
}

void CLinkageView::OnAlignRectangle()
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->MakeParallelogramSelected( true );
	UpdateForDocumentChange();
}

void CLinkageView::OnUpdateAlignRectangle( CCmdUI *pCmdUI )
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pCmdUI->Enable( !m_bSimulating && pDoc->IsSelectionRectangle() && m_bAllowEdit );
}

void CLinkageView::OnAlignParallelogram()
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->MakeParallelogramSelected( false );
	UpdateForDocumentChange();
}

void CLinkageView::OnUpdateAlignParallelogram( CCmdUI *pCmdUI )
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pCmdUI->Enable( !m_bSimulating && pDoc->IsSelectionRectangle() && m_bAllowEdit );
}

void CLinkageView::OnEditSplit()
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->SplitSelected();
	UpdateForDocumentChange();
}

void CLinkageView::InsertDouble( CFPoint *pPoint )
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	pDoc->InsertLink(  CLinkageDoc::MECHANISMLAYER, Unscale( LINK_INSERT_PIXELS ), pPoint == 0 ? GetDocumentViewCenter() : *pPoint, pPoint != 0, 2, m_bNewLinksSolid );
	m_bSuperHighlight = true;
	UpdateForDocumentChange();
}

void CLinkageView::InsertTriple( CFPoint *pPoint )
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->InsertLink(  CLinkageDoc::MECHANISMLAYER, Unscale( LINK_INSERT_PIXELS ), pPoint == 0 ? GetDocumentViewCenter() : *pPoint, pPoint != 0, 3, m_bNewLinksSolid );
	m_bSuperHighlight = true;
	UpdateForDocumentChange();
}

void CLinkageView::InsertQuad( CFPoint *pPoint )
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->InsertLink(  CLinkageDoc::MECHANISMLAYER, Unscale( LINK_INSERT_PIXELS ), pPoint == 0 ? GetDocumentViewCenter() : *pPoint, pPoint != 0, 4, m_bNewLinksSolid );
	m_bSuperHighlight = true;
	UpdateForDocumentChange();
}

void CLinkageView::OnViewLabels()
{
	m_bShowLabels = !m_bShowLabels;
	SaveSettings();
	InvalidateRect( 0 );
}

void CLinkageView::OnViewAngles()
{
	m_bShowAngles = !m_bShowAngles;
	SaveSettings();
	InvalidateRect( 0 );
}

void CLinkageView::OnUpdateViewLabels(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck( m_bShowLabels );
	pCmdUI->Enable( !m_bSimulating );
}

void CLinkageView::OnUpdateViewAngles(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck( m_bShowAngles );
	pCmdUI->Enable( !m_bSimulating );
}

void CLinkageView::OnEditSnap()
{
	m_bSnapOn = !m_bSnapOn;
	SaveSettings();
}

void CLinkageView::OnUpdateEditSnap(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck( m_bSnapOn );
	pCmdUI->Enable( !m_bSimulating );
}

void CLinkageView::OnEditGridSnap()
{
	m_bGridSnap = !m_bGridSnap;
	SaveSettings();
}

void CLinkageView::OnUpdateEditGridSnap(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck( m_bGridSnap );
	pCmdUI->Enable( !m_bSimulating );
}

void CLinkageView::OnEditAutoJoin()
{
	m_bAutoJoin = !m_bAutoJoin;
	SaveSettings();
}

void CLinkageView::OnUpdateEditAutoJoin(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck( m_bAutoJoin );
	pCmdUI->Enable( !m_bSimulating && m_bSnapOn );
}

void CLinkageView::OnViewData()
{
	m_bShowData = !m_bShowData;
	SaveSettings();
	InvalidateRect( 0 );
}

void CLinkageView::OnUpdateViewData(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck( m_bShowData );
	pCmdUI->Enable( !m_bSimulating );
}

void CLinkageView::OnViewDimensions()
{
	m_bShowDimensions = !m_bShowDimensions;
	SaveSettings();
	InvalidateRect( 0 );
}

void CLinkageView::OnUpdateViewDimensions(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck( m_bShowDimensions );
	pCmdUI->Enable( !m_bSimulating );
}

void CLinkageView::OnViewGroundDimensions()
{
	m_bShowGroundDimensions = !m_bShowGroundDimensions;
	SaveSettings();
	InvalidateRect( 0 );
}

void CLinkageView::OnUpdateViewGroundDimensions(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck( m_bShowGroundDimensions );
	pCmdUI->Enable( !m_bSimulating );
}

void CLinkageView::OnViewSolidLinks()
{
	m_bNewLinksSolid = !m_bNewLinksSolid;
	SaveSettings();
	InvalidateRect( 0 );
}

void CLinkageView::OnUpdateViewSolidLinks(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck( m_bNewLinksSolid );
	pCmdUI->Enable( !m_bSimulating );
}

void CLinkageView::OnViewDrawing()
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if( m_SelectedViewLayers & CLinkageDoc::DRAWINGLAYER )
		m_SelectedViewLayers &= ~CLinkageDoc::DRAWINGLAYER;
	else
		m_SelectedViewLayers |= CLinkageDoc::DRAWINGLAYER;
	SaveSettings();

	InvalidateRect( 0 );
}

void CLinkageView::OnUpdateViewDrawing(CCmdUI *pCmdUI)
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	pCmdUI->SetCheck( ( m_SelectedViewLayers & CLinkageDoc::DRAWINGLAYER ) != 0 ? 1 : 0 );
	pCmdUI->Enable( !m_bSimulating );
}

void CLinkageView::OnViewMechanism()
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if( m_SelectedViewLayers & CLinkageDoc::MECHANISMLAYERS )
		m_SelectedViewLayers &= ~CLinkageDoc::MECHANISMLAYERS;
	else
		m_SelectedViewLayers |= CLinkageDoc::MECHANISMLAYERS;
	SaveSettings();

	InvalidateRect( 0 );
}

void CLinkageView::OnUpdateViewMechanism(CCmdUI *pCmdUI)
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	pCmdUI->SetCheck( ( m_SelectedViewLayers & CLinkageDoc::MECHANISMLAYERS ) != 0 ? 1 : 0 );
	pCmdUI->Enable( !m_bSimulating );
}

void CLinkageView::OnEditDrawing()
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if( m_SelectedEditLayers & CLinkageDoc::DRAWINGLAYER )
		m_SelectedEditLayers &= ~CLinkageDoc::DRAWINGLAYER;
	else
		m_SelectedEditLayers |= CLinkageDoc::DRAWINGLAYER;
	SaveSettings();

	InvalidateRect( 0 );
}

void CLinkageView::OnUpdateEditDrawing(CCmdUI *pCmdUI)
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	pCmdUI->SetCheck( ( m_SelectedViewLayers & CLinkageDoc::DRAWINGLAYER ) != 0 && ( m_SelectedEditLayers & CLinkageDoc::DRAWINGLAYER ) != 0 ? 1 : 0 );
	pCmdUI->Enable( !m_bSimulating && ( m_SelectedViewLayers & CLinkageDoc::DRAWINGLAYER ) != 0 );
}

void CLinkageView::OnEditMechanism()
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	unsigned int CurrentEditLayers = m_SelectedEditLayers; //pDoc->GetEditLayers();

	if( m_SelectedEditLayers & CLinkageDoc::MECHANISMLAYERS )
		m_SelectedEditLayers &= ~CLinkageDoc::MECHANISMLAYERS;
	else
		m_SelectedEditLayers |= CLinkageDoc::MECHANISMLAYERS;
	SaveSettings();

	InvalidateRect( 0 );
}

void CLinkageView::OnUpdateEditMechanism(CCmdUI *pCmdUI)
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	pCmdUI->SetCheck( ( m_SelectedViewLayers & CLinkageDoc::MECHANISMLAYERS ) != 0 && ( m_SelectedEditLayers & CLinkageDoc::MECHANISMLAYERS ) != 0 ? 1 : 0 );
	pCmdUI->Enable( !m_bSimulating && ( m_SelectedViewLayers & CLinkageDoc::MECHANISMLAYERS ) != 0 );
}

void CLinkageView::OnViewDebug()
{
	m_bShowDebug = !m_bShowDebug;
	SaveSettings();
	InvalidateRect( 0 );
}

void CLinkageView::OnViewBold()
{
	m_bShowBold = !m_bShowBold;
	SaveSettings();
	InvalidateRect( 0 );
}

void CLinkageView::OnUpdateViewAnicrop(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck( m_bShowAnicrop );
	pCmdUI->Enable( !m_bSimulating );
}

void CLinkageView::OnViewAnicrop()
{
	m_bShowAnicrop = !m_bShowAnicrop;
	SaveSettings();
	InvalidateRect( 0 );
}

void CLinkageView::OnUpdateViewLargeFont(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck( m_bShowLargeFont );
	pCmdUI->Enable( !m_bSimulating );
}

void CLinkageView::OnViewLargeFont()
{
	m_bShowLargeFont = !m_bShowLargeFont;
	SetupFont();
	SaveSettings();
	InvalidateRect( 0 );
}

void CLinkageView::OnUpdateViewGrid(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck( m_bShowGrid );
	pCmdUI->Enable( !m_bSimulating );
}

void CLinkageView::OnViewGrid()
{
	m_bShowGrid = !m_bShowGrid;
	SaveSettings();
	InvalidateRect( 0 );
}

void CLinkageView::OnUpdateViewParts(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck( m_bShowParts );
	pCmdUI->Enable( !m_bSimulating );
}

void CLinkageView::OnViewParts()
{
	m_bShowParts = !m_bShowParts;
	m_bAllowEdit = !m_bShowParts;
	if( m_bShowParts )
	{
		CLinkageDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		pDoc->SelectElement();
	}
	GetSetGroundDimensionVisbility( false );
	SaveSettings();
	InvalidateRect( 0 );
}

void CLinkageView::OnUpdateViewDebug(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck( m_bShowDebug );
	pCmdUI->Enable( !m_bSimulating );
}

void CLinkageView::OnUpdateViewBold(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck( m_bShowBold );
	pCmdUI->Enable( !m_bSimulating );
}

void CLinkageView::OnEditCut()
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->Copy( true );
	UpdateForDocumentChange();
}

void CLinkageView::OnEditCopy()
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->Copy( false );
}

void CLinkageView::OnEditPaste()
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->Paste();
	m_VisibleAdjustment = ADJUSTMENT_STRETCH;
	m_SelectionContainerRect = GetDocumentArea( false, true );
	m_SelectionAdjustmentRect = GetDocumentAdjustArea( true );
	m_SelectionRotatePoint.SetPoint( ( m_SelectionContainerRect.left + m_SelectionContainerRect.right ) / 2,
									 ( m_SelectionContainerRect.top + m_SelectionContainerRect.bottom ) / 2 );
	UpdateForDocumentChange();
}

void CLinkageView::OnUpdateEditPaste(CCmdUI *pCmdUI)
{
    UINT CF_Linkage = RegisterClipboardFormat( "RECTORSQUID_Linkage_CLIPBOARD_XML_FORMAT" );
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pCmdUI->Enable( !m_bSimulating && ::IsClipboardFormatAvailable( CF_Linkage ) != 0 && m_bAllowEdit );
}

void CLinkageView::OnRButtonDown(UINT nFlags, CPoint MousePoint)
{
	CFPoint point = AdjustClientAreaPoint( MousePoint );

	SetCapture();
	m_bSuperHighlight = 0;
	m_MouseAction = ACTION_PAN;
	m_PreviousDragPoint = point;
	m_DragStartPoint = point;
	m_bMouseMovedEnough	= false;
}

void CLinkageView::OnRButtonUp(UINT nFlags, CPoint MousePoint)
{
	ReleaseCapture();
	m_MouseAction = ACTION_NONE;
	SetScrollExtents();

	if( m_bSimulating )
		return;

	if( !m_bMouseMovedEnough && m_bAllowEdit )
	{
		CLinkageDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		CLink *pLink = 0;
		CConnector *pConnector = 0;
		CFPoint point = AdjustClientAreaPoint( MousePoint );
		if( FindDocumentItem( point, pLink, pConnector ) )
		{
			bool bResult = false;
			if( pConnector != 0 )
				EditProperties( pConnector, 0, false );
			else if( pLink != 0 )
				EditProperties( 0, pLink, false );
		}
		else
		{
			m_PopupPoint = point;
			ClientToScreen( &MousePoint );
			if( m_pPopupGallery != 0 )
			{
				m_pPopupGallery->ShowGallery( this, MousePoint.x, MousePoint.y, 0 );
			}
		}
	}
}

BOOL CLinkageView::OnMouseWheel(UINT nFlags, short zDelta, CPoint MousePoint)
{
	if( m_bSimulating )
		return TRUE;

	if( zDelta == 0 )
		return TRUE;
	if( zDelta > 0 && m_ScreenZoom >= 256 )
		return TRUE;
	else if( zDelta < 0 && m_ScreenZoom <= .0005 )
		return TRUE;

	ScreenToClient( &MousePoint );
	CFPoint pt = AdjustClientAreaPoint( MousePoint );

	m_Zoom = m_ScreenZoom;
	CFPoint AdjustedPoint = Unscale( pt );

	if( zDelta > 0 )
		m_ScreenZoom *= ZOOM_AMOUNT;
	else if( zDelta < 0 )
		m_ScreenZoom /= ZOOM_AMOUNT;

	m_Zoom = m_ScreenZoom;
	CFPoint AdjustedPoint2 = Unscale( pt );

	double dx = ( AdjustedPoint2.x - AdjustedPoint.x ) * m_ScreenZoom;
	double dy = ( AdjustedPoint2.y - AdjustedPoint.y ) * m_ScreenZoom;

	m_ScreenScrollPosition.x -= (int)dx;
	m_ScreenScrollPosition.y += (int)dy;

	m_ScrollPosition = m_ScreenScrollPosition;

	SetScrollExtents();
	InvalidateRect( 0 );
	return TRUE;
}

CPoint CLinkageView::GetDefaultUnscalePoint( void )
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	return CPoint( m_DrawingRect.Width() / 2, m_DrawingRect.Height() / 2 );
}

void CLinkageView::OnViewZoomin()
{
	if( m_bSimulating )
		return;

	CPoint Point = GetDefaultUnscalePoint();
	ClientToScreen( &Point );
	OnMouseWheel( 0, 120, Point );
}

void CLinkageView::OnViewZoomout()
{
	if( m_bSimulating )
		return;

	CPoint Point = GetDefaultUnscalePoint();
	ClientToScreen( &Point );
	OnMouseWheel( 0, -120, Point );
}

void CLinkageView::GetDocumentViewArea( CFRect &Rect, CLinkageDoc *pDoc )
{
	if( pDoc == 0 )
	{
		CLinkageDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
	}

	CFRect Area = GetDocumentArea();
	CFRect PixelRect = Area;
	}

void CLinkageView::OnMenuZoomfit()
{
	if( m_bSimulating )
		return;

	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if( pDoc->IsEmpty() )
	{
		m_ScreenZoom = 1;
		m_ScreenScrollPosition.x = 0;
		m_ScreenScrollPosition.y = 0;
		SetScrollExtents();
		InvalidateRect( 0 );
		return;
	}

	int cx = m_DrawingRect.Width();
	int cy = m_DrawingRect.Height();

	// Some margin because the dimensions are not done well and we can't predict them just yet.
	cx -= 20;
	cy -= 20;

	CFRect Area = GetDocumentArea( m_bShowDimensions );

	// Figure out the Unscaling needed once we are at a zoom level of 1.
	double xScaleChange = cx / fabs( Area.Width() );
	double yScaleChange = cy / fabs( Area.Height() );

	double Scale = min( xScaleChange, yScaleChange );

	// But screen zoom needs to one of the "standard" zoom
	// levels that a user would see if they just srolled in and out.
	m_ScreenZoom = pow( ZOOM_AMOUNT, floor( log( Scale ) / log( ZOOM_AMOUNT ) ) );

	// Center the mechanism.
	m_ScreenScrollPosition.x = m_ScreenZoom * ( Area.left + fabs( Area.Width() ) / 2 );
	m_ScreenScrollPosition.y = m_ScreenZoom * -( Area.top + fabs( Area.Height() ) / 2 );

	SetScrollExtents();
	InvalidateRect( 0 );
}

void CLinkageView::OnUpdate( CView* pSender, LPARAM lHint, CObject* pHint )
{
	SetScrollExtents();

	CMFCRibbonBar *pRibbon = ((CFrameWndEx*) AfxGetMainWnd())->GetRibbonBar();
	if( pRibbon == 0 )
		return;
	CMFCRibbonComboBox * pComboBox = DYNAMIC_DOWNCAST( CMFCRibbonComboBox, pRibbon->FindByID( ID_VIEW_UNITS ) );
	if( pComboBox == 0 )
		return;

	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	CLinkageDoc::_Units Units = pDoc->GetUnits();

	pComboBox->SelectItem( (DWORD_PTR)Units );

	UpdateForDocumentChange();

	CView::OnUpdate( pSender, lHint, pHint );
}

void CLinkageView::SetPrinterOrientation( void )
{
	CLinkageApp *pApp = (CLinkageApp*)AfxGetApp();
	if( pApp == 0 )
		return;
	CFRect DocumentArea;
	CLinkageDoc* pDoc = GetDocument();
	DocumentArea = GetDocumentArea();

	if( fabs( DocumentArea.Width() ) >= fabs( DocumentArea.Height() ) )
		pApp->SetPrinterOrientation( DMORIENT_LANDSCAPE );
	else
		pApp->SetPrinterOrientation( DMORIENT_PORTRAIT );
}

void CLinkageView::OnFilePrintSetup()
{
	SetPrinterOrientation();

	CLinkageApp *pApp = (CLinkageApp*)AfxGetApp();
	if( pApp != 0 )
		pApp->OnFilePrintSetup();
}

void CLinkageView::OnPrintFull()
{
	m_bPrintFullSize = !m_bPrintFullSize;
	SaveSettings();
}

void CLinkageView::OnUpdatePrintFull( CCmdUI *pCmdUI )
{
	pCmdUI->SetCheck( m_bPrintFullSize );
}

BOOL CLinkageView::OnPreparePrinting(CPrintInfo* pInfo)
{
	SetPrinterOrientation();

	CDC dc ;
	AfxGetApp()->CreatePrinterDC(dc) ;

	pInfo->SetMaxPage( GetPrintPageCount( &dc, 0, m_bPrintFullSize ) );

	dc.DeleteDC() ;

	return DoPreparePrinting( pInfo );
}

void CLinkageView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	if( m_bSimulating )
		OnMechanismSimulate();
}

void CLinkageView::SetZoom( double Zoom )
{
	if( Zoom == m_ScreenZoom )
		return;
	if( Zoom > 256 )
		Zoom = 256;
	else if( Zoom <= 0.0005 )
		Zoom = 0.0005;
	m_ScreenZoom = Zoom;
	m_Zoom = m_ScreenZoom;
	SetScrollExtents();
	InvalidateRect( 0 );
}

void CLinkageView::SetOffset( CFPoint Offset )
{
	if( Offset == m_ScreenScrollPosition )
		return;
	m_ScreenScrollPosition = Offset;
	m_ScrollPosition = m_ScreenScrollPosition;
	SetScrollExtents();
	InvalidateRect( 0 );
}

void CLinkageView::OnSelectNext( void )
{
}

void CLinkageView::OnMechanismQuicksim()
{
	if( m_TimerID != 0 )
		timeKillEvent( m_TimerID );

	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	pDoc->SelectElement();
	InvalidateRect( 0 );
	m_MouseAction = ACTION_NONE;

	if( m_bSimulating )
	{
		m_bSimulating = false;
		m_TimerID = 0;
		OnMechanismReset();
	}

	// Always do the quick simulation even if it was already simulating.

	pDoc->Reset( true );
	m_Simulator.Reset();

	int Steps = m_Simulator.GetSimulationSteps( pDoc );

	if( Steps == 0 )
		return;

	++Steps; // There is always a last step needed to get back to the start location.

	bool bGoodSimulation = true;
	int Counter = 0;
	for( ; Counter < Steps && bGoodSimulation; ++Counter )
		bGoodSimulation = m_Simulator.SimulateStep( pDoc, 1, false, 0, 0, 0, false );

	// Do one more step so that the final resting place for the drawing connectors are included in the motion path.
	if( !bGoodSimulation )
		m_Simulator.SimulateStep( pDoc, 1, false, 0, 0, 0, false );

	DebugItemList.Clear();

	//pDoc->Reset( !bGoodSimulation );
	pDoc->Reset( false );
	m_Simulator.Reset();

	InvalidateRect( 0 );
}

void CLinkageView::DebugDrawConnector( CRenderer* pDC, unsigned int OnLayers, CConnector* pConnector, bool bShowlabels, bool bUnused, bool bHighlight, bool bDrawConnectedLinks )
{
	if( !m_bShowDebug )
		return;

	if( ( pConnector->GetLayers() & OnLayers ) == 0 )
		return;
}

double CLinkageView::AdjustYCoordinate( double y )
{
	return y * m_YUpDirection;
}

void CLinkageView::DrawSliderTrack( CRenderer* pRenderer, unsigned int OnLayers, CConnector* pConnector )
{
	if( ( pConnector->GetLayers() & OnLayers ) == 0 )
		return;

	if( !pConnector->IsSlider() )
		return;

	CConnector *Limit1;
	CConnector *Limit2;

	if( !pConnector->GetSlideLimits( Limit1, Limit2 ) )
		return;

	COLORREF Color = RGB( 190, 165, 165 );

	// Sort the points so that other sliders on these same points draw from the same location.
	CLink * pCommon = Limit1->GetSharingLink( Limit2 );
	if( pCommon != 0 )
	{
		if( !pCommon->IsSolid() && pConnector->GetSlideRadius() == 0.0 )
		{
			int PointCount = 0;
			CFPoint* Points = pCommon->GetHull( PointCount );
			int Found1 = -2; // -2 keeps the test later from accidentially thinking the points are along the hull.
			int Found2 = -2;
			for( int Index = 0; Index < PointCount; ++Index )
			{
				if( Limit1->GetOriginalPoint() == Points[Index] )
					Found1 = Index;
				if( Limit2->GetOriginalPoint() == Points[Index] )
					Found2 = Index;
				if( Found1 >= 0 && Found2 >= 0 )
					break;
			}
			if( Found1 == Found2 + 1 || Found1 == Found2 - 1
			    || ( Found1 == 0 && Found2 == PointCount - 1 )
			    || ( Found2 == 0 && Found1 == PointCount - 1 ) )
			{
				// The points are along the hull of a non-solid link so there is already a line being drawn between them.
				return;
			}
		}

		Color = pCommon->GetColor();
	}

	CFPoint Point1 = Limit1->GetPoint();
	CFPoint Point2 = Limit2->GetPoint();

	//if( Point2.x < Point1.x || ( Point2.x == Point1.x && Point2.y < Point1.y ) )
	//{
	//	CFPoint Temp = Point1;
	//	Point1 = Point2;
	//	Point2 = Temp;
	//}

	CPen DotPen( PS_DOT, 1, Color );
	CPen *pOldPen = pRenderer->SelectObject( &DotPen );

	if( pConnector->GetOriginalSlideRadius() != 0.0 )
	{
		CFArc SliderArc;
		if( !pConnector->GetSliderArc( SliderArc ) )
			return;

		SliderArc = Scale( SliderArc );
		pRenderer->Arc( SliderArc );
	}
	else
		pRenderer->DrawLine( Scale( CFPoint( Point1.x, Point1.y ) ), Scale( CFPoint( Point2.x, Point2.y ) ) );

	pRenderer->SelectObject( pOldPen );
}

double CLinkageView::UnscaledUnits( double Count )
{
	return Count * m_BaseUnscaledUnit;
}

void CLinkageView::DrawFailureMarks( CRenderer* pRenderer, unsigned int OnLayers, CFPoint Point, double Radius )
{
	CPen RedPen( PS_SOLID, 3, RGB( 255, 32, 32 ) );
	CPen *pOldPen = pRenderer->SelectObject( &RedPen );
	double Distance1 = Radius + UnscaledUnits( 4 );
	double Distance2 = Radius + UnscaledUnits( 9 );

	pRenderer->MoveTo( Point.x + Distance1, Point.y );
	pRenderer->LineTo( Point.x + Distance2, Point.y );
	pRenderer->MoveTo( Point.x - Distance1, Point.y );
	pRenderer->LineTo( Point.x - Distance2, Point.y );
	pRenderer->MoveTo( Point.x, Point.y - AdjustYCoordinate( Distance1 ) );
	pRenderer->LineTo( Point.x, Point.y - AdjustYCoordinate( Distance2 ) );
	pRenderer->MoveTo( Point.x, Point.y + AdjustYCoordinate( Distance1 ) );
	pRenderer->LineTo( Point.x, Point.y + AdjustYCoordinate( Distance2 ) );

	Distance1 = (int)( ( ( (double)Radius + 4.0 ) / 1.414 ) * m_BaseUnscaledUnit );
	Distance2 = (int)( ( ( (double)Radius + 9.9 ) / 1.414 ) * m_BaseUnscaledUnit );

	pRenderer->MoveTo( Point.x + Distance1, Point.y - AdjustYCoordinate( Distance1 ) );
	pRenderer->LineTo( Point.x + Distance2, Point.y - AdjustYCoordinate( Distance2 ) );
	pRenderer->MoveTo( Point.x - Distance1, Point.y + AdjustYCoordinate( Distance1 ) );
	pRenderer->LineTo( Point.x - Distance2, Point.y + AdjustYCoordinate( Distance2 ) );
	pRenderer->MoveTo( Point.x + Distance1, Point.y + AdjustYCoordinate( Distance1 ) );
	pRenderer->LineTo( Point.x + Distance2, Point.y + AdjustYCoordinate( Distance2 ) );
	pRenderer->MoveTo( Point.x - Distance1, Point.y - AdjustYCoordinate( Distance1 ) );
	pRenderer->LineTo( Point.x - Distance2, Point.y - AdjustYCoordinate( Distance2 ) );
	pRenderer->SelectObject( pOldPen );
}

void CLinkageView::DrawConnector( CRenderer* pRenderer, unsigned int OnLayers, CConnector* pConnector, bool bShowlabels, bool bUnused, bool bHighlight, bool bDrawConnectedLinks, bool bControlKnob )
{
	if( ( pConnector->GetLayers() & OnLayers ) == 0 )
		return;

	// Draw only the connector in the proper fashion
	CPen Pen;
	CPen BlackPen( PS_SOLID, 1, RGB( 0, 0, 0 ) );
	CPen* pOldPen = 0;
	CBrush Brush;
	CBrush *pOldBrush = 0;
	COLORREF Color;
	bool bSkipConnectorDraw = false;

	if( !pConnector->IsOnLayers( CLinkageDoc::MECHANISMLAYERS ) )
	{
		Color = pConnector->IsAlone() ? pConnector->GetColor() : COLOR_DRAWINGDARK;
		bSkipConnectorDraw = true;
		POSITION Position = pConnector->GetLinkList()->GetHeadPosition();
		while( Position != 0 )
		{
			CLink* pLink = pConnector->GetLinkList()->GetNext( Position );
			if( pLink != 0 && !pLink->IsMeasurementElement() )
			{
				bSkipConnectorDraw = false;
				break;
			}
		}
	}
	else if( pRenderer->GetColorCount() == 2 )
		Color = RGB( 0, 0, 0 );
	else
	{
		Color = pConnector->GetColor();
		int UsefulLinkCount = 0;
		POSITION Position = pConnector->GetLinkList()->GetHeadPosition();
		if( Position != 0 && pConnector->GetLinkList()->GetCount() == 1 )
		{
			CLink* pLink = pConnector->GetLinkList()->GetNext( Position );
			if (pLink != 0 && pLink->GetConnectorCount() > 1)
			{
				++UsefulLinkCount;
				Color = pLink->GetColor();
			}
		}
		if( UsefulLinkCount > 1 )
			Color = RGB(0, 0, 0);
	}

	CFPoint Point = pConnector->GetPoint();
	Point = Scale( Point );

	if( !bSkipConnectorDraw )
	{
		if( !pConnector->IsOnLayers( CLinkageDoc::MECHANISMLAYERS ) )
		{
			Pen.CreatePen( PS_SOLID, 1, Color );
			Brush.CreateSolidBrush( Color );
		}
		else if( pConnector->IsSelected() )
		{
			Pen.CreatePen( PS_SOLID, 1, Color );
			Brush.CreateSolidBrush( Color );
		}
		else
		{
			Pen.CreatePen( PS_SOLID, 1, Color );
			Brush.CreateStockObject( NULL_BRUSH );
		}

		if( bShowlabels && !bControlKnob )
		{
			/*
			 * Don't draw generic ID values for drawing elements. Only
			 * mechanism elements can show an ID string when no name is
			 * assigned by the user.
			 */
			CString &Name = pConnector->GetName();
			if( !Name.IsEmpty() || ( pConnector->GetLayers() & CLinkageDoc::MECHANISMLAYERS ) != 0 )
			{
				CString& Number = pConnector->GetIdentifierString( m_bShowDebug );

				pRenderer->SetTextAlign( TA_BOTTOM | TA_LEFT );
				pRenderer->SetBkMode( TRANSPARENT );
				double dx = m_ConnectorRadius - UnscaledUnits( 1 );
				double dy = m_ConnectorRadius - UnscaledUnits( 2 );
				if( pConnector->IsInput() )
				{
					dx += UnscaledUnits( 2 );
					dy += UnscaledUnits( 2 );
				}
				if( pConnector->IsDrawing() )
				{
					dx += UnscaledUnits( 3 );
					dy -= UnscaledUnits( m_UsingFontHeight + 1 );
				}

				//CFCircle Circle( Point.x + dx, Point.y + AdjustYCoordinate( dy ), 0.5 );
				//pRenderer->Circle( Circle );
				pRenderer->TextOut( Point.x + dx, Point.y + AdjustYCoordinate( dy ), Number );

				if( pConnector->IsInput() )
				{
					pRenderer->SetTextAlign( TA_TOP | TA_CENTER );
					pRenderer->SetBkMode( TRANSPARENT );
					CString Speed;
					double RPM = pConnector->GetRPM();
					if( fabs( RPM - floor( RPM ) ) < .0001 )
						Speed.Format( "%d", (int)RPM );
					else
						Speed.Format( "%.2lf", RPM );
					if( pConnector->IsAlwaysManual() )
						Speed += "m";
					double yOffset = m_ConnectorRadius + UnscaledUnits( 2 );
					if( pConnector->IsAnchor() )
						yOffset = ( m_ConnectorTriangle * UnscaledUnits( 2 ) ) + m_ArrowBase;
					pRenderer->TextOut( Point.x, Point.y - AdjustYCoordinate( yOffset ), Speed );
				}
			}
		}

		pOldPen = pRenderer->SelectObject( &Pen );
		pOldBrush = pRenderer->SelectObject( &Brush );

		if( !pConnector->IsOnLayers( CLinkageDoc::MECHANISMLAYERS ) )
		{
			if( pConnector->IsAlone() )
			{
				pRenderer->MoveTo( Point.x, Point.y + AdjustYCoordinate( m_ConnectorRadius ) );
				pRenderer->LineTo( Point.x, Point.y - AdjustYCoordinate( m_ConnectorRadius ) );
				pRenderer->MoveTo( Point.x - m_ConnectorRadius, Point.y );
				pRenderer->LineTo( Point.x + m_ConnectorRadius, Point.y );
			}
			else
			{
				CFCircle Circle( Point, UnscaledUnits( 1 ) );
				pRenderer->Circle( Circle );
			}
		}
		else if( pConnector->IsSlider() && !bControlKnob )
		{
			double Angle = 0.0;
			if( pConnector->GetSlideRadius() != 0 )
			{
				CFArc Arc;
				if( pConnector->GetSliderArc( Arc ) )
					Angle = GetAngle( pConnector->GetPoint(), Arc.GetCenter() );
			}
			else
			{
				CFPoint Point1;
				CFPoint Point2;
				if( pConnector->GetSlideLimits( Point1, Point2 ) )
					Angle = GetAngle( Point1, Point2 );
			}
			pRenderer->DrawRect( -Angle, Point.x - m_ConnectorRadius, Point.y + AdjustYCoordinate( m_ConnectorRadius ), Point.x + m_ConnectorRadius, Point.y - AdjustYCoordinate( m_ConnectorRadius ) );
		}
		else
		{
			double Radius = m_ConnectorRadius;
			if( bControlKnob )
				Radius -= UnscaledUnits( 1 );

			pRenderer->Arc( Point.x - Radius, Point.y + AdjustYCoordinate( Radius ), Point.x + Radius, Point.y - AdjustYCoordinate( Radius ), Point.x, Point.y + AdjustYCoordinate( Radius ), Point.x, Point.y + AdjustYCoordinate( Radius ) );
		}

		if( pConnector->IsAnchor() )
		{
			pRenderer->MoveTo( Point.x, Point.y );
			pRenderer->LineTo( Point.x - m_ConnectorTriangle, Point.y - AdjustYCoordinate( m_ConnectorTriangle * 2 ) );
			pRenderer->LineTo( Point.x + m_ConnectorTriangle, Point.y - AdjustYCoordinate( m_ConnectorTriangle * 2 ) );
			pRenderer->MoveTo( Point.x, Point.y );
			pRenderer->LineTo( Point.x + m_ConnectorTriangle, Point.y - AdjustYCoordinate( m_ConnectorTriangle * 2 ) );

			// Little line thingies.
			for( double Counter = 0; Counter <= m_ConnectorTriangle * 2; Counter += UnscaledUnits( 3 ) )
			{
				pRenderer->MoveTo( Point.x - m_ConnectorTriangle + Counter - UnscaledUnits( 2 ), Point.y - AdjustYCoordinate( m_ConnectorTriangle + m_ConnectorTriangle + m_ArrowBase ) );
				pRenderer->LineTo( Point.x - m_ConnectorTriangle + Counter, Point.y - AdjustYCoordinate( m_ConnectorTriangle + m_ConnectorTriangle ) );
			}
		}

		if( pConnector->IsInput() )
		{
			double Radius = ( m_ConnectorRadius + UnscaledUnits( 2 ) );
			pRenderer->Arc( Point.x - Radius, Point.y + AdjustYCoordinate( Radius ), Point.x + Radius, Point.y - AdjustYCoordinate( Radius ), Point.x, Point.y + AdjustYCoordinate( Radius ), Point.x, Point.y + AdjustYCoordinate( Radius ) );

			// Draw a small line to show rotation angle.
			CFPoint StartPoint( Point.x, Point.y + AdjustYCoordinate( m_ConnectorRadius ) );
			CFPoint EndPoint( Point.x, Point.y + AdjustYCoordinate( m_ConnectorRadius + UnscaledUnits( 2 ) ) );
			StartPoint.RotateAround( Point, -pConnector->GetRotationAngle() );
			EndPoint.RotateAround( Point, -pConnector->GetRotationAngle() );
			pRenderer->MoveTo( StartPoint );
			pRenderer->LineTo( EndPoint );
		}

		if( pConnector->IsDrawing() && bShowlabels )
		{
			pRenderer->SelectObject( &BlackPen );
			CFPoint Pencil( Point );
			Pencil.x += m_ConnectorRadius;
			Pencil.y += AdjustYCoordinate( m_ConnectorRadius );

			if( pConnector->IsInput() )
			{
				/*
				 * no one ever sets an input to draw but make the pencil look
				 * good if they do it.
				 */
				Pencil.x += UnscaledUnits( 2 );
				Pencil.y -= UnscaledUnits( 2 );
			}

			pRenderer->MoveTo( Pencil.x, Pencil.y );
			pRenderer->LineTo( Pencil.x, Pencil.y + AdjustYCoordinate( UnscaledUnits( 2 ) ) );
			pRenderer->LineTo( Pencil.x + UnscaledUnits( 3 ), Pencil.y + AdjustYCoordinate( UnscaledUnits( 5 ) ) );
			pRenderer->LineTo( Pencil.x + UnscaledUnits( 5 ), Pencil.y + AdjustYCoordinate( UnscaledUnits( 3 ) ) );
			pRenderer->LineTo( Pencil.x + UnscaledUnits( 2 ), Pencil.y );
			pRenderer->LineTo( Pencil.x, Pencil.y );
			pRenderer->SelectObject( &Pen );
		}

		if( !pConnector->IsPositionValid() )
			DrawFailureMarks( pRenderer, OnLayers, Point, m_ConnectorRadius );
	}

	double Radius = pConnector->GetDrawCircleRadius();
	if( Radius != 0 )
	{
		Radius = Scale( Radius );
		CFCircle Circle( Point, Radius );
		CBrush *pBrush = (CBrush*)pRenderer->SelectStockObject( NULL_BRUSH );
		CPen CirclePen(  PS_SOLID, 1, pConnector->GetColor() );
		CPen *pPen = pRenderer->SelectObject( &CirclePen );
		pRenderer->Circle( Circle );
		pRenderer->SelectObject( pPen );
		pRenderer->SelectObject( pBrush );
	}

	if( m_bShowSelection )
	{
		if( pConnector->IsSelected() || bHighlight )
		{
			CLinkageDoc* pDoc = GetDocument();
			ASSERT_VALID(pDoc);
			CBrush Brush( pDoc->GetLastSelectedConnector() == pConnector ? COLOR_LASTSELECTION : COLOR_BLACK );
			static const int BOX_SIZE = 5;
			int Adjustment = ( BOX_SIZE - 1 ) / 2;
			CFRect Rect( Point.x - Adjustment,  Point.y + AdjustYCoordinate( Adjustment ),  Point.x + Adjustment,  Point.y - AdjustYCoordinate( Adjustment ) );

			pRenderer->FillRect( &Rect, &Brush );

			DrawFasteners( pRenderer, OnLayers, pConnector );
		}
		if( pConnector->IsLinkSelected() && !pConnector->IsAlone() )
		{
			CPen GrayPen;
			GrayPen.CreatePen( PS_SOLID, 1, COLOR_MINORSELECTION ) ;
			pRenderer->SelectObject( &GrayPen );
			pRenderer->Arc( CFArc( Point, m_ConnectorRadius * 2, Point, Point ) );
		}
	}

	if( pOldPen != 0 )
		pRenderer->SelectObject( pOldPen );
	if( pOldBrush != 0 )
		pRenderer->SelectObject( pOldBrush );
}

void CLinkageView::DebugDrawLink( CRenderer* pRenderer, unsigned int OnLayers, CLink *pLink, bool bShowlabels, bool bDrawHighlight, bool bDrawFill )
{
	if( !m_bShowDebug )
		return;

	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CPen CustomPen;
	CustomPen.CreatePen( PS_USERSTYLE, 1, RGB( 0, 0, 0 ) );
	CPen *pOldPen = pRenderer->SelectObject( &CustomPen );

	if( 1 || pLink->IsGear() )
	{
		CFPoint AveragePoint;
		pLink->GetAveragePoint( *pDoc->GetGearConnections(), AveragePoint );
		CConnector* pConnector = pLink->GetConnector( 0 );
		if( pConnector != 0 )
		{
			CFPoint Point = pConnector->GetPoint();
			Point = Scale( AveragePoint );

			pRenderer->SetTextColor( RGB( 25, 0, 0 ) );
			pRenderer->SetBkMode( OPAQUE );
			pRenderer->SetTextAlign( TA_LEFT | TA_TOP );
			pRenderer->SetBkColor( RGB( 255, 255, 255 ) );

			CString String;
			//double DocumentScale = pDoc->GetUnitScale();
			String.Format( "LA %.4lf", pLink->GetRotationAngle() );

			double Radius = m_ConnectorRadius * 5;

			pRenderer->TextOut( Point.x + Radius, Point.y - ( UnscaledUnits( m_UsingFontHeight + 1 ) / 2 ), String );
		}
	}
	pRenderer->SelectObject( pOldPen );
}

void CLinkageView::DrawFasteners( CRenderer* pRenderer, unsigned int OnLayers, CElement *pElement )
{
	ElementList TempList;
	CElementItem *pTempItem = 0;

	if( pElement == 0 )
		return;

	CElementItem *pFastenedTo = pElement->GetFastenedTo();
	if( pFastenedTo != 0 && pFastenedTo->GetElement() != 0 && !pFastenedTo->GetElement()->IsSelected() )
		TempList.AddHead( pFastenedTo );

	ElementList *pList = pElement->GetFastenedElementList();
	if( pList != 0 )
	{
		POSITION Position = pList->GetHeadPosition();
		while( Position != 0 )
		{
			CElementItem *pItem = pList->GetNext( Position );
			if( pItem != 0 )
			{
				if( ( pItem->GetConnector() != 0 && !pItem->GetConnector()->IsSelected() )
					|| ( pItem->GetLink() != 0 && !pItem->GetLink()->IsSelected() ) )
					TempList.AddHead( pItem );
			}
		}
	}

	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CBrush Brush;
	Brush.CreateSolidBrush( COLOR_MINORSELECTION );
	LOGBRUSH lBrush = { 0 };
	Brush.GetLogBrush( &lBrush );
    DWORD Style[] = { 3, 1 };
	CPen CustomPen;
	CustomPen.CreatePen( PS_USERSTYLE, 1, &lBrush, 2, Style );

	CPen *pOldPen = pRenderer->SelectObject( &CustomPen, Style, 2, 1, COLOR_MINORSELECTION );

	POSITION Position = TempList.GetHeadPosition();
	while( Position != 0 )
	{
		CElementItem *pItem = TempList.GetNext( Position );
		if( pItem == 0 )
			continue;

		CFRect AreaRect;
		if( pItem->GetConnector() != 0 )
			pItem->GetConnector()->GetArea( AreaRect );
		else
			pItem->GetLink()->GetArea( *pDoc->GetGearConnections(), AreaRect );

//		if( pFastenedTo->GetConnectorList()->GetCount() > 1 || pFastenedTo->IsGear() )
//		{
			AreaRect = Scale( AreaRect );
			AreaRect.InflateRect( 2 + m_ConnectorRadius, 2 + m_ConnectorRadius );
			pRenderer->DrawRect( AreaRect );
//		}
	}

	pRenderer->SelectObject( pOldPen );

	if( pTempItem != 0 )
		delete pTempItem;
}

void CLinkageView::DrawActuator( CRenderer* pRenderer, unsigned int OnLayers, CLink *pLink, bool bDrawFill )
{
	if( pLink->GetConnectorCount() != 2 )
		return;

	if( ( pLink->GetLayers() & OnLayers ) == 0 )
		return;

	COLORREF Color;

	if( pRenderer->GetColorCount() == 2 )
		Color = RGB( 0, 0, 0 );
	else
		Color = pLink->GetColor(); //Colors[pLink->GetIdentifier() % COLORS_COUNT];

	double Red = GetRValue( Color ) / 255.0;
	double Green = GetGValue( Color ) / 255.0;
	double Blue = GetBValue( Color ) / 255.0;
	if( Red < 1.0 )
		Red += ( 1.0 - Red ) * .7;
	if( Green < 1.0 )
		Green += ( 1.0 - Green ) * .7;
	if( Blue < 1.0 )
		Blue += ( 1.0 - Blue ) * .7;

	COLORREF SecondColor = RGB( (int)( Red * 255 ), (int)( Green * 255 ), (int)( Blue * 255 ) );

	int PointCount = 0;
	CFPoint* Points = pLink->GetHull( PointCount );

	CFPoint StrokePoint;
	if( !pLink->GetStrokePoint( StrokePoint ) )
		StrokePoint = Points[1];

	CFLine Line( Points[0], Points[1] );

	int StrokeLineSize = pLink->GetLineSize();

	if( pLink->IsSolid() )
	{
		CFPoint TempPoints[2];
		TempPoints[0] = Scale( Points[0] );
		TempPoints[1] = Scale( StrokePoint );

		pRenderer->LinkageDrawExpandedPolygon( TempPoints, 0, 2, Color, bDrawFill, pLink->IsSolid() ? m_ConnectorRadius + 3 : 1 );

		StrokeLineSize = 3;
	}
	else
	{
		CPen Pen;
		Pen.CreatePen( PS_SOLID, pLink->GetLineSize() * 5, SecondColor );
		CPen *pOldPen = pRenderer->SelectObject( &Pen );

		CFLine NewLine;
		NewLine = Line;
		NewLine.SetDistance( pLink->GetStroke() );
		pRenderer->MoveTo( Scale( NewLine.GetStart() ) );
		pRenderer->LineTo( Scale( NewLine.GetEnd() ) );

		pRenderer->SelectObject( pOldPen );
	}

	Line.ReverseDirection();
	if( pLink->GetCPM() >= 0 )
		Line.SetDistance( Line.GetDistance() - pLink->GetExtendedDistance() );
	else
		Line.SetDistance( Line.GetDistance() - pLink->GetExtendedDistance() - pLink->GetStroke() );

	CPen Pen;
	Pen.CreatePen( PS_SOLID, StrokeLineSize, Color );
	CPen *pOldPen = pRenderer->SelectObject( &Pen );

	pRenderer->MoveTo( Scale( Line.GetStart() ) );
	pRenderer->LineTo( Scale( Line.GetEnd() ) );

	pRenderer->SelectObject( pOldPen );

	Points = 0;
}

void CLinkageView::DrawStackedConnectors( CRenderer* pRenderer, unsigned int OnLayers )
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if( ( OnLayers & CLinkageDoc::MECHANISMLAYERS ) == 0 )
		return;

	if( m_bSimulating )
		return;

	CPen Pen;
	Pen.CreatePen( PS_SOLID, 1, RGB( 128, 128, 128 ) );
	CPen *pOldPen = pRenderer->SelectObject( &Pen );

	ConnectorList* pConnectors = pDoc->GetConnectorList();
	POSITION Position = pConnectors->GetHeadPosition();
	while( Position != NULL )
	{
		CConnector* pConnector = pConnectors->GetNext( Position );
		if( pConnector == 0 )
			continue;

		// Test for mechanism layers because all other layers don't need the stacking indicator.
		if( ( pConnector->GetLayers() & CLinkageDoc::MECHANISMLAYERS  ) == 0 )
			continue;

		POSITION Position2 = pConnectors->GetHeadPosition();
		while( Position2 != NULL )
		{
			CConnector* pConnector2 = pConnectors->GetNext( Position2 );
			if( pConnector2 == 0 || pConnector2 == pConnector )
				continue;

			if( ( pConnector->GetLayers() & pConnector2->GetLayers() ) == 0 )
				continue;

			CFPoint Point1 = Scale( pConnector->GetPoint() );
			CFPoint Point2 = Scale( pConnector2->GetPoint() );

			// Compare pixel locatons - convert to integers to detect pixel variations.
			if( (int)Point1.x == (int)Point2.x && (int)Point1.y == (int)Point2.y )
			{
				double Radius = ( m_ConnectorRadius + UnscaledUnits( 2 ) );
				if( pConnector->IsInput() || pConnector2->IsInput() )
					Radius += UnscaledUnits( 2 );
				pRenderer->Arc( Point1.x - Radius, Point1.y + AdjustYCoordinate( Radius ), Point1.x + Radius, Point1.y - AdjustYCoordinate( Radius ), Point1.x - Radius, Point1.y + AdjustYCoordinate( Radius ), Point1.x + Radius, Point1.y - AdjustYCoordinate( Radius ) );
			}
		}
	}

	pRenderer->SelectObject( pOldPen );
}

CFArea CLinkageView::DrawGroundDimensions( CRenderer* pRenderer, CLinkageDoc *pDoc, unsigned int OnLayers, CLink *pGroundLink, bool bDrawLines, bool bDrawText)
{
	if( !m_bShowDimensions || !m_bShowGroundDimensions )
		return CFArea();

	if( ( OnLayers & CLinkageDoc::MECHANISMLAYERS ) == 0 )
		return CFArea();

	POSITION Position;

	if( pGroundLink == 0 && false )
	{
		int Anchors = 0;
		Position = pDoc->GetConnectorList()->GetHeadPosition();
		while( Position != 0 )
		{
			CConnector *pConnector = pDoc->GetConnectorList()->GetNext( Position );
			if( pConnector == 0 || !pConnector->IsAnchor() )
				continue;

			++Anchors;
		}

		if( Anchors == 0 ) // Empty or non-functioning mechanism.
			return 0;

		Position = pDoc->GetLinkList()->GetHeadPosition();
		while( Position != 0 )
		{
			CLink *pLink = pDoc->GetLinkList()->GetNext( Position );
			if( pLink == 0  )
				continue;

			int FoundAnchors = pLink->GetAnchorCount();
			if( FoundAnchors > 0 && FoundAnchors == Anchors )
				return CFArea(); // The total anchor count all on this link so there is no need for a ground link.
		}
	}

	CFArea DimensionsArea;

	int ConnectorCount = 0;
	CConnector *pLeftMost = 0;
	CConnector *pBottomMost = 0;
	double xAllLeftMost = DBL_MAX;
	double yAllBottomMost = DBL_MAX;

	ConnectorList* pConnectors;
	if( pGroundLink == 0 )
		pConnectors = pDoc->GetConnectorList();
	else
		pConnectors = pGroundLink->GetConnectorList();

	Position = pConnectors->GetHeadPosition();
	while( Position != NULL )
	{
		CConnector* pConnector = pConnectors->GetNext( Position );
		if( pConnector == 0 )
			continue;

		// Keep track of the left-most and bottom-most connectors for
		// positioning the measurement lines later.
		if( m_bSimulating )
		{
			xAllLeftMost = min( xAllLeftMost, pConnector->GetOriginalPoint().x );
			yAllBottomMost = min( yAllBottomMost, pConnector->GetOriginalPoint().y );
		}
		else
		{
			// The current points are used when not simulating.
			xAllLeftMost = min( xAllLeftMost, pConnector->GetPoint().x );
			yAllBottomMost = min( yAllBottomMost, pConnector->GetPoint().y );
		}

		if( pGroundLink == 0 && !pConnector->IsAnchor() )
			continue;

		++ConnectorCount;

		if( pLeftMost == 0 || pConnector->GetPoint().x < pLeftMost->GetPoint().x )
			pLeftMost = pConnector;
		if( pBottomMost == 0 || pConnector->GetPoint().y < pBottomMost->GetPoint().y )
			pBottomMost = pConnector;
	}

	if( ConnectorCount <= 1 )
		return DimensionsArea;

	vector<ConnectorDistance> ConnectorReference( ConnectorCount );
	vector<ConnectorDistance> ConnectorReferencePerp( ConnectorCount );

	CPen Pen;
	Pen.CreatePen( PS_SOLID, 1, RGB( 192, 192, 192 ) );
	CPen *pOldPen = pRenderer->SelectObject( &Pen );
	COLORREF OldFontColor = pRenderer->GetTextColor();
	pRenderer->SetTextColor( RGB( 96, 96, 96 ) );

	/*
	 * Create line(s) for alignment and comparison of the measurement points.
	 */

	CFLine OrientationLine( CFPoint( pLeftMost->GetPoint().x, pBottomMost->GetPoint().y ), CFPoint( pLeftMost->GetPoint().x + 1000, pBottomMost->GetPoint().y ) );

	CFLine PerpOrientationLine;
	CFLine Temp = OrientationLine;
	Temp.ReverseDirection();
	Temp.PerpendicularLine( PerpOrientationLine );

	/*
	 * Create lists of connectors that are sorted according to location along the
	 * two different orientation lines. The sorting is not necessary if there are
	 * only two connectors. Neither is the perpendicular orientation line.
	 */

	Position = pConnectors->GetHeadPosition();
	int Counter = 0;
	while( Position != NULL )
	{
		CConnector* pConnector = pConnectors->GetNext( Position );
		if( pConnector == 0 || ( pGroundLink == 0 && !pConnector->IsAnchor() ) )
			continue;

		ConnectorReference[Counter].m_pConnector = pConnector;
		ConnectorReference[Counter].m_Distance = DistanceAlongLine( OrientationLine, pConnector->GetPoint() );
		ConnectorReferencePerp[Counter].m_pConnector = pConnector;
		ConnectorReferencePerp[Counter].m_Distance = DistanceAlongLine( PerpOrientationLine, pConnector->GetPoint() );
		++Counter;
	}

	/*
	 * Sort the connectors along the orientation lines.
	 */

	sort( ConnectorReference.begin(), ConnectorReference.end(), CompareConnectorDistance() );
	sort( ConnectorReferencePerp.begin(), ConnectorReferencePerp.end(), CompareConnectorDistance() );

	/*
	 * Draw Measurement Lines.
	 */

	double Offset = Scale( OrientationLine.GetStart().x - xAllLeftMost );
	Offset += OFFSET_INCREMENT;
	for( int Counter = 0; Counter < ConnectorCount; ++Counter )
	{
		if( fabs( ConnectorReferencePerp[Counter].m_Distance ) < 0.001 )
			continue;
		CFLine MeasurementLine = PerpOrientationLine;
		MeasurementLine.SetDistance(  ConnectorReferencePerp[Counter].m_Distance );
		DimensionsArea += DrawMeasurementLine( pRenderer, MeasurementLine, ConnectorReferencePerp[Counter].m_pConnector->GetPoint(), pBottomMost->GetPoint(), Offset, bDrawLines, bDrawText );
		Offset += OFFSET_INCREMENT;
	}

	Offset = Scale( OrientationLine.GetStart().y - yAllBottomMost );

	if( ConnectorCount == 2 )
	{
		// Draw a possibly diagonal measurement between the anchors but only
		// if there are two of them. Any more than that and the diagonal
		// measurement will not be useful in real life.
		if( ConnectorReference[0].m_pConnector->GetSharingLink( ConnectorReference[1].m_pConnector ) == 0 || pGroundLink != 0 )
		{
			// Skip this is they are lined up horizontally or vertically
			// because there is already a measurement shown.
			if( fabs( ConnectorReference[0].m_pConnector->GetPoint().x - ConnectorReference[1].m_pConnector->GetPoint().x ) > 0.001
			    && fabs( ConnectorReference[0].m_pConnector->GetPoint().y - ConnectorReference[1].m_pConnector->GetPoint().y ) > 0.001 )
			{
				CFLine MeasurementLine( ConnectorReference[0].m_pConnector->GetPoint(), ConnectorReference[1].m_pConnector->GetPoint() );

				double UseOffset = pGroundLink == 0 ? 0 : -OFFSET_INCREMENT;
				DimensionsArea += DrawMeasurementLine( pRenderer, MeasurementLine, MeasurementLine.GetEnd(), MeasurementLine.GetStart(), UseOffset, bDrawLines, bDrawText );

				double Angle = MeasurementLine.GetAngle();

				if( Angle < 45 )
					Offset += OFFSET_INCREMENT;
			}
		}
	}

	Offset += OFFSET_INCREMENT;
	for( int Counter = 0; Counter < ConnectorCount; ++Counter )
	{
		if( fabs( ConnectorReference[Counter].m_Distance ) < 0.001 )
			continue;
		CFLine MeasurementLine = OrientationLine;
		MeasurementLine.SetDistance(  ConnectorReference[Counter].m_Distance );
		DimensionsArea += DrawMeasurementLine( pRenderer, MeasurementLine, ConnectorReference[Counter].m_pConnector->GetPoint(), pLeftMost->GetPoint(), -Offset, bDrawLines, bDrawText );
		Offset += OFFSET_INCREMENT;
	}

	pRenderer->SelectObject( pOldPen );
	pRenderer->SetTextColor( OldFontColor );

	return DimensionsArea;
}

CFLine CLinkageView::CreateMeasurementEndLine( CFLine &InputLine, CFPoint &MeasurementPoint, CFPoint &MeasurementLinePoint, int InputLinePointIndex, double Offset )
{
	bool bUseOffset = Offset == 0 ? false : true;
	Offset = Unscale( bUseOffset ? Offset : 8 );

	CFLine Line = InputLine;
	if( InputLinePointIndex > 0 )
		Line.ReverseDirection();
	Line.PerpendicularLine( Line, UnscaledUnits( Offset ), InputLinePointIndex == 0 ? 1 : -1 );
	MeasurementLinePoint = bUseOffset ? Line.m_End : Line.m_Start;
	Line.m_Start = MeasurementPoint;
	if( bUseOffset )
		Line.MoveEnds( Unscale( UnscaledUnits( 3 ) ), Unscale( UnscaledUnits( 8 ) ) );
	else
		Line.MoveEnds( -UnscaledUnits( Offset ), 0 );

	return Line;
}

void CLinkageView::DrawMeasurementLine( CRenderer* pRenderer, CFLine &InputLine, double Offset, bool bDrawLines, bool bDrawText )
{
	DrawMeasurementLine( pRenderer, InputLine, InputLine[1], InputLine[0], Offset, bDrawLines, bDrawText );
}

CFArea CLinkageView::DrawMeasurementLine( CRenderer* pRenderer, CFLine &InputLine, CFPoint &FirstPoint, CFPoint &SecondPoint, double Offset, bool bDrawLines, bool bDrawText )
{
	CFArea DimensionsArea;
	CFLine MeasurementLine;

	CFLine Line1 = CreateMeasurementEndLine( InputLine, FirstPoint, MeasurementLine.m_Start, 0, Offset );
	CFLine Line2 = CreateMeasurementEndLine( InputLine, SecondPoint, MeasurementLine.m_End, 1, Offset );

	DimensionsArea += MeasurementLine;
	DimensionsArea += Line1;
	DimensionsArea += Line2;

	if( bDrawLines )
	{
		pRenderer->DrawLine( Scale( Line1 ) );
		pRenderer->DrawLine( Scale( Line2 ) );
	}

	MeasurementLine = Scale( MeasurementLine );

	if( bDrawLines )
	{
		CFLine Temp = MeasurementLine;
		Temp.MoveEnds( UnscaledUnits( 4 ), -UnscaledUnits( 4 ) );

		MeasurementLine.MoveEnds( UnscaledUnits( 0.5 ), -UnscaledUnits( 0.5 ) );

		pRenderer->DrawLine( Temp );
		pRenderer->DrawArrow( MeasurementLine.GetEnd(), MeasurementLine.GetStart(), UnscaledUnits( 3 ), UnscaledUnits( 5 ) );
		pRenderer->DrawArrow( MeasurementLine.GetStart(), MeasurementLine.GetEnd(), UnscaledUnits( 3 ), UnscaledUnits( 5 ) );
	}

	if( bDrawText )
	{
		CLinkageDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);

		pRenderer->SetBkMode( OPAQUE );
		pRenderer->SetTextAlign( TA_CENTER | TA_TOP );
		pRenderer->SetBkColor( RGB( 255, 255, 255 ) );

		CString String;
		double DocumentScale = pDoc->GetUnitScale();
		String.Format( "%.4lf", DocumentScale * InputLine.GetDistance() );

		CFLine Temp( MeasurementLine );
		Temp.SetDistance( MeasurementLine.GetDistance() / 2 );
		pRenderer->TextOut( Temp.GetEnd().x, Temp.GetEnd().y + AdjustYCoordinate( ( UnscaledUnits( m_UsingFontHeight + 1 ) / 2 ) + 1 ), String );
	}
	return DimensionsArea;
}

CFArea CLinkageView::DrawCirclesRadius( CRenderer *pRenderer, CFPoint Center, std::list<double> &RadiusList, bool bDrawLines, bool bDrawText )
{
	if( pRenderer == 0 || RadiusList.size() == 0 )
		return CFArea();

	CFArea DimensionsArea;

	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	std::list<double>::iterator it = RadiusList.end();
	--it;
	double LargestRadius = *it;
	double SecondLargest = 0.0;
	if( it != RadiusList.begin() )
	{
		--it;
		SecondLargest = *it;
	}

	if( SecondLargest > 0.0 )
	{
		if( bDrawLines )
		{
			CFPoint Point = Center;
			Point.SetPoint( Point, SecondLargest, 45 );
			CFLine MeasurementLine( Center, Point );
			MeasurementLine = Scale( MeasurementLine );

			CFLine Temp = MeasurementLine;
			if( pRenderer->GetScale() > 1.0 )
				Temp.MoveEnds( 0, -4 );

			pRenderer->DrawLine( Temp );
			pRenderer->DrawArrow( MeasurementLine.GetStart(), MeasurementLine.GetEnd(), UnscaledUnits( 3 ), UnscaledUnits( 5 ) );
		}

		if( bDrawText )
		{
			std::list<double>::iterator end = RadiusList.end();
			--end;
			for( std::list<double>::iterator it = RadiusList.begin(); it != end; ++it )
			{
				double Radius = *it;

				CFPoint Point = Center;
				Point.SetPoint( Point, Radius, 45 );
				CFLine MeasurementLine( Center, Point );
				MeasurementLine.SetDistance( MeasurementLine.GetDistance() / 2 );

				MeasurementLine = Scale( MeasurementLine );

				CString String;
				double DocumentScale = pDoc->GetUnitScale();
				String.Format( "R%.4lf", DocumentScale * Radius );

				pRenderer->SetTextAlign( TA_CENTER | TA_TOP );
				pRenderer->TextOut( MeasurementLine.GetEnd().x + UnscaledUnits( 2 ), MeasurementLine.GetEnd().y - ( UnscaledUnits( m_UsingFontHeight + 1 ) / 2 ) - UnscaledUnits( 1 ), String );
			}
		}
	}

	CFPoint Point1 = Center;
	Point1.SetPoint( Point1, LargestRadius, 45 );
	CFPoint Point2;
	Point2.SetPoint( Point1, Unscale( RADIUS_MEASUREMENT_OFFSET ), 45 );
	CFPoint Point3( Point2.x + Unscale( RADIUS_MEASUREMENT_OFFSET ), Point2.y );

	if( bDrawLines )
	{
		CFLine MeasurementLine( Point1, Point2 );
		MeasurementLine = Scale( MeasurementLine );
		pRenderer->DrawLine( MeasurementLine );
		pRenderer->DrawArrow( MeasurementLine.GetEnd(), MeasurementLine.GetStart(), UnscaledUnits( 3 ), UnscaledUnits( 5 ) );
		MeasurementLine.SetLine( Point2, Point3 );
		MeasurementLine = Scale( MeasurementLine );
		pRenderer->DrawLine( MeasurementLine );
	}

	if( bDrawText )
	{
		CString String;
		double DocumentScale = pDoc->GetUnitScale();
		String.Format( "R%.4lf", DocumentScale * LargestRadius );

		CFPoint Point = Scale( Point3 );

		pRenderer->SetTextAlign( TA_LEFT | TA_TOP );
		pRenderer->TextOut( Point.x + UnscaledUnits( 2 ), Point.y - ( UnscaledUnits( m_UsingFontHeight + 1 ) / 2 ) - UnscaledUnits( 1 ), String );
	}

	return DimensionsArea;
}

CFArea CLinkageView::DrawConnectorLinkDimensions( CRenderer* pRenderer, const GearConnectionList *pGearConnections, unsigned int OnLayers, CLink *pLink, bool bDrawLines, bool bDrawText )
{
	if( !m_bShowDimensions )
		return CFArea();

	if( ( pLink->GetLayers() & OnLayers ) == 0 )
		return CFArea();

	CPen Pen;
	Pen.CreatePen( PS_SOLID, 1, RGB( 192, 192, 192 ) );
	CPen *pOldPen = pRenderer->SelectObject( &Pen );
	COLORREF OldFontColor = pRenderer->GetTextColor();
	pRenderer->SetTextColor( RGB( 96, 96, 96 ) );

	pRenderer->SetBkMode( OPAQUE );
	pRenderer->SetTextAlign( TA_CENTER | TA_TOP );
	pRenderer->SetBkColor( RGB( 255, 255, 255 ) );

	int ConnectorCount = pLink->GetConnectorList()->GetCount();
	if( ConnectorCount <= 1 )
	{
		CConnector* pConnector = pLink->GetConnector( 0 );
		if( pConnector != 0 )
		{
			if( pLink->IsGear() )
			{
				std::list<double> RadiusList;
				pLink->GetGearRadii( *pGearConnections, RadiusList );

				DrawCirclesRadius( pRenderer, pConnector->GetPoint(), RadiusList, bDrawLines, bDrawText );
			}

			if( ( pConnector->GetLayers() & CLinkageDoc::DRAWINGLAYER ) && bDrawText )
			{
				CLinkageDoc* pDoc = GetDocument();
				ASSERT_VALID(pDoc);

				CString String;
				double DocumentScale = pDoc->GetUnitScale();
				CFPoint Point = pConnector->GetPoint();
				String.Format( "%.4lf,%.4lf", DocumentScale * Point.x, DocumentScale * Point.y );

				Point = Scale( Point );
				pRenderer->TextOut( Point.x, Point.y + m_ConnectorRadius + UnscaledUnits( 2 ), String );
			}
		}
	}

	pRenderer->SelectObject( pOldPen );
	pRenderer->SetTextColor( OldFontColor );

	return CFArea();
}

static int GetLongestHullDimensionLine( CLink *pLink, int &BestEndPoint, CFPoint *pHullPoints, int HullPointCount, CConnector **pStartConnector )
{
	int BestStartPoint = -1;
	BestEndPoint = -1;

	if( pLink->GetConnectorCount() == 2 )
	{
		BestStartPoint = 0;
		BestEndPoint = 1;
	}
	else
	{
		double LargestDistance = 0.0;
		for( int Counter = 0; Counter < HullPointCount; ++Counter )
		{
			double TestDistance = 0;
			if( Counter < HullPointCount - 1 )
				TestDistance = Distance( pHullPoints[Counter], pHullPoints[Counter+1] );
			else
				TestDistance = Distance( pHullPoints[Counter], pHullPoints[0] );
			if( TestDistance > LargestDistance )
			{
				LargestDistance = TestDistance;
				BestStartPoint = Counter;
			}
		}
		// The hull is a clockwise collection of points so the end point of
		// the measurement is a better visual choice for the start connector.
		if( BestStartPoint < HullPointCount - 1 )
			BestEndPoint = BestStartPoint + 1;
		else
			BestEndPoint = 0;
	}

	if( pStartConnector != 0 && BestStartPoint >= 0 )
	{
		*pStartConnector = 0;
		POSITION Position = pLink->GetConnectorList()->GetHeadPosition();
		int Counter = 0;
		while( Position != 0 )
		{
			CConnector *pConnector = pLink->GetConnectorList()->GetNext( Position );
			if( pConnector == 0 )
				continue;

			if( pConnector->GetPoint() == pHullPoints[BestStartPoint] )
				*pStartConnector = pConnector;
		}
	}

	return BestStartPoint;
}

CFArea CLinkageView::DrawDimensions( CRenderer* pRenderer, const GearConnectionList *pGearConnections, unsigned int OnLayers, CLink *pLink, bool bDrawLines, bool bDrawText )
{
	if( !m_bShowDimensions )
		return CFArea();

	if( ( pLink->GetLayers() & OnLayers ) == 0 )
		return CFArea();

	CFArea DimensionsArea;

	int ConnectorCount = pLink->GetConnectorList()->GetCount();
	if( ConnectorCount <= 1 )
		return DrawConnectorLinkDimensions( pRenderer, pGearConnections, OnLayers, pLink, bDrawLines, bDrawText );

	CPen Pen;
	Pen.CreatePen( PS_SOLID, 1, RGB( 192, 192, 192 ) );
	CPen *pOldPen = pRenderer->SelectObject( &Pen );
	COLORREF OldFontColor = pRenderer->GetTextColor();
	pRenderer->SetTextColor( RGB( 96, 96, 96 ) );

	pRenderer->SetBkMode( OPAQUE );
	pRenderer->SetTextAlign( TA_CENTER | TA_TOP );
	pRenderer->SetBkColor( RGB( 255, 255, 255 ) );

	double LargestDistance = 0.0;

	int HullCount = 0;
	CFPoint *pPoints = pLink->GetHull( HullCount );

	/*
	 * Find two adjacent hull locations that have the largest distance between
	 * them. This is used to pick the orientation of the measurement lines.
	 * these points are not used for any other purpose.
	 */

	int BestStartPoint = -1;
	int BestEndPoint = -1;

	CConnector *pStartConnector = 0;
	BestStartPoint = GetLongestHullDimensionLine( pLink, BestEndPoint, pPoints, HullCount, &pStartConnector );

	if( BestStartPoint < 0 )
	{
		pRenderer->SelectObject( pOldPen );
		pRenderer->SetTextColor( OldFontColor );
		return CFArea();
	}

	/*
	 * Create line(s) for alignment and comparison of the measurement points.
	 */

	CFLine OrientationLine( pPoints[BestStartPoint], pPoints[BestEndPoint] );

	if( pLink->IsActuator() )
	{
		CFLine MeasurementLine = OrientationLine;

		POSITION Position = pLink->GetConnectorList()->GetHeadPosition();
		CConnector *pEndConnector = Position == 0 ? 0 : pLink->GetConnectorList()->GetNext( Position );
		if( pEndConnector == pStartConnector )
			pEndConnector = Position == 0 ? 0 : pLink->GetConnectorList()->GetNext( Position );
		if( pStartConnector == 0 || pEndConnector == 0 )
		{
			pRenderer->SelectObject( pOldPen );
			pRenderer->SetTextColor( OldFontColor );
			return CFArea();
		}

		CFLine LengthLine = OrientationLine;
		CFLine ThrowLine = OrientationLine;
		CFLine ExtensionLine;

		double StartDistance = Distance( pStartConnector->GetPoint(), pEndConnector->GetPoint() );

		LengthLine.SetDistance( StartDistance );
		ThrowLine.SetDistance( pLink->GetStroke() );
		ExtensionLine.SetLine( ThrowLine.GetEnd(), LengthLine.GetEnd() );

		double Offset = OFFSET_INCREMENT * -2;
		DimensionsArea += DrawMeasurementLine( pRenderer, LengthLine, LengthLine.GetEnd(), LengthLine.GetStart(), Offset, bDrawLines, bDrawText );

		Offset = -OFFSET_INCREMENT;
		DimensionsArea += DrawMeasurementLine( pRenderer, ThrowLine, ThrowLine.GetEnd(), ThrowLine.GetStart(), Offset, bDrawLines, bDrawText );
		DimensionsArea += DrawMeasurementLine( pRenderer, ExtensionLine, ExtensionLine.GetEnd(), ExtensionLine.GetStart(), Offset, bDrawLines, bDrawText );
	}
	else
	{
		CFLine PerpOrientationLine;
		CFLine Temp = OrientationLine;
		Temp.ReverseDirection();
		Temp.PerpendicularLine( PerpOrientationLine );

		/*
		 * Create lists of connectors that are sorted according to location along the
		 * two different orientation lines. The sorting is not necessary if there are
		 * only two connectors. Neither is the perpendicular orientation line.
		 */

		vector<ConnectorDistance> ConnectorReference( ConnectorCount );
		vector<ConnectorDistance> ConnectorReferencePerp( ConnectorCount );

		POSITION Position = pLink->GetConnectorList()->GetHeadPosition();
		int Counter = 0;
		while( Position != 0 )
		{
			CConnector *pConnector = pLink->GetConnectorList()->GetNext( Position );
			if( pConnector == 0 )
				continue;

			if( Counter < ConnectorCount )
			{
				ConnectorReference[Counter].m_pConnector = pConnector;
				ConnectorReference[Counter].m_Distance = DistanceAlongLine( OrientationLine, pConnector->GetPoint() );
				ConnectorReferencePerp[Counter].m_pConnector = pConnector;
				ConnectorReferencePerp[Counter].m_Distance = DistanceAlongLine( PerpOrientationLine, pConnector->GetPoint() );
				++Counter;
			}
		}

		/*
		 * Sort the connectors along the orientation lines.
		 */

		if( ConnectorCount > 2 )
		{
			sort( ConnectorReference.begin(), ConnectorReference.end(), CompareConnectorDistance() );
			sort( ConnectorReferencePerp.begin(), ConnectorReferencePerp.end(), CompareConnectorDistance() );
		}

		/*
		 * Draw Measurement Lines.
		 */

		double Offset = OFFSET_INCREMENT;
		int NextOffset = 0;
		int StartPointInList = 0;

		// Find start point in list.
		for( int Counter = 0; Counter < ConnectorCount; ++Counter )
		{
			if( ConnectorReference[Counter].m_pConnector == pStartConnector )
			{
				StartPointInList = Counter;
				break;
			}
		}

		NextOffset = (int)ConnectorReference[0].m_Distance;

		// Points before the start point on the orientation line.
		for( int Counter = StartPointInList - 1; Counter >= 0; --Counter )
		{
			if( fabs( ConnectorReference[Counter].m_Distance ) < 0.001 )
				continue;
			CFLine MeasurementLine = OrientationLine;
			MeasurementLine.SetDistance( ConnectorReference[Counter].m_Distance );
			MeasurementLine.ReverseDirection();
			DimensionsArea += DrawMeasurementLine( pRenderer, MeasurementLine, pStartConnector->GetPoint(), ConnectorReference[Counter].m_pConnector->GetPoint(), Offset, bDrawLines, bDrawText );
			Offset -= OFFSET_INCREMENT;
		}

		Offset = -OFFSET_INCREMENT;

		// Points after the start point on the orientation line.
		for( int Counter = StartPointInList + 1; Counter < ConnectorCount; ++Counter )
		{
			if( ConnectorReference[Counter].m_pConnector == pStartConnector )
				break;
			if( fabs( ConnectorReference[Counter].m_Distance ) < 0.001 )
				continue;
			CFLine MeasurementLine = OrientationLine;
			MeasurementLine.SetDistance(  ConnectorReference[Counter].m_Distance );
			DimensionsArea += DrawMeasurementLine( pRenderer, MeasurementLine, ConnectorReference[Counter].m_pConnector->GetPoint(), pStartConnector->GetPoint(), Offset, bDrawLines, bDrawText );
			Offset -= OFFSET_INCREMENT;
		}

		if( ConnectorCount > 2 )
		{
			// Find start point in list.
			for( int Counter = 0; Counter < ConnectorCount; ++Counter )
			{
				if( ConnectorReferencePerp[Counter].m_pConnector == pStartConnector )
				{
					StartPointInList = Counter;
					break;
				}
			}

			Offset = NextOffset - OFFSET_INCREMENT;
			// Points before the start point on the perp orientation line.
			for( int Counter = StartPointInList - 1; Counter >= 0; --Counter )
			{
				if( fabs( ConnectorReferencePerp[Counter].m_Distance ) < 0.001 )
					continue;
				CFLine MeasurementLine = PerpOrientationLine;
				MeasurementLine.SetDistance( ConnectorReferencePerp[Counter].m_Distance );
				MeasurementLine.ReverseDirection();
				DimensionsArea += DrawMeasurementLine( pRenderer, MeasurementLine, pStartConnector->GetPoint(), ConnectorReferencePerp[Counter].m_pConnector->GetPoint(), Offset, bDrawLines, bDrawText );
				Offset -= OFFSET_INCREMENT;
			}

			Offset = NextOffset - OFFSET_INCREMENT;
			// Points after the start point on the perp orientation line.
			for( int Counter = StartPointInList + 1; Counter < ConnectorCount; ++Counter )
			{
				if( fabs( ConnectorReferencePerp[Counter].m_Distance ) < 0.001 )
					continue;
				CFLine MeasurementLine = PerpOrientationLine;
				MeasurementLine.SetDistance(  ConnectorReferencePerp[Counter].m_Distance );
				DimensionsArea += DrawMeasurementLine( pRenderer, MeasurementLine, ConnectorReferencePerp[Counter].m_pConnector->GetPoint(), pStartConnector->GetPoint(), Offset, bDrawLines, bDrawText );
				Offset -= OFFSET_INCREMENT;
			}
		}
	}

	pRenderer->SelectObject( pOldPen );
	pRenderer->SetTextColor( OldFontColor );

	return DimensionsArea;
}

CFArea CLinkageView::DrawDimensions( CRenderer* pRenderer, unsigned int OnLayers, CConnector *pConnector, bool bDrawLines, bool bDrawText )
{
	if( !m_bShowDimensions )
		return CFArea();

	if( ( pConnector->GetLayers() & OnLayers ) == 0 )
		return CFArea();

	CFArea DimensionsArea;

	CPen Pen;
	Pen.CreatePen( PS_SOLID, 1, RGB( 192, 192, 192 ) );
	CPen *pOldPen = pRenderer->SelectObject( &Pen );
	COLORREF OldFontColor = pRenderer->GetTextColor();
	pRenderer->SetTextColor( RGB( 96, 96, 96 ) );

	if( pConnector->GetDrawCircleRadius() > 0 )
	{
		std::list<double> RadiusList;
		RadiusList.push_back( pConnector->GetDrawCircleRadius() );
		DimensionsArea += DrawCirclesRadius( pRenderer, pConnector->GetPoint(), RadiusList, bDrawLines, bDrawText );
	}

	pRenderer->SelectObject( pOldPen );
	pRenderer->SetTextColor( OldFontColor );

	return DimensionsArea;
}

static void DrawChainLine( CRenderer* pRenderer, CFLine Line, double ShiftingFactor )
{
	const double Gapping = 6.0;

	double Offset = fmod( ShiftingFactor, Gapping * 2 );

	double Start = -Gapping + Offset;
	double End = Start + Gapping;

	CFLine DarkLine( Line );
	if( End > 0 )
	{
		DarkLine.MoveEndsFromStart( Start < 0 ? 0 : Start, End );
		pRenderer->DrawLine( DarkLine );
	}

	double Length = Line.GetDistance();

	for( int Step = 1; ; ++Step )
	{
		Start = End;
		End = Start + Gapping;
		if( Step % 2 == 0 )
		{
			DarkLine.SetLine( Line );
			DarkLine.MoveEndsFromStart( Start < 0 ? 0 : Start, End > Length ? Length : End );
			pRenderer->DrawLine( DarkLine );
		}
		if( End >= Length )
			break;
	}
}

void CLinkageView::DrawChain( CRenderer* pRenderer, unsigned int OnLayers, CGearConnection *pGearConnection )
{
	if( pGearConnection == 0 || pGearConnection->m_pGear1 == 0 || pGearConnection->m_pGear2 == 0 )
		return;

	if( ( pGearConnection->m_pGear1->GetLayers() & OnLayers ) == 0 || ( pGearConnection->m_pGear2->GetLayers() & OnLayers ) == 0 )
		return;

	CConnector *pConnector1 = pGearConnection->m_pGear1->GetConnector( 0 );
	if( pConnector1 == 0 )
		return;

	CConnector *pConnector2 = pGearConnection->m_pGear2->GetConnector( 0 );
	if( pConnector2 == 0 )
		return;

	double Radius1 = 0;
	double Radius2 = 0;
	pGearConnection->m_pGear1->GetGearsRadius( pGearConnection, Radius1, Radius2 );

	CPen DotPen( PS_SOLID, 1, COLOR_CHAIN );
	CPen* pOldPen = pRenderer->SelectObject( &DotPen );

	CFLine Line1;
	CFLine Line2;
	if( !GetTangents( CFCircle( pConnector1->GetPoint(), Radius1 ), CFCircle( pConnector2->GetPoint(), Radius2 ), Line1, Line2 ) )
		return;

	// SOME CHAIN MOVEMENT TEST CODE...
	double GearAngle = pGearConnection->m_pGear1->GetRotationAngle();
	CLink *pConnectionLink = pGearConnection->m_pGear1->GetConnector( 0 )->GetSharingLink( pGearConnection->m_pGear2->GetConnector( 0 ) );
	double LinkAngle = pConnectionLink == 0 ? 0 : -pConnectionLink->GetTempRotationAngle();
	double UseAngle = GearAngle + LinkAngle;

	double DistanceAround = ( -UseAngle / 360.0 ) * Radius1 * 2 * PI_FOR_THIS_CODE;

	DrawChainLine( pRenderer, Scale( Line1 ), Scale( DistanceAround ) );
	Line2.ReverseDirection();
	DrawChainLine( pRenderer, Scale( Line2 ), Scale( DistanceAround ) );

	pRenderer->SelectObject( pOldPen );
}

void CLinkageView::DrawLink( CRenderer* pRenderer, const GearConnectionList *pGearConnections, unsigned int OnLayers, CLink *pLink, bool bShowlabels, bool bDrawHighlight, bool bDrawFill )
{
	if( ( pLink->GetLayers() & OnLayers ) == 0 )
		return;

	CPen* pOldPen = 0;

	int Count = pLink->GetConnectorList()->GetCount();

	if( bDrawHighlight )
	{
		/*
		 * Draw only the highlight and then return.
		 * this allows the highlight to be behind all other objects.
		 */
		if( m_bShowSelection )
		{
			if( pLink->IsSelected( true ) )
			{
				CPen GrayPen;

				CLinkageDoc* pDoc = GetDocument();
				ASSERT_VALID(pDoc);

				GrayPen.CreatePen( PS_SOLID, 1, pDoc->GetLastSelectedLink() == pLink ? RGB( 255, 200, 200 ) : COLOR_MINORSELECTION ) ;
				pOldPen = pRenderer->SelectObject( &GrayPen );

				CFRect AreaRect;
				pLink->GetArea( *pGearConnections, AreaRect );

				if( pLink->GetConnectorList()->GetCount() > 1 || pLink->IsGear() )
				{
					AreaRect = Scale( AreaRect );
					AreaRect.InflateRect( 2 * m_ConnectorRadius, 2 * m_ConnectorRadius );
					pRenderer->DrawRect( AreaRect );
				}
				pRenderer->SelectObject( pOldPen );

				DrawFasteners( pRenderer, OnLayers, pLink );
			}
			return;
		}
	}

	if( pLink->IsMeasurementElement() )
	{
		CPen Pen;
		Pen.CreatePen( PS_SOLID, 1, RGB( 192, 192, 192 ) );
		CPen *pOldPen = pRenderer->SelectObject( &Pen );
		COLORREF OldFontColor = pRenderer->GetTextColor();
		pRenderer->SetTextColor( RGB( 96, 96, 96 ) );
		pRenderer->SetBkMode( OPAQUE );
		pRenderer->SetTextAlign( TA_LEFT | TA_TOP );
		pRenderer->SetBkColor( RGB( 255, 255, 255 ) );

		int PointCount = 0;
		CFPoint* Points = pLink->GetHull( PointCount );
		// reverse the points because the -y change causes the hull to be in the wrong direction.
		for( int Index = 0; Index < PointCount - 1; ++Index )
		{
			CFLine Line( Points[Index], Points[Index+1] );
			DrawMeasurementLine( pRenderer, Line, 0, true, true );
		}
		pRenderer->SelectObject( pOldPen );
	}

	CPen Pen;
	COLORREF Color;

	if( !pLink->IsOnLayers( CLinkageDoc::MECHANISMLAYERS ) )
		Color = pLink->GetColor();
	else if( pRenderer->GetColorCount() == 2 )
		Color = RGB( 0, 0, 0 );
	else
		Color = pLink->GetColor();

	if( bShowlabels && ( Count > 1 || pLink->IsGear() ) )
	{
		/*
		 * Don't draw generic ID values for drawing elements. Only
		 * mechanism elements can show an ID string when no name is
		 * assigned by the user.
		 */
		CString &Name = pLink->GetName();
		if( !Name.IsEmpty() || ( pLink->GetLayers() & CLinkageDoc::MECHANISMLAYERS ) != 0 )
		{
			pRenderer->SetBkMode( TRANSPARENT );
			CString Number = pLink->GetIdentifierString( m_bShowDebug );

			CConnector *pConnector1 = pLink->GetConnectorList()->GetHead();

			CFPoint Average;
			double LinkLabelOffset;

			if( pLink->IsGear() )
			{
				pRenderer->SetTextAlign( TA_BOTTOM | TA_RIGHT );
				double dx = m_ConnectorRadius - UnscaledUnits( 1 );
				double dy = m_ConnectorRadius - UnscaledUnits( 2 );

				Average = pConnector1->GetPoint();
				Average = Scale( Average );
				Average.x -= dx;
				Average.y += AdjustYCoordinate( dy );
				LinkLabelOffset = 0;
			}
			else
			{
				pRenderer->SetTextAlign( TA_BOTTOM | TA_LEFT );
				pLink->GetAveragePoint( *pGearConnections, Average );
				Average = Scale( Average );
				Average.y -= AdjustYCoordinate( UnscaledUnits( m_UsingFontHeight ) * 0.75 );
				LinkLabelOffset = UnscaledUnits( pLink->IsSolid() ? 14 : 6 );
			}

			if( Count == 2 || true ) // it seems ok to do this all of the time, which works better for some parts list information.
			{
				CConnector *pConnector2 = pLink->GetConnectorList()->GetTail();
				if( pConnector1 != 0 && pConnector2 != 0 && pConnector1 != pConnector2 )
				{
					// Note that this is based on the original point to keep the location from jumping while moving.
					double dx = fabs( pConnector1->GetOriginalPoint().x - pConnector2->GetOriginalPoint().x );
					double dy = fabs( pConnector1->GetOriginalPoint().y - pConnector2->GetOriginalPoint().y );
					if( dx > dy )
						Average.y += AdjustYCoordinate( LinkLabelOffset ) * 1.5;
					else
						Average.x += LinkLabelOffset;
				}
			}
			else
			{
				// A guess to keep the text from overlapping the average point of a link.
				//Average.y += AdjustYCoordinate( LinkLabelOffset ) * 1.5;
				Average.x += LinkLabelOffset;
			}

			int Spacing = 0;
			if( pLink->IsLocked() )
			{
				CPen LockPen;
				LockPen.CreatePen( PS_SOLID, pLink->GetLineSize(), RGB( 0, 0, 0 ) );
				pOldPen = pRenderer->SelectObject( &LockPen );

				pRenderer->SelectObject( &LockPen );
				CFPoint LockLocation( Average );
				LockLocation.x += m_ConnectorRadius;
				LockLocation.y += AdjustYCoordinate( m_ConnectorRadius );

				pRenderer->MoveTo( LockLocation.x, LockLocation.y );
				pRenderer->LineTo( LockLocation.x, LockLocation.y + AdjustYCoordinate( UnscaledUnits( 4 ) ) );
				pRenderer->LineTo( LockLocation.x + UnscaledUnits( 5 ), LockLocation.y + AdjustYCoordinate( UnscaledUnits( 4 ) ) );
				pRenderer->LineTo( LockLocation.x + UnscaledUnits( 5 ), LockLocation.y );
				pRenderer->LineTo( LockLocation.x, LockLocation.y );

				pRenderer->MoveTo( LockLocation.x + UnscaledUnits( 1 ), LockLocation.y + AdjustYCoordinate( UnscaledUnits( 4 ) ) );
				pRenderer->LineTo( LockLocation.x + UnscaledUnits( 1 ), LockLocation.y + AdjustYCoordinate( UnscaledUnits( 6 ) ) );
				pRenderer->LineTo( LockLocation.x + UnscaledUnits( 2 ), LockLocation.y + AdjustYCoordinate( UnscaledUnits( 7 ) ) );
				pRenderer->LineTo( LockLocation.x + UnscaledUnits( 3 ), LockLocation.y + AdjustYCoordinate( UnscaledUnits( 7 ) ) );
				pRenderer->LineTo( LockLocation.x + UnscaledUnits( 4 ), LockLocation.y + AdjustYCoordinate( UnscaledUnits( 6 ) ) );
				pRenderer->LineTo( LockLocation.x + UnscaledUnits( 4 ), LockLocation.y + AdjustYCoordinate( UnscaledUnits( 4 ) ) );

				pRenderer->SelectObject( pOldPen );

				Spacing += (int)UnscaledUnits( 12 );
			}

			pRenderer->TextOut( Average.x + Spacing, Average.y, Number );

			if( pLink->IsActuator() )
			{
				Average.y += AdjustYCoordinate( UnscaledUnits( m_UsingFontHeight ) );
				CString Speed;
				double CPM = pLink->GetCPM();
				if( fabs( CPM - floor( CPM ) ) < .0001 )
					Speed.Format( "%d", (int)CPM );
				else
					Speed.Format( "%.2lf", CPM );
				if( pLink->IsAlwaysManual() )
					Speed += "m";
				pRenderer->TextOut( Average.x - UnscaledUnits( 3 ) + Spacing, Average.y + UnscaledUnits( 4 ), Speed );
			}
		}
	}

	if( pLink->IsMeasurementElement() )
		return;

	Pen.CreatePen( PS_SOLID, pLink->GetLineSize(), Color );
	pOldPen = pRenderer->SelectObject( &Pen );

	if( pLink->IsActuator() )
		DrawActuator( pRenderer, OnLayers, pLink, bDrawFill );
	else
	{
		CConnector *pSlider = pLink->GetConnectedSlider( 0 );
		if( 0 && pLink->GetConnectorCount() == 2 && pLink->GetConnectedSliderCount() > 0 && !pLink->IsSolid() && pSlider != 0 && pSlider->GetSlideRadius() != 0 )
		{
			CFArc SliderArc;
			if( !pSlider->GetSliderArc( SliderArc ) )
				return;

			SliderArc = Scale( SliderArc );
			pRenderer->Arc( SliderArc );
		}
		else
		{
			int PointCount = 0;
			CFPoint* Points = pLink->GetHull( PointCount );
			CFPoint* PixelPoints = new CFPoint[PointCount];
			double *pHullRadii = new double[PointCount];
			// reverse the points because the -y change causes the hull to be in the wrong direction.
			for( int Index = 0; Index < PointCount; ++Index )
			{
				PixelPoints[PointCount-1-Index] = Scale( Points[Index] );
				pHullRadii[Index] = 0;
			}
			if( pLink->GetConnectedSliderCount() > 0 )
			{
				for( int Index = 0; Index < PointCount; ++Index )
				{
					for( int Counter = 0; Counter < pLink->GetConnectedSliderCount(); ++Counter )
					{
						CConnector *pSlider = pLink->GetConnectedSlider( Counter );
						if( pSlider == 0 || pSlider->GetSlideRadius() == 0 )
							continue;
						CFPoint Limit1;
						CFPoint Limit2;
						pSlider->GetSlideLimits( Limit1, Limit2 );
						Limit1 = Scale( Limit1 );
						Limit2 = Scale( Limit2 );
						CFPoint NextPixelPoint = Index == PointCount - 1 ? PixelPoints[0] : PixelPoints[Index+1];
						if( ( PixelPoints[Index] == Limit1 && NextPixelPoint == Limit2 ) 
						    || ( PixelPoints[Index] == Limit2 && NextPixelPoint == Limit1 ) )
						{
							pHullRadii[Index] = pSlider->GetSlideRadius();
							break;
						}
					}
				}
			}
			
			pRenderer->LinkageDrawExpandedPolygon( PixelPoints, pHullRadii, PointCount, Color, bDrawFill, pLink->IsSolid() ? ( m_ConnectorRadius + UnscaledUnits( 3 ) ) : UnscaledUnits( 0 ) );
			delete [] PixelPoints;
			delete [] pHullRadii;
			PixelPoints = 0;
		}

		if( pLink->IsGear() )
		{
			COLORREF GearColor = Color;

			CElementItem *pFastenedTo = pLink->GetFastenedTo();
			if( pFastenedTo != 0 && pFastenedTo->GetElement() != 0 )
				GearColor = pFastenedTo->GetElement()->GetColor();

			CConnector *pGearConnector = pLink->GetGearConnector();
			if( pGearConnector != 0 )
			{
				std::list<double> RadiusList;
				pLink->GetGearRadii( *pGearConnections, RadiusList );

				double LargestRadius = RadiusList.back();
				double SmallestRadius = RadiusList.front();

				if( bDrawFill )
				{
					double Red = GetRValue( GearColor ) / 255.0;
					double Green = GetGValue( GearColor ) / 255.0;
					double Blue = GetBValue( GearColor ) / 255.0;
					if( Red < 1.0 )
						Red += ( 1.0 - Red ) * .965;
					if( Green < 1.0 )
						Green += ( 1.0 - Green ) * .965;
					if( Blue < 1.0 )
						Blue += ( 1.0 - Blue ) * .965;

					COLORREF NewColor = RGB( (int)( Red * 255 ), (int)( Green * 255 ), (int)( Blue * 255 ) );
					CBrush Brush;
					Brush.CreateSolidBrush( NewColor );
					CBrush *pOldBrush = pRenderer->SelectObject( &Brush );
					pRenderer->SelectStockObject( NULL_PEN );

					CFCircle Circle( Scale( pGearConnector->GetPoint() ), Scale( LargestRadius ) );
					pRenderer->Circle( Circle );

					pRenderer->SelectObject( pOldBrush );
				}
				else
				{
					CPen AngleMarkerPen( PS_SOLID, 1, GearColor );
					//CPen GearPen( PS_DOT, 1, GearColor );
					CPen GearPen( PS_SOLID, pLink->GetLineSize(), GearColor );

					CBrush *pOldBrush = (CBrush*)pRenderer->SelectStockObject( NULL_BRUSH );

					if( pGearConnector != 0 )
					{
						pRenderer->SelectObject( &GearPen );
						for( std::list<double>::iterator it = RadiusList.begin(); it != RadiusList.end(); ++it )
						{
							CFCircle Circle( Scale( pGearConnector->GetPoint() ), Scale( *it ) );
							pRenderer->Circle( Circle );
						}
					}

					pRenderer->SelectObject( &AngleMarkerPen );
					CFPoint Point = pGearConnector->GetPoint();
					Point = Scale( Point );

					// Draw a small line to show rotation angle.
					CFPoint StartPoint( Point.x, Point.y + AdjustYCoordinate( Scale( LargestRadius ) ) );
					CFPoint EndPoint( Point.x, Point.y + AdjustYCoordinate( Scale( SmallestRadius / 2 ) ) );
					StartPoint.RotateAround( Point, -pLink->GetRotationAngle() );
					EndPoint.RotateAround( Point, -pLink->GetRotationAngle() );
					pRenderer->MoveTo( StartPoint );
					pRenderer->LineTo( EndPoint );

					pRenderer->SelectObject( pOldBrush );
					pRenderer->SelectObject( (CPen*)0 );

					if( !pLink->IsPositionValid() )
						DrawFailureMarks( pRenderer, OnLayers, Point, Scale( LargestRadius ) );
				}
			}
		}
	}

	pRenderer->SelectObject( pOldPen );
}

void CLinkageView::OnEditMakeparallelogram()
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if( pDoc->GetSelectedConnectorCount() != 4 || pDoc->GetFirstSelectedConnector() == 0 )
		return;
}

#if 0
void CLinkageView::OnUpdateEditMakeparallelogram(CCmdUI *pCmdUI)
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pCmdUI->Enable( !m_bSimulating && pDoc->GetSelectedConnectorCount() == 4 && pDoc->GetLastSelectedConnector() != 0 );
	pCmdUI->Enable( FALSE );
}

void CLinkageView::OnEditMakerighttriangle()
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if( pDoc->GetSelectedConnectorCount() != 3 || pDoc->GetLastSelectedConnector() == 0 )
		return;

	CImageSelectDialog Dlg;
	Dlg.DoModal();
}

void CLinkageView::OnUpdateEditMakerighttriangle(CCmdUI *pCmdUI)
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pCmdUI->Enable( pDoc->GetSelectedConnectorCount() == 3 && pDoc->GetLastSelectedConnector() != 0 );
//	pCmdUI->Enable( FALSE );
}
#endif

void CLinkageView::OnUpdateEditUndo(CCmdUI *pCmdUI)
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pCmdUI->Enable( pDoc->CanUndo() && !m_bSimulating );
}

void CLinkageView::OnEditUndo()
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->Undo();
	if( m_bShowParts )
		pDoc->SelectElement();
	UpdateForDocumentChange();
}

CRect CLinkageView::GetDocumentScrollingRect( void )
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CFRect DocumentArea = GetDocumentArea();

	CFRect DocumentRect = Scale( DocumentArea );

	static const int EDITBORDER = 50;

	DocumentRect.InflateRect( EDITBORDER, EDITBORDER );

	return CRect( (int)DocumentRect.left, (int)DocumentRect.top, (int)DocumentRect.right, (int)DocumentRect.bottom );
}

void CLinkageView::SetScrollExtents( bool bEnableBars )
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	m_ScrollOffsetAdjustment.SetPoint( 0, 0 );

	SCROLLINFO ScrollInfo;
	memset( &ScrollInfo, 0, sizeof( ScrollInfo ) );
	ScrollInfo.cbSize = sizeof( ScrollInfo );
	ScrollInfo.fMask = SIF_DISABLENOSCROLL | SIF_POS | SIF_PAGE | SIF_RANGE;

	if( pDoc->IsEmpty() || !bEnableBars || m_bSimulating )
	{
		SetScrollInfo( SB_HORZ, &ScrollInfo, TRUE );
		SetScrollInfo( SB_VERT, &ScrollInfo, TRUE );
		return;
	}

	CRect ClientRect = m_DrawingRect;

	m_Zoom = m_ScreenZoom;
	m_ScrollPosition = m_ScreenScrollPosition;

	CRect DocumentRect = GetDocumentScrollingRect();

	CRect ScrollDocumentRect( min( DocumentRect.left, ClientRect.left ),
				              min( DocumentRect.top, ClientRect.top ),
				              max( DocumentRect.right, ClientRect.right ) + 1,
				              max( DocumentRect.bottom, ClientRect.bottom ) + 1 );

	ScrollInfo.nMin = ScrollDocumentRect.left;
	ScrollInfo.nMax = ScrollDocumentRect.right - 1;
	ScrollInfo.nPage = ClientRect.Width() + 1;
	ScrollInfo.nPos = ClientRect.left;
	SetScrollInfo( SB_HORZ, &ScrollInfo, TRUE );

	ScrollInfo.nMin = ScrollDocumentRect.top;
	ScrollInfo.nMax = ScrollDocumentRect.bottom - 1;
	ScrollInfo.nPage = ClientRect.Height() + 1;
	ScrollInfo.nPos = ClientRect.top;
	SetScrollInfo( SB_VERT, &ScrollInfo, TRUE );
}

void CLinkageView::OnScroll( int WhichBar, UINT nSBCode, UINT nPos )
{
	int Minimum;
	int Maximum;
	GetScrollRange( WhichBar, &Minimum, &Maximum );
	int Current = GetScrollPos( WhichBar );
	int New = Current;
	int Move = 0;

	switch( nSBCode )
	{
		case SB_LEFT:
			New = Minimum;
			break;

		case SB_RIGHT:
			New = Maximum;
			break;

		case SB_ENDSCROLL:
			return;

		case SB_LINELEFT:
			New -= 10;
			if( New < Minimum )
				New = Minimum;
			break;

		case SB_LINERIGHT:
			New += 10;
			if( New > Maximum )
				New = Maximum;
			break;

		case SB_PAGELEFT:
		{
			// Get the page size.
			SCROLLINFO   info;
			GetScrollInfo( WhichBar, &info, SIF_ALL );

			if( info.nPage < 60 )
				info.nPage = 60;

			New -= info.nPage;
			if( New < Minimum )
				New = Minimum;
			break;
		}

		case SB_PAGERIGHT:
		{
			// Get the page size.
			SCROLLINFO   info;
			GetScrollInfo( WhichBar, &info, SIF_ALL );

			if( info.nPage < 60 )
				info.nPage = 60;

			New += info.nPage;
			if( New > Maximum - (int)info.nPage )
				New = Maximum - (int)info.nPage;
			break;
		}

		case SB_THUMBPOSITION:
			New = nPos;
			break;

		case SB_THUMBTRACK:
			New = nPos;
			break;
	}

	if( WhichBar == SB_VERT )
		m_ScreenScrollPosition.y += New - Current;
	else
		m_ScreenScrollPosition.x += New - Current;

	/*
	 * If thumb tracking, don't tweak the scroll bar size and position until after the scrolling is done.
	 */
	if( nSBCode == SB_THUMBTRACK )
		SetScrollPos( WhichBar, New );
	else
		SetScrollExtents();

	InvalidateRect( 0 );
}

void CLinkageView::OnVScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar )
{
	OnScroll( SB_VERT, nSBCode, nPos );
}

void CLinkageView::OnHScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar )
{
	OnScroll( SB_HORZ, nSBCode, nPos );
}

void CLinkageView::OnEditSlide()
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	pDoc->ConnectSliderLimits();

	UpdateForDocumentChange();
}

void CLinkageView::OnUpdateEditSlide(CCmdUI *pCmdUI)
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	pCmdUI->Enable( pDoc->IsSelectionSlideable() && !m_bSimulating && m_bAllowEdit );
}

void CLinkageView::InsertlinkSlider( CFPoint *pPoint )
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->InsertLink(  CLinkageDoc::MECHANISMLAYER, Unscale( LINK_INSERT_PIXELS ), pPoint == 0 ? GetDocumentViewCenter() : *pPoint, pPoint != 0, 1, false, false, true, false, false, false, false );
	m_bSuperHighlight = true;
	UpdateForDocumentChange();
}

void CLinkageView::InsertSliderCombo( CFPoint *pPoint )
{
	#if 0
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	static const char *SLIDER_COMBO_XML =
		"<Linkage>\
		<connector anchor=\"true\" x=\"520\" y=\"160\" id=\"0\"/>\
		<connector anchor=\"true\" x=\"647\" y=\"160\" id=\"1\"/>\
		<connector x=\"591\" y=\"160\" id=\"2\" slider=\"true\">\
			<slidelimit id=\"0\"/>\
			<slidelimit id=\"1\"/>\
		</connector>\
		<Link id=\"0\" linesize=\"1\" solid=\"true\">\
			<connector id=\"0\"/>\
			<connector id=\"1\"/>\
		</Link>\
		<Link id=\"2\" linesize=\"1\" actuator=\"true\" throw=\"38\" cpm=\"20\" >\
			<connector id=\"0\"/>\
			<connector id=\"2\"/>\
		</Link>\
		</Linkage>";

	pDoc->InsertXml( pPoint == 0 ? GetDocumentViewCenter() : *pPoint, pPoint != 0, SLIDER_COMBO_XML );
	m_bSuperHighlight = true;
	UpdateForDocumentChange();
	#endif
}

bool CLinkageView::ConnectorProperties( CConnector *pConnector )
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	double DocumentScale = pDoc->GetUnitScale();

	CConnectorPropertiesDialog Dialog;
	Dialog.m_RPM = pConnector->GetRPM();
	Dialog.m_bAnchor = pConnector->IsAnchor();
	Dialog.m_bInput = pConnector->IsInput();
	Dialog.m_bDrawing = pConnector->IsDrawing();
	Dialog.m_bAlwaysManual = pConnector->IsAlwaysManual();
	Dialog.m_xPosition = pConnector->GetPoint().x * DocumentScale;
	Dialog.m_yPosition = pConnector->GetPoint().y * DocumentScale;
	Dialog.m_bIsSlider = pConnector->IsSlider();
	Dialog.m_SlideRadius = pConnector->GetSlideRadius() * DocumentScale;
	Dialog.m_Name = pConnector->GetName();
	Dialog.m_MinimumSlideRadius = 0;
	if( pConnector->GetFastenedTo() != 0 && pConnector->GetFastenedTo()->GetElement() != 0 )
		Dialog.m_FastenedTo = "Fastened to " + pConnector->GetFastenedTo()->GetElement()->GetIdentifierString( m_bShowDebug );
	Dialog.m_Color = pConnector->GetColor();

	if( pConnector->IsSlider() )
	{
		CFLine LimitsLine;
		CFPoint Point2;
		if( pConnector->GetSlideLimits( LimitsLine.m_Start, LimitsLine.m_End ) )
			Dialog.m_MinimumSlideRadius = ( ( LimitsLine.GetDistance() / 2 )  * DocumentScale ) + 0.00009;
	}
	Dialog.m_Label = pConnector->GetIdentifierString( m_bShowDebug );
	Dialog.m_Label += " - Connector Properties";

	if( Dialog.DoModal() == IDOK )
	{
		pDoc->PushUndo();

		pConnector->SetRPM( Dialog.m_RPM );
		bool bNoLongerAnAnchor = pConnector->IsAnchor() && !Dialog.m_bAnchor;
		pConnector->SetAsAnchor( Dialog.m_bAnchor );
		pConnector->SetAsInput( Dialog.m_bInput );
		pConnector->SetAsDrawing( Dialog.m_bDrawing );
		pConnector->SetAlwaysManual( Dialog.m_bAlwaysManual );
		pConnector->SetName( Dialog.m_Name );
		pConnector->SetColor( Dialog.m_Color );

		if( Dialog.m_xPosition != pConnector->GetPoint().x ||
		    Dialog.m_yPosition != pConnector->GetPoint().y )
		{
			pConnector->SetPoint( Dialog.m_xPosition / DocumentScale, Dialog.m_yPosition / DocumentScale );
		}
		double NewSlideRadius = Dialog.m_SlideRadius / DocumentScale;
		if( NewSlideRadius != pConnector->GetSlideRadius() )
		{
			pConnector->SetSlideRadius( NewSlideRadius );
			CFPoint Point = pConnector->GetPoint();
			CFArc TheArc;
			if( pConnector->GetSliderArc( TheArc ) )
			{
				Point.SnapToArc( TheArc );
				pConnector->SetPoint( Point );
			}
		}

		if( bNoLongerAnAnchor )
		{
			// Anchor-to-anchor gear connections break when the connector is no longer an anchor.
			pDoc->RemoveGearRatio( pConnector, 0 );
		}

		return true;
	}
	return false;
}

bool CLinkageView::PointProperties( CConnector *pConnector )
{
//	CTestDialog Test;
//	Test.DoModal();
//	return false;

	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	double DocumentScale = pDoc->GetUnitScale();

	CPointPropertiesDialog Dialog;
	Dialog.m_RPM = pConnector->GetRPM();
	Dialog.m_xPosition = pConnector->GetPoint().x * DocumentScale;
	Dialog.m_yPosition = pConnector->GetPoint().y * DocumentScale;
	Dialog.m_Radius = pConnector->GetDrawCircleRadius() * DocumentScale;
	Dialog.m_bDrawCircle = pConnector->GetDrawCircleRadius() != 0 ? TRUE : FALSE;
	Dialog.m_Name = pConnector->GetName();
	Dialog.m_Label = pConnector->GetIdentifierString( m_bShowDebug );
	Dialog.m_Label += " - Point Properties";
	if( pConnector->GetFastenedTo() != 0 && pConnector->GetFastenedTo()->GetElement() != 0 )
		Dialog.m_FastenTo = "Fastened to " + pConnector->GetFastenedTo()->GetElement()->GetIdentifierString( m_bShowDebug );
	Dialog.m_Color = pConnector->GetColor();

	if( Dialog.DoModal() == IDOK )
	{
		pDoc->PushUndo();

		pConnector->SetRPM( Dialog.m_RPM );
		pConnector->SetName( Dialog.m_Name );
		pConnector->SetDrawCircleRadius( Dialog.m_bDrawCircle != FALSE ? Dialog.m_Radius / DocumentScale : 0.0 );
		pConnector->SetColor( Dialog.m_Color );

		if( Dialog.m_xPosition != pConnector->GetPoint().x ||
		    Dialog.m_yPosition != pConnector->GetPoint().y )
		{
			pConnector->SetPoint( Dialog.m_xPosition / DocumentScale, Dialog.m_yPosition / DocumentScale );
		}

		return true;
	}
	return false;
}

bool CLinkageView::LinkProperties( CLink *pLink )
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CLinkPropertiesDialog Dialog;
	if( pLink == 0 )
	{
		Dialog.m_LineSize = 1;
		Dialog.m_bSolid = FALSE;
		Dialog.m_bActuator = FALSE;
		Dialog.m_ConnectorCount = 2;
		Dialog.m_ActuatorCPM = 0;
		Dialog.m_bAlwaysManual = FALSE;
		Dialog.m_SelectedLinkCount = 0;
		Dialog.m_ThrowDistance = 0;
		Dialog.m_bIsGear = false;
		Dialog.m_Label = "Multiple Link Properties";
		Dialog.m_Color = RGB( 0, 0, 0 );
		Dialog.m_bFastened = false;
		Dialog.m_bLocked = false;

		LinkList* pLinkList = pDoc->GetLinkList();
		POSITION Position = pLinkList->GetHeadPosition();
		while( Position != NULL )
		{
			CLink* pSelectedLink = pLinkList->GetNext( Position );
			if( pSelectedLink != 0 && pSelectedLink->IsSelected() )
			{
				Dialog.m_LineSize = max( Dialog.m_LineSize, pSelectedLink->GetLineSize() );
				Dialog.m_bSolid = Dialog.m_bSolid || pSelectedLink->IsSolid();
				// Dialog.m_bIsGear = Dialog.m_bIsGear || pSelectedLink->IsGear(); CANT CHANGE THIS SO DONT SHOW IT!
				++Dialog.m_SelectedLinkCount;
			}
		}
	}
	else
	{
		Dialog.m_LineSize = pLink->GetLineSize();
		Dialog.m_bSolid = pLink->IsSolid();
		Dialog.m_bActuator = pLink->IsActuator();
		Dialog.m_ConnectorCount = pLink->GetConnectorCount();
		Dialog.m_ActuatorCPM = pLink->GetCPM();
		Dialog.m_bAlwaysManual = pLink->IsAlwaysManual();
		Dialog.m_ThrowDistance = pLink->GetStroke() * pDoc->GetUnitScale();
		if( !pLink->IsActuator() )
			Dialog.m_ThrowDistance = pLink->GetLength() / 2 * pDoc->GetUnitScale();
		Dialog.m_SelectedLinkCount = 1;
		Dialog.m_bIsGear = pLink->IsGear();
		Dialog.m_bLocked = pLink->IsLocked();
		Dialog.m_Label = pLink->GetIdentifierString( m_bShowDebug );
		Dialog.m_Label += " - Link Properties";
		Dialog.m_Name = pLink->GetName();
		if( pLink->GetFastenedTo() != 0 && pLink->GetFastenedTo()->GetElement() != 0 )
			Dialog.m_FastenedTo = "Fastened to " + pLink->GetFastenedTo()->GetElement()->GetIdentifierString( m_bShowDebug );
		Dialog.m_bFastened = pLink->GetFastenedTo() != 0;
		Dialog.m_Color = pLink->GetColor();
	}
	if( Dialog.DoModal() == IDOK )
	{
		pDoc->PushUndo();

		if( pLink == 0 )
		{
			LinkList* pLinkList = pDoc->GetLinkList();
			POSITION Position = pLinkList->GetHeadPosition();
			while( Position != NULL )
			{
				CLink* pSelectedLink = pLinkList->GetNext( Position );
				if( pSelectedLink != 0 && pSelectedLink->IsSelected() )
				{
					pSelectedLink->SetLineSize( Dialog.m_LineSize );
					pSelectedLink->SetSolid( Dialog.m_bSolid != 0 );
//					pSelectedLink->SetActuator( Dialog.m_bActuator != 0 );
//					pSelectedLink->SetCPM( Dialog.m_ActuatorCPM );
//					pSelectedLink->SetAlwaysManual( Dialog.m_bAlwaysManual != 0 );
				}
			}
		}
		else
		{
			pLink->SetLineSize( Dialog.m_LineSize );
			pLink->SetSolid( Dialog.m_bSolid != 0 );
			pLink->SetLocked( Dialog.m_bLocked != 0 );
			pLink->SetActuator( Dialog.m_bActuator != 0 );
			pLink->SetCPM( Dialog.m_ActuatorCPM );
			pLink->SetAlwaysManual( Dialog.m_bAlwaysManual != 0 );
			pLink->SetName( Dialog.m_Name );
			pLink->SetStroke( Dialog.m_ThrowDistance / pDoc->GetUnitScale() );
			pLink->SetColor( Dialog.m_Color );
		}

		return true;
	}
	return false;
}

bool CLinkageView::LineProperties( CLink *pLink )
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CLinePropertiesDialog Dialog;
	if( pLink == 0 )
	{
		Dialog.m_LineSize = 1;
		Dialog.m_SelectedLinkCount = 0;
		Dialog.m_bMeasurementLine = FALSE;
		Dialog.m_Label = "Multiple Line/Link Properties";
//		Dialog.m_Color = pLink->GetColor();

		LinkList* pLinkList = pDoc->GetLinkList();
		POSITION Position = pLinkList->GetHeadPosition();
		while( Position != NULL )
		{
			CLink* pSelectedLink = pLinkList->GetNext( Position );
			if( pSelectedLink != 0 && pSelectedLink->IsSelected() )
			{
				Dialog.m_LineSize = max( Dialog.m_LineSize, pSelectedLink->GetLineSize() );
				++Dialog.m_SelectedLinkCount;
			}
		}
	}
	else
	{
		Dialog.m_LineSize = pLink->GetLineSize();
		Dialog.m_bMeasurementLine = pLink->IsMeasurementElement() ? TRUE : FALSE;
		Dialog.m_SelectedLinkCount = 1;
		Dialog.m_Label = pLink->GetIdentifierString( m_bShowDebug );
		Dialog.m_Label += " - Line Properties";
		Dialog.m_Name = pLink->GetName();
		if( pLink->GetFastenedTo() != 0 && pLink->GetFastenedTo()->GetElement() != 0 )
			Dialog.m_FastenTo = "Fastened to " + pLink->GetFastenedTo()->GetElement()->GetIdentifierString( m_bShowDebug );
		Dialog.m_Color = pLink->GetColor();
	}
	if( Dialog.DoModal() == IDOK )
	{
		pDoc->PushUndo();

		if( pLink == 0 )
		{
			LinkList* pLinkList = pDoc->GetLinkList();
			POSITION Position = pLinkList->GetHeadPosition();
			while( Position != NULL )
			{
				CLink* pSelectedLink = pLinkList->GetNext( Position );
				if( pSelectedLink != 0 && pSelectedLink->IsSelected() )
				{
					pSelectedLink->SetLineSize( Dialog.m_LineSize );
				}
			}
		}
		else
		{
			pLink->SetLineSize( Dialog.m_LineSize );
			pLink->SetName( Dialog.m_Name );
			pLink->SetMeasurementElement( Dialog.m_bMeasurementLine != FALSE );
			pLink->SetColor( Dialog.m_Color );
		}

		return true;
	}
	return false;
}

void CLinkageView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
	if( pDC->IsPrinting() )
	{
		CLinkageDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		CFRect Area = GetDocumentArea();
		if( Area.Width() > Area.Height() )
		{
			DEVMODE* pDevMode = pInfo->m_pPD->GetDevMode();
			pDevMode->dmOrientation = DMORIENT_LANDSCAPE;
			pDevMode->dmFields |= DM_ORIENTATION;
			pDC->ResetDC( pDevMode );
		}
	}

	CView::OnPrepareDC(pDC, pInfo);
}

double CLinkageView::ConvertToSeconds( const char *pHMSString )
{
	double Value1 = 0;
	double Value2 = 0;
	double Value3 = 0;
	char Dummy = 0;

	int Count = sscanf_s( pHMSString, "%lf:%lf:%lf%c", &Value1, &Value2, &Value3, &Dummy, 1 );

	switch( Count )
	{
		case 1: return Value1;
		case 2: return ( Value1 * 60.0 ) + Value2;
		case 3: return ( Value1 * 3600.0 ) + ( Value2 * 60.0 ) + Value3;
		default: return 0.0;
	}
	return 0.0;
}

void CLinkageView::OnFileSaveVideo()
{
	CFileDialog dlg( 0, "avi", 0, 0, "AVI Video Files|*.avi|All Files|*.*||" );
	dlg.m_ofn.lpstrTitle = "Save Video";
	dlg.m_ofn.Flags |= OFN_NOCHANGEDIR | OFN_NONETWORKBUTTON | OFN_OVERWRITEPROMPT;
	if( dlg.DoModal() != IDOK )
		return;

	CRecordDialog RecordDialog;
	if( RecordDialog.DoModal() != IDOK )
		return;

	if( !InitializeVideoFile( dlg.GetPathName(), RecordDialog.m_EncoderName ) )
		return;

	m_bRecordingVideo = true;
	m_RecordQuality = RecordDialog.m_QualitySelection;

	m_VideoStartFrames = (int)( RecordDialog.m_bUseStartTime ? ConvertToSeconds( RecordDialog.m_StartTime ) * FRAMES_PER_SECONDS : 0 );
	m_VideoRecordFrames = (int)( RecordDialog.m_bUseRecordTime ? ConvertToSeconds( RecordDialog.m_RecordTime ) * FRAMES_PER_SECONDS : 0 );
	m_VideoEndFrames = (int)( RecordDialog.m_bUseEndTime ? ConvertToSeconds( RecordDialog.m_EndTime ) * FRAMES_PER_SECONDS : 0 );

	if( RecordDialog.m_bUseStartTime || RecordDialog.m_bUseRecordTime || RecordDialog.m_bUseEndTime )
		m_bUseVideoCounters = true;
	else
		m_bUseVideoCounters = false;

	ConfigureControlWindow( AUTO );
	StartMechanismSimulate( AUTO );
}

void CLinkageView::OnFileSaveImage()
{
	CExportImageSettingsDialog SizeDialog;
	SizeDialog.m_pLinkageView = this;
	if( SizeDialog.DoModal() != IDOK )
		return;

	CFileDialog dlg( 0, "jpg", 0, 0, "JPEG Files (*.jpg)|*.jpg|PNG Files (*.png)|*.png|All Supported Image Files (*.jpg;*.png)|*.jpg;*.png|All Files (*.*)|*.*||" );
	dlg.m_ofn.lpstrTitle = "Save As JPEG or PNG Image";
	dlg.m_ofn.Flags |= OFN_NOCHANGEDIR | OFN_NONETWORKBUTTON | OFN_OVERWRITEPROMPT;
	if( dlg.DoModal() != IDOK )
		return;

	SaveAsImage( dlg.GetPathName(), SizeDialog.GetResolutionWidth(), SizeDialog.GetResolutionHeight(), SizeDialog.GetResolutionScale(), SizeDialog.GetMarginScale() );
}

void CLinkageView::OnFileSaveDXF()
{
	if( AfxMessageBox( "This feature is not complete. Solid links and dotted lines, as well as some other features, may not work properly.", MB_OKCANCEL | MB_ICONEXCLAMATION ) != IDOK )
		return;

	CFileDialog dlg( 0, "dxf", 0, 0, "DXF Files (*.dxf)|*.dxf|All Files (*.*)|*.*||" );
	dlg.m_ofn.lpstrTitle = "Export to DXF";
	dlg.m_ofn.Flags |= OFN_NOCHANGEDIR | OFN_NONETWORKBUTTON | OFN_OVERWRITEPROMPT;
	if( dlg.DoModal() != IDOK )
		return;

	SaveAsDXF( dlg.GetPathName() );
}

void CLinkageView::OnUpdateFileSaveMotion( CCmdUI *pCmdUI )
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	int Count = 0;

	ConnectorList* pConnectors = pDoc->GetConnectorList();
	POSITION Position = pConnectors->GetHeadPosition();
	while( Position != 0 )
	{
		CConnector *pConnector = pConnectors->GetNext( Position );
		if( pConnector == 0 )
			continue;
		if( !pConnector->IsDrawing() )
			continue;

		int DrawPoint = 0;
		int PointCount = 0;
		int MaxPoints = 0;
		pConnector->GetMotionPath( DrawPoint, PointCount, MaxPoints );
		Count += PointCount;
		if( Count > 0 )
			break;
	}

	pCmdUI->Enable( Count > 0 );
}

void CLinkageView::OnFileSaveMotion()
{
	CFileDialog dlg( 0, "txt", 0, 0, "Text Files (*.txt)|*.txt|All Files (*.*)|*.*||" );
	dlg.m_ofn.lpstrTitle = "Export Motion Paths";
	dlg.m_ofn.Flags |= OFN_NOCHANGEDIR | OFN_NONETWORKBUTTON | OFN_OVERWRITEPROMPT;
	if( dlg.DoModal() != IDOK )
		return;

	CFile OutputFile;
	if( !OutputFile.Open( dlg.GetPathName(), CFile::modeCreate | CFile::modeWrite ) )
	{
		AfxMessageBox( "The selected file cannot be opened.", MB_OKCANCEL | MB_ICONEXCLAMATION );
		return;
	}

	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	double DocumentScale = pDoc->GetUnitScale();

	ConnectorList* pConnectors = pDoc->GetConnectorList();
	POSITION Position = pConnectors->GetHeadPosition();
	bool bFirst = true;
	while( Position != 0 )
	{
		CConnector *pConnector = pConnectors->GetNext( Position );
		if( pConnector == 0 )
			continue;
		if( !pConnector->IsDrawing() )
			continue;

		int DrawPoint = 0;
		int PointCount = 0;
		int MaxPoints = 0;
		CFPoint *pPoints = pConnector->GetMotionPath( DrawPoint, PointCount, MaxPoints );

		CString Name = "Connector ";
		Name += pConnector->GetIdentifierString( m_bShowDebug );
		Name += "\r\n";
		OutputFile.Write( Name, Name.GetLength() );

		CString Temp;
		Temp.Format( "%d\r\n", PointCount );
		OutputFile.Write( Temp, Temp.GetLength() );

		for( int Counter = 0; Counter < PointCount; ++Counter )
		{
			Temp.Format( "%.6f,%.6f\r\n", pPoints[Counter].x, pPoints[Counter].y );
			OutputFile.Write( (const char*)Temp, Temp.GetLength() );
		}
	}
}

void CLinkageView::OnFilePrint()
{
}

void CLinkageView::OnFilePrintPreview()
{
	ShowPrintPreviewCategory();
	CView::OnFilePrintPreview();
}

void CLinkageView::OnEndPrintPreview( CDC* pDC, CPrintInfo* pInfo, POINT point, CPreviewView* pView )
{
	CView::OnEndPrintPreview( pDC, pInfo, point, pView );
	ShowPrintPreviewCategory( false );
}

void CLinkageView::OnSimulateInteractive()
{
	ConfigureControlWindow( INDIVIDUAL );
	StartMechanismSimulate( INDIVIDUAL );
}

void CLinkageView::OnSimulatePause()
{
	if( m_bSimulating && m_SimulationControl == AUTO )
		m_SimulationControl = STEP;
	else
	{
		ConfigureControlWindow( STEP );
		StartMechanismSimulate( STEP );
	}
}

void CLinkageView::OnSimulateForward()
{
	m_SimulationSteps = 1;
}

void CLinkageView::OnSimulateBackward()
{
	m_SimulationSteps = -1;
}

void CLinkageView::OnSimulateForwardBig()
{
	m_SimulationSteps = 10;
}

void CLinkageView::OnSimulateBackwardBig()
{
	m_SimulationSteps = -10;
}

void CLinkageView::OnUpdateSimulatePause(CCmdUI *pCmdUI)
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pCmdUI->Enable( ( !m_bSimulating || m_SimulationControl == AUTO ) && !pDoc->IsEmpty() && m_bAllowEdit );
}

void CLinkageView::OnUpdateSimulateForwardBackward(CCmdUI *pCmdUI)
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pCmdUI->Enable( m_bSimulating && m_SimulationControl == STEP && !pDoc->IsEmpty() );
}

void CLinkageView::OnUpdateSimulateInteractive(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( !m_bSimulating && m_bAllowEdit );
}

void CLinkageView::OnSimulateManual()
{
	ConfigureControlWindow( GLOBAL );
	StartMechanismSimulate( GLOBAL );
}

void CLinkageView::OnUpdateSimulateManual(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( !m_bSimulating && m_bAllowEdit );
}

void CLinkageView::OnInsertDouble()
{
	InsertDouble( 0 );
}

void CLinkageView::OnInsertAnchor()
{
	InsertAnchor( 0 );
}

void CLinkageView::OnInsertAnchorLink()
{
	InsertAnchorLink( 0 );
}

void CLinkageView::OnInsertRotatinganchor()
{
	InsertRotatinganchor( 0 );
}

void CLinkageView::OnInsertActuator()
{
	InsertActuator( 0 );
}

void CLinkageView::OnInsertConnector()
{
	InsertConnector( 0 );
}

void CLinkageView::OnInsertlinkSlider()
{
	InsertlinkSlider( 0 );
}

void CLinkageView::OnInsertTriple()
{
	InsertTriple( 0 );
}

void CLinkageView::OnInsertQuad()
{
	InsertQuad( 0 );
}

void CLinkageView::OnInsertPoint()
{
	InsertPoint( 0 );
}

void CLinkageView::OnInsertLine()
{
	InsertLine( 0 );
}

void CLinkageView::OnInsertMeasurement()
{
	InsertMeasurement( 0 );
}

void CLinkageView::OnInsertGear()
{
	InsertGear( 0 );
}

void CLinkageView::OnInsertLink()
{
	InsertLink( 0 );
}

void CLinkageView::OnPopupGallery()
{
	if( m_pPopupGallery == 0 )
		return;
	int Selection = m_pPopupGallery->GetSelection();

	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CFPoint PopupPoint = Unscale( m_PopupPoint );

	switch( Selection )
	{
		case 0: InsertConnector( &PopupPoint ); break;
		case 1: InsertAnchor( &PopupPoint ); break;
		case 2: InsertDouble( &PopupPoint ); break;
		case 3: InsertAnchorLink( &PopupPoint ); break;
		case 4: InsertRotatinganchor( &PopupPoint ); break;
		case 5: InsertActuator( &PopupPoint ); break;
		case 6: InsertTriple( &PopupPoint ); break;
		case 7: InsertQuad( &PopupPoint ); break;
		case 8: InsertPoint( &PopupPoint ); break;
		case 9: InsertLine( &PopupPoint ); break;
		case 10: InsertMeasurement( &PopupPoint ); break;
		case 11: InsertGear( &PopupPoint ); break;
		default: return;
	}
}

void CLinkageView::InsertActuator( CFPoint *pPoint )
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->InsertLink( CLinkageDoc::MECHANISMLAYER, Unscale( LINK_INSERT_PIXELS ), pPoint == 0 ? GetDocumentViewCenter() : *pPoint, pPoint != 0, 2, false, false, false, true, false, m_bNewLinksSolid, false );
	m_bSuperHighlight = true;
	UpdateForDocumentChange();
}

int CLinkageView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	#if defined( LINKAGE_USE_DIRECT2D )
		CWindowDC DC( this );
		int PPI = DC.GetDeviceCaps( LOGPIXELSX );
		m_DPIScale = (double)PPI / 96.0;
	#else
		m_DPIScale = 1.0;
	#endif

	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	m_ControlWindow.Create( 0, 0, WS_CHILD | WS_CLIPCHILDREN, CRect( 0, 0, 100, 100 ), this, ID_CONTROL_WINDOW );
	m_ControlWindow.ShowWindow( SW_HIDE );

    BOOL success = m_OleWndDropTarget.Register(this);

	return 0;
}

void CLinkageView::OnDestroy()
{
	m_bRequestAbort = true;

	m_ControlWindow.DestroyWindow();

	CView::OnDestroy();
}

COleDropWndTarget::COleDropWndTarget() {}

COleDropWndTarget::~COleDropWndTarget() {}

DROPEFFECT COleDropWndTarget::OnDragEnter(CWnd* pWnd, COleDataObject*
           pDataObject, DWORD dwKeyState, CPoint point )
{
	// We always copy the dropped item.
	return DROPEFFECT_COPY;
}

void COleDropWndTarget::OnDragLeave(CWnd* pWnd)
{
    COleDropTarget::OnDragLeave(pWnd);
}

DROPEFFECT COleDropWndTarget::OnDragOver(CWnd* pWnd, COleDataObject*
           pDataObject, DWORD dwKeyState, CPoint point )
{
	// We always copy the dropped item and allow a drop anywhere in the view window.
	return DROPEFFECT_COPY;
}

BOOL COleDropWndTarget::OnDrop(CWnd* pWnd, COleDataObject* pDataObject,
                 DROPEFFECT dropEffect, CPoint point )
{
    HGLOBAL  hGlobal;
    LPCSTR   pData;

    UINT CF_Linkage = RegisterClipboardFormat( "RECTORSQUID_Linkage_CLIPBOARD_XML_FORMAT" );

    // Get text data from COleDataObject
    hGlobal=pDataObject->GetGlobalData(CF_Linkage);

	if( hGlobal == 0 )
		return FALSE;

    // Get pointer to data
    pData=(LPCSTR)GlobalLock(hGlobal);

    ASSERT(pData!=NULL);

    // Unlock memory - Send dropped text into the "bit-bucket"
    GlobalUnlock(hGlobal);

    return TRUE;
}

void CLinkageView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CView::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CLinkageView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	/*
	 * Handle a few special keys here because accelerators cannot be used
	 * for them. The accelerators keep the characters from being used in
	 * an edit control in the ribbon bar.
	 */

	switch( nChar )
	{
		case '-':
		case '_':
			OnViewZoomin();
			break;
		case '+':
		case '=':
			OnViewZoomout();
			break;
	}
	CView::OnChar(nChar, nRepCnt, nFlags);
}

void CLinkageView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	bool ShiftControlPressed = false;
	if( ( GetKeyState( VK_SHIFT ) & 0x8000 ) != 0 )
		ShiftControlPressed = true;
	else if( ( GetKeyState( VK_CONTROL ) & 0x8000 ) != 0)
		ShiftControlPressed = true;

	if( nChar == VK_TAB )
	{
		CLinkageDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);

		if( pDoc->SelectNext( ShiftControlPressed ? pDoc->PREVIOUS : pDoc->NEXT ) )
		{
			ShowSelectedElementStatus();
			UpdateForDocumentChange();
		}
	}
	else if( nChar == VK_LEFT || nChar == VK_RIGHT )
	{
		if( nChar == VK_RIGHT )
		{
			if( ShiftControlPressed )
				OnSimulateForwardBig();
			else
				OnSimulateForward();
		}
		else
		{
			if( ShiftControlPressed )
				OnSimulateBackwardBig();
			else
				OnSimulateBackward();
		}
	}
	else
		CView::OnKeyDown( nChar, nRepCnt, nFlags );
}

BOOL CLinkageView::PreTranslateMessage(MSG* pMsg)
{
	if( pMsg->message == WM_KEYDOWN && pMsg->wParam==VK_TAB )
	{
		OnKeyDown( VK_TAB, 0, 0 );
		return TRUE;
	}

	return CView::PreTranslateMessage(pMsg);
}

/*
 * Scale to get from document units to pixels. Document units are 96 DPI on a
 * typical screen. The document unit selection, inches or mm, are not used in
 * internal processes, just in showing measurement data to the user and
 * measurement input.
 *
 * Scale is also a document to pixel conversion function.
 *
 * Y document coordinates are negated to get from document to the screen
 * coordinate system and back.
 */

void CLinkageView::Scale( double &x, double &y )
{
	y *= m_YUpDirection;
	x *= m_Zoom;
	y *= m_Zoom;
	x -= m_ScrollPosition.x;
	y -= m_ScrollPosition.y;
	x += m_DrawingRect.Width() / m_DPIScale / 2;
	y += m_DrawingRect.Height() / m_DPIScale / 2;
}

void CLinkageView::Unscale( double &x, double &y )
{
	x -= m_DrawingRect.Width() / m_DPIScale / 2;
	y -= m_DrawingRect.Height() / m_DPIScale / 2;
	x += m_ScrollPosition.x;
	y += m_ScrollPosition.y;
	x /= m_Zoom;
	y /= m_Zoom;
	y *= m_YUpDirection;
}

CFPoint CLinkageView::Unscale( CFPoint Point )
{
	CFPoint NewPoint = Point;
	Unscale( NewPoint.x, NewPoint.y );
	return NewPoint;
}

double CLinkageView::Unscale( double Distance )
{
	return Distance / m_Zoom;
}

double CLinkageView::Scale( double Distance )
{
	return Distance * m_Zoom;
}

CFRect CLinkageView::Unscale( CFRect Rect )
{
	CFRect NewRect( Rect.left, Rect.top, Rect.right, Rect.bottom );
	Unscale( NewRect.left, NewRect.top );
	Unscale( NewRect.right, NewRect.bottom );
	NewRect.Normalize();
	return NewRect;
}

CFRect CLinkageView::Scale( CFRect Rect )
{
	Scale( Rect.left, Rect.top );
	Scale( Rect.right, Rect.bottom );
	CFRect NewRect( Rect.left, Rect.top, Rect.right, Rect.bottom );
	NewRect.Normalize();
	return NewRect;
}

CFArc CLinkageView::Unscale( CFArc TheArc )
{
	CFPoint Center = Unscale( CFPoint( TheArc.x, TheArc.y ) );
	CFLine TempLine = Unscale( CFLine( TheArc.m_Start, TheArc.m_End ) );
	CFArc NewArc( Center, TheArc.r / m_Zoom, TempLine.m_Start, TempLine.m_End );
	return NewArc;
}

CFArc CLinkageView::Scale( CFArc TheArc )
{
	CFPoint Center = Scale( CFPoint( TheArc.x, TheArc.y ) );
	CFLine TempLine = Scale( CFLine( TheArc.m_Start, TheArc.m_End ) );
	CFArc NewArc( Center, TheArc.r * m_Zoom, TempLine.m_Start, TempLine.m_End );
	return NewArc;
}

CFLine CLinkageView::Unscale( CFLine Line )
{
	CFLine NewLine = Line;
	Unscale( NewLine.m_Start.x, NewLine.m_Start.y );
	Unscale( NewLine.m_End.x, NewLine.m_End.y );
	return NewLine;
}

CFLine CLinkageView::Scale( CFLine Line )
{
	CFLine NewLine = Line;
	Scale( NewLine.m_Start.x, NewLine.m_Start.y );
	Scale( NewLine.m_End.x, NewLine.m_End.y );
	return NewLine;
}

CFPoint CLinkageView::Scale( CFPoint Point )
{
	Scale( Point.x, Point.y );
	return Point;
}

CFCircle CLinkageView::Unscale( CFCircle Circle )
{
	CFCircle NewCircle = Circle;
	Unscale( NewCircle.x, NewCircle.y );
	NewCircle.r /= m_Zoom;
	return NewCircle;
}

CFCircle CLinkageView::Scale( CFCircle Circle )
{
	CFCircle NewCircle = Circle;
	Scale( NewCircle.x, NewCircle.y );
	NewCircle.r *= m_Zoom;
	return NewCircle;
}

static int Selection = 0;

void CLinkageView::OnViewUnits()
{
	CMFCRibbonBar *pRibbon = ((CFrameWndEx*) AfxGetMainWnd())->GetRibbonBar();
	if( pRibbon == 0 )
		return;
	CMFCRibbonComboBox * pComboBox = DYNAMIC_DOWNCAST( CMFCRibbonComboBox, pRibbon->FindByID( ID_VIEW_UNITS ) );
	if( pComboBox == 0 )
		return;

	int nCurSel = pComboBox->GetCurSel();
	if( nCurSel < 0 )
		return;

	DWORD Data = pComboBox->GetItemData( nCurSel );

	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if( pDoc->GetUnits() != (CLinkageDoc::_Units)Data )
	{
		pDoc->SetUnits( (CLinkageDoc::_Units)Data );
		pDoc->SetModifiedFlag();
	}

	ShowSelectedElementCoordinates();
	InvalidateRect( 0 );
}

void CLinkageView::OnUpdateViewUnits( CCmdUI *pCmdUI )
{
//	CLinkageDoc* pDoc = GetDocument();
//	ASSERT_VALID(pDoc);
//	CLinkageDoc::_Units Units = pDoc->GetUnits();

	pCmdUI->Enable( !m_bSimulating );
}

void CLinkageView::ShowSelectedElementCoordinates( void )
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

//	if( pDoc->GetSelectedLinkCount( true ) > 0 )
//		return;

	CMFCRibbonBar *pRibbon = ((CFrameWndEx*) AfxGetMainWnd())->GetRibbonBar();
	if( pRibbon == 0 )
		return;
	CMFCRibbonEdit * pEditBox = DYNAMIC_DOWNCAST( CMFCRibbonEdit, pRibbon->FindByID( ID_VIEW_COORDINATES ) );
	if( pEditBox == 0 )
		return;

	CString Text;

	if( pDoc->IsSelectionMeshableGears() )
	{
		CLink *pLink1 = pDoc->GetSelectedLink( 0, false );
		CLink *pLink2 = pDoc->GetSelectedLink( 1, false );
		double Size1 = 0;
		double Size2 = 0;

		CGearConnection *pConnection = pDoc->GetGearRatio( pLink1, pLink2, &Size1, &Size2 );

		if( pConnection == 0 || Size1 == 0 || Size2 == 0 )
			pEditBox->SetEditText( "" );
		else
		{
			CString Temp;
			Temp.Format( "%.4lf", Size1 );
			Temp.TrimRight( ".0" );
			Text = Temp;
			Text += ":";
			Temp.Format( "%.4lf", Size2 );
			Temp.TrimRight( ".0" );
			Text += Temp;
			pEditBox->SetEditText( Text );
		}
		return;
	}

	double DocumentScale = pDoc->GetUnitScale();

	CConnector *pConnector0 = 0;
	CConnector *pConnector1 = 0;
	CConnector *pConnector2 = 0;

	CLink *pSelectedLink = 0;

	bool bEnableEdit = true;

	int SelectedCount = pDoc->GetSelectedConnectorCount();

	pConnector0 = pDoc->GetSelectedConnector( 0 );
	if( pConnector0 == 0 )
	{
		if( pDoc->GetSelectedLinkCount( false ) != 1 )
		{
			pEditBox->SetEditText( "" );
			return;
		}

		CLink *pSelectedLink = pDoc->GetSelectedLink( 0, false );
		if( pSelectedLink == 0 )
		{
			pEditBox->SetEditText( "" );
			return;
		}

		if( pSelectedLink->GetConnectorCount() != 2 )
		{
			pEditBox->SetEditText( "" );
			return;
		}

		pConnector0 = pSelectedLink->GetConnector( 0 );
		pConnector1 = pSelectedLink->GetConnector( 1 );

		SelectedCount = 2;
	}
	else
	{
		pConnector1 = pDoc->GetSelectedConnector( 1 );
		pConnector2 = pDoc->GetSelectedConnector( 2 );
	}

	if( SelectedCount < 1 || SelectedCount > 3 )
	{
		pEditBox->SetEditText( "" );
		return;
	}

	CFPoint Point0 = pConnector0->GetOriginalPoint();
	Point0.x *= DocumentScale;
	Point0.y *= DocumentScale;

	switch( SelectedCount )
	{
		case 1:
		{
			Text.Format( "%.4lf,%.4lf", Point0.x, Point0.y );
			break;
		}

		case 2:
		{
			if( pConnector1 == 0 )
			{
				pEditBox->SetEditText( "" );
				return;
			}
			CFPoint Point1 = pConnector1->GetOriginalPoint();
			Point1.x *= DocumentScale;
			Point1.y *= DocumentScale;

			double distance = Distance( Point0, Point1 );
			Text.Format( "%.4lf", distance );
			break;
		}

		case 3:
		{
			if( pConnector1 == 0 || pConnector2 == 0 )
			{
				pEditBox->SetEditText( "" );
				return;
			}
			double Angle = GetAngle( pConnector1->GetOriginalPoint(), pConnector0->GetOriginalPoint(), pConnector2->GetOriginalPoint() );
			if( Angle > 180 )
				Angle = 360 - Angle;
			Text.Format( "%.4lf", Angle );
			break;
		}
	}

	pEditBox->SetEditText( Text );
}

void CLinkageView::OnViewCoordinates()
{
	CMFCRibbonBar *pRibbon = ((CFrameWndEx*) AfxGetMainWnd())->GetRibbonBar();
	if( pRibbon == 0 )
		return;
	CMFCRibbonEdit * pEditBox = DYNAMIC_DOWNCAST( CMFCRibbonEdit, pRibbon->FindByID( ID_VIEW_COORDINATES ) );
	if( pEditBox == 0 )
		return;

	CString Text = pEditBox->GetEditText();
	if( Text.GetLength() == 0 )
		return;

	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if( pDoc->IsSelectionMeshableGears() )
	{
		double Size1 = 0;
		double Size2 = 0;
		int Count = sscanf_s( (const char*)Text, "%lf:%lf", &Size1, &Size2 );
		if( Count < 2 )
			return;

		CLink *pLink1 = pDoc->GetSelectedLink( 0, false );
		CLink *pLink2 = pDoc->GetSelectedLink( 1, false );

		CGearConnection *pConnection = pDoc->GetGearRatio( pLink1, pLink2 );

		if( !pDoc->SetGearRatio( pLink1, pLink2, Size1, Size2, pConnection == 0 ? false : pConnection->m_bUseSizeAsRadius, pConnection == 0 ? CGearConnection::GEARS : pConnection->m_ConnectionType ) )
			return;
	}
	else
	{
		if( !pDoc->SetSelectedElementCoordinates( Text ) )
			return;
	}

	ShowSelectedElementCoordinates();

	m_SelectionContainerRect = GetDocumentArea( false, true );
	m_SelectionAdjustmentRect = GetDocumentAdjustArea( true );

	UpdateForDocumentChange();
}

void CLinkageView::OnUpdateViewCoordinates( CCmdUI *pCmdUI )
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	int SelectedConnectors = pDoc->GetSelectedConnectorCount();
	int SelectedLinks = pDoc->GetSelectedLinkCount( true );

	//pCmdUI->Enable( !m_bSimulating && ( ( SelectedConnectors == 1 || SelectedConnectors == 2 || SelectedConnectors == 3 || pDoc->IsSelectionMeshableGears() ) ? 1 : 0 ) && m_bAllowEdit );
	pCmdUI->Enable( !m_bSimulating && m_bAllowEdit && ( SelectedConnectors + SelectedLinks > 0 || pDoc->IsSelectionMeshableGears() ) ? 1 : 0 );
}

static bool GetEncoderClsid(WCHAR *format, CLSID *pClsid)
{
	unsigned int num = 0,  size = 0;
	GetImageEncodersSize(&num, &size);
	if( size == 0 )
		return false;
	ImageCodecInfo *pImageCodecInfo = (ImageCodecInfo *)(malloc(size));
	if( pImageCodecInfo == NULL )
		return false;
	GetImageEncoders(num, size, pImageCodecInfo);
	for( unsigned int Index = 0; Index < num; ++Index )
	{
		if(wcscmp(pImageCodecInfo[Index].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[Index].Clsid;
			free(pImageCodecInfo);
			return true;
		}
	}
	free(pImageCodecInfo);
	return false;
}

bool CLinkageView::DisplayAsImage( CDC *pOutputDC, int xOut, int yOut, int OutWidth, int OutHeight, int RenderWidth, int RenderHeight, double ResolutionAdjustmentFactor, double MarginScale, COLORREF BackgroundColor, bool bAddBorder )
{
	CRenderer Renderer( CRenderer::WINDOWS_GDI );
	CBitmap MemoryBitmap;

	int Width = (int)( 1920 * ResolutionAdjustmentFactor );
	double Temp = 1920 * RenderHeight / RenderWidth;
	int Height = (int)( Temp * ResolutionAdjustmentFactor );

	CRect ImageRect( 0, 0, Width, Height );

	CDC *pDC = GetDC();

	double ScaleFactor = 4.0;

	double SaveDPIScale = m_DPIScale;
	m_DPIScale = 1.0;

	PrepareRenderer( Renderer, &ImageRect, &MemoryBitmap, pDC, ScaleFactor, true, MarginScale, 1.0, false, true, false, 0 );

	DoDraw( &Renderer );

	m_DPIScale = SaveDPIScale;

	CDC ShrunkDC;
	CBitmap ShrunkBitmap;

	ShrunkDC.CreateCompatibleDC( pDC );
	ShrunkBitmap.CreateCompatibleBitmap( pDC, RenderWidth, RenderHeight );
	ShrunkDC.SelectObject( &ShrunkBitmap );
	ShrunkDC.PatBlt( 0, 0, RenderWidth, RenderHeight, WHITENESS );

	StretchBltPlus( ShrunkDC.GetSafeHdc(),
		            0, 0, RenderWidth, RenderHeight,
		            Renderer.GetSafeHdc(),
					0, 0, (int)( Width * ScaleFactor ), (int)( Height * ScaleFactor ),
					SRCCOPY );

	double WidthScale = (double)OutWidth / (double)RenderWidth;
	double HeightScale = (double)OutHeight / (double)RenderHeight;
	double Scale = min( WidthScale, HeightScale );
	if( Scale > 1.0 )
		Scale = 1.0;
	int FinalWidth = (int)( RenderWidth * Scale );
	int FinalHeight = (int)( RenderHeight * Scale );
	int xOffset = ( OutWidth - FinalWidth ) / 2;
	int yOffset = ( OutHeight - FinalHeight ) / 2;

	CBrush BackgroundBrush( BackgroundColor );
	CRect AllRect( xOut, yOut, xOut + OutWidth, yOut + OutHeight );
	pOutputDC->FillRect( &AllRect, &BackgroundBrush );

	StretchBltPlus( pOutputDC->GetSafeHdc(),
		            xOut + xOffset, yOut + yOffset, FinalWidth, FinalHeight,
		            ShrunkDC.GetSafeHdc(),
					0, 0, RenderWidth, RenderHeight,
					SRCCOPY );

	if( bAddBorder )
	{
		CBrush Brush( RGB( 0, 0, 0 ) );
		CRect Rect( xOut + xOffset, yOut + yOffset, xOut + xOffset + FinalWidth, yOut + yOffset + FinalHeight );
		pOutputDC->FrameRect( &Rect, &Brush );
	}

	return true;
}

bool CLinkageView::SaveAsImage( const char *pFileName, int RenderWidth, int RenderHeight, double ResolutionAdjustmentFactor, double MarginScale )
{
	CString SearchableFileName = pFileName;
	SearchableFileName.MakeUpper();
	bool bJPEG = false;
	if( SearchableFileName.Find( ".JPG" ) == SearchableFileName.GetLength() - 4 )
		bJPEG = true;
	else if( SearchableFileName.Find( ".PNG" )  != SearchableFileName.GetLength() - 4 )
	{
		AfxMessageBox( "Images can be saved as either JPEG or PNG files. Please enter or pick a file name with a .jpg or .png file extension." );
		return false;
	}

	CRenderer Renderer( CRenderer::WINDOWS_GDI );
	CBitmap MemoryBitmap;

	int Width = (int)( 1920 * ResolutionAdjustmentFactor );
	double Temp = 1920 * RenderHeight / RenderWidth;
	int Height = (int)( Temp * ResolutionAdjustmentFactor );

	CRect ImageRect( 0, 0, Width, Height );

	CDC *pDC = GetDC();

	double ScaleFactor = 4.0;

	PrepareRenderer( Renderer, &ImageRect, &MemoryBitmap, pDC, ScaleFactor, true, MarginScale, 1.0, false, true, false, 0 );

	DoDraw( &Renderer );

	CDC ShrunkDC;
	CBitmap ShrunkBitmap;

	int FinalWidth = RenderWidth;
	int FinalHeight = RenderHeight;

	ShrunkDC.CreateCompatibleDC( pDC );
	ShrunkBitmap.CreateCompatibleBitmap( pDC, FinalWidth, FinalHeight );
	ShrunkDC.SelectObject( &ShrunkBitmap );
	ShrunkDC.PatBlt( 0, 0, FinalWidth, FinalHeight, WHITENESS );

	StretchBltPlus( ShrunkDC.GetSafeHdc(),
		            0, 0, FinalWidth, FinalHeight,
		            Renderer.GetSafeHdc(),
					0, 0, (int)( Width * ScaleFactor ), (int)( Height * ScaleFactor ),
					SRCCOPY );

	Gdiplus::Bitmap *pSaveBitmap = ::new Gdiplus::Bitmap( (HBITMAP)ShrunkBitmap.GetSafeHandle(), (HPALETTE)0 );
	EncoderParameters JPEGParams;
	JPEGParams.Count = 1;
	JPEGParams.Parameter[0].NumberOfValues = 1;
	JPEGParams.Parameter[0].Guid  = EncoderQuality;
	JPEGParams.Parameter[0].Type  = EncoderParameterValueTypeLong;
	ULONG Quality = 85;
	JPEGParams.Parameter[0].Value = &Quality;

	CLSID imageCLSID;
	if( !GetEncoderClsid( bJPEG ? L"image/png" : L"image/png", &imageCLSID ) )
	{
		AfxMessageBox( "There is an internal error with the encoder for your image file." );
		return false;
	}

	wchar_t *wFileName = ToUnicode( pFileName );
	bool bResult = pSaveBitmap->Save( wFileName, &imageCLSID, bJPEG ? &JPEGParams : 0 ) == Gdiplus::Ok;

	ReleaseDC( pDC );

	::delete pSaveBitmap;

	InvalidateRect( 0 );

	return bResult;
}

bool CLinkageView::SaveAsDXF( const char *pFileName )
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	double UnitScale = pDoc->GetUnitScale();
	CFRect DocumentRect;

	DocumentRect = GetDocumentArea();

	double ConnectorSize = max( fabs( DocumentRect.Height() ), fabs( DocumentRect.Width() ) ) / 750;
	ConnectorSize *= pDoc->GetUnitScale();

	CRenderer Renderer( CRenderer::DXF_FILE );

	int Width = (int)( DocumentRect.Width() * pDoc->GetUnitScale() ) + 2;
	int Height = (int)( DocumentRect.Height() * pDoc->GetUnitScale() ) + 2;

	CRect ImageRect( 0, 0, Width, Height );

	PrepareRenderer( Renderer, &ImageRect, 0, 0, 1.0, false, 0.0, ConnectorSize, false, false, false, 0 );

	m_Zoom = pDoc->GetUnitScale();
	DoDraw( &Renderer );

	Renderer.SaveDXF( pFileName );

	InvalidateRect( 0 );

	return true;
}

void CLinkageView::OnSelectSample (UINT ID )
{
	CLinkageDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CSampleGallery GalleryData;

	int Counter;
	CString ExampleData;
	CString ExampleName;
	int Count = GalleryData.GetCount();
	for( Counter = 0; Counter < Count; ++Counter )
	{
		if( ID == GalleryData.GetCommandID( Counter ) )
		{
			if( m_bSimulating )
				StopSimulation();
			pDoc->SelectSample( Counter );
			break;
		}
	}
}

void CLinkageView::UpdateForDocumentChange( void )
{
	SetScrollExtents();
	MarkSelection( true );
	ShowSelectedElementCoordinates();
	InvalidateRect( 0 );
}

#if 0

LRESULT CLinkageView::OnGesture(WPARAM wParam, LPARAM lParam)
{
    // Create a structure to populate and retrieve the extra message info.
    GESTUREINFO gi;

    ZeroMemory(&gi, sizeof(GESTUREINFO));

    gi.cbSize = sizeof(GESTUREINFO);

    BOOL bResult  = GetGestureInfo((HGESTUREINFO)lParam, &gi);
    BOOL bHandled = FALSE;

    if (bResult){
        // now interpret the gesture
        switch (gi.dwID){
           case GID_ZOOM:
               // Code for zooming goes here
               bHandled = TRUE;
               break;
           case GID_PAN:
               // Code for panning goes here
               bHandled = TRUE;
               break;
           case GID_ROTATE:
               // Code for rotation goes here
               bHandled = TRUE;
               break;
           case GID_TWOFINGERTAP:
               // Code for two-finger tap goes here
               bHandled = TRUE;
               break;
           case GID_PRESSANDTAP:
               // Code for roll over goes here
               bHandled = TRUE;
               break;
           default:
               // A gesture was not recognized
               break;
        }
    }else{
        DWORD dwErr = GetLastError();
        if (dwErr > 0){
            //MessageBoxW(hWnd, L"Error!", L"Could not retrieve a GESTUREINFO structure.", MB_OK);
        }
    }
    if (bHandled){
        return 0;
    }else{
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
  }
}

#endif
