#include "stdafx.h"
#include "DXFFile.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include <sstream>
#include <string>
#include <vector>
#include <cstddef>

using namespace std;

struct splitempties
{
  enum empties_t { empties_ok, no_empties };
};

template <typename Container>
Container& split(
  Container&                            result,
  const typename Container::value_type& s,
  const typename Container::value_type& delimiters,
  splitempties::empties_t                      empties = splitempties::empties_ok )
{
  result.clear();
  size_t current;
  size_t next = -1;
  do
  {
    if (empties == splitempties::no_empties)
    {
      next = s.find_first_not_of( delimiters, next + 1 );
      if (next == Container::value_type::npos) break;
      next -= 1;
    }
    current = next + 1;
    next = s.find_first_of( delimiters, current );
    result.push_back( s.substr( current, next - current ) );
  }
  while (next != Container::value_type::npos);
  return result;
}

CDXFFile::CDXFFile()
{
	BlockIndex = 0;
	DimScale = 1.0f;
	BlockIndex = 0;
	DXF_BodyText = "";
	DXF_BlockBody = "";
	Layer = "DEFAULT";
}

std::string CDXFFile::DXF_Header()
{
	string HS;
	HS += "  0|SECTION|  2|HEADER|  9";
	HS += "|$ACADVER|  1|AC1009|  9";
	HS += "|$INSBASE| 10|0.0| 20|0.0| 30|0.0|  9";

	HS += "|$EXTMIN| 10|  0| 20|  0| 30|  0|  9";
	HS += "|$EXTMAX| 10|3680| 20|3260| 30|0.0|  9";

	HS += "|$LIMMIN| 10|0.0| 20|0.0|  9";
	HS += "|$LIMMAX| 10|100.0| 20|100.0|  9";
	HS += "|$ORTHOMODE| 70|     1|  9";
	std::ostringstream temp;
	temp << DimScale;
	HS += "|$DIMSCALE| 40|" + temp.str() + "|  9";
	HS += "|$DIMSTYLE|  2|STANDARD|  9";
	HS += "|$FILLETRAD| 40|0.0|  9";
	HS += "|$PSLTSCALE| 70|     1|  0";
	HS += "|ENDSEC|  0";
	HS += "|SECTION|  2|TABLES|  0";
	HS += "|TABLE|  2|VPORT| 70|     2|  0|VPORT|  2|*ACTIVE| 70|     0| 10|0.0| 20|0.0| 11|1.0| 21|1.0| 12|50.0| 22|50.0| 13|0.0| 23|0.0| 14|1.0| 24|1.0| 15|0.0| 25|0.0| 16|0.0| 26|0.0| 36|1.0| 17|0.0| 27|0.0| 37|0.0| 40|100.0| 41|1.55| 42|50.0| 43|0.0| 44|0.0| 50|0.0| 51|0.0| 71|     0| 72|   100| 73|     1| 74|     1| 75|     0| 76|     0| 77|     0| 78|     0|  0|ENDTAB|  0";
	HS += "|TABLE|  2|LTYPE| 70|     1|  0|LTYPE|  2|CONTINUOUS|  70|     0|  3|Solid Line| 72|    65| 73|     0| 40|0.0|  0|ENDTAB|  0";
	HS += "|TABLE|  2|LAYER| 70|     3|  0|LAYER|  2|0| 70|     0| 62|     7|  6|CONTINUOUS|  0|LAYER|  2|" + Layer + "| 70|     0| 62|     7|  6|CONTINUOUS|  0|LAYER|  2|DEFPOINTS| 70|     0| 62|     7| 6|CONTINUOUS|  0|ENDTAB|  0";
	HS += "|TABLE|  2|VIEW| 70|     0|  0|ENDTAB|  0";
	HS += "|TABLE|  2|DIMSTYLE| 70|     1|  0|DIMSTYLE|  2|STANDARD| 70|     0|  3||  4||  5||  6||  7|| 40|1.0| 41|0.125| 42|0.04| 43|0.25| 44|0.125| 45|0.0| 46|0.0| 47|0.0| 48|0.0|140|0.125|141|0.06|142|0.0|143|25.4|144|1.0|145|0.0|146|1.0|147|0.09| 71|     0| 72|     0| 73|     1| 74|     1| 75|     0| 76|     0| 77|     0| 78|     0|170|     0|171|     2|172|     0|173|     0|174|     0|175|     0|176|     0|177|     0|178|     0|  0|ENDTAB|  0";
	HS += "|ENDSEC|";
	return HS;
}

std::string CDXFFile::DXF_BlockHeader()
{
	return "  0|SECTION|  2|BLOCKS|";
}

void CDXFFile::DXF_BuildBlockBody()
{
	std::ostringstream blockindex;
	blockindex << BlockIndex;

	DXF_BlockBody = DXF_BlockBody + "  0|BLOCK|  8|0|  2|*D" + blockindex.str() + "|70|     1| 10|0.0| 20|0.0| 30|0.0|  3|*D" + blockindex.str() + "|  1||0|ENDBLK|  8|0|";
	BlockIndex = BlockIndex + 1;
}

