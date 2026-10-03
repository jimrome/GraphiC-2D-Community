/* (c) 1993 by Scientific Endeavors Corp.
	All rights reserved.

	Routines for drawing curves and error bars.
	Vesrion 7.0 3/2/93
********************************************************/
#include <graphic.h>

/**************************************************************/
/* draw a curve of y versus x with npts of data*/
/* If plot_typeq&2 (xlog) OR plot_typeq&4 (ylog) IS 1, ITS A LOG SCALE */
void TYPE GCLASS curve(
	float *x,	/* pointer to the array of x values */
	float *y,	/* pointer to the array of y values */
	int npts,		/* dimension of the x and y arrays  */
	int sym			/* The symbol to be used in the curve:
					If sym = 0 --- DRAW CURVE ONLY
					If sym > 0 --- DRAW CURVE AND SYMBOLS EVERY SYM PTS
					If sym < 0 --- DRAW POINTS ONLY EVERY SYM POINTS */
)
{
	int dsh1 = 1, clrsav, tlsav;
	int symsav = 1;
	int x1, y1;
	int i, abssym, ipts;
	double xt, yt;
	static double powmax, powmin;
	static char first = 1;
	double xbig, xsmall, ybig, ysmall;
	int message = 0;
	int wplane;
	G_PROCESS_ID

	wplane = wplane_flgq;
	if (sim_flgq)
		g_lq.penup = 1;
	tlsav = g_lq.thick;		/* save thickened line value for resetting */
	if (tcurveq != 0)
		tline(tcurveq);
	clrsav = g_lq.color;
	if (leg_flgq == LEG_ON && n_legq < strcntq) {
		if (styleq[n_legq] < 10) {
			dsh1 = g_lq.pattern;
			dashf(styleq[n_legq]);
		}
		else {
			symsav = symcntq;
			symcntq = styleq[n_legq] - 10;
		}
		if (g_lq.color != symclrq[n_legq])
			color(symclrq[n_legq]);
	}
	n_legq++;
	if (npts <= 0) {
		messageq(WARNQ, 28);		/* Number of points cannot be zero */
		return;
	}
	if (!crop_flgq && !wplane)
		acropq(1);
	else
		acropq(0);
	/* Create limits to be used to indicate scaling the input values
		will overflow our integer-based plot. Absolute values need
		to be used to handle the case where an axis goes from large
		to small. */
	xbig = (float)(32767 - xphysorq) / xfact_pltq + xorigq;
	xsmall = xorigq - (float)(32767 - xphysorq) / xfact_pltq;
	ybig = (float)(32767 - yphysorq) / yfact_pltq + yorigq;
	ysmall = yorigq - (float)(32767 - yphysorq) / yfact_pltq;
	if (xbig < xsmall) {
		xt = xbig;
		xbig = xsmall;
		xsmall = xt;
	}
	if (ybig < ysmall) {
		yt = ybig;
		ybig = ysmall;
		ysmall = yt;
	}
	if (first && (plot_typeq & (XLOG_PLOT | YLOG_PLOT))) {
		/* Find max allowed exponents */
		powmax = glogq((double)FLT_MAX, GPC_XAXIS);
		powmin = glogq((double)FLT_MIN, GPC_YAXIS);
		first = 0;
	}
	if (plot_typeq & XLOG_PLOT) {
		if (xbig > powmax)
			xbig = powmax;
		if (xbig < powmin)
			xbig = powmin;
		xbig = pow(10.0, xbig);
		xsmall = pow(10.0, xsmall);
	}
	if (plot_typeq & YLOG_PLOT) {
		if (ybig > powmax)
			ybig = powmax;
		if (ybig < powmin)
			ybig = powmin;
		ybig = pow(10.0, ybig);
		ysmall = pow(10.0, ysmall);
	}
	abssym = gabs(sym);
	if (!dotq && npts > 3 && sym >= 0 && !panfilq) {
		if (g_lq.xyp != NULL)
			gpcfree((void **) &g_lq.xyp);
		g_lq.xyp = (PGXYPAIRS)gpcalloc(64, sizeof(struct GXYPAIRSQ));
		g_lq.action = 1;
		g_lq.npts = 1;
	}
	else
		g_lq.action = 0;
	npts--;
	for (i = 0, ipts = 0; i <= npts; i++) {
		/* Check for NANs */
		if((IsNotRealFloat(x[i])) || (IsNotRealFloat(y[i]))) {
			ipts = 0;
			continue;	/* Don't have anything to plot */
		}
		xt = x[i];
		yt = y[i];
		if (xt > xbig || xt < xsmall) {
			if (xt > xbig)
				xt = xbig;
			else
				xt = xsmall;
			if (!(message & 1)) {
				messageq(WARNQ, 13);		/* X value out of range */
				message |= 1;
			}
		}
		if (yt > ybig || yt < ysmall) {
			if (yt > ybig)
				yt = ybig;
			else
				yt = ysmall;
			if (!(message & 2)) {
				messageq(WARNQ, 14);		/* Y value out of range */
				message |= 2;
			}
		}
		usertopix((float)xt, (float)yt, axposq,
			(int *)&x1, (int *)&y1);	/* Treats log case */
		if (!ipts) {	/* First pt or first after a NAN */
			lmoveq(x1, y1);
			if (dotq)
				ldrawq(x1, y1);
		}
		else if (sym >= 0) {
			if (dotq)
				lmoveq(x1, y1);
			ldrawq(x1, y1);
		}
		if (sym && !(i % abssym))
			sympltq(x1, y1, symcntq);
		ipts++;
	}
	if (g_lq.action) {
		g_lq.xyp[0].x = g_lq.npts - 1;
		f_tableq(&g_lq);
		gpcfree((void **) &g_lq.xyp);
		g_lq.action = 0;
		g_lq.npts = 0;
	}
	if (leg_flgq == LEG_ON && n_legq < strcntq) {
		if (styleq[n_legq - 1] < 10)
			dashf(dsh1);
		else
			symcntq = symsav;
		color(clrsav);
	}
	if (tcurveq != 0)
		tline(tlsav);
	acropq(0);
	return;
}

