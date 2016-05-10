// LinkageDoc.cpp : implementation of the CLinkageDoc class
//

#include "stdafx.h"
#include "Linkage.h"
#include "afxcoll.h"
#include "geometry.h"
#include "link.h"
#include "connector.h"
#include "LinkageDoc.h"
#include "Linkageview.h"
#include "quickxml.h"
#include "examples_xml.h"
#include "bitarray.h"
#include "SampleGallery.h"
#include "DebugItem.h"
#include "colors.h"
#include <algorithm>
#include <vector>

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const char *CRLF = "\r\n";

static const int DEFAULT_RPM = 15;

static const int INTERMEDIATE_STEPS = 2;

/////////////////////////////////////////////////////////////////////////////
// CLinkageDoc

IMPLEMENT_DYNCREATE(CLinkageDoc, CDocument)

BEGIN_MESSAGE_MAP(CLinkageDoc, CDocument)
	//{{AFX_MSG_MAP(CLinkageDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	//ON_COMMAND_RANGE( ID_SAMPLE_SIMPLE, ID_SAMPLE_UNUSED19, OnSelectSample )

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLinkageDoc construction/destruction

CString CLinkageDoc::GetUnitsString( CLinkageDoc::_Units Units )
{
	switch( Units )
	{
		case CLinkageDoc::INCH: return CString( "Inches" ); break;
		case CLinkageDoc::MM:
		default: return CString( "Millimeters" ); break;
	}
}

CLinkageDoc::_Units CLinkageDoc::GetUnitsValue( const char *pUnits )
{
	CString Units = pUnits;
	Units.MakeUpper();
	if( Units == "MILLIMETERS" )
		return CLinkageDoc::MM;
	else if( Units == "INCHES" )
		return CLinkageDoc::INCH;
	else
		return CLinkageDoc::MM;
}

bool CLinkageDoc::IsEmpty( void )
{
	return m_Connectors.GetCount() == 0;
}

void CLinkageDoc::SetLinkConnector( CLink* pLink, CConnector* pConnector )
{
	if( pLink == 0 || pConnector == 0 )
		return;

	pLink->AddConnector( pConnector );
	pConnector->AddLink( pLink );
}

CLinkageDoc::CLinkageDoc()
{
	m_pCapturedConnector = 0;
	m_pCapturedController = 0;
	m_pUndoList = 0;
	m_pUndoListEnd = 0;
	m_UndoCount = 0;

	m_bSelectionMakeAnchor = false;
	m_bSelectionConnectable = false;
	m_bSelectionCombinable = false;
	m_bSelectionJoinable = false;
	m_bSelectionSlideable = false;
	m_bSelectionSplittable = false;
	m_bSelectionTriangle = false;
	m_bSelectionRectangle = false;
	m_bSelectionLineable = false;
	m_bSelectionFastenable = false;
	m_bSelectionUnfastenable = false;
	m_bSelectionLockable = false;
	m_AlignConnectorCount = 0;
	m_HighestConnectorID = -1;
	m_HighestLinkID = -1;
	m_UnitScaling = 1;
	m_SelectedLayers = 0;

	SetViewLayers( ALLLAYERS );
	SetEditLayers( ALLLAYERS );

	SetUnits( CLinkageDoc::MM );
	m_ScaleFactor = 1.0;
}

CLinkageDoc::~CLinkageDoc()
{
	DeleteContents();
}

