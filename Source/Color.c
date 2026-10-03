/* Copyright 1993 by Scientific Endeavors Corporation.
   All rights reserved.
This file contains routines that convert color representations.
Version 7.0 11/12/93
*******************************************************************/

#include <graphic.h>
#include <pattern.h>
#include <color.h>

/*************************************************************/
static unsigned char IBMrgb[32][3] = {	/* 1st 32 IBM palette */
{0,0,0},{0,0,41},{0,41,0},{0,41,41},
{41,0,0},{41,0,41},{41,21,0},{41,41,41},
{21,21,21},{21,21,63},{21,63,21},{21,63,63},
{63,21,21},{63,21,63},{63,63,21},{63,63,63},
/* Gray scale */
{0,0,0},{5,5,5},{8,8,8},{11,11,11},
{14,14,14},{17,17,17},{20,20,20},{24,24,24},
{28,28,28},{32,32,32},{36,36,36},{40,40,40},
{45,45,45},{50,50,50},{56,56,56},{63,63,63}
};
static int delta[6] = {0,13,26,38,51,63};	/* sum = 63 */

/*********************************************************************/
/* This routine converts from RGB colors to HLS colors.
r, g, b go from 0 to 63
h goes from 0 to 360
l, s go from 0 to 100. if s==0, h is undefined (we set to 0)
*/
void TYPE GCLASS RGBtoHLS(int r, int g, int b, int *h, int *l, int *s)
{
	int min = 63, max = 0;
	int tmp;
	long deltal;

	/* Find min */
	if (r < min)
		min = r;
	if (g < min)
		min = g;
	if (b < min)
		min = b;

	/* Find max */
	if (r > max)
		max = r;
	if (g > max)
		max = g;
	if (b > max)
		max = b;

	/* The lightness */
	tmp = (max + min) >> 1;		/* 0 to 63, need 0 to 100 */
	*l = (tmp * 100) / 63;


	/* Now do the saturation */
	if (max == min) {	/* achromatic since r == g == b */
		*s = *h = 0;		/* h is really undefined */
	}
	else {
		if (*l <= 50) {
			*s = (100 * (max - min)) / (max + min);
		}
		else {
			*s = (100 * (max - min)) / (126 - max - min);
		}
	}

	/* Now get the hue */
	deltal = (long)(max - min);
	if (r == max)				/* between yellow and magenta */
		*h = (int)((60L * (g - b)) / deltal);
	else if (g == max)			/* between cyan  and yellow */
		*h = (int)(120 + ((60L * (b - r)) / deltal));
	else if (b == max)			 /* between magenta and cyan */
		*h = (int)(240 + ((60L * (r - g)) / deltal));
	if (*h < 0)
		*h += 360;
}
/*********************************************************************/
/* Convert and HLS color to an RGB color.
r, g, b go from 0 to 63
h goes from 0 to 360
l, s go from 0 to 100. if s==0, h is undefined (we set to 0)
*/
void TYPE GCLASS HLStoRGB(int h, int l, int s, int *r, int *g, int *b)
{
	float m1, m2;
	float hf, lf, sf;

	lf = (float).01 * l; hf = (float)h, sf = (float).01 * s;

	if (s == 0) {
		*r = *b = *g = (int)(lf * (float)0.63);
		return;
	}
	if (lf < (float).5)
		m2 = lf * ((float)1.0 + sf);
	else
		m2 = lf + sf - lf * sf;
	m1 = (float)2. * lf - m2;
	*r = (int)(63.f * valueq(m1, m2, hf + 120.f));
	*g = (int)(63.f * valueq(m1, m2, hf));
	*b = (int)(63.f * valueq(m1, m2, hf - 120.f));
}
/*********************************************************************/
/* Auxiliary fuction for HLStoRGB returns color in range 0 to 1 */
float GCLASS valueq(float n1, float n2, float hue)
{
	/* Make sure hue is in range */
	while (hue > (float)360)
		hue -= (float)360;
	while (hue < (float)0)
		hue += 360.f;

	if (hue < 60.f)
		return(n1 + ((n2 - n1) * hue) / 60.f);
	if (hue < 180.f)
		return(n2);
	if (hue < 240.f)
		return(n1 + ((n2 - n1) * (240.f - hue)) / 60.f);
	else
		return(n1);
}
/*********************************************************************/
/* Convert RGB to CMY.                                               */
/* c = cyan;  m = magenta;  y = yellow;                              */
/* Colors run from 0 to 63.                                          */
/*********************************************************************/
void TYPE GCLASS RGBtoCMY(int r, int g, int b, int *c, int *m, int *y)
{
	*c = 63 - r;
	*m = 63 - g;
	*y = 63 - b;
}
/*********************************************************************/
/* Convert RGB to CMYK. Printers need this color representation.     */
/* c = cyan;  m = magenta;  y = yellow;  k = black                   */
/* Colors run from 0 to 63.                                          */
/*********************************************************************/
void TYPE GCLASS RGBtoCMYK(int r, int g, int b, int *c, int *m, int *y, int *k)
{

	int blk = 63;

	*c = 63 - r;
	*m = 63 - g;
	*y = 63 - b;
	/* Find the amount of black (= minimum common component) */
	if (*c < blk)
		blk = *c;
	if (*m < blk)
		blk = *m;
	if (*y < blk)
		blk = *y;
	if (blk > 0) {
		*c -= blk;
		*m -= blk;
		*y -= blk;
	}
	*k = blk;
}
/*********************************************************************/
/* Convert a CMY color to a square four-pixel dither pattern.        */
/* CMY values run from 0 to 63.                                      */
/*********************************************************************/

