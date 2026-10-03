/*
	(c) 1984-1995 by Scientific Endeavors Corporation.
	All rights reserved.

	Miscellaneous service and parameter setting routines.
	Version 7.10 3/5/93
*/

#include <graphic.h>

void ftype(char *xl, char *format, float value);
void ftype2(char *xl, float value, int count);
void dateitq(int xp, int yp, Uchar fontt);
/**************************************************************/
/* DRAW A BOX ABOUT THE WHOLE MAXRESX x MAXRESY AREA                  */
void TYPE GCLASS pbox(void)
{
	int dsh;
	G_PROCESS_ID

	dsh = g_lq.pattern;
	dashf(1);
	lmoveq(0, 0);
	ldrawq(acrossq, 0);
	ldrawq(acrossq, topq);
	ldrawq(0, topq);
	ldrawq(0, 0);
	dashf(dsh);
}
/**************************************************************/
/* DRAW A BOX AROUND THE DRAWING PAGE                         */
void TYPE GCLASS box(void)
{
	int dsh;
	G_PROCESS_ID

	dsh = g_lq.pattern;
	dashf(1);
	lmoveq(xbaseq, ybaseq);
	ldrawq(xbaseq + xpageq, ybaseq);
	ldrawq(xbaseq + xpageq, ybaseq + ypageq);
	ldrawq(xbaseq, ybaseq + ypageq);
	ldrawq(xbaseq, ybaseq);
	dashf(dsh);
}

/***********************************************************/
/* DEFINE THE PHYSICAL ORIGIN WITH RESPECT TO THE LOWER-LEFT
CORNER OF THE PAGE. OFFSET IN INCHES */
void TYPE GCLASS physor(float xphys, float yphys)
{
	G_PROCESS_ID

	xphysq = xphys;
	yphysq = yphys;
	psetflgq = 0;
}

/***********************************************************/
/* DEFINE PHYSICAL ORIGIN RELATIVE TO THE PREVIOUS PHYSICAL ORIGIN.
OFFSET IN INCHES. If the user has not called physor() xphysq and yphysq
need to be set equal to the inch loccation of xphysorq and yphysorq
respectivly. */

void TYPE GCLASS orel(float xorel, float yorel)
{
	G_PROCESS_ID

	if (xphysq < (float)-999.0)		/* Not previously user defined */
		xphysq = pixtoin(xphysorq);
	xphysq += xorel;
	if (yphysq < (float)-999.0)		/* Not previously user defined. */
		yphysq = pixtoin(yphysorq);
	yphysq += yorel;
	psetflgq = 0;
}

/**********************************************************/
/* SHIFT THE PAGE ORIGIN FROM LOWER LEFT CORNER. SHIFT IN INCHES */
void TYPE GCLASS pgshift(float xinch, float yinch)
{
	char out_of_bounds = 0;
	G_PROCESS_ID

	xbaseq = intopix(xinch);
	ybaseq = intopix(yinch);
	/*CHECK FOR OUT-OF-BOUNDS*/
	if (xbaseq + xpageq > acrossq) {
		out_of_bounds = 1;
		xbaseq = acrossq - xpageq;
	}
	if (ybaseq + ypageq > topq) {
		out_of_bounds = 1;
		ybaseq = topq - ypageq;
	}
	if (xbaseq < 0) {
		out_of_bounds = 1;
		xbaseq = 0;
	}
	if (ybaseq < 0) {
		out_of_bounds = 1;
		ybaseq = 0;
	}
	if (out_of_bounds)
		messageq(WARNQ, 43);	/* Shift goes out of bounds. */
	psetflgq = 0;
}

