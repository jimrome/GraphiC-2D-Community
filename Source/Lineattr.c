/* (c) 1993 by Scientific Endeavors Corporation.
   All rights reserved.
Routines for setting the various line attributes and putting these
attributes in the TKF file.

Version 7.0 3/3/93
**************************************************************/
#include <graphic.h>

void TYPE GCLASS dashf(int type)
{
	G_PROCESS_ID

	if (g_lq.pattern == type)
		return;
	if (type < 1 || type > 9) {
		messageq(ERRQ, 23);
		type = 1;
	}
	dashfq(type);
}
/******************************************************************
Force putting the line pattern into the TKF file. */
void GCLASS dashfq(int type)
{
	char res[5];
	G_PROCESS_ID

	encodeq(type - 1, res);	/* res is index code string. */
	sprintfq(cmd_strq, "\33MV%s", res);
	gprosq(2);
#if GPC_UNX || GPC_PORTABLE || !GPC_GUI
	g_lq.pattern = type;
#endif
}
/*******************************************************************
 User-called routine to set the plot colors */
void TYPE GCLASS color(int col)
{
	G_PROCESS_ID

	if (col == g_lq.color)
		return;
	if ((col & 0xFF000000) == 0x01000000) {
		g_lq.color = col;
		return;
	}
	if (!((0 <= col && col <= 15) || (50 <= col && col <= 174) ||
		(200 <= col && col <= 447 ) ) ) {
		messageq(ERRQ, 3);		/* Color must be 0 - 15 or 200 - 455 */
		if (g_clsq.color < 16)
			col = 15 - g_clsq.color;	/* Inverse of background color */
		else
			col = WHITE;
	}
	ccolorq('g', col);
}
/*******************************************************************
This routine is called to set the color of the text, the background, or
the graphic lines. The values used are the IBM 16-color values. When
these values are sent to the disk file, they are converted to the TEK
default convention. Play retranslates them to IBM. */
void GCLASS ccolorq(char mode, int value)
{
	char res[5];
	int tval;
	G_PROCESS_ID

	if (mode == 'p')
		tval = (value < 0) ? -itotekq[-value] : value;
	else if (value >= 0)
		tval = (value <= 15) ? itotekq[value] : value;
	else
		tval = 0;
	encodeq(tval, res);	/* res is index code string. */
	if (mode == 'g')
		sprintfq(cmd_strq, "\033ML%s\35", res);
	else if (mode == 'p')
		sprintfq(cmd_strq, "\033MP%s\35", res);
	else
		sprintfq(cmd_strq, "\033RA1%s1\35", res);
	gprosq(2);
#if GPC_UNX || GPC_PORTABLE || !GPC_GUI
	g_lq.color = value;
#endif
}
/*******************************************************************
 User-called routine to set the plot colors using RGB values.
 This routine returns the closest color number (>=216)
*/
char grayscaleq[] = {0,5,8,11,14,17,20,24,28,32,36,40,45,50,56,63};
char colorstepq[] = {0,14,26,38,51,63};

int TYPE GCLASS RGBcolor(int r, int g, int b)
{
	int col, j;

	/* Make sure values are legal */
	j = 0;
	if (0 > r) {
		j = 1;
		r = 0;
	}
	if (0 > g) {
		j = 1;
		g = 0;
	}
	if (0 > b) {
		j = 1;
		b = 0;
	}
	if (r > 63) {
		j = 1;
		r = 63;
	}
	if (g > 63) {
		j = 1;
		g = 63;
	}
	if (b > 63) {
		j = 1;
		b = 63;
	}
	if (j) {
		messageq(WARNQ, 31);
	}

	/* First, check on gray scale */
	if (r == g && r == b) {
		col = 216 + closest_indexq(grayscaleq, r, 16);
	}
	else {
		r = closest_indexq(colorstepq, r, 6);
		g = closest_indexq(colorstepq, g, 6);
		b = closest_indexq(colorstepq, b, 6);
		col = 232 + r * 36 + g * 6 + b;
	}
	color(col);
	return(col);
}
/*******************************************************************/
/* Return the closest index to an array value.
   end is the dimension of the array
*/
int GCLASS closest_indexq(char *array, int value, int end)
{
	int i, j;

	if (*array > (char)value)		/* Below first value */
		return(0);
	for (i = 0; i < end; i++) {
		if (*(array + i) >= (char)value) {
			break;
		}
	}
	if (i == end - 1)
		j = end;
	else {
		j = (*(array + i + 1) - value) < (value - *(array + 1)) ?
			i + 1 : i;
	}
	return(j);
}
/*************************************************************/
/* Set line thickness in inches (or cm) for ALL following lines */
void TYPE GCLASS tiline(float wid)
{
	G_PROCESS_ID

	tline(intopix(wid));
}

/*************************************************************/
/* A "GT" in the TKF file turns on thickened lines. The following coded
number is the width. */
/* tline now is the FULL line thickness */
void TYPE GCLASS tline(int wid)
{
	G_PROCESS_ID

	if (g_lq.thick == wid)
		return;
	tlineq(wid);
}

/**************************************************************/
/* Forces putting the line thickness in the TKF file for initialization
purposes. */
void GCLASS tlineq(int wid)
{
	char res[8];
	G_PROCESS_ID

	encodeq(wid, res);
	sprintfq(cmd_strq, "\33GT%s", res);
	gprosq(2);
}
