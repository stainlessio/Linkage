#define EXAMPLE1_XML "\
<linkage2>\
	<program zoom=\"1\" xoffset=\"268\" yoffset=\"166\" scalefactor=\"1\" units=\"Millimeters\"/>\
	<connector anchor=\"true\" input=\"true\" rpm=\"30\" x=\"200\" y=\"-100\" id=\"0\"/>\
	<connector x=\"200\" y=\"-65\" id=\"1\"/>\
	<connector x=\"300\" y=\"-100\" id=\"2\"/>\
	<connector anchor=\"true\" x=\"300\" y=\"-300\" id=\"3\"/>\
	<Link id=\"0\">\
		<connector id=\"0\"/>\
		<connector id=\"1\"/>\
	</Link>\
	<Link id=\"1\">\
		<connector id=\"1\"/>\
		<connector id=\"2\"/>\
	</Link>\
	<Link id=\"2\">\
		<connector id=\"2\"/>\
		<connector id=\"3\"/>\
	</Link>\
</linkage2>\
"

#define EXAMPLE2_XML "\
<linkage2>\
	<program zoom=\"1\" xoffset=\"359\" yoffset=\"297\" scalefactor=\"1\" units=\"Millimeters\"/>\
	<connector anchor=\"true\" input=\"true\" rpm=\"15\" x=\"228\" y=\"-111\" id=\"0\"/>\
	<connector x=\"218\" y=\"-74\" id=\"1\"/>\
	<connector x=\"310\" y=\"-93\" id=\"2\"/>\
	<connector anchor=\"true\" x=\"227\" y=\"-182\" id=\"3\"/>\
	<connector x=\"272\" y=\"-179\" id=\"7\"/>\
	<connector x=\"189\" y=\"-218\" id=\"9\"/>\
	<connector draw=\"true\" x=\"324\" y=\"-373\" id=\"11\"/>\
	<connector x=\"271\" y=\"-294\" id=\"12\"/>\
	<connector anchor=\"true\" x=\"184\" y=\"-432\" id=\"13\"/>\
	<connector x=\"178\" y=\"-267\" id=\"14\"/>\
	<connector anchor=\"true\" input=\"true\" rpm=\"10\" x=\"78\" y=\"-264\" id=\"16\"/>\
	<connector x=\"94\" y=\"-246\" id=\"17\"/>\
	<Link id=\"0\">\
		<connector id=\"0\"/>\
		<connector id=\"1\"/>\
	</Link>\
	<Link id=\"1\">\
		<connector id=\"1\"/>\
		<connector id=\"2\"/>\
	</Link>\
	<Link id=\"2\">\
		<connector id=\"2\"/>\
		<connector id=\"3\"/>\
		<connector id=\"7\"/>\
	</Link>\
	<Link id=\"6\">\
		<connector id=\"11\"/>\
		<connector id=\"12\"/>\
		<connector id=\"9\"/>\
	</Link>\
	<Link id=\"7\">\
		<connector id=\"7\"/>\
		<connector id=\"12\"/>\
	</Link>\
	<Link id=\"8\">\
		<connector id=\"13\"/>\
		<connector id=\"14\"/>\
		<connector id=\"9\"/>\
	</Link>\
	<Link id=\"9\">\
		<connector id=\"16\"/>\
		<connector id=\"17\"/>\
	</Link>\
	<Link id=\"10\">\
		<connector id=\"14\"/>\
		<connector id=\"17\"/>\
	</Link>\
</linkage2>\
"

#define EXAMPLE3_XML "\
<linkage2>\
	<program zoom=\"1\" xoffset=\"359\" yoffset=\"297\" scalefactor=\"1\" units=\"Millimeters\"/>\
	<connector anchor=\"true\" x=\"339\" y=\"-95\" id=\"4\"/>\
	<connector x=\"303\" y=\"-60\" id=\"5\"/>\
	<connector anchor=\"true\" x=\"201\" y=\"-52\" id=\"6\"/>\
	<connector x=\"262\" y=\"-120\" id=\"7\"/>\
	<connector x=\"365\" y=\"-16\" id=\"9\"/>\
	<connector anchor=\"true\" input=\"true\" rpm=\"15\" x=\"252\" y=\"-197\" id=\"12\"/>\
	<connector x=\"243\" y=\"-240\" id=\"13\"/>\
	<connector anchor=\"true\" x=\"13\" y=\"-181\" id=\"14\"/>\
	<connector x=\"368\" y=\"-181\" id=\"15\"/>\
	<connector anchor=\"true\" input=\"true\" rpm=\"-15\" x=\"717\" y=\"-326\" id=\"16\"/>\
	<connector x=\"718\" y=\"-299\" id=\"17\"/>\
	<connector draw=\"true\" x=\"373\" y=\"-449\" id=\"18\"/>\
	<connector x=\"370\" y=\"-306\" id=\"19\"/>\
	<connector anchor=\"true\" input=\"true\" rpm=\"15\" x=\"342\" y=\"-389\" id=\"37\"/>\
	<connector draw=\"true\" x=\"339\" y=\"-365\" id=\"38\"/>\
	<connector anchor=\"true\" input=\"true\" rpm=\"18\" x=\"398\" y=\"-389\" id=\"39\"/>\
	<connector draw=\"true\" x=\"421\" y=\"-399\" id=\"40\"/>\
	<Link id=\"3\">\
		<connector id=\"4\"/>\
		<connector id=\"5\"/>\
	</Link>\
	<Link id=\"4\">\
		<connector id=\"6\"/>\
		<connector id=\"7\"/>\
	</Link>\
	<Link id=\"5\">\
		<connector id=\"9\"/>\
		<connector id=\"7\"/>\
		<connector id=\"5\"/>\
	</Link>\
	<Link id=\"7\">\
		<connector id=\"12\"/>\
		<connector id=\"13\"/>\
	</Link>\
	<Link id=\"8\">\
		<connector id=\"7\"/>\
		<connector id=\"13\"/>\
	</Link>\
	<Link id=\"9\">\
		<connector id=\"14\"/>\
		<connector id=\"15\"/>\
	</Link>\
	<Link id=\"10\">\
		<connector id=\"9\"/>\
		<connector id=\"15\"/>\
	</Link>\
	<Link id=\"11\">\
		<connector id=\"16\"/>\
		<connector id=\"17\"/>\
	</Link>\
	<Link id=\"12\">\
		<connector id=\"18\"/>\
		<connector id=\"19\"/>\
		<connector id=\"15\"/>\
	</Link>\
	<Link id=\"14\">\
		<connector id=\"17\"/>\
		<connector id=\"19\"/>\
	</Link>\
	<Link id=\"28\">\
		<connector id=\"37\"/>\
		<connector id=\"38\"/>\
	</Link>\
	<Link id=\"29\">\
		<connector id=\"39\"/>\
		<connector id=\"40\"/>\
	</Link>\
</linkage2>\
"

#define EXAMPLE4_XML "\
<linkage2>\
	<program zoom=\"1\" xoffset=\"268\" yoffset=\"166\" scalefactor=\"1\" units=\"Millimeters\"/>\
	<connector anchor=\"true\" input=\"true\" rpm=\"30\" x=\"385\" y=\"-156\" id=\"0\"/>\
	<connector x=\"385\" y=\"-181\" id=\"1\"/>\
	<connector x=\"301\" y=\"-197\" id=\"2\"/>\
	<connector anchor=\"true\" x=\"280\" y=\"-21\" id=\"3\"/>\
	<connector anchor=\"true\" x=\"97\" y=\"-359\" id=\"4\"/>\
	<connector x=\"286\" y=\"-257\" id=\"19\"/>\
	<connector x=\"186\" y=\"-227\" id=\"21\"/>\
	<connector x=\"96\" y=\"-233\" id=\"23\"/>\
	<connector x=\"51\" y=\"-265\" id=\"24\"/>\
	<connector x=\"385\" y=\"-364\" id=\"25\"/>\
	<connector x=\"230\" y=\"-294\" id=\"26\"/>\
	<Link id=\"0\">\
		<connector id=\"0\"/>\
		<connector id=\"1\"/>\
	</Link>\
	<Link id=\"1\">\
		<connector id=\"1\"/>\
		<connector id=\"2\"/>\
	</Link>\
	<Link id=\"2\">\
		<connector id=\"2\"/>\
		<connector id=\"3\"/>\
		<connector id=\"19\"/>\
	</Link>\
	<Link id=\"14\">\
		<connector id=\"21\"/>\
		<connector id=\"23\"/>\
		<connector id=\"4\"/>\
	</Link>\
	<Link id=\"16\">\
		<connector id=\"24\"/>\
		<connector id=\"25\"/>\
		<connector id=\"26\"/>\
	</Link>\
	<Link id=\"18\">\
		<connector id=\"23\"/>\
		<connector id=\"24\"/>\
	</Link>\
	<Link id=\"19\">\
		<connector id=\"21\"/>\
		<connector id=\"19\"/>\
		<connector id=\"26\"/>\
	</Link>\
</linkage2>\
"

#define JANSEN_XML "\
<linkage2>\
	<program zoom=\"1\" xoffset=\"270\" yoffset=\"210\" scalefactor=\"1\" units=\"Millimeters\"/>\
	<connector anchor=\"true\" input=\"true\" rpm=\"15\" x=\"384.245751\" y=\"-145.608179\" id=\"0\"/>\
	<connector x=\"352\" y=\"-118\" id=\"1\"/>\
	<connector anchor=\"true\" x=\"258.454035\" y=\"-163.759126\" id=\"2\"/>\
	<connector x=\"220.920794\" y=\"-79.237028\" id=\"3\"/>\
	<connector x=\"189.749768\" y=\"-282.257424\" id=\"4\"/>\
	<connector x=\"163.542305\" y=\"-192.212969\" id=\"5\"/>\
	<connector draw=\"true\" x=\"325.002325\" y=\"-386.623847\" id=\"6\"/>\
	<connector x=\"284.474312\" y=\"-252.118760\" id=\"7\"/>\
	<Link id=\"0\">\
		<connector id=\"0\"/>\
		<connector id=\"1\"/>\
	</Link>\
	<Link id=\"1\">\
		<connector id=\"2\"/>\
		<connector id=\"3\"/>\
		<connector id=\"5\"/>\
	</Link>\
	<Link id=\"2\">\
		<connector id=\"1\"/>\
		<connector id=\"3\"/>\
	</Link>\
	<Link id=\"3\">\
		<connector id=\"4\"/>\
		<connector id=\"5\"/>\
	</Link>\
	<Link id=\"6\">\
		<connector id=\"6\"/>\
		<connector id=\"7\"/>\
		<connector id=\"4\"/>\
	</Link>\
	<Link id=\"7\">\
		<connector id=\"2\"/>\
		<connector id=\"7\"/>\
	</Link>\
	<Link id=\"17\">\
		<connector id=\"1\"/>\
		<connector id=\"7\"/>\
	</Link>\
	<Link id=\"19\">\
		<connector id=\"0\"/>\
		<connector id=\"2\"/>\
	</Link>\
</linkage2>\
"

#define KLANN_XML "\
<linkage2>\
	<program zoom=\"1\" xoffset=\"315\" yoffset=\"281\" scalefactor=\"1\" units=\"Millimeters\"/>\
	<connector anchor=\"true\" x=\"384.188120\" y=\"-263.389741\" id=\"0\"/>\
	<connector x=\"389.964056\" y=\"-172.445336\" id=\"1\"/>\
	<connector anchor=\"true\" x=\"374.335829\" y=\"-342.089810\" id=\"2\"/>\
	<connector x=\"376.117415\" y=\"-281.360534\" id=\"3\"/>\
	<connector anchor=\"true\" input=\"true\" rpm=\"15\" x=\"471.970983\" y=\"-296.009544\" id=\"4\"/>\
	<connector x=\"470.517568\" y=\"-332.484023\" id=\"5\"/>\
	<connector draw=\"true\" x=\"158.277169\" y=\"-418.544812\" id=\"8\"/>\
	<connector x=\"313.573392\" y=\"-224.432992\" id=\"9\"/>\
	<Link id=\"0\">\
		<connector id=\"0\"/>\
		<connector id=\"1\"/>\
	</Link>\
	<Link id=\"1\">\
		<connector id=\"2\"/>\
		<connector id=\"3\"/>\
	</Link>\
	<Link id=\"2\">\
		<connector id=\"4\"/>\
		<connector id=\"5\"/>\
	</Link>\
	<Link id=\"3\">\
		<connector id=\"3\"/>\
		<connector id=\"5\"/>\
		<connector id=\"9\"/>\
	</Link>\
	<Link id=\"5\">\
		<connector id=\"8\"/>\
		<connector id=\"9\"/>\
		<connector id=\"1\"/>\
	</Link>\
</linkage2>\
"

#define JOULES_XML "\
<linkage2>\
	<program zoom=\"1\" xoffset=\"246\" yoffset=\"340\" scalefactor=\"1\" units=\"Millimeters\"/>\
	<connector anchor=\"true\" x=\"171.597360\" y=\"-290.478548\" id=\"0\"/>\
	<connector x=\"162.597360\" y=\"-192.478548\" id=\"1\"/>\
	<connector x=\"349\" y=\"-374\" id=\"2\"/>\
	<connector draw=\"true\" x=\"306\" y=\"-563\" id=\"3\"/>\
	<connector x=\"349.902640\" y=\"-330.021452\" id=\"5\"/>\
	<connector x=\"146\" y=\"-307\" id=\"6\"/>\
	<connector x=\"172.500000\" y=\"-246.500000\" id=\"8\"/>\
	<connector x=\"137\" y=\"-209\" id=\"9\"/>\
	<connector x=\"256.240568\" y=\"-163.972436\" id=\"11\"/>\
	<connector anchor=\"true\" input=\"true\" rpm=\"35\" x=\"256\" y=\"-187\" id=\"12\"/>\
	<Link id=\"0\">\
		<connector id=\"0\"/>\
		<connector id=\"1\"/>\
		<connector id=\"2\"/>\
	</Link>\
	<Link id=\"1\">\
		<connector id=\"3\"/>\
		<connector id=\"5\"/>\
		<connector id=\"2\"/>\
	</Link>\
	<Link id=\"2\">\
		<connector id=\"6\"/>\
		<connector id=\"8\"/>\
		<connector id=\"0\"/>\
	</Link>\
	<Link id=\"3\">\
		<connector id=\"9\"/>\
		<connector id=\"11\"/>\
		<connector id=\"1\"/>\
	</Link>\
	<Link id=\"4\">\
		<connector id=\"6\"/>\
		<connector id=\"9\"/>\
	</Link>\
	<Link id=\"5\">\
		<connector id=\"5\"/>\
		<connector id=\"8\"/>\
	</Link>\
	<Link id=\"6\">\
		<connector id=\"12\"/>\
		<connector id=\"11\"/>\
	</Link>\
</linkage2>\
"

#define HOEKEN_XML "\
<linkage2>\
	<program zoom=\"1\" xoffset=\"-128\" yoffset=\"0\" scalefactor=\"1\" units=\"Millimeters\"/>\
	<connector anchor=\"true\" input=\"true\" rpm=\"-5\" x=\"-136\" y=\"-119\" id=\"0\"/>\
	<connector x=\"-136\" y=\"-191\" id=\"1\"/>\
	<connector anchor=\"true\" x=\"8\" y=\"-119\" id=\"2\"/>\
	<connector x=\"-136\" y=\"-11\" id=\"3\"/>\
	<connector draw=\"true\" x=\"-136\" y=\"169\" id=\"5\"/>\
	<Link id=\"0\">\
		<connector id=\"0\"/>\
		<connector id=\"1\"/>\
	</Link>\
	<Link id=\"1\">\
		<connector id=\"2\"/>\
		<connector id=\"3\"/>\
	</Link>\
	<Link id=\"2\">\
		<connector id=\"5\"/>\
		<connector id=\"1\"/>\
		<connector id=\"3\"/>\
	</Link>\
</linkage2>\
"

#define EXAMPLE8_XML "\
<linkage2>\
	<program zoom=\"2\" xoffset=\"739\" yoffset=\"397\" scalefactor=\"1\" units=\"Millimeters\"/>\
	<connector anchor=\"true\" input=\"true\" rpm=\"10\" x=\"537\" y=\"-152\" id=\"0\"/>\
	<connector x=\"513\" y=\"-162.500000\" id=\"1\"/>\
	<connector x=\"426\" y=\"-199\" id=\"2\"/>\
	<connector x=\"374\" y=\"-226\" id=\"3\"/>\
	<connector anchor=\"true\" x=\"442\" y=\"-168\" id=\"4\"/>\
	<connector x=\"312\" y=\"-196\" id=\"5\"/>\
	<connector x=\"381\" y=\"-144.500000\" id=\"6\"/>\
	<connector x=\"352\" y=\"-125\" id=\"7\"/>\
	<connector x=\"425.500000\" y=\"-155\" id=\"8\"/>\
	<connector anchor=\"true\" x=\"363.500000\" y=\"-279\" id=\"9\"/>\
	<connector anchor=\"true\" x=\"320.500000\" y=\"-267.500000\" id=\"10\"/>\
	<connector draw=\"true\" x=\"283.125000\" y=\"-195.875000\" id=\"11\"/>\
	<connector draw=\"true\" x=\"283\" y=\"-175.750000\" id=\"12\"/>\
	<Link id=\"0\">\
		<connector id=\"0\"/>\
		<connector id=\"1\"/>\
	</Link>\
	<Link id=\"1\">\
		<connector id=\"2\"/>\
		<connector id=\"3\"/>\
		<connector id=\"4\"/>\
	</Link>\
	<Link id=\"2\">\
		<connector id=\"1\"/>\
		<connector id=\"2\"/>\
	</Link>\
	<Link id=\"3\">\
		<connector id=\"5\"/>\
		<connector id=\"6\"/>\
	</Link>\
	<Link id=\"4\">\
		<connector id=\"3\"/>\
		<connector id=\"5\"/>\
		<connector id=\"6\"/>\
	</Link>\
	<Link id=\"5\">\
		<connector id=\"6\"/>\
		<connector id=\"7\"/>\
		<connector id=\"8\"/>\
	</Link>\
	<Link id=\"6\">\
		<connector id=\"9\"/>\
		<connector id=\"8\"/>\
	</Link>\
	<Link id=\"7\">\
		<connector id=\"10\"/>\
		<connector id=\"6\"/>\
	</Link>\
	<Link id=\"8\">\
		<connector id=\"11\"/>\
		<connector id=\"12\"/>\
		<connector id=\"5\"/>\
	</Link>\
	<Link id=\"9\">\
		<connector id=\"7\"/>\
		<connector id=\"12\"/>\
	</Link>\
</linkage2>\
"