/*************************************************************
acrop sets a flag that turns off the cropping. If the flag is set
when curve is called, acropq(0) is called. */
void TYPE GCLASS acrop(char flag)
{
	G_PROCESS_ID

	crop_flgq = flag;
}
/*************************************************************
acropq() changes the cropping of a curve from page boundaries to plot
(area2d) boundaries. The default is to crop to the plot boundries. Call
acrop(1) to turn off cropping */
void GCLASS acropq(char flag)
{
	G_PROCESS_ID

	if (flag && !wplane_flgq) {
		xl_cropq = xphysorq;
		yl_cropq = yphysorq;
		xh_cropq = xl_cropq + xlenq;
		yh_cropq = yl_cropq + ylenq;
		if (xl_cropq < 0)
			xl_cropq = 0;
		if (yl_cropq < 0)
			yl_cropq = 0;
		if (xh_cropq < 0)
			xh_cropq = 0;
		if (yh_cropq < 0)
			yh_cropq = 0;
		if (xl_cropq > acrossq)
			xl_cropq = acrossq;
		if (yl_cropq > topq)
			yl_cropq = topq;
		if (xh_cropq > acrossq)
			xh_cropq = acrossq;
		if (yh_cropq > topq)
			yh_cropq = topq;
	}
	else {
		xl_cropq = 0;
		yl_cropq = 0;
		xh_cropq = acrossq;
		yh_cropq = topq;
	}
}
/*************************************************
rightax() sets the flag for drawing a y-axis on the right side
of the plot without redrawing the x-axis */
void TYPE GCLASS rightax(char yright)
{
	G_PROCESS_ID

	right_flgq = yright;
}
/******************************************************************/
/* Set the program to thicken plot curves only. This feature works
only when using a curve drawing function. */
void TYPE GCLASS tcurve(int width)
{
	G_PROCESS_ID

	tcurveq = width;
}
/******************************************************************/
/* Set the program to thicken plot curves only. Thickness in inches	(or cm)
This feature works only when using a curve drawing function. */
void TYPE GCLASS ticurve(float width)
{
	G_PROCESS_ID

	tcurveq = intopix(width);
}
/********************************************************************
Allow the user to place the date/time stamp. Location is in inches.
********************************************************************/
void TYPE GCLASS DateItXY(float xinch, float yinch, unsigned char fontt)
{
	G_PROCESS_ID

	dateitq(intopix(xinch), intopix(yinch), fontt);
}
/********************************************************************
Place the date/time stamp in the default location.
********************************************************************/
void TYPE GCLASS Dateit(unsigned char fontt)
{
	int xp, yp;
	G_PROCESS_ID

	xp = (int)XSCALE_PAGE(91) + xbaseq;
	yp = ybaseq + ypageq - (int)XSCALE_PAGE(90);
	dateitq(xp, yp, fontt);
}

#undef dateit
void TYPE GCLASS dateit(unsigned char fontt)
{
	Dateit(fontt);
}
/******************************************************************/
/* WRITE DATE AND TIME ON PLOT IN FONT OF ARGUMENT */
void dateitq(int xp, int yp, Uchar fontt)
{
	char *str1 = NULL, str2[3], str3[60];
	int lsav, ang;
	struct DATE_TIMEQ dt;
	G_PROCESS_ID

	lsav = g_lq.pattern;
	dashf(1);
	if (fontt == '\0')
		return;
	fntchg(fontt);
	get_dtq(&dt);

	switch(dt.month) {
		case 1:
			str1 = "Jan.";
			break;
		case 2:
			str1 = "Feb.";
			break;
		case 3:
			str1 = "Mar.";
			break;
		case 4:
			str1 = "Apr.";
			break;
		case 5:
			str1 = "May";
			break;
		case 6:
			str1 = "June";
			break;
		case 7:
			str1 = "July";
			break;
		case 8:
			str1 = "Aug.";
			break;
		case 9:
			str1 = "Sept.";
			break;
		case 10:
			str1 = "Oct.";
			break;
		case 11:
			str1 = "Nov.";
			break;
		case 12:
			str1 = "Dec.";
			break;
		default:
			str1 = "MONTH";
			break;
	}
	if (dt.hour >= 12) {
		dt.hour -= 12;
		strcpy(str2, "PM");
	}
	else
		strcpy(str2, "AM");
	if (dt.hour == 0)
		dt.hour = 12;

	sprintfq(str3, "%cGraphiC 7.1  %s %d, %d  %d:%02d:%02d %s",
		fontt, str1, dt.day, dt.year, dt.hour, dt.minute, dt.second, str2);
	ang = 0;
	fntpltq(xp, yp, str3, (float)XSCALE_PAGE(68), ang);

	dashf(lsav);
}
/***********************************************************/
void TYPE GCLASS plotabsolute(int set_flag)
{
	G_PROCESS_ID

	size_flagq = (char)set_flag;
}
/**************************************************************/
void TYPE GCLASS gtext(char mode)
{
	G_PROCESS_ID

	gtextq = mode;
}
/*********************************************************************/
/* This routine scales user units to pixels. For polar plots,
the first coordinate is r, the second theta */