BOOL CLinkageDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	m_pCapturedConnector = 0;
	m_SelectedConnectors.RemoveAll();
	m_SelectedLinks.RemoveAll();

	m_pCapturedConnector = 0;
	m_pCapturedController = 0;
	m_pUndoList = 0;
	m_pUndoListEnd = 0;
	m_UndoCount = 0;

	m_bSelectionMakeAnchor= false;
	m_bSelectionConnectable = false;
	m_bSelectionCombinable = false;
	m_bSelectionJoinable = false;
	m_bSelectionSlideable = false;
	m_bSelectionSplittable = false;
	m_bSelectionTriangle = false;
	m_bSelectionRectangle = false;
	m_bSelectionLineable = false;
	m_bSelectionFastenable = false;
	m_bSelectionUnfastenable = false;
	m_bSelectionLockable = false;
	m_AlignConnectorCount = 0;
	m_HighestConnectorID = -1;
	m_HighestLinkID = -1;

	CFrameWnd *pFrame = (CFrameWnd*)AfxGetMainWnd();
	CLinkageView *pView = pFrame == 0 ? 0 : (CLinkageView*)pFrame->GetActiveView();
	if( pView != 0 )
	{
		pView->SetOffset( CPoint( 0, 0 ) );
		pView->SetZoom( 1 );
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CLinkageDoc serialization

CConnector *CLinkageDoc::FindConnector( int ID )
{
	POSITION Position = m_Connectors.GetHeadPosition();
	while( Position != 0 )
	{
		CConnector *pConnector = m_Connectors.GetNext( Position );
		if( pConnector != 0 && pConnector->GetIdentifier() == ID )
			return pConnector;
	}
	return 0;
}

CLink *CLinkageDoc::FindLink( int ID )
{
	POSITION Position = m_Links.GetHeadPosition();
	while( Position != 0 )
	{
		CLink *pLink = m_Links.GetNext( Position );
		if( pLink != 0 && pLink->GetIdentifier() == ID )
			return pLink;
	}
	return 0;
}

static void AppendXMLAttribute( CString &String, const char *pName, const char *pValue, bool bIsSet = true )
{
	if( String.GetLength() < 2 || !bIsSet )
		return;

	if( String[String.GetLength()-1] == '>' )
		return;

	if( String[String.GetLength()-1] != ' ' )
		String += " ";

	String += pName;
	String += "=";
	String += "\"";
	String += pValue;
	String += "\"";
}

static void AppendXMLAttribute( CString &String, const char *pName, int Value, bool bIsSet = true )
{
	if( !bIsSet )
		return;
	CString TempString;
	TempString.Format( "%d", Value );
	AppendXMLAttribute( String, pName, (const char*)TempString, bIsSet );
}

static void AppendXMLAttribute( CString &String, const char *pName, unsigned int Value, bool bIsSet = true )
{
	if( !bIsSet )
		return;
	CString TempString;
	TempString.Format( "%u", Value );
	AppendXMLAttribute( String, pName, (const char*)TempString, bIsSet );
}

static void AppendXMLAttribute( CString &String, const char *pName, bool bValue, bool bIsSet = true )
{
	if( !bIsSet )
		return;
	AppendXMLAttribute( String, pName, bValue ? "true" : "false", bIsSet );
}

static void AppendXMLAttribute( CString &String, const char *pName, double Value, bool bIsSet = true )
{
	if( !bIsSet )
		return;
	CString TempString;
	TempString.Format( "%lf", Value );
	AppendXMLAttribute( String, pName, (const char*)TempString, bIsSet );
}

bool CLinkageDoc::ReadIn( CArchive& ar, bool bSelectAll, bool bObeyUnscaleOffset, bool bUseSavedSelection, bool bResetColors )
{
	CWaitCursor Wait;
	int OffsetConnectorIdentifer = m_HighestConnectorID + 1;
	int OffsetLinkIdentifer = m_HighestLinkID + 1;
	unsigned long long Length = ar.GetFile()->GetLength();
	CString Data;
	ar.Read( Data.GetBuffer( (int)Length + 1 ), (unsigned int)Length );
	Data.ReleaseBuffer( (int)Length );

	QuickXMLNode XMLData;
	if( !XMLData.Parse( Data ) )
		return false;

	QuickXMLNode *pRootNode = XMLData.GetFirstChild();
	if( pRootNode == 0 || !pRootNode->IsLink() || pRootNode->GetText() != "linkage2" )
		return false;

	m_SelectedConnectors.RemoveAll();
	m_SelectedLinks.RemoveAll();

	CPoint Offset( 0, 0 );
	double Unscale = 1;
	double ScaleFactor = 1.0;
	CString Units = GetUnitsString( CLinkageDoc::MM );

	CString Value;

	QuickXMLNode *pSelectedNode = 0;

	/*
	 * Get program information first. Skip anything else.
	 */
	for( QuickXMLNode *pNode = pRootNode->GetFirstChild(); pNode != 0; pNode = pNode->GetNextSibling() )
	{
		if( !pNode->IsLink() )
			continue;

		if( pNode->GetText() == "program" )
		{
			Value = pNode->GetAttribute( "zoom" );
			Unscale = atof( Value );
			if( Unscale == 0 )
				Unscale = 1;
			Value = pNode->GetAttribute( "xoffset" );
			Offset.x = atoi( Value );
			Value = pNode->GetAttribute( "yoffset" );
			Offset.y = atoi( Value );
			Value = pNode->GetAttribute( "units" );
			if( !Value.IsEmpty() )
				Units = Value;
			Value = pNode->GetAttribute( "scalefactor" );
			ScaleFactor = atoi( Value );
			if( Value.IsEmpty() || ScaleFactor == 0.0 )
				ScaleFactor = 1.0;
			Value = pNode->GetAttribute( "viewlayers" );
			if( atoi( Value ) == 0 )
				m_ViewLayers = ALLLAYERS;
			else
				m_ViewLayers = atoi( Value );
			Value = pNode->GetAttribute( "editlayers" );
			if( atoi( Value ) == 0 )
				m_EditLayers = ALLLAYERS;
			else
				m_EditLayers = atoi( Value );
		}

		if( pNode->GetText() == "selected" )
			pSelectedNode = pNode;
	}

	CFPoint UpperLeft( DBL_MAX, DBL_MAX );

	/*
	 * Get connector information. Some data is skipped here because all of
	 * the connectors are needed to handle sliding connector information.
	 */
	for( QuickXMLNode *pNode = pRootNode->GetFirstChild(); pNode != 0; pNode = pNode->GetNextSibling() )
	{
		if( !pNode->IsLink() )
			continue;

		if( pNode->GetText() == "connector" )
		{
			CConnector *pConnector = new CConnector;
			if( pConnector == 0 )
				break;
			CFPoint Point;
			CString Value;
			Value = pNode->GetAttribute( "anchor" );
			pConnector->SetAsAnchor( Value == "true" );
			Value = pNode->GetAttribute( "input" );
			pConnector->SetAsInput( Value == "true" );
			Value = pNode->GetAttribute( "draw" );
			pConnector->SetAsDrawing( Value == "true" );
			Value = pNode->GetAttribute( "rpm" );
			pConnector->SetRPM( Value.IsEmpty() ? DEFAULT_RPM : atof( Value ) );
			Value = pNode->GetAttribute( "x" );
			Point.x = atof( Value );
			Value = pNode->GetAttribute( "y" );
			Point.y = atof( Value );
			pConnector->SetPoint( Point );
			Value = pNode->GetAttribute( "drawcircle" );
			pConnector->SetDrawCircleRadius( atof( Value ) );
			Value = pNode->GetAttribute( "rpm" );
			pConnector->SetRPM( atof( Value ) );
			Value = pNode->GetAttribute( "slideradius" );
			pConnector->SetSlideRadius( atof( Value ) );
			Value = pNode->GetAttribute( "id" );
			pConnector->SetIdentifier( atoi( Value ) + OffsetConnectorIdentifer );
			Value = pNode->GetAttribute( "alwaysmanual" );
			pConnector->SetAlwaysManual( Value == "true" );
			Value = pNode->GetAttribute( "measurementelement" );
			pConnector->SetMeasurementElement( Value == "true" );
			Value = pNode->GetAttribute( "name" );
			pConnector->SetName( Value );
			Value = pNode->GetAttribute( "layer" );
			if( Value.IsEmpty() || atoi( Value ) == 0 )
				pConnector->SetLayers( MECHANISMLAYER );
			else
				pConnector->SetLayers( atoi( Value ) );
			Value = pNode->GetAttribute( "color" );
			if( Value.IsEmpty() || bResetColors )
			{
				if( ( pConnector->GetLayers() & DRAWINGLAYER ) != 0 )
					pConnector->SetColor( RGB( 200, 200, 200 ) );
				else
					pConnector->SetColor( Colors[pConnector->GetIdentifier() % COLORS_COUNT] );
			}
			else
				pConnector->SetColor( (COLORREF)atoi( Value ) );
			Value = pNode->GetAttribute( "selected" );
//			if( bUseSavedSelection && Value == "true" )
//				SelectElement( pConnector );
			if( bSelectAll )
				SelectElement( pConnector );
			m_Connectors.AddTail( pConnector );
		}
	}

	/*
	 * Read the link information.
	 */
	for( QuickXMLNode *pNode = pRootNode->GetFirstChild(); pNode != 0; pNode = pNode->GetNextSibling() )
	{
		if( !pNode->IsLink() )
			continue;

		if( pNode->GetText() == "Link" || pNode->GetText() == "element" )
		{
			CLink *pLink = new CLink;
			if( pLink == 0 )
				break;
			CString Value;
			Value = pNode->GetAttribute( "id" );
			pLink->SetIdentifier( atoi( Value ) + OffsetLinkIdentifer );
			Value = pNode->GetAttribute( "linesize" );
			pLink->SetLineSize( atoi( Value ) );
			Value = pNode->GetAttribute( "solid" );
			pLink->SetSolid( Value == "true" );
			Value = pNode->GetAttribute( "locked" );
			pLink->SetLocked( Value == "true" );
			Value = pNode->GetAttribute( "measurementelement" );
			pLink->SetMeasurementElement( Value == "true" );
			Value = pNode->GetAttribute( "name" );
			pLink->SetName( Value );
			Value = pNode->GetAttribute( "selected" );
			if( bUseSavedSelection && Value == "true" )
				SelectElement( pLink );
			Value = pNode->GetAttribute( "layer" );
			if( Value.IsEmpty() || atoi( Value ) == 0 )
				pLink->SetLayers( MECHANISMLAYER );
			else
				pLink->SetLayers( atoi( Value ) );

			/*
			 * Loop through the child connector list and find the actual
			 * document connectors that have been created and connect things.
			 */
			for( QuickXMLNode *pConnectorNode = pNode->GetFirstChild(); pConnectorNode != 0; pConnectorNode = pConnectorNode->GetNextSibling() )
			{
				Value = pConnectorNode->GetAttribute( "id" );
				int Identifier = atoi( Value ) + OffsetConnectorIdentifer;
				CConnector *pConnector = FindConnector( Identifier );
				if( pConnector == 0 )
					continue;

				if( pConnectorNode->GetText() == "connector" )
				{
					pLink->AddConnector( pConnector );
					pConnector->AddLink( pLink );
				}
				else if( pConnectorNode->GetText() == "fastenconnector" )
					FastenThese( pConnector, pLink );
			}

			/*
			 * Read the actuator information after the connectors are all set
			 * to avoid any connector count or position problems.
			 */
			Value = pNode->GetAttribute( "actuator" );
			pLink->SetActuator( Value == "true" );
			Value = pNode->GetAttribute( "throw" );
			if( !Value.IsEmpty() )
				pLink->SetStroke( atof( Value ) );
			Value = pNode->GetAttribute( "cpm" );
			pLink->SetCPM( Value.IsEmpty() ? DEFAULT_RPM : atof( Value ) );
			Value = pNode->GetAttribute( "alwaysmanual" );
			pLink->SetAlwaysManual( Value == "true" );
			Value = pNode->GetAttribute( "gear" );
			pLink->SetGear( Value == "true" );
			Value = pNode->GetAttribute( "color" );
			if( Value.IsEmpty() || bResetColors )
			{
				if( ( pLink->GetLayers() & DRAWINGLAYER ) != 0 )
					pLink->SetColor( RGB( 200, 200, 200 ) );
				else
					pLink->SetColor( Colors[pLink->GetIdentifier() % COLORS_COUNT] );
			}
			else
				pLink->SetColor( (COLORREF)atoi( Value ) );
			if( bSelectAll )
				SelectElement( pLink );
			m_Links.AddTail( pLink );
		}
	}

	for( QuickXMLNode *pNode = pRootNode->GetFirstChild(); pNode != 0; pNode = pNode->GetNextSibling() )
	{
		if( !pNode->IsLink() )
			continue;

		if( pNode->GetText() == "Link" || pNode->GetText() == "element" )
		{
			CString Value;
			Value = pNode->GetAttribute( "id" );
			CLink *pLink = FindLink( atoi( Value ) + OffsetLinkIdentifer );
			if( pLink == 0 )
				break;

			Value = pNode->GetAttribute( "fastenlink" );
			if( !Value.IsEmpty() )
			{
				int FastenToId = atoi( Value ) + OffsetConnectorIdentifer;
				CLink *pFastenToLink = FindLink( FastenToId );
				if( pFastenToLink != 0 )
					FastenThese( pLink, pFastenToLink );
			}
			Value = pNode->GetAttribute( "fastenconnector" );
			if( !Value.IsEmpty() )
			{
				int FastenToId = atoi( Value ) + OffsetConnectorIdentifer;
				CConnector *pFastenToConnector = FindConnector( FastenToId );
				if( pFastenToConnector != 0 )
					FastenThese( pLink, pFastenToConnector );
			}
		}
	}

	/*
	 * Find connectors to link fastening and handle it.
	 * Read the sliding connector information and connect the sliders to
	 * the limiting connectors.
	 */
	for( QuickXMLNode *pNode = pRootNode->GetFirstChild(); pNode != 0; pNode = pNode->GetNextSibling() )
	{
		if( !pNode->IsLink() )
			continue;

		if( pNode->GetText() == "connector" )
		{
			Value = pNode->GetAttribute( "id" );
			int TestIdentifier = atoi( Value ) + OffsetConnectorIdentifer;

			/*
			 * Find the connector for this entry that is now part of the
			 * document.
			 */
			CConnector *pConnector = FindConnector( TestIdentifier );
			if( pConnector == 0 )
				break;

			Value = pNode->GetAttribute( "fasten" );
			if( Value.IsEmpty() )
				Value = pNode->GetAttribute( "fastenlink" );
			if( !Value.IsEmpty() )
			{
				int FastenToId = atoi( Value ) + OffsetConnectorIdentifer;
				CLink *pFastenToLink = FindLink( FastenToId );
				if( pFastenToLink != 0 )
					FastenThese( pConnector, pFastenToLink );
			}

			Value = pNode->GetAttribute( "slider" );
			if( Value != "true" )
				continue;

			/*
			 * Loop through the child connector list and find the actual
			 * document connectors that have been created and connect things.
			 */
			CConnector *pStart = 0;
			CConnector *pEnd = 0;
			for( QuickXMLNode *pLimiterNode = pNode->GetFirstChild(); pLimiterNode != 0; pLimiterNode = pLimiterNode->GetNextSibling() )
			{
				if( pLimiterNode->GetText() != "slidelimit" )
					continue;
				Value = pLimiterNode->GetAttribute( "id" );
				int LimitIdentifier = atoi( Value ) + OffsetConnectorIdentifer;

				CConnector *pLimitConnector = FindConnector( LimitIdentifier );
				if( pLimitConnector != 0 )
				{
					if( pStart == 0 )
						pStart = pLimitConnector;
					else if( pEnd == 0 )
						pEnd = pLimitConnector;
					else
						break;
				}
			}
			if( pEnd == 0 )
				pStart = 0;
			pConnector->SlideBetween( pStart, pEnd );
		}
	}

	for( QuickXMLNode *pNode = pRootNode->GetFirstChild(); pNode != 0; pNode = pNode->GetNextSibling() )
	{
		if( !pNode->IsLink() )
			continue;

		if( pNode->GetText() == "ratios" )
		{
			for( QuickXMLNode *pRatioNode = pNode->GetFirstChild(); pRatioNode != 0; pRatioNode = pRatioNode->GetNextSibling() )
			{
				if( pRatioNode->GetText() != "ratio" )
					continue;

				Value = pRatioNode->GetAttribute( "type" );
				CGearConnection::ConnectionType Type = ( Value == "gears" ? CGearConnection::GEARS : CGearConnection::CHAIN );
				Value = pRatioNode->GetAttribute( "sizeassize" );
				bool bSizeAsSize = Value == "true" && Type == CGearConnection::CHAIN;

				CLink *pLink1 = 0;
				CLink *pLink2 = 0;
				double Size1 = 0;
				double Size2 = 0;
				for( QuickXMLNode *pGearLinkNode = pRatioNode->GetFirstChild(); pGearLinkNode != 0; pGearLinkNode = pGearLinkNode->GetNextSibling() )
				{
					if( pGearLinkNode->GetText() != "link" )
						continue;
					Value = pGearLinkNode->GetAttribute( "id" );
					int GearLinkIdentifier = atoi( Value ) + OffsetConnectorIdentifer;
					Value = pGearLinkNode->GetAttribute( "size" );
					double Size = atof( Value );

					CLink *pGearLink = FindLink( GearLinkIdentifier );
					if( pGearLink != 0 )
					{
						if( pLink1 == 0 )
						{
							pLink1 = pGearLink;
							Size1 = Size;
						}
						else
						{
							pLink2 = pGearLink;
							Size2 = Size;
						}
					}
				}

				if( bSizeAsSize )
				{
					// This cancels out unit scaling done inside of the SetGearRatio function so that
					// the value set is the document gear size, not the gear size for display.
					Size1 *= m_UnitScaling;
					Size2 *= m_UnitScaling;
				}

				if( pLink1 != 0 && pLink2 != 0 && Size1 > 0.0 && Size2 > 0.0 )
					SetGearRatio( pLink1, pLink2, Size1, Size2, bSizeAsSize, Type, false );
			}
		}
	}

	if( pSelectedNode != 0 && bUseSavedSelection && !bSelectAll )
	{
		// Select connectors based on the <selected> elements so that the selection is in the same order as when originally selected.
		for( QuickXMLNode *pNode = pSelectedNode->GetFirstChild(); pNode != 0; pNode = pNode->GetNextSibling() )
		{
			if( !pNode->IsLink() )
				continue;

			Value = pNode->GetAttribute( "id" );
			int TestIdentifier = atoi( Value ) + OffsetLinkIdentifer;
			if( TestIdentifier < 0 )
				continue;

			if( pNode->GetText() == "connector" )
			{
				POSITION Position = m_Connectors.GetHeadPosition();
				while( Position != 0 )
				{
					CConnector *pConnector = m_Connectors.GetNext( Position );
					if( pConnector == 0 )
						break;
					if( pConnector->GetIdentifier() == TestIdentifier )
						SelectElement( pConnector );
				}
			}
			else if( pNode->GetText() == "link" )
			{
				POSITION Position = m_Links.GetHeadPosition();
				while( Position != 0 )
				{
					CLink *pLink = m_Links.GetNext( Position );
					if( pLink == 0 )
						break;
					if( pLink->GetIdentifier() == TestIdentifier )
						SelectElement( pLink );
				}
			}
		}
	}

	// Detect paste operations or any operation that adds to an existing
	// document. Any addition to an existing document causes the ID's to
	// be adjusted to use the lowest ID's available. Only stand-alone
	// document read operations result in maintaining the original ID's

	bool bChangeIdentifiers = ( OffsetConnectorIdentifer > 0 || OffsetLinkIdentifer > 0 );
	int NewHighConnectorID = m_HighestConnectorID;
	int NewHighLinkID = m_HighestLinkID;

	POSITION Position = m_Connectors.GetHeadPosition();
	while( Position != 0 )
	{
		CConnector *pConnector = m_Connectors.GetNext( Position );
		if( pConnector == 0 || pConnector->GetIdentifier() < OffsetConnectorIdentifer )
			continue;
		int UseID;
		if( bChangeIdentifiers )
		{
			UseID = m_IdentifiedConnectors.FindAndSetBit();
			pConnector->SetIdentifier( UseID );
		}
		else
		{
			UseID = pConnector->GetIdentifier();
			m_IdentifiedConnectors.SetBit( UseID );
		}
		if( UseID > NewHighConnectorID )
			NewHighConnectorID = UseID;
	}

	Position = m_Links.GetHeadPosition();
	while( Position != 0 )
	{
		CLink *pLink = m_Links.GetNext( Position );
		if( pLink == 0 || pLink->GetIdentifier() < OffsetLinkIdentifer )
			continue;
		int UseID;
		if( bChangeIdentifiers )
		{
			UseID = m_IdentifiedLinks.FindAndSetBit();
			pLink->SetIdentifier( UseID );
		}
		else
		{
			UseID = pLink->GetIdentifier();
			m_IdentifiedLinks.SetBit( UseID );
		}
		if( UseID > NewHighLinkID )
			NewHighLinkID = UseID;
	}

	m_HighestConnectorID = NewHighConnectorID;
	m_HighestLinkID = NewHighLinkID;

	NormalizeConnectorLinks();

	if( bObeyUnscaleOffset )
	{
		//AfxMessageBox( "ObeyUnscaleOffset" );
		CFrameWnd *pFrame = (CFrameWnd*)AfxGetMainWnd();
		CLinkageView *pView = pFrame == 0 ? 0 : (CLinkageView*)pFrame->GetActiveView();
		if( pView != 0 )
		{
			pView->SetOffset( Offset );
			pView->SetZoom( Unscale );
		}

		SetUnits( GetUnitsValue( Units ) );
		m_ScaleFactor = ScaleFactor;
	}

	SetSelectedModifiableCondition();

	return true;
}

bool CLinkageDoc::WriteOut( CArchive& ar, bool bSelectedOnly )
{
	CWaitCursor Wait;

	ar.WriteString( "<linkage2>" );
	ar.WriteString( CRLF );

	CString TempString;

	CFrameWnd *pFrame = (CFrameWnd*)AfxGetMainWnd();
	CLinkageView *pView = pFrame == 0 ? 0 : (CLinkageView*)pFrame->GetActiveView();
	if( pView != 0 )
	{
		CFPoint Point = pView->GetOffset();
		CString Units = GetUnitsString( m_Units );

		TempString = "\t<program";
		AppendXMLAttribute( TempString, "zoom", pView->GetZoom() );
		AppendXMLAttribute( TempString, "xoffset", (int)Point.x );
		AppendXMLAttribute( TempString, "yoffset", (int)Point.y );
		AppendXMLAttribute( TempString, "scalefactor", m_ScaleFactor );
		AppendXMLAttribute( TempString, "units", (const char*)Units );
		AppendXMLAttribute( TempString, "viewlayers", m_ViewLayers );
		AppendXMLAttribute( TempString, "editlayers", m_EditLayers );
		TempString += "/>";

//		TempString.Format( "\t<program zoom=\"%lf\" xoffset=\"%d\" yoffset=\"%d\" scalefactor=\"%lf\" units=\"%s\" viewlayers=\"%u\" editlayers=\"%u\"/>",
//		                   pView->GetZoom(), (int)Point.x, (int)Point.y, m_ScaleFactor, (const char*)Units,
//						   m_ViewLayers, m_EditLayers );
		ar.WriteString( TempString );
		ar.WriteString( CRLF );
	}

	POSITION Position = m_Connectors.GetHeadPosition();
	while( Position != 0 )
	{
		CConnector* pConnector = m_Connectors.GetNext( Position );
		if( pConnector == 0 )
			continue;
		if( bSelectedOnly &&!pConnector->IsSelected() && !pConnector->IsLinkSelected() )
			continue;
		CFPoint Point = pConnector->GetOriginalPoint();

		CConnector *pSlideStart;
		CConnector *pSlideEnd;
		bool bSlideLimits = pConnector->IsSlider() && pConnector->GetSlideLimits( pSlideStart, pSlideEnd ) && pSlideStart != 0 && pSlideEnd != 0;

//		CString FastenId;
//		FastenId.Format( "%d", pConnector->GetFastenedTo() == 0 ? 0 : pConnector->GetFastenedTo()->GetIdentifier() );

		TempString = "\t<connector";
		AppendXMLAttribute( TempString, "id", pConnector->GetIdentifier() );
		AppendXMLAttribute( TempString, "selected", pConnector->IsSelected() );
		AppendXMLAttribute( TempString, "name", (const char*)pConnector->GetName(), !pConnector->GetName().IsEmpty() );
		AppendXMLAttribute( TempString, "layer", pConnector->GetLayers() );
		AppendXMLAttribute( TempString, "x", Point.x );
		AppendXMLAttribute( TempString, "y", Point.y );
		AppendXMLAttribute( TempString, "anchor", pConnector->IsAnchor(), pConnector->IsAnchor() );
		AppendXMLAttribute( TempString, "input", pConnector->IsInput(), pConnector->IsInput() );
		AppendXMLAttribute( TempString, "draw", pConnector->IsDrawing(), pConnector->IsDrawing() );
		AppendXMLAttribute( TempString, "measurementelement", pConnector->IsMeasurementElement(), pConnector->IsMeasurementElement() );
		AppendXMLAttribute( TempString, "rpm", pConnector->GetRPM(), pConnector->IsInput() );
		AppendXMLAttribute( TempString, "slider", pConnector->IsSlider(), pConnector->IsSlider() );
		AppendXMLAttribute( TempString, "alwaysmanual", pConnector->IsAlwaysManual(), pConnector->IsAlwaysManual() );
		AppendXMLAttribute( TempString, "drawcircle", pConnector->GetDrawCircleRadius(), pConnector->GetDrawCircleRadius() > 0 );
		AppendXMLAttribute( TempString, "fastenlink", pConnector->GetFastenedToLink() == 0 ? 0 : pConnector->GetFastenedToLink()->GetIdentifier(), pConnector->GetFastenedToLink() != 0 );
		AppendXMLAttribute( TempString, "fastenconnector", pConnector->GetFastenedToConnector() == 0 ? 0 : pConnector->GetFastenedToConnector()->GetIdentifier(), pConnector->GetFastenedToConnector() != 0 );
		AppendXMLAttribute( TempString, "slideradius", pConnector->GetSlideRadius(), pConnector->GetSlideRadius() > 0 && pConnector->IsSlider() );
		AppendXMLAttribute( TempString, "color", (unsigned int)(COLORREF)pConnector->GetColor() );
		TempString += bSlideLimits ? ">" : "/>";

//		TempString.Format( "\t<connector id=\"%d\" selected=\"%s\" name=\"%s\" layer=\"%d\" anchor=\"%s\" input=\"%s\" draw=\"%s\" "
//						   "measurementelement=\"%s\" rpm=\"%lf\" x=\"%lf\" y=\"%lf\" slider=\"%s\" alwaysmanual=\"%s\" drawcircle=\"%f\" fasten=\"%s\" "
//						   "slideradius=\"%f\" color=\"%d\">",
//						   pConnector->GetIdentifier(), pConnector->IsSelected() ? "true" : "false", (const char*)pConnector->GetName(), pConnector->GetLayers(),
//		                   pConnector->IsAnchor() ? "true" : "false", pConnector->IsInput() ? "true" : "false",  pConnector->IsDrawing() ? "true" : "false",
//						   pConnector->IsMeasurementElement() ? "true" : "false",
//		                   pConnector->GetRPM(), Point.x, Point.y, pConnector->IsSlider() ? "true" : "false", pConnector->IsAlwaysManual() ? "true" : "false",
//						   pConnector->GetDrawCircleRadius(), pConnector->GetFastenedTo() == 0 ? "" : (const char*)FastenId,
//		                   pConnector->GetSlideRadius(), (COLORREF)pConnector->GetColor() );
		ar.WriteString( TempString );
		ar.WriteString( CRLF );

		if( bSlideLimits )
		{
			TempString.Format( "\t\t<slidelimit id=\"%d\"/>", pSlideStart->GetIdentifier() );
			ar.WriteString( TempString );
			ar.WriteString( CRLF );
			TempString.Format( "\t\t<slidelimit id=\"%d\"/>", pSlideEnd->GetIdentifier() );
			ar.WriteString( TempString );
			ar.WriteString( CRLF );

			ar.WriteString( "\t</connector>" );
			ar.WriteString( CRLF );
		}
	}

	Position = m_Links.GetHeadPosition();
	while( Position != 0 )
	{
		CLink* pLink = m_Links.GetNext( Position );
		if( pLink == 0 )
			continue;
		if( bSelectedOnly && !pLink->IsSelected() && !pLink->IsAnySelected() )
			continue;

		/*
		 * We may end up with multiple Links sharing a single connector
		 * while having no other connectors. This is because of the way a
		 * connector can alone be selected. Instead of trying to detect this,
		 * the code that reads data will detect this after things have been
		 * read using the normalizing function. We can ignore the problem
		 * since it only makes the files a tony bit confusing if read by eye.
		 */

//		CString FastenId;
//		FastenId.Format( "%d", pLink->GetFastenedTo() == 0 ? 0 : pLink->GetFastenedTo()->GetIdentifier() );

		TempString = "\t<Link";
		AppendXMLAttribute( TempString, "id", pLink->GetIdentifier() );
		AppendXMLAttribute( TempString, "selected", pLink->IsSelected() );
		AppendXMLAttribute( TempString, "name", (const char*)pLink->GetName(), !pLink->GetName().IsEmpty() );
		AppendXMLAttribute( TempString, "layer", pLink->GetLayers() );
		AppendXMLAttribute( TempString, "linesize", pLink->GetLineSize() );
		AppendXMLAttribute( TempString, "solid", pLink->IsSolid(), pLink->IsSolid() );
		AppendXMLAttribute( TempString, "locked", pLink->IsLocked(), pLink->IsLocked() );
		AppendXMLAttribute( TempString, "actuator", pLink->IsActuator(), pLink->IsActuator() );
		AppendXMLAttribute( TempString, "alwaysmanual", pLink->IsAlwaysManual(), pLink->IsAlwaysManual() );
		AppendXMLAttribute( TempString, "measurementelement", pLink->IsMeasurementElement(), pLink->IsMeasurementElement() );
		AppendXMLAttribute( TempString, "throw", pLink->GetStroke(), pLink->IsActuator() );
		AppendXMLAttribute( TempString, "cpm", pLink->GetCPM(), pLink->IsActuator() );
		AppendXMLAttribute( TempString, "fastenlink", pLink->GetFastenedToLink() == 0 ? 0 : pLink->GetFastenedToLink()->GetIdentifier(), pLink->GetFastenedToLink() != 0 );
		AppendXMLAttribute( TempString, "fastenconnector", pLink->GetFastenedToConnector() == 0 ? 0 : pLink->GetFastenedToConnector()->GetIdentifier(), pLink->GetFastenedToConnector() != 0 );
		AppendXMLAttribute( TempString, "gear", pLink->IsGear(), pLink->IsGear() );
		AppendXMLAttribute( TempString, "color", (unsigned int)(COLORREF)pLink->GetColor() );
		TempString += ">";

//		TempString.Format( "\t<Link id=\"%d\" selected=\"%s\" name=\"%s\" layer=\"%d\" linesize=\"%d\" solid=\"%s\" actuator=\"%s\" alwaysmanual=\"%s\" "
//						   "measurementelement=\"%s\" throw=\"%lf\" cpm=\"%lf\" fasten=\"%s\" gear=\"%s\" color=\"%d\">",
//		                   pLink->GetIdentifier(), pLink->IsSelected() ? "true" : "false", (const char*)pLink->GetName(), pLink->GetLayers(),
//						   pLink->GetLineSize(), pLink->IsSolid() ? "true" : "false",
//		                   pLink->IsActuator() ? "true" : "false", pLink->IsAlwaysManual() ? "true" : "false",
//						   pLink->IsMeasurementElement() ? "true" : "false",
//						   pLink->GetStroke(), pLink->GetCPM(), pLink->GetFastenedTo() == 0 ? "" : (const char*)FastenId,
//						   pLink->IsGear() ? "true" : "false", (COLORREF)pLink->GetColor() );
		ar.WriteString( TempString );
		ar.WriteString( CRLF );

		POSITION Position2 = pLink->GetConnectorList()->GetHeadPosition();
		while( Position2 != 0 )
		{
			CConnector* pConnector = pLink->GetConnectorList()->GetNext( Position2 );
			if( pConnector == 0 )
				continue;
			if( bSelectedOnly )
			{
				if( !pConnector->IsSelected() && !pLink->IsSelected() )
					continue;
			}
			TempString.Format( "\t\t<connector id=\"%d\"/>", pConnector->GetIdentifier() );
			ar.WriteString( TempString );
			ar.WriteString( CRLF );
		}

		ar.WriteString( "\t</Link>" );
		ar.WriteString( CRLF );
	}

	ar.WriteString( "\t<selected>" );
	ar.WriteString( CRLF );

	Position = m_SelectedConnectors.GetHeadPosition();
	while( Position != 0 )
	{
		CConnector* pConnector = m_SelectedConnectors.GetNext( Position );
		if( pConnector != 0 )
		{
			TempString.Format( "\t\t<connector id=\"%d\"/>", pConnector->GetIdentifier() );
			ar.WriteString( TempString );
			ar.WriteString( CRLF );
		}
	}
	Position = m_SelectedLinks.GetHeadPosition();
	while( Position != 0 )
	{
		CLink* pLink = m_SelectedLinks.GetNext( Position );
		if( pLink != 0 )
		{
			TempString.Format( "\t\t<link id=\"%d\"/>", pLink->GetIdentifier() );
			ar.WriteString( TempString );
			ar.WriteString( CRLF );
		}
	}
	ar.WriteString( "\t</selected>" );
	ar.WriteString( CRLF );

	Position = m_GearConnectionList.GetHeadPosition();
	if( Position != 0 )
	{
		ar.WriteString( "\t<ratios>" );
		ar.WriteString( CRLF );
		while( Position != 0 )
		{
			CGearConnection *pGearConnection = m_GearConnectionList.GetNext( Position );
			if( pGearConnection == 0 )
				continue;

			TempString.Format( "\t\t<ratio type=\"%s\" sizeassize=\"%s\">", pGearConnection->m_ConnectionType == pGearConnection->GEARS ? "gears" : "chain", pGearConnection->m_bUseSizeAsRadius ? "true" : "false" );
			ar.WriteString( TempString );
			ar.WriteString( CRLF );

			TempString.Format( "\t\t\t<link id=\"%d\" size=\"%lf\"/>", pGearConnection->m_pGear1->GetIdentifier(), pGearConnection->m_Gear1Size );
			ar.WriteString( TempString );
			ar.WriteString( CRLF );
			TempString.Format( "\t\t\t<link id=\"%d\" size=\"%lf\"/>", pGearConnection->m_pGear2->GetIdentifier(), pGearConnection->m_Gear2Size );
			ar.WriteString( TempString );
			ar.WriteString( CRLF );

			ar.WriteString( "\t\t</ratio>" );
			ar.WriteString( CRLF );
		}
		ar.WriteString( "\t</ratios>" );
		ar.WriteString( CRLF );
	}

	// This extra step is to get save Links for connectors that are
	// orphans. Those that are selected but are not part of any Links
	// that have already been saved. These get an Link just for them.
	ar.WriteString( "</linkage2>" );
	ar.WriteString( CRLF );

	return true;
}

void CLinkageDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
		WriteOut( ar );
	else
		ReadIn( ar, false, true, false, false );
}

/////////////////////////////////////////////////////////////////////////////
// CLinkageDoc diagnostics

#ifdef _DEBUG
void CLinkageDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CLinkageDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

bool CLinkageDoc::ClearSelection( void )
{
	bool bChanged = false;
	POSITION Position = m_Links.GetHeadPosition();
	while( Position != 0 )
	{
		CLink* pLink = m_Links.GetNext( Position );
		if( pLink == 0 )
			continue;
		if( pLink->IsSelected() )
			bChanged = true;
		pLink->Select( false );
	}
	Position = m_Connectors.GetHeadPosition();
	while( Position != 0 )
	{
		CConnector* pConnector = m_Connectors.GetNext( Position );
		if( pConnector == 0 )
			continue;
		if( pConnector->IsSelected() )
			bChanged = true;
		pConnector->Select( false );
	}
	m_pCapturedConnector = 0;
	m_pCapturedController = 0;
	m_SelectedConnectors.RemoveAll();
	m_SelectedLinks.RemoveAll();

	SetSelectedModifiableCondition();

	return bChanged;
}

bool CLinkageDoc::SelectElement( void )
{
	ClearSelection();
	return false;
}

bool CLinkageDoc::SelectElement( CLink *pLink )
{
	if( pLink->IsSelected() )
		return false;

	m_SelectedLinks.AddHead( pLink );

	pLink->Select( true );
	return true;
}

bool CLinkageDoc::DeSelectElement( CLink *pLink )
{
	if( !pLink->IsSelected() )
		return false;

	if( !m_SelectedLinks.Remove( pLink ) )
		return false;

	pLink->Select( false );

	return true;
}

bool CLinkageDoc::DeSelectElement( CConnector *pConnector )
{
	if( !pConnector->IsSelected() )
		return false;

	if( !m_SelectedConnectors.Remove( pConnector ) )
		return false;

	pConnector->Select( false );

	if( m_pCapturedConnector == pConnector )
		m_pCapturedConnector = 0;

	return true;
}

bool CLinkageDoc::SelectElement( CConnector *pConnector )
{
	if( pConnector->IsSelected() )
		return false;

	m_SelectedConnectors.AddHead( pConnector );
	pConnector->Select( true );
	m_pCapturedConnector = pConnector;

	return true;
}

