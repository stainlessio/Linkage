// LinkageView.h : interface of the CLinkageView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_LinkageVIEW_H__E3F43B66_810B_40E4_B14B_ECABF59CFB06__INCLUDED_)
#define AFX_LinkageVIEW_H__E3F43B66_810B_40E4_B14B_ECABF59CFB06__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "mmsystem.h"
#include "connector.h"
#include "link.h"
#include "ControlWindow.h"
#include "PopupGallery.h"
#include "Renderer.h"
#include "AviFile.h"
#include "Simulator.h"

class COleDropWndTarget : public COleDropTarget
{
// Construction
public:
    COleDropWndTarget();

// Implementation
public:
    virtual ~COleDropWndTarget();   
    
   //
   // These members MUST be overridden for an OLE drop target
   // See DRAG and DROP section of OLE classes reference
   //
   DROPEFFECT OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject, DWORD 
                                                dwKeyState, CPoint point );
   DROPEFFECT OnDragOver(CWnd* pWnd, COleDataObject* pDataObject, DWORD 
                                               dwKeyState, CPoint point );
   void OnDragLeave(CWnd* pWnd);
   
   BOOL OnDrop(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT 
                                          dropEffect, CPoint point );
};

class CLinkageView : public CView
{
protected: // create from serialization only
	CLinkageView();
	DECLARE_DYNCREATE(CLinkageView)

// Attributes
public:
	class CLinkageDoc* GetDocument();

// Operations
public:

    COleDropWndTarget m_OleWndDropTarget;  

	CAviFile *m_pAvi;
	bool m_bRecordingVideo;
	int m_RecordQuality;

	bool InitializeVideoFile( const char *pVideoFile, const char *pCompressorString );
	void SaveVideoFrame( CRenderer *pRenderer, CRect &CaptureRect );
	bool SaveAsImage( const char *pFileName, int RenderWidth, int RenderHeight, double ResolutionAdjustmentFactor, double MarginFactor );
	bool DisplayAsImage( CDC *pOutputDC, int xOut, int yOut, int OutWidth, int OutHeight, int RenderWidth, int RenderHeight, double ResolutionAdjustmentFactor, double MarginFactor, COLORREF BackgroundColor, bool bAddBorder );
	bool SaveAsDXF( const char *pFileName );
	void CloseVideoFile( void );

	void OnDraw( CDC* pDC, CPrintInfo *pPrintInfo );

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLinkageView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual void OnPrint(CDC* pDC, CPrintInfo* pPrintInfo);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

	CFArea DoDraw( CRenderer* pRenderer );
	void DrawGrid( CRenderer* pRenderer );
	CFArea DrawMechanism( CRenderer* pRenderer );
	CFArea DrawPartsList( CRenderer* pRenderer );
	CFRect CLinkageView::GetDocumentArea( bool bWithDimensions = false, bool bSelectedOnly = false );
	CFRect CLinkageView::GetDocumentAdjustArea( bool bSelectedOnly = false );

	double CalculateDefaultGrid( void );

	void VideoThreadFunction( void );

// Implementation
public:
	virtual ~CLinkageView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	void StopSimulation( void );
	
	void SetZoom( double Zoom );
	void SetOffset( CFPoint Offset );
	CFPoint GetOffset( void ) { return m_ScreenScrollPosition; }
	double GetUnscale( void ) { return m_ScreenZoom; }

	void HighlightSelected( bool bHighlight ) { m_bSuperHighlight = bHighlight; InvalidateRect( 0 ); }

	enum _SimulationControl { AUTO, GLOBAL, INDIVIDUAL, STEP };

private:
	typedef enum _AdjustmentType { ADJUSTMENT_NONE, ADJUSTMENT_ROTATE, ADJUSTMENT_STRETCH } AdjustmentType;
	typedef enum _AdjustmentControl { AC_ROTATE, AC_TOP_LEFT, AC_TOP, AC_TOP_RIGHT, AC_RIGHT, AC_BOTTOM_RIGHT, AC_BOTTOM, AC_BOTTOM_LEFT, AC_LEFT } AdjustmentControl;
	typedef enum _MouseAction { ACTION_NONE = 0, ACTION_SELECT, ACTION_DRAG, ACTION_ROTATE, ACTION_PAN, ACTION_RECENTER, ACTION_STRETCH } MouseAction;

