#include "StdAfx.h"
#include "avifile.h"

#ifndef __countof
#define __countof(x)	((sizeof(x)/sizeof(x[0])))
#endif

CAviFile:: CAviFile(LPCSTR lpszFileName /* =_T("Output.avi") */, 
			DWORD dwCodec /* = mmioFOURCC('M','P','G','4') */,
			DWORD dwFrameRate /* = 1 */)
{

	AVIFileInit();

	m_hHeap=NULL;
	m_hAviDC=NULL;
	m_lpBits=NULL;
	m_lSample=NULL;
	m_pAviFile=NULL;
	m_pAviStream=NULL;
	m_pAviCompressedStream=NULL;

	m_dwFCCHandler = dwCodec;
	m_dwFrameRate = dwFrameRate;

	m_szFileName = lpszFileName;
	m_szErrMsg = "Method Succeeded";
	m_LastError = S_OK;

	pAppendFrame[0]= &CAviFile::AppendDummy;			// VC8 requires & for Function Pointer; Remove it if your compiler complains;
	pAppendFrame[1]= &CAviFile::AppendFrameFirstTime;
	pAppendFrame[2]= &CAviFile::AppendFrameUsual;

	pAppendFrameDIBSection[0]= &CAviFile::AppendDummy;			// VC8 requires & for Function Pointer; Remove it if your compiler complains;
	pAppendFrameDIBSection[1]= &CAviFile::AppendFrameFirstTime;
	pAppendFrameDIBSection[2]= &CAviFile::AppendFrameUsual;

	pAppendFrameBits[0]=&CAviFile::AppendDummy;
	pAppendFrameBits[1]=&CAviFile::AppendFrameFirstTime;
	pAppendFrameBits[2]=&CAviFile::AppendFrameUsual;

	m_nAppendFuncSelector=1;		//0=Dummy	1=FirstTime	2=Usual
}

CAviFile::~CAviFile(void)
{
	ReleaseMemory();

	AVIFileExit();
}


void CAviFile::GetCompressorList( CStringList &List, BITMAPINFO &SourceBitmapInfo )
{
	///////////////////////////////////////////////////////////
	LPBITMAPINFOHEADER lpbi = 0;
	HIC hIC;
	ICINFO icinfo;

	int fccType = ICTYPE_VIDEO;	//0 to get all installed codecs
	for (int i=0; ICInfo(fccType, i, &icinfo); i++) {

		hIC = ICOpen(icinfo.fccType, icinfo.fccHandler, ICMODE_QUERY);

		if (hIC) {

			//Find out the compressor name.
			ICGetInfo(hIC, &icinfo, sizeof(icinfo));

			//Skip this compressor if it can't handle the format.
			if (fccType == ICTYPE_VIDEO )
			{
				if( icinfo.dwFlags != 0 )
				{
					if( ICCompressQuery( hIC, &SourceBitmapInfo, NULL ) == ICERR_OK ) 
					{
						CString Temp = CString( icinfo.szDescription );
						if( icinfo.fccHandler == 1129730893 )
							Temp += " - Low Resolution";
						Temp += " (";
						Temp.Append( (const char*)&icinfo.fccHandler, 4 ); // Assuming little endian system.
						Temp += ")";
						List.AddTail( Temp );
					}
				}
			}

			//check here whether it is the driver you want
			ICClose(hIC);
		}
	}
}

void CAviFile::ReleaseMemory()
{
	m_nAppendFuncSelector=0;		//Point to DummyFunction

	if(m_hAviDC)
	{
		DeleteDC(m_hAviDC);
		m_hAviDC=NULL;
	}
	if(m_pAviCompressedStream)
	{
		AVIStreamRelease(m_pAviCompressedStream);
		m_pAviCompressedStream=NULL;
	}
	if(m_pAviStream)
	{
		AVIStreamRelease(m_pAviStream);
		m_pAviStream=NULL;
	}
	if(m_pAviFile)
	{
		AVIFileRelease(m_pAviFile);
		m_pAviFile=NULL;
	}
	if(m_lpBits)
	{
		HeapFree(m_hHeap,HEAP_NO_SERIALIZE,m_lpBits);
		m_lpBits=NULL;
	}
	if(m_hHeap)
	{
		HeapDestroy(m_hHeap);
		m_hHeap=NULL;
	}
}

void CAviFile::SetErrorMessage(LPCTSTR lpszErrorMessage)
{
	m_szErrMsg = lpszErrorMessage;
}
	