void TYPE GCLASS usertopix(float x_in, float y_in, float z_in,
	int *x_out, int *y_out)
{
	float temp;
	G_PROCESS_ID

	if (plot_typeq & XLOG_PLOT) {
		if (x_in != (float)0.0)
			x_in = (float)(glogq((double)x_in, GPC_XAXIS));
		else
			x_in = xorigq;
	}
	if (plot_typeq & YLOG_PLOT) {
		if (y_in != (float)0.0)
			y_in = (float)(glogq((double)y_in, GPC_YAXIS));
		else
			y_in = yorigq;
	}
	if ((plot_typeq & POLAR_PLOT) || (plot_typeq & POLAR_LOG_PLOT)) {
		if (plot_typeq & POLAR_LOG_PLOT) {
			float temp1;
			temp1 = -((float)pixrq / rfact_polq);
			temp = x_in - r_polminq;
			if (temp < temp1)
				temp = temp1;
			temp = pixrq + temp * rfact_polq;
		}
		else
			temp = x_in * rfact_polq;
		*x_out = x0_polq + (int)(temp * (float)cos((double)y_in) + (float).5);
		*y_out = y0_polq + (int)(temp * (float)sin((double)y_in) + (float).5);
	}
	else if (plot_typeq & (LINEAR_PLOT |
		XLOG_PLOT | YLOG_PLOT | ERF_PLOT)) {
		*x_out = xphysorq + (int)(XSCALE_PLOT(x_in - xorigq));
		*y_out = yphysorq + (int)(YSCALE_PLOT(y_in - yorigq));
	}
	else if (plot_typeq & D3_PLOT) {
		x_in = (xorigq > xmaxq) ? (xorigq - x_in) : (x_in - xorigq);
		y_in = (yorigq > ymaxq) ? (yorigq - y_in) : (y_in - yorigq);
		z_in = (zorigq > zmaxq) ? (zorigq - z_in) : (z_in - zorigq);
	/* Convert pixels from origin to window coordinates. */
		d3convq(XSCALE_PLOT(x_in), YSCALE_PLOT(y_in),
			ZSCALE_PLOT(z_in), x_out, y_out);
	}
}
/*********************************************************************/
/* CONVERT SCREEN PIXELS TO USER UNITS */
void TYPE GCLASS pixtouser(int x_in, int y_in, float *x_out, float *y_out)
{
	float xfact, yfact, xoff, yoff;
	double rtem, theta;
	double x, y, x0, y0, rf;
	G_PROCESS_ID

	/* Polar plot. r-> x; theta->y */
	if ((plot_typeq & POLAR_PLOT) || (plot_typeq & POLAR_LOG_PLOT)) {
		x = (double)x_in;
		y = (double)y_in;
		x0 = (double)x0_polq;
		y0 = (double)y0_polq;
		rf = (double)rfact_polq;
		if (rotate_flgq & 1) {
			rtem = x;
			x = (double)MAXRESYMO - y - x0;
			y = rtem - y0;
		}
		else {
			x -= x0;
			y -= y0;
		}
		rtem = ((x * x + y * y) / (rf * rf));
		rtem = sqrt(rtem);	/* radius in user-units */
		if (fabs(rtem) > 1.e-10) {
			theta = atan2(y, x);
		}
		else {
			theta = 0.0;
		}
		theta *= (180.0 / 3.141592654);
		if (plot_typeq & POLAR_LOG_PLOT) {	/* log scale */
			rtem = rtem + (double)r_polminq - ((double)pixrq) / rf;
		}
		*x_out = (float)rtem;
		*y_out = (float)theta;
	}
	else {
		if (xlenq) {
			xfact = xfact_pltq;
			xoff = xorigq;
		}
		else {
			xfact = pix_convertq;
			xoff = (float)0.;
		}
		if (ylenq) {
			yfact = yfact_pltq;
			yoff = yorigq;
		}
		else {
			yfact = pix_convertq;
			yoff = (float)0.;
		}

		if (rotate_flgq & 1) {
			*x_out = (xoff + xfact * (float)(MAXRESYMO - y_in));
			*y_out = (yoff + yfact * x_in);
		}
		else {
			*x_out = (xoff + (x_in - xphysorq) / xfact);
			*y_out = (yoff + (y_in - yphysorq) / yfact);
		}
 		if (plot_typeq & 2)
			*x_out = (float)pow(10.0, (double)(*x_out));
 		if (plot_typeq & 4)
			*y_out = (float)pow(10.0, (double)(*y_out));
	}
}