	CSimulator m_Simulator;

	ULONG_PTR m_gdiplusToken;
	CPoint m_PreviousDragPoint;
	bool m_bSimulating;
	int m_SimulationSteps;
	_SimulationControl m_SimulationControl;
	CFont m_SmallFont;
	CFont m_MediumFont;
	CFont *m_pUsingFont;
	int m_UsingFontHeight;
	MMRESULT m_TimerID;
	bool m_bShowLabels;
	bool m_bShowAngles;
	bool m_bSnapOn;
	bool m_bGridSnap;
	bool m_bAutoJoin;
	bool m_bShowDimensions;
	bool m_bNewLinksSolid;
	double m_xGrid;
	double m_yGrid;
	bool m_bShowData;
	bool m_bShowDebug;
	bool m_bShowBold;
	int m_BoldMultiplier;
	bool m_bShowAnicrop;
	bool m_bShowLargeFont;
	bool m_bShowGrid;
	bool m_bShowParts;
	double m_Zoom;
	double m_ScreenZoom;
	CFPoint m_ScrollPosition;
	CFPoint m_ScreenScrollPosition;
	CFPoint m_DragStartPoint;
	bool m_bSelectModeChange;
	bool m_bChangeAdjusters;
	CFPoint m_DragOffset;
	bool m_bMouseMovedEnough;
	CRect m_SelectRect;
	bool m_bSuperHighlight;
	CControlWindow m_ControlWindow;
	AdjustmentType m_VisibleAdjustment;
	MouseAction m_MouseAction;
	HICON m_Rotate0;
	HICON m_Rotate1;
	HICON m_Rotate2;
	HICON m_Rotate3;
	HICON m_Rotate4;
	bool m_bWasValid; // Simulation was valid (for purpose of error reporting).
	CPoint m_ScrollOffsetAdjustment;
	CFRect m_SelectionContainerRect;
	CFRect m_SelectionAdjustmentRect;
	CFPoint m_SelectionRotatePoint;
	CFPoint GetDocumentViewCenter( void );
	AdjustmentControl m_StretchingControl;
	CPopupGallery * m_pPopupGallery;
	CPoint m_PopupPoint;
	CRect m_DrawingRect;
	int m_YUpDirection;
	unsigned int m_SelectedEditLayers;
	bool m_bAllowEdit;

	bool m_bPrintFullSize;

	double m_BaseUnscaledUnit;
	double m_ConnectorRadius;
	double m_ConnectorTriangle;
	double m_ArrowBase;

	bool m_bRequestAbort;
	bool m_bProcessingVideoThread;
	CDIBSection *m_pVideoBitmapQueue; // a queue of 1 item!!!!
	HANDLE m_hVideoFrameEvent;
	int m_VideoStartFrames;
	int m_VideoRecordFrames;
	int m_VideoEndFrames;
	bool m_bUseVideoCounters;
	bool m_bShowSelection;
	
	void MarkSelection( bool bSelectionChanged );
	CPoint GetDefaultUnscalePoint( void );
	
	double UnscaledUnits( double Count );

	void OnMouseMoveSelect(UINT nFlags, CPoint point);
	void OnMouseMoveDrag(UINT nFlags, CPoint point);
	void OnMouseMovePan(UINT nFlags, CPoint point);
	void OnMouseMoveRotate(UINT nFlags, CPoint point);
	void OnMouseMoveRecenter(UINT nFlags, CPoint point);
	void OnMouseMoveStretch(UINT nFlags, CPoint point,  bool bEndStretch);

	void OnMouseEndSelect(UINT nFlags, CPoint point);
	void OnMouseEndDrag(UINT nFlags, CPoint point);
	void OnMouseEndPan(UINT nFlags, CPoint point);
	void OnMouseEndRotate(UINT nFlags, CPoint point);
	void OnMouseEndRecenter(UINT nFlags, CPoint point);
	void OnMouseEndStretch(UINT nFlags, CPoint point);
	void OnMouseEndChangeAdjusters(UINT nFlags, CPoint point);
	
	void OnButtonUp(UINT nFlags, CPoint point);
	double ConvertToSeconds( const char *pHMSString );

