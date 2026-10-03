/* (c) 1984-1993 by Scientific Endeavors Corporation.
   All rights reserved.
This file is used to generate linear axes with grid lines, tick marks and
labels.
Version 7.0 1/2/92
**************************************************************/
#include <graphic.h>

int xupq(int nxstep, char *xformat, int npix, float orig,
	int start, PLABFUNC lab, int lab_flag);
int yupq(int nxstep, char *xformat, int npix, float orig,
	int start, PLABFUNC lab, int lab_flag);
/***************************************************************/
/* LABEL AXES AND MAKE TICK MARKS */
void TYPE GCLASS graf(char *xformat, float xori, float xste, float xma, char *yformat,
	float yori, float yste, float yma)
{
	int dash_save;
	int nxstep, nystep;
	int npixx, npixy;
	G_PROCESS_ID

	xorigq = xori;
	yorigq = yori;
	xstepq = xste;
	ystepq = yste;
	xmaxq = xma;
	ymaxq = yma;

	if (crossq.flg & USER_WANTS_CROSSED_AXES) {
	/* user requested default crossed axes */
			/* SETUP FOR CROSSED AXES */
		if (((xma < 0 && xori < 0) || (xma > 0 && xori > 0)) &&
			((yma < 0 && yori < 0) || (yma > 0 && yori > 0))) {
			crossq.flg = (X_AXES_ON_BOTTOM | Y_AXES_ON_LEFT);
			messageq(WARNQ, 39);	/* axes can not be crossed at 0,0, both
									   ends of one of the axes less than or
									   greater than 0. */
		}
		else {
			if (xori < (float)0)
				/* y-axis not on left */
				crossq.flg &= ~Y_AXES_ON_LEFT;
			else
				/* y-axis on left */
				crossq.flg |= Y_AXES_ON_LEFT;
			if (xma <= (float)0) {
				/* y-axis on right */
				crossq.flg &= ~Y_AXES_ON_LEFT;
				crossq.flg |= Y_AXES_ON_RIGHT;
			}
			if (yori < (float)0)
				/* x-axis not on bottom */
				crossq.flg &= ~X_AXES_ON_BOTTOM;
			else
				/* x-axis on bottom */
				crossq.flg |= X_AXES_ON_BOTTOM;
			if (yma <= (float)0) {
				/* x-axis on top */
				crossq.flg &= ~X_AXES_ON_BOTTOM;
				crossq.flg |= X_AXES_ON_TOP;
			}
		}
	}
	plot_typeq |= LINEAR_PLOT;
	dash_save = g_lq.pattern;
	if (axis_flgq)
		dashf(1);
	if (lab_flgq & X_TICK_POS)
		nxstep = x_tickposq -> nsteps;
	else
		nxstep = (int)((xmaxq - xori) / xste + (float)RNDONE);
	if (lab_flgq & Y_TICK_POS)
		nystep = y_tickposq -> nsteps;
	else
		nystep = (int)((ymaxq - yori) / yste + (float)RNDONE);
	if (!psetflgq) {
		set_spacingq(nxstep, xformat, nystep, yformat);
		psetq();
	}
	npixx = xlenq / (nxstep);
	npixy = ylenq / (nystep);	/*#pixels per axis division*/
	xlenq = npixx * nxstep;
	ylenq = npixy * nystep;
	xmaxq = xori + xste * (float)nxstep;
	ymaxq = yori + yste * (float)nystep;
/* if the origins or the maxes are changed from the user's input, these
scaling factors need to be updated. */
	xfact_pltq = (float)xlenq / (xmaxq - xori);
	yfact_pltq = (float)ylenq / (ymaxq - yori);
	xzeroq = xphysorq;
	yzeroq = yphysorq;
	if (USER_SET_CROSS_LOC) {
		usertopix(crossq.x, crossq.y, 0.0f, &xzeroq, &yzeroq);
		if (xzeroq < xphysorq)
			messageq(WARNQ, 35);	/* Requested x value of crossed axes
									   too small. */
		if (yzeroq < yphysorq)
			messageq(WARNQ, 36);	/* Requested y value of crossed axes
									   too small */
		if (xzeroq > (xphysorq + xlenq))
			messageq(WARNQ, 37);	/* Requested x value of crossed axes
									   too big. */
		if (yzeroq > (yphysorq + ylenq))
			messageq(WARNQ, 38);	/* Requested y value of crossed axes
									   too big. */
	}
	if (!(USER_SET_CROSS_LOC)) {
		if (!X_ON_BOTTOM)
			yzeroq += (int)YSCALE_PLOT(-yori);
		if (!Y_ON_LEFT)
			xzeroq += (int)XSCALE_PLOT(-xori);
	}

		/* LABEL AND DRAW THE AXES */
	if (axis_flgq || frame_flgq) {
		if (!(right_flgq & LEFT_AXIS_DRAWN))
			xupq(nxstep, xformat, npixx, xori, xphysorq,
				AMF_GCLASS xlabq,
				(lab_flgq & (X_STRING_LABELS | X_TICK_POS)));
		yupq(nystep, yformat, npixy, yori, yphysorq,
			AMF_GCLASS ylabq,
			(lab_flgq & (Y_STRING_LABELS | Y_TICK_POS)));
		if (axis_flgq)
			dashf(dash_save);
	}
	labels2q();			/* Label axes and make heading. */
	crossq.flg &= ~USER_SET;
}
/*************************************************************/
/*Self-scaling routine to draw axes*/
void TYPE GCLASS scales(int nxdiv, int nydiv, float *x, float *y, int npts)
{
	float xmax, xmin, ymax, ymin, xste, yste;
	char xformat[8], yformat[8];

	minmax(&xmin, &xmax, x, npts);
	minmax(&ymin, &ymax, y, npts);
	roundq(nxdiv, &xmin, &xste, &xmax, xformat);
	roundq(nydiv, &ymin, &yste, &ymax, yformat);
	graf(xformat, xmin, xste, xmax, yformat, ymin, yste, ymax);
}