/*********************************************************************/
/* This routine scales user units to inches. For polar plots,
the first coordinate is r, the second theta */

void TYPE GCLASS usertoinch(float x_in, float y_in, float *x_out, float *y_out)
{
	int xp, yp;
	G_PROCESS_ID

	usertopix(x_in, y_in, 0.0f, &xp, &yp);
	*x_out = pixtoin(xp);
	*y_out = pixtoin(yp);
}

/********************************************************************
    FUNCTION NAME: metric_units
    ARGUMENTS: onoff ==
                  1 enables metric units
                  0 disables metric units
    PURPOSE: Turn on or off metric units
********************************************************************/
void TYPE GCLASS metricunits(int onoff)
{
	int i;
	G_PROCESS_ID

	if (onoff) {
		if (!g_iq[dev_typeq].metric)
			pix_convertq = pix_convertq / (float)2.54;
	}
	else {
		if (g_iq[dev_typeq].metric)
			pix_convertq = pix_convertq * (float)2.54;
	}
	for (i = 0; i < 3; i++)
		g_iq[i].metric = onoff;
}
/**************************************************************/
/* This routine increments or decrements the least significant digit of
a 7 digit number. It does so by converting to the ascii representation
of the number in exponential form. The 7th significant digit is
incremented or decremented and the carry or borrow operation is
performed. Space is provided at the beginning of the ascii string for a
digit. It is needed if the addition moves the number into the next
decade. Positive sense means make more positive or less negative. */

float GCLASS fudgeitq(float z, float diff)
{
	float temf, z1, eps;
	int sign_flag;
	int sense;

	sense = (diff > z) ? 1 : 0;
	sign_flag = (z >= (float)0.) ? 1 : -1;
	if (z == 0.f)
		eps = .00000001f;
	else
		eps = z * .00000001f;

	z *= sign_flag;

	z1 = z;
	while ((float)z == (float)z1) {
		if (sense)
			temf = z1 + eps;
		else
			temf = z1 - eps;
		z = temf;
		eps *= 10.f;
	}
	return(z * sign_flag);
}

double GCLASS glogq(double n, int x_or_y)
{
	double log_base = 0.0;
	G_PROCESS_ID

	if (x_or_y & GPC_XAXIS)
		log_base = x_log_baseq;
	if (x_or_y & GPC_YAXIS)
		log_base = y_log_baseq;
	if (log_base == (double)'n')
		return(log(n));
	else if (log_base == (double)10.)
		return(log10(n));
	return(log10(n) / log10(log_base));
}