HRESULT CAviFile::InitMovieCreation(int nFrameWidth, int nFrameHeight, int nBitsPerPixel)
{
	int	nMaxWidth=GetSystemMetrics(SM_CXSCREEN),nMaxHeight=GetSystemMetrics(SM_CYSCREEN);

	m_hAviDC = CreateCompatibleDC(NULL);
	if(m_hAviDC==NULL)	
	{
		SetErrorMessage("Unable to Create Compatible DC");
		m_LastError = E_FAIL;
		return E_FAIL;
	}
	
	if(nFrameWidth > nMaxWidth)	nMaxWidth= nFrameWidth;
	if(nFrameHeight > nMaxHeight)	nMaxHeight = nFrameHeight;

	m_hHeap=HeapCreate(HEAP_NO_SERIALIZE, nMaxWidth*nMaxHeight*4, 0);
	if(m_hHeap==NULL)
	{
		SetErrorMessage("Unable to Create Heap");
		m_LastError = E_FAIL;
		return E_FAIL;
	}
	
	m_lpBits=HeapAlloc(m_hHeap, HEAP_ZERO_MEMORY|HEAP_NO_SERIALIZE, nMaxWidth*nMaxHeight*4);
	if(m_lpBits==NULL)	
	{	
		SetErrorMessage("Unable to Allocate Memory on Heap");
		m_LastError = E_FAIL;
		return E_FAIL;
	}

	// Make sure that the file doesn't exist before acting on it.
	_unlink( m_szFileName );

	DWORD Result = AVIFileOpen( &m_pAviFile, m_szFileName, OF_CREATE|OF_WRITE, NULL );
	if( FAILED( Result ) )
	{
		SetErrorMessage("Unable to Create the Movie File");
		m_LastError = E_FAIL;
		return E_FAIL;
	}

	ZeroMemory(&m_AviStreamInfo,sizeof(AVISTREAMINFO));
	m_AviStreamInfo.fccType		= streamtypeVIDEO;
	m_AviStreamInfo.fccHandler	= m_dwFCCHandler;
	m_AviStreamInfo.dwScale		= 1;
	m_AviStreamInfo.dwRate		= m_dwFrameRate;	// Frames Per Second;
	m_AviStreamInfo.dwQuality	= -1;				// Default Quality
	m_AviStreamInfo.dwSuggestedBufferSize = nMaxWidth*nMaxHeight*4;
    SetRect(&m_AviStreamInfo.rcFrame, 0, 0, nFrameWidth, nFrameHeight);
	strcpy_s( m_AviStreamInfo.szName, sizeof( m_AviStreamInfo.szName ), _T("Video Stream"));

	if(FAILED(AVIFileCreateStream(m_pAviFile,&m_pAviStream,&m_AviStreamInfo)))
	{
		SetErrorMessage("Unable to Create Video Stream in the Movie File");
		m_LastError = E_FAIL;
		return E_FAIL;
	}

	ZeroMemory(&m_AviCompressOptions,sizeof(AVICOMPRESSOPTIONS));
	m_AviCompressOptions.fccType=streamtypeVIDEO;
	m_AviCompressOptions.fccHandler=m_AviStreamInfo.fccHandler;
	m_AviCompressOptions.dwFlags=AVICOMPRESSF_VALID;
	// m_AviCompressOptions.dwKeyFrameEvery=100;
	// m_AviCompressOptions.dwBytesPerSecond=1000; this is ignored for xvid recording
	// m_AviCompressOptions.dwQuality=500; this is ignored for xvid recording

	HRESULT hResult = AVIMakeCompressedStream( &m_pAviCompressedStream, m_pAviStream, &m_AviCompressOptions, NULL);

	if(FAILED(hResult))
	{
		// One reason this error might occur is if you are using a Codec that is not 
		// available on your system. Check the mmioFOURCC() code you are using and make
		// sure you have that codec installed properly on your machine.
		SetErrorMessage("Unable to Create Compressed Stream: Check your CODEC options");
		m_LastError = E_FAIL;
		return E_FAIL;
	}

	BITMAPINFO bmpInfo;
	ZeroMemory(&bmpInfo,sizeof(BITMAPINFO));
	bmpInfo.bmiHeader.biPlanes		= 1;
	bmpInfo.bmiHeader.biWidth		= nFrameWidth;
	bmpInfo.bmiHeader.biHeight		= nFrameHeight;
	bmpInfo.bmiHeader.biCompression	= BI_RGB;
	bmpInfo.bmiHeader.biBitCount	= nBitsPerPixel;
	bmpInfo.bmiHeader.biSize		= sizeof(BITMAPINFOHEADER);
	int LineByteWidth = ( ( bmpInfo.bmiHeader.biWidth * 3 ) + 3 )  & ~3;
	bmpInfo.bmiHeader.biSizeImage	= LineByteWidth*bmpInfo.bmiHeader.biHeight;

	if(FAILED(AVIStreamSetFormat(m_pAviCompressedStream,0,(LPVOID)&bmpInfo, bmpInfo.bmiHeader.biSize)))
	{
		// One reason this error might occur is if your bitmap does not meet the Codec requirements.
		// For example, 
		//   your bitmap is 32bpp while the Codec supports only 16 or 24 bpp; Or
		//   your bitmap is 274 * 258 size, while the Codec supports only sizes that are powers of 2; etc...
		// Possible solution to avoid this is: make your bitmap suit the codec requirements,
		// or Choose a codec that is suitable for your bitmap.
		SetErrorMessage("Unable to Set Video Stream Format");
		m_LastError = E_FAIL;
		return E_FAIL;
	}

	return S_OK;	// Everything went Fine
}