	void ShowSelectedElementCoordinates( void );

	CRect PrepareRenderer( CRenderer &Renderer, CRect *pDrawRect, CBitmap *pBitmap, CDC *pDC, double ForceScaling, bool bScaleToFit, double MarginScale, double UnscaledUnitSize, bool bForScreen, bool bAntiAlias, bool bActualSize, int PageNumber );
	int GetPrintPageCount( CDC *pDC, CPrintInfo *pPrintInfo, bool bPrintActualSize );
	void DrawAdjustmentControls( CRenderer *pRenderer );
	void GetAdjustmentControlRect( AdjustmentControl Control, CFRect &Rect );
	void GetAdjustmentControlRect( AdjustmentControl Control, CRect &Rect );
		
	bool SelectVideoBox( UINT nFlags, CPoint Point );
	bool SelectAdjustmentControl( UINT nFlags, CPoint Point );
	bool SelectDocumentItem( UINT nFlags, CPoint Point );
	bool DragSelectionBox( UINT nFlags, CPoint point );
	bool FindDocumentItem( CPoint Point, CLink *&pFoundLink, CConnector *&pFoundConnector );

	void SetLocationAsStatus( CFPoint &Point );
	
	void DebugDrawConnector( CRenderer* pRenderer, unsigned int OnLayers, CConnector *pConnector, bool bShowlabels, bool bErase = false, bool bHighlight = false, bool bDrawConnectedLinks = false );
	void DrawConnector( CRenderer* pRenderer, unsigned int OnLayers, CConnector *pConnector, bool bShowlabels, bool bErase = false, bool bHighlight = false, bool bDrawConnectedLinks = false, bool bControlKnob = false );
	void DrawMotionPath( CRenderer* pRenderer, CConnector* pConnector );
	void DebugDrawLink( CRenderer* pRenderer, unsigned int OnLayers, CLink *pLink, bool bShowlabels, bool bDrawHighlight = false, bool bDrawFill = false );
	void DrawLink( CRenderer* pRenderer, const GearConnectionList *pGearConnections, unsigned int OnLayers, CLink *pLink, bool bShowlabels, bool bDrawHighlight = false, bool bDrawFill = false );
	void DrawDebugItems( CRenderer *pRenderer );
	void ClearDebugItems( void );
	void DrawChain( CRenderer* pRenderer, unsigned int OnLayers, CGearConnection *pGearConnection );
	void DrawFailureMarks( CRenderer* pRenderer, unsigned int OnLayers, CFPoint Point, double Radius );
	CFArea DrawMeasurementLine( CRenderer* pRenderer, CFLine &InputLine, CFPoint &Firstpoint, CFPoint &SecondPoint, double Offset, bool bDrawLines, bool bDrawText );
	CFLine CreateMeasurementEndLine( CFLine &InputLine, CFPoint &MeasurementPoint, CFPoint &MeasurementLinePoint, int InputLinePointIndex, double Offset );
	void DrawMeasurementLine( CRenderer* pRenderer, CFLine &InputLine, double Offset, bool bDrawLines, bool bDrawText );
	CFArea DrawDimensions( CRenderer* pRenderer, const GearConnectionList *pGearConnections, unsigned int OnLayers, CLink *pLink, bool bDrawLines, bool bDrawText );
	CFArea DrawDimensions( CRenderer* pRenderer, unsigned int OnLayers, CConnector *pConnector, bool bDrawLines, bool bDrawText );
	CFArea DrawConnectorLinkDimensions( CRenderer* pRenderer, const GearConnectionList *pGearConnections, unsigned int OnLayers, CLink *pLink, bool bDrawLines, bool bDrawText );
	CFArea DrawGroundDimensions( CRenderer* pRenderer, CLinkageDoc *pDoc, unsigned int OnLayers, bool bDrawLines, bool bDrawText );
	CFArea DrawCirclesRadius( CRenderer *pRenderer, CFPoint Center, std::list<double> &RadiusList, bool bDrawLines, bool bDrawText );
	void DrawStackedConnectors( CRenderer* pRenderer, unsigned int OnLayers );
	void DrawActuator( CRenderer* pRenderer, unsigned int OnLayers, CLink *pLink, bool bDrawFill = false );
	void DrawFasteners( CRenderer* pRenderer, unsigned int OnLayers, CElement *pElement );
	void DrawSliderTrack( CRenderer* pRenderer, unsigned int OnLayers, CConnector* pConnector );
	void DrawSnapLines( CRenderer *pRenderer );
	void DrawAlignmentLines( CRenderer *pRenderer );
	void DrawData( CRenderer *pRenderer );
	void DrawAnicrop( CRenderer *pRenderer );
	void DrawRuler( CRenderer *pRenderer );
	double AdjustYCoordinate( double y );
	void MovePartsLinkToOrigin( CLink *pPartsLink, CFPoint Origin, GearConnectionList *pGearConnections );
	class CTempLink* CLinkageView::GetTemporaryPartsLink( CLink *pLink, CFPoint PartOrigin, GearConnectionList *pGearConnections );
	
