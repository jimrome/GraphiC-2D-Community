/* (c) 1984-1993 by Scientific Endeavors Corporation.
   All rights reserved.
This file contains the routines required for drawing bar charts.
Version 7.0 2/26/93
*************************************************************/
#include <graphic.h>

/* Bar draws a bar chart. Bars are centered on the x datum values. They
may be based on the lower edge of the grid space or on the y = 0 line.
tpe[?] provides shading options based on the panel filling options.
tpe==NULL draws hollow bars outlined in the color specified by car[?] or
if car==NULL in the color which was active when bar was called.

**************************************************************/
/* bar is called with nxdiv and npts. These numbers differ if an x
value is repeated, indicating a multiple-segment bar. npts is the
size of the x and y matrices. */

void TYPE GCLASS bar(int nxdiv, float *x, float *y, int npts, float bwid,
	int mode, int *car, int *tpe)
{
	int i, j;
	int x0, x1, y0, y1, delx, del2x;
	char array = 1;
	int dsh, ptype;
	int filled = 1;
	int color_sav;
	int ybase = 0;
	int yold = 0;
	int nbars = 1, maxbars = 0;
	float xsav;
	struct G_WIN *winmem = NULL;
	G_PROCESS_ID

	color_sav = g_lq.color;
	dsh = g_lq.pattern;
	dashf(1);
	if (tpe == NULL)
		filled = 0;
	if (car == NULL)
		array = 0;
	if (mode >= 10) {
		mode -= 10;
		ybase = 1;
	}
	delx = (int)(bwid * xlenq / (float)nxdiv * (float)0.5);
	del2x = delx / 2;
	y0 = yphysorq;
	xsav = x[0] - (float)1.;
	for (i = 0; i < npts; i++) {
		g_pq.xyp = (struct GXYPAIRSQ *)gpcalloc(5,
			sizeof(struct GXYPAIRSQ));
		if (g_pq.xyp == (void *)NULL) {
			messageq(ERRQ, 1);				/* Out of Memory */
			break;
		}
		y0 = yphysorq;
		if (ybase)
			y0 += (int)(.499999 - yfact_pltq * yorigq);
		usertopix(x[i], y[i], 0.0f, &x1, &y1);
		x1 += delx;
		x0 = x1 - delx - delx;
		if (x0 < xphysorq)
			x0 = xphysorq;
		if (x1 > xphysorq + xlenq)
			x1 = xphysorq + xlenq;
		if (x1 <= x0)
			continue;
		if (xsav == x[i]) {
			nbars++;
			if (nbars > maxbars)
				maxbars = nbars;
			if (mode == 1) {
				y1 += yold - y0;
				y0 = yold;
			}
			else if (mode == 2) {
				x0 += del2x * nbars;
				x1 += del2x * nbars;
			}
		}
		else {
			for (j = win_numq - 1; j >= 0; j--) {
				if (win_structq[j].type != GPC_IS_BAR)
					break;
				win_numq--;
			}
			if (win_numq > 0) {
				winmem = (struct G_WIN *)gpcrealloc(win_structq,
					win_numq * sizeof(struct G_WIN));
				if(winmem == NULL) {
					messageq(ERRQ, 1);				/* Out of Memory */
					gpcfree((void **) &win_structq);
					break;
				}
				else
					win_structq = winmem;
			}
			else
				gpcfree((void **) &win_structq);
			nbars = 0;
		}
		if (leg_flgq == LEG_ON) {
			ptype = styleq[nbars];
			if (ptype > 30) {
				filled = 1;
				ptype -= 50;
				if (ptype < 50 && ptype > 0)
					color(symclrq[nbars]);
				if (ptype <= 0)
					color((char)gabs(ptype));
			}
			else if (ptype < 10) {
				filled = 0;
				color(symclrq[nbars]);
			}
		}
		else {
			ptype = tpe[i];
			if (filled && ptype < 17) {
				if (array)
					color((char)car[i]);
			}
		}
		if (y[i] != 0) {
			g_pq.xyp[0].x = x1;				/* top right */
			g_pq.xyp[0].y = y1;
			g_pq.xyp[1].x = x1;				/* bottom right */
			g_pq.xyp[1].y = y0;
			g_pq.xyp[2].x = x0;
			g_pq.xyp[2].y = y0;
			g_pq.xyp[3].x = x0;
			g_pq.xyp[3].y = y1;
			g_pq.xyp[4].x = x1;
			g_pq.xyp[4].y = y1;
			if (filled) {
				g_pq.pattern = ptype;
				g_pq.nsides = 4;
				g_pq.border = 1;
				panelq();
				panproc();
			}
			else {
				lmoveq(g_pq.xyp[0].x, g_pq.xyp[0].y);
				for (j = 1; j <= 4; j++)
					ldrawq(g_pq.xyp[j].x, g_pq.xyp[j].y);
			}
			win2q(x0, y0, x1, y1, GPC_IS_BAR);
		}
		yold = y1;
		xsav = x[i];
		gpcfree((void **) &g_pq.xyp);
	}
	for (j = win_numq - 1; j >= 0; j--) {
		if (win_structq[j].type != GPC_IS_BAR)
			break;
		win_numq--;
	}
	if (win_numq > 0) {
		winmem = (struct G_WIN *)gpcrealloc(win_structq,
			win_numq * sizeof(struct G_WIN));
		if(winmem == NULL) {
			messageq(ERRQ, 1);				/* Out of Memory */
			gpcfree((void **) &win_structq);
		}
		else
			win_structq = winmem;
	}
	else
		gpcfree((void **) &win_structq);
	if (yzeroq != yphysorq) {
		lmoveq(xphysorq, y0);
		ldrawq(xphysorq + xlenq, y0);
	}
	dashf(dsh);
	color(color_sav);
	n_legq = maxbars + 1;
}

