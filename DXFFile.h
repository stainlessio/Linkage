#pragma once

#include "string"

class CDXFFile
{
public:
	CDXFFile(void);

	~CDXFFile(void) {}

	//================================================================
	//Save countless hours of CAD design time by automating the design
	//process.  Output DXF files on the fly by using this source code.
	//Create a simple GUI to gather data to generate the geometry,
	//then export it instantly to a ".dxf" file which is a format
	//supported by most CAD software and even some design software.

	//This DXF generating source code was created by David S. Tufts,
	//you can contact me at kdtufts@juno.com.  The variables set
	//up in the DXF header are my personal preferences. The
	//variables can be changed by observing the settings of any
	//DXF file which was saved with your desired preferences.  Also,
	//any additional geometry can be added to this code in the form of
	//a function by observing the DXF output of any CAD software that
	//supports DXF files.  Good luck and have fun...
	//================================================================

private:
	std::string DXF_BodyText;
	std::string DXF_BlockBody;
	int BlockIndex;

public:
	double DimScale;

	std::string Layer;

	//Build the header, this header is set with my personal preferences
private:
	std::string DXF_Header();


	//The block header, body, and footer are used to append the
	//header with any dimensional information added in the body.
	std::string DXF_BlockHeader();

	void DXF_BuildBlockBody();

	std::string DXF_BlockFooter();

	//The body header, and footer will always remain the same
	std::string DXF_BodyHeader();

	std::string DXF_BodyFooter();

	std::string DXF_Footer();

public:
	void DXF_Save(const std::string &fpath);

	//====================================================
	//All geometry is appended to the text: "DXF_BodyText"
	//====================================================

	void DXF_Line(double x1, double y1, double Z1, double x2, double y2, double Z2);
	void DXF_Circle(double x, double y, double Z, double Radius);
	void DXF_Arc(double x, double y, double Z, double Radius, double StartAngle, double EndAngle);
	void DXF_Text(double x, double y, double Z, double Height, const std::string &iText, bool bCenter );
	void DXF_Dimension(double x1, double y1, double x2, double y2, double PX1, double PY1, double PX2, double PY2, const std::string &iText);
	void DXF_Dimension(double x1, double y1, double x2, double y2, double PX1, double PY1, double PX2, double PY2, double iAng, const std::string &iText);
	void DXF_Rectangle(double x1, double y1, double Z1, double x2, double y2, double Z2);
private:
	void DXF_Border(double x1, double y1, double Z1, double x2, double y2, double Z2);
public:
	void DXF_ShowText(double x, double y, double eAng, double eRad, const std::string &eText);
	void DXF_ArrowHead(double iRadians, double sngX, double sngY);

	void DXF_Polyline( int Count, double *coordinate, bool bClosed );

	void DXF_Point(double x, double y);

	void DXF_Note(double x, double y, const std::string &note);
	void DXF_Note(double x, double y, const std::string &note, double txt_height);
};

