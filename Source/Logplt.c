/* (c) 1984-1993 by Scientific Endeavors Corporation.
   All rights reserved.
These routines create axes with logarithmic divisions.
Rewritten for partial decades 3/16/87 - GGK
Version 7.0 1/2/91
**************************************************************/
#include <graphic.h>
#include <logplt.h>

#define BIG tkhtq
#define MED ((5*tkhtq)/7)
#define SMALL (tkhtq/2)
#define VSMALL (tkhtq/3)

#define GTEMF 0.0001

#if !GPC_CPP
extern int get_lab_posq(int ang, float lablht, int above, int on_edge,
	int *xp, int *yp, int width, int spc, int physor, int tick,
	int zero, int count, int shift_flg);
#endif

/**************************************************************/
/* SELF-SCALING ROUTINE FOR LOG X-AXIS */

void TYPE GCLASS xlgscle(int nydiv, float *x, float *y, int npts)
{
	axprepq(GPC_XAXIS, nydiv, x, y, npts); /* axis = GPC_XAXIS */
}
/*************************************************************/
/* SELF-SCALING ROUTINE FOR LOG Y-AXIS */

void TYPE GCLASS ylgscle(int nxdiv, float *x, float *y, int npts)
{
	axprepq(GPC_YAXIS, nxdiv, x, y, npts);
}
/*************************************************************/
/* SELF-SCALING ROUTINE FOR LOG-LOG AXIS */

void TYPE GCLASS lglgscle(float *x, float *y, int npts)
{
	int dummy = 0;
	axprepq(GPC_XAXIS | GPC_YAXIS, dummy, x, y, npts);
}
/*************************************************************/
/* DOES SELF-SCALING HOUSEKEEPING */

void GCLASS axprepq(int axis, int ndiv, float *x, float *y, int npts)
{
	float xmax, xmin, ymax, ymin, xste, yste;
	char format[10];
	G_PROCESS_ID

	axis_flgq |= GPC_AUTOSCAL;					/* Force full decades. */
	minmax(&xmin, &xmax, x, npts);
	minmax(&ymin, &ymax, y, npts);
	if (((axis & GPC_XAXIS) && xmin <= (float)0.0) ||
		((axis & GPC_YAXIS) && ymin <= (float)0.0))
		messageq(FATALQ, 5);		/* Numbers on log scale must be >0 */
	if (!(axis & GPC_YAXIS)) {		/*  Linear y axis */
		roundq(ndiv, &ymin, &yste, &ymax, format);
		xlog(xmin, xmax, format, ymin, yste, ymax);
	}
	else if (!(axis & GPC_XAXIS)) {		/* Linear x axis */
		roundq(ndiv, &xmin, &xste, &xmax, format);
		ylog(format, xmin, xste, xmax, ymin, ymax);
	}
	else
		loglog(xmin, xmax, ymin, ymax);
	axis_flgq = axis_flgq & ~GPC_AUTOSCAL;				/* Force full decades. */
}
/*************************************************************/
/* MANUAL SCALING OF X-LOG */

void TYPE GCLASS xlog(float xorig, float xmax, char *format, float yorig,
	float ystep, float ymax)
{
	G_PROCESS_ID

	plot_typeq |= XLOG_PLOT;
	xstepq = (xmax - xorig) / 10.f;
	xorigq = xorig;
	xmaxq = xmax;
	yorigq = yorig;
	ystepq = ystep;
	ymaxq = ymax;
	logscq(GPC_XAXIS, xorig, xmax, format);
}
/*************************************************************/
/* MANUAL SCALING OF Y-LOG */

void TYPE GCLASS ylog(char *format, float xorig, float xstep, float xmax,
	float yorig, float ymax)
{
	G_PROCESS_ID

	plot_typeq |= YLOG_PLOT;
	ystepq = (ymax - yorig) / 10.f;
	xorigq = xorig;
	xstepq = xstep;
	xmaxq = xmax;
	yorigq = yorig;
	ymaxq = ymax;
	if (right_flgq) {
		messageq(WARNQ, 11);		/* Can't have right-hand log axis */
		right_flgq = 0;
	}
	logscq(GPC_YAXIS, yorig, ymax, format);
}
/*************************************************************/
/* MANUAL SCALING OF LOG-LOG */