/*********************************************************************/
/* Auxiliary routine to return the color index given the z-value     */
/* We use a binary search routine.                                    */
/*********************************************************************/
int zvalueq(float *values, int dim, float tval)
{
	int j, minv, maxv;

	/* CHECK RANGE */
	if(tval >= values[dim])
		return(dim - 1);
	if(tval <= values[0])
		return(0);

	minv = 0;
	maxv = dim;
	while(maxv > minv + 1) {
		j = (maxv + minv) >> 1;
		if(tval == values[j])
			return(j);
		else if(tval > values[j])
			minv = j;
		else
			maxv = j;
	}
	return(minv);
}

#ifdef PANEL

/**************************************************************/
/* Called by user. Vectors are in inches. */
/* This routine cannot be used in projections */
void TYPE GCLASS panelinch(float *x, float *y, int nlines, int pcolor, int border)
{
	int i;
	PGXYPAIRS xyp;
	G_PROCESS_ID

/* Make sure the panel is closed */
	if (x[nlines - 1] == x[0] && y[nlines - 1] == y[0])
		nlines--;
	else {							/* Make room for the closing point. */
		x[nlines] = x[0];
		y[nlines] = y[0];
	}
	xyp = (PGXYPAIRS)gpcrealloc((void *)g_pq.xyp,
		((size_t)nlines + 1) * sizeof(struct GXYPAIRSQ));
   	if (xyp == NULL) {
		gpcfree((void **)&g_pq.xyp);
   		messageq(ERRQ, 1);		/* Out of Memory */
   		return;
	}
	g_pq.xyp = xyp;
	for (i = 0; i <= nlines; i++) {
		g_pq.xyp[i].x = intopix(x[i]);
		g_pq.xyp[i].y = intopix(y[i]);
	}
	g_pq.nsides = nlines + 1;
	g_pq.pattern = pcolor;
	g_pq.border = border;
	panelq();
	gpcfree((void **) &g_pq.xyp);
}
/**************************************************************/
/* Called by user. Vectors are in user units. */
void TYPE GCLASS panel(float *x, float *y, int nlines, int pcolor, int border)
{
	int i;
	int wpsav;
	PGXYPAIRS xyp;
	G_PROCESS_ID

	if (!crop_flgq)
		acropq(1);
	else
		acropq(0);
/* Make sure the panel is closed */
	if (x[nlines - 1] == x[0] && y[nlines - 1] == y[0])
		nlines--;
	else {							/* Make room for the closing point. */
		x[nlines] = x[0];
		y[nlines] = y[0];
	}
	xyp = (PGXYPAIRS)gpcrealloc(g_pq.xyp,
		((size_t)nlines + 1) * sizeof(struct GXYPAIRSQ));
	if (xyp == NULL) {
		gpcfree((void **)&g_pq.xyp);
		messageq(ERRQ, 1);		/* Out of Memory */
		return;
	}
	g_pq.xyp = xyp;
	if (wplane_flgq) {
		panfilq = 1;
		curve(x, y, nlines + 1, 0);		/* Call to curve to load ?panq arrays
									*  with points scaled for 3-D projection */
		/* TURN OFF WPLANEQ TO GET CORRECT drawbq() */
		wpsav = wplane_flgq;
		wplane_flgq = 0;
		g_pq.pattern = pcolor;
		g_pq.border = border;
		panelq();
		gpcfree((void **) &g_pq.xyp);
		panfilq = 0;
		wplane_flgq = wpsav;
	}
	else {
		for (i = 0; i <= nlines; i++)
			usertopix(x[i], y[i], 0.0f,
				(int *)&g_pq.xyp[i].x, (int *)&g_pq.xyp[i].y);
		g_pq.nsides = nlines + 1;
		g_pq.pattern = pcolor;
		g_pq.border = border;
		panelq();
		gpcfree((void **) &g_pq.xyp);
	}
	acrop(0);
}
/****************************************************************************/
/* This routine puts the first and second curve vector arrays together
and calls panel to fill between curves. */
void TYPE GCLASS curvfill(float *x, float *y, int npts, float *x1,
	float *y1, int npts1, int pcolor, int border)
{
	int i, n;
	int wpsav;
	float *xpts;
	float *ypts;
	G_PROCESS_ID

	wpsav = wplane_flgq;
	n = npts + npts1;
	if (n <= 2)
		return;
	xpts = NULL;
	ypts = NULL;
	xpts = (float *)gpcalloc((size_t)(n + 1), sizeof(float));
	ypts = (float *)gpcalloc((size_t)(n + 1), sizeof(float));
	if ((ypts == (void *)NULL) || (xpts == (void *)NULL)) {
		messageq(ERRQ, 32);		/* Out of Memory */
		curve(x, y, npts, 0);
		curve(x1, y1, npts1, 0);
		return;
	}
	for (i = 0; i < npts; i++) {
		xpts[i] = x[i];
		ypts[i] = y[i];
	}
	for (i = 0; i < npts1; i++) {
		xpts[npts + i] = x1[npts1 - 1 - i];
		ypts[npts + i] = y1[npts1 - 1 - i];
	}
	g_pq.pattern = pcolor;
	g_pq.border = border;
	panel(xpts, ypts, n, pcolor, border);
	wplane_flgq = 0;
	panproc();
	panfilq = 0;
	wplane_flgq = wpsav;
	gpcfree((void **) &xpts);
	gpcfree((void **) &ypts);
}

