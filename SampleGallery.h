#pragma once
class CSampleGallery
{
public:
	CSampleGallery(void);
	~CSampleGallery(void);

	int GetCount( void );
	int GetCommandID( int Index );
	int GetStringID( int Index );
	const char *GetXMLData( int Index );
};