void TYPE GCLASS loglog(float xorig, float xmax, float yorig, float ymax)
{
	G_PROCESS_ID

	plot_typeq |= (XLOG_PLOT | YLOG_PLOT);
	xorigq = xorig;
	xmaxq = xmax;
	xstepq = (xmaxq - xorigq) / 10.f;
	yorigq = yorig;
	ymaxq = ymax;
	ystepq = (ymaxq - yorigq) / 10.f;
	if (right_flgq) {
		messageq(WARNQ, 11);		/* Can't have right-hand log axis */
		right_flgq = 0;
	}
	logscq(GPC_XAXIS, xorig, xmax, 0L);
	logscq(GPC_YAXIS, yorig, ymax, 0L);
}
/*************************************************************/
/* THE EXECUTIVE ROUTINE FOR MAKING THE AXES */

void GCLASS logscq(int axis, float orig1, float max1, char *format)
{
	double lpix, lfact, temf, ltem;
	int lrun, lexp, mexp, i, j, npix = 0;
	int expsav, nxstep, nystep, leng, lflag = 0;
	double pixpos, pixold;
	int pixint;
	float log_base;
	char start = 1;
	char use_all_sav = symoff(0);
	int line_style;
	char *ctemp1, *ctemp2;
	G_PROCESS_ID

	line_style = g_lq.pattern;
	dashf(1);
	crossq.flg &= ~AXES_ARE_CROSSED;	/* No crossed axes with log scale */
	if (lab_flgq & X_TICK_POS)
		nxstep = x_tickposq -> nsteps;
	else
		nxstep = (int)((xmaxq - xorigq) / xstepq + RNDONE);
	if (lab_flgq & Y_TICK_POS)
		nystep = y_tickposq -> nsteps;
	else
		nystep = (int)((ymaxq - yorigq) / ystepq + RNDONE);

	if (!psetflgq) {
		set_spacingq(nxstep, format, nystep, format);
		psetq();
	}

	if (axis & GPC_XAXIS)
		log_base = (float)x_log_baseq;
	else
		log_base = (float)y_log_baseq;

	if ((axis & GPC_XAXIS) || (right_flgq & LEFT_AXIS_DRAWN)) {
		pixpos = (double)xphysorq;
		leng = xlenq;			/* Prepare for rt axis or upright labels. */
	}
	else {
		pixpos = (double)yphysorq;
		leng = ylenq;
	}

	pixint = (int)(pixpos + .5);
	temf = orig1;
	temf = roundoffq(temf, GTEMF);		/* Clean up temf and max1 */
	lexp = getexpq(&temf, log_base);			/* Returns decade and normalized value. */
	orig1 = (float)temf;
	temf = max1;
	temf = roundoffq(temf, GTEMF);
	mexp = getexpq(&temf, log_base);
	max1 = (float)temf;
	expsav = lexp;

	/* If the limits of the data are on even decades, max1 and orig1
	will be equal to 1. If the rounding process leaves some value to the
	right of the decimal, orig should equal one with no change in the lexp
	value and max1 should be made equal to 1 and mexp should be
	incremented. The result is that max rounds up to the nearest decade
	and orig rounds down., */
	if (axis_flgq & GPC_AUTOSCAL) { /* Force full decades with autoscale. */
		orig1 = (float)1.0;
		if ((int)(max1 + .9999999) != (int)max1) { /* Not an even decade. */
			mexp++;
			max1 = 1.f;
		}
	}

	if (axis & GPC_XAXIS) {
		xorigq = (float)glogq((double)orig1, axis) + expsav;
		xmaxq = (float)glogq((double)max1, axis) + mexp;
		xfact_pltq = xlenq / (xmaxq - xorigq);
	}
	else {
		yorigq = (float)glogq((double)orig1, axis) + expsav;
		ymaxq = (float)glogq((double)max1, axis) + mexp;
		yfact_pltq = ylenq / (ymaxq - yorigq);
	}

	lfact = (double)leng / (glogq((double)max1, axis) -
		glogq((double)orig1, axis) + mexp - lexp);

	lrun = (int)floor(orig1) - 1;
	if (lrun < 0)
		lrun = 0;

	j = 0;
	/* STEP ALONG LOG AXIS FROM orig1 TO max1 */
	for (;;) {
		lrun++;
		if ((start && lrun == 1) || lrun >= (int)(log_base + .5)) {
												/* Need to increment decade. */
			if (lrun >= (int)(log_base + .5))
				lexp++;
			if (axis & GPC_XAXIS)
				pixint = xphysorq + (int)XSCALE_PLOT((float)lexp - xorigq);
			if (axis & GPC_YAXIS)
				pixint = yphysorq + (int)YSCALE_PLOT((float)lexp - yorigq);
			tickq(axis, BIG, pixint);				/* Big tick called for */
			axlabq(axis, lexp, pixint, 1, j++);	/* Label the tick. */
			pixpos = (float)pixint;
			lflag = 1;
			lrun = 1;
			start = 0;
		}
		else
			tickq(axis, MED, pixint);
		pixold = pixpos;
		if (lexp == mexp && lrun >= (int)floor(max1))
			break;
		if (mexp >= 0 || (axis & GPC_AUTOSCAL)) {
			if (lexp == (mexp + 1) && floor(max1) >= (int)(log_base + .5))
				break;
		}
		else {
			if (lexp == (mexp - 1) && floor(max1) >= (int)(log_base + .5))
				break;
		}
		ltem = (double)lrun;
		lpix = lfact * (glogq(ltem + 1, axis) -
			glogq(ltem, axis)); /* Major inc. */
		if (lpix < (660. * xpageq) / acrossq) {
			pixpos += lpix;			/* No intermediate ticks */
			pixint = (int)(pixpos + .5);
			continue;
		}
		if (lpix < (900. * xpageq) / acrossq) {
										/* Make 3 intermediate steps. */
			if (!lflag)
				axlabq(axis, lrun, pixint, 2, j++);
			for (i = 0; i < 3; i++) {
				pixpos += lfact * (glogq(ltem + .25, axis) -
					glogq(ltem, axis));
				pixint = (int)(pixpos + .5);
				if (i == 1)
					tickq(axis, SMALL, pixint);
				else
					tickq(axis, VSMALL, pixint);
				ltem += .25;
			}
			pixpos = pixold + lpix;
			pixint = (int)(pixpos + .5);
			if (lrun < (int)log_base - 1)
				axlabq(axis, lrun + 1, pixint, 2, j++);
			lflag = 1;
		}
		else {						/* Make 9 intermediate steps. */
			if (!lflag)
				axlabq(axis, lrun, pixint, 2, j++);
			for (i = 0; i < 9; i++) {
				pixpos += lfact * (glogq(ltem + .1, axis) -
					glogq(ltem, axis));
				pixint = (int)(pixpos + .5);
				if (i == 4) {
					tickq(axis, SMALL, pixint);
					axlabq(axis, lrun, pixint, 3, j++);
				}
				else
					tickq(axis, VSMALL, pixint);
				ltem += .1;
			}
			pixpos = pixold + lpix;
			pixint = (int)(pixpos + .5);
			if (lrun < 9)
				axlabq(axis, lrun + 1, pixint, 2, j++);
			lflag = 1;
		}
	}

/* RESET SCALING PARAMETERS FOR CURVE() AND DO LINEAR AXES. */
	lgaxisq(axis, pixold);					/* Draw the log axis. */
	if (axis & GPC_XAXIS) {
		if (!(plot_typeq & YLOG_PLOT)) {
			npix = ylenq / nystep;
			ylenq = npix * nystep;
			if (axis_flgq & GPC_YAXIS)
				yupq(nystep, format, npix, yorigq, yphysorq,
					AMF_GCLASS ylabq,
					(lab_flgq & (Y_STRING_LABELS | Y_TICK_POS)));
			ctemp1 = xnameq.string;
			xnameq.string = NULL;
			labels2q();
			xnameq.string = ctemp1;
			yfact_pltq = ylenq / (ymaxq - yorigq);
		}
		ctemp1 = ynameq.string;
		ctemp2 = titleq.string;
		ynameq.string = NULL;
		titleq.string = NULL;
		labels2q();
		ynameq.string = ctemp1;
		titleq.string = ctemp2;
	}
	else {
		if (!(plot_typeq & XLOG_PLOT)) {
			npix = xlenq / nxstep;
			xlenq = npix * nxstep;
			if (axis_flgq & GPC_XAXIS)
				xupq(nxstep, format, npix, xorigq, xphysorq,
					AMF_GCLASS xlabq,
					(lab_flgq & (X_STRING_LABELS | X_TICK_POS)));
			ctemp1 = ynameq.string;
			ynameq.string = NULL;
			labels2q();
			ynameq.string = ctemp1;
			xfact_pltq = xlenq / (xmaxq - xorigq);
		}
		ctemp1 = xnameq.string;
		xnameq.string = NULL;
		labels2q();
		xnameq.string = ctemp1;
	}
	dashf(line_style);
	symoff(use_all_sav);
}
/**************************************************************/
/* DRAW LOGARITHMIC AXES */

