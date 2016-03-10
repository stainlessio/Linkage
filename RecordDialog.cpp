// LinkPropertiesDialog.cpp : implementation file
//

#include "stdafx.h"
#include "Linkage.h"
#include "RecordDialog.h"
#include "AviFile.h"
#include "mmsystem.h"


// CLinkPropertiesDialog dialog

IMPLEMENT_DYNAMIC(CRecordDialog, CMyDialog)

CRecordDialog::CRecordDialog(CWnd* pParent /*=NULL*/)
	: CMyDialog( pParent, CRecordDialog::IDD )
	, m_EncoderName(_T(""))
	, m_QualitySelection(0)
	, m_bUseStartTime(FALSE)
	, m_bUseRecordTime(FALSE)
	, m_bUseEndTime(FALSE)
	, m_StartTime(_T("0:0:0.0"))
	, m_RecordTime(_T("0:0:0.0"))
	, m_EndTime(_T("0:0:0.0"))
{

}

CRecordDialog::~CRecordDialog()
{
}

void CRecordDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_EncoderComboBox);
	DDX_Control(pDX, IDC_COMBO2, m_QualityControl);

	CWinApp *pApp = AfxGetApp();

	if( !pDX->m_bSaveAndValidate )
	{
		CString UseEncoder;
		if( pApp != 0 )
		{
			UseEncoder = pApp->GetProfileString( "Settings", "Encoderselection", 0 );
			m_QualitySelection = pApp->GetProfileInt( "Settings", "VideoQuality", m_QualitySelection );

			m_bUseStartTime = pApp->GetProfileInt( "Settings", "RecordUseStartTime", m_bUseStartTime ) != 0;
			m_bUseRecordTime = pApp->GetProfileInt( "Settings", "RecordUseRecordTime", m_bUseRecordTime ) != 0;
			m_bUseEndTime = pApp->GetProfileInt( "Settings", "RecordUseEndTime", m_bUseEndTime ) != 0;
			m_StartTime = pApp->GetProfileString( "Settings", "RecordStartTime", m_StartTime );
			m_RecordTime = pApp->GetProfileString( "Settings", "RecordRecordTime", m_RecordTime );
			m_EndTime = pApp->GetProfileString( "Settings", "RecordEndTime", m_EndTime );
		}

		// Get a list of choices for video Encoding.
		BITMAPINFO bmpInfo;
		memset( &bmpInfo, 0, sizeof( bmpInfo ) );
		bmpInfo.bmiHeader.biSize = sizeof( BITMAPINFOHEADER );
		bmpInfo.bmiHeader.biBitCount = 24;
		bmpInfo.bmiHeader.biCompression = BI_RGB;
		bmpInfo.bmiHeader.biSizeImage = 640 * 480 * 4;
		bmpInfo.bmiHeader.biPlanes = 1;
		bmpInfo.bmiHeader.biWidth = 640;
		bmpInfo.bmiHeader.biHeight = 480;

		CStringList CompressorList;
		CAviFile::GetCompressorList( CompressorList, bmpInfo );

		POSITION Position = CompressorList.GetHeadPosition();
		int Index = 0;
		int Selection = 0;
		while( Position != 0 )
		{
			CString &Temp = CompressorList.GetNext( Position );
			m_EncoderComboBox.InsertString( -1, Temp );

			if( Temp == UseEncoder )
				Selection = Index;

			++Index;
		}

		m_EncoderComboBox.SetCurSel( Selection );

		m_QualityControl.AddString( "Standard Quality (Slow)" );
		m_QualityControl.AddString( "Higher Quality (Slower)" );

		m_QualityControl.SetCurSel( m_QualitySelection );
	}

	DDX_Check(pDX, IDC_CHECK1, m_bUseStartTime);
	DDX_Check(pDX, IDC_CHECK2, m_bUseRecordTime);
	DDX_Check(pDX, IDC_CHECK3, m_bUseEndTime);
	DDX_Text(pDX, IDC_EDIT1, m_StartTime);
	DDX_Text(pDX, IDC_EDIT2, m_RecordTime);
	DDX_Text(pDX, IDC_EDIT3, m_EndTime);

	if( pDX->m_bSaveAndValidate )
	{
		DDX_CBIndex(pDX, IDC_COMBO2, m_QualitySelection);
		DDX_CBString(pDX, IDC_COMBO1, m_EncoderName);

		if( pApp != 0 )
		{
			pApp->WriteProfileString( "Settings", "Encoderselection", m_EncoderName  );
			pApp->WriteProfileInt( "Settings", "VideoQuality", m_QualitySelection );

			pApp->WriteProfileInt( "Settings", "RecordUseStartTime", m_bUseStartTime ? 1 : 0 );
			pApp->WriteProfileInt( "Settings", "RecordUseRecordTime", m_bUseRecordTime ? 1 : 0 );
			pApp->WriteProfileInt( "Settings", "RecordUseEndTime", m_bUseEndTime ? 1 : 0 );
			pApp->WriteProfileString( "Settings", "RecordStartTime", m_StartTime  );
			pApp->WriteProfileString( "Settings", "RecordRecordTime", m_RecordTime  );
			pApp->WriteProfileString( "Settings", "RecordEndTime", m_EndTime  );
		}
	}
}

BEGIN_MESSAGE_MAP(CRecordDialog, CMyDialog)
	ON_BN_CLICKED( IDC_BUTTON1, &CRecordDialog::OnBnClickedButton1 )
END_MESSAGE_MAP()


// CLinkPropertiesDialog message handlers




void CRecordDialog::OnBnClickedButton1()
{
	UpdateData();

	const char *pFCC = strchr( m_EncoderName, '(' );
	if( pFCC == 0 )
		return;

	CString FCC;
	FCC.Append( pFCC+1, 4 );

	DWORD fccType = ICTYPE_VIDEO;
	DWORD fccHandler = mmioFOURCC( FCC[0], FCC[1], FCC[2], FCC[3] );
	HIC hic = ICOpen(fccType, fccHandler, ICMODE_QUERY);
	if( hic != INVALID_HANDLE_VALUE && hic != 0 )
		ICSendMessage(hic, ICM_CONFIGURE, (DWORD_PTR)(AfxGetMainWnd()->m_hWnd), 0);
}
