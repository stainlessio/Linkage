#include "stdafx.h"
#include "resource.h"
#include "SampleGallery.h"
#include "examples_xml.h"

class CGallerySample
{
	public:
	int m_CommandID;
	int m_TooltipID;
	const char *m_XML;
};

static const int SAMPLE_GALLERY_COUNT = 31;
static CGallerySample Samples[] = {
		{ ID_SAMPLE_SIMPLE, IDS_SAMPLE_SIMPLE, EXAMPLE1_XML },
		{ ID_SAMPLE_SMILE, IDS_SAMPLE_SMILE, EXAMPLE3_XML },
		{ ID_SAMPLE_JANSEN, IDS_SAMPLE_JANSEN, JANSEN_XML },
		{ ID_SAMPLE_KLANN, IDS_SAMPLE_KLANN, KLANN_XML },
		{ ID_SAMPLE_JOULES, IDS_SAMPLE_JOULES, JOULES_XML },
		{ ID_SAMPLE_HOEKEN, IDS_SAMPLE_HOEKEN, HOEKEN_XML },
		{ ID_SAMPLE_PARALLEL, IDS_SAMPLE_PARALLEL, EXAMPLE8_XML },
		{ ID_SAMPLE_FOLDINGTOP, IDS_SAMPLE_FOLDINGTOP, FOLDINGTOP_XML },
		{ ID_SAMPLE_SLIDING1, IDS_SAMPLE_SLIDING1, SLIDERDEMO_XML },
		{ ID_SAMPLE_SLIDING2, IDS_SAMPLE_SLIDING2, SLIDERDEMO2_XML },
		{ ID_SAMPLE_LINK2, IDS_SAMPLE_LINK2, ELLIPTICAL_XML },
		{ ID_SAMPLE_LINK4, IDS_SAMPLE_LINK4, SLIDERDEMO7_XML },
		{ ID_SAMPLE_ACTUATOR2, IDS_SAMPLE_ACTUATOR2, ACTUATORDEMO2_XML },
		{ ID_SAMPLE_ACTUATOR3, IDS_SAMPLE_ACTUATOR3, ACTUATORDEMO3_XML },
		{ ID_SAMPLE_COMPLICATED, IDS_SAMPLE_COMPLICATED, FANCYEXAMPLE_XML },
		{ ID_SAMPLE_PEAUCELLIER, IDS_SAMPLE_PEAUCELLIER, PEAUCELLIER_XML },
		{ ID_SAMPLE_SCISSORS, IDS_SAMPLE_SCISSORS, SCISSORSDEMO_XML },
		{ ID_SAMPLE_VARYSPEED, IDS_SAMPLE_VARYSPEED, VARYSPEEDDEMO_XML },
		{ ID_SAMPLE_WATT, IDS_SAMPLE_WATT, WATTPARALLEL_XML },
		{ ID_SAMPLE_THROW, IDS_SAMPLE_THROW, THROWTEST_XML },
		{ ID_SAMPLE_PISTON, IDS_SAMPLE_PISTON, PISTON_XML },
		{ ID_SAMPLE_CURVEDSLIDER, IDS_SAMPLE_CURVEDSLIDER, CURVEDSLIDER1_XML },
		{ ID_SAMPLE_CURVEDSLIDER2, IDS_SAMPLE_CURVEDSLIDER2, CURVEDSLIDER2_XML },
		{ ID_SAMPLE_CHEBYSHEV, IDS_SAMPLE_CHEBYSHEV, CHEBYCHEV_XML },
		{ ID_SAMPLE_DOUBLECURVESLIDE, IDS_SAMPLE_DOUBLECURVESLIDE, DOUBLECURVESLIDE_XML },
		{ ID_SAMPLE_LIFTER, IDS_SAMPLE_LIFTER, LIFTER_XML },
		{ ID_SAMPLE_FOURGEARS, IDS_SAMPLE_FOURGEARS, FOURGEARS_XML },
		{ ID_SAMPLE_ORBITGEARS, IDS_SAMPLE_ORBITGEARS, ORBITGEARS_XML },
		{ ID_SAMPLE_OSCILLATORGEARS, IDS_SAMPLE_OSCILLATORGEARS, OSCILLATORS_XML },
		{ ID_SAMPLE_STEPHENSON, IDS_SAMPLE_STEPHENSON, STEPHENSON_XML },
		{ 0, 0 }
		};

CSampleGallery::CSampleGallery(void)
{
}

CSampleGallery::~CSampleGallery(void)
{
}

int CSampleGallery::GetCount( void )
{
	return SAMPLE_GALLERY_COUNT - 1;
}

int CSampleGallery::GetCommandID( int Index )
{
	if( Index >= SAMPLE_GALLERY_COUNT )
		return 0;
	return Samples[Index].m_CommandID;
}

int CSampleGallery::GetStringID( int Index )
{
	if( Index >= SAMPLE_GALLERY_COUNT )
		return 0;
	return Samples[Index].m_TooltipID;
}

const char * CSampleGallery::GetXMLData( int Index )
{
	if( Index >= SAMPLE_GALLERY_COUNT )
		return "";
	return Samples[Index].m_XML;
}