#define CLOSER_XML "\
<linkage2>\
	<connector x=\"572.598361\" y=\"-427.418033\" id=\"0\"/>\
	<connector x=\"641.323770\" y=\"-394.731557\" id=\"1\"/>\
	<connector anchor=\"true\" x=\"643\" y=\"-471\" id=\"2\"/>\
	<connector x=\"379\" y=\"-520.750000\" id=\"3\"/>\
	<connector x=\"427.766393\" y=\"-511.817623\" id=\"4\"/>\
	<connector draw=\"true\" x=\"293.668033\" y=\"-541.989754\" id=\"5\"/>\
	<connector x=\"701.668033\" y=\"-307.567623\" id=\"8\"/>\
	<connector anchor=\"true\" x=\"598.579918\" y=\"-424.065574\" id=\"11\"/>\
	<connector anchor=\"true\" input=\"true\" rpm=\"3\" x=\"601\" y=\"-698\" id=\"13\"/>\
	<connector x=\"630.448770\" y=\"-595.497951\" id=\"14\"/>\
	<connector draw=\"true\" x=\"318.750000\" y=\"-536.500000\" id=\"15\"/>\
	<Link id=\"0\">\
		<connector id=\"0\"/>\
		<connector id=\"1\"/>\
		<connector id=\"2\"/>\
	</Link>\
	<Link id=\"1\">\
		<connector id=\"3\"/>\
		<connector id=\"4\"/>\
		<connector id=\"5\"/>\
		<connector id=\"15\"/>\
	</Link>\
	<Link id=\"2\">\
		<connector id=\"8\"/>\
		<connector id=\"4\"/>\
		<connector id=\"1\"/>\
	</Link>\
	<Link id=\"4\">\
		<connector id=\"0\"/>\
		<connector id=\"3\"/>\
	</Link>\
	<Link id=\"7\">\
		<connector id=\"8\"/>\
		<connector id=\"11\"/>\
	</Link>\
	<Link id=\"8\">\
		<connector id=\"13\"/>\
		<connector id=\"14\"/>\
	</Link>\
	<Link id=\"10\">\
		<connector id=\"8\"/>\
		<connector id=\"14\"/>\
	</Link>\
</linkage2>\
"

#define FOLDINGTOP_XML "\
<linkage2>\
	<program zoom=\"1\" xoffset=\"433\" yoffset=\"333\" scalefactor=\"1\" units=\"Millimeters\"/>\
	<connector anchor=\"true\" x=\"534\" y=\"-474\" id=\"0\"/>\
	<connector x=\"507\" y=\"-241\" id=\"1\"/>\
	<connector anchor=\"true\" x=\"510\" y=\"-414\" id=\"2\"/>\
	<connector x=\"502\" y=\"-322\" id=\"3\"/>\
	<connector anchor=\"true\" x=\"521\" y=\"-446\" id=\"4\"/>\
	<connector x=\"552\" y=\"-372\" id=\"5\"/>\
	<connector x=\"448.500000\" y=\"-240.500000\" id=\"7\"/>\
	<connector x=\"306\" y=\"-266\" id=\"8\"/>\
	<connector x=\"281\" y=\"-246\" id=\"11\"/>\
	<connector draw=\"true\" x=\"212\" y=\"-273\" id=\"12\"/>\
	<connector draw=\"true\" x=\"267\" y=\"-273.500000\" id=\"13\"/>\
	<connector x=\"439\" y=\"-194\" id=\"15\"/>\
	<connector x=\"279\" y=\"-235\" id=\"17\"/>\
	<connector anchor=\"true\" input=\"true\" rpm=\"3\" x=\"620\" y=\"-549\" id=\"18\"/>\
	<connector x=\"598\" y=\"-492\" id=\"19\"/>\
	<Link id=\"0\">\
		<connector id=\"0\"/>\
		<connector id=\"1\"/>\
	</Link>\
	<Link id=\"1\">\
		<connector id=\"2\"/>\
		<connector id=\"3\"/>\
		<connector id=\"7\"/>\
		<connector id=\"15\"/>\
	</Link>\
	<Link id=\"2\">\
		<connector id=\"4\"/>\
		<connector id=\"5\"/>\
	</Link>\
	<Link id=\"4\">\
		<connector id=\"5\"/>\
		<connector id=\"3\"/>\
	</Link>\
	<Link id=\"6\">\
		<connector id=\"8\"/>\
		<connector id=\"7\"/>\
		<connector id=\"1\"/>\
		<connector id=\"11\"/>\
	</Link>\
	<Link id=\"8\">\
		<connector id=\"12\"/>\
		<connector id=\"13\"/>\
		<connector id=\"11\"/>\
	</Link>\
	<Link id=\"10\">\
		<connector id=\"17\"/>\
		<connector id=\"13\"/>\
	</Link>\
	<Link id=\"11\">\
		<connector id=\"8\"/>\
		<connector id=\"17\"/>\
	</Link>\
	<Link id=\"12\">\
		<connector id=\"15\"/>\
		<connector id=\"17\"/>\
	</Link>\
	<Link id=\"13\">\
		<connector id=\"18\"/>\
		<connector id=\"19\"/>\
	</Link>\
	<Link id=\"14\">\
		<connector id=\"5\"/>\
		<connector id=\"19\"/>\
	</Link>\
</linkage2>\
"

#define SLIDERDEMO_XML "\
<linkage2>\
	<program zoom=\"1\" xoffset=\"15\" yoffset=\"54\"/>\
	<connector anchor=\"true\" input=\"true\" rpm=\"15\" x=\"71\" y=\"-72\" id=\"0\"/>\
	<connector x=\"72\" y=\"-48.500000\" id=\"1\" slider=\"true\">\
		<slidelimit id=\"4\"/>\
		<slidelimit id=\"5\"/>\
	</connector>\
	<connector anchor=\"true\" x=\"-59\" y=\"-71\" id=\"4\"/>\
	<connector x=\"203\" y=\"-26\" id=\"5\"/>\
	<Link id=\"0\">\
		<connector id=\"0\"/>\
		<connector id=\"1\"/>\
	</Link>\
	<Link id=\"2\">\
		<connector id=\"4\"/>\
		<connector id=\"5\"/>\
	</Link>\
</linkage2>\
"

#define SLIDERDEMO2_XML "\
<linkage2>\
	<program zoom=\"1\"xoffset=\"81\" yoffset=\"307\"/>\
	<connector anchor=\"true\" input=\"true\" rpm=\"15\" x=\"-71\" y=\"-362\" id=\"0\"/>\
	<connector x=\"-90\" y=\"-346\" id=\"1\"/>\
	<connector anchor=\"true\" x=\"-8\" y=\"-275\" id=\"2\"/>\
	<connector x=\"206\" y=\"-342\" id=\"3\"/>\
	<connector anchor=\"true\" x=\"251\" y=\"-275\" id=\"4\"/>\
	<connector x=\"116.196429\" y=\"-313.883929\" id=\"5\" slider=\"true\">\
		<slidelimit id=\"2\"/>\
		<slidelimit id=\"3\"/>\
	</connector>\
	<connector x=\"-60\" y=\"-260\" id=\"6\"/>\
	<Link id=\"0\">\
		<connector id=\"0\"/>\
		<connector id=\"1\"/>\
	</Link>\
	<Link id=\"1\">\
		<connector id=\"2\"/>\
		<connector id=\"3\"/>\
		<connector id=\"6\"/>\
	</Link>\
	<Link id=\"3\">\
		<connector id=\"4\"/>\
		<connector id=\"5\"/>\
	</Link>\
	<Link id=\"5\">\
		<connector id=\"1\"/>\
		<connector id=\"6\"/>\
	</Link>\
</linkage2>\
"

#define SLIDERDEMO3_XML "\
<linkage2>\
	<program zoom=\"1\" xoffset=\"154\" yoffset=\"71\"/>\
	<connector x=\"238\" y=\"-237\" id=\"0\"/>\
	<connector x=\"340\" y=\"-192\" id=\"1\"/>\
	<connector x=\"430\" y=\"-237\" id=\"2\"/>\
	<connector anchor=\"true\" x=\"298\" y=\"-237\" id=\"3\" slider=\"true\">\
		<slidelimit id=\"0\"/>\
		<slidelimit id=\"2\"/>\
	</connector>\
	<connector anchor=\"true\" x=\"374.896861\" y=\"-237\" id=\"4\" slider=\"true\">\
		<slidelimit id=\"0\"/>\
		<slidelimit id=\"2\"/>\
	</connector>\
	<connector anchor=\"true\" input=\"true\" rpm=\"15\" x=\"534\" y=\"-187\" id=\"5\"/>\
	<connector x=\"534\" y=\"-150\" id=\"6\"/>\
	<Link id=\"0\">\
		<connector id=\"0\"/>\
		<connector id=\"1\"/>\
		<connector id=\"2\"/>\
	</Link>\
	<Link id=\"1\">\
		<connector id=\"3\"/>\
	</Link>\
	<Link id=\"2\">\
		<connector id=\"4\"/>\
	</Link>\
	<Link id=\"3\">\
		<connector id=\"5\"/>\
		<connector id=\"6\"/>\
	</Link>\
	<Link id=\"4\">\
		<connector id=\"1\"/>\
		<connector id=\"6\"/>\
	</Link>\
</linkage2>\
"

#define SLIDERDEMO4_XML "\
<linkage2>\
	<program zoom=\"1\" xoffset=\"384\" yoffset=\"235\"/>\
	<connector x=\"238\" y=\"-237\" id=\"0\"/>\
	<connector x=\"339\" y=\"-192\" id=\"1\"/>\
	<connector x=\"430\" y=\"-237\" id=\"2\"/>\
	<connector anchor=\"true\" x=\"298\" y=\"-237\" id=\"3\" slider=\"true\">\
		<slidelimit id=\"0\"/>\
		<slidelimit id=\"2\"/>\
	</connector>\
	<connector x=\"352\" y=\"-237\" id=\"4\" slider=\"true\">\
		<slidelimit id=\"0\"/>\
		<slidelimit id=\"2\"/>\
	</connector>\
	<connector anchor=\"true\" input=\"true\" rpm=\"15\" x=\"534\" y=\"-187\" id=\"5\"/>\
	<connector x=\"534\" y=\"-150\" id=\"6\"/>\
	<connector anchor=\"true\" x=\"342\" y=\"-282\" id=\"12\"/>\
	<connector x=\"394\" y=\"-334\" id=\"14\"/>\
	<connector anchor=\"true\" input=\"true\" rpm=\"9\" x=\"292\" y=\"-348\" id=\"15\"/>\
	<connector x=\"299\" y=\"-366\" id=\"16\"/>\
	<Link id=\"0\">\
		<connector id=\"0\"/>\
		<connector id=\"1\"/>\
		<connector id=\"2\"/>\
	</Link>\
	<Link id=\"1\">\
		<connector id=\"3\"/>\
	</Link>\
	<Link id=\"2\">\
		<connector id=\"4\"/>\
		<connector id=\"12\"/>\
		<connector id=\"14\"/>\
	</Link>\
	<Link id=\"3\">\
		<connector id=\"5\"/>\
		<connector id=\"6\"/>\
	</Link>\
	<Link id=\"4\">\
		<connector id=\"1\"/>\
		<connector id=\"6\"/>\
	</Link>\
	<Link id=\"10\">\
		<connector id=\"15\"/>\
		<connector id=\"16\"/>\
	</Link>\
	<Link id=\"11\">\
		<connector id=\"14\"/>\
		<connector id=\"16\"/>\
	</Link>\
</linkage2>\
"

#define SLIDERDEMO5_XML "\
<linkage2>\
	<program zoom=\"1\" xoffset=\"-14\" yoffset=\"79\"/>\
	<connector x=\"238\" y=\"-236\" id=\"0\"/>\
	<connector x=\"339\" y=\"-192\" id=\"1\"/>\
	<connector x=\"437\" y=\"-236\" id=\"2\" slider=\"true\">\
		<slidelimit id=\"19\"/>\
		<slidelimit id=\"20\"/>\
	</connector>\
	<connector anchor=\"true\" x=\"301.982011\" y=\"-236\" id=\"3\" slider=\"true\">\
		<slidelimit id=\"0\"/>\
		<slidelimit id=\"2\"/>\
	</connector>\
	<connector anchor=\"true\" x=\"361.951470\" y=\"-236\" id=\"4\" slider=\"true\">\
		<slidelimit id=\"0\"/>\
		<slidelimit id=\"2\"/>\
	</connector>\
	<connector anchor=\"true\" input=\"true\" rpm=\"15\" x=\"605\" y=\"-190\" id=\"5\"/>\
	<connector x=\"605\" y=\"-130\" id=\"6\"/>\
	<connector anchor=\"true\" x=\"437\" y=\"-309\" id=\"19\"/>\
	<connector x=\"437\" y=\"-205\" id=\"20\"/>\
	<Link id=\"0\">\
		<connector id=\"0\"/>\
		<connector id=\"1\"/>\
		<connector id=\"2\"/>\
	</Link>\
	<Link id=\"1\">\
		<connector id=\"3\"/>\
	</Link>\
	<Link id=\"2\">\
		<connector id=\"4\"/>\
	</Link>\
	<Link id=\"3\">\
		<connector id=\"5\"/>\
		<connector id=\"6\"/>\
	</Link>\
	<Link id=\"13\">\
		<connector id=\"19\"/>\
		<connector id=\"20\"/>\
	</Link>\
	<Link id=\"14\">\
		<connector id=\"6\"/>\
		<connector id=\"20\"/>\
	</Link>\
</linkage2>\
"

#define SLIDERDEMO6_XML "\
<linkage2>\
	<program zoom=\"1\" xoffset=\"252\" yoffset=\"288\"/>\
	<connector x=\"75\" y=\"-426\" id=\"21\"/>\
	<connector x=\"176\" y=\"-382\" id=\"22\"/>\
	<connector x=\"276\" y=\"-426\" id=\"23\"/>\
	<connector anchor=\"true\" x=\"138.982011\" y=\"-426\" id=\"24\" slider=\"true\">\
		<slidelimit id=\"21\"/>\
		<slidelimit id=\"23\"/>\
	</connector>\
	<connector anchor=\"true\" x=\"198.951470\" y=\"-426\" id=\"25\" slider=\"true\">\
		<slidelimit id=\"21\"/>\
		<slidelimit id=\"23\"/>\
	</connector>\
	<connector anchor=\"true\" input=\"true\" rpm=\"15\" x=\"442\" y=\"-413\" id=\"26\"/>\
	<connector x=\"443\" y=\"-365\" id=\"27\"/>\
	<connector x=\"72\" y=\"-225\" id=\"42\"/>\
	<connector x=\"173\" y=\"-181\" id=\"43\"/>\
	<connector x=\"271\" y=\"-225\" id=\"44\" slider=\"true\">\
		<slidelimit id=\"61\"/>\
		<slidelimit id=\"62\"/>\
	</connector>\
	<connector x=\"135.982011\" y=\"-225\" id=\"45\" slider=\"true\">\
		<slidelimit id=\"42\"/>\
		<slidelimit id=\"44\"/>\
	</connector>\
	<connector anchor=\"true\" x=\"195.951470\" y=\"-225\" id=\"46\" slider=\"true\">\
		<slidelimit id=\"42\"/>\
		<slidelimit id=\"44\"/>\
	</connector>\
	<connector anchor=\"true\" input=\"true\" rpm=\"15\" x=\"439\" y=\"-179\" id=\"47\"/>\
	<connector x=\"439\" y=\"-119\" id=\"48\"/>\
	<connector anchor=\"true\" x=\"271\" y=\"-298\" id=\"61\"/>\
	<connector x=\"271\" y=\"-194\" id=\"62\"/>\
	<connector anchor=\"true\" rpm=\"15\" x=\"71\" y=\"-266\" id=\"63\"/>\
	<connector x=\"162\" y=\"-266\" id=\"64\"/>\
	<Link id=\"15\">\
		<connector id=\"21\"/>\
		<connector id=\"22\"/>\
		<connector id=\"23\"/>\
	</Link>\
	<Link id=\"16\">\
		<connector id=\"24\"/>\
	</Link>\
	<Link id=\"17\">\
		<connector id=\"25\"/>\
	</Link>\
	<Link id=\"18\">\
		<connector id=\"26\"/>\
		<connector id=\"27\"/>\
	</Link>\
	<Link id=\"29\">\
		<connector id=\"27\"/>\
		<connector id=\"23\"/>\
	</Link>\
	<Link id=\"30\">\
		<connector id=\"42\"/>\
		<connector id=\"43\"/>\
		<connector id=\"44\"/>\
	</Link>\
	<Link id=\"31\">\
		<connector id=\"45\"/>\
		<connector id=\"63\"/>\
		<connector id=\"64\"/>\
	</Link>\
	<Link id=\"32\">\
		<connector id=\"46\"/>\
	</Link>\
	<Link id=\"33\">\
		<connector id=\"47\"/>\
		<connector id=\"48\"/>\
	</Link>\
	<Link id=\"43\">\
		<connector id=\"61\"/>\
		<connector id=\"62\"/>\
	</Link>\
	<Link id=\"44\">\
		<connector id=\"48\"/>\
		<connector id=\"62\"/>\
	</Link>\
	<Link id=\"46\">\
		<connector id=\"22\"/>\
		<connector id=\"64\"/>\
	</Link>\
</linkage2>\
"