#else

/**********************************************************************
Dummy routines to provide curvfill() and panel() if PANEL is not defined
in USER.H. Some operations are performed inside these dummy routines to
prevent warnings during compiling.
*/
void TYPE GCLASS panelinch(float *x, float *y, int nlines, int pcolor, int border)
{
	if (x[nlines - 1] == x[0] && y[nlines - 1] == y[0])
		nlines--;
	pcolor = border;
	border = pcolor;
	panelq();
}
void TYPE GCLASS panel(float *x, float *y, int nlines, int pcolor, int border)
{
	if (x[nlines - 1] == x[0] && y[nlines - 1] == y[0])
		nlines--;
	pcolor = border;
	border = pcolor;
	panelq();
}
void TYPE GCLASS curvfill(float *x, float *y, int npts, float *x1,
float *y1, int npts1,
	int pcolor, int border)
{
	int pts;
	float xa, ya;

	xa = *x + *x1;
	ya = *y + *y1;
	pts = npts + npts1;
	pts++;
	ya = xa;
	xa = ya;
	pcolor = border;
	border = pcolor;
	panelq();
}
#endif
#if GPC_WIN
/*************************************************************
Modified sprintf. It formats scientific notation compactly. */
void kprintfq(char *xl, char *format, float value)
{
	int power = 0, temi, ch, decfig, i, j = 0, flag = 0;
	int count;
	char buf[10];
	float temf, decpart;
    
    /* Check for valid format strings */
	if (format == NULL) {
		*xl = '\0';
		return;
	}
	if (*format == '\0') {
		*xl = '\0';
		return;
	}

	/* Check for valid float type */
	if (strchr(format, 'f') || strchr(format, 'F')) {
		ftype(xl, format, value);
		return;
	}

	/* Check for valid float type */
	if (strchr(format, 'g') || strchr(format, 'G')) {
		ftype(xl, format, value);
		return;
	}
	
	/* Check for valid float type */
	if (strchr(format, 'e') || strchr(format, 'E')) {
    	i = 0;
    	while ((ch = (*(format + i++))) != '.');
    	while ((ch = (*(format + i++))) != 'e')
    		*(buf + j++) = (char)ch; /* Recover precision. */
    	*(buf + j) = 0;
    	decfig = gabs(atoi(buf)) + 1;
    	if (value < 0) {
    		flag = 1;
    		value = ( - value);
    	}
    	if (value != 0) {
    		if (value < 1)
    			power = (int)log10(value) - 1; /* Move decimal place this much. */
    		else
    			power = (int)log10(value);
    	}
    	temf = (float)pow(0.1, (double)power) * value; /*Normalized value */
    	temi = (int)(temf + .00001f);	/* Integer part. */
    	decpart = (float)(temf - temi) + (float)(5 / pow(10.0, (double)decfig));
    															/* Decimal part.*/
    	/* Adding the (5/pow()) will round the correct number of decimal places */
    	for (i = 1; i < decfig; i++) {
    		ch = (int)(decpart * (float)10.);
    		decpart = (decpart * 10) - ch;
    		buf[i - 1] = (char)(ch + 48);		/* put characters in a string */
    	}
    	buf[i - 1] = 'E';
    	buf[i] = 0;
    	if (decfig > 1 && temi != 0) {
    		if (flag)
    			sprintfq(xl, "-%d.%s%d", temi, buf, power);
    		else
    			sprintfq(xl, "%d.%s%d", temi, buf, power);
    	}
    	else if (temi == 0) {
    		count = 0;
    		ftype2(xl, value, count);
    	}
    		/*sprintfq(xl, "%-1.0f", value); */
    	else if (flag)
    		sprintfq(xl, "-%dE%d", temi, power);
    	else
    		sprintfq(xl, "%dE%d", temi, power);
	}
	else {
        /* Other than float or exponential, so drop out */
        sprintfq(xl, format, value); /* Normal action */
        return;
    }
}