/**************************************************************/
/* PLOT ERROR BARS AT POINT  x, y*/
void TYPE GCLASS errorbar(
	float x,	/* x data point */
	float xmin,	/* minimum x value */
	float xmax,	/* maximun x value */
	float y,	/* y data point */
	float ymin,	/* minimum y value */
	float ymax	/* maximun y value */
)
{
	int tlold;
	int dashold;
	int symold;
	G_PROCESS_ID

	tlold = (char)(g_lq.thick * 2);
	dashold = g_lq.pattern;
	symold = symcntq;
	if (!crop_flgq && !wplane_flgq)
		acropq(1);
	else
		acropq(0);
	dashf(1);
	if (tcurveq != 0)
		tline(0);
	if ((ymax - ymin) > 0) {
		uline(x, ymin, x, ymax, 3);
		symbol(x, ymin, 9);
		symbol(x, ymax, 9);
	}
	if ((xmax - xmin) > 0) {
		uline(xmin, y, xmax, y, 3);
		symbol(xmin, y, 10);
		symbol(xmax, y, 10);
	}
	acropq(0);
	symcntq = symold;
	tline(tlold);
	dashf(dashold);
}
/*********************************************************************/
/* This function transforms the input vectors into a "staircase" set
of output vectors. The y value is held constant between x data points.
The jump points are determined by the variable mode.
*/
void TYPE GCLASS staircase(
	float *x,		/* Pointer to input x vector (dimension npts) */
	float *y,		/* Pointer to input y vector (dimension npts) */
	float *xnew,	/* Pointer to output x vector (dimension npts * 2) */
	float *ynew,	/* Pointer to output y vector (dimension npts * 2) */
	int *npts,			/* Dimension of input vectors. It will return the
						dimension of the output vectors */
	int mode)			/* 	= 0 jumps occur at the midpoints of x
							= 1 jumps occur at the first x
							= 2 jumps occur at the second x
						*/
{
	int i, j;
	float xo, xn, xc;

	xnew[0] = xo = x[0];
	ynew[0] = y[0];
	j = 1;

	switch(mode) {
		case 0:	/* Jump levels between data points */
			/* First point there */
			for (i = 1; i < *npts; i++) {
				xn = x[i];
				xc = (float)0.5 * (xn + xo);
				/* Old level */
				ynew[j] = y[i-1];
				xnew[j++] = xc;
				/* New level */
				ynew[j] = y[i];
				xnew[j++] = xc;
				xo = xn;
			}
			i--;
			xnew[j] = x[i];
			ynew[j++] = y[i];
			break;
		case 1:	/* Jump levels at first data point */
			/* First point there */
			for (i = 1; i < *npts; i++) {
				xn = x[i];
				/* Old level */
				ynew[j] = y[i];
				xnew[j++] = xo;
				/* New level */
				ynew[j] = y[i];
				xnew[j++] = xn;
				xo = xn;
			}
			break;
		case 2:	/* Jump at second point */
			/* First point there */
			for (i = 1; i < *npts; i++) {
				xn = x[i];
				/* Old level */
				ynew[j] = y[i-1];
				xnew[j++] = xn;
				/* New x */
				ynew[j] = y[i];
				xnew[j++] = xn;
			}
			break;
	}

	*npts = j;
}
/***********************************************************************/
void TYPE GCLASS scatplot(char dot)
{
	G_PROCESS_ID

	dotq = dot;
}