#define SLIDERSMILE_XML "\
<linkage2>\
	<program zoom=\"1\" xoffset=\"370\" yoffset=\"256\"/>\
	<connector anchor=\"true\" x=\"320.894936\" y=\"3.097270\" id=\"4\" slider=\"true\">\
		<slidelimit id=\"7\"/>\
		<slidelimit id=\"9\"/>\
	</connector>\
	<connector anchor=\"true\" x=\"180\" y=\"-29\" id=\"6\" />\
	<connector x=\"262\" y=\"-65\" id=\"7\" />\
	<connector x=\"358\" y=\"46\" id=\"9\" />\
	<connector anchor=\"true\" input=\"true\" rpm=\"15\" x=\"252\" y=\"-142\" id=\"12\" />\
	<connector x=\"247\" y=\"-181\" id=\"13\" />\
	<connector x=\"365\" y=\"-43\" id=\"15\" />\
	<connector x=\"710\" y=\"-307\" id=\"17\" />\
	<connector anchor=\"true\" input=\"true\" rpm=\"15\" x=\"337\" y=\"-389\" id=\"37\" />\
	<connector draw=\"true\" x=\"334\" y=\"-365\" id=\"38\" />\
	<connector anchor=\"true\" input=\"true\" rpm=\"18\" x=\"393\" y=\"-389\" id=\"39\" />\
	<connector draw=\"true\" x=\"416\" y=\"-399\" id=\"40\" />\
	<connector anchor=\"true\" input=\"true\" rpm=\"-15\" x=\"845\" y=\"-304\" id=\"46\" />\
	<connector x=\"844\" y=\"-281\" id=\"47\" />\
	<connector x=\"365\" y=\"-306.750000\" id=\"2\" slider=\"true\">\
		<slidelimit id=\"5\"/>\
		<slidelimit id=\"8\"/>\
	</connector>\
	<connector draw=\"true\" x=\"365\" y=\"-446\" id=\"5\" />\
	<connector x=\"365\" y=\"-194\" id=\"8\" />\
	<connector anchor=\"true\" x=\"611.541250\" y=\"-306.928653\" id=\"0\" slider=\"true\">\
		<slidelimit id=\"17\"/>\
		<slidelimit id=\"2\"/>\
	</connector>\
	<connector anchor=\"true\" x=\"478.245948\" y=\"-306.832062\" id=\"1\" slider=\"true\">\
		<slidelimit id=\"17\"/>\
		<slidelimit id=\"2\"/>\
	</connector>\
	<connector anchor=\"true\" x=\"365\" y=\"-110.891473\" id=\"3\" slider=\"true\">\
		<slidelimit id=\"15\"/>\
		<slidelimit id=\"8\"/>\
	</connector>\
	<connector anchor=\"true\" x=\"365\" y=\"-144.837209\" id=\"10\" slider=\"true\">\
		<slidelimit id=\"15\"/>\
		<slidelimit id=\"8\"/>\
	</connector>\
	<Link id=\"3\" >\
		<connector id=\"4\"/>\
	</Link>\
	<Link id=\"4\" >\
		<connector id=\"6\"/>\
		<connector id=\"7\"/>\
	</Link>\
	<Link id=\"5\" >\
		<connector id=\"9\"/>\
		<connector id=\"7\"/>\
	</Link>\
	<Link id=\"7\" >\
		<connector id=\"12\"/>\
		<connector id=\"13\"/>\
	</Link>\
	<Link id=\"8\" >\
		<connector id=\"7\"/>\
		<connector id=\"13\"/>\
	</Link>\
	<Link id=\"10\" >\
		<connector id=\"9\"/>\
		<connector id=\"15\"/>\
	</Link>\
	<Link id=\"28\" >\
		<connector id=\"37\"/>\
		<connector id=\"38\"/>\
	</Link>\
	<Link id=\"29\" >\
		<connector id=\"39\"/>\
		<connector id=\"40\"/>\
	</Link>\
	<Link id=\"32\" >\
		<connector id=\"46\"/>\
		<connector id=\"47\"/>\
	</Link>\
	<Link id=\"34\" >\
		<connector id=\"17\"/>\
		<connector id=\"47\"/>\
	</Link>\
	<Link id=\"2\" >\
		<connector id=\"2\"/>\
		<connector id=\"17\"/>\
	</Link>\
	<Link id=\"6\" >\
		<connector id=\"5\"/>\
		<connector id=\"8\"/>\
	</Link>\
	<Link id=\"0\" >\
		<connector id=\"0\"/>\
	</Link>\
	<Link id=\"1\" >\
		<connector id=\"1\"/>\
	</Link>\
	<Link id=\"0\" >\
		<connector id=\"15\"/>\
		<connector id=\"8\"/>\
	</Link>\
	<Link id=\"9\" >\
		<connector id=\"3\"/>\
	</Link>\
	<Link id=\"11\" >\
		<connector id=\"10\"/>\
	</Link>\
</linkage2>\
"

#define SLIDERDEMO7_XML "\
<linkage2>\
	<program zoom=\"1\" xoffset=\"542\" yoffset=\"451\"/>\
	<connector anchor=\"true\" input=\"true\" rpm=\"15\" x=\"398\" y=\"-392\" id=\"0\" />\
	<connector x=\"422\" y=\"-382\" id=\"1\" />\
	<connector anchor=\"true\" x=\"471\" y=\"-557\" id=\"2\" />\
	<connector anchor=\"true\" x=\"424\" y=\"-446\" id=\"3\" />\
	<connector x=\"447.500000\" y=\"-501.500000\" id=\"4\" slider=\"true\">\
		<slidelimit id=\"2\"/>\
		<slidelimit id=\"3\"/>\
	</connector>\
	<connector anchor=\"true\" x=\"530\" y=\"-622\" id=\"5\" />\
	<connector anchor=\"true\" x=\"566\" y=\"-414\" id=\"6\" />\
	<connector x=\"548\" y=\"-518\" id=\"7\" slider=\"true\">\
		<slidelimit id=\"5\"/>\
		<slidelimit id=\"6\"/>\
	</connector>\
	<connector anchor=\"true\" input=\"true\" rpm=\"15.700000\" x=\"590\" y=\"-281\" id=\"8\" />\
	<connector x=\"637\" y=\"-299\" id=\"9\" />\
	<connector x=\"475\" y=\"-608\" id=\"10\" />\
	<connector anchor=\"true\" x=\"587.991306\" y=\"-607.008848\" id=\"11\" slider=\"true\">\
		<slidelimit id=\"10\"/>\
		<slidelimit id=\"13\"/>\
	</connector>\
	<connector draw=\"true\" x=\"817\" y=\"-605\" id=\"13\" />\
	<Link id=\"0\" >\
		<connector id=\"0\"/>\
		<connector id=\"1\"/>\
	</Link>\
	<Link id=\"1\" solid=\"true\">\
		<connector id=\"2\"/>\
		<connector id=\"3\"/>\
	</Link>\
	<Link id=\"0\" >\
		<connector id=\"1\"/>\
		<connector id=\"4\"/>\
	</Link>\
	<Link id=\"2\" solid=\"true\">\
		<connector id=\"5\"/>\
		<connector id=\"6\"/>\
	</Link>\
	<Link id=\"4\" >\
		<connector id=\"8\"/>\
		<connector id=\"9\"/>\
	</Link>\
	<Link id=\"0\" >\
		<connector id=\"7\"/>\
		<connector id=\"9\"/>\
	</Link>\
	<Link id=\"3\" >\
		<connector id=\"10\"/>\
		<connector id=\"7\"/>\
	</Link>\
	<Link id=\"0\" >\
		<connector id=\"4\"/>\
		<connector id=\"10\"/>\
	</Link>\
	<Link id=\"5\" >\
		<connector id=\"11\"/>\
	</Link>\
	<Link id=\"6\" >\
		<connector id=\"13\"/>\
		<connector id=\"10\"/>\
	</Link>\
</linkage2>\
"

#define ACTUATORDEMO1_XML "\
<linkage2>\
	<program zoom=\"1\" xoffset=\"392\" yoffset=\"240\"/>\
	<connector x=\"238\" y=\"-237\" id=\"0\" />\
	<connector x=\"349\" y=\"-198\" id=\"1\" />\
	<connector x=\"430\" y=\"-249\" id=\"2\" />\
	<connector anchor=\"true\" x=\"298\" y=\"-240.750000\" id=\"3\" slider=\"true\" >\
		<slidelimit id=\"0\"/>\
		<slidelimit id=\"2\"/>\
	</connector>\
	<connector x=\"354\" y=\"-244.250000\" id=\"4\" slider=\"true\" >\
		<slidelimit id=\"0\"/>\
		<slidelimit id=\"2\"/>\
	</connector>\
	<connector anchor=\"true\" x=\"398\" y=\"-253\" id=\"12\" />\
	<connector x=\"400\" y=\"-298\" id=\"14\" />\
	<connector anchor=\"true\" x=\"549\" y=\"-298\" id=\"7\" />\
	<connector anchor=\"true\" x=\"538.500000\" y=\"-197\" id=\"5\" />\
	<Link id=\"0\"  >\
		<connector id=\"0\"/>\
		<connector id=\"1\"/>\
		<connector id=\"2\"/>\
	</Link>\
	<Link id=\"1\"  >\
		<connector id=\"3\"/>\
	</Link>\
	<Link id=\"2\"  >\
		<connector id=\"4\"/>\
		<connector id=\"12\"/>\
		<connector id=\"14\"/>\
	</Link>\
	<Link id=\"5\"  actuator=\"true\" throw=\"21.213203\" cpm=\"9\" >\
		<connector id=\"7\"/>\
		<connector id=\"14\"/>\
	</Link>\
	<Link id=\"3\"  actuator=\"true\" throw=\"21.213203\" cpm=\"15\" >\
		<connector id=\"5\"/>\
		<connector id=\"1\"/>\
	</Link>\
</linkage2>\
"

#define ACTUATORDEMO2_XML "\
<linkage2>\
	<program zoom=\"1\" xoffset=\"311\" yoffset=\"276\"/>\
	<connector anchor=\"true\" x=\"207\" y=\"-237\" id=\"0\" />\
	<connector x=\"316.500000\" y=\"-306\" id=\"1\" slider=\"true\" >\
		<slidelimit id=\"3\"/>\
		<slidelimit id=\"4\"/>\
	</connector>\
	<connector x=\"427.500000\" y=\"-241\" id=\"2\" />\
	<connector anchor=\"true\" x=\"250\" y=\"-306\" id=\"3\" />\
	<connector anchor=\"true\" x=\"383\" y=\"-306\" id=\"4\" />\
	<connector anchor=\"true\" x=\"436\" y=\"-207\" id=\"5\" />\
	<Link id=\"0\"  actuator=\"true\" throw=\"21.213203\" cpm=\"15\" >\
		<connector id=\"0\"/>\
		<connector id=\"1\"/>\
	</Link>\
	<Link id=\"1\" >\
		<connector id=\"2\"/>\
		<connector id=\"1\"/>\
	</Link>\
	<Link id=\"2\" >\
		<connector id=\"3\"/>\
		<connector id=\"4\"/>\
	</Link>\
	<Link id=\"3\" >\
		<connector id=\"5\"/>\
		<connector id=\"2\"/>\
	</Link>\
</linkage2>\
"

#define ACTUATORDEMO3_XML "\
<linkage2>\
	<program zoom=\"1.300000\" xoffset=\"890\" yoffset=\"618\" scalefactor=\"1.000000\" units=\"Inches\" viewlayers=\"2147483647\" editlayers=\"2147483647\"/>\
	<connector id=\"0\" layer=\"16\" x=\"406.436924\" y=\"-452.091481\" color=\"16711680\"/>\
	<connector id=\"1\" layer=\"16\" x=\"427.633792\" y=\"-410.630648\" color=\"12632064\"/>\
	<connector id=\"2\" layer=\"16\" x=\"481.892668\" y=\"-461.388292\" color=\"32768\"/>\
	<connector id=\"3\" layer=\"16\" x=\"481.951902\" y=\"-426.828343\" color=\"16711808\"/>\
	<connector id=\"4\" layer=\"16\" x=\"498.145152\" y=\"-457.827101\" color=\"6854344\"/>\
	<connector id=\"5\" layer=\"16\" x=\"528.238360\" y=\"-440.833207\" color=\"11012476\"/>\
	<connector id=\"6\" layer=\"16\" x=\"741.016455\" y=\"-439.031681\" color=\"16744448\"/>\
	<connector id=\"7\" layer=\"16\" x=\"711.950227\" y=\"-445.741325\" color=\"11272364\"/>\
	<connector id=\"8\" layer=\"16\" x=\"917.163206\" y=\"-447.056748\" color=\"8439808\"/>\
	<connector id=\"10\" layer=\"16\" x=\"918.262062\" y=\"-517.000000\" anchor=\"true\" color=\"12615935\"/>\
	<connector id=\"12\" layer=\"16\" x=\"712.261081\" y=\"-430.192648\" color=\"223719\"/>\
	<connector id=\"9\" layer=\"16\" x=\"953.500000\" y=\"-517.000000\" anchor=\"true\" color=\"13123282\"/>\
	<connector id=\"13\" layer=\"16\" x=\"482.422369\" y=\"-401.172218\" color=\"13123648\"/>\
	<Link id=\"0\" layer=\"16\" linesize=\"1\" color=\"16711680\">\
		<connector id=\"0\"/>\
		<connector id=\"1\"/>\
		<connector id=\"2\"/>\
		<connector id=\"3\"/>\
		<connector id=\"13\"/>\
	</Link>\
	<Link id=\"1\" layer=\"16\" linesize=\"1\" color=\"12632064\">\
		<connector id=\"4\"/>\
		<connector id=\"5\"/>\
	</Link>\
	<Link id=\"2\" layer=\"16\" linesize=\"1\" color=\"32768\">\
		<connector id=\"3\"/>\
		<connector id=\"5\"/>\
	</Link>\
	<Link id=\"3\" layer=\"16\" linesize=\"1\" color=\"16711808\">\
		<connector id=\"2\"/>\
		<connector id=\"4\"/>\
		<connector id=\"6\"/>\
		<connector id=\"7\"/>\
		<connector id=\"12\"/>\
	</Link>\
	<Link id=\"4\" layer=\"16\" linesize=\"1\" color=\"6854344\">\
		<connector id=\"7\"/>\
		<connector id=\"8\"/>\
		<connector id=\"10\"/>\
	</Link>\
	<Link id=\"5\" layer=\"16\" linesize=\"1\" actuator=\"true\" throw=\"54.008751\" cpm=\"5.000000\" color=\"11012476\">\
		<connector id=\"6\"/>\
		<connector id=\"8\"/>\
	</Link>\
	<Link id=\"6\" layer=\"16\" linesize=\"1\" actuator=\"true\" throw=\"64.800000\" cpm=\"5.000000\" color=\"16744448\">\
		<connector id=\"5\"/>\
		<connector id=\"12\"/>\
	</Link>\
	<Link id=\"7\" layer=\"16\" linesize=\"1\" actuator=\"true\" throw=\"30.644311\" cpm=\"-5.000000\" color=\"11272364\">\
		<connector id=\"9\"/>\
		<connector id=\"8\"/>\
	</Link>\
</linkage2>\
"