HRESULT	CAviFile::AppendFrameFirstTime(HBITMAP	hBitmap, int Count)
{
	BITMAP Bitmap;

	GetObject(hBitmap, sizeof(BITMAP), &Bitmap);

	if(SUCCEEDED(InitMovieCreation( Bitmap.bmWidth, 
									Bitmap.bmHeight, 
									Bitmap.bmBitsPixel)))
	{
		m_nAppendFuncSelector=2;		//Point to the UsualAppend Function

		return AppendFrameUsual(hBitmap, Count);
	}

	ReleaseMemory();

	return E_FAIL;
}

HRESULT	CAviFile::AppendFrameFirstTime( CDIBSection *pDIBSection, int Count )
{
	BITMAP Bitmap;

	GetObject( pDIBSection->m_Bitmap, sizeof(BITMAP), &Bitmap);

	if(SUCCEEDED(InitMovieCreation( Bitmap.bmWidth, 
									Bitmap.bmHeight, 
									Bitmap.bmBitsPixel)))
	{
		m_nAppendFuncSelector=2;		//Point to the UsualAppend Function

		return AppendFrameUsual( pDIBSection, Count );
	}

	ReleaseMemory();

	return E_FAIL;
}

HRESULT CAviFile::AppendFrameUsual(HBITMAP hBitmap, int Count)
{
	BITMAPINFO	bmpInfo;

	bmpInfo.bmiHeader.biBitCount=0;
	bmpInfo.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
	
	GetDIBits(m_hAviDC,hBitmap,0,0,NULL,&bmpInfo,DIB_RGB_COLORS);

	bmpInfo.bmiHeader.biCompression=BI_RGB;	

	GetDIBits(m_hAviDC,hBitmap,0,bmpInfo.bmiHeader.biHeight,m_lpBits,&bmpInfo,DIB_RGB_COLORS);

	while( Count > 0 )
	{
		if(FAILED(AVIStreamWrite(m_pAviCompressedStream,m_lSample++,1,m_lpBits,bmpInfo.bmiHeader.biSizeImage,0,NULL,NULL)))
		{
			SetErrorMessage(_T("Unable to Write Video Stream to the output Movie File"));

			ReleaseMemory();

			m_LastError = E_FAIL;
			return E_FAIL;
		}
		--Count;
	}
    return S_OK;
}

HRESULT CAviFile::AppendFrameUsual( CDIBSection *pDIBSection, int Count )
{
	while( Count > 0 )
	{
		if(FAILED(AVIStreamWrite(m_pAviCompressedStream,m_lSample++,1,pDIBSection->m_pBits,pDIBSection->m_bmpInfo.bmiHeader.biSizeImage,0,NULL,NULL)))
		{
			SetErrorMessage(_T("Unable to Write Video Stream to the output Movie File"));

			ReleaseMemory();

			m_LastError = E_FAIL;
			return E_FAIL;
		}
		--Count;
	}

    return S_OK;
}

HRESULT CAviFile::AppendDummy(HBITMAP, int)
{
	return E_FAIL;
}

HRESULT CAviFile::AppendDummy( CDIBSection *, int )
{
	return E_FAIL;
}

HRESULT CAviFile::AppendNewFrame(HBITMAP hBitmap, int Count)
{
	return (this->*pAppendFrame[m_nAppendFuncSelector])((HBITMAP)hBitmap, Count);
}

HRESULT CAviFile::AppendNewFrame( CDIBSection *pDIBSection, int Count )
{
	return (this->*pAppendFrameDIBSection[m_nAppendFuncSelector])(pDIBSection,Count);
}

HRESULT	CAviFile::AppendNewFrame(int nWidth, int nHeight, LPVOID pBits,int nBitsPerPixel, int Count)
{
	return (this->*pAppendFrameBits[m_nAppendFuncSelector])(nWidth,nHeight,pBits,nBitsPerPixel,Count);
}

HRESULT	CAviFile::AppendFrameFirstTime(int nWidth, int nHeight, LPVOID pBits,int nBitsPerPixel,int Count)
{
	if(SUCCEEDED(InitMovieCreation(nWidth, nHeight, nBitsPerPixel)))
	{
		m_nAppendFuncSelector=2;		//Point to the UsualAppend Function

		return AppendFrameUsual(nWidth, nHeight, pBits, nBitsPerPixel, Count);
	}

	ReleaseMemory();

	return E_FAIL;
}

HRESULT	CAviFile::AppendFrameUsual(int nWidth, int nHeight, LPVOID pBits,int nBitsPerPixel,int Count)
{
	DWORD	dwSize=nWidth*nHeight*nBitsPerPixel/8;

	while( Count > 0 )
	{
		if(FAILED(AVIStreamWrite(m_pAviCompressedStream,m_lSample++,1,pBits,dwSize,0,NULL,NULL)))
		{
			SetErrorMessage(_T("Unable to Write Video Stream to the output Movie File"));

			ReleaseMemory();

			m_LastError = E_FAIL;
			return E_FAIL;
		}
		--Count;
	}

    return S_OK;
}

HRESULT	CAviFile::AppendDummy(int nWidth, int nHeight, LPVOID pBits,int nBitsPerPixel,int Count)
{
	return E_FAIL;
}