bool CLinkageDoc::SelectElement( CFRect Rect, bool bMultiSelect, bool &bSelectionChanged )
{
	bSelectionChanged = false;
	if( !bMultiSelect )
		bSelectionChanged = ClearSelection();

	POSITION Position = m_Links.GetHeadPosition();
	while( Position != 0 )
	{
		CLink* pLink = m_Links.GetNext( Position );
		if( pLink == 0 || ( pLink->GetLayers() & m_EditLayers ) == 0 )
			continue;
		CFRect Area;
		pLink->GetAdjustArea( m_GearConnectionList, Area );
		if( Area.IsInsideOf( Rect ) )
			bSelectionChanged |= SelectElement( pLink );
	}
	Position = m_Connectors.GetHeadPosition();
	while( Position != 0 )
	{
		CConnector* pConnector = m_Connectors.GetNext( Position );
		if( pConnector == 0 || ( pConnector->GetLayers() & m_EditLayers ) == 0 )
			continue;
		CFPoint Point = pConnector->GetPoint();
		if( Point.IsInsideOf( Rect ) )
			bSelectionChanged |= SelectElement( pConnector );
	}
	m_pCapturedConnector = 0;
	m_pCapturedController = 0;

	SetSelectedModifiableCondition();

	return bSelectionChanged;
}

bool CLinkageDoc::FindElement( CFPoint Point, double GrabDistance, CLink *&pFoundLink, CConnector *&pFoundConnector )
{
	pFoundLink = 0;
	pFoundConnector = 0;

	POSITION Position = m_Connectors.GetHeadPosition();
	while( Position != 0 )
	{
		CConnector* pConnector = m_Connectors.GetNext( Position );
		if( pConnector != 0 && pConnector->PointOnConnector( Point, GrabDistance )
		    && ( pConnector->GetLayers() & m_EditLayers ) != 0 )
		{
			pFoundConnector = pConnector;
			return true;
		}
	}
	Position = m_Links.GetHeadPosition();
	while( Position != NULL )
	{
		CLink* pLink = m_Links.GetNext( Position );
		if( pLink != 0 && pLink->PointOnLink( m_GearConnectionList, Point, GrabDistance )
		    && ( pLink->GetLayers() & m_EditLayers ) != 0 )
		{
			pFoundLink = pLink;
			return true;
		}
	}
	return false;
}

bool CLinkageDoc::AutoJoinSelected( void )
{
	if( m_SelectedConnectors.GetCount() != 1 || m_SelectedLinks.GetCount() > 0 )
		return false;

	CConnector *pConnector = (CConnector*)m_SelectedConnectors.GetHead();
	if( pConnector == 0 )
		return false;

	CFPoint Point = pConnector->GetPoint();

	POSITION Position = m_Connectors.GetHeadPosition();
	while( Position != NULL )
	{
		CConnector* pCheckConnector = m_Connectors.GetNext( Position );
		if( pCheckConnector == 0 || pCheckConnector == pConnector
		    || ( pConnector->GetLayers() & pCheckConnector->GetLayers() ) == 0 )
			continue;

		//if( pCheckConnector->IsAlone() )
		//	continue;

		CFPoint CheckPoint = pCheckConnector->GetPoint();

		if( Point == CheckPoint )
			SelectElement( pCheckConnector );
	}

	JoinSelected( false );

	return true;
}

bool CLinkageDoc::SelectElement( CFPoint Point, double GrabDistance, bool bMultiSelect, bool &bSelectionChanged )
{
	CConnector* pSelectingConnector = 0;
	CLink *pSelectingLink = 0;
	CConnector *pSelectingController = 0;

	bSelectionChanged = false;

	// Check only the already selected items first before checking all items.
	// This makes it easier to drag a pasted item that is selected but not
	// checked first otherwise.

	POSITION Position = m_Connectors.GetHeadPosition();
	while( Position != 0 )
	{
		CConnector* pConnector = m_Connectors.GetNext( Position );
		if( pConnector != 0 && pConnector->IsSelected() && pConnector->PointOnConnector( Point, GrabDistance )
		    && ( pConnector->GetLayers() & m_EditLayers ) != 0 )
		{
			pSelectingConnector = pConnector;
			break;
		}
	}

	if( pSelectingConnector == 0 && pSelectingLink == 0 && pSelectingController == 0 )
	{
		Position = m_Links.GetHeadPosition();
		while( Position != NULL )
		{
			CLink* pLink = m_Links.GetNext( Position );
			if( pLink != 0 && pLink->IsSelected() && pLink->PointOnLink( m_GearConnectionList, Point, GrabDistance )
			    && ( pLink->GetLayers() & m_EditLayers ) != 0 )
			{
				if( pLink->GetConnectorCount() == 1 && !pLink->IsGear() )
					pSelectingConnector = pLink->GetConnector( 0 );
				else
					pSelectingLink = pLink;
				break;
			}
		}
	}

	/*
	 * Test for selecting an unselected connector, controller, or link.
	 */

	if( pSelectingConnector == 0 && pSelectingLink == 0 && pSelectingController == 0 )
	{
		Position = m_Links.GetHeadPosition();
		while( Position != NULL )
		{
			CLink* pLink = m_Links.GetNext( Position );
			if( pLink != NULL )
			{
				CConnector *pStrokeConnector = pLink->GetStrokeConnector( 0 );
				if( !bMultiSelect && pStrokeConnector != 0 && pStrokeConnector->PointOnConnector( Point, GrabDistance )
				    && ( pLink->GetLayers() & m_EditLayers ) != 0 )
				{
					pSelectingController = pStrokeConnector;
					break;
				}
			}
		}
	}

	if( pSelectingConnector == 0 && pSelectingLink == 0 && pSelectingController == 0 )
	{
		Position = m_Connectors.GetHeadPosition();
		while( Position != 0 )
		{
			CConnector* pConnector = m_Connectors.GetNext( Position );
			if( pConnector != 0 && pConnector->PointOnConnector( Point, GrabDistance )
			    && ( pConnector->GetLayers() & m_EditLayers ) != 0 )
			{
				pSelectingConnector = pConnector;
				break;
			}
		}
	}

	if( pSelectingConnector == 0 && pSelectingLink == 0 && pSelectingController == 0 )
	{
		Position = m_Links.GetHeadPosition();
		while( Position != NULL )
		{
			CLink* pLink = m_Links.GetNext( Position );
			if( pLink != NULL && pLink->PointOnLink( m_GearConnectionList, Point, GrabDistance )
			    && ( pLink->GetLayers() & m_EditLayers ) != 0 )
			{
				CConnector *pStrokeConnector = pLink->GetStrokeConnector( 0 );
				if( pLink->GetConnectorCount() == 1 && !pLink->IsGear() )
					pSelectingConnector = pLink->GetConnector( 0 );
				else
					pSelectingLink = pLink;
				break;
			}
		}
	}

	if( pSelectingController != 0 )
	{
		if( !pSelectingController->IsSelected() )
		{
			ClearSelection();
			pSelectingController->Select( true );
			bSelectionChanged = true;
			m_SelectedConnectors.AddHead( pSelectingConnector );
		}
		m_pCapturedController = pSelectingController;
		m_CaptureOffset.x = Point.x - pSelectingController->GetPoint().x;
		m_CaptureOffset.y = Point.y - pSelectingController->GetPoint().y;

		SetSelectedModifiableCondition();

		return true;
	}

	if( pSelectingConnector != 0 )
	{
		if( bMultiSelect )
		{
			if( pSelectingConnector->IsSelected() )
			{
				/*
				 * Remove this from the selected connector list.
				 */
				POSITION Position = m_SelectedConnectors.GetHeadPosition();
				while( Position != 0 )
				{
					CConnector *pCheckConnector = m_SelectedConnectors.GetAt( Position );
					if( pCheckConnector == pSelectingConnector )
					{
						m_SelectedConnectors.RemoveAt( Position );
						break;
					}
					m_SelectedConnectors.GetNext( Position );
				}
			}
			else
			{
				/*
				 * Add this to the selected connectors list at the beginning
				 * of the list.
				 */
				m_SelectedConnectors.AddHead( pSelectingConnector );
			}

			m_pCapturedConnector = 0;
			m_pCapturedController = 0;
			pSelectingConnector->Select( !pSelectingConnector->IsSelected() );
			bSelectionChanged = true;

			SetSelectedModifiableCondition();

			return true;
		}
		else
		{
			if( !pSelectingConnector->IsSelected() )
			{
				ClearSelection();
				bSelectionChanged = SelectElement( pSelectingConnector );
			}
			m_pCapturedConnector = pSelectingConnector;
			m_CaptureOffset.x = Point.x - pSelectingConnector->GetPoint().x;
			m_CaptureOffset.y = Point.y - pSelectingConnector->GetPoint().y;
		}

		SetSelectedModifiableCondition();

		return m_pCapturedConnector != 0;
	}

	if( pSelectingLink != 0 )
	{
		m_pCapturedConnector = 0;
		m_pCapturedController = 0;
		if( bMultiSelect )
		{
			if( pSelectingLink->IsSelected() )
			{
				/*
				 * Remove this from the selected connector list.
				 */
				POSITION Position = m_SelectedLinks.GetHeadPosition();
				while( Position != 0 )
				{
					CLink *pCheckLink = m_SelectedLinks.GetAt( Position );
					if( pCheckLink == pSelectingLink )
					{
						m_SelectedLinks.RemoveAt( Position );
						break;
					}
					m_SelectedLinks.GetNext( Position );
				}
			}
			else
			{
				/*
				 * Add this to the selected connectors list at the beginning
				 * of the list.
				 */
				m_SelectedLinks.AddHead( pSelectingLink );
			}

			pSelectingLink->Select( !pSelectingLink->IsSelected() );
			bSelectionChanged = true;
			SetSelectedModifiableCondition();
			return true;
		}
		else
		{
			if( !pSelectingLink->IsSelected() )
			{
				ClearSelection();
				bSelectionChanged = SelectElement( pSelectingLink );
			}
			m_pCapturedConnector = pSelectingLink->GetConnectorList()->GetHead();
			m_CaptureOffset.x = Point.x - m_pCapturedConnector->GetPoint().x;
			m_CaptureOffset.y = Point.y - m_pCapturedConnector->GetPoint().y;
		}
		SetSelectedModifiableCondition();
		return m_pCapturedConnector != 0;
	}

	if( !bMultiSelect )
		ClearSelection();
	else
		SetSelectedModifiableCondition();

	return false;
}

bool CLinkageDoc::StretchSelected( double Percentage )
{
	CFPoint AveragePoint;
	int Points = 0;

	POSITION Position = m_Connectors.GetHeadPosition();
	while( Position != 0 )
	{
		CConnector* pConnector = m_Connectors.GetNext( Position );
		if( pConnector == 0 )
			continue;
		if( pConnector->IsSelected() || pConnector->IsLinkSelected() )
		{
			AveragePoint += pConnector->GetOriginalPoint();
			++Points;
		}
	}
	AveragePoint.x /= Points;
	AveragePoint.y /= Points;

	CFRect OriginalRect( AveragePoint.x - 100, AveragePoint.y - 100, AveragePoint.x + 100, AveragePoint.y + 100 );
	CFRect NewRect( AveragePoint.x - Percentage, AveragePoint.y - Percentage, AveragePoint.x + Percentage, AveragePoint.y + Percentage );

	bool bResult = StretchSelected( OriginalRect, NewRect, DIAGONAL );

	if( bResult )
		FinishChangeSelected();

	return bResult;
}

bool CLinkageDoc::StretchSelected( CFRect OriginalRect, CFRect NewRect, _Direction Direction )
{
	POSITION Position = m_Connectors.GetHeadPosition();
	while( Position != 0 )
	{
		CConnector* pConnector = m_Connectors.GetNext( Position );
		if( pConnector == 0 )
			continue;

		if( !pConnector->IsSelected() && !pConnector->IsLinkSelected() )
			continue;

		CFPoint Scale( OriginalRect.Width() == 0 ? 1 : NewRect.Width() / OriginalRect.Width(), OriginalRect.Height() == 0 ? 1 : NewRect.Height() / OriginalRect.Height() );

		CFPoint OldPoint = pConnector->GetOriginalPoint();
		CFPoint NewPoint;
		NewPoint.x = NewRect.left + ( ( OldPoint.x - OriginalRect.left ) * Scale.x );
		NewPoint.y = NewRect.top + ( ( OldPoint.y - OriginalRect.top ) * Scale.y );

		if( Direction == DIAGONAL )
		{
			// These adjustments to the scale are based on there being different x and y scale factors. For
			// diagonal scaling, the scaling should be the same in both directions but this code is left
			// here for when this changes or for when scaling is needed to work when just one direction is scaled.
			double UseScale = max( Scale.x, Scale.y );
			if( Scale.x == 1.0 )
				UseScale = Scale.y;
			else if( Scale.y == 1.0 )
				UseScale = Scale.x;

			pConnector->SetIntermediateDrawCircleRadius( pConnector->GetOriginalDrawCircleRadius() * UseScale );
			pConnector->SetIntermediateSlideRadius( pConnector->GetOriginalSlideRadius() * UseScale );
		}

		bool bSkipThisOne = false;
		POSITION Position2 = pConnector->GetLinkList()->GetHeadPosition();
		while( Position2 != 0 )
		{
			CLink *pLink = pConnector->GetLinkList()->GetNext( Position2 );
			if( pLink == 0 )
				continue;

			if( pLink->IsLocked() )
			{
				bSkipThisOne = true;
				break;
			}
		}

		if( !bSkipThisOne )
			pConnector->SetIntermediatePoint( NewPoint );
	}

	FixupSliderLocations();

    SetModifiedFlag( true );
	return true;
}

bool CLinkageDoc::MoveCapturedController( CFPoint Point )
{
	CFPoint Temp = m_pCapturedController->GetOriginalPoint();
	CFPoint Offset;
	Offset.x = ( Point.x - m_CaptureOffset.x ) - Temp.x;
	Offset.y = ( Point.y - m_CaptureOffset.y ) - Temp.y;

	if( Offset.x == 0.0 && Offset.y == 0.0 )
		return false;

	CFPoint ConnectorPoint = m_pCapturedController->GetOriginalPoint();
	ConnectorPoint += Offset;
	m_pCapturedController->SetIntermediatePoint( ConnectorPoint );

	CConnector *pStart;
	CConnector *pEnd;
	m_pCapturedController->GetSlideLimits( pStart, pEnd );

	if( m_pCapturedController->IsSlider() && pStart != 0 && pEnd != 0 )
	{
		CFLine TempLine( pStart->GetPoint(), pEnd->GetPoint() );
		CFPoint ConnectorPoint = m_pCapturedController->GetPoint();
		ConnectorPoint.SnapToLine( TempLine, true, true );
		m_pCapturedController->SetIntermediatePoint( ConnectorPoint );
	}

	CLink *pLink = m_pCapturedController->GetLink( 0 );
	if( pLink != 0 )
		pLink->UpdateFromController();

    SetModifiedFlag( true );
	return true;
}

CFPoint CLinkageDoc::CheckForSnap( double SnapDistance, bool bElementSnap, bool bGridSnap, double xGrid, double yGrid, CFPoint &ReferencePoint )
{
	CFPoint Adjustment( 0, 0 );

	bool bSnapGrid = false;

	if( bGridSnap )
	{
		POSITION Position = m_Connectors.GetHeadPosition();
		while( Position != 0 && !bSnapGrid )
		{
			CConnector* pConnector = m_Connectors.GetNext( Position );
			if( pConnector == 0 )
				continue;

			if( !pConnector->IsSelected() && !pConnector->IsLinkSelected() )
				continue;

			if( pConnector->IsSlider() )
				continue;

			CFPoint ConnectorPoint = pConnector->GetPoint();

			double xMin = ( (int)( ConnectorPoint.x / xGrid ) ) * xGrid;
			double xMax = xMin + ( xGrid * ( ConnectorPoint.x >= 0 ? 1 : -1 ) );

			double yMin = ( (int)( ConnectorPoint.y / yGrid ) ) * yGrid;
			double yMax = yMin + ( yGrid * ( ConnectorPoint.y >= 0 ? 1 : -1 ) );

			if( fabs( ConnectorPoint.x - xMin ) < SnapDistance )
			{
				bSnapGrid = true;
				Adjustment.x = xMin - ConnectorPoint.x;
				m_SnapLine[0].SetLine( xMin, -99999, xMin, 99999 );
				ReferencePoint.x = ConnectorPoint.x + Adjustment.x;
			}
			else if( fabs( ConnectorPoint.x - xMax ) < SnapDistance )
			{
				bSnapGrid = true;
				Adjustment.x = xMax - ConnectorPoint.x;
				m_SnapLine[0].SetLine( xMax, -99999, xMax, 99999 );
				ReferencePoint.x = ConnectorPoint.x + Adjustment.x;
			}
			if( fabs( ConnectorPoint.y - yMin ) < SnapDistance )
			{
				bSnapGrid = true;
				Adjustment.y = yMin - ConnectorPoint.y;
				m_SnapLine[1].SetLine( -99999, yMin, 99999, yMin );
				ReferencePoint.y = ConnectorPoint.y + Adjustment.y;
			}
			else if( fabs( ConnectorPoint.y - yMax ) < SnapDistance )
			{
				bSnapGrid = true;
				Adjustment.y = yMax - ConnectorPoint.y;
				m_SnapLine[1].SetLine( -99999, yMax, 99999, yMax );
				ReferencePoint.y = ConnectorPoint.y + Adjustment.y;
			}
		}
	}

	/*
	 * Item snap overrides grid snap to make auto-join easier.
	 */

	bool bSnapItem = false;

	if( bElementSnap )
	{
		/*
		 * Check each selected connector to see if it is close to a non-selected
		 * connector. If so, move all of the selected connectors so that the close
		 * one is lined up on the snap connector.
		 */

		POSITION Position = m_Connectors.GetHeadPosition();
		while( Position != 0 && !bSnapItem )
		{
			CConnector* pConnector = m_Connectors.GetNext( Position );
			if( pConnector == 0 || ( pConnector->GetLayers() & m_EditLayers ) == 0 )
				continue;

			if( !pConnector->IsSelected() && !pConnector->IsLinkSelected() )
				continue;

			if( pConnector->IsSlider() )
				continue;

			CFPoint ConnectorPoint = pConnector->GetPoint();

			POSITION Position2 = m_Connectors.GetHeadPosition();
			while( Position2 != 0 )
			{
				CConnector* pCheckConnector = m_Connectors.GetNext( Position2 );
				if( pCheckConnector == 0 || ( pConnector->GetLayers() & m_EditLayers ) == 0 )
					continue;

				if( pCheckConnector->IsSelected() || pCheckConnector->IsLinkSelected() )
					continue;

				/*
				 * This is a snap to the horizontal or vertical position of the
				 * connector, not a snap to both coordinates. Check the distance
				 * in each direction instead of an absolute distance.
				 */

				CFPoint CheckPoint = pCheckConnector->GetOriginalPoint();
				if( fabs( ConnectorPoint.x - CheckPoint.x ) < SnapDistance )
				{
					bSnapItem = true;
					Adjustment.x = CheckPoint.x - ConnectorPoint.x;
					m_SnapLine[0].SetLine( CheckPoint, CFPoint( ConnectorPoint.x + Adjustment.x, ConnectorPoint.y ) );
					ReferencePoint.x = ConnectorPoint.x + Adjustment.x;
				}
				if( fabs( ConnectorPoint.y - CheckPoint.y ) < SnapDistance )
				{
					bSnapItem = true;
					Adjustment.y = CheckPoint.y - ConnectorPoint.y;
					m_SnapLine[1].SetLine( CheckPoint, CFPoint( ConnectorPoint.x, ConnectorPoint.y + Adjustment.y ) );
					ReferencePoint.y = ConnectorPoint.y + Adjustment.y;
				}
			}
		}
	}

	#if 0
	if( bSnapItem || bSnapGrid )
	{
		POSITION Position = m_Connectors.GetHeadPosition();
		while( Position != 0 )
		{
			CConnector* pConnector = m_Connectors.GetNext( Position );
			if( pConnector == 0 )
				continue;

			if( !pConnector->IsSelected() && !pConnector->IsLinkSelected() )
				continue;

			CFPoint ConnectorPoint = pConnector->GetPoint();
			ConnectorPoint += Adjustment;
			pConnector->SetIntermediatePoint( ConnectorPoint );
		}
	}
	#endif

	if( bSnapItem || bSnapGrid )
		return Adjustment; //bSnapItem || bSnapGrid;
	else
		return CFPoint( 0, 0 );
}