#define FANCYEXAMPLE_XML "\
<linkage2>\
	<program zoom=\"1.000000\" xoffset=\"506\" yoffset=\"248\" scalefactor=\"1.000000\" units=\"Millimeters\" viewlayers=\"2147483647\" editlayers=\"2147483647\"/>\
	<connector id=\"0\" selected=\"true\" layer=\"16\" x=\"228.000000\" y=\"-235.000000\" draw=\"true\" color=\"16711680\"/>\
	<connector id=\"1\" selected=\"true\" layer=\"16\" x=\"358.000000\" y=\"-186.000000\" color=\"12632064\"/>\
	<connector id=\"2\" selected=\"true\" layer=\"16\" x=\"442.000000\" y=\"-235.000000\" color=\"32768\"/>\
	<connector id=\"3\" selected=\"true\" layer=\"16\" x=\"327.000000\" y=\"-235.000000\" slider=\"true\" color=\"16711808\">\
		<slidelimit id=\"0\"/>\
		<slidelimit id=\"2\"/>\
	</connector>\
	<connector id=\"4\" selected=\"true\" layer=\"16\" x=\"370.000000\" y=\"-235.000000\" slider=\"true\" color=\"6854344\">\
		<slidelimit id=\"0\"/>\
		<slidelimit id=\"2\"/>\
	</connector>\
	<connector id=\"12\" selected=\"true\" layer=\"16\" x=\"419.000000\" y=\"-273.000000\" anchor=\"true\" color=\"223719\"/>\
	<connector id=\"14\" selected=\"true\" layer=\"16\" x=\"375.000000\" y=\"-322.000000\" color=\"4243584\"/>\
	<connector id=\"7\" selected=\"true\" layer=\"16\" x=\"312.000000\" y=\"-320.000000\" color=\"11272364\"/>\
	<connector id=\"8\" selected=\"true\" layer=\"16\" x=\"255.000000\" y=\"-272.000000\" anchor=\"true\" color=\"8439808\"/>\
	<connector id=\"9\" selected=\"true\" layer=\"16\" x=\"698.000000\" y=\"-339.575758\" anchor=\"true\" input=\"true\" rpm=\"15.000000\" color=\"13123282\"/>\
	<connector id=\"10\" selected=\"true\" layer=\"16\" x=\"698.000000\" y=\"-320.000000\" color=\"12615935\"/>\
	<connector id=\"6\" selected=\"true\" layer=\"16\" x=\"451.000000\" y=\"-272.000000\" color=\"16744448\"/>\
	<connector id=\"11\" selected=\"true\" layer=\"16\" x=\"695.000000\" y=\"-263.575758\" color=\"2146336\"/>\
	<connector id=\"13\" selected=\"true\" layer=\"16\" x=\"525.638904\" y=\"-269.423048\" slider=\"true\" color=\"13123648\">\
		<slidelimit id=\"6\"/>\
		<slidelimit id=\"11\"/>\
	</connector>\
	<connector id=\"15\" selected=\"true\" layer=\"16\" x=\"561.000000\" y=\"-200.000000\" color=\"8421440\"/>\
	<connector id=\"18\" selected=\"true\" layer=\"16\" x=\"589.289530\" y=\"-267.225473\" slider=\"true\" color=\"6324424\">\
		<slidelimit id=\"6\"/>\
		<slidelimit id=\"11\"/>\
	</connector>\
	<connector id=\"17\" selected=\"true\" layer=\"16\" x=\"650.096751\" y=\"-185.718339\" anchor=\"true\" slider=\"true\" color=\"2121760\">\
		<slidelimit id=\"21\"/>\
		<slidelimit id=\"23\"/>\
	</connector>\
	<connector id=\"19\" selected=\"true\" layer=\"16\" x=\"705.853436\" y=\"-184.962976\" anchor=\"true\" slider=\"true\" color=\"8421440\">\
		<slidelimit id=\"21\"/>\
		<slidelimit id=\"23\"/>\
	</connector>\
	<connector id=\"21\" selected=\"true\" layer=\"16\" x=\"629.306122\" y=\"-186.000000\" color=\"8405120\"/>\
	<connector id=\"22\" selected=\"true\" layer=\"16\" x=\"705.306122\" y=\"-132.000000\" color=\"6324288\"/>\
	<connector id=\"23\" selected=\"true\" layer=\"16\" x=\"780.090048\" y=\"-183.957256\" slider=\"true\" color=\"16711680\">\
		<slidelimit id=\"5\"/>\
		<slidelimit id=\"16\"/>\
	</connector>\
	<connector id=\"5\" selected=\"true\" layer=\"16\" x=\"756.000000\" y=\"-218.000000\" anchor=\"true\" color=\"11012476\"/>\
	<connector id=\"16\" selected=\"true\" layer=\"16\" x=\"816.857143\" y=\"-132.000000\" color=\"12599551\"/>\
	<connector id=\"20\" selected=\"true\" layer=\"16\" x=\"879.999935\" y=\"-218.000000\" anchor=\"true\" color=\"13140032\"/>\
	<Link id=\"0\" selected=\"true\" layer=\"16\" linesize=\"1\" color=\"16711680\">\
		<connector id=\"0\"/>\
		<connector id=\"1\"/>\
		<connector id=\"2\"/>\
	</Link>\
	<Link id=\"2\" selected=\"true\" layer=\"16\" linesize=\"1\" color=\"32768\">\
		<connector id=\"4\"/>\
		<connector id=\"12\"/>\
		<connector id=\"14\"/>\
		<connector id=\"6\"/>\
	</Link>\
	<Link id=\"5\" selected=\"true\" layer=\"16\" linesize=\"1\" color=\"11012476\">\
		<connector id=\"7\"/>\
		<connector id=\"8\"/>\
		<connector id=\"3\"/>\
	</Link>\
	<Link id=\"6\" selected=\"true\" layer=\"16\" linesize=\"1\" color=\"16744448\">\
		<connector id=\"9\"/>\
		<connector id=\"10\"/>\
	</Link>\
	<Link id=\"1\" selected=\"true\" layer=\"16\" linesize=\"1\" color=\"12632064\">\
		<connector id=\"4\"/>\
		<connector id=\"7\"/>\
	</Link>\
	<Link id=\"7\" selected=\"true\" layer=\"16\" linesize=\"1\" color=\"11272364\">\
		<connector id=\"14\"/>\
		<connector id=\"10\"/>\
	</Link>\
	<Link id=\"4\" selected=\"true\" layer=\"16\" linesize=\"1\" color=\"6854344\">\
		<connector id=\"6\"/>\
		<connector id=\"11\"/>\
	</Link>\
	<Link id=\"8\" selected=\"true\" layer=\"16\" linesize=\"1\" color=\"8439808\">\
		<connector id=\"13\"/>\
		<connector id=\"15\"/>\
		<connector id=\"18\"/>\
	</Link>\
	<Link id=\"10\" selected=\"true\" layer=\"16\" linesize=\"1\" color=\"12615935\">\
		<connector id=\"15\"/>\
		<connector id=\"21\"/>\
	</Link>\
	<Link id=\"11\" selected=\"true\" layer=\"16\" linesize=\"1\" color=\"2146336\">\
		<connector id=\"1\"/>\
		<connector id=\"15\"/>\
	</Link>\
	<Link id=\"13\" selected=\"true\" layer=\"16\" linesize=\"1\" color=\"13123648\">\
		<connector id=\"10\"/>\
		<connector id=\"11\"/>\
	</Link>\
	<Link id=\"3\" selected=\"true\" layer=\"16\" linesize=\"1\" color=\"16711808\">\
		<connector id=\"17\"/>\
	</Link>\
	<Link id=\"9\" selected=\"true\" layer=\"16\" linesize=\"1\" color=\"6324424\">\
		<connector id=\"19\"/>\
	</Link>\
	<Link id=\"12\" selected=\"true\" layer=\"16\" linesize=\"1\" color=\"223719\">\
		<connector id=\"21\"/>\
		<connector id=\"22\"/>\
		<connector id=\"23\"/>\
	</Link>\
	<Link id=\"14\" selected=\"true\" layer=\"16\" linesize=\"1\" color=\"4243584\">\
		<connector id=\"5\"/>\
		<connector id=\"16\"/>\
	</Link>\
	<Link id=\"15\" selected=\"true\" layer=\"16\" linesize=\"1\" actuator=\"true\" throw=\"85.926150\" cpm=\"20.000000\" color=\"8421440\">\
		<connector id=\"20\"/>\
		<connector id=\"16\"/>\
	</Link>\
</linkage2>\
"

#define REVERSERDEMO_XML "\
<linkage2>\
	<program zoom=\"1\" xoffset=\"776\" yoffset=\"377\"/>\
	<connector anchor=\"true\" x=\"908.206818\" y=\"-305.500317\" id=\"2\"/>\
	<connector anchor=\"true\" x=\"990\" y=\"-305.500317\" id=\"3\"/>\
	<connector x=\"772.825005\" y=\"-349.678954\" id=\"4\"/>\
	<connector anchor=\"true\" x=\"772.825005\" y=\"-305.500317\" id=\"5\"/>\
	<connector x=\"772.825005\" y=\"-264.321681\" id=\"6\"/>\
	<connector anchor=\"true\" x=\"772.825005\" y=\"-438.061681\" id=\"7\"/>\
	<connector x=\"772.825005\" y=\"-268.312458\" id=\"8\" slider=\"true\">\
		<slidelimit id=\"4\"/>\
		<slidelimit id=\"6\"/>\
	</connector>\
	<connector anchor=\"true\" input=\"true\" rpm=\"45\" x=\"620.373724\" y=\"-264.321681\" id=\"9\"/>\
	<connector x=\"619.809633\" y=\"-246.834862\" id=\"10\"/>\
	<connector x=\"949.103409\" y=\"-305.500317\" id=\"1\" slider=\"true\">\
		<slidelimit id=\"2\"/>\
		<slidelimit id=\"3\"/>\
	</connector>\
	<connector anchor=\"true\" rpm=\"15\" x=\"908.206818\" y=\"-264.321681\" id=\"0\"/>\
	<connector anchor=\"true\" rpm=\"15\" x=\"990\" y=\"-264.321681\" id=\"11\"/>\
	<connector rpm=\"15\" x=\"949.103409\" y=\"-264.321681\" id=\"12\" slider=\"true\">\
		<slidelimit id=\"0\"/>\
		<slidelimit id=\"11\"/>\
	</connector>\
	<Link id=\"1\" >\
		<connector id=\"2\"/>\
		<connector id=\"3\"/>\
	</Link>\
	<Link id=\"2\" solid=\"true\">\
		<connector id=\"4\"/>\
		<connector id=\"5\"/>\
		<connector id=\"6\"/>\
	</Link>\
	<Link id=\"4\" actuator=\"true\" alwaysmanual=\"true\" throw=\"71.061681\" cpm=\"-45\" >\
		<connector id=\"7\"/>\
		<connector id=\"8\"/>\
	</Link>\
	<Link id=\"7\">\
		<connector id=\"9\"/>\
		<connector id=\"10\"/>\
	</Link>\
	<Link id=\"0\">\
		<connector id=\"6\"/>\
		<connector id=\"10\"/>\
	</Link>\
	<Link id=\"3\">\
		<connector id=\"1\"/>\
		<connector id=\"8\"/>\
	</Link>\
	<Link id=\"5\">\
		<connector id=\"0\"/>\
		<connector id=\"11\"/>\
	</Link>\
	<Link id=\"8\">\
		<connector id=\"6\"/>\
		<connector id=\"12\"/>\
	</Link>\
</linkage2>\
"
#define SCISSORSDEMO_XML "\
<linkage2>\
	<program zoom=\"1\" xoffset=\"-512\" yoffset=\"946\"/>\
	<connector anchor=\"true\" x=\"-580\" y=\"-1180\" id=\"0\"/>\
	<connector x=\"-500\" y=\"-1160\" id=\"1\"/>\
	<connector x=\"-420\" y=\"-1140\" id=\"2\"/>\
	<connector x=\"-580\" y=\"-1140\" id=\"3\"/>\
	<connector x=\"-500\" y=\"-1120\" id=\"4\"/>\
	<connector x=\"-420\" y=\"-1100\" id=\"5\"/>\
	<connector x=\"-580\" y=\"-1100\" id=\"6\"/>\
	<connector x=\"-500\" y=\"-1080\" id=\"7\"/>\
	<connector x=\"-420\" y=\"-1060\" id=\"8\"/>\
	<connector x=\"-580\" y=\"-1060\" id=\"9\"/>\
	<connector x=\"-500\" y=\"-1040\" id=\"10\"/>\
	<connector x=\"-420\" y=\"-1020\" id=\"11\"/>\
	<connector x=\"-580\" y=\"-1020\" id=\"12\"/>\
	<connector x=\"-500\" y=\"-1000\" id=\"13\"/>\
	<connector x=\"-420\" y=\"-980\" id=\"14\"/>\
	<connector x=\"-420\" y=\"-1180\" id=\"18\" slider=\"true\">\
		<slidelimit id=\"0\"/>\
		<slidelimit id=\"15\"/>\
	</connector>\
	<connector x=\"-580\" y=\"-980\" id=\"32\"/>\
	<connector anchor=\"true\" x=\"-400\" y=\"-1180\" id=\"15\"/>\
	<Link id=\"0\" solid=\"true\" >\
		<connector id=\"0\"/>\
		<connector id=\"1\"/>\
		<connector id=\"2\"/>\
	</Link>\
	<Link id=\"1\" solid=\"true\" >\
		<connector id=\"3\"/>\
		<connector id=\"4\"/>\
		<connector id=\"5\"/>\
	</Link>\
	<Link id=\"2\" solid=\"true\" >\
		<connector id=\"6\"/>\
		<connector id=\"7\"/>\
		<connector id=\"8\"/>\
	</Link>\
	<Link id=\"3\" solid=\"true\" >\
		<connector id=\"9\"/>\
		<connector id=\"10\"/>\
		<connector id=\"11\"/>\
	</Link>\
	<Link id=\"4\" solid=\"true\" >\
		<connector id=\"12\"/>\
		<connector id=\"13\"/>\
		<connector id=\"14\"/>\
	</Link>\
	<Link id=\"6\" solid=\"true\" >\
		<connector id=\"18\"/>\
		<connector id=\"1\"/>\
		<connector id=\"3\"/>\
	</Link>\
	<Link id=\"7\" solid=\"true\" >\
		<connector id=\"7\"/>\
		<connector id=\"5\"/>\
		<connector id=\"9\"/>\
	</Link>\
	<Link id=\"8\" solid=\"true\" >\
		<connector id=\"10\"/>\
		<connector id=\"8\"/>\
		<connector id=\"12\"/>\
	</Link>\
	<Link id=\"9\" solid=\"true\" >\
		<connector id=\"4\"/>\
		<connector id=\"2\"/>\
		<connector id=\"6\"/>\
	</Link>\
	<Link id=\"10\" solid=\"true\" >\
		<connector id=\"32\"/>\
		<connector id=\"13\"/>\
		<connector id=\"11\"/>\
	</Link>\
	<Link id=\"5\" solid=\"true\" >\
		<connector id=\"15\"/>\
		<connector id=\"0\"/>\
	</Link>\
	<Link id=\"11\" actuator=\"true\" throw=\"74.342601\" cpm=\"-5\" >\
		<connector id=\"0\"/>\
		<connector id=\"18\"/>\
	</Link>\
</linkage2>\
"

#define VARYSPEEDDEMO_XML "\
<linkage2>\
	<program zoom=\"1\" xoffset=\"484\" yoffset=\"270\"/>\
	<connector anchor=\"true\"  x=\"505.500000\" y=\"-266.500000\" id=\"0\"/>\
	<connector x=\"505.500000\" y=\"-126\" id=\"1\"/>\
	<connector anchor=\"true\" input=\"true\" rpm=\"3.5\" x=\"461\" y=\"-266.500000\" id=\"2\"/>\
	<connector x=\"505.500000\" y=\"-204\" id=\"3\" slider=\"true\">\
		<slidelimit id=\"0\"/>\
		<slidelimit id=\"1\"/>\
	</connector>\
	<connector id=\"4\" name=\"Variable Speed Rotation Test\" layer=\"1\" x=\"422.000000\" y=\"-317.999902\"/>\
	<Link id=\"2\" layer=\"1\" linesize=\"1\"  >\
		<connector id=\"4\"/>\
	</Link>\
	<Link id=\"0\" >\
		<connector id=\"0\"/>\
		<connector id=\"1\"/>\
	</Link>\
	<Link id=\"1\" >\
		<connector id=\"2\"/>\
		<connector id=\"3\"/>\
	</Link>\
</linkage2>\
"

#define BOXEXAMPLE_XML "\
<linkage2>\
	<program zoom=\"1\" xoffset=\"607\" yoffset=\"64\" scalefactor=\"1\" units=\"Millimeters\"/>\
	<connector anchor=\"true\" x=\"660\" y=\"-114\" id=\"0\"/>\
	<connector x=\"529\" y=\"-216\" id=\"1\"/>\
	<connector anchor=\"true\" x=\"619\" y=\"-91\" id=\"2\"/>\
	<connector x=\"529\" y=\"-170\" id=\"3\"/>\
	<connector x=\"451\" y=\"-117\" id=\"4\"/>\
	<connector x=\"707\" y=\"-155\" id=\"5\"/>\
	<connector anchor=\"true\" input=\"true\" rpm=\"8\" x=\"729.612274\" y=\"-143.728556\" id=\"6\"/>\
	<connector x=\"770.550182\" y=\"-122.236154\" id=\"7\"/>\
	<connector x=\"460\" y=\"-75.500000\" id=\"8\"/>\
	<connector draw=\"true\" x=\"511\" y=\"-25\" id=\"10\"/>\
	<connector anchor=\"true\" x=\"771\" y=\"32\" id=\"9\"/>\
	<connector x=\"565\" y=\"-217\" id=\"11\"/>\
	<connector anchor=\"true\" x=\"396\" y=\"15\" id=\"12\"/>\
	<connector x=\"640\" y=\"15\" id=\"13\"/>\
	<connector x=\"479\" y=\"15\" id=\"14\"/>\
	<connector x=\"776\" y=\"-172\" id=\"15\"/>\
	<connector anchor=\"true\" x=\"396\" y=\"-242\" id=\"16\"/>\
	<connector anchor=\"true\" x=\"820\" y=\"-242\" id=\"17\"/>\
	<connector anchor=\"true\" x=\"820\" y=\"15\" id=\"18\"/>\
	<connector anchor=\"true\" x=\"651\" y=\"15\" id=\"19\"/>\
	<Link id=\"0\" >\
		<connector id=\"0\"/>\
		<connector id=\"1\"/>\
		<connector id=\"5\"/>\
		<connector id=\"11\"/>\
	</Link>\
	<Link id=\"1\" >\
		<connector id=\"2\"/>\
		<connector id=\"3\"/>\
	</Link>\
	<Link id=\"2\">\
		<connector id=\"1\"/>\
		<connector id=\"3\"/>\
		<connector id=\"4\"/>\
		<connector id=\"8\"/>\
	</Link>\
	<Link id=\"3\" >\
		<connector id=\"6\"/>\
		<connector id=\"7\"/>\
		<connector id=\"15\"/>\
	</Link>\
	<Link id=\"4\">\
		<connector id=\"5\"/>\
		<connector id=\"7\"/>\
	</Link>\
	<Link id=\"5\" >\
		<connector id=\"10\"/>\
		<connector id=\"4\"/>\
		<connector id=\"8\"/>\
	</Link>\
	<Link id=\"6\" >\
		<connector id=\"9\"/>\
	</Link>\
	<Link id=\"7\" >\
		<connector id=\"12\"/>\
		<connector id=\"13\"/>\
		<connector id=\"14\"/>\
	</Link>\
	<Link id=\"8\">\
		<connector id=\"14\"/>\
		<connector id=\"15\"/>\
	</Link>\
	<Link id=\"9\" >\
		<connector id=\"16\"/>\
		<connector id=\"12\"/>\
	</Link>\
	<Link id=\"10\" >\
		<connector id=\"17\"/>\
		<connector id=\"18\"/>\
	</Link>\
	<Link id=\"11\">\
		<connector id=\"16\"/>\
		<connector id=\"17\"/>\
	</Link>\
	<Link id=\"13\">\
		<connector id=\"18\"/>\
		<connector id=\"19\"/>\
	</Link>\
</linkage2>\
"