void GCLASS lgaxisq(int axis, double pixold)
{
	int i;
	G_PROCESS_ID

	if (right_flgq & LEFT_AXIS_DRAWN)
		return;
	if (!axis_flgq) {		/* set xlen and ylen without drawing axes */
		if (axis & GPC_XAXIS)
			xlenq = (int)(pixold - xphysorq);
		else
			ylenq = (int)(pixold - yphysorq);
		return;
	}
	for (i = 0; i <= 4; i++) {
		if (axis & GPC_XAXIS) {
			if (i == 0)
				xlenq = (int)(pixold - xphysorq);
			if ((frame_flgq) || (axis_flgq & GPC_XAXIS))
				lineq(xphysorq - i, yphysorq - i, xphysorq +
					xlenq, yphysorq - i);
		}
		else {
			ylenq = (int)(pixold - yphysorq);
			if ((frame_flgq) || (axis_flgq & GPC_YAXIS))
				lineq(xphysorq - i, yphysorq + ylenq,
					xphysorq - i, yphysorq - i);
		}
		if (frame_flgq && axis_flgq) {
			lineq(xphysorq + xlenq + i, yphysorq - i, xphysorq
				 + xlenq + i, yphysorq + ylenq);
			lineq(xphysorq + xlenq + i, yphysorq + ylenq
				 + i, xphysorq - i, yphysorq + ylenq + i);
		}
	}
}
/*************************************************************/
/* DRAW THE LOGARITHMIC TICKS */