void ftype(char *xl, char *format, float value)
{
    char *ip;
    char cp[10];
    int count;

    /* parse format for fcvt call */
    ip = strchr(format, '.');
    if (ip[2] == 'f') {
    	strncpy(cp, ip + 1, 1);
    	cp[1] = '\0';
    }
    else {
    	strncpy(cp, ip + 1, 2);    
    	cp[2] = '\0';
    }
    count = atoi(cp);
    ftype2(xl, value, count);
}

void ftype2(char *xl, float value, int count)
{
    char *xlp;
    char *fbuff; 
    int dec;
    int i;
    int sign;

	fbuff = fcvt(value, count, &dec, &sign);
	xlp = xl;
	
	if (sign) {
		*xl = '-';
		xlp++;
	}
	if (dec < 1) {	/* if leading decimal point */
		if (count > 0) {
			xlp[0] = '\0';
			strcat(xlp, "0.");
			xlp += 2;
			if (dec < 0) {
				dec *= -1;
				for (i = 0; i < dec; i++) {
					*xlp = '0';
					xlp++;
				}
			}
			strncat(xlp, fbuff, strlen(fbuff));
		}
		else if (fbuff[0] == '\0')
			strcpy(xlp, "0");		
	}
	else {
		strncpy(xlp, fbuff, dec);
		xlp += dec;
		if (count > 0) {
			*xlp = '.';
			xlp++;
			strncpy(xlp, fbuff + dec, strlen(fbuff) - dec);
			strcpy(xlp + (strlen(fbuff) - dec) + 1, "");
		}		
	}
}
#else
/*************************************************************
Modified sprintf. It formats scientific notation compactly. */
void kprintfq(char *xl, char *format, float value)
{
	sprintfq(xl, format, value);
}
#endif

/********************************************************************
This routine returns the inch value for the given number of pixels in
8192 space. The variable pix_convertq = pixels / inch.
********************************************************************/
float TYPE GCLASS pixtoin(int pixels)
{
    G_PROCESS_ID
	return((float)pixels / pix_convertq);
}

/********************************************************************
This routine returns the number of pixels in 8192 space for the given
inch value. The variable pix_convertq = pixels / inch.
********************************************************************/
int TYPE GCLASS intopix(float inches)
{
    G_PROCESS_ID
	return((int)(inches * pix_convertq));
}