int CLinkageDoc::BuildSelectedLockGroup( ConnectorList *pLockGroup )
{
	/*
	 * Build a list of all connectors that are selected or are attached to links that are locked and
	 * have a selected connector.
	 */

	CBitArray LockedConnectors;
	LockedConnectors.SetLength( 0 );

	POSITION Position = m_Connectors.GetHeadPosition();
	while( Position != 0 )
	{
		CConnector* pConnector = m_Connectors.GetNext( Position );
		if( pConnector == 0 )
			continue;

		if( !pConnector->IsSelected() && !pConnector->IsLinkSelected() )
			continue;

		LockedConnectors.SetBit( pConnector->GetIdentifier() );
	}

	for(;;)
	{
		int ChangeCount = 0;
		POSITION Position = m_Links.GetHeadPosition();
		while( Position != 0 )
		{
			CLink *pLink = m_Links.GetNext( Position );
			if( pLink == 0 || !pLink->IsLocked() )
				continue;

			bool bOneIsSelectedLocked = false;
			POSITION Position2 = pLink->GetConnectorList()->GetHeadPosition();
			while( Position2 != 0 )
			{
				CConnector *pConnector = pLink->GetConnectorList()->GetNext( Position2 );
				if( pConnector == 0 )
					continue;
				if( LockedConnectors.GetBit( pConnector->GetIdentifier() ) )
				{
					bOneIsSelectedLocked = true;
					break;
				}
			}

			if( bOneIsSelectedLocked )
			{
				POSITION Position2 = pLink->GetConnectorList()->GetHeadPosition();
				while( Position2 != 0 )
				{
					CConnector *pConnector = pLink->GetConnectorList()->GetNext( Position2 );
					if( pConnector == 0 || LockedConnectors.GetBit( pConnector->GetIdentifier() ) )
						continue;

					LockedConnectors.SetBit( pConnector->GetIdentifier() );
					++ChangeCount;
				}
			}
		}
		if( ChangeCount == 0 )
			break;
	}

	Position = m_Connectors.GetHeadPosition();
	while( Position != 0 )
	{
		CConnector *pConnector = m_Connectors.GetNext( Position );
		if( pConnector == 0 )
			continue;
		if( LockedConnectors.GetBit( pConnector->GetIdentifier() ) )
			pLockGroup->AddTail( pConnector );
	}

	return pLockGroup->GetCount();
}

bool CLinkageDoc::MoveSelected( CFPoint Point, bool bElementSnap, bool bGridSnap, double xGrid, double yGrid, double SnapDistance, CFPoint &ReferencePoint )
{
	m_SnapLine[0].SetLine( 0, 0, 0, 0 );
	m_SnapLine[1].SetLine( 0, 0, 0, 0 );

	if( m_pCapturedController != 0 )
		return MoveCapturedController( Point );

	if( m_pCapturedConnector == 0 )
		return false;

	CFPoint OriginalPoint = m_pCapturedConnector->GetOriginalPoint();
	CFPoint Offset;
	Offset.x = ( Point.x - m_CaptureOffset.x ) - OriginalPoint.x;
	Offset.y = ( Point.y - m_CaptureOffset.y ) - OriginalPoint.y;

	if( Offset.x == 0.0 && Offset.y == 0.0 )
		return false;

	/*
	 * There is one special case where a single moved connector on a single locked link that has only two connectors
	 * will rotate the locked link.
	 */

	if( m_SelectedConnectors.GetCount() == 1 && m_SelectedLinks.GetCount() == 0 )
	{
		CConnector *pConnector = m_SelectedConnectors.GetHead();
		if( pConnector != 0 && pConnector == m_pCapturedConnector )
		{
			int LockedLinkCount = 0;
			CLink *pLockedLink = 0;
			POSITION cPosition = pConnector->GetLinkList()->GetHeadPosition();
			while( cPosition != 0 )
			{
				CLink *pLink = pConnector->GetLinkList()->GetNext( cPosition );
				if( pLink == 0 || !pLink->IsLocked() || pLink->GetConnectorCount() != 2 )
					continue;
				++LockedLinkCount;
				pLockedLink = pLink;
			}

			if( LockedLinkCount == 1 )
			{
				// Rotate the locked link around it's other connector to get the
				// selected connector to be as close to the desired position as possible.
				CConnector *pCenter = pLockedLink->GetConnector( 0 );
				if( pCenter == pConnector )
					pCenter = pLockedLink->GetConnector( 1 );

				double Angle = GetAngle( pCenter->GetOriginalPoint(), CFPoint( Point.x - m_CaptureOffset.x, Point.y - m_CaptureOffset.y ), OriginalPoint );
				pConnector->MovePoint( pConnector->GetOriginalPoint() );
				pConnector->RotateAround( pCenter->GetOriginalPoint(), Angle );
				pConnector->MakePermanent();

				return true;
			}
		}
	}

	ConnectorList MoveConnectors;
	int MoveCount = BuildSelectedLockGroup( &MoveConnectors );
	POSITION Position = MoveConnectors.GetHeadPosition();
	while( Position != 0 )
	{
		CConnector* pConnector = MoveConnectors.GetNext( Position );
		if( pConnector == 0 )
			continue;

		CFPoint ConnectorPoint = pConnector->GetOriginalPoint();
		ConnectorPoint += Offset;
		pConnector->SetIntermediatePoint( ConnectorPoint );
	}

	bool bSnapReference = false;

	if( bElementSnap || bGridSnap )
	{
		CFPoint SnapReference = ReferencePoint;
		CFPoint Adjustment = CheckForSnap( SnapDistance, bElementSnap, bGridSnap, xGrid, yGrid, SnapReference );

		if( Adjustment.x != 0 || Adjustment.y != 0 )
		{
			Position = MoveConnectors.GetHeadPosition();
			while( Position != 0 )
			{
				CConnector* pConnector = MoveConnectors.GetNext( Position );
				if( pConnector == 0 )
					continue;

				CFPoint ConnectorPoint = pConnector->GetPoint();
				ConnectorPoint += Adjustment;
				pConnector->SetIntermediatePoint( ConnectorPoint );
			}
		}
	}

	FixupSliderLocations();

	if( !bSnapReference && m_pCapturedConnector != 0 && m_SelectedConnectors.GetCount() == 1 )
		ReferencePoint = m_pCapturedConnector->GetPoint();

    SetModifiedFlag( true );
	return true;
}

bool CLinkageDoc::FixupSliderLocation( CConnector *pConnector )
{
	CConnector *pStart;
	CConnector *pEnd;
	pConnector->GetSlideLimits( pStart, pEnd );

	if( !pConnector->IsSlider() || pStart == 0 || pEnd == 0 )
		return false;

	if( pStart->IsSelected() || pEnd->IsSelected()
		|| pStart->IsLinkSelected() || pEnd->IsLinkSelected() )
	{
		// The slider needs to be placed on the path at a position that
		// is the same relative distance along the path as it was originally.
		// This is because it is the path that is being changed at this time.
		if( pConnector->GetSlideRadius() == 0 )
		{
			double DistanceRatio = pStart->GetOriginalPoint().DistanceToPoint( pConnector->GetOriginalPoint() ) / pStart->GetOriginalPoint().DistanceToPoint( pEnd->GetOriginalPoint() );
			CFLine Line( pStart->GetPoint(), pEnd->GetPoint() );
			Line.SetDistance( Line.GetDistance() * DistanceRatio );
			pConnector->SetIntermediatePoint( Line.GetEnd() );
		}
		else
		{
			// Use the original points to find where the line out to the slider hits the straight
			// line between the start and end. This gives a ratio that can be used to position the
			// slider on the arc in the right spot.
			// First use the originbal arc.
			CFArc TheArc;
			if( !pConnector->GetSliderArc( TheArc, true ) )
				return false;

			CFLine Ray( CFPoint( TheArc.x, TheArc.y ), pConnector->GetOriginalPoint() );
			CFLine LimitLine( TheArc.GetStart(), TheArc.GetEnd() );
			CFPoint Intersection;
			if( !Intersects( Ray, LimitLine, Intersection ) )
				return false;

			double DistanceRatio = TheArc.GetStart().DistanceToPoint( Intersection ) / LimitLine.GetDistance();
			CFPoint ConnectorPoint = pConnector->GetPoint();

			// Now use the new arc.
			if( !pConnector->GetSliderArc( TheArc, false ) )
				return false;

			double StartAngle = GetAngle( CFPoint( TheArc.x, TheArc.y ), TheArc.m_Start );
			double EndAngle = GetAngle( CFPoint( TheArc.x, TheArc.y ), TheArc.m_End );
			if( EndAngle < StartAngle )
				EndAngle += 360.0;
			double Angle = StartAngle + ( ( EndAngle - StartAngle ) * DistanceRatio );

			ConnectorPoint.SetPoint( CFPoint( TheArc.x, TheArc.y ), fabs( TheArc.r ), Angle );
			pConnector->SetIntermediatePoint( ConnectorPoint );
		}
	}
	else //if( pConnector->IsSelected() || pConnector->IsLinkSelected() )
	{
		// the slider needs to be snapped onto the slider path because it is
		// the element that is being changed at this time.
		CFPoint ConnectorPoint = pConnector->GetPoint();
		if( pConnector->GetSlideRadius() == 0 )
		{
			CFLine TempLine( pStart->GetPoint(), pEnd->GetPoint() );
			ConnectorPoint.SnapToLine( TempLine, true );
			pConnector->SetIntermediatePoint( ConnectorPoint );
		}
		else
		{
			CFArc TheArc;
			if( !pConnector->GetSliderArc( TheArc ) )
				return false;
			ConnectorPoint.SnapToArc( TheArc );
			pConnector->SetIntermediatePoint( ConnectorPoint );
		}
	}

	return true;
}

bool CLinkageDoc::FixupSliderLocations( void )
{
	for( int Counter = 0; Counter < 2; ++Counter)
	{
		POSITION Position = m_Connectors.GetHeadPosition();
		while( Position != 0 )
		{
			CConnector* pConnector = m_Connectors.GetNext( Position );
			if( pConnector == 0 )
				continue;

			if( !pConnector->IsSlider() )
				continue;

			FixupSliderLocation( pConnector );
		}

		// Also fixup controllers for all links that have them.
		Position = m_Links.GetHeadPosition();
		while( Position != 0 )
		{
			CLink *pLink = m_Links.GetNext( Position );
			if( pLink == 0 )
				continue;

			pLink->UpdateController();
		}
	}

	return true;
}

bool CLinkageDoc::FixupGearConnections( void )
{
	POSITION Position = m_GearConnectionList.GetHeadPosition();
	while( Position != 0 )
	{
		CGearConnection *pGC = m_GearConnectionList.GetNext( Position );
	}

	return true;
}

bool CLinkageDoc::RotateSelected( CFPoint CenterPoint, double Angle )
{
	if( Angle == 0 )
		return true;

	ConnectorList RotateConnectors;
	int RotateCount = BuildSelectedLockGroup( &RotateConnectors );
	POSITION Position = RotateConnectors.GetHeadPosition();
	while( Position != 0 )
	{
		CConnector* pConnector = RotateConnectors.GetNext( Position );
		if( pConnector == 0 )
			continue;

		pConnector->MovePoint( pConnector->GetOriginalPoint() );
		pConnector->RotateAround( CenterPoint, Angle );
		pConnector->MakePermanent();
	}

	FixupSliderLocations();

    SetModifiedFlag( true );
	return true;
}

bool CLinkageDoc::StartChangeSelected( void )
{
	PushUndo();
	return true;
}

bool CLinkageDoc::FinishChangeSelected( void )
{
	POSITION Position = m_Connectors.GetHeadPosition();
	while( Position != 0 )
	{
		CConnector* pConnector = m_Connectors.GetNext( Position );
		if( pConnector == 0 )
			continue;

		pConnector->SetPoint( pConnector->GetPoint() );
		pConnector->SetDrawCircleRadius( pConnector->GetDrawCircleRadius() );
		pConnector->SetSlideRadius( pConnector->GetSlideRadius() );
		pConnector->Reset( true );
	}

	if( m_pCapturedController != 0 )
	{
		m_pCapturedController->SetPoint( m_pCapturedController->GetPoint() );
		m_pCapturedController->Reset( true );
		m_pCapturedController->Select( false );
		m_pCapturedController = 0;
	}

	m_SnapLine[0].SetLine( 0, 0, 0, 0 );
	m_SnapLine[1].SetLine( 0, 0, 0, 0 );

    SetModifiedFlag( true );
	return true;
}

void CLinkageDoc::Reset( bool bClearMotionPath, bool KeepCurrentPositions )
{
	if( KeepCurrentPositions )
		PushUndo();

	ClearSelection();

	POSITION Position;

	Position = m_Connectors.GetHeadPosition();
	while( Position != NULL )
	{
		CConnector* pConnector = m_Connectors.GetNext( Position );
		if( pConnector == 0 )
			continue;

		if( KeepCurrentPositions )
			pConnector->SetPoint( pConnector->GetPoint() );
		pConnector->Reset( bClearMotionPath );
	}

	Position = m_Links.GetHeadPosition();
	while( Position != NULL )
	{
		CLink *pLink = m_Links.GetNext( Position );
		if( pLink == 0 )
			continue;

		pLink->Reset();
	}

	SetSelectedModifiableCondition();
}

bool CLinkageDoc::LockSelected( void )
{
	if( m_SelectedConnectors.GetCount() > 0 || m_SelectedLinks.GetCount() == 0 )
		return false;

	int Count = 0;
	POSITION Position = m_SelectedLinks.GetHeadPosition();
	while( Position != NULL )
	{
		CLink* pLink = m_SelectedLinks.GetNext( Position );
		if( pLink == 0 )
			continue;

		pLink->SetLocked( true );
	}

	return Count > 0;
}

bool CLinkageDoc::IsLinkLocked( CConnector *pConnector )
{
	if( pConnector == 0 )
		return false;

	POSITION Position = pConnector->GetLinkList()->GetHeadPosition();
	while( Position != 0 )
	{
		CLink *pLink = pConnector->GetLinkList()->GetNext( Position );
		if( pLink == 0 )
			continue;

		if( pLink->IsLocked() )
			return true;
	}
	return false;
}

bool CLinkageDoc::JoinSelected( bool bSaveUndoState )
{
	if( !m_bSelectionJoinable )
		return false;

	CFPoint AveragePoint;
	CFPoint LockedPoint;
	int Counter = 0;
	bool bInput = false;
	bool bAnchor = false;
	bool bIsSlider = false;
	CFPoint ForcePoint;
	int Sliders = 0;
	int CombinedState = 0;
	unsigned int CombinedLayers = 0;
	int LockedLinks = 0;
	CConnector *pLockedConnector = 0;
	int Connectors = m_SelectedConnectors.GetCount();

	POSITION Position = m_SelectedConnectors.GetHeadPosition();
	CConnector *pKeepConnector = m_SelectedConnectors.GetAt( Position );

	for( ; Position != NULL; )
	{
		CConnector* pConnector = m_SelectedConnectors.GetNext( Position );
		if( pConnector == 0 || !pConnector->IsSelected() )
			continue;

		POSITION Position2 = pConnector->GetLinkList()->GetHeadPosition();
		while( Position2 != 0 )
		{
			CLink *pLink = pConnector->GetLinkList()->GetNext( Position2 );
			if( pLink == 0 || !pLink->IsLocked() )
				continue;
			++LockedLinks;
			pLockedConnector = pConnector;
			LockedPoint = pLockedConnector->GetOriginalPoint();
		}

		bInput |= pConnector->IsInput();
		bAnchor |= pConnector->IsAnchor();
		bIsSlider |= pConnector->IsSlider();

		if( pConnector->IsSlider() )
		{
			++Sliders;
			pKeepConnector = pConnector;
		}
	}

	if( Sliders > 1 )
		return false;

	if( LockedLinks > 1 )
		return false;

	if( m_SelectedLinks.GetCount() > 0 && Sliders > 0 )
		return false;

	if( pKeepConnector->IsSlider() && LockedLinks > 0 && pLockedConnector != pKeepConnector )
		return false;

	if( bSaveUndoState )
		PushUndo();

	Position = m_SelectedConnectors.GetHeadPosition();

	for( Counter = 0; Position != NULL; ++Counter )
	{
		CConnector* pConnector = m_SelectedConnectors.GetNext( Position );
		if( pConnector == 0 || !pConnector->IsSelected() )
			continue;

		CombinedLayers |= pConnector->GetLayers();

		AveragePoint += pConnector->GetOriginalPoint();

		// After this is stuff to be done to the links that are being deleted.
		if( pConnector == pKeepConnector )
			continue;

		POSITION Position2 = pConnector->GetLinkList()->GetHeadPosition();
		while( Position2 != 0 )
		{
			CLink *pLink = pConnector->GetLinkList()->GetNext( Position2 );
			if( pLink == 0 )
				continue;

			pLink->RemoveConnector( pConnector );
			pLink->AddConnector( pKeepConnector );
			pKeepConnector->AddLink( pLink );
		}

		m_Connectors.Remove( pConnector );
		m_IdentifiedConnectors.ClearBit( pConnector->GetIdentifier() );
		delete pConnector;
	}

	/*
	 * Check for sliding connectors that have any of the joined conectors as their limits. Change the limits to use the new joined connector.
	 * IMPORTANT: The joined connectors are all deleted except for the one being kept. The pointers are stil available but are not pointing to
	 * any valid memory!
	 */
	Position = m_Connectors.GetHeadPosition();
	while( Position != NULL )
	{
		CConnector *pConnector = m_Connectors.GetNext( Position );
		if( pConnector == NULL )
			continue;

		CConnector *pLimit1 = NULL;
		CConnector *pLimit2 = NULL;

		if( pConnector->GetSlideLimits( pLimit1, pLimit2 ) )
		{
			if( pLimit1->IsSelected() && pLimit2->IsSelected() )
			{
				pConnector->SlideBetween();
				continue;
			}

			POSITION Position2 = m_SelectedConnectors.GetHeadPosition();

			while( Position2 != NULL )
			{
				CConnector* pTestConnector = m_SelectedConnectors.GetNext( Position2 );
				if( pTestConnector == 0 || !pTestConnector->IsSelected() || pTestConnector == pKeepConnector )
					continue;

				if( pLimit1 == pTestConnector )
					pLimit1 = pKeepConnector;
				else if( pLimit2 == pTestConnector )
					pLimit2 = pKeepConnector;
				else
					continue;

				pConnector->SlideBetween( pLimit1, pLimit2 );
			}
		}
	}

	AveragePoint.x /= Counter;
	AveragePoint.y /= Counter;

	if( pLockedConnector != 0 )
		AveragePoint = LockedPoint;

	if( !pKeepConnector->IsSlider() )
		pKeepConnector->SetPoint( AveragePoint );

	pKeepConnector->SetAsInput( bInput );
	pKeepConnector->SetAsAnchor( bAnchor );
	pKeepConnector->SetLayers( CombinedLayers );

	m_SelectedConnectors.RemoveAll();
	m_SelectedConnectors.AddHead( pKeepConnector );

	/*
	 * Now that all connectors are joined, join the new single connector to all of the selected links.
	 */
	if( m_SelectedLinks.GetCount() )
	{
		Position = m_SelectedLinks.GetHeadPosition();
		while( Position != 0 )
		{
			CLink *pLink = m_SelectedLinks.GetNext( Position );
			if( pLink == 0 )
				continue;
			pLink->AddConnector( pKeepConnector );
		}
	}

	NormalizeConnectorLinks();

	SetSelectedModifiableCondition();

	FixupSliderLocations();

    SetModifiedFlag( true );

	return true;
}

void CLinkageDoc::ConnectSelected( void )
{
	PushUndo();

	// Make a new Link and add the selected connectors to it.
	// Do not move anything.

	if( GetSelectedConnectorCount() <= 1 )
		return;

	CLink *pLink = new CLink();
	if( pLink == 0 )
		return;

	int NewID = m_IdentifiedLinks.FindAndSetBit();
	if( NewID > m_HighestLinkID )
		m_HighestLinkID = NewID;
	pLink->SetIdentifier( NewID );

	unsigned int Layers = 0;

	POSITION Position = m_Connectors.GetHeadPosition();
	while( Position != NULL )
	{
		CConnector* pConnector = m_Connectors.GetNext( Position );
		if( pConnector == 0 || !pConnector->IsSelected() )
			continue;
		// Check for an existing single link for this connector.
		CLink *pLoneLink = pConnector->GetLink( 0 );
		if( pLoneLink != 0 && pLoneLink->GetConnectorCount() == 1 && !pLoneLink->IsGear() )
		{
			pLoneLink->RemoveAllConnectors();
			pConnector->RemoveAllLinks();
			m_Links.Remove( pLoneLink );
			m_SelectedLinks.Remove( pLoneLink );
			m_IdentifiedLinks.ClearBit( pLoneLink->GetIdentifier() );
			delete pLoneLink;
		}

		Layers = pConnector->GetLayers();
		pLink->SetLayers( Layers );
		pConnector->AddLink( pLink );
		pLink->AddConnector( pConnector );
	}

	pLink->SetColor( ( Layers & DRAWINGLAYER ) != 0 ? RGB( 200, 200, 200 ) : Colors[pLink->GetIdentifier() % COLORS_COUNT] );

	m_Links.AddTail( pLink );

	SetSelectedModifiableCondition();

    SetModifiedFlag( true );
}

