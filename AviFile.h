#pragma once

#ifndef __AVIFILE_H
#define __AVIFILE_H

#include <vfw.h>
#include <Wmsdk.h>

class CDIBSection
{
	public:

	CDIBSection()
	{
		memset( &m_bmpInfo, 0, sizeof( m_bmpInfo ) );
		m_Bitmap = (HBITMAP)INVALID_HANDLE_VALUE;
		m_pBits = 0;
	}

	~CDIBSection()
	{
		if( IsValid() )
			DeleteObject( m_Bitmap );
		m_Bitmap = (HBITMAP)INVALID_HANDLE_VALUE;
		m_pBits = 0;
		SelectObject( m_DC, (HBITMAP)0 );
		m_DC.DeleteDC(); 
	}

	bool CreateDibSection( CDC *pCompatibleDC, int Width, int Height )
	{
		memset( &m_bmpInfo, 0, sizeof( m_bmpInfo ) );
		m_bmpInfo.bmiHeader.biSize = sizeof( BITMAPINFOHEADER );
		m_bmpInfo.bmiHeader.biBitCount = 24;
		m_bmpInfo.bmiHeader.biCompression = BI_RGB;
		int LineByteWidth = ( ( Width * 3 ) + 3 )  & ~3;
		m_bmpInfo.bmiHeader.biSizeImage = LineByteWidth * Height;
		m_bmpInfo.bmiHeader.biPlanes = 1;
		m_bmpInfo.bmiHeader.biWidth = Width;
		m_bmpInfo.bmiHeader.biHeight = Height;

		if( !m_DC.CreateCompatibleDC( pCompatibleDC ) )
			return false;

		m_pBits = 0;

		m_Bitmap = CreateDIBSection( m_DC.GetSafeHdc(), &m_bmpInfo, DIB_RGB_COLORS, &m_pBits, NULL, NULL );
		if( m_Bitmap == (HBITMAP)INVALID_HANDLE_VALUE || (HBITMAP)m_Bitmap == 0 )
		{
			m_DC.DeleteDC(); 
			return false;
		}
	
		SelectObject( m_DC, m_Bitmap );
		return true;
	}

	CDC m_DC;
	BITMAPINFO	m_bmpInfo;
	HBITMAP m_Bitmap;
	void *m_pBits;

	bool IsValid() { return m_Bitmap != (HBITMAP)INVALID_HANDLE_VALUE && m_pBits != (HBITMAP)0; }
};

class CAviFile
{
	HDC					m_hAviDC;
	HANDLE				m_hHeap;
	LPVOID				m_lpBits;					// Useful for holding the bitmap content, if any
	LONG				m_lSample;					// Keeps track of the current Frame Index
	PAVIFILE			m_pAviFile;
	PAVISTREAM			m_pAviStream;
	PAVISTREAM			m_pAviCompressedStream;
	AVISTREAMINFO		m_AviStreamInfo;
	AVICOMPRESSOPTIONS	m_AviCompressOptions;
	DWORD				m_dwFrameRate;				// Frames Per Second Rate (FPS)
	DWORD				m_dwFCCHandler;				// Video Codec FourCC	
	CString				m_szFileName;
	CString				m_szErrMsg;					// Holds the Last Error Message, if any
	HRESULT				m_LastError;
	int					m_nAppendFuncSelector;		//0=Dummy	1=FirstTime	2=Usual

	HRESULT	AppendFrameFirstTime(HBITMAP,int );
	HRESULT	AppendFrameUsual(HBITMAP,int);
	HRESULT	AppendDummy(HBITMAP,int);
	HRESULT	(CAviFile::*pAppendFrame[3])(HBITMAP hBitmap,int Count);

	HRESULT	AppendFrameFirstTime( CDIBSection *,int );
	HRESULT	AppendFrameUsual( CDIBSection *,int );
	HRESULT	AppendDummy( CDIBSection *,int );
	HRESULT	(CAviFile::*pAppendFrameDIBSection[3])( CDIBSection *pDIBSection, int Count );

	HRESULT	AppendFrameFirstTime(int, int, LPVOID,int,int );
	HRESULT	AppendFrameUsual(int, int, LPVOID,int,int );
	HRESULT	AppendDummy(int, int, LPVOID,int,int );
	HRESULT	(CAviFile::*pAppendFrameBits[3])(int, int, LPVOID, int, int Count);

	/// Takes care of creating the memory, streams, compression options etc. required for the movie
	HRESULT InitMovieCreation(int nFrameWidth, int nFrameHeight, int nBitsPerPixel);

	/// Takes care of releasing the memory and movie related handles
	void ReleaseMemory();

	/// Sets the Error Message
	void SetErrorMessage(LPCTSTR lpszErrMsg);

public:

	/// <Summary>
	/// Constructor accepts the filename, video code and frame rate settings
	/// as parameters.
	/// lpszFileName: Name of the output movie file to create
	/// dwCodec: FourCC of the Video Codec to be used for compression
	/// dwFrameRate: The Frames Per Second (FPS) setting to be used for the movie
	/// </Summary>
	/// <Remarks>
	/// The default Codec used here is MPG4. To use a different codec, pass its Fourcc
	/// value as the input parameter for dwCodec. 
	/// For example,
	/// pass mmioFOURCC('D','I','V','X') to use DIVX codec, or
	/// pass mmioFOURCC('I','V','5','0') to use IV50 codec etc...
	///
	/// Also, you can pass just 0 to avoid the codecs altogether. In that case, Frames 
	/// would be saved as they are without any compression; However, the output movie file
	/// size would be very huge in that case.
	///
	/// Finally, make sure you have the codec installed on the machine before 
	/// passing its Fourcc here.
	/// </Remarks>
	CAviFile(LPCSTR	lpszFileName=_T("Output.avi"), 
			DWORD dwCodec = mmioFOURCC('M','P','G','4'),
			DWORD dwFrameRate = 1);

	static void GetCompressorList( CStringList &List, BITMAPINFO &SourceBitmapInfo );
	
	/// <Summary> 
	/// Destructor closes the movie file and flushes all the frames
	/// </Summary>
	~CAviFile(void);

	/// </Summary>
	/// Inserts the given HBitmap into the movie as a new Frame at the end.
	/// </Summary>
	HRESULT	AppendNewFrame(HBITMAP hBitmap, int Count);

	HRESULT	AppendNewFrame(  CDIBSection *pDIBSection, int Count );

	/// </Summary>
	/// Inserts the given bitmap bits into the movie as a new Frame at the end.
	/// The width, height and nBitsPerPixel are the width, height and bits per pixel
	/// of the bitmap pointed to by the input pBits.
	/// </Summary>
	HRESULT	AppendNewFrame(int nWidth, int nHeight, LPVOID pBits,int nBitsPerPixel, int Count);

	/// <Summary>
	/// Returns the last error message, if any.
	/// </Summary>
	LPCTSTR GetLastErrorMessage() const {	return m_szErrMsg;	}

	HRESULT GetLastError( void ) { return m_LastError; }
};

#endif