void GCLASS tickq(int axis, int size, int pos)
{
	int dashsav;
	G_PROCESS_ID

	if ((right_flgq & LEFT_AXIS_DRAWN) || !axis_flgq)
		return;
	if (axis & GPC_XAXIS) {
		lineq(pos, yphysorq, pos, yphysorq + size);
		if (frame_flgq & DRAW_FRAME_TICKS)
			lineq(pos, yphysorq + ylenq, pos, yphysorq + ylenq
				 - size);
	}
	else {
		lineq(xphysorq, pos, xphysorq + size, pos);
		if (frame_flgq & DRAW_FRAME_TICKS)
			lineq(xphysorq + xlenq, pos, xphysorq + xlenq - size, pos);
	}
	if (((axis & GPC_XAXIS) && xgrid_flgq)
		 || ((axis & GPC_YAXIS && ygrid_flgq))) {
		dashsav = g_lq.pattern;
		if (size == BIG) {
			if ((axis & GPC_XAXIS) && xgrid_flgq) {
				if (xgrid_flgq > -1)
					dashf(xgrid_flgq);
				lineq(pos, yphysorq, pos, yphysorq + ylenq);
			}
			else if ((axis & GPC_YAXIS) && ygrid_flgq) {
				if (ygrid_flgq > -1)
					dashf(ygrid_flgq);
				lineq(xphysorq, pos, xphysorq + xlenq, pos);
			}
		}
		else if (size == MED) {
			if ((axis & GPC_XAXIS) && xfgrid_flgq > 0) {
				dashf(xfgrid_flgq);
				lineq(pos, yphysorq, pos, yphysorq + ylenq);
			}
			else if ((axis & GPC_YAXIS) && yfgrid_flgq > 0) {
				dashf(yfgrid_flgq);
				lineq(xphysorq, pos, xphysorq + xlenq, pos);
			}
		}
		dashf(dashsav);
	}
}
/*************************************************************/
/* LABEL THE AXES */

