/* (c) 1984-1993 by Scientific Endeavors Corporation.
   All rights reserved.
This file contains legpos() and legend() which are used to define
and set up a legend. All calculations and all calls are in pixel units. In
this way, the scaling for change of picture size and for offsets is taken
care of in the same way as in other routines.
Version 7.0 3/3/93
**************************************************************/
#include <graphic.h>

/*********************************************************/
/* Positions the upper left hand corner of the legend */
void TYPE GCLASS legpos(int entries, float xleg, float yleg, int border)
{
	int *style, *thick, *sym;
	G_PROCESS_ID

		/* Pos. in pixels on the page */
	xlegq = (int)XSCALE_PAGE((intopix(xleg))) + xbaseq;
	ylegq = (int)YSCALE_PAGE((intopix(yleg))) + ybaseq;
	iq = n_legq = OFF;
	leg_flgq = LEG_ON;
	style = (int *)gpcrealloc(styleq, (size_t)(entries * sizeof(int)));
	sym = (int *)gpcrealloc(symclrq, (size_t)(entries * sizeof(int)));
	thick = (int *)gpcrealloc(curve_thickness,
		(size_t)(entries * sizeof(int)));
	if (style == NULL || sym == NULL ||	thick == NULL) {
		gpcfree((void **) &symclrq);
		gpcfree((void **) &styleq);
		gpcfree((void **) &curve_thickness);
		messageq(ERRQ, 1);		/* Out of Memory */
		leg_flgq = LEG_OFF;
	}
	symclrq = sym;
	styleq = style;
	curve_thickness = thick;
	strcntq = entries;
	leg_txtclr = g_lq.color;
	legwidq = OFF;
	drawb = ON;
	if (border == 0)
		drawb = OFF;
}
/***********************************************************/

void TYPE GCLASS legend(int marker, char *string, int style, float strhgt, int scolor)
{
	float hgt;
	float symhtsav;
	int dashsav;
	int plotsav;
	int tlsav;
	int linxpos, linypos, width, x0, y0, x1, y1;
	int symnum;
	static int n;
	G_PROCESS_ID

	if (leg_flgq == LEG_OFF)
		return;
	symhtsav = symhtq;
	dashsav = g_lq.pattern;
	plotsav = g_lq.color;
	tlsav = g_lq.thick;
	switch(marker) {
		case 0:									/* Entry is a symbol. */
			if (style < 0 || style > 16) {
				messageq(ERRQ, 22);		/* Invalid symbol style */
				style = 0;
			}
			styleq[iq] = style + 10;					/* symbol style */
			break;
		case 1:									/* Entry is a line. */
			if (style < 1 || style > 9) {
				messageq(ERRQ, 23);		/* Invalid line style */
				style = 1;
			}
			styleq[iq] = style;
			break;
		case 2:							/* Entry is a filled pattern. */
			styleq[iq] = style + 50;
			break;
	}
	symht(strhgt);
	symclrq[iq] = scolor;			/* color for curve or panel border */
	curve_thickness[iq] = tcurveq;
	iq++;
	hgt = YSCALE_PAGE(pix_convertq * strhgt);					/* Height in pixels */
	width = strwidtq(string, hgt);
	width += width;								/* Full width in pixles */
	if (width > legwidq)
		legwidq = width;			/* Get maximum width. */
	/* (.6 + 4.3 + .6)*hgt -- x offset */
	x0 = xlegq + (int)(5.5 * hgt + .5);
	/* First string is down 1.6*hgt. */
	y0 = ylegq - (int)(1.6 * hgt * iq + .5);
	fntpltq(x0, y0, string, hgt, 0);
	linxpos = xlegq + (int)(.6 * hgt + .5);
										/* Sample lines are offset .6*hgt. */
	if (iq == strcntq) {					/* Last time through */
		for (n = 0; n < strcntq; n++) { /* Draw sample lines and symbols. */
			linypos = ylegq - (int)(hgt * (1.6 * n + 1.1) + .5);
														/* Center on string. */
			if (g_lq.color != symclrq[n])
				color(symclrq[n]);
			if (styleq[n] < 10) {			/* This is a dashed line */
				dashf(styleq[n]);
				tline(curve_thickness[n]);
				line(linxpos, linypos, linxpos + (int)(4.3 * hgt + .5),
					linypos, 3);
				tline(tlsav);
				dashf(dashsav);
			}
			else if (styleq[n] <= 30) {		/* This is a symbol. */
				symnum = styleq[n] - 10;
				sympltq(linxpos + (int)(hgt * 2.15 + .5), linypos,
					(char)symnum);
			}
			else {						/* This is a panel-filled box. */
				symfillq = styleq[n] - 30;
									/* Tell lprocq() to use panel pattern. */
				color(symclrq[n]);
				sympltq(linxpos + (int)(hgt * 2.15 + .5), linypos, 20);
			}
		}
		x0 = xlegq;								/* Bottom left corner */
		y0 = ylegq - (int)(hgt * (1.6 * iq + .8) + .5);
											/* 0.8*hgt for border & dec. */
		x1 = xlegq + legwidq + (int)(6.1 * hgt + .5);
										/* Leaves .6*hgt for rt. border. */
		y1 = ylegq;
		color(leg_txtclr);
		if (drawb == 1)
			win2q(x0, y0, x1, y1, GPC_HAS_BORDER);
		else
			win2q(x0, y0, x1, y1, GPC_NO_BORDER);
		gpcfree((void **) &curve_thickness);
	}
	color(plotsav);
	symhtq= symhtsav;
}
/*********************************************************************
Sets leg_flgq to 0 to turn off the automatic line style and color changing
feature. */
void TYPE GCLASS autolegend(char on_off)
{
	G_PROCESS_ID

	if (leg_flgq == LEG_OFF)
		return;
	if (on_off == 0)
		leg_flgq = LEG_DISABLED;
	else
		leg_flgq = LEG_ON;
}