void GCLASS CMYto4q(int c, int m, int y, int square[4])
{
	int min = 63;
	int i, j, k;

	for (i = 0; i < 4; i++) {	/* Set the square to black */
		square[i] = 0;
	}
	if (c < min)
		min = c;
	if (m < min)
		min = m;
	if (y < min)
		min = y;
	/* min is the amount of black in the color, subtract it out! */
	c -= min;
	m = - min;
	y -= min;
	j = (min + 8) << 4;		/* # of black pixels */

	/* c = 1, m = 2, y = 4 in the next steps */
	if (c > 0) {
		k = (c + 8) << 4;	/* Amount of cyan */
		for (i = j; (i < j + k) && (i < 4); i++) {
			square[i] += 1;
		}
	}

	if (m > 0) {
		k = (m + 8) << 4;	/* Amount of magenta */
		for (i = j; (i < j + k) && (i < 4); i++) {
			square[i] += 2;
		}
	}

	if (y > 0) {
		k = (y + 8) << 4;	/* Amount of yellow */
		for (i = j; (i < j + k) && (i < 4); i++) {
			square[i] += 4;
		}
	}
}
/*************************************************************/
/* This routine takes a GraphiC color number, i, in the      */
/* ranges [0-15], [50-174], [200-447] and returns its        */
/* R, G, and B components [0-63] together with the closest   */
/* IBMcolor number.                                          */
/*************************************************************/

void TYPE GCLASS GRCtoIBM(
int i,	/* Color number from the patterns or from the standard colors */
int rgb[3],	/* array of three rgb values */
int *IBMcolor)
{
	int t, j = 0, k, m, n, out;

	if (i <= 0) {	/* Do the solid colors also */
		i = 135 - i;
	}
	else if (i >= 200 && i <= 447)
		i -= 65;
	else if (i < 16) {
		i += 135;
	}
	else if (i >= 50 && i < 175 ) {
		i -= 40;
	}
	else  	/* Error condition */
		i = 0;
	if (i > 134) {	/* Solid colors */
		if (i <= 166) {	/* Use table */
			j = i - 135;
			rgb[0] = IBMrgb[j][0];
			rgb[1] = IBMrgb[j][1];
			rgb[2] = IBMrgb[j][2];
		}
		else {	/* Generate arrays */
			j = i - 167;	/* local index */
			k = j/36;
			rgb[0] = delta[k];
			m = j%36;
			n = m/6;
			rgb[1] = delta[n];
			rgb[2] = delta[m%6];
		}
	}
	else {
		/* RED */
		t = cmaskq[i][1] | cmaskq[i][2] | cmaskq[i][6] | cmaskq[i][7];
		k = 1;
		out = 0;
		for (j = 0; j < 8; j++) {
			if (t & k) out++;
			k = k << 1;
		}
		rgb[0] = out;

		/* GREEN */
		t = cmaskq[i][3] | cmaskq[i][5] | cmaskq[i][1] | cmaskq[i][7];
		k = 1;
		out = 0;
		for (j = 0; j < 8; j++) {
			if (t & k) out++;
			k = k << 1;
		}
		rgb[1] = out;

		/* BLUE */
		t = cmaskq[i][1] | cmaskq[i][4] | cmaskq[i][6] | cmaskq[i][5];
		k = 1;
		out = 0;
		for (j = 0; j < 8; j++) {
			if (t & k) out++;
			k = k << 1;
		}
		rgb[2] = out;

		/* Make range 0-63 */
		for (k = 0; k < 3; k++) {
			rgb[k] *= 8;
			if (rgb[k] == 64)
				rgb[k]--;
		}
	}
	*IBMcolor = RGBtoIBM(rgb);
}
/*****************************************************************/
/* Return the closest IBM color [0-15] for a given R,G,B (0-63)  */
/*****************************************************************/
int TYPE GCLASS RGBtoIBM(int rgb[3])
{
	int k, t, j, n;
	int IBMcolor = 0;

	/* Get each color from pattern */
	k = 500;
	for (j = 0; j < 16; j++) {	/* Loop thru the IBM colors */
		t = 0;
		for (n = 0; n < 3; n++) {
			t += abs( rgb[n] - IBMrgb[j][n] );
		}
		if (t < k) {		/* better match */
			k = t;
			IBMcolor = j;
		}
	}
	return(IBMcolor);
}
/*************************************************************/
/* This routine takes R,G,B values and returns the closest   */
/* GraphiC color number [200-447]. It also returns the       */
/* closest IBM color number for compatibility.               */
/* Input r,g,b values run from 0-63                          */
/*************************************************************/
int TYPE GCLASS RGBtoGPC(int r, int g, int b, int *IBMcolor)
{
	int rgb[3];
	int rgbr[3];	/* Rounded index values */
	int k, n, d, m;

	rgb[0] = r; rgb[1] = g; rgb[2] = b;
	for (k = 0; k < 3; k++) {
		for (n = 0, d = 64, m = 0; n < 6; n++) {
			if (abs(rgb[k] - delta[n]) <= d) {
				d = abs(rgb[k] - delta[n]);
				m = n;
			}
		}
		rgbr[k] = m;
	}
	*IBMcolor = RGBtoIBM(rgb);
	return(232 + rgbr[0] * 36 + rgbr[1] * 6 + rgbr[2]);
}