void CLinkageDoc::RawAngleSelected( double Angle )
{
	CConnector *pConnectors[3];

	for( int Counter = 0; Counter < 3; ++Counter )
	{
		pConnectors[Counter] = (CConnector*)GetSelectedConnector( Counter );
		if( pConnectors[Counter] == 0 )
			return;
	}
	double OldAngle = GetAngle( pConnectors[1]->GetPoint(), pConnectors[0]->GetPoint(), pConnectors[2]->GetPoint() );
	double ChangeAngle;
	if( OldAngle <= 180 )
		ChangeAngle = OldAngle - Angle;
	else
		ChangeAngle = OldAngle - ( 360 - Angle );

	CFPoint MovePoint = pConnectors[2]->GetPoint();

	MovePoint.RotateAround( pConnectors[1]->GetPoint(), ChangeAngle );
	pConnectors[2]->SetPoint( MovePoint );
}

void CLinkageDoc::MakeSelectedAtAngle( double Angle )
{
	if( !IsSelectionTriangle() )
		return;

	PushUndo();

	RawAngleSelected( Angle );

    SetModifiedFlag( true );

	NormalizeConnectorLinks();

	SetSelectedModifiableCondition();
}

void CLinkageDoc::MakeRightAngleSelected( void )
{
	if( !IsSelectionTriangle() )
		return;

	PushUndo();

	RawAngleSelected( 90 );

    SetModifiedFlag( true );

	NormalizeConnectorLinks();

	SetSelectedModifiableCondition();
}

// Need a global variable for the distance sort function :(
static CFPoint DistanceCompareStart;

// Distance sort function.
static int DistanceCompare( const void *tp1, const void *tp2 )
{
}

void CLinkageDoc::AlignSelected( _Direction Direction )
{
	switch( Direction )
	{
		case INLINE:
		case INLINESPACED:
		{
			if( m_AlignConnectorCount <= 2 )
				return;

			PushUndo();

			CFLine Line;

			POSITION Position = m_SelectedConnectors.GetTailPosition();
			if( Position == 0 )
				return;

			CConnector *pConnector = m_SelectedConnectors.GetAt( Position );
			if( pConnector == 0 )
				return;

			Line.m_Start = pConnector->GetPoint();

			Position = m_SelectedConnectors.GetHeadPosition();
			if( Position == 0 )
				return;

			pConnector = m_SelectedConnectors.GetNext( Position );
			if( pConnector == 0 )
				return;

			Line.m_End = pConnector->GetPoint();

			int PointCount = m_SelectedConnectors.GetCount();

			vector<ConnectorDistance> ConnectorReference( PointCount - 2 );

			int Counter = 0;
			while( Position != 0 && Position != m_SelectedConnectors.GetTailPosition() )
			{
				CConnector *pConnector = m_SelectedConnectors.GetNext( Position );
				CFPoint MovePoint = pConnector->GetPoint();

				CFLine TempLine;
				Line.PerpendicularLine( TempLine );
				TempLine -= TempLine[0];
				TempLine += MovePoint;
				if( Intersects( TempLine, Line, MovePoint ) )
					pConnector->SetPoint( MovePoint );

				ConnectorReference[Counter].m_pConnector = pConnector;
				ConnectorReference[Counter].m_Distance = Distance( Line.m_Start, MovePoint );

				++Counter;
			}

			if( Direction == INLINESPACED )
			{
				sort( ConnectorReference.begin(), ConnectorReference.end(), CompareConnectorDistance() );

				double Space = 0.0;
				double Distance = Line.GetDistance();
				Space = Distance / ( PointCount - 1 );

				for( int Counter = 0; Counter < PointCount - 2; ++Counter )
				{
					Line.SetDistance( Space * ( Counter + 1 ) );
					ConnectorReference[Counter].m_pConnector->SetPoint( Line.m_End );
				}
			}

			break;
		}

		case HORIZONTAL:
		case VERTICAL:
		{
			if( m_AlignConnectorCount == 0 )
				return;
			PushUndo();

			CConnector *pFirstConnector = 0;

			POSITION Position = m_SelectedConnectors.GetTailPosition();
			if( Position != 0 )
				pFirstConnector = m_SelectedConnectors.GetPrev( Position );
			while( Position != 0 )
			{
				CConnector *pConnector = m_SelectedConnectors.GetPrev( Position );
				CFPoint MovePoint = pConnector->GetPoint();
				if( Direction == HORIZONTAL )
					MovePoint.y = pFirstConnector->GetPoint().y;
				else
					MovePoint.x = pFirstConnector->GetPoint().x;
				pConnector->SetPoint( MovePoint );
			}
			break;
		}

		case FLIPHORIZONTAL:
		case FLIPVERTICAL:
		{
			int Selected = GetSelectedConnectorCount();
			if( GetSelectedLinkCount( true ) > 0 )
				Selected += 2;

			if( Selected < 2 )
				return;

			PushUndo();

			double LowestLocation = DBL_MAX;
			double HighestLocation = -DBL_MAX;

			POSITION Position = m_Connectors.GetHeadPosition();
			while( Position != 0 )
			{
				CConnector *pConnector = m_SelectedConnectors.GetNext( Position );
				if( pConnector == 0 )
					continue;

				if( pConnector->IsSelected() || pConnector->IsLinkSelected() )
				{
					CFPoint Point = pConnector->GetPoint();
					if( Direction == FLIPHORIZONTAL )
					{
						if( Point.x < LowestLocation )
							LowestLocation = Point.x;
						if( Point.x > HighestLocation )
							HighestLocation = Point.x;
					}
					else
					{
						if( Point.y < LowestLocation )
							LowestLocation = Point.y;
						if( Point.y > HighestLocation )
							HighestLocation = Point.y;
					}

					pConnector->SetRPM( -pConnector->GetRPM() );
				}
			}

			double Midpoint = ( LowestLocation + HighestLocation ) / 2;

			Position = m_Connectors.GetHeadPosition();
			while( Position != 0 )
			{
				CConnector *pConnector = m_SelectedConnectors.GetNext( Position );
				if( pConnector == 0 )
					continue;

				if( pConnector->IsSelected() || pConnector->IsLinkSelected() )
				{
					CFPoint Point = pConnector->GetPoint();
					double Distance = 0;
					if( Direction == FLIPHORIZONTAL )
					{
						Distance = Point.x - Midpoint;
						Point.x = Midpoint - Distance;
					}
					else
					{
						Distance = Point.y - Midpoint;
						Point.y = Midpoint - Distance;
					}
					pConnector->SetPoint( Point );
				}
			}
			break;
		}
	}

	FinishChangeSelected();

    SetModifiedFlag( true );

	NormalizeConnectorLinks();

	SetSelectedModifiableCondition();
}

void CLinkageDoc::MakeParallelogramSelected( bool bMakeRectangle )
{
	if( !IsSelectionRectangle() )
		return;

	PushUndo();

	if( bMakeRectangle )
	{
		// Create the right triangle first.
		RawAngleSelected( 90 );
	}

	// Move the last point into position.
	CConnector *pConnectors[4];

	for( int Counter = 0; Counter < 4; ++Counter )
	{
		pConnectors[Counter] = (CConnector*)GetSelectedConnector( Counter );
		if( pConnectors[Counter] == 0 )
			return;
	}

	double dx = 0;
	double dy = 0;

	dx = pConnectors[1]->GetPoint().x - pConnectors[0]->GetPoint().x;
	dy = pConnectors[1]->GetPoint().y - pConnectors[0]->GetPoint().y;

	CFPoint NewPoint( pConnectors[2]->GetPoint().x - dx, pConnectors[2]->GetPoint().y - dy );
	pConnectors[3]->SetPoint( NewPoint );

    SetModifiedFlag( true );

	FinishChangeSelected();

	NormalizeConnectorLinks();

	SetSelectedModifiableCondition();
}

void CLinkageDoc::MakeAnchorSelected( void )
{
	PushUndo();

	POSITION Position = m_Connectors.GetHeadPosition();
	while( Position != NULL )
	{
		POSITION DeletePosition = Position;
		CConnector* pConnector = m_Connectors.GetNext( Position );
		if( pConnector == 0 || !pConnector->IsSelected() )
			continue;

		pConnector->SetAsAnchor( true );
	}

	FinishChangeSelected();

	SetSelectedModifiableCondition();
}

void CLinkageDoc::CombineSelected( void )
{
	PushUndo();

	int Count = 0;
	CLink *pKeepLink = 0;
	POSITION Position = m_Links.GetHeadPosition();
	while( Position != NULL )
	{
		POSITION DeletePosition = Position;
		CLink* pLink = m_Links.GetNext( Position );
		if( pLink == 0 || !pLink->IsSelected() )
			continue;

		++Count;
		if( Count == 1 )
		{
			pKeepLink = pLink;
			continue;
		}

		POSITION Position2 = pLink->GetConnectorList()->GetHeadPosition();
		while( Position2 != NULL )
		{
			CConnector* pConnector = pLink->GetConnectorList()->GetNext( Position2 );
			if( pConnector == 0 )
				return;
			pConnector->RemoveLink( pLink );
			pConnector->AddLink( pKeepLink );
			pKeepLink->AddConnector( pConnector );
		}
		m_Links.RemoveAt( DeletePosition );
		m_IdentifiedLinks.ClearBit( pLink->GetIdentifier() );
		RemoveGearRatio( 0, pLink );
		delete pLink;
	    SetModifiedFlag( true );
	}

	NormalizeConnectorLinks();

	FinishChangeSelected();

	SetSelectedModifiableCondition();
}

void CLinkageDoc::DeleteSelected( void )
{
	PushUndo();

	POSITION Position = m_Links.GetHeadPosition();
	while( Position != 0 )
	{
		POSITION DeletePosition = Position;
		CLink *pLink = m_Links.GetNext( Position );
		if( pLink == 0 || !pLink->IsSelected() )
			continue;

		DeleteLink( pLink );

		// Start over because the list may have changed.
		POSITION Position = m_Links.GetHeadPosition();
	}

	Position = m_Connectors.GetHeadPosition();
	while( Position != NULL )
	{
		POSITION DeletePosition = Position;
		CConnector* pConnector = m_Connectors.GetNext( Position );
		if( pConnector == 0 || !pConnector->IsSelected() )
			continue;

		DeleteConnector( pConnector );

		// Start over because the list may have changed.
		POSITION Position = m_Connectors.GetHeadPosition();
	}

	NormalizeConnectorLinks();

	SetSelectedModifiableCondition();
}

void CLinkageDoc::RemoveConnector( CConnector *pConnector )
{
/*
	POSITION Position = m_Links.GetHeadPosition();
	while( Position != 0 )
	{
		CLink *pLink = m_Links.GetNext( Position );
		if( pLink == 0 )
			continue;
		pLink->RemoveConnector( pConnector );
	    SetModifiedFlag( true );
	}

	Position = m_Connectors.GetHeadPosition();
	while( Position != NULL )
	{
		CConnector* pCheckConnector = m_Connectors.GetNext( Position );
		if( pCheckConnector == 0 || pCheckConnector == pConnector )
			continue;

		CConnector* pLimit1 = 0;
		CConnector* pLimit2 = 0;
		if( pCheckConnector->IsSlider() && pCheckConnector->GetSlideLimits( pLimit1, pLimit2 ) )
		{
			if( pConnector == pLimit1 || pConnector == pLimit2 )
				pCheckConnector->SlideBetween();
		}
	}

	RemoveGearRatio( pConnector, 0 );
	//SetSelectedModifiableCondition();
	*/
}

void CLinkageDoc::RemoveLink( CLink *pLink )
{
/*
	POSITION Position = m_Connectors.GetHeadPosition();
	while( Position != 0 )
	{
		CConnector *pConnector = m_Connectors.GetNext( Position );
		if( pConnector == 0 )
			continue;
		pConnector->RemoveLink( pLink );
		Unfasten( pLink );
	    SetModifiedFlag( true );
		break;
	}
*/
}

bool CLinkageDoc::DeleteLink( CLink *pLink, CConnector *pDeletingConnector )
{
	ConnectorList *pList = pLink->GetConnectorList();
	POSITION Position = pList->GetHeadPosition();
	while( Position != 0 )
	{
		POSITION DeletePosition = Position;
		CConnector* pConnector = pList->GetNext( Position );
		if( pConnector == 0 || pConnector == pDeletingConnector )
			continue;

		if( pConnector->GetLinkCount() == 1 )
			DeleteConnector( pConnector, pLink );
		else
			pConnector->RemoveLink( pLink );
	}

	pLink->RemoveAllConnectors();
	m_Links.Remove( pLink );
	m_SelectedLinks.Remove( pLink );
	m_IdentifiedLinks.ClearBit( pLink->GetIdentifier() );
	RemoveGearRatio( 0, pLink );
	Unfasten( pLink );
	delete pLink;

	SetModifiedFlag( true );

	return true;
}

bool CLinkageDoc::DeleteConnector( CConnector *pConnector, CLink *pDeletingLink )
{
	CList<CLink*,CLink*> *pList = pConnector->GetLinkList();
	POSITION Position = pList->GetHeadPosition();
	while( Position != NULL )
	{
		POSITION DeletePosition = Position;
		CLink *pLink = pList->GetNext( Position );
		if( pLink == 0 || pLink == pDeletingLink )
			continue;

		if( pLink->GetConnectorCount() == 1 )
		{
			DeleteLink( pLink, pConnector );
			continue;
		}

		pLink->RemoveConnector( pConnector );
	}

	pConnector->RemoveAllLinks();
	m_IdentifiedConnectors.ClearBit( pConnector->GetIdentifier() );

	Position = m_Connectors.GetHeadPosition();
	while( Position != NULL )
	{
		CConnector* pCheckConnector = m_Connectors.GetNext( Position );
		if( pCheckConnector == 0 || pCheckConnector == pConnector )
			continue;

		CConnector* pLimit1 = 0;
		CConnector* pLimit2 = 0;
		if( pCheckConnector->IsSlider() && pCheckConnector->GetSlideLimits( pLimit1, pLimit2 ) )
		{
			if( pConnector == pLimit1 || pConnector == pLimit2 )
				pCheckConnector->SlideBetween();
		}
	}

	RemoveGearRatio( pConnector, 0 );
	Unfasten( pConnector );
	m_Connectors.Remove( pConnector );
	m_SelectedConnectors.Remove( pConnector );
	delete pConnector;

	return true;
}

void CLinkageDoc::NormalizeConnectorLinks( void )
{
	POSITION Position = m_Links.GetHeadPosition();
	while( Position != 0 )
	{
		POSITION DeletePosition = Position;
		CLink *pLink = m_Links.GetNext( Position );
		if( pLink == 0 )
			continue;

		POSITION Position = pLink->GetConnectorList()->GetHeadPosition();
		while( Position != NULL )
		{
			CConnector* pCheckConnector = pLink->GetConnectorList()->GetNext( Position );
			if( pCheckConnector == 0 )
				continue;
			if( pCheckConnector->IsSelected() )
			{
				pLink->UpdateController();
				break;
			}
		}

		CConnector *pConnector = 0;

		if( pLink->GetConnectorCount() == 1 )
			pConnector = pLink->GetConnectorList()->GetHead();
		else if( pLink->GetConnectorCount() > 1 )
			continue;

		// Detect when there is a link with a single connector and that single connector
		// is also on another link. This is a meaningless configuration unless the single-connector
		// link is a gear.

		if( pConnector == 0 || ( pConnector->GetLinkCount() > 1 && !pLink->IsGear() ) )
		{
			pLink->RemoveAllConnectors();
			if( pConnector != 0 )
				pConnector->RemoveLink( pLink );
			m_Links.RemoveAt( DeletePosition );
			m_SelectedLinks.Remove( pLink );
			m_IdentifiedLinks.ClearBit( pLink->GetIdentifier() );
			RemoveGearRatio( 0, pLink );
			delete pLink;
		    SetModifiedFlag( true );
		}
	}
}
void CLinkageDoc::SetViewLayers( unsigned int Layers )
{
	m_ViewLayers = Layers;
	SetEditLayers( m_EditLayers & m_ViewLayers );
}

void CLinkageDoc::SetEditLayers( unsigned int Layers )
{
	m_EditLayers = Layers;

	// Make sure that nothing is selected unless it is on one of the specified layers.
	POSITION Position = m_Connectors.GetHeadPosition();
	while( Position != NULL )
	{
		CConnector* pConnector = m_Connectors.GetNext( Position );
		if( pConnector != 0 && ( pConnector->GetLayers() & m_EditLayers ) == 0 )
			DeSelectElement( pConnector );
	}

	Position = m_Links.GetHeadPosition();
	while( Position != NULL )
	{
		CLink* pLink = m_Links.GetNext( Position );
		if( pLink != 0 && ( pLink->GetLayers() & m_EditLayers ) == 0 )
			DeSelectElement( pLink );
	}
}

int CLinkageDoc::GetSelectedConnectorCount( void )
{
	POSITION Position = m_Connectors.GetHeadPosition();
	int Count = 0;
	while( Position != NULL )
	{
		CConnector* pCheckConnector = m_Connectors.GetNext( Position );
		if( pCheckConnector != 0 && !pCheckConnector->IsSelected() )
			continue;
		++Count;
	}
	return Count;
}

int CLinkageDoc::GetSelectedLinkCount( bool bOnlyWithMultipleConnectors )
{
	int Count = 0;
	POSITION Position = m_SelectedLinks.GetHeadPosition();
	while( Position != NULL )
	{
		CLink* pLink = m_SelectedLinks.GetNext( Position );
		if( pLink == 0 )
			continue;

		if( pLink->IsSelected() )
		{
			if( !bOnlyWithMultipleConnectors || pLink->GetConnectorCount() > 1 )
				++Count;
		}
	}

	return Count;
}

bool CLinkageDoc::FindRoomFor( CFRect NeedRect, CFPoint &PlaceHere )
{
	PlaceHere.x -= NeedRect.Width() / 2;
	PlaceHere.y -= NeedRect.Height() / 2;
	for( int Counter = 0; Counter < 2000; ++Counter )
	{
		CFRect TestRect( PlaceHere.x, PlaceHere.y, PlaceHere.x+NeedRect.Width(), PlaceHere.y+NeedRect.Height() );

		bool bOverlapped = false;
		POSITION Position = m_Links.GetHeadPosition();
		while( Position != NULL )
		{
			CLink* pLink = m_Links.GetNext( Position );
			if( pLink == 0 || ( pLink->GetLayers() & m_EditLayers ) == 0 )
				continue;
			CFRect Rect;
			pLink->GetArea( m_GearConnectionList, Rect );
			Rect.InflateRect( 8, 8 );

			if( TestRect.IsOverlapped( Rect ) )
			{
				bOverlapped = true;
				break;
			}
		}
		if( !bOverlapped )
		{
			PlaceHere.x = TestRect.left;
			PlaceHere.y = TestRect.top;
			return true;
		}

		PlaceHere.x += 3;
		PlaceHere.y += 1;
	}
	return false;
}