/********************************************************************
    FUNCTION NAME: histogram
    ARGUMENTS: x,y -- arrays for user data points
               npts -- number of user data points
               type -- type of histogram
					type == 0 for outline histogram
					type == 1 for hollow bar histogram
					type == 2 for filled bar histogram
               colors -- array of colors or panel patterns
					there must be npts elements in array
					the array may be NULL
    PURPOSE: Draw a histogram.
********************************************************************/
void TYPE GCLASS histogram(float *x, float *y, int npts, int type,
	int *colors)
{
	int i;
	int x0;
	int x1, y1;
	int dsh;
	int clr;
	G_PROCESS_ID

	usertopix(x[0], y[0], 0.0f, &x0, &y1);
	dsh = g_lq.pattern;
	clr = g_lq.color;
	if (type < 0 || type > 2){
		messageq(WARNQ, 18);				/* Invalid histogram type */
		type = 0;
	}
	dashf(1);				/* solid lines */
	lmoveq(x0, yphysorq);
	for (i = 0; i < npts; i++){
		usertopix(x[i], y[i], 0.0f, &x1, &y1);
		if ((x1 < xphysorq) || (x1 > xphysorq + xlenq))
			continue;
		if (type == 0){
			ldrawq(x0, y1);
			ldrawq(x1, y1);
		}
		else if (type == 1){
			if (colors != NULL)
				color(colors[i]);
			lmoveq(x0, yphysorq);
			ldrawq(x0, y1);
			ldrawq(x1, y1);
			ldrawq(x1, yphysorq);
			ldrawq(x0, yphysorq);
		}
		else{
			g_pq.xyp = (struct GXYPAIRSQ *)gpcalloc(5,
				sizeof(struct GXYPAIRSQ));
			if (g_pq.xyp == (void *)NULL) {
				messageq(ERRQ, 1);				/* Out of Memory */
				break;
			}
			g_pq.xyp[0].x = x0;
			g_pq.xyp[0].y = yphysorq;
			g_pq.xyp[1].x = x0;
			g_pq.xyp[1].y = y1;
			g_pq.xyp[2].x = x1;
			g_pq.xyp[2].y = y1;
			g_pq.xyp[3].x = x1;
			g_pq.xyp[3].y = yphysorq;
			g_pq.xyp[4].x = x0;
			g_pq.xyp[4].y = yphysorq;
			if (colors != NULL)
				g_pq.pattern = colors[i];
			else
				g_pq.pattern = 1;
			g_pq.nsides = 4;
			g_pq.border = 1;
			panelq();
			panproc();
			gpcfree((void **) &g_pq.xyp);
		}
		x0 = x1;
	}
	if (type == 0)
		ldrawq(x0, yphysorq);
	dashf(dsh);				/* restore previous line style */
	color(clr);
}