#define WATTPARALLEL_XML "\
<linkage2>\
	<program zoom=\"1.274931\" xoffset=\"504\" yoffset=\"468\" scalefactor=\"1\" units=\"Millimeters\"/>\
	<connector anchor=\"true\" x=\"395.730997\" y=\"-244.275831\" id=\"0\"/>\
	<connector x=\"268.692308\" y=\"-282.641329\" id=\"1\"/>\
	<connector x=\"507.364427\" y=\"-209.726053\" id=\"2\"/>\
	<connector x=\"457.887500\" y=\"-224.758579\" id=\"3\"/>\
	<connector x=\"457.887500\" y=\"-252.650000\" id=\"4\"/>\
	<connector x=\"507.364427\" y=\"-238.378471\" id=\"5\"/>\
	<connector anchor=\"true\" input=\"true\" rpm=\"15\" x=\"268.692308\" y=\"-348.076923\" id=\"8\"/>\
	<connector x=\"268.692308\" y=\"-387.615385\" id=\"9\"/>\
	<connector anchor=\"true\" x=\"537.115043\" y=\"-270.633251\" id=\"10\"/>\
	<connector anchor=\"true\" x=\"507.364427\" y=\"-512.775148\" id=\"6\"/>\
	<connector anchor=\"true\" x=\"507.364427\" y=\"-382.775148\" id=\"11\"/>\
	<connector x=\"507.364427\" y=\"-412.624977\" id=\"7\" slider=\"true\">\
		<slidelimit id=\"6\"/>\
		<slidelimit id=\"11\"/>\
	</connector>\
	<Link id=\"0\" >\
		<connector id=\"0\"/>\
		<connector id=\"1\"/>\
		<connector id=\"2\"/>\
		<connector id=\"3\"/>\
	</Link>\
	<Link id=\"1\" >\
		<connector id=\"4\"/>\
		<connector id=\"5\"/>\
	</Link>\
	<Link id=\"2\" >\
		<connector id=\"3\"/>\
		<connector id=\"4\"/>\
	</Link>\
	<Link id=\"3\" >\
		<connector id=\"2\"/>\
		<connector id=\"5\"/>\
	</Link>\
	<Link id=\"6\" >\
		<connector id=\"8\"/>\
		<connector id=\"9\"/>\
	</Link>\
	<Link id=\"7\" >\
		<connector id=\"1\"/>\
		<connector id=\"9\"/>\
	</Link>\
	<Link id=\"8\" >\
		<connector id=\"10\"/>\
		<connector id=\"4\"/>\
	</Link>\
	<Link id=\"4\" >\
		<connector id=\"6\"/>\
		<connector id=\"11\"/>\
	</Link>\
	<Link id=\"5\" >\
		<connector id=\"7\"/>\
		<connector id=\"5\"/>\
	</Link>\
</linkage2>\
"

#define THROWTEST_XML "\
<linkage2>\
	<program zoom=\"1.300000\" xoffset=\"-240\" yoffset=\"-145\" scalefactor=\"1\" units=\"Millimeters\" viewlayers=\"4294967295\" editlayers=\"4294967295\"/>\
	<connector id=\"0\" layer=\"16\" anchor=\"true\" x=\"-372.244898\" y=\"184.999837\"  />\
	<connector id=\"1\" layer=\"16\" x=\"-97.959184\" y=\"184.999837\"  />\
	<connector id=\"2\" layer=\"16\" anchor=\"true\" x=\"-39.183673\" y=\"0\"  />\
	<connector id=\"3\" layer=\"1\" x=\"-372.244898\" y=\"202.072213\"  />\
	<connector id=\"4\" layer=\"1\" x=\"19.591837\" y=\"202.072213\"  />\
	<connector id=\"5\" layer=\"1\" x=\"-372.244898\" y=\"211.538461\"  />\
	<connector id=\"6\" layer=\"1\" x=\"-97.959184\" y=\"211.538461\"  />\
	<connector id=\"7\" name=\" \" layer=\"1\" x=\"-97.959184\" y=\"184.999837\"  />\
	<connector id=\"8\" name=\" \" layer=\"1\" x=\"19.591837\" y=\"184.999837\"  />\
	<Link id=\"0\" layer=\"16\" actuator=\"true\"  throw=\"117.551020\" cpm=\"15\" >\
		<connector id=\"0\"/>\
		<connector id=\"1\"/>\
	</Link>\
	<Link id=\"1\" layer=\"16\">\
		<connector id=\"2\"/>\
		<connector id=\"1\"/>\
	</Link>\
	<Link id=\"2\" layer=\"1\" measurementelement=\"true\">\
		<connector id=\"3\"/>\
		<connector id=\"4\"/>\
	</Link>\
	<Link id=\"3\" layer=\"1\" measurementelement=\"true\">\
		<connector id=\"5\"/>\
		<connector id=\"6\"/>\
	</Link>\
	<Link id=\"4\" layer=\"1\">\
		<connector id=\"7\"/>\
	</Link>\
	<Link id=\"5\" layer=\"1\">\
		<connector id=\"8\"/>\
	</Link>\
</linkage2>\
"

#define PISTON_XML "\
<linkage2>\
	<program zoom=\"1.300000\" xoffset=\"-292\" yoffset=\"-69\" scalefactor=\"1.000000\" units=\"Millimeters\" viewlayers=\"15\" editlayers=\"15\"/>\
	<connector id=\"0\" name=\" \" layer=\"16\" anchor=\"true\" x=\"-235.102041\" y=\"297.000098\"/>\
	<connector id=\"1\" name=\" \" layer=\"16\" anchor=\"true\" x=\"-235.102041\" y=\"163.999739\"/>\
	<connector id=\"2\" name=\" \" layer=\"16\" x=\"-235.102041\" y=\"243.779998\" slider=\"true\" >\
		<slidelimit id=\"0\"/>\
		<slidelimit id=\"1\"/>\
	</connector>\
	<connector id=\"3\" name=\" \" layer=\"16\" x=\"-235.102041\" y=\"219.209763\" slider=\"true\" >\
		<slidelimit id=\"0\"/>\
		<slidelimit id=\"1\"/>\
	</connector>\
	<connector id=\"4\" name=\" \" layer=\"16\" x=\"-235.102041\" y=\"231.999935\"/>\
	<connector id=\"5\" name=\" \" layer=\"16\" anchor=\"true\" input=\"true\" rpm=\"-15.000000\" x=\"-235.102041\" y=\"-65.000163\"/>\
	<connector id=\"6\" name=\" \" layer=\"16\" x=\"-203.384550\" y=\"-65.000163\"/>\
	<connector id=\"8\" layer=\"1\" x=\"-249.949932\" y=\"248.961653\"/>\
	<connector id=\"9\" layer=\"1\" x=\"-219.949932\" y=\"214.500115\"/>\
	<connector id=\"10\" layer=\"1\" x=\"-219.949932\" y=\"248.961653\"/>\
	<connector id=\"12\" layer=\"1\" x=\"-249.949932\" y=\"214.500115\"/>\
	<connector id=\"7\" layer=\"1\" x=\"-215.510204\" y=\"163.999739\"/>\
	<connector id=\"11\" layer=\"1\" x=\"-215.510204\" y=\"297.000098\"/>\
	<connector id=\"13\" layer=\"1\" x=\"-254.693878\" y=\"163.999739\"/>\
	<connector id=\"14\" layer=\"1\" x=\"-254.693878\" y=\"297.000098\"/>\
	<connector id=\"15\" layer=\"1\" x=\"-235.102041\" y=\"231.999935\"/>\
	<connector id=\"16\" layer=\"1\" x=\"-203.384550\" y=\"-65.000163\"/>\
	<connector id=\"18\" layer=\"1\" x=\"-235.102041\" y=\"-65.000163\"/>\
	<Link id=\"0\" name=\" \" layer=\"16\" linesize=\"1\"  >\
		<connector id=\"0\"/>\
		<connector id=\"1\"/>\
	</Link>\
	<Link id=\"1\" name=\" \" layer=\"16\" linesize=\"3\"  >\
		<connector id=\"2\"/>\
		<connector id=\"3\"/>\
		<connector id=\"4\"/>\
		<fastenconnector id=\"8\"/>\
		<fastenconnector id=\"9\"/>\
		<fastenconnector id=\"10\"/>\
		<fastenconnector id=\"12\"/>\
	</Link>\
	<Link id=\"2\" name=\" \" layer=\"16\" linesize=\"1\"  >\
		<connector id=\"5\"/>\
		<connector id=\"6\"/>\
		<fastenconnector id=\"17\"/>\
		<fastenconnector id=\"18\"/>\
	</Link>\
	<Link id=\"3\" name=\" \" layer=\"16\" linesize=\"1\"  >\
		<connector id=\"4\"/>\
		<connector id=\"6\"/>\
		<fastenconnector id=\"15\"/>\
		<fastenconnector id=\"16\"/>\
	</Link>\
	<Link id=\"4\" layer=\"1\" linesize=\"3\"  >\
		<connector id=\"8\"/>\
		<connector id=\"10\"/>\
	</Link>\
	<Link id=\"5\" layer=\"1\" linesize=\"3\"  >\
		<connector id=\"9\"/>\
		<connector id=\"10\"/>\
	</Link>\
	<Link id=\"6\" layer=\"1\" linesize=\"3\"  >\
		<connector id=\"12\"/>\
		<connector id=\"8\"/>\
	</Link>\
	<Link id=\"7\" layer=\"1\" linesize=\"3\"  >\
		<connector id=\"9\"/>\
		<connector id=\"12\"/>\
	</Link>\
	<Link id=\"8\" layer=\"1\" linesize=\"3\"  >\
		<connector id=\"7\"/>\
		<connector id=\"11\"/>\
	</Link>\
	<Link id=\"9\" layer=\"1\" linesize=\"3\"  >\
		<connector id=\"13\"/>\
		<connector id=\"14\"/>\
	</Link>\
	<Link id=\"10\" layer=\"1\" linesize=\"3\"  >\
		<connector id=\"15\"/>\
		<connector id=\"16\"/>\
	</Link>\
	<Link id=\"11\" layer=\"1\" linesize=\"3\"  >\
		<connector id=\"18\"/>\
		<connector id=\"16\"/>\
	</Link>\
</linkage2>\
"

#define CURVEDSLIDER1_XML "\
<linkage2>\
	<program zoom=\"1.000000\" xoffset=\"52\" yoffset=\"14\" scalefactor=\"1.000000\" units=\"Millimeters\" viewlayers=\"4294967295\" editlayers=\"4294967295\"/>\
	<connector id=\"0\" layer=\"16\" anchor=\"true\" input=\"true\" rpm=\"15.000000\" x=\"47.000000\" y=\"62.000000\"/>\
	<connector id=\"1\" layer=\"16\"x=\"56.359625\" y=\"10.077468\" slider=\"true\" slideradius=\"-176.326531\">\
		<slidelimit id=\"4\"/>\
		<slidelimit id=\"5\"/>\
	</connector>\
	<connector id=\"4\" layer=\"16\" anchor=\"true\" x=\"-59.000000\" y=\"-71.000000\"/>\
	<connector id=\"5\" layer=\"16\"x=\"203.000000\" y=\"-26.000000\"/>\
	<Link id=\"0\" layer=\"16\">\
		<connector id=\"0\"/>\
		<connector id=\"1\"/>\
	</Link>\
	<Link id=\"2\" layer=\"16\">\
		<connector id=\"4\"/>\
		<connector id=\"5\"/>\
	</Link>\
</linkage2>\
"

#define CURVEDSLIDER2_XML "\
<linkage2>\
	<program zoom=\"0.769231\" xoffset=\"-130\" yoffset=\"-49\" scalefactor=\"1.000000\" units=\"Millimeters\" viewlayers=\"4294967295\" editlayers=\"4294967295\"/>\
	<connector id=\"0\" layer=\"16\" x=\"-172.000000\" y=\"102.000000\" />\
	<connector id=\"1\" layer=\"16\" x=\"-89.000000\" y=\"102.000000\" />\
	<connector id=\"2\" layer=\"16\" anchor=\"true\" x=\"-130.393568\" y=\"-39.183673\" />\
	<connector id=\"3\" layer=\"16\" anchor=\"true\" input=\"true\" rpm=\"15.000000\" x=\"37.000000\" y=\"102.000000\" />\
	<connector id=\"4\" layer=\"16\" x=\"37.000000\" y=\"136.914554\"/>\
	<connector id=\"5\" layer=\"16\" anchor=\"true\" x=\"-274.285714\" y=\"143.227449\"/>\
	<connector id=\"6\" layer=\"16\" x=\"-130.393568\" y=\"143.227449\" slider=\"true\" slideradius=\"-41.500900\">\
		<slidelimit id=\"0\"/>\
		<slidelimit id=\"1\"/>\
	</connector>\
	<Link id=\"0\" layer=\"16\" >\
		<connector id=\"0\"/>\
		<connector id=\"1\"/>\
		<connector id=\"2\"/>\
	</Link>\
	<Link id=\"1\" layer=\"16\" >\
		<connector id=\"3\"/>\
		<connector id=\"4\"/>\
	</Link>\
	<Link id=\"2\" layer=\"16\" >\
		<connector id=\"1\"/>\
		<connector id=\"4\"/>\
	</Link>\
	<Link id=\"3\" layer=\"16\" >\
		<connector id=\"5\"/>\
		<connector id=\"6\"/>\
	</Link>\
</linkage2>\
"

#define CURVEDSLIDER3_XML "\
<linkage2>\
	<program zoom=\"1.300000\" xoffset=\"-186\" yoffset=\"-71\" scalefactor=\"1.000000\" units=\"Millimeters\" viewlayers=\"4294967295\" editlayers=\"4294967295\"/>\
	<connector id=\"0\" layer=\"16\" x=\"-313.469388\" y=\"27.133965\" />\
	<connector id=\"1\" layer=\"16\" x=\"-226.571191\" y=\"101.000000\" />\
	<connector id=\"2\" layer=\"16\" x=\"-151.858908\" y=\"27.246118\" slider=\"true\" slideradius=\"-70.530612\">\
		<slidelimit id=\"5\"/>\
		<slidelimit id=\"6\"/>\
	</connector>\
	<connector id=\"3\" layer=\"16\" anchor=\"true\"  x=\"-253.923515\" y=\"27.175288\" slider=\"true\">\
		<slidelimit id=\"0\"/>\
		<slidelimit id=\"2\"/>\
	</connector>\
	<connector id=\"4\" layer=\"16\" anchor=\"true\"  x=\"-209.891614\" y=\"27.205845\" slider=\"true\">\
		<slidelimit id=\"0\"/>\
		<slidelimit id=\"2\"/>\
	</connector>\
	<connector id=\"5\" layer=\"16\" anchor=\"true\"  x=\"-117.551020\" y=\"-29.183673\" />\
	<connector id=\"6\" layer=\"16\" x=\"-117.551020\" y=\"92.000000\" />\
	<connector id=\"7\" layer=\"16\" anchor=\"true\" input=\"true\" rpm=\"15.000000\" x=\"14.000000\" y=\"74.551020\" />\
	<connector id=\"8\" layer=\"16\" x=\"8.000000\" y=\"155.469388\" />\
	<Link id=\"0\" layer=\"16\">\
		<connector id=\"0\"/>\
		<connector id=\"1\"/>\
		<connector id=\"2\"/>\
	</Link>\
	<Link id=\"1\" layer=\"16\">\
		<connector id=\"3\"/>\
	</Link>\
	<Link id=\"2\" layer=\"16\">\
		<connector id=\"4\"/>\
	</Link>\
	<Link id=\"3\" layer=\"16\">\
		<connector id=\"5\"/>\
		<connector id=\"6\"/>\
	</Link>\
	<Link id=\"4\" layer=\"16\">\
		<connector id=\"7\"/>\
		<connector id=\"8\"/>\
	</Link>\
	<Link id=\"5\" layer=\"16\">\
		<connector id=\"6\"/>\
		<connector id=\"8\"/>\
	</Link>\
</linkage2>\
"