void CLinkageDoc::InsertLink( unsigned int Layers, double ScaleFactor, CFPoint DesiredPoint, bool bForceToPoint, int ConnectorCount, bool bAnchor, bool bRotating, bool bSlider, bool bActuator, bool bMeasurement, bool bSolid, bool bGear )
{
	static bool bSelectInsert = true;

	if( ConnectorCount == 0 )
		return;

	static const int MAX_CONNECTOR_COUNT = 4;

	/*
	 * right now, there is no clever way to generate the location for
	 * the new connectors so 3 is the maximum.
	 */
	if( ConnectorCount > MAX_CONNECTOR_COUNT )
		return;

	PushUndo();

	ClearSelection();

	static CFPoint AddPoints[MAX_CONNECTOR_COUNT][MAX_CONNECTOR_COUNT] =
	{
		{ CFPoint( 0, 0 ), CFPoint( 0, 0 ), CFPoint( 0, 0 ), CFPoint( 0, 0 ) },
		{ CFPoint( 0, 0 ), CFPoint( 1.0, 1.0 ), CFPoint( 0, 0 ), CFPoint( 0, 0 ) },
		{ CFPoint( 0, 0 ), CFPoint( 0, 1.0 ), CFPoint( 1.0, 1.0 ), CFPoint( 0, 0 ) },
		{ CFPoint( 0, 0 ), CFPoint( 0, 1.0 ), CFPoint( 1.0, 1.0 ), CFPoint( 1.0, 0 ) }
	};

	CConnector **Connectors = new CConnector* [ConnectorCount];
	if( Connectors == 0 )
		return;
	int Index;
	for( Index = 0; Index < ConnectorCount; ++Index )
		Connectors[Index] = 0;

	CLink *pLink = new CLink;
	if( pLink == 0 )
	{
		delete [] Connectors;
		return;
	}

	for( Index = 0; Index < ConnectorCount; ++Index )
	{
		CConnector *pConnector = new CConnector;
		if( pConnector == 0 )
		{
			for( int Counter = 0; Counter < ConnectorCount; ++Counter )
			{
				if( Connectors[Counter] != 0 )
					delete Connectors[Counter];
			}
			delete [] Connectors;
			RemoveGearRatio( 0, pLink );
			delete pLink;
			return;
		}
		Connectors[Index] = pConnector;
		pConnector->SetLayers( Layers );
		pConnector->SetAsAnchor( bAnchor );
		pConnector->SetAsInput( bRotating );
		pConnector->SetRPM( DEFAULT_RPM );
		pConnector->SetPoint( CFPoint( AddPoints[ConnectorCount-1][Index].x * ScaleFactor, AddPoints[ConnectorCount-1][Index].y * ScaleFactor ) );
		pConnector->AddLink( pLink );
		pLink->AddConnector( pConnector );

		bAnchor = false;
		bRotating = false;
	}

	CFRect Rect( 0, 0, 0, 0 );

	pLink->GetArea( m_GearConnectionList, Rect );
	CFPoint Offset( -Rect.left, -Rect.bottom );
	Rect.left += Offset.x;
	Rect.top += Offset.x;
	Rect.right += Offset.x;
	Rect.bottom += Offset.x;

	Rect.right += 16;
	Rect.bottom += 16;

	CFPoint Place = DesiredPoint;
	if( !bForceToPoint )
		FindRoomFor( Rect, Place );
//	if( !bForceToPoint && !FindRoomFor( Rect, Place ) )
//	{
//		for( int Counter = 0; Counter < ConnectorCount; ++Counter )
//		{
//			if( Connectors[Counter] != 0 )
//				delete Connectors[Counter];
//		}
//		delete [] Connectors;
//		RemoveGearRatio( 0, pLink );
//		delete pLink;
//		return;
//	}

	int NewID = m_IdentifiedLinks.FindAndSetBit();
	if( NewID > m_HighestLinkID )
		m_HighestLinkID = NewID;
	pLink->SetIdentifier( NewID );
	m_Links.AddTail( pLink );
	for( int Counter = 0; Counter < ConnectorCount; ++Counter )
	{
		CFPoint Point = Connectors[Counter]->GetOriginalPoint();
		Point += Place;
		Point += Offset;
		Connectors[Counter]->SetPoint( Point );
		int NewID = m_IdentifiedConnectors.FindAndSetBit();
		if( NewID > m_HighestConnectorID )
			m_HighestConnectorID = NewID;
		Connectors[Counter]->SetIdentifier( NewID );
		Connectors[Counter]->SetColor( ( Layers & DRAWINGLAYER ) != 0 ? RGB( 200, 200, 200 ) : Colors[Connectors[Counter]->GetIdentifier() % COLORS_COUNT] );
		m_Connectors.AddTail( Connectors[Counter] );
		if( bSelectInsert )
			SelectElement( Connectors[Counter] );
	}

	if( ConnectorCount != 2 )
		bActuator = false;

	pLink->SetLayers( Layers );
	pLink->SetActuator( bActuator );
	pLink->SetCPM( DEFAULT_RPM );
	pLink->SetSolid( bSolid );
	pLink->SetGear( bGear );
	pLink->SetColor( ( Layers & DRAWINGLAYER ) != 0 ? RGB( 200, 200, 200 ) : Colors[pLink->GetIdentifier() % COLORS_COUNT] );
	pLink->SetMeasurementElement( bMeasurement && ( Layers & DRAWINGLAYER ) != 0 );
	if( bSelectInsert )
		SelectElement( pLink );

	SetSelectedModifiableCondition();

    SetModifiedFlag( true );

	FinishChangeSelected();

	delete [] Connectors;
}

void CLinkageDoc::DeleteContents( bool bDeleteUndoInfo )
{
	POSITION Position = m_GearConnectionList.GetHeadPosition();
	while( Position != 0 )
	{
		CGearConnection *pGearConnection = m_GearConnectionList.GetNext( Position );
		if( pGearConnection != 0 )
			delete pGearConnection;
	}
	m_GearConnectionList.RemoveAll();

	Position = m_Connectors.GetHeadPosition();
	while( Position != NULL )
	{
		CConnector* pConnector = m_Connectors.GetNext( Position );
		if( pConnector == 0 )
			continue;
		delete pConnector;
	}
	m_Connectors.RemoveAll();

	Position = m_Links.GetHeadPosition();
	while( Position != NULL )
	{
		CLink* pLink = m_Links.GetNext( Position );
		if( pLink == 0 )
			continue;
		delete pLink;
	}
	m_Links.RemoveAll();

	// This, and even the code to delete gear connections in one of the loops above, is redundant. Better safe than sorry.
	Position = m_GearConnectionList.GetHeadPosition();
	while( Position != 0 )
	{
		CGearConnection *pGearConnection = m_GearConnectionList.GetNext( Position );
		if( pGearConnection == 0 )
			continue;
		delete pGearConnection;
	}
	m_GearConnectionList.RemoveAll();

	if( bDeleteUndoInfo )
	{
		while( m_pUndoList != 0 )
		{
			CUndoRecord *pNext = m_pUndoList->m_pNext;
			delete m_pUndoList;
			m_pUndoList = pNext;
		}
		m_UndoCount = 0;
		m_pUndoListEnd = 0;
	}

	m_pCapturedConnector = 0;
	m_SelectedConnectors.RemoveAll();

	m_IdentifiedConnectors.SetLength( 0 );
	m_IdentifiedLinks.SetLength( 0 );
	m_HighestConnectorID = -1;
	m_HighestLinkID = -1;
}

void CLinkageDoc::DeleteContents()
{
	DeleteContents( true );
	CDocument::DeleteContents();
}

void CLinkageDoc::SelectAll( void )
{
	POSITION Position = m_Connectors.GetHeadPosition();
	while( Position != NULL )
	{
		CConnector* pConnector = m_Connectors.GetNext( Position );
		if( pConnector == 0 || ( pConnector->GetLayers() & m_EditLayers ) == 0 )
			continue;
		SelectElement( pConnector );
	}
	Position = m_Links.GetHeadPosition();
	while( Position != NULL )
	{
		CLink* pLink = m_Links.GetNext( Position );
		if( pLink == 0 || ( pLink->GetLayers() & m_EditLayers ) == 0 )
			continue;
		SelectElement( pLink );
	}
}

void CLinkageDoc::SplitSelected( void )
{
	PushUndo();

	int SplitCount = 0;

	POSITION Position = m_Connectors.GetHeadPosition();
	while( Position != NULL )
	{
		POSITION DeletePosition = Position;
		CConnector* pConnector = m_Connectors.GetNext( Position );
		if( pConnector == 0 || !pConnector->IsSelected() )
			continue;

		CFPoint Point = pConnector->GetOriginalPoint();

		if( pConnector->IsSlider() )
		{
			// Remove the slid limits when splitting a slider. A second split might
			// be needed to actually split this into two connectors if possible.
			pConnector->SlideBetween();
			++SplitCount;
			continue;
		}

		if( pConnector->GetLinkCount() <= 1 )
			continue;

		// Deal with all Links that are using this connector.
		// Remove this connector from the Link and then add
		// a new connector.

		int SplitLinkCount = 0;
		POSITION Position2 = pConnector->GetLinkList()->GetHeadPosition();
		while( Position2 != 0 )
		{
			CLink *pLink = pConnector->GetLinkList()->GetNext( Position2 );
			if( pLink == 0 )
				continue;

			++SplitLinkCount;
		}

		if( SplitLinkCount > 1 )
		{
			Position2 = pConnector->GetLinkList()->GetHeadPosition();
			while( Position2 != 0 )
			{
				CLink *pLink = pConnector->GetLinkList()->GetNext( Position2 );
				if( pLink == 0 )
					continue;

				CConnector *pNewConnector = new CConnector( *pConnector );
				if( pNewConnector == 0 )
					continue;

				pLink->RemoveConnector( pConnector );
				pLink->AddConnector( pNewConnector );
				pNewConnector->AddLink( pLink );
				int NewID = m_IdentifiedConnectors.FindAndSetBit();
				if( NewID > m_HighestConnectorID )
					m_HighestConnectorID = NewID;
				pNewConnector->SetIdentifier( NewID );
				m_Connectors.AddTail( pNewConnector );
				if( pConnector->IsSelected() )
					SelectElement( pNewConnector );

//				pNewConnector->SetLayers( pConnector->GetLayers() );
//				pNewConnector->SetAsAnchor( pConnector->IsAnchor() );
//				pNewConnector->SetAsInput( pConnector->IsInput() );
//				pNewConnector->SetRPM( pConnector->GetRPM() );
//				pNewConnector->SetPoint( Point );

				SetModifiedFlag( true );
				++SplitCount;
			}

			RemoveGearRatio( pConnector, 0 );
			pConnector->RemoveAllLinks();
			DeleteConnector( pConnector );
		}
	}

	if( SplitCount > 0 )
	{
		NormalizeConnectorLinks();
		FixupSliderLocations();
		FinishChangeSelected();
	}
	else
		PopUndoDelete();
}

void CLinkageDoc::Copy( bool bCut )
{
    CMemFile mFile;
    CArchive ar( &mFile,CArchive::store );
    WriteOut( ar, true );
    ar.Write( "\0", 1 );
    ar.Close();

    int Size = (int)mFile.GetLength();
    BYTE* pData = mFile.Detach();

    UINT CF_Linkage = RegisterClipboardFormat( "RECTORSQUID_Linkage_CLIPBOARD_XML_FORMAT" );
    CWnd *pWnd = AfxGetMainWnd();
    ::OpenClipboard( pWnd == 0 ? 0 : pWnd->GetSafeHwnd() );
    ::EmptyClipboard();
    HANDLE hTextMemory = ::GlobalAlloc( NULL, Size );
    HANDLE hMemory = ::GlobalAlloc( NULL, Size );
    char *pTextMemory = (char*)::GlobalLock( hTextMemory );
    char *pMemory = (char*)::GlobalLock( hMemory );

    memcpy( pTextMemory, pData, Size );
    memcpy( pMemory, pData, Size );

    ::GlobalUnlock( hTextMemory );
    ::GlobalUnlock( hMemory );
    ::SetClipboardData( CF_TEXT, hTextMemory );
    ::SetClipboardData( CF_Linkage, hMemory );
    ::CloseClipboard();

    if( bCut )
		DeleteSelected();
}

void CLinkageDoc::Paste( void )
{
    UINT CF_Linkage = RegisterClipboardFormat( "RECTORSQUID_Linkage_CLIPBOARD_XML_FORMAT" );
    if( ::IsClipboardFormatAvailable( CF_Linkage ) == 0 )
		return;
    CWnd *pWnd = AfxGetMainWnd();
    if( !::OpenClipboard( pWnd == 0 ? 0 : pWnd->GetSafeHwnd() ) )
		return;
    HANDLE hMemory = ::GetClipboardData( CF_Linkage );
    if( hMemory == 0 )
    {
		::CloseClipboard();
		return;
    }
    BYTE *pMemory = (BYTE*)GlobalLock( hMemory );
    if( pMemory == 0 )
    {
		GlobalUnlock( hMemory );
		::CloseClipboard();
		return;
    }

	SelectElement();

    /*
     * There is  no way to query how much data is available on the clipboard.
     * The application (us) is responsible for figuring it out based on the
     * clipboard format. In our case, the data is a null terminated string
     * a strlen will give us the size. it is not a comfortable operation
     * since someone could write data in "our" format but leave off the
     * terminating null.
     */
    int Size = strlen( (char*)pMemory );
    if( Size == 0 )
    {
		GlobalUnlock( hMemory );
		::CloseClipboard();
		return;
    }

    CMemFile mFile;
    mFile.Attach( pMemory, Size, 0 );
    CArchive ar( &mFile, CArchive::load );

	CFrameWnd *pFrame = (CFrameWnd*)AfxGetMainWnd();
	CLinkageView *pView = pFrame == 0 ? 0 : (CLinkageView*)pFrame->GetActiveView();
	if( pView != 0 )
		pView->HighlightSelected( true );

    PushUndo();

    ReadIn( ar, true, false, false, true );
    mFile.Detach();
    ::GlobalUnlock( hMemory );
    ::CloseClipboard();

    SetModifiedFlag( true );
}

void CLinkageDoc::SelectSample( int Index )
{
	CSampleGallery GalleryData;

	CString ExampleData;
	CString ExampleName;
	ExampleData = GalleryData.GetXMLData( Index );
	ExampleName.LoadString( GalleryData.GetStringID( Index ) );

	int EOL = ExampleName.Find( '\n' );
	if( EOL >= 0 )
		ExampleName = ExampleName.Mid( EOL + 1 );

	ExampleName += " Example";

	if (!SaveModified())
		return;

	OnNewDocument();

	SetTitle( ExampleName );

    CMemFile mFile;
    mFile.Attach( (BYTE*)ExampleData.GetBuffer(), ExampleData.GetLength(), 0 );
    CArchive ar( &mFile, CArchive::load );

    ReadIn( ar, false, true, false, false );
    mFile.Detach();
    ExampleData.ReleaseBuffer();
    UpdateAllViews( 0 );
}

void CLinkageDoc::GetDocumentArea( CFRect &BoundingRect, bool bSelectedOnly )
{
	BoundingRect.SetRect( DBL_MAX, -DBL_MAX, -DBL_MAX, DBL_MAX );
	POSITION Position = m_Links.GetHeadPosition();
	CFRect Rect;
	while( Position != 0 )
	{
		CLink* pLink = m_Links.GetNext( Position );
		if( pLink == 0 )
			continue;
		if( bSelectedOnly && !pLink->IsSelected() )
			continue;
		pLink->GetArea( m_GearConnectionList, Rect );
		BoundingRect.left = min( BoundingRect.left, Rect.left );
		BoundingRect.right = max( BoundingRect.right, Rect.right );
		BoundingRect.top = max( BoundingRect.top, Rect.top );
		BoundingRect.bottom = min( BoundingRect.bottom, Rect.bottom );
	}

	Position = m_Connectors.GetHeadPosition();
	while( Position != 0 )
	{
		CConnector *pConnector = m_Connectors.GetNext( Position );
		if( pConnector == 0 || !pConnector->IsSelected() )
			continue;
		pConnector->GetArea( Rect );
		BoundingRect.left = min( BoundingRect.left, Rect.left );
		BoundingRect.right = max( BoundingRect.right, Rect.right );
		BoundingRect.top = max( BoundingRect.top, Rect.top );
		BoundingRect.bottom = min( BoundingRect.bottom, Rect.bottom );
	}

	if( BoundingRect.left == DBL_MAX
		|| BoundingRect.top == -DBL_MAX
		|| BoundingRect.bottom == DBL_MAX
		|| BoundingRect.right == -DBL_MAX )
	BoundingRect.SetRect( 0, 0, 0, 0 );
}

void CLinkageDoc::GetDocumentAdjustArea( CFRect &BoundingRect, bool bSelectedOnly )
{
	BoundingRect.SetRect( DBL_MAX, -DBL_MAX, -DBL_MAX, DBL_MAX );
	POSITION Position = m_Links.GetHeadPosition();
	CFRect Rect;
	while( Position != 0 )
	{
		CLink* pLink = m_Links.GetNext( Position );
		if( pLink == 0 )
			continue;
		if( bSelectedOnly && !pLink->IsSelected() )
			continue;
		pLink->GetAdjustArea( m_GearConnectionList, Rect );
		BoundingRect.left = min( BoundingRect.left, Rect.left );
		BoundingRect.right = max( BoundingRect.right, Rect.right );
		BoundingRect.top = max( BoundingRect.top, Rect.top );
		BoundingRect.bottom = min( BoundingRect.bottom, Rect.bottom );
	}

	if( bSelectedOnly )
	{
		// If checking selected elements then check individual connectors.
		POSITION Position = m_Connectors.GetHeadPosition();
		while( Position != 0 )
		{
			CConnector *pConnector = m_Connectors.GetNext( Position );
			if( pConnector == 0 || !pConnector->IsSelected() )
				continue;
			pConnector->GetAdjustArea( Rect );
			BoundingRect.left = min( BoundingRect.left, Rect.left );
			BoundingRect.right = max( BoundingRect.right, Rect.right );
			BoundingRect.top = max( BoundingRect.top, Rect.top );
			BoundingRect.bottom = min( BoundingRect.bottom, Rect.bottom );
		}
	}

	if( BoundingRect.left == DBL_MAX
		|| BoundingRect.top == -DBL_MAX
		|| BoundingRect.bottom == DBL_MAX
		|| BoundingRect.right == -DBL_MAX )
	BoundingRect.SetRect( 0, 0, 0, 0 );
}

bool CLinkageDoc::IsAnySelected( void )
{
	return GetSelectedConnectorCount() + GetSelectedLinkCount( false ) > 0;
}

bool CLinkageDoc::IsSelectionAdjustable( void )
{
	int ConnectorCount = GetSelectedConnectorCount();
	int LinkCount = GetSelectedLinkCount( false );

	if( ConnectorCount > 1 || LinkCount > 1 )
		return true;

	if( LinkCount == 1 )
	{
		// If it has more than a single connector then it can be adjusted.
		POSITION Position = m_Links.GetHeadPosition();
		while( Position != NULL )
		{
			CLink* pLink = m_Links.GetNext( Position );
			if( pLink == 0 || !pLink->IsSelected() )
				continue;

			return pLink->GetConnectorCount() > 1;
		}
	}

	return false;
}

CUndoRecord::CUndoRecord( BYTE *pData )
{
	m_pData = pData;
}

CUndoRecord::~CUndoRecord()
{
	if( m_pData != 0 )
		delete [] m_pData;
	m_pData = 0;
}

void CUndoRecord::ClearContent( BYTE **ppData )
{
	if( ppData == 0 )
		return;
	*ppData = m_pData;
	m_pData = 0;
}

void CLinkageDoc::PushUndo( void )
{
	static const int MAX_UNDO = 300;

    CMemFile mFile;
    CArchive ar( &mFile,CArchive::store );
    WriteOut( ar, false );
    ar.Write( "\0", 1 );
    ar.Close();

    int Size = (int)mFile.GetLength();
    BYTE* pData = mFile.Detach();

	CUndoRecord *pNewRecord = new CUndoRecord( pData );
	if( pNewRecord == 0 )
		return;

	++m_UndoCount;

	if( m_UndoCount > MAX_UNDO )
	{
		// Delete the tail item in the list.
		if( m_pUndoListEnd != 0 )
		{
			CUndoRecord *pSecondToLast = m_pUndoListEnd->m_pPrevious;
			if( pSecondToLast != 0 )
			{
				pSecondToLast->m_pNext = 0;
				delete m_pUndoListEnd;
				m_pUndoListEnd = pSecondToLast;
				--m_UndoCount;
			}
		}
	}

	if( m_pUndoList != 0 )
		m_pUndoList->m_pPrevious = pNewRecord;

	pNewRecord->m_pNext = m_pUndoList;
	m_pUndoList = pNewRecord;

	if( m_pUndoListEnd == 0 )
		m_pUndoListEnd = pNewRecord;

    SetModifiedFlag( true );
}

void CLinkageDoc::PopUndo( void )
{
	if( m_UndoCount == 0 || m_pUndoList == 0 )
		return;

	DeleteContents( false );

	BYTE *pData = 0;
	m_pUndoList->ClearContent( &pData );

    CMemFile mFile;
    mFile.Attach( pData, strlen( (char*)pData ), 0 );
    CArchive ar( &mFile, CArchive::load );

    ReadIn( ar, false, false, true, false );
    mFile.Detach();

	PopUndoDelete();

    SetModifiedFlag( true );

    UpdateAllViews( 0 );
}

void CLinkageDoc::PopUndoDelete( void )
{
	CUndoRecord *pNewUndoList = m_pUndoList->m_pNext;
	delete m_pUndoList;
	m_pUndoList = pNewUndoList;
	--m_UndoCount;

	if( m_pUndoList == 0 )
	{
		m_UndoCount = 0; // Make sure this is correct now.
		m_pUndoListEnd = 0;
	}
}

bool CLinkageDoc::Undo( void )
{
	bool bResult = m_UndoCount > 0 && m_pUndoList != 0;

	PopUndo();
//	m_pCapturedConnector = 0;
//	m_SelectedConnectors.RemoveAll();

	// Set the identifier bits in the new document.
	POSITION Position = m_Connectors.GetHeadPosition();
	while( Position != 0 )
	{
		CConnector *pConnector = m_Connectors.GetNext( Position );
		if( pConnector == 0 )
			continue;
		int NewID = pConnector->GetIdentifier();
		if( NewID > m_HighestConnectorID )
			m_HighestConnectorID = NewID;
		m_IdentifiedConnectors.SetBit( NewID );
	}

	Position = m_Links.GetHeadPosition();
	while( Position != 0 )
	{
		CLink *pLink = m_Links.GetNext( Position );
		if( pLink == 0 )
			continue;
		int NewID = pLink->GetIdentifier();
		if( NewID > m_HighestLinkID )
			m_HighestLinkID = NewID;
		m_IdentifiedLinks.SetBit( NewID );
	}

	return bResult;
}

