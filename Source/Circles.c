/* (c) 1993 by Scientific Endeavors Corp.
This module contains the circle, ellipse, and radial line drawing
routines.

V 7.0 3/1/93 */

/************************************************************/
#include <graphic.h>

/************************************************************/
/* TURN ON / OFF FILLED ELLIPSE */
void TYPE GCLASS fillellipse(int onoff, int pattern, int border)
{
	G_PROCESS_ID

	if (onoff == 1){
		filled_ellipseq = 1;
		ellipse_patternq = pattern;
		if (border ==  1)
			ellipse_borderq = 1;
		else
			ellipse_borderq = 0;
	}
	else
		filled_ellipseq = 0;
}

/*************************************************************
ellipse() draws a segment of an ellipse centered at xc, yc.
The angular increment is between th1 and th2.
The size and position are referenced to page(). */
void TYPE GCLASS ellipse(float xc, float yc, float xr, float yr, float rotation,
	float th1, float th2, char degrees, int units)
{
	int xl, yl, xrl, yrl;
	float degtorad = (float)0.0174533;
	G_PROCESS_ID

	if (degrees){
		rotation *= degtorad;
		th1 *= degtorad;
		th2 *= degtorad;
	}
	switch(units) {
		case USER_UNITS:
			usertopix((float)xc, (float)yc, 0.0f,
				(int *)&xl, (int *)&yl);
		/*
			CANNOT CALL usertopix() HERE!!  FOR THE RADII, WE WANT
			DISTANCES INSTEAD OF COORDINATES.
		*/
			if ((plot_typeq & POLAR_PLOT) || (plot_typeq & POLAR_LOG_PLOT)) {
				xrl = (int)(xr * rfact_polq);
				yrl = (int)(yr * rfact_polq);
			}
			else {
				if (plot_typeq & XLOG_PLOT) {
					if (xr != (float)0.0)
						xr = (float)log10((double)xr);
				}
				if (plot_typeq & YLOG_PLOT) {
					if (yr != (float)0.0)
						yr = (float)log10((double)yr);
				}
				xrl = (int)(xr * xfact_pltq);
				yrl = (int)(yr * yfact_pltq);
			}
			break;
		case INCH_UNITS:
			xl = (int)XSCALE_PAGE(intopix(xc)) + xbaseq;
			yl = (int)XSCALE_PAGE(intopix(yc)) + ybaseq;
			xrl = (int)XSCALE_PAGE(intopix(xr));
			yrl = (int)YSCALE_PAGE(intopix(yr));
			break;
		default:
			xl = (int)xc;
			yl = (int)yc;
			xrl = (int)xr;
			yrl = (int)yr;
			break;
	}
	ellipseq(xl, yl, xrl, yrl, rotation, th1, th2);
}