void GCLASS axlabq(int axis, int number, int pos, int type, int count)
{
	int width, xp, yp, ang;
	int lab_spc;
	char *lab = NULL;
	float lbase;
	float labht;
	int lab_flag, shift = 0;
	G_PROCESS_ID

	if ((right_flgq & LEFT_AXIS_DRAWN) || !axis_flgq)
		return;

	if (count < 0)
		return;
	lab_flag = 0;
	if (axis & GPC_XAXIS) {
		lbase = (float)x_log_baseq;
		if (lab_flgq & X_STRING_LABELS) {
			lab = xlabq(0, NULL, count + 1);
			lab_flag = 1;
		}
	}
	else {
		lbase = (float)y_log_baseq;
		if (lab_flgq & Y_STRING_LABELS) {
			lab = ylabq(0, NULL, count + 1);
			lab_flag = 1;
		}
	}
	if (!lab_flag) {
		lab = (char *)gpcalloc(LABLEN, sizeof(char));
		if (type == 1) {
			if (lbase < (float)3.) {
				if ((int)(lbase * (float)10000.) == (int)(BASE_E * 10000.))
					sprintfq(lab, "e[%d]", number);
				else {
					kprintfq(lab, "%-1.0f", lbase);
					sprintfq(stringq, "[%d]", number);
					strcat(lab, stringq);
				}
			}
			else {
				kprintfq(lab, "%-1.0f", lbase);
				sprintfq(stringq, "[%d]", number);
				strcat(lab, stringq);
			}
		}
		else if (type == 2)
			sprintfq(lab, "%d", number);
		else
			kprintfq(lab, "%-1.1f", (float)number + .5f);
	}
	if (lab == NULL)
		return;
	if (type == 1)
		labht = lablhtq;
	else
		labht = (float)0.7 * lablhtq;
	xp = 0;
	yp = 0;
	width = strwidtq(lab, labht);
	if ((axis & GPC_XAXIS) && (axis_flgq & GPC_XLAB)){
		ang = x_lab_angq(0, 1);
		if (ang == DEFAULT_INT)
			ang = 0;
		lab_spc = get_lab_posq(ang, lablhtq, 0, 1, &xp, &yp,
			width, xlspcq, xphysorq, pos, xzeroq, 0, 0);
		if (ang == DEFAULT_INT)
			ang = 0;
		yp = yzeroq - yp;
		fntpltq(xp, yp, lab, labht, ang);
		if (!(ang % 180))
			lab_spc = (int)lablhtq + xlspcq + ((tkhtq < 0) ? -tkhtq : 0);
		lab_wid_setq(GPC_X_AXIS, lab_spc);
	}
	else if ((axis & GPC_YAXIS) && (axis_flgq & GPC_YLAB)) {
		ang = y_lab_angq(0, 1);
		if (ang == DEFAULT_INT)
			ang = 0;
		if (pos - yphysorq < width)
			shift = 1;
		lab_spc = get_lab_posq(ang, lablhtq, 1, 1, &yp, &xp,
			width, ylspcq, yphysorq, pos, yzeroq, 0, shift);
		xp = xzeroq - xp;
		fntpltq(xp, yp, lab, labht, ang + 90);
		if (!(ang % 180))
			lab_spc = (int)lablhtq + ylspcq + ((tkhtq < 0) ? -tkhtq : 0);
		lab_wid_setq(GPC_Y_AXIS, lab_spc);
	}
	if (!lab_flag)
		gpcfree((void **) &lab);
}
/************************************************************************/
/* This routine sets the base for taking the logs of data allowing the
user to specify a base other than ten. All calls to the C-runtime log
function filter through g_logq() where the base is applied. */

void TYPE GCLASS SetLogBase(double base_x, double base_y)
{
	G_PROCESS_ID

	if (base_x == (double)'e' || base_x == (double)'E')
		x_log_baseq = BASE_E;
	else
		x_log_baseq = base_x;
	if (base_y == (double)'e' || base_y == (double)'E')
		y_log_baseq = BASE_E;
	else
		y_log_baseq = base_y;
}