	bool StartVideoThread( void );

	void SetScrollExtents( bool bEnableBars = true );
	void OnScroll( int WhichBar, UINT nSBCode, UINT nPos );
	void OnScroll( int xPos, int yPos, bool Tacking );

	CRect GetDocumentScrollingRect( void );

	bool ConnectorProperties( CConnector *pConnector );
	bool LinkProperties( CLink *pLink );
	bool PointProperties( CConnector *pConnector );
	bool LineProperties( CLink *pLink );
	
	void SetupFont( void );

	void OnEndPrintPreview( CDC* pDC, CPrintInfo* pInfo, POINT point, CPreviewView* pView );

	void StepSimulation( enum _SimulationControl SimulationControl );
	void StartMechanismSimulate( enum _SimulationControl SimulationControl );
	void StopMechanismSimulate( bool KeepCurrentPositions = false );

	void ConfigureControlWindow( enum _SimulationControl SimulationControl );

	void InsertPoint( CFPoint *pPoint );
	void InsertLine( CFPoint *pPoint );
	void InsertMeasurement( CFPoint *pPoint );
	void InsertConnector( CFPoint *pPoint );
	void InsertAnchor( CFPoint *pPoint );
	void InsertAnchorLink( CFPoint *pPoint );
	void InsertRotatinganchor( CFPoint *pPoint );
	void InsertLink( CFPoint *pPoint );
	void InsertDouble( CFPoint *pPoint );
	void InsertTriple( CFPoint *pPoint );
	void InsertQuad( CFPoint *pPoint );
	void InsertlinkSlider( CFPoint *pPoint );
	void InsertActuator( CFPoint *pPoint );
	void InsertSliderCombo( CFPoint *pPoint );
	void InsertGear( CFPoint *pPoint );

	void Scale( double &x, double &y );
	void Unscale( double &x, double &y );
	CFPoint Unscale( CFPoint Point );
	CFPoint Scale( CFPoint Point );
	CFRect Unscale( CFRect Rect );
	CFRect Scale( CFRect Rect );
	CFCircle Unscale( CFCircle Circle );
	CFCircle Scale( CFCircle Circle );
	CFLine Unscale( CFLine Line );
	CFLine Scale( CFLine Line );
	double Unscale( double Distance );
	double Scale( double Distance );
	CFArc Unscale( CFArc TheArc );
	CFArc Scale( CFArc TheArc );

	bool CLinkageView::AnyAlwaysManual( void );

	void EditProperties( CConnector *pConnector, CLink *pLink, bool bSelectedLinks );

	void GetDocumentViewArea( CFRect &Rect, CLinkageDoc *pDoc );

	void SetPrinterOrientation( void );