/**************************************************************
ellipseq() draws an ellipse segment in pixel units. */
void GCLASS ellipseq(int xc, int yc, int xr, int yr,
	float rotation, float th1, float th2)
{
	double del, delt, x0, y0, crot, srot;
	int x1, y1, i, npts = 127;
	float twopi = (float)6.2831853, dth= (float)0.04986655;
	G_PROCESS_ID

	if (fabs(th2 - th1) > twopi) {                     /* normalize angles */
		th1 = (float)0.;
		th2 = twopi;
	}
	while (th1 > th2)
		th2 += twopi;
	while (th1 > twopi) {
		th2 -= twopi;
		th1 -= twopi;
	}
	npts = (int)((th2 - th1) / dth);
	if (npts < 2)
		npts = 2;
	del = (th2 - th1) / (float)(npts - 1);
	delt = th1;

	if (filled_ellipseq) {                        /* allocate panel arrays */
		if (g_pq.xyp == (void *)NULL) {
			g_pq.xyp = (struct GXYPAIRSQ *)gpcalloc((size_t)(npts + 1),
				sizeof(struct GXYPAIRSQ));
			if (g_pq.xyp == (void *)NULL) {
				messageq(ERRQ, 1);		/* Out of Memory */
				filled_ellipseq = 0;
			}
		}
	}
	if (!filled_ellipseq) {
		if (g_lq.npts > 1 && g_lq.action) {
			g_lq.xyp[0].x = g_lq.npts - 1;
			f_tableq(&g_lq);
			gpcfree((void **) &g_lq.xyp);
		}
		g_lq.xyp = (PGXYPAIRS)gpcalloc(64, sizeof(struct GXYPAIRSQ));
		g_lq.action = 1;
		g_lq.npts = 1;
	}

	crot = cos((double)rotation);                          /* draw ellipse */
	srot = sin((double)rotation);
	for (i = 0; i < npts; i++) {
		x0 = xr * cos(delt);
		y0 = yr * sin(delt);
		x1 = (int)(x0 * crot - y0 * srot + .5) + xc;
		y1 = (int)(y0 * crot + x0 * srot + .5) + yc;
		if (filled_ellipseq) {
			g_pq.xyp[i].x = x1;
			g_pq.xyp[i].y = y1;
		}
		else if (i == 0)
			lmoveq(x1, y1);
		else
			ldrawq(x1, y1);
		delt += del;
	}

	if (filled_ellipseq) {                                /* process panel */
		g_pq.xyp[i].x = g_pq.xyp[0].x;
		g_pq.xyp[i].y = g_pq.xyp[0].y;
		g_pq.nsides = npts + 1;
		g_pq.pattern = ellipse_patternq;
		g_pq.border = ellipse_borderq;
		panelq();
		gpcfree((void **) &g_pq.xyp);
		panproc();
	}
	else if (g_lq.npts > 1) {
		g_lq.xyp[0].x = g_lq.npts - 1;
		f_tableq(&g_lq);
		gpcfree((void **) &g_lq.xyp);
		g_lq.action = 0;
		g_lq.npts = 0;
	}
}

/*************************************************************
uradlin() draws a radial line centered on x, y at an angle of
th, between r1 and r2 -- in user units. */
void TYPE GCLASS uradlin(float x, float y, float r1, float r2, float th)
{
	int x0, y0, r10, r20;
	G_PROCESS_ID

	usertopix(x, y, 0.0f, &x0, &y0);
	r10 = (int)XSCALE_PAGE((float)(intopix(r1)));
	r20 = (int)XSCALE_PAGE((float)(intopix(r2)));
	radlinq(x0, y0, r10, r20, th);
}
/*************************************************************
uradlinp() draws a radial line centered on x, y at an angle of
th, between r1 and r2 -- in user units. */
void TYPE GCLASS uradlinp(float x, float y, float r1, float r2,
	float th, int axis)
{
	int x0, y0, r10, r20;
	G_PROCESS_ID

	usertopix(x, y, 0.0f, &x0, &y0);
	if (axis & GPC_X_AXIS) {
		r10 = (int)XSCALE_PLOT(r1);
		r20 = (int)XSCALE_PLOT(r2);
	}
	else if (axis & GPC_Y_AXIS) {
		r10 = (int)YSCALE_PLOT(r1);
		r20 = (int)YSCALE_PLOT(r2);
	}
	else
		return;
	radlinq(x0, y0, r10, r20, th);
}
/*************************************************************
radlin() draws a radial line centered at x, y, at an angle of th,
between r1 and r2 in inches (or cm), with size scaling with the x plot
dimension. */
void TYPE GCLASS radlin(float x, float y, float r1, float r2, float th)
{
	int xint, yint, r1int, r2int;
	G_PROCESS_ID

	xint = (int)XSCALE_PAGE(intopix(x)) + xbaseq;
	yint = (int)YSCALE_PAGE(intopix(y)) + ybaseq;
	r1int = (int)XSCALE_PAGE(intopix(r1));
	r2int = (int)YSCALE_PAGE(intopix(r2));
	radlinq(xint, yint, r1int, r2int, th);
}
/*************************************************************
radlinq() draws a radial line in pixel units. */
void GCLASS radlinq(int x, int y, int r1, int r2, float th)
{
	double cosn, sine;
	int x0, y0, x1, y1;

	cosn = cos((double)th);
	sine = sin((double)th);
	x0 = (int)(r1 * cosn + .5) + x;
	y0 = (int)(r1 * sine + .5) + y;
	x1 = (int)(r2 * cosn + .5) + x;
	y1 = (int)(r2 * sine + .5) + y;
	lineq(x0, y0, x1, y1);
}