bool CLinkageDoc::ConnectSliderLimits( bool bTestOnly )
{
	if( !bTestOnly )
		PushUndo();

	if( GetSelectedConnectorCount() != 3 )
		return false;

	CConnector *pUseConnectors[3] = { 0, 0, 0 };

	int Index = 0;
	POSITION Position = m_Connectors.GetHeadPosition();
	while( Position != NULL )
	{
		CConnector* pConnector = m_Connectors.GetNext( Position );
		if( pConnector == 0 || !pConnector->IsSelected() )
			continue;

		pUseConnectors[Index++] = pConnector;
	}

	int FirstLimit = -1;
	int SecondLimit = -1;
	int Slider = -1;

	// Make sure that two are on the same link and the other is not.
	int ShareCount = 0;
	if( pUseConnectors[0]->IsSharingLink( pUseConnectors[1] ) != 0 )
	{
		++ShareCount;
		FirstLimit = 0;
		SecondLimit = 1;
		Slider = 2;
	}
	if( pUseConnectors[0]->IsSharingLink( pUseConnectors[2] ) != 0 )
	{
		++ShareCount;
		FirstLimit = 0;
		SecondLimit = 2;
		Slider = 1;
	}
	if( pUseConnectors[1]->IsSharingLink( pUseConnectors[2] ) != 0 )
	{
		++ShareCount;
		FirstLimit = 1;
		SecondLimit = 2;
		Slider = 0;
	}

	if( ShareCount != 1 )
		return false;

	if( bTestOnly )
		return true;

	pUseConnectors[Slider]->SlideBetween( pUseConnectors[FirstLimit], pUseConnectors[SecondLimit] );

	// Test code... pUseConnectors[Slider]->SetSlideRadius( 400 );

	pUseConnectors[Slider]->SetPoint( pUseConnectors[FirstLimit]->GetPoint().MidPoint( pUseConnectors[SecondLimit]->GetPoint(), .5 ) );

	Position = pUseConnectors[Slider]->GetLinkList()->GetHeadPosition();
	while( Position != 0 )
	{
		CLink *pLink = pUseConnectors[Slider]->GetLinkList()->GetNext( Position );
		if( pLink == 0 )
			continue;
		pLink->UpdateController();
	}

    SetModifiedFlag( true );

	FinishChangeSelected();

    return true;
}