#define DOUBLECURVESLIDE_XML "\
<linkage2>\
	<program zoom=\"1.300000\" xoffset=\"-187\" yoffset=\"-99\" scalefactor=\"1.000000\" units=\"Millimeters\" viewlayers=\"4294967295\" editlayers=\"4294967295\"/>\
	<connector id=\"0\" layer=\"16\" x=\"-327.315542\" y=\"51.229887\" />\
	<connector id=\"1\" layer=\"16\" x=\"-226.571191\" y=\"101.000000\" />\
	<connector id=\"2\" layer=\"16\" x=\"-123.277115\" y=\"51.204017\" slider=\"true\" slideradius=\"78.367347\">\
		<slidelimit id=\"7\"/>\
		<slidelimit id=\"9\"/>\
	</connector>\
	<connector id=\"3\" layer=\"16\" anchor=\"true\" x=\"-272.586227\" y=\"28.357350\" slider=\"true\" slideradius=\"195.918367\">\
		<slidelimit id=\"0\"/>\
		<slidelimit id=\"2\"/>\
	</connector>\
	<connector id=\"4\" layer=\"16\" anchor=\"true\" x=\"-185.972224\" y=\"26.541848\" slider=\"true\" slideradius=\"195.918367\">\
		<slidelimit id=\"0\"/>\
		<slidelimit id=\"2\"/>\
	</connector>\
	<connector id=\"5\" layer=\"16\" anchor=\"true\" x=\"-94.615249\" y=\"-66.923037\" />\
	<connector id=\"6\" layer=\"16\" anchor=\"true\" x=\"-94.615249\" y=\"193.846335\" />\
	<connector id=\"7\" layer=\"16\" x=\"-94.615249\" y=\"112.331351\" slider=\"true\" >\
		<slidelimit id=\"5\"/>\
		<slidelimit id=\"6\"/>\
	</connector>\
	<connector id=\"9\" layer=\"16\" x=\"-94.615249\" y=\"-8.846043\" slider=\"true\" >\
		<slidelimit id=\"5\"/>\
		<slidelimit id=\"6\"/>\
	</connector>\
	<connector id=\"10\" layer=\"16\" anchor=\"true\" input=\"true\" rpm=\"15.000000\" x=\"-13.437991\" y=\"240.486656\" />\
	<connector id=\"11\" layer=\"16\" x=\"-50.000000\" y=\"240.486656\" />\
	<Link id=\"0\" layer=\"16\" >\
		<connector id=\"0\"/>\
		<connector id=\"1\"/>\
		<connector id=\"2\"/>\
	</Link>\
	<Link id=\"1\" layer=\"16\" >\
		<connector id=\"3\"/>\
	</Link>\
	<Link id=\"2\" layer=\"16\" >\
		<connector id=\"4\"/>\
	</Link>\
	<Link id=\"3\" layer=\"16\" >\
		<connector id=\"5\"/>\
		<connector id=\"6\"/>\
	</Link>\
	<Link id=\"4\" layer=\"16\" >\
		<connector id=\"7\"/>\
		<connector id=\"9\"/>\
	</Link>\
	<Link id=\"5\" layer=\"16\" >\
		<connector id=\"10\"/>\
		<connector id=\"11\"/>\
	</Link>\
	<Link id=\"6\" layer=\"16\" >\
		<connector id=\"7\"/>\
		<connector id=\"11\"/>\
	</Link>\
</linkage2>\
"

#define CHEBYCHEV_XML "\
<linkage2>\
	<program zoom=\"0.769231\" xoffset=\"-148\" yoffset=\"-53\" scalefactor=\"1.000000\" units=\"Inches\" viewlayers=\"4294967295\" editlayers=\"4294967295\"/>\
	<connector id=\"0\" layer=\"16\" anchor=\"true\" x=\"-475.000000\" y=\"-96.000000\"/>\
	<connector id=\"1\" layer=\"16\" x=\"-192.000000\" y=\"288.000000\" />\
	<connector id=\"2\" layer=\"16\" anchor=\"true\" x=\"-101.000000\" y=\"-96.000000\"/>\
	<connector id=\"3\" layer=\"16\" x=\"-384.000000\" y=\"288.000000\"/>\
	<connector id=\"4\" layer=\"16\" anchor=\"true\" input=\"true\" rpm=\"15.000000\" x=\"112.000000\" y=\"-17.000000\"/>\
	<connector id=\"5\" layer=\"16\" x=\"192.000000\" y=\"68.000000\"/>\
	<connector id=\"6\" layer=\"16\" draw=\"true\" x=\"-288.000000\" y=\"288.000000\"/>\
	<Link id=\"0\" layer=\"16\" linesize=\"1\">\
		<connector id=\"0\"/>\
		<connector id=\"1\"/>\
	</Link>\
	<Link id=\"1\" layer=\"16\" linesize=\"1\">\
		<connector id=\"2\"/>\
		<connector id=\"3\"/>\
	</Link>\
	<Link id=\"2\" layer=\"16\" linesize=\"1\">\
		<connector id=\"1\"/>\
		<connector id=\"3\"/>\
		<connector id=\"6\"/>\
	</Link>\
	<Link id=\"3\" layer=\"16\" linesize=\"1\">\
		<connector id=\"4\"/>\
		<connector id=\"5\"/>\
	</Link>\
	<Link id=\"4\" layer=\"16\" linesize=\"1\">\
		<connector id=\"1\"/>\
		<connector id=\"5\"/>\
	</Link>\
</linkage2>\
"

#define LIFTER_XML "\
<linkage2>\
	<program zoom=\"0.455166\" xoffset=\"349\" yoffset=\"-177\" scalefactor=\"1.000000\" units=\"Inches\" viewlayers=\"4294967295\" editlayers=\"4294967295\"/>\
	<connector id=\"0\" layer=\"16\" anchor=\"true\" x=\"978.778654\" y=\"-273.547078\" />\
	<connector id=\"1\" layer=\"16\" x=\"803.029393\" y=\"14.288846\" />\
	<connector id=\"2\" layer=\"16\" anchor=\"true\" x=\"1076.518913\" y=\"-273.547078\" />\
	<connector id=\"3\" layer=\"16\" x=\"1054.963813\" y=\"-76.450458\" />\
	<connector id=\"4\" name=\"Motor\" layer=\"16\" anchor=\"true\" input=\"true\" rpm=\"-2.000000\" x=\"1266.310155\" y=\"-109.836069\" />\
	<connector id=\"8\" name=\"Ball Lift\" layer=\"16\" draw=\"true\" x=\"311.495607\" y=\"-307.800858\" />\
	<connector id=\"9\" layer=\"16\" x=\"749.547269\" y=\"-193.596923\" />\
	<connector id=\"6\" name=\"Location of a second ball on the pickup ramp\" layer=\"1\" x=\"39.196800\" y=\"-256.742400\" drawcircle=\"48.000000\" />\
	<connector id=\"16\" layer=\"1\" x=\"1961.595171\" y=\"730.578099\" />\
	<connector id=\"5\" layer=\"16\" anchor=\"true\" x=\"1409.999944\" y=\"-274.080082\" />\
	<connector id=\"7\" layer=\"16\" x=\"1695.701593\" y=\"175.006548\" />\
	<connector id=\"11\" layer=\"16\" x=\"1388.308526\" y=\"296.030945\" />\
	<connector id=\"14\" name=\"Ball Lift\" layer=\"16\" draw=\"true\" x=\"1265.522656\" y=\"1097.472603\" />\
	<connector id=\"15\" layer=\"16\" x=\"1546.134926\" y=\"442.387477\" />\
	<connector id=\"19\" layer=\"16\" x=\"1288.467833\" y=\"12.727543\" />\
	<connector id=\"18\" layer=\"1\" x=\"1265.518537\" y=\"1097.469996\" drawcircle=\"48.000000\" />\
	<connector id=\"20\" layer=\"1\" x=\"294.162067\" y=\"-353.856408\" />\
	<connector id=\"21\" layer=\"1\" x=\"280.779310\" y=\"-345.184347\" />\
	<connector id=\"22\" layer=\"1\" x=\"310.582909\" y=\"-357.584381\" />\
	<connector id=\"23\" layer=\"1\" x=\"330.755872\" y=\"-353.913045\" />\
	<connector id=\"24\" layer=\"1\" x=\"348.409890\" y=\"-341.433163\" />\
	<connector id=\"25\" layer=\"1\" x=\"359.029566\" y=\"-321.490566\" />\
	<connector id=\"12\" layer=\"1\" x=\"348.911532\" y=\"-276.846730\" />\
	<connector id=\"26\" layer=\"1\" x=\"360.044082\" y=\"-298.582328\" />\
	<connector id=\"13\" layer=\"1\" x=\"1216.760493\" y=\"1104.952192\" />\
	<connector id=\"28\" layer=\"1\" x=\"1217.645025\" y=\"1082.222451\" />\
	<connector id=\"31\" layer=\"1\" x=\"1235.940028\" y=\"1056.497520\" />\
	<connector id=\"33\" layer=\"1\" x=\"1260.204592\" y=\"1047.650643\" />\
	<connector id=\"35\" layer=\"1\" x=\"1285.957782\" y=\"1052.629779\" />\
	<connector id=\"27\" layer=\"1\" x=\"311.532016\" y=\"-307.865175\" drawcircle=\"48.000000\" />\
	<connector id=\"29\" layer=\"1\" x=\"-4.000000\" y=\"-374.167653\" />\
	<connector id=\"30\" layer=\"1\" x=\"1766.855158\" y=\"-374.167653\" />\
	<connector id=\"37\" layer=\"1\" x=\"257.775138\" y=\"-300.591704\" />\
	<connector id=\"36\" layer=\"1\" x=\"-4.733728\" y=\"-300.591704\" />\
	<connector id=\"41\" layer=\"1\" x=\"1470.267575\" y=\"136.872287\" drawcircle=\"18.000000\" />\
	<connector id=\"46\" layer=\"1\" x=\"1266.310155\" y=\"-109.836069\" drawcircle=\"124.550400\" />\
	<connector id=\"48\" layer=\"1\" x=\"1286.532197\" y=\"1053.275638\" />\
	<connector id=\"49\" layer=\"1\" x=\"1301.391120\" y=\"1063.400690\" />\
	<connector id=\"50\" layer=\"1\" x=\"1216.415615\" y=\"1105.440958\" />\
	<connector id=\"51\" layer=\"1\" x=\"1220.618804\" y=\"1117.087573\" />\
	<connector id=\"53\" layer=\"1\" x=\"1653.947629\" y=\"1037.269472\" />\
	<connector id=\"55\" layer=\"1\" x=\"1250.109456\" y=\"1039.593279\" />\
	<connector id=\"58\" layer=\"16\" anchor=\"true\" x=\"1470.267575\" y=\"136.872287\" />\
	<connector id=\"10\" layer=\"1\" x=\"1232.086892\" y=\"1045.514287\" />\
	<connector id=\"43\" layer=\"1\" x=\"1219.476766\" y=\"1057.299247\" />\
	<connector id=\"17\" name=\"Tabletop\" layer=\"1\" x=\"1580.147018\" y=\"-374.167653\" />\
	<connector id=\"32\" name=\"Dropoff track\" layer=\"1\" x=\"1407.935398\" y=\"1038.740258\" />\
	<Link id=\"0\" layer=\"16\" linesize=\"1\" >\
		<connector id=\"0\"/>\
		<connector id=\"1\"/>\
	</Link>\
	<Link id=\"1\" layer=\"16\" linesize=\"1\" >\
		<connector id=\"2\"/>\
		<connector id=\"3\"/>\
	</Link>\
	<Link id=\"2\" layer=\"16\" linesize=\"1\" >\
		<connector id=\"4\"/>\
		<connector id=\"19\"/>\
	</Link>\
	<Link id=\"3\" layer=\"16\" linesize=\"1\" >\
		<connector id=\"3\"/>\
		<connector id=\"9\"/>\
		<connector id=\"19\"/>\
	</Link>\
	<Link id=\"5\" layer=\"16\" linesize=\"1\" >\
		<connector id=\"8\"/>\
		<connector id=\"9\"/>\
		<connector id=\"1\"/>\
		<fastenconnector id=\"20\"/>\
		<fastenconnector id=\"21\"/>\
		<fastenconnector id=\"22\"/>\
		<fastenconnector id=\"23\"/>\
		<fastenconnector id=\"24\"/>\
		<fastenconnector id=\"25\"/>\
		<fastenconnector id=\"12\"/>\
		<fastenconnector id=\"26\"/>\
		<fastenconnector id=\"27\"/>\
	</Link>\
	<Link id=\"4\" layer=\"1\" linesize=\"1\" >\
		<connector id=\"6\"/>\
	</Link>\
	<Link id=\"11\" layer=\"1\" linesize=\"1\" measurementelement=\"true\" >\
		<connector id=\"16\"/>\
	</Link>\
	<Link id=\"6\" layer=\"16\" linesize=\"1\" >\
		<connector id=\"5\"/>\
		<connector id=\"7\"/>\
	</Link>\
	<Link id=\"7\" layer=\"16\" linesize=\"1\" >\
		<connector id=\"11\"/>\
		<connector id=\"58\"/>\
	</Link>\
	<Link id=\"9\" layer=\"16\" linesize=\"1\" >\
		<connector id=\"11\"/>\
		<connector id=\"15\"/>\
		<connector id=\"19\"/>\
	</Link>\
	<Link id=\"10\" layer=\"16\" linesize=\"1\" >\
		<connector id=\"14\"/>\
		<connector id=\"15\"/>\
		<connector id=\"7\"/>\
		<fastenconnector id=\"18\"/>\
		<fastenconnector id=\"28\"/>\
		<fastenconnector id=\"31\"/>\
		<fastenconnector id=\"33\"/>\
		<fastenconnector id=\"35\"/>\
		<fastenconnector id=\"48\"/>\
		<fastenconnector id=\"49\"/>\
		<fastenconnector id=\"13\"/>\
		<fastenconnector id=\"50\"/>\
		<fastenconnector id=\"51\"/>\
	</Link>\
	<Link id=\"8\" layer=\"1\" linesize=\"1\" >\
		<connector id=\"20\"/>\
		<connector id=\"22\"/>\
	</Link>\
	<Link id=\"13\" layer=\"1\" linesize=\"1\" >\
		<connector id=\"18\"/>\
	</Link>\
	<Link id=\"14\" layer=\"1\" linesize=\"1\" >\
		<connector id=\"20\"/>\
		<connector id=\"21\"/>\
	</Link>\
	<Link id=\"15\" layer=\"1\" linesize=\"1\" >\
		<connector id=\"22\"/>\
		<connector id=\"23\"/>\
	</Link>\
	<Link id=\"16\" layer=\"1\" linesize=\"1\" >\
		<connector id=\"23\"/>\
		<connector id=\"24\"/>\
	</Link>\
	<Link id=\"17\" layer=\"1\" linesize=\"1\" >\
		<connector id=\"24\"/>\
		<connector id=\"25\"/>\
	</Link>\
	<Link id=\"18\" layer=\"1\" linesize=\"1\" >\
		<connector id=\"25\"/>\
		<connector id=\"26\"/>\
	</Link>\
	<Link id=\"19\" layer=\"1\" linesize=\"1\" >\
		<connector id=\"12\"/>\
		<connector id=\"26\"/>\
	</Link>\
	<Link id=\"20\" layer=\"1\" linesize=\"1\" >\
		<connector id=\"13\"/>\
		<connector id=\"28\"/>\
	</Link>\
	<Link id=\"21\" layer=\"1\" linesize=\"1\" >\
		<connector id=\"28\"/>\
		<connector id=\"31\"/>\
	</Link>\
	<Link id=\"22\" layer=\"1\" linesize=\"1\" >\
		<connector id=\"31\"/>\
		<connector id=\"33\"/>\
	</Link>\
	<Link id=\"23\" layer=\"1\" linesize=\"1\" >\
		<connector id=\"33\"/>\
		<connector id=\"35\"/>\
	</Link>\
	<Link id=\"25\" layer=\"1\" linesize=\"1\" >\
		<connector id=\"27\"/>\
	</Link>\
	<Link id=\"24\" layer=\"1\" linesize=\"1\" >\
		<connector id=\"29\"/>\
		<connector id=\"30\"/>\
	</Link>\
	<Link id=\"28\" layer=\"1\" linesize=\"1\" measurementelement=\"true\" >\
		<connector id=\"36\"/>\
		<connector id=\"37\"/>\
	</Link>\
	<Link id=\"30\" layer=\"1\" linesize=\"1\" >\
		<connector id=\"41\"/>\
	</Link>\
	<Link id=\"34\" layer=\"1\" linesize=\"1\" >\
		<connector id=\"46\"/>\
	</Link>\
	<Link id=\"36\" layer=\"1\" linesize=\"1\" >\
		<connector id=\"48\"/>\
		<connector id=\"49\"/>\
	</Link>\
	<Link id=\"37\" layer=\"1\" linesize=\"1\" >\
		<connector id=\"50\"/>\
		<connector id=\"51\"/>\
	</Link>\
	<Link id=\"38\" layer=\"1\" linesize=\"1\" >\
		<connector id=\"53\"/>\
		<connector id=\"55\"/>\
	</Link>\
	<Link id=\"39\" layer=\"1\" linesize=\"1\" >\
		<connector id=\"55\"/>\
		<connector id=\"10\"/>\
	</Link>\
	<Link id=\"32\" layer=\"1\" linesize=\"1\" >\
		<connector id=\"10\"/>\
		<connector id=\"43\"/>\
	</Link>\
	<Link id=\"12\" layer=\"1\" linesize=\"1\" >\
		<connector id=\"17\"/>\
	</Link>\
	<Link id=\"26\" layer=\"1\" linesize=\"1\" >\
		<connector id=\"32\"/>\
	</Link>\
</linkage2>\
"