std::string CDXFFile::DXF_BlockFooter()
{
	return "  0|ENDSEC|";
}

std::string CDXFFile::DXF_BodyHeader()
{
	return "  0|SECTION|  2|ENTITIES|";
}

std::string CDXFFile::DXF_BodyFooter()
{
	return "  0|ENDSEC|";
}

std::string CDXFFile::DXF_Footer()
{
	return "  0|EOF";
}

void CDXFFile::DXF_Save(const std::string &fpath)
{
	string strDXF_Output = DXF_Header() + DXF_BlockHeader() + DXF_BlockBody + DXF_BlockFooter() + DXF_BodyHeader() + DXF_BodyText + DXF_BodyFooter() + DXF_Footer();

	//split the text string at "|" and output to specified file
	vector <string> varDXF;
	varDXF = split( varDXF, strDXF_Output, "|" );

	CFile File;
	if( File.Open( fpath.c_str(), CFile::modeWrite | CFile::modeCreate ) )
	{
		for (int i = 0; i < (int)varDXF.size(); i++)
		{
			File.Write( varDXF[i].c_str(), varDXF[i].size() );
			File.Write( "\r\n", 2 );
		}
	}
}

void CDXFFile::DXF_Line(double x1, double y1, double Z1, double x2, double y2, double Z2)
{
	std::ostringstream temp;
	temp << "  0|LINE|8|" << Layer << "| 10|" << x1 << "| 20|" << y1 << "| 30|" << Z1 << "| 11|" << x2 << "| 21|" << y2 << "| 31|" << Z2 << "|";
	DXF_BodyText += temp.str();
}

void CDXFFile::DXF_Circle(double x, double y, double Z, double Radius)
{
	std::ostringstream temp;
	temp << "  0|CIRCLE|8|" << Layer << "| 10|" << x << "| 20|" << y << "| 30|" << Z << "| 40|" << Radius << "| 39|  0|";
	DXF_BodyText += temp.str();
}

void CDXFFile::DXF_Arc(double x, double y, double Z, double Radius, double StartAngle, double EndAngle)
{
	std::ostringstream temp;
	//"|62|1|" after Layer sets the to color (Red)
	temp << "  0|ARC|8|" << Layer << "| 10|" << x << "| 20|" << y << "| 30|" << Z << "| 40|" << Radius << "| 50|" << StartAngle << "| 51|" << EndAngle << "|";
	DXF_BodyText += temp.str();
}

void CDXFFile::DXF_Text(double x, double y, double Z, double Height, const std::string &iText, bool bCenter )
{
	std::ostringstream temp;
	temp << "  0|TEXT|8|" << Layer << "| 10|" << x << "| 11|" << x << "| 20|" << y << "| 21|" << y << "| 30|" << Z << "| 31|" << Z << "| 40|" << DimScale * Height << "|1|" << iText << "| 50|  0|";
	if( bCenter )
		temp << " 72|  1|";
	DXF_BodyText += temp.str();
}

void CDXFFile::DXF_Dimension(double x1, double y1, double x2, double y2, double PX1, double PY1, double PX2, double PY2, const std::string &iText)
{
	DXF_Dimension(x1,y1,x2,y2,PX1,PY1,PX2,PY2, 0, iText);
}

void CDXFFile::DXF_Dimension(double x1, double y1, double x2, double y2, double PX1, double PY1, double PX2, double PY2, double iAng, const std::string &iText)
{
	//I know for sure that this works in AutoCAD.
	std::ostringstream temp;
	temp << "  0|DIMENSION|  8|" << Layer << "|  6|CONTINUOUS|  2|*D" << BlockIndex;
	temp << "| 10|" << PX1 << "| 20|" << PY1 << "| 30|0.0";
	temp << "| 11|" << PX2 << "| 21|" << PY2 << "| 31|0.0";
	temp << "| 70|     0";
	if( iText != "None" )
		temp << "|  1|" << iText;
	temp << "| 13|" << x1 << "| 23|" << y1 << "| 33|0.0";
	temp << "| 14|" << x2 << "| 24|" << y2 << "| 34|0.0";
	if( iAng != 0 )
		temp << "| 50|" << iAng;
	temp << "|1001|ACAD|1000|DSTYLE|1002|{|1070|   287|1070|     3|1070|    40|1040|" << DimScale << "|1070|   271|1070|     3|1070|   272|1070|     3|1070|   279|1070|     0|1002|}|";

	DXF_BodyText += temp.str();

	//All dimensions need to be referenced in the header information
	DXF_BuildBlockBody();
}