static unsigned int NumberOfSetBits( unsigned int Value )
{
    Value = Value - ((Value >> 1) & 0x55555555);
    Value = (Value & 0x33333333) + ((Value >> 2) & 0x33333333);
    return (((Value + (Value >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
}

void CLinkageDoc::SetSelectedModifiableCondition( void )
{
	int SelectedRealLinks = 0;
	int SelectedLinks = 0;
	int SelectedConnectors = 0;
	int SelectedMechanismLinks = 0;
	int SelectedDrawingElements = 0;
	int Actuators = 0;
	int Sliders = 0;
	int SelectedGears = 0;
	int FastenedCount = 0;
	int GearFastenToCount = 0;

	CConnector *pFastenToConnector = 0;

	m_SelectedLayers = 0;

	POSITION Position = m_Connectors.GetHeadPosition();
	while( Position != 0 )
	{
		CConnector *pConnector = m_Connectors.GetNext( Position );
		if( pConnector == 0 )
			continue;
		if( pConnector->IsSelected() )
		{
			m_SelectedLayers |= pConnector->GetLayers();
			++SelectedConnectors;
			if( pConnector->IsSlider() )
				++Sliders;
			if( pConnector->GetLayers() & DRAWINGLAYER )
				++SelectedDrawingElements;
			if( pConnector->GetFastenedTo() != 0 )
				++FastenedCount;

			// This is tentative because I dont know if people will expect ALL fastened elements to become unfastened
			// from this element, or if they only expect to use the unfasten action when a fastened thing is selected.
			if( pConnector->GetFastenedElementList()->GetCount() > 0 )
				++FastenedCount;

			if( pConnector->IsAnchor() && pConnector->IsAnchor() && !pConnector->IsInput() )
			{
				++GearFastenToCount;
				pFastenToConnector = pConnector;
			}
		}
	}

	CLink *pFastenToLink = 0;

	Position = m_Links.GetHeadPosition();
	while( Position != 0 )
	{
		CLink *pLink = m_Links.GetNext( Position );
		if( pLink == 0 )
			continue;
		if( pLink->IsSelected() )
		{
			++SelectedLinks;

			if( pLink->GetConnectorCount() > 1 || pLink->IsGear() ) // Lone connectors don't count as real links!
				++SelectedRealLinks;
			if( pLink->IsActuator() )
				++Actuators;
			if( pLink->IsGear() )
				++SelectedGears;
			m_SelectedLayers |= pLink->GetLayers();
			if( pLink->GetLayers() & MECHANISMLAYERS )
			{
				++SelectedMechanismLinks;
				if( !pLink->IsGear() && pLink->GetConnectorCount() > 1 )
					pFastenToLink = pLink;
			}
			if( pLink->GetLayers() & DRAWINGLAYER )
				++SelectedDrawingElements;
			if( pLink->GetFastenedTo() != 0 )
				++FastenedCount;
			// This is tentative because I dont know if people will expect ALL fastened elements to become unfastened
			// from this element, or if they only expect to use the unfasten action when a fastened thing is selected.
			if( pLink->GetFastenedElementList()->GetCount() > 0 )
				++FastenedCount;
			if( !pLink->IsGear() && pLink->GetConnectorCount() > 1 )
				++GearFastenToCount;
		}
	}

	CConnector* pConnector1 = GetSelectedConnector( 0 );

	if( NumberOfSetBits( m_SelectedLayers ) > 1 )
	{
		m_bSelectionCombinable = false;
		m_bSelectionConnectable = false;
		m_bSelectionJoinable = false;
		m_bSelectionSlideable = false;
		m_bSelectionLockable = false;
		m_bSelectionMakeAnchor = false;
	}
	else
	{
		CConnector* pConnector2 = GetSelectedConnector( 1 );
		m_bSelectionCombinable = SelectedLinks > 1 && Actuators == 0 && SelectedGears == 0;
		m_bSelectionConnectable = SelectedRealLinks == 0 && SelectedConnectors == 2 && pConnector1->GetSharingLink( pConnector2 ) == 0 && !pConnector1->IsSlider();
		m_bSelectionJoinable = ( SelectedConnectors > 1 && Sliders <= 1 ) || ( SelectedConnectors == 1 && SelectedRealLinks >= 1 );
		m_bSelectionSlideable = ConnectSliderLimits( true );
		m_bSelectionLockable = SelectedConnectors == 0 && SelectedRealLinks > 0;
		m_bSelectionMakeAnchor = SelectedConnectors > 0 && SelectedRealLinks == 0;
	}

	m_bSelectionSplittable = SelectedRealLinks == 0 && SelectedConnectors == 1;
	if( m_bSelectionSplittable )
	{
		if (pConnector1->GetLinkCount() <= 1 && !pConnector1->IsSlider() )
			m_bSelectionSplittable = false;
	}

	m_bSelectionTriangle = SelectedRealLinks == 0 && SelectedConnectors == 3;
	m_bSelectionRectangle = SelectedRealLinks == 0 && SelectedConnectors == 4;
	m_bSelectionLineable = SelectedRealLinks == 0 && SelectedConnectors >= 3;
	m_AlignConnectorCount = SelectedRealLinks > 0 ? 0 : SelectedConnectors;
	m_bSelectionUnfastenable = FastenedCount > 0;

	m_bSelectionFastenable = false;
	if( SelectedDrawingElements > 0 && SelectedMechanismLinks == 1 )
		m_bSelectionFastenable = true;
	else if( SelectedGears >= 1 && ( SelectedMechanismLinks - SelectedGears ) == 1 && GearFastenToCount == 1 )
	{
		m_bSelectionFastenable = true;
		Position = m_Links.GetHeadPosition();
		while( Position != 0 && ( pFastenToLink != 0 || pFastenToConnector != 0 ) )
		{
			CLink *pLink = m_Links.GetNext( Position );
			if( pLink == 0 )
				continue;
			if( pLink->IsSelected() && pLink->IsGear() )
			{
				if( pFastenToLink != 0 )
				{
					CConnector *pCommon = CLink::GetCommonConnector( pFastenToLink, pLink );
					if( pCommon == 0 )
					{
						m_bSelectionFastenable = false;
						break;
					}
				}
				if( pFastenToConnector != 0 )
				{
					if( !pLink->GetGearConnector()->IsAnchor() )
					{
						m_bSelectionFastenable = false;
						break;
					}
				}
			}
		}
	}

	m_bSelectionMeshable = CheckMeshableGears();
}

CConnector* CLinkageDoc::GetSelectedConnector( int Index )
{
	POSITION Position = m_SelectedConnectors.GetTailPosition();
	while( Position != 0 )
	{
		CConnector *pConnector = m_SelectedConnectors.GetPrev( Position );
		if( Index == 0 )
			return pConnector;
		--Index;
	}
	return 0;
}

void CLinkageDoc::GetNextAutoSelectable( _SelectDirection SelectDirection, POSITION &Position, bool &bIsConnector )
{
	if( SelectDirection == PREVIOUS )
	{
		if( bIsConnector )
		{
			m_Connectors.GetPrev( Position );
			if( Position == 0 )
			{
				Position = m_Links.GetTailPosition();
				bIsConnector = false;
				if( Position == 0 )
				{
					Position = m_Connectors.GetTailPosition();
					bIsConnector = true;
				}
			}
		}
		else
		{
			m_Links.GetPrev( Position );
			if( Position == 0 )
			{
				Position = m_Connectors.GetTailPosition();
				bIsConnector = true;
				if( Position == 0 )
				{
					Position = m_Links.GetTailPosition();
					bIsConnector = false;
				}
			}
		}
	}
	else
	{
		if( bIsConnector )
		{
			m_Connectors.GetNext( Position );
			if( Position == 0 )
			{
				Position = m_Links.GetHeadPosition();
				bIsConnector = false;
				if( Position == 0 )
				{
					Position = m_Connectors.GetHeadPosition();
					bIsConnector = true;
				}
			}
		}
		else
		{
			m_Links.GetNext( Position );
			if( Position == 0 )
			{
				Position = m_Connectors.GetHeadPosition();
				bIsConnector = true;
				if( Position == 0 )
				{
					Position = m_Links.GetHeadPosition();
					bIsConnector = false;
				}
			}
		}
	}
}

bool CLinkageDoc::SelectNext( _SelectDirection SelectDirection )
{
	POSITION Position = 0;
	POSITION LastSelection = 0;
	bool bLastIsConnector = 0;

	/*
	 * Find the last selected item, be it a connector or a link.
	 * This will be the item that is used for picking the next or
	 * previous selected item.
	 */

	Position = m_Connectors.GetHeadPosition();
	while( Position != 0 )
	{
		CConnector *pConnector = m_Connectors.GetAt( Position );
		if( pConnector != 0 && pConnector->IsSelected() )
		{
			LastSelection = Position;
			bLastIsConnector = true;
		}
		m_Connectors.GetNext( Position );
	}

	Position = m_Links.GetHeadPosition();
	while( Position != 0 )
	{
		CLink *pLink = m_Links.GetAt( Position );
		if( pLink != 0 && pLink->IsSelected( false ) )
		{
			LastSelection = Position;
			bLastIsConnector = false;
		}
		m_Links.GetNext( Position );
	}

	Position = LastSelection;

	if( Position == 0 )
	{
		Position = m_Connectors.GetHeadPosition();
		if( Position == 0 )
			return false;
		bLastIsConnector = true;
	}
	else
		GetNextAutoSelectable( SelectDirection, Position, bLastIsConnector );

	/*
	 * Skip links that have a single connector. This will get selected
	 * when the connector is selected.
	 */

	/*
		* Unless there are links and zero connectors, this loop is guaranteed
		* to terminate as soon as a connector is picked. It may also
		* terminate if a link with more than one connector is picked.
		*/
	while( Position != 0 && !bLastIsConnector )
	{
		CLink *pLink = m_Links.GetAt( Position );
		if( pLink == 0 )
			break;
		if( pLink->GetConnectorCount() > 1 || pLink->IsGear() )
			break;

		// Find another element since this link has a single connector.
		GetNextAutoSelectable( SelectDirection, Position, bLastIsConnector );
	}

	ClearSelection();

	if( bLastIsConnector )
	{
		CConnector *pConnector = m_Connectors.GetAt( Position );
		if( pConnector == 0 )
			return false;
		SelectElement( pConnector );
	}
	else
	{
		CLink *pLink = m_Links.GetAt( Position );
		if( pLink == 0 )
			return false;
		SelectElement( pLink );
	}

	SetSelectedModifiableCondition();
	return true;
}

#if 0

void CLinkageDoc::InsertXml( CFPoint DesiredPoint, bool bForceToPoint, const char *pXml )
{
    CMemFile mFile;
	unsigned int Size = strlen( pXml );
    mFile.Attach( (BYTE*)pXml, Size, 0 );
    CArchive ar( &mFile, CArchive::load );

    PushUndo();

    ReadIn( ar, true, false, false, false );
    mFile.Detach();

    SetModifiedFlag( true );
}

#endif

void CLinkageDoc::SetUnits( CLinkageDoc::_Units Units )
{
	double OneInch = GetBaseUnitsPerInch();

	m_Units = Units;
	switch( Units )
	{
		case INCH: m_UnitScaling = 1 / OneInch; break;
		case MM:
		default: m_UnitScaling = 24.5 / OneInch; break;
	}
}

CLinkageDoc::_Units CLinkageDoc::GetUnits( void )
{
	return m_Units;
}

bool CLinkageDoc::ChangeLinkLength( CLink *pLink, double Value, bool bPercentage )
{
	CConnector *pConnector = pLink->GetConnector( 0 );
	CConnector *pConnector2 = pLink->GetConnector( 1 );
	if( pConnector == 0 || pConnector2 == 0 )
		return false;
	if( IsLinkLocked( pConnector ) || IsLinkLocked( pConnector2 ) )
		return false;
	CFLine Line( pConnector->GetPoint(), pConnector2->GetPoint() );
	double LineLength = Line.GetDistance();
	double NewLineLength = LineLength;
	if( bPercentage )
		NewLineLength *= Value / 100;
	else
		NewLineLength = Value;
	double OneEndAdd = ( NewLineLength - LineLength ) / 2;
	PushUndo();
	Line.SetDistance( LineLength + OneEndAdd );
	Line.ReverseDirection();
	Line.SetDistance( NewLineLength );
	pConnector->SetPoint( Line.GetEnd() );
	pConnector2->SetPoint( Line.GetStart() );
	return true;
}

bool CLinkageDoc::SetSelectedElementCoordinates( const char *pCoordinateString )
{
	CString Text = pCoordinateString;

	Text.Replace( " ", "" );

	double xValue = 0.0;
	double yValue = 0.0;
	char Dummy1;
	char PercentSign;
	int CoordinateCount = 0;
	double Percentage = 0.0;

	int SelectedConnectorCount = GetSelectedConnectorCount();
	int SelectedLinkCount = GetSelectedLinkCount( true );
	CConnector *pConnector = (CConnector*)GetSelectedConnector( 0 );

	CoordinateCount = sscanf_s( (const char*)Text, "%lf %c%c", &Percentage, &PercentSign, 1, &Dummy1, 1 );
	if( CoordinateCount == 2 && PercentSign == '%' )
	{
		if( SelectedConnectorCount == 2 && SelectedLinkCount == 0 )
		{
			// Distance
			CConnector *pConnector2 = (CConnector*)GetSelectedConnector( 1 );
			if( pConnector == 0 || pConnector2 == 0 )
				return false;
			if( IsLinkLocked( pConnector ) )
				return false;
			CFLine Line( pConnector->GetPoint(), pConnector2->GetPoint() );
			Line.SetDistance( Line.GetDistance() * Percentage / 100 );
			PushUndo();
			pConnector2->SetPoint( Line.GetEnd() );
		}
		else if( SelectedConnectorCount == 0 && SelectedLinkCount == 1 )
		{
			CLink *pLink = GetSelectedLink( 0, false );
			if( pLink != 0 && pLink->GetConnectorCount() == 2 && !pLink->IsLocked() )
			{
				if( ChangeLinkLength( pLink, Percentage, true ) )
					return true;
			}
		}
		else
		{
			// Scale
			StretchSelected( Percentage );
		}
		return true;
	}

	CoordinateCount = sscanf_s( (const char*)Text, "%lf , %lf%c", &xValue, &yValue, &Dummy1, 1 );

	if( CoordinateCount < 1 || CoordinateCount > 2 )
		return false;

	double Angle = xValue;

	double DocumentScale = GetUnitScale();
	xValue /= DocumentScale;
	yValue /= DocumentScale;

	if( SelectedLinkCount > 0 )
	{
		if( SelectedLinkCount == 1 && CoordinateCount == 1 )
		{
			CLink *pLink = GetSelectedLink( 0, false );
			if( pLink == 0 || pLink->GetConnectorCount() != 2 || pLink->IsLocked() )
				return false;

			return ChangeLinkLength( pLink, xValue, false );
		}
		return false;
	}

	int ExpectedCoordinateCount = SelectedConnectorCount == 1 ? 2 : 1;

	if( CoordinateCount != ExpectedCoordinateCount )
		return false;

	if( CoordinateCount == 2 )
	{
		if( IsLinkLocked( pConnector ) )
			return false;
		PushUndo();
		pConnector->SetPoint( CFPoint( xValue, yValue ) );
		return true;
	}
	else if( SelectedConnectorCount == 2 )
	{
		// Distance
		CConnector *pConnector2 = (CConnector*)GetSelectedConnector( 1 );
		if( pConnector2 == 0 )
			return false;
		if( IsLinkLocked( pConnector2 ) )
			return false;
		CFLine Line( pConnector->GetPoint(), pConnector2->GetPoint() );
		Line.SetDistance( xValue );
		PushUndo();
		pConnector2->SetPoint( Line.GetEnd() );
		return true;
	}
	else if( SelectedConnectorCount == 3 )
	{
		// Angle
		CConnector *pConnector2 = GetSelectedConnector( 2 );
		if( pConnector2 != 0 && pConnector2->IsSlider() )
			return false;
		if( IsLinkLocked( pConnector2 ) )
			return false;

		PushUndo();
		MakeSelectedAtAngle( Angle );
		return true;
	}

	return false;
}

CLink *CLinkageDoc::GetSelectedLink( int Index, bool bOnlyWithMultipleConnectors )
{
	int Counter = 0;
	POSITION Position = m_SelectedLinks.GetTailPosition();
	while( Position != 0 )
	{
		CLink *pLink = m_SelectedLinks.GetPrev( Position );
		if( pLink == 0 || ( !pLink->IsSelected( false ) && !pLink->IsSelected( true ) ) )
			continue;
		if( bOnlyWithMultipleConnectors && pLink->GetConnectorCount() == 1 )
			continue;
		if( Counter == Index )
			return pLink;
		++Counter;
	}
	return 0;
}

bool CLinkageDoc::AddConnectorToSelected( double Offset )
{
	if( !CanAddConnector() )
		return false;

	CLink* pLink = GetSelectedLink( 0, false );
	if( pLink == 0 )
	{
		CConnector *pConnector = GetSelectedConnector( 0 );
		if( pConnector == 0 )
			return false;

		if( pConnector->IsAlone() )
		{
			pLink = pConnector->GetLink( 0 );
			if( pLink == 0 )
				return false;
		}
	}

	CFPoint Point;
	pLink->GetAveragePoint( m_GearConnectionList, Point );
	CFRect Rect;
	pLink->GetAdjustArea( m_GearConnectionList, Rect );

	Point.x = Rect.right + Offset;

	CConnector *pConnector = new CConnector;
	if( pConnector == 0 )
		return false;

	pConnector->SetLayers( pLink->GetLayers() );
	pConnector->SetAsAnchor( false );
	pConnector->SetAsInput( false );
	pConnector->SetRPM( DEFAULT_RPM );
	pConnector->SetPoint( Point );
	pConnector->AddLink( pLink );
	pLink->AddConnector( pConnector );

	int NewID = m_IdentifiedConnectors.FindAndSetBit();
	if( NewID > m_HighestConnectorID )
		m_HighestConnectorID = NewID;
	pConnector->SetIdentifier( NewID );
	m_Connectors.AddTail( pConnector );

	return true;
}

bool CLinkageDoc::CanAddConnector( void )
{
	if (GetSelectedLinkCount(false) == 0)
	{
		if( GetSelectedConnector(0) == 0 )
			return false;
		return GetSelectedConnectorCount() == 1 && GetSelectedConnector(0)->IsAlone() /*&& (GetSelectedConnector(0)->GetLayers() & MECHANISMLAYERS) != 0*/;
	}
	else if( GetSelectedLinkCount( false ) == 1 )
	{
		if( GetSelectedLink( 0, false )->IsActuator() )
			return false;
		return /* ( GetSelectedLink( 0, false )->GetLayers() & MECHANISMLAYERS ) != 0 && */ GetSelectedConnectorCount() == 0 || ( GetSelectedConnectorCount() == 1 && GetSelectedConnector( 0 )->IsAlone() );
	}
	else
		return false;
}

bool CLinkageDoc::UnfastenSelected( void )
{
	PushUndo();

	POSITION Position = m_Links.GetHeadPosition();
	while( Position != 0 )
	{
		CLink *pLink = m_Links.GetNext( Position );
		if( pLink == 0 || !pLink->IsSelected() )
			continue;

		Unfasten( pLink );
	}

	Position = m_Connectors.GetHeadPosition();
	while( Position != NULL )
	{
		CConnector* pConnector = m_Connectors.GetNext( Position );
		if( pConnector == 0 || ( pConnector->GetLayers() & MECHANISMLAYERS ) != 0 || !pConnector->IsSelected() )
			continue;

		Unfasten( pConnector );

		//CLink *pFastenedTo = pConnector->GetFastenedTo() == 0 ? 0 : pConnector->GetFastenedTo()->GetLink();
		//if( pFastenedTo != 0 )
		//	pFastenedTo->RemoveFastenElement( pConnector );

		//pConnector->UnfastenTo();
	}

	SetSelectedModifiableCondition();

	return true;
}

bool CLinkageDoc::Unfasten( CElement *pElement )
{
	POSITION Position = pElement->GetFastenedElementList()->GetHeadPosition();
	while( Position != 0 )
	{
		CElementItem *pItem = pElement->GetFastenedElementList()->GetNext( Position );
		if( pItem == 0 || pItem->GetElement() == 0 )
			continue;
		pItem->GetElement()->RemoveFastenElement( pElement );
		if( pItem->GetElement()->GetFastenedTo() != 0 && pElement == pItem->GetElement()->GetFastenedTo()->GetElement() )
			pItem->GetElement()->UnfastenTo();
	}

	pElement->RemoveFastenElement( 0 ); // All of them.
	pElement->UnfastenTo();

	return true;
}

bool CLinkageDoc::FastenSelected( void )
{
	if( !m_bSelectionFastenable )
		return false;

	bool bFastenDrawingElements = false;
	int DrawingFastenToCount = 0;
	CLink *pDrawingFastenTo = 0;
	CLink *pGearFastenToLink = 0;
	CConnector *pGearFastenToConnector = 0;
	int GearFastenToCount = 0;

	POSITION Position = m_Links.GetHeadPosition();
	while( Position != 0 )
	{
		CLink *pLink = m_Links.GetNext( Position );
		if( pLink == 0 )
			continue;

		if( pLink->IsSelected() )
		{
			if( ( pLink->GetLayers() & DRAWINGLAYER ) != 0 )
				bFastenDrawingElements = true;
			else
			{
				++DrawingFastenToCount;
				pDrawingFastenTo = pLink;

				if( !pLink->IsGear() && pLink->GetConnectorCount() > 1 )
				{
					pGearFastenToLink = pLink;
					++GearFastenToCount;
				}
			}
		}
	}

	Position = m_Connectors.GetHeadPosition();
	while( Position != NULL )
	{
		CConnector* pConnector = m_Connectors.GetNext( Position );
		if( pConnector == 0 || !pConnector->IsSelected() )
			continue;

		if( ( pConnector->GetLayers() & DRAWINGLAYER ) != 0 )
			bFastenDrawingElements = true;
		else
		{
			DrawingFastenToCount += 2; // A single connector it too many to fasten to so just increment this by 2.

			pGearFastenToConnector = pConnector;
			++GearFastenToCount;
		}
	}

	if( bFastenDrawingElements )
	{
		if( DrawingFastenToCount > 1 )
			return false;

		PushUndo();

		Position = m_Links.GetHeadPosition();
		while( Position != 0 )
		{
			CLink *pLink = m_Links.GetNext( Position );
			if( pLink == 0 || !pLink->IsSelected() || ( pLink->GetLayers() & DRAWINGLAYER ) == 0 )
				continue;

			FastenThese( pLink, pDrawingFastenTo );
		}

		Position = m_Connectors.GetHeadPosition();
		while( Position != NULL )
		{
			CConnector* pConnector = m_Connectors.GetNext( Position );
			if( pConnector == 0 || !pConnector->IsSelected() || ( pConnector->GetLayers() &DRAWINGLAYER ) == 0 )
				continue;

			FastenThese( pConnector, pDrawingFastenTo );
		}
	}
	else
	{
		if( GearFastenToCount > 1 )
			return false;

		PushUndo();

		Position = m_Links.GetHeadPosition();
		while( Position != 0 )
		{
			CLink *pLink = m_Links.GetNext( Position );
			if( pLink == 0 || !pLink->IsSelected() || ( pLink->GetLayers() & DRAWINGLAYER ) != 0 || !pLink->IsGear() )
				continue;

			if( pGearFastenToConnector != 0 )
				FastenThese( pLink, pGearFastenToConnector );
			else
				FastenThese( pLink, pGearFastenToLink );
		}
	}

	SetSelectedModifiableCondition();

	return true;
}
bool CLinkageDoc::ConnectGears( CLink *pLink1, CLink *pLink2, double Size1, double Size2 )
{
	if( pLink1 == 0 || !pLink1->IsGear() || pLink2 == 0 || !pLink2->IsGear() || pLink1 == pLink2 )
		return false;

	POSITION Position = m_GearConnectionList.GetHeadPosition();
	while( Position != 0 )
	{
		POSITION CurrentPosition = Position;
		CGearConnection *pConnection = m_GearConnectionList.GetNext( Position );
		if( pConnection == 0 )
			continue;

		if( ( pConnection->m_pGear1 == pLink1 && pConnection->m_pGear2 == pLink2 )
		    || ( pConnection->m_pGear2 == pLink1 && pConnection->m_pGear1 == pLink2 ) )
		{
			delete pConnection;
			m_GearConnectionList.RemoveAt( CurrentPosition );
			break;
		}
	}

	CGearConnection *pConnection = new CGearConnection;
	if( pConnection == 0 )
		return false;

	pConnection->m_pGear1 = pLink1;
	pConnection->m_pGear2 = pLink2;
	pConnection->m_Gear1Size = Size1;
	pConnection->m_Gear2Size = Size2;

	return true;
}

bool CLinkageDoc::DisconnectGear( CLink *pLink )
{
	if( pLink == 0 || !pLink->IsGear() )
		return false;

	POSITION Position = m_GearConnectionList.GetHeadPosition();
	while( Position != 0 )
	{
		POSITION CurrentPosition = Position;
		CGearConnection *pConnection = m_GearConnectionList.GetNext( Position );
		if( pConnection == 0 )
			continue;

		if( pConnection->m_pGear1 == pLink || pConnection->m_pGear2 == pLink )
		{
			delete pConnection;
			m_GearConnectionList.RemoveAt( CurrentPosition );
			break;
		}
	}

	return true;
}

bool CLinkageDoc::CheckMeshableGears( void )
{
	if( GetSelectedLinkCount( false ) != 2 )
		return false;

	CLink *pGear1 = GetSelectedLink( 0, false );
	CLink *pGear2 = GetSelectedLink( 1, false );

	return CheckMeshableGears( pGear1, pGear2 );
}

bool CLinkageDoc::CheckMeshableGears( CLink *pGear1, CLink *pGear2 )
{
	if( pGear1 == 0 || pGear2 == 0 )
		return false;

	if( !pGear1->IsGear() || !pGear2->IsGear() )
		return false;

	CConnector *pConnector1 = pGear1->GetConnector( 0 );
	CConnector *pConnector2 = pGear2->GetConnector( 0 );

	if( pConnector1->IsSharingLink( pConnector2 ) )
		return true;

	if( pConnector1->IsAnchor() && pConnector2->IsAnchor() )
		return true;

	return false;

#if 0

	POSITION Position = m_Connectors.GetHeadPosition();
	while( Position != 0 )
	{
		CConnector *pConnector = m_Connectors.GetNext( Position );
		if( pConnector != 0 && pConnector->IsSelected() )
		{
			bool bIsGear = false;
			CLink *pLink = 0;
			POSITION Position2 = pConnector->GetLinkList()->GetHeadPosition();
			while( Position2 != 0 )
			{
				pLink = pConnector->GetLinkList()->GetNext( Position2 );
				if( pLink == 0 )
					continue;

				if( pConnector == pLink->GetGearConnector() )
				{
					bIsGear = true;
					break;
				}
			}
			if( !bIsGear )
				return false;

			if( pConnector1 == 0 )
				pConnector1 = pConnector;
			else
				pConnector2 = pConnector;
		}
	}

	if( pConnector1 != 0 && pConnector2 != 0 )
		return true;

	/*
	 * There is one special case: If one of the gear connectors is an anchor and the other gear
	 * connector connects to an anchor at that same location, it is safe to mesh the gears.
	 */

	 CConnector *pAnchorConnector = 0;
	 CConnector *pOtherConnector = 0;
	 if( pConnector1->IsAnchor() )
	 {
		pAnchorConnector = pConnector1;
		pOtherConnector = pConnector2;
	 }
	 else if( pConnector2->IsAnchor() )
	 {
		pAnchorConnector = pConnector2;
		pOtherConnector = pConnector1;
	 }
	 else
		return false;

	Position = pOtherConnector->GetLinkList()->GetHeadPosition();
	while( Position != 0 )
	{
		CLink *pLink = pOtherConnector->GetLinkList()->GetNext( Position );
		if( pLink == 0 )
			continue;

		POSITION Position2 = pLink->GetConnectorList()->GetHeadPosition();
		while( Position2 != 0 )
		{
			CConnector *pCheckConnector = pLink->GetConnectorList()->GetNext( Position2 );
			if( pCheckConnector == 0 )
				continue;

			if( pCheckConnector->IsAnchor() && pAnchorConnector->GetPoint() == pCheckConnector->GetPoint() )
				return true;
		}
	}

	return false;
	#endif
}

CGearConnection * CLinkageDoc::GetGearRatio( CLink *pGear1, CLink *pGear2, double *pSize1, double *pSize2 )
{
	if( pGear1 == 0 || pGear2 == 0 || !pGear1->IsGear() || !pGear2->IsGear() )
		return 0;

	POSITION Position = m_GearConnectionList.GetHeadPosition();
	while( Position != 0 )
	{
		CGearConnection *pGearConnection = m_GearConnectionList.GetNext( Position );
		if( pGearConnection == 0 || pGearConnection->m_pGear1 == 0 || pGearConnection->m_pGear2 == 0 )
			continue;

		// Scale the size values if using a chain and gears whose size is set.
		double UseDocumentScale = pGearConnection->m_bUseSizeAsRadius ? GetUnitScale() : 1;

		if( ( pGearConnection->m_pGear1 == pGear1 && pGearConnection->m_pGear2 == pGear2 )
		    || ( pGearConnection->m_pGear1 == pGear2 && pGearConnection->m_pGear2 == pGear1 ) )
		{
			if( pGearConnection->m_pGear1 == pGear1 )
			{
				if( pSize1 != 0 )
					*pSize1 = pGearConnection->m_Gear1Size * UseDocumentScale;
				if( pSize2 != 0 )
					*pSize2 = pGearConnection->m_Gear2Size * UseDocumentScale;
			}
			else
			{
				if( pSize1 != 0 )
					*pSize1 = pGearConnection->m_Gear2Size * UseDocumentScale;
				if( pSize2 != 0 )
					*pSize2 = pGearConnection->m_Gear1Size * UseDocumentScale;
			}
			return pGearConnection;
		}
	}

	return 0;
}

bool CLinkageDoc::SetGearRatio( CLink *pGear1, CLink *pGear2, double Size1, double Size2, bool bUseSizeAsSize, CGearConnection::ConnectionType ConnectionType )
{
	return SetGearRatio( pGear1, pGear2, Size1, Size2, bUseSizeAsSize, ConnectionType, true );
}

bool CLinkageDoc::SetGearRatio( CLink *pGear1, CLink *pGear2, double Size1, double Size2, bool bUseSizeAsSize, CGearConnection::ConnectionType ConnectionType, bool bFromUI )
{
	if( pGear1 == 0 || pGear2 == 0 || !pGear1->IsGear() || !pGear2->IsGear() )
		return false;

	CGearConnection * pGearConnection = GetGearRatio( pGear1, pGear2 );

	bool bNew = false;
	if( pGearConnection == 0 )
	{
		pGearConnection = new CGearConnection;
		bNew = true;
	}

	if( pGearConnection == 0 )
		return false;

	if( bFromUI )
		PushUndo();

	if( bUseSizeAsSize )
	{
		double DocumentScale = GetUnitScale();
		Size1 /= DocumentScale;
		Size2 /= DocumentScale;
	}

	if( bNew || pGearConnection->m_pGear1 == pGear1 )
	{
		pGearConnection->m_pGear1 = pGear1;
		pGearConnection->m_pGear2 = pGear2;
		pGearConnection->m_Gear1Size = Size1;
		pGearConnection->m_Gear2Size = Size2;
	}
	else
	{
		pGearConnection->m_pGear1 = pGear2;
		pGearConnection->m_pGear2 = pGear1;
		pGearConnection->m_Gear1Size = Size2;
		pGearConnection->m_Gear2Size = Size1;
	}
	pGearConnection->m_ConnectionType = ConnectionType;
	pGearConnection->m_bUseSizeAsRadius = bUseSizeAsSize;

	if( bNew )
		m_GearConnectionList.AddTail( pGearConnection );

	return true;
}

void CLinkageDoc::RemoveGearRatio( CConnector *pGearConnector, CLink *pGearLink )
{
	POSITION Position = m_GearConnectionList.GetHeadPosition();
	while( Position != 0 )
	{
		POSITION CurrentPosition = Position;
		CGearConnection *pTestGearConnection = m_GearConnectionList.GetNext( Position );
		if( pTestGearConnection == 0 || pTestGearConnection->m_pGear1 == 0 || pTestGearConnection->m_pGear2 == 0 )
			continue;

		if( pTestGearConnection->m_pGear1 == pGearLink || pTestGearConnection->m_pGear2 == pGearLink
		    || pTestGearConnection->m_pGear1->GetGearConnector() == pGearConnector || pTestGearConnection->m_pGear2->GetGearConnector() == pGearConnector )
		{
			m_GearConnectionList.RemoveAt( CurrentPosition );
			delete pTestGearConnection;
		}
	}
}

void CLinkageDoc::FastenThese( CConnector *pFastenThis, CLink *pFastenToThis )
{
	Unfasten( pFastenThis );
	pFastenThis->FastenTo( pFastenToThis );
	pFastenToThis->AddFastenConnector( pFastenThis );
}

void CLinkageDoc::FastenThese( CLink *pFastenThis, CLink *pFastenToThis )
{
	Unfasten( pFastenThis );
	pFastenThis->FastenTo( pFastenToThis );
	pFastenToThis->AddFastenLink( pFastenThis );
}

void CLinkageDoc::FastenThese( CLink *pFastenThis, CConnector *pFastenToThis )
{
	Unfasten( pFastenThis );
	pFastenThis->FastenTo( pFastenToThis );
	pFastenToThis->AddFastenLink( pFastenThis );
}

#if 0

CLinkageDoc *CLinkageDoc::GetPartsDocument( bool bRecompute )
{
	/*
	 * The parts document is a copy of the document but with some HUGE changes.
	 * Each part if rotated and aligned so that the bottom left corner of the part is at
	 * 0,0 in the document space. There are no connections between links; each link has a
	 * unique set of connectors. Right now, drawing elements are not part of the parts list,
	 * although I may change that in the future to allow some drawing elements to be used as parts.
	 * nothing is fastened in this parts docment.
	 *
	 * HOW DO I HANDLE GEAR SIZES?!?!
	 */

	if( !bRecompute && m_pPartsDoc != 0 )
		return m_pPartsDoc;

	if( m_pPartsDoc != 0 )
		delete m_pPartsDoc;

	m_pPartsDoc = new CLinkageDoc;
	m_pPartsDoc->CreatePartsFromDocument( this );
	return m_pPartsDoc;
}

void CLinkageDoc::MovePartsLinkToOrigin( CFPoint Origin, CLink *pPartsLink )
{
	int HullCount = 0;
	CFPoint *pPoints = pPartsLink->GetHull( HullCount );
	int BestStartPoint = -1;
	int BestEndPoint = -1;

	if( pPartsLink->GetConnectorCount() == 2 )
	{
		BestStartPoint = 0;
		BestEndPoint = 1;
	}
	else
	{
		double LargestDistance = 0.0;
		for( int Counter = 0; Counter < HullCount; ++Counter )
		{
			double TestDistance = 0;
			if( Counter < HullCount - 1 )
				TestDistance = Distance( pPoints[Counter], pPoints[Counter+1] );
			else
				TestDistance = Distance( pPoints[Counter], pPoints[0] );
			if( TestDistance > LargestDistance )
			{
				LargestDistance = TestDistance;
				BestStartPoint = Counter;
				// The hull is a clockwise collection of points so the end point of
				// the measurement is a better visual choice for the start connector.
				if( Counter < HullCount - 1 )
					BestEndPoint = Counter + 1;
				else
					BestEndPoint = 0;
			}
		}
	}
	if( BestStartPoint < 0 )
		return;

	CConnector *pStartConnector = 0;

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
	double Angle = GetAngle( pPoints[BestStartPoint], pPoints[BestEndPoint] );

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

void CLinkageDoc::CreatePartsFromDocument( CLinkageDoc *pOriginalDoc )
{
	CFRect Area;
	pOriginalDoc->GetDocumentArea( Area );

	POSITION Position = pOriginalDoc->m_Links.GetHeadPosition();
	while( Position != 0 )
	{
		CLink *pLink = pOriginalDoc->m_Links.GetNext( Position );
		if( pLink == 0 || ( pLink->GetLayers() & DRAWINGLAYER ) != 0 )
			continue;

		if( pLink->GetConnectorCount() <= 1 && !pLink->IsGear() )
			continue;

		CLink *pPartsLink = new CLink( *pLink );
		// MUST REMOVE THE COPIED CONNECTORS HERE BECAUSE THEY ARE JUST POINTER COPIES!
		pPartsLink->RemoveAllConnectors();

		POSITION Position2 = pLink->GetConnectorList()->GetHeadPosition();
		while( Position2 != 0 )
		{
			CConnector *pConnector = pLink->GetConnectorList()->GetNext( Position2 );
			if( pConnector == 0 )
				continue;

			CConnector *pPartsConnector = new CConnector( *pConnector );
			pPartsConnector->SetColor( pPartsLink->GetColor() );
			pPartsConnector->SetAsAnchor( false );
			pPartsConnector->SetAsInput( false );
			pPartsConnector->SetAsDrawing( false );
			pPartsLink->AddConnector( pPartsConnector );
			m_Connectors.AddTail( pPartsConnector );
		}

		MovePartsLinkToOrigin( Area.TopLeft(), pPartsLink );
		m_Links.AddTail( pPartsLink );
	}

	// Make a ground link. There must be at least one anchor for any mechanism.
	CLink *pGroundLink = new CLink;
	pGroundLink->SetIdentifier( 0x7FFFFFFF );
	pGroundLink->SetName( "Ground" );

	Position = pOriginalDoc->GetConnectorList()->GetHeadPosition();
	while( Position != 0 )
	{
		CConnector *pConnector = pOriginalDoc->GetConnectorList()->GetNext( Position );
		if( pConnector == 0 || !pConnector->IsAnchor() )
			continue;

		pGroundLink->SetLayers( pConnector->GetLayers() );

		CConnector *pPartsConnector = new CConnector( *pConnector );
		pPartsConnector->SetColor( CNullableColor( RGB( 0, 0, 0 ) ) ); // pPartsLink->GetColor() );
		pPartsConnector->SetAsAnchor( false );
		pPartsConnector->SetAsInput( false );
		pPartsConnector->SetAsDrawing( false );
		pGroundLink->AddConnector( pPartsConnector );
		m_Connectors.AddTail( pPartsConnector );
	}
	MovePartsLinkToOrigin( Area.TopLeft(), pGroundLink );
	m_Links.AddTail( pGroundLink );
}
#endif