#define FOURGEARS_XML "\
<linkage2>\
	<program zoom=\"0.769231\" xoffset=\"-84\" yoffset=\"-23\" scalefactor=\"1.000000\" units=\"Millimeters\" viewlayers=\"2147483647\" editlayers=\"2147483647\"/>\
	<connector id=\"0\" layer=\"16\" anchor=\"true\" input=\"true\" rpm=\"45.000000\" x=\"-298.693813\" y=\"76.999837\" color=\"16711680\">\
	</connector>\
	<connector id=\"8\" layer=\"16\" anchor=\"true\" x=\"-149.795853\" y=\"76.999837\" color=\"8439808\">\
	</connector>\
	<connector id=\"10\" layer=\"16\" anchor=\"true\" x=\"-235.999935\" y=\"76.999837\" color=\"12615935\">\
	</connector>\
	<connector id=\"5\" layer=\"16\" anchor=\"true\" x=\"-54.000098\" y=\"76.999837\" color=\"16711680\">\
	</connector>\
	<Link id=\"0\" layer=\"16\" linesize=\"1\" alwaysmanual=\"false\" gear=\"true\" color=\"16711680\">\
		<connector id=\"0\"/>\
	</Link>\
	<Link id=\"8\" layer=\"16\" linesize=\"1\" gear=\"true\" color=\"8439808\">\
		<connector id=\"8\"/>\
	</Link>\
	<Link id=\"10\" layer=\"16\" linesize=\"1\" gear=\"true\" color=\"12615935\">\
		<connector id=\"10\"/>\
	</Link>\
	<Link id=\"4\" layer=\"16\" linesize=\"1\" gear=\"true\" color=\"6854344\">\
		<connector id=\"5\"/>\
	</Link>\
	<selected>\
	</selected>\
	<ratios>\
		<ratio type=\"gears\">\
			<link id=\"10\" size=\"2.000000\"/>\
			<link id=\"0\" size=\"1.000000\"/>\
		</ratio>\
		<ratio type=\"gears\">\
			<link id=\"8\" size=\"2.000000\"/>\
			<link id=\"10\" size=\"1.000000\"/>\
		</ratio>\
		<ratio type=\"gears\">\
			<link id=\"4\" size=\"2.000000\"/>\
			<link id=\"8\" size=\"1.000000\"/>\
		</ratio>\
	</ratios>\
</linkage2>\
"

#define ORBITGEARS_XML "\
<linkage2>\
	<program zoom=\"0.769231\" xoffset=\"-58\" yoffset=\"-98\" scalefactor=\"1.000000\" units=\"Millimeters\" viewlayers=\"4294967295\" editlayers=\"4294967295\"/>\
	<connector id=\"2\" selected=\"false\" layer=\"16\" x=\"-360.400000\" y=\"166.000065\" anchor=\"true\" color=\"32768\"/>\
	<connector id=\"4\" selected=\"false\" layer=\"16\" x=\"-319.000163\" y=\"166.000065\" anchor=\"true\" input=\"true\" rpm=\"3.000000\" color=\"6854344\"/>\
	<connector id=\"6\" selected=\"false\" layer=\"16\" x=\"184.400000\" y=\"166.000065\" slideradius=\"0.000000\" color=\"16744448\"/>\
	<connector id=\"0\" selected=\"false\" layer=\"16\" x=\"311.500000\" y=\"166.000065\" color=\"16711680\"/>\
	<connector id=\"1\" selected=\"false\" layer=\"16\" x=\"400.000000\" y=\"78.000000\" color=\"12632064\"/>\
	<Link id=\"0\" selected=\"false\" layer=\"16\" linesize=\"1\" gear=\"true\" color=\"16711680\">\
		<connector id=\"6\"/>\
	</Link>\
	<Link id=\"1\" selected=\"false\" layer=\"16\" linesize=\"1\" fastenlink=\"2\" gear=\"true\" color=\"12632064\">\
		<connector id=\"0\"/>\
	</Link>\
	<Link id=\"4\" selected=\"false\" layer=\"16\" linesize=\"1\" fastenconnector=\"2\" gear=\"true\" color=\"6854344\">\
		<connector id=\"4\"/>\
	</Link>\
	<Link id=\"3\" selected=\"false\" layer=\"16\" linesize=\"1\" color=\"16711808\">\
		<connector id=\"2\"/>\
	</Link>\
	<Link id=\"5\" selected=\"false\" layer=\"16\" linesize=\"1\" color=\"11012476\">\
		<connector id=\"4\"/>\
		<connector id=\"6\"/>\
		<connector id=\"0\"/>\
	</Link>\
	<Link id=\"2\" selected=\"false\" layer=\"16\" linesize=\"1\" color=\"32768\">\
		<connector id=\"0\"/>\
		<connector id=\"1\"/>\
	</Link>\
	<selected>\
	</selected>\
	<ratios>\
		<ratio type=\"gears\" sizeassize=\"false\">\
			<link id=\"0\" size=\"8.000000\"/>\
			<link id=\"1\" size=\"1.000000\"/>\
		</ratio>\
		<ratio type=\"chain\" sizeassize=\"true\">\
			<link id=\"4\" size=\"25.000000\"/>\
			<link id=\"0\" size=\"25.000000\"/>\
		</ratio>\
	</ratios>\
</linkage2>\
"

#define OSCILLATORS_XML "\
<linkage2>\
	<program zoom=\"1.690000\" xoffset=\"-152\" yoffset=\"-57\" scalefactor=\"1.000000\" units=\"Millimeters\" viewlayers=\"2147483647\" editlayers=\"2147483647\"/>\
	<connector id=\"4\" selected=\"false\" layer=\"16\" x=\"-213.893486\" y=\"33.532604\" color=\"6854344\"/>\
	<connector id=\"3\" selected=\"false\" layer=\"16\" x=\"-244.378838\" y=\"61.343195\" anchor=\"true\" color=\"16711808\"/>\
	<connector id=\"8\" selected=\"false\" layer=\"16\" x=\"-244.378838\" y=\"17.751564\" anchor=\"true\" color=\"8439808\"/>\
	<connector id=\"0\" selected=\"false\" layer=\"16\" x=\"-119.526628\" y=\"42.011710\" anchor=\"true\" color=\"16711680\"/>\
	<connector id=\"10\" selected=\"false\" layer=\"16\" x=\"-202.366864\" y=\"17.751564\" color=\"12615935\"/>\
	<connector id=\"1\" selected=\"false\" layer=\"16\" x=\"2.082845\" y=\"33.532604\" color=\"8421440\"/>\
	<connector id=\"2\" selected=\"false\" layer=\"16\" x=\"-28.402507\" y=\"61.343195\" anchor=\"true\" color=\"4243584\"/>\
	<connector id=\"5\" selected=\"false\" layer=\"16\" x=\"-28.402507\" y=\"17.751564\" anchor=\"true\" color=\"8855748\"/>\
	<connector id=\"7\" selected=\"false\" layer=\"16\" x=\"13.609467\" y=\"17.751564\" color=\"8405120\"/>\
	<connector id=\"9\" selected=\"false\" layer=\"16\" x=\"92.307802\" y=\"40.236562\" anchor=\"true\" input=\"true\" rpm=\"5.000000\" color=\"13123282\"/>\
	<connector id=\"11\" selected=\"false\" layer=\"16\" x=\"128.402367\" y=\"50.295858\" color=\"2146336\"/>\
	<Link id=\"0\" selected=\"false\" layer=\"16\" linesize=\"1\" color=\"16711680\">\
		<connector id=\"4\"/>\
		<connector id=\"3\"/>\
	</Link>\
	<Link id=\"3\" selected=\"false\" layer=\"16\" linesize=\"1\" fastenlink=\"0\" gear=\"true\" color=\"16711808\">\
		<connector id=\"3\"/>\
	</Link>\
	<Link id=\"4\" selected=\"false\" layer=\"16\" linesize=\"1\" fastenlink=\"5\" gear=\"true\" color=\"6854344\">\
		<connector id=\"8\"/>\
	</Link>\
	<Link id=\"5\" selected=\"false\" layer=\"16\" linesize=\"1\" color=\"11012476\">\
		<connector id=\"8\"/>\
		<connector id=\"10\"/>\
	</Link>\
	<Link id=\"6\" selected=\"false\" layer=\"16\" linesize=\"1\" actuator=\"true\" throw=\"76.050029\" cpm=\"5.000000\" color=\"16744448\">\
		<connector id=\"0\"/>\
		<connector id=\"10\"/>\
	</Link>\
	<Link id=\"1\" selected=\"false\" layer=\"16\" linesize=\"1\" color=\"11272364\">\
		<connector id=\"1\"/>\
		<connector id=\"2\"/>\
	</Link>\
	<Link id=\"2\" selected=\"false\" layer=\"16\" linesize=\"1\" fastenlink=\"1\" gear=\"true\" color=\"12615935\">\
		<connector id=\"2\"/>\
	</Link>\
	<Link id=\"7\" selected=\"false\" layer=\"16\" linesize=\"1\" fastenlink=\"8\" gear=\"true\" color=\"2146336\">\
		<connector id=\"5\"/>\
	</Link>\
	<Link id=\"8\" selected=\"false\" layer=\"16\" linesize=\"1\" color=\"223719\">\
		<connector id=\"5\"/>\
		<connector id=\"7\"/>\
	</Link>\
	<Link id=\"10\" selected=\"false\" layer=\"16\" linesize=\"1\" color=\"12615935\">\
		<connector id=\"9\"/>\
		<connector id=\"11\"/>\
	</Link>\
	<Link id=\"9\" selected=\"false\" layer=\"16\" linesize=\"1\" color=\"13123282\">\
		<connector id=\"7\"/>\
		<connector id=\"11\"/>\
	</Link>\
	<selected>\
		<connector id=\"-2013261708\"/>\
		<connector id=\"6029432\"/>\
		<connector id=\"7\"/>\
		<connector id=\"0\"/>\
		<connector id=\"0\"/>\
	</selected>\
	<ratios>\
		<ratio type=\"gears\" sizeassize=\"false\">\
			<link id=\"4\" size=\"2.000000\"/>\
			<link id=\"3\" size=\"1.000000\"/>\
		</ratio>\
		<ratio type=\"gears\" sizeassize=\"false\">\
			<link id=\"7\" size=\"2.000000\"/>\
			<link id=\"2\" size=\"1.000000\"/>\
		</ratio>\
	</ratios>\
</linkage2>\
"

#define STEPHENSON_XML "\
<linkage2>\
	<program zoom=\"0.455\" xoffset=\"6\" yoffset=\"-191\" scalefactor=\"1.000000\" units=\"Millimeters\" viewlayers=\"2147483647\" editlayers=\"2147483647\"/>\
	<connector id=\"0\" layer=\"16\" x=\"-641.434552\" y=\"64.091174\" color=\"13123648\"/>\
	<connector id=\"1\" layer=\"16\" x=\"-634.186664\" y=\"102.598685\" anchor=\"true\" input=\"true\" rpm=\"20.000000\" color=\"4243584\"/>\
	<connector id=\"2\" layer=\"16\" x=\"-626.938776\" y=\"141.106196\" color=\"8421440\"/>\
	<connector id=\"3\" layer=\"16\" x=\"-39.183673\" y=\"-16.842237\" color=\"12599551\"/>\
	<connector id=\"4\" layer=\"16\" x=\"4.303654\" y=\"214.202828\" color=\"2121760\"/>\
	<connector id=\"5\" layer=\"16\" x=\"12.744556\" y=\"162.011036\" slider=\"true\" slideradius=\"274.285714\" color=\"8855748\">\
		<slidelimit id=\"3\"/>\
		<slidelimit id=\"4\"/>\
	</connector>\
	<connector id=\"6\" layer=\"16\" x=\"2.160001\" y=\"276.885714\" anchor=\"true\" color=\"13140032\"/>\
	<connector id=\"7\" layer=\"16\" x=\"-7.736345\" y=\"340.815374\" color=\"8405120\"/>\
	<connector id=\"8\" layer=\"16\" x=\"-145.567347\" y=\"470.204082\" anchor=\"true\" color=\"6324288\"/>\
	<connector id=\"9\" layer=\"16\" x=\"-253.302041\" y=\"519.621023\" color=\"16711680\"/>\
	<connector id=\"10\" layer=\"16\" x=\"-23.839999\" y=\"416.204082\" color=\"12632064\"/>\
	<connector id=\"11\" layer=\"16\" x=\"-253.302041\" y=\"674.221637\" anchor=\"true\" color=\"32768\"/>\
	<connector id=\"12\" layer=\"16\" x=\"458.499967\" y=\"313.469388\" anchor=\"true\" color=\"223719\"/>\
	<connector id=\"13\" layer=\"16\" x=\"566.969388\" y=\"313.469388\" anchor=\"true\" color=\"13123648\"/>\
	<connector id=\"14\" layer=\"16\" x=\"512.734678\" y=\"313.469388\" slider=\"true\" color=\"4243584\">\
		<slidelimit id=\"12\"/>\
		<slidelimit id=\"13\"/>\
	</connector>\
	<Link id=\"0\" layer=\"16\" linesize=\"1\" solid=\"true\" color=\"8439808\">\
		<connector id=\"0\"/>\
		<connector id=\"1\"/>\
		<connector id=\"2\"/>\
	</Link>\
	<Link id=\"1\" layer=\"16\" linesize=\"1\" solid=\"true\" color=\"13123282\">\
		<connector id=\"3\"/>\
		<connector id=\"4\"/>\
	</Link>\
	<Link id=\"2\" layer=\"16\" linesize=\"2\" color=\"12615935\">\
		<connector id=\"2\"/>\
		<connector id=\"4\"/>\
	</Link>\
	<Link id=\"3\" layer=\"16\" linesize=\"2\" color=\"2146336\">\
		<connector id=\"0\"/>\
		<connector id=\"3\"/>\
	</Link>\
	<Link id=\"4\" layer=\"16\" linesize=\"1\" solid=\"true\" color=\"223719\">\
		<connector id=\"5\"/>\
		<connector id=\"6\"/>\
		<connector id=\"7\"/>\
	</Link>\
	<Link id=\"5\" layer=\"16\" linesize=\"1\" solid=\"true\" color=\"13123648\">\
		<connector id=\"8\"/>\
		<connector id=\"9\"/>\
		<connector id=\"10\"/>\
	</Link>\
	<Link id=\"6\" layer=\"16\" linesize=\"1\" solid=\"true\" actuator=\"true\" alwaysmanual=\"true\" throw=\"117.301943\" cpm=\"100.000000\" color=\"4243584\">\
		<connector id=\"11\"/>\
		<connector id=\"9\"/>\
	</Link>\
	<Link id=\"7\" layer=\"16\" linesize=\"2\" color=\"11272364\">\
		<connector id=\"3\"/>\
		<connector id=\"10\"/>\
	</Link>\
	<Link id=\"8\" layer=\"16\" linesize=\"1\" solid=\"true\" color=\"8439808\">\
		<connector id=\"12\"/>\
		<connector id=\"13\"/>\
	</Link>\
	<Link id=\"10\" layer=\"16\" linesize=\"2\" color=\"12615935\">\
		<connector id=\"7\"/>\
		<connector id=\"14\"/>\
	</Link>\
</linkage2>\
"

#define PEAUCELLIER_XML "\
<linkage2>\
	<program zoom=\"0.350128\" xoffset=\"-31\" yoffset=\"75\" scalefactor=\"1.000000\" units=\"Inches\" viewlayers=\"2147483647\" editlayers=\"2147483647\"/>\
	<connector id=\"4\" selected=\"true\" layer=\"16\" x=\"96.060235\" y=\"-468.511630\" color=\"4243584\"/>\
	<connector id=\"5\" selected=\"true\" layer=\"16\" x=\"288.000000\" y=\"-135.974400\" draw=\"true\" color=\"8421440\"/>\
	<connector id=\"1\" selected=\"true\" layer=\"16\" x=\"-383.957843\" y=\"-135.982347\" anchor=\"true\" color=\"12632064\"/>\
	<connector id=\"2\" selected=\"true\" layer=\"16\" x=\"-96.000000\" y=\"-135.992659\" color=\"32768\"/>\
	<connector id=\"7\" selected=\"true\" layer=\"16\" x=\"-671.957843\" y=\"-135.992657\" anchor=\"true\" color=\"11272364\"/>\
	<connector id=\"8\" selected=\"true\" layer=\"16\" x=\"96.000000\" y=\"196.583759\" color=\"8439808\"/>\
	<connector id=\"0\" selected=\"true\" layer=\"16\" x=\"-233.433615\" y=\"-136.045211\" anchor=\"true\" input=\"true\" rpm=\"15.000000\" color=\"16711680\"/>\
	<connector id=\"3\" selected=\"true\" layer=\"16\" x=\"-124.489042\" y=\"-135.991639\" slider=\"true\" color=\"16711808\">\
		<slidelimit id=\"1\"/>\
		<slidelimit id=\"2\"/>\
	</connector>\
	<Link id=\"0\" selected=\"true\" layer=\"16\" linesize=\"1\" color=\"16711680\">\
		<connector id=\"2\"/>\
		<connector id=\"8\"/>\
	</Link>\
	<Link id=\"1\" selected=\"true\" layer=\"16\" linesize=\"1\" color=\"8439808\">\
		<connector id=\"5\"/>\
		<connector id=\"8\"/>\
	</Link>\
	<Link id=\"2\" selected=\"true\" layer=\"16\" linesize=\"1\" color=\"13123282\">\
		<connector id=\"4\"/>\
		<connector id=\"5\"/>\
	</Link>\
	<Link id=\"3\" selected=\"true\" layer=\"16\" linesize=\"1\" color=\"16711808\">\
		<connector id=\"4\"/>\
		<connector id=\"2\"/>\
	</Link>\
	<Link id=\"4\" selected=\"true\" layer=\"16\" linesize=\"1\" color=\"6854344\">\
		<connector id=\"1\"/>\
		<connector id=\"2\"/>\
	</Link>\
	<Link id=\"6\" selected=\"true\" layer=\"16\" linesize=\"1\" color=\"16744448\">\
		<connector id=\"7\"/>\
		<connector id=\"8\"/>\
	</Link>\
	<Link id=\"5\" selected=\"true\" layer=\"16\" linesize=\"1\" color=\"11012476\">\
		<connector id=\"4\"/>\
		<connector id=\"7\"/>\
	</Link>\
	<Link id=\"7\" selected=\"true\" layer=\"16\" linesize=\"1\" color=\"11272364\">\
		<connector id=\"0\"/>\
		<connector id=\"3\"/>\
	</Link>\
</linkage2>\
"