void CDXFFile::DXF_Rectangle(double x1, double y1, double Z1, double x2, double y2, double Z2)
{
	std::ostringstream temp;
	temp << "  0|POLYLINE|  5|48|  8|" << Layer << "|66|1| 10|0| 20|0| 30|0| 70|1|0";
	temp << "|VERTEX|5|50|8|" << Layer << "| 10|" << x1 << "| 20|" << y1 << "| 30|" << Z1 << "|  0";
	temp << "|VERTEX|5|51|8|" << Layer << "| 10|" << x2 << "| 20|" << y1 << "| 30|" << Z2 << "|  0";
	temp << "|VERTEX|5|52|8|" << Layer << "| 10|" << x2 << "| 20|" << y2 << "| 30|" << Z2 << "|  0";
	temp << "|VERTEX|5|53|8|" << Layer << "| 10|" << x1 << "| 20|" << y2 << "| 30|" << Z1 << "|  0";
	temp << "|SEQEND|     8|" << Layer << "|";
	DXF_BodyText += temp.str();
}

void CDXFFile::DXF_Border(double x1, double y1, double Z1, double x2, double y2, double Z2)
{
	std::ostringstream temp;
	temp << "  0|POLYLINE|  8|" << Layer << "| 40|1| 41|1| 66|1| 70|1|0";
	temp << "|VERTEX|  8|" << Layer << "| 10|" << x1 << "| 20|" << y1 << "| 30|" << Z1 << "|  0";
	temp << "|VERTEX|  8|" << Layer << "| 10|" << x2 << "| 20|" << y1 << "| 30|" << Z2 << "|  0";
	temp << "|VERTEX|  8|" << Layer << "| 10|" << x2 << "| 20|" << y2 << "| 30|" << Z2 << "|  0";
	temp << "|VERTEX|  8|" << Layer << "| 10|" << x1 << "| 20|" << y2 << "| 30|" << Z1 << "|  0";
	temp << "|SEQEND|  8|" << Layer << "|";
	DXF_BodyText += temp.str();
}

#if 0

// Not sure what this is good for.

void CDXFFile::DXF_ShowText(double x, double y, double eAng, double eRad, const std::string &eText)
{
	double eX = 0;
	double eY = 0;
	double iRadians = 0;
	iRadians = static_cast<double>(M_PI*eAng / 180);
	//Find the angle at which to draw the arrow head and leader
	eX = x - static_cast<double>(eRad * (cos(iRadians)));
	eY = y - static_cast<double>(eRad * (sin(iRadians)));
	//Draw an arrow head
	DXF_ArrowHead(iRadians + static_cast<double>(M_PI), x, y);
	//Draw the leader lines
	DXF_Line(x, y, 0, eX, eY, 0);
	DXF_Line(eX, eY, 0, eX + 2, eY, 0);
	//Place the text
	DXF_Text(eX + static_cast<double>(2.5), eY - static_cast<double>(0.75), 0.0f, 1.5f, eText);
}

#endif

#if 0

// The caller draws their own arrow heads.

void CDXFFile::DXF_ArrowHead(double iRadians, double sngX, double sngY)
{
	double x[2];
	double y[2];
	//The number "3" is the length of the arrow head.
	//Adding or subtracting 170 degrees from the angle
	//gives us a 10 degree angle on the arrow head.
	//Finds the first side of the arrow head
	double r170 = static_cast<double>(M_PI * 170 / 180);
	x[0] = sngX + static_cast<double>(3 * (sin(iRadians + r170)));
	y[0] = sngY + static_cast<double>(3 * (cos(iRadians + r170)));
	//Finds the second side of the arrow head
	x[1] = sngX + static_cast<double>(3 * (sin(iRadians - r170)));
	y[1] = sngY + static_cast<double>(3 * (cos(iRadians - r170)));
	//Draw the first side of the arrow head
	DXF_Line(sngX, sngY, 0, x[0], y[0], 0);
	///
	//Draw the second side of the arrow head
	DXF_Line(sngX, sngY, 0, x[1], y[1], 0);
	//\
	//Draw the bottom side of the arrow head
	DXF_Line(x[0], y[0], 0, x[1], y[1], 0);
	//_
}

#endif

void CDXFFile::DXF_Polyline( int Count, double *coordinate, bool bClosed )
{
	std::ostringstream temp;

	temp << "  0|POLYLINE| 8|" << Layer << "| 66| 1|";
	if( bClosed )
		temp << " 70| 1|";
	int i = 0;
	for (i = 0; i < Count; i++)
	{
		temp << "  0|VERTEX| 8|" << Layer << "| 10| " << coordinate[2 * i] << "| 20| " << coordinate[2 * i + 1] << "|";
	}
	temp << "  0|SEQEND| 8|" << Layer << "|";
	DXF_BodyText += temp.str();
}

void CDXFFile::DXF_Point(double x, double y)
{
	std::ostringstream temp;
	temp << "  0|POINT| 8|" << Layer << "| 10|" << x << "| 20|" << y << "|";
	DXF_BodyText += temp.str();
}

void CDXFFile::DXF_Note(double x, double y, const std::string &note)
{
	DXF_Note(x,y,note,0.125);
}

void CDXFFile::DXF_Note(double x, double y, const std::string &note, double txt_height)
{
	vector <string> lines;

	lines = split( lines, note, "|" );

	for (int i = 0; i < (int)lines.size(); i++)
	{
		DXF_Text(static_cast<double>(x), static_cast<double>(y), 0, static_cast<double>(txt_height), lines[i], false );
		y = y - 2 * txt_height;
	}
}