	void UpdateForDocumentChange( void );

// Generated message map functions
protected:
	//{{AFX_MSG(CLinkageView)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMechanismReset();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSelectSample( UINT nID );
	afx_msg void OnMechanismSimulate();
	afx_msg void OnUpdateAlignButton(CCmdUI *pCmdUI);
	afx_msg void OnUpdateMechanismSimulate(CCmdUI *pCmdUI);
	afx_msg void OnUpdateMechanismQuicksim(CCmdUI *pCmdUI);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnUpdateMechanismReset(CCmdUI *pCmdUI);
	afx_msg void OnUpdateNotSimulating(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEdit(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditSelected(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditConnect(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditFasten(CCmdUI *pCmdUI);
	afx_msg void OnEditFasten();
	afx_msg void OnUpdateEditUnfasten(CCmdUI *pCmdUI);
	afx_msg void OnEditUnfasten();
	afx_msg void OnUpdateEditSetRatio(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditJoin(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditLock(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditCombine(CCmdUI *pCmdUI);
	afx_msg void OnEditCombine();
	afx_msg void OnEditConnect();
	afx_msg void OnEditSetRatio();
	// afx_msg void OnUpdateEditDeleteselected(CCmdUI *pCmdUI);
	afx_msg void OnEditDeleteselected();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPrintFull();
	afx_msg void OnUpdatePrintFull( CCmdUI *pCmdUI );
	
	afx_msg void OnUpdateButtonRun(CCmdUI *pCmdUI);
	afx_msg void OnUpdateButtonStop(CCmdUI *pCmdUI);
	afx_msg void OnButtonRun();
	afx_msg void OnButtonStop();
	afx_msg void OnButtonPin();
	afx_msg void OnUpdateButtonPin(CCmdUI *pCmdUI);

	afx_msg void OnInsertConnector();
	afx_msg void OnInsertAnchor();
	afx_msg void OnInsertAnchorLink();
	afx_msg void OnInsertRotatinganchor();
	afx_msg void OnInsertLink();
	afx_msg void OnInsertDouble();
	afx_msg void OnInsertTriple();
	afx_msg void OnInsertQuad();
	afx_msg void OnInsertlinkSlider();
	afx_msg void OnInsertActuator();
	afx_msg void OnInsertPoint();
	afx_msg void OnInsertLine();
	afx_msg void OnInsertMeasurement();
	afx_msg void OnInsertGear();

	afx_msg void OnUpdateEditProperties(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditSplit(CCmdUI *pCmdUI);
	afx_msg void OnEditProperties();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnEditJoin();
	afx_msg void OnEditLock();
	afx_msg void OnEditSelectall();
	afx_msg void OnUpdateSelectall( CCmdUI *pCmdUI );


	afx_msg void OnAlignHorizontal();
	afx_msg void OnUpdateAlignHorizontal( CCmdUI *pCmdUI );
	afx_msg void OnAlignVertical();
	afx_msg void OnUpdateAlignVertical( CCmdUI *pCmdUI );
	afx_msg void OnAlignLineup();
	afx_msg void OnUpdateAlignLineup( CCmdUI *pCmdUI );

	afx_msg void OnAlignEvenSpace();
	afx_msg void OnUpdateAlignEvenSpace( CCmdUI *pCmdUI );

	afx_msg void OnFlipHorizontal();
	afx_msg void OnUpdateFlipHorizontal( CCmdUI *pCmdUI );
	afx_msg void OnFlipVertical();
	afx_msg void OnUpdateFlipVertical( CCmdUI *pCmdUI );

	afx_msg void OnAddConnector();
	afx_msg void OnUpdateAddConnector( CCmdUI *pCmdUI );

	afx_msg void OnAlignSetAngle();
	afx_msg void OnUpdateAlignSetAngle( CCmdUI *pCmdUI );
	afx_msg void OnAlignRightAngle();
	afx_msg void OnUpdateAlignRightAngle( CCmdUI *pCmdUI );
	afx_msg void OnAlignRectangle();
	afx_msg void OnUpdateAlignRectangle( CCmdUI *pCmdUI );
	afx_msg void OnAlignParallelogram();
	afx_msg void OnUpdateAlignParallelogram( CCmdUI *pCmdUI );
	afx_msg void OnViewUnits();
	afx_msg void OnViewCoordinates();
	afx_msg void OnUpdateViewUnits( CCmdUI *pCmdUI );
	afx_msg void OnUpdateViewCoordinates( CCmdUI *pCmdUI );
	afx_msg void OnEditSplit();
	afx_msg void OnViewLabels();
	afx_msg void OnViewAngles();
	afx_msg void OnViewData();
	afx_msg void OnUpdateViewAnicrop(CCmdUI *pCmdUI);
	afx_msg void OnViewAnicrop();
	afx_msg void OnUpdateViewParts(CCmdUI *pCmdUI);
	afx_msg void OnViewParts();
	afx_msg void OnUpdateViewGrid(CCmdUI *pCmdUI);
	afx_msg void OnViewGrid();
	afx_msg void OnUpdateViewLargeFont(CCmdUI *pCmdUI);
	afx_msg void OnViewLargeFont();
	afx_msg void OnViewDebug();
	afx_msg void OnViewBold();
	afx_msg void OnEditSnap();
	afx_msg void OnUpdateEditSnap(CCmdUI *pCmdUI);
	afx_msg void OnEditGridSnap();
	afx_msg void OnEditAutoJoin();
	afx_msg void OnUpdateEditGridSnap(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditAutoJoin(CCmdUI *pCmdUI);
	afx_msg void OnViewDimensions();
	afx_msg void OnUpdateViewDimensions(CCmdUI *pCmdUI);
	afx_msg void OnViewSolidLinks();
	afx_msg void OnUpdateViewSolidLinks(CCmdUI *pCmdUI);
	afx_msg void OnViewDrawing();
	afx_msg void OnUpdateViewDrawing(CCmdUI *pCmdUI);
	afx_msg void OnViewMechanism();
	afx_msg void OnUpdateViewMechanism(CCmdUI *pCmdUI);
	afx_msg void OnEditmakeAnchor();
	afx_msg void OnUpdateEditmakeAnchor(CCmdUI *pCmdUI);

	afx_msg void OnEditDrawing();
	afx_msg void OnUpdateEditDrawing(CCmdUI *pCmdUI);
	afx_msg void OnEditMechanism();
	afx_msg void OnUpdateEditMechanism(CCmdUI *pCmdUI);

	afx_msg void OnUpdateViewLabels(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewAngles(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewData(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewDebug(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewBold(CCmdUI *pCmdUI);
	afx_msg void OnEditCut();
	afx_msg void OnEditCopy();
	afx_msg void OnEditPaste();
	afx_msg void OnSelectNext();
	afx_msg void OnUpdateEditPaste(CCmdUI *pCmdUI);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnViewUnscalein();
	afx_msg void OnViewUnscaleout();
	afx_msg void OnMenuUnscalefit();

	afx_msg void OnSimulatePause();
	afx_msg void OnSimulateForward();
	afx_msg void OnSimulateBackward();
	afx_msg void OnSimulateForwardBig();
	afx_msg void OnSimulateBackwardBig();
	afx_msg void OnUpdateSimulatePause(CCmdUI *pCmdUI);
	afx_msg void OnUpdateSimulateForwardBackward(CCmdUI *pCmdUI);

protected:
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
public:

	void OnFilePrintSetup();
	afx_msg void OnMechanismQuicksim();
	afx_msg void OnEditMakeparallelogram();
	afx_msg void OnUpdateEditMakeparallelogram(CCmdUI *pCmdUI);
	//afx_msg void OnEditMakerighttriangle();
	//afx_msg void OnUpdateEditMakerighttriangle(CCmdUI *pCmdUI);
	//afx_msg void OnEditMakeParallelogram();
	//afx_msg void OnUpdateEditMakeParallelogram(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditUndo(CCmdUI *pCmdUI);
	afx_msg void OnEditUndo();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnEditSlide();
	afx_msg void OnUpdateEditSlide(CCmdUI *pCmdUI);
	afx_msg void OnFilePrintPreview();
	afx_msg void OnFilePrint();
	afx_msg void OnFileSaveVideo();
	afx_msg void OnFileSaveImage();
	afx_msg void OnFileSaveDXF();
	afx_msg void OnFileSaveMotion();
	afx_msg void OnUpdateFileSaveMotion(CCmdUI *pCmdUI);
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
	afx_msg void OnSimulateInteractive();
	afx_msg void OnUpdateSimulateInteractive(CCmdUI *pCmdUI);
	afx_msg void OnSimulateManual();
	afx_msg void OnUpdateSimulateManual(CCmdUI *pCmdUI);
	afx_msg void OnPopupGallery();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnControlWindow();
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

#ifndef _DEBUG  // debug version in LinkageView.cpp
inline CLinkageDoc* CLinkageView::GetDocument()
   { return (CLinkageDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LinkageVIEW_H__E3F43B66_810B_40E4_B14B_ECABF59CFB06__INCLUDED_)