#define ELLIPTICAL_XML "\
<linkage2>\
	<program zoom=\"0.591716\" xoffset=\"82\" yoffset=\"-94\" scalefactor=\"1.000000\" units=\"Millimeters\" viewlayers=\"2147483647\" editlayers=\"2147483647\"/>\
	<connector id=\"0\" selected=\"true\" layer=\"16\" x=\"68.000131\" y=\"88.999837\" anchor=\"true\" input=\"true\" rpm=\"10.000000\" color=\"16711680\"/>\
	<connector id=\"1\" selected=\"true\" layer=\"16\" x=\"68.000499\" y=\"3.999674\" color=\"12632064\"/>\
	<connector id=\"2\" selected=\"true\" layer=\"16\" x=\"-413.816327\" y=\"11.632490\" anchor=\"true\" color=\"32768\"/>\
	<connector id=\"3\" selected=\"true\" layer=\"16\" x=\"-195.918367\" y=\"11.632490\" anchor=\"true\" color=\"16711808\"/>\
	<connector id=\"4\" selected=\"true\" layer=\"16\" x=\"-324.904052\" y=\"11.632490\" slider=\"true\" color=\"6854344\">\
		<slidelimit id=\"2\"/>\
		<slidelimit id=\"3\"/>\
	</connector>\
	<connector id=\"5\" selected=\"true\" layer=\"16\" x=\"-206.790613\" y=\"40.983139\" color=\"11012476\"/>\
	<connector id=\"6\" selected=\"true\" layer=\"16\" x=\"39.183673\" y=\"235.102041\" anchor=\"true\" color=\"16744448\"/>\
	<connector id=\"8\" selected=\"true\" layer=\"16\" x=\"102.142994\" y=\"36.873722\" color=\"8439808\"/>\
	<connector id=\"9\" selected=\"true\" layer=\"16\" x=\"-303.581323\" y=\"52.964543\" draw=\"true\" color=\"13123282\"/>\
	<connector id=\"11\" selected=\"true\" layer=\"1\" x=\"-52.726340\" y=\"388.290206\" fastenlink=\"3\" color=\"13158600\"/>\
	<connector id=\"12\" selected=\"true\" layer=\"1\" x=\"-37.365535\" y=\"288.695528\" fastenlink=\"3\" color=\"13158600\"/>\
	<connector id=\"13\" selected=\"true\" layer=\"1\" x=\"39.183673\" y=\"235.102041\" fastenlink=\"3\" color=\"13158600\"/>\
	<connector id=\"7\" selected=\"true\" layer=\"1\" x=\"-350.174492\" y=\"60.597395\" fastenlink=\"4\" color=\"13158600\"/>\
	<connector id=\"10\" selected=\"true\" layer=\"1\" x=\"-251.215695\" y=\"54.739293\" fastenlink=\"4\" color=\"13158600\"/>\
	<connector id=\"14\" selected=\"true\" layer=\"16\" x=\"68.000499\" y=\"88.999837\" anchor=\"true\" input=\"true\" rpm=\"10.000000\" color=\"4243584\"/>\
	<connector id=\"15\" selected=\"true\" layer=\"16\" x=\"68.000368\" y=\"174.000000\" color=\"8421440\"/>\
	<connector id=\"16\" selected=\"true\" layer=\"16\" x=\"-413.815959\" y=\"11.632490\" anchor=\"true\" color=\"12599551\"/>\
	<connector id=\"17\" selected=\"true\" layer=\"16\" x=\"-195.917999\" y=\"11.632490\" anchor=\"true\" color=\"2121760\"/>\
	<connector id=\"18\" selected=\"true\" layer=\"16\" x=\"-289.866979\" y=\"11.632490\" slider=\"true\" color=\"6324424\">\
		<slidelimit id=\"16\"/>\
		<slidelimit id=\"17\"/>\
	</connector>\
	<connector id=\"19\" selected=\"true\" layer=\"16\" x=\"-195.917999\" y=\"88.999837\" color=\"8855748\"/>\
	<connector id=\"20\" selected=\"true\" layer=\"16\" x=\"39.184041\" y=\"235.102041\" anchor=\"true\" color=\"13140032\"/>\
	<connector id=\"21\" selected=\"true\" layer=\"16\" x=\"109.000368\" y=\"39.183673\" color=\"8405120\"/>\
	<connector id=\"22\" selected=\"true\" layer=\"16\" x=\"-289.866979\" y=\"115.183510\" draw=\"true\" color=\"6324288\"/>\
	<connector id=\"23\" selected=\"true\" layer=\"1\" x=\"-57.999632\" y=\"385.000000\" fastenlink=\"11\" color=\"13158600\"/>\
	<connector id=\"24\" selected=\"true\" layer=\"1\" x=\"-39.183305\" y=\"286.000000\" fastenlink=\"11\" color=\"13158600\"/>\
	<connector id=\"25\" selected=\"true\" layer=\"1\" x=\"39.184041\" y=\"235.102041\" fastenlink=\"11\" color=\"13158600\"/>\
	<connector id=\"26\" selected=\"true\" layer=\"1\" x=\"-334.815959\" y=\"129.632490\" fastenlink=\"12\" color=\"13158600\"/>\
	<connector id=\"27\" selected=\"true\" layer=\"1\" x=\"-237.815959\" y=\"109.183510\" fastenlink=\"12\" color=\"13158600\"/>\
	<connector id=\"28\" selected=\"true\" layer=\"16\" x=\"453.671457\" y=\"266.241184\" anchor=\"true\" input=\"true\" rpm=\"10.000000\" color=\"11012476\"/>\
	<connector id=\"29\" selected=\"true\" layer=\"16\" x=\"453.671404\" y=\"300.452416\" color=\"16744448\"/>\
	<connector id=\"30\" selected=\"true\" layer=\"16\" x=\"259.747893\" y=\"235.102041\" anchor=\"true\" color=\"11272364\"/>\
	<connector id=\"31\" selected=\"true\" layer=\"16\" x=\"347.448403\" y=\"235.102041\" anchor=\"true\" color=\"8439808\"/>\
	<connector id=\"32\" selected=\"true\" layer=\"16\" x=\"309.635412\" y=\"235.102041\" slider=\"true\" color=\"13123282\">\
		<slidelimit id=\"30\"/>\
		<slidelimit id=\"31\"/>\
	</connector>\
	<connector id=\"33\" selected=\"true\" layer=\"16\" x=\"347.448403\" y=\"266.241184\" color=\"12615935\"/>\
	<connector id=\"34\" selected=\"true\" layer=\"16\" x=\"442.073285\" y=\"325.045029\" anchor=\"true\" color=\"2146336\"/>\
	<connector id=\"35\" selected=\"true\" layer=\"16\" x=\"470.173261\" y=\"246.190960\" color=\"223719\"/>\
	<connector id=\"36\" selected=\"true\" layer=\"16\" x=\"309.635412\" y=\"276.779701\" draw=\"true\" color=\"13123648\"/>\
	<connector id=\"37\" selected=\"true\" layer=\"1\" x=\"402.958381\" y=\"385.376605\" fastenlink=\"19\" color=\"13158600\"/>\
	<connector id=\"38\" selected=\"true\" layer=\"1\" x=\"410.531658\" y=\"345.530658\" fastenlink=\"19\" color=\"13158600\"/>\
	<connector id=\"39\" selected=\"true\" layer=\"1\" x=\"442.073285\" y=\"325.045029\" fastenlink=\"19\" color=\"13158600\"/>\
	<connector id=\"40\" selected=\"true\" layer=\"1\" x=\"291.544153\" y=\"282.595189\" fastenlink=\"20\" color=\"13158600\"/>\
	<connector id=\"41\" selected=\"true\" layer=\"1\" x=\"330.585131\" y=\"274.364796\" fastenlink=\"20\" color=\"13158600\"/>\
	<connector id=\"42\" selected=\"true\" layer=\"16\" x=\"453.671404\" y=\"95.587837\" anchor=\"true\" input=\"true\" rpm=\"10.000000\" color=\"8855748\"/>\
	<connector id=\"43\" selected=\"true\" layer=\"16\" x=\"453.671552\" y=\"61.376605\" color=\"13140032\"/>\
	<connector id=\"44\" selected=\"true\" layer=\"16\" x=\"259.747840\" y=\"64.448694\" anchor=\"true\" color=\"8405120\"/>\
	<connector id=\"45\" selected=\"true\" layer=\"16\" x=\"347.448350\" y=\"64.448694\" anchor=\"true\" color=\"6324288\"/>\
	<connector id=\"46\" selected=\"true\" layer=\"16\" x=\"295.533636\" y=\"64.448694\" slider=\"true\" color=\"16711680\">\
		<slidelimit id=\"44\"/>\
		<slidelimit id=\"45\"/>\
	</connector>\
	<connector id=\"47\" selected=\"true\" layer=\"16\" x=\"343.072441\" y=\"76.261869\" color=\"12632064\"/>\
	<connector id=\"48\" selected=\"true\" layer=\"16\" x=\"442.073232\" y=\"154.391682\" anchor=\"true\" color=\"32768\"/>\
	<connector id=\"49\" selected=\"true\" layer=\"16\" x=\"467.413371\" y=\"74.607893\" color=\"16711808\"/>\
	<connector id=\"50\" selected=\"true\" layer=\"16\" x=\"304.115699\" y=\"81.084196\" draw=\"true\" color=\"6854344\"/>\
	<connector id=\"51\" selected=\"true\" layer=\"1\" x=\"405.080894\" y=\"216.047514\" fastenlink=\"27\" color=\"13158600\"/>\
	<connector id=\"52\" selected=\"true\" layer=\"1\" x=\"411.263377\" y=\"175.962219\" fastenlink=\"27\" color=\"13158600\"/>\
	<connector id=\"53\" selected=\"true\" layer=\"1\" x=\"442.073232\" y=\"154.391682\" fastenlink=\"27\" color=\"13158600\"/>\
	<connector id=\"54\" selected=\"true\" layer=\"1\" x=\"285.362680\" y=\"84.156300\" fastenlink=\"28\" color=\"13158600\"/>\
	<connector id=\"55\" selected=\"true\" layer=\"1\" x=\"325.192043\" y=\"81.798505\" fastenlink=\"28\" color=\"13158600\"/>\
	<Link id=\"0\" selected=\"true\" layer=\"16\" linesize=\"1\" color=\"16711680\">\
		<connector id=\"0\"/>\
		<connector id=\"1\"/>\
	</Link>\
	<Link id=\"1\" selected=\"true\" layer=\"16\" linesize=\"1\" color=\"12632064\">\
		<connector id=\"2\"/>\
		<connector id=\"3\"/>\
	</Link>\
	<Link id=\"2\" selected=\"true\" layer=\"16\" linesize=\"1\" color=\"32768\">\
		<connector id=\"4\"/>\
		<connector id=\"5\"/>\
		<connector id=\"1\"/>\
	</Link>\
	<Link id=\"3\" selected=\"true\" layer=\"16\" linesize=\"1\" color=\"16711808\">\
		<connector id=\"6\"/>\
		<connector id=\"8\"/>\
	</Link>\
	<Link id=\"4\" selected=\"true\" layer=\"16\" linesize=\"1\" color=\"6854344\">\
		<connector id=\"9\"/>\
		<connector id=\"8\"/>\
		<connector id=\"5\"/>\
	</Link>\
	<Link id=\"5\" selected=\"true\" layer=\"1\" linesize=\"3\" color=\"13158600\">\
		<connector id=\"11\"/>\
		<connector id=\"12\"/>\
	</Link>\
	<Link id=\"6\" selected=\"true\" layer=\"1\" linesize=\"3\" color=\"13158600\">\
		<connector id=\"12\"/>\
		<connector id=\"13\"/>\
	</Link>\
	<Link id=\"7\" selected=\"true\" layer=\"1\" linesize=\"3\" color=\"13158600\">\
		<connector id=\"7\"/>\
		<connector id=\"10\"/>\
	</Link>\
	<Link id=\"8\" selected=\"true\" layer=\"16\" linesize=\"1\" color=\"8439808\">\
		<connector id=\"14\"/>\
		<connector id=\"15\"/>\
	</Link>\
	<Link id=\"9\" selected=\"true\" layer=\"16\" linesize=\"1\" color=\"13123282\">\
		<connector id=\"16\"/>\
		<connector id=\"17\"/>\
	</Link>\
	<Link id=\"10\" selected=\"true\" layer=\"16\" linesize=\"1\" color=\"12615935\">\
		<connector id=\"18\"/>\
		<connector id=\"19\"/>\
		<connector id=\"15\"/>\
	</Link>\
	<Link id=\"11\" selected=\"true\" layer=\"16\" linesize=\"1\" color=\"2146336\">\
		<connector id=\"20\"/>\
		<connector id=\"21\"/>\
	</Link>\
	<Link id=\"12\" selected=\"true\" layer=\"16\" linesize=\"1\" color=\"223719\">\
		<connector id=\"22\"/>\
		<connector id=\"21\"/>\
		<connector id=\"19\"/>\
	</Link>\
	<Link id=\"13\" selected=\"true\" layer=\"1\" linesize=\"3\" color=\"13158600\">\
		<connector id=\"23\"/>\
		<connector id=\"24\"/>\
	</Link>\
	<Link id=\"14\" selected=\"true\" layer=\"1\" linesize=\"3\" color=\"13158600\">\
		<connector id=\"24\"/>\
		<connector id=\"25\"/>\
	</Link>\
	<Link id=\"15\" selected=\"true\" layer=\"1\" linesize=\"3\" color=\"13158600\">\
		<connector id=\"26\"/>\
		<connector id=\"27\"/>\
	</Link>\
	<Link id=\"16\" selected=\"true\" layer=\"16\" linesize=\"1\" color=\"12599551\">\
		<connector id=\"28\"/>\
		<connector id=\"29\"/>\
	</Link>\
	<Link id=\"17\" selected=\"true\" layer=\"16\" linesize=\"1\" color=\"2121760\">\
		<connector id=\"30\"/>\
		<connector id=\"31\"/>\
	</Link>\
	<Link id=\"18\" selected=\"true\" layer=\"16\" linesize=\"1\" color=\"6324424\">\
		<connector id=\"32\"/>\
		<connector id=\"33\"/>\
		<connector id=\"29\"/>\
	</Link>\
	<Link id=\"19\" selected=\"true\" layer=\"16\" linesize=\"1\" color=\"8855748\">\
		<connector id=\"34\"/>\
		<connector id=\"35\"/>\
	</Link>\
	<Link id=\"20\" selected=\"true\" layer=\"16\" linesize=\"1\" color=\"13140032\">\
		<connector id=\"36\"/>\
		<connector id=\"35\"/>\
		<connector id=\"33\"/>\
	</Link>\
	<Link id=\"21\" selected=\"true\" layer=\"1\" linesize=\"3\" color=\"13158600\">\
		<connector id=\"37\"/>\
		<connector id=\"38\"/>\
	</Link>\
	<Link id=\"22\" selected=\"true\" layer=\"1\" linesize=\"3\" color=\"13158600\">\
		<connector id=\"38\"/>\
		<connector id=\"39\"/>\
	</Link>\
	<Link id=\"23\" selected=\"true\" layer=\"1\" linesize=\"3\" color=\"13158600\">\
		<connector id=\"40\"/>\
		<connector id=\"41\"/>\
	</Link>\
	<Link id=\"24\" selected=\"true\" layer=\"16\" linesize=\"1\" color=\"12632064\">\
		<connector id=\"42\"/>\
		<connector id=\"43\"/>\
	</Link>\
	<Link id=\"25\" selected=\"true\" layer=\"16\" linesize=\"1\" color=\"32768\">\
		<connector id=\"44\"/>\
		<connector id=\"45\"/>\
	</Link>\
	<Link id=\"26\" selected=\"true\" layer=\"16\" linesize=\"1\" color=\"16711808\">\
		<connector id=\"46\"/>\
		<connector id=\"47\"/>\
		<connector id=\"43\"/>\
	</Link>\
	<Link id=\"27\" selected=\"true\" layer=\"16\" linesize=\"1\" color=\"6854344\">\
		<connector id=\"48\"/>\
		<connector id=\"49\"/>\
	</Link>\
	<Link id=\"28\" selected=\"true\" layer=\"16\" linesize=\"1\" color=\"11012476\">\
		<connector id=\"50\"/>\
		<connector id=\"49\"/>\
		<connector id=\"47\"/>\
	</Link>\
	<Link id=\"29\" selected=\"true\" layer=\"1\" linesize=\"3\" color=\"13158600\">\
		<connector id=\"51\"/>\
		<connector id=\"52\"/>\
	</Link>\
	<Link id=\"30\" selected=\"true\" layer=\"1\" linesize=\"3\" color=\"13158600\">\
		<connector id=\"52\"/>\
		<connector id=\"53\"/>\
	</Link>\
	<Link id=\"31\" selected=\"true\" layer=\"1\" linesize=\"3\" color=\"13158600\">\
		<connector id=\"54\"/>\
		<connector id=\"55\"/>\
	</Link>\
</linkage2>\
"