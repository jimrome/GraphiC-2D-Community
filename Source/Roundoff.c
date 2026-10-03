/*	(c) 1993 by Scientific Endeavors Corp.
	All rights reserved.
This module contains the routines for axes label rounding.
Version 7.0
*/
/*************************************************************/
#include <graphic.h>

/**************************************************************/
/* FIND THE EXTREMA OF X OR Y */
void TYPE GCLASS minmax(
	float *minq,	/* pointer to the (returned) minimum value */
	float *maxq,	/* pointer to the (returned) maximum value */
	float *xy,		/* pointer to the array for which extrema are desired */
	int npts		/* dimension of the array */
)
{
	float *end = xy + npts;

	while (xy < end) {			/* Initialize min and max to a valid float */
		if (!IsNotRealFloat(*xy)) {
			*maxq = *minq = *(xy++);
			break;
		}
		xy++;
	}
	if (xy >= end) {
		*minq = (float)0.0;
		*maxq = (float)1.0;
		messageq(WARNQ, 47);	/* No valid floats */
		return;
	}
	while (xy < end) {			/* Test valid floats for min and max */
		if (!IsNotRealFloat(*xy)) {
			if (*xy > *maxq)
				*maxq = *xy;
			if (*xy < *minq)
				*minq = *xy;
		}
		xy++;
	}
}
#if MSQ
#pragma optimize("", off)
#endif
/*************************************************************
This routine selects the label intervals and their format. It uses
three criteria:
	1. Enough sig. fig's are chosen to make the steps unique;
	2. Enough sig. fig's are chosen so that ticks fall within
	   a half-(floating-point)step of the curve maxima and minima;
	3. The origin is on a tick.
*/
void GCLASS roundq(
	int nstep,			/* The number of steps requested */
	float *orip,		/* address of the minimum value variable */
	float *step,		/* address of the step size variable */
	float *maxp,		/* address of the maximum value variable */
	char *format		/* pointer to the format string array */
)
{
	double maxq, minq, stepq, maxd, mind, ratio;
	long temp;
	double gtr, les, ref, tempf, lesold, eps, eps1, hstep;
	int nabove, pwr10 = 0, c, flag = 0, flag2 = 0, cflg = 0;
	G_PROCESS_ID

	maxq = maxd = (double) *maxp;
	minq = mind = (double) *orip;
	eps = (maxq - minq) * .000001;
	if ((maxq + eps >= minq) && (maxq - eps <= minq)) {
		maxq = maxd = (double)(*maxp + (float)1.);
		minq = mind = (double)(*orip - (float)1.);
		messageq(WARNQ, 16);	/* Max cannot equal min */
	}
	if (maxq < minq)
		messageq(ERRQ, 34);	/* Max must be greater that min */

		/* GET RELATIVE ABSOLUTE VALUES */
	tempf = gabs(maxq);
	ref = gabs(minq);
	eps = (maxq - minq) * .000001;
	if ((tempf + eps) > ref) {
		gtr = tempf;
		les = ref;
		flag = 1;					/* Higher above the axis */
	}
	else {
		gtr = ref;
		les = tempf;
	}
	ratio = (maxd - mind) / gtr;
	if (ratio < 0.0004)
		messageq(ERRQ, 33);		/* Range is too small relative to size */
	eps1 = 0.0001 * (maxq - minq) / gtr;
	gtr = roundoffq(gtr, eps1);
	les = roundoffq(les, eps1);
	lesold = les;

		/* DO CASE OF AXIS-STRADDLING */

	if (maxq > 0. && minq < 0.) {
		nabove = (int)((nstep * gtr) / (maxq - minq) + .4);
		stepq = gtr / nabove;
		flag2 = 1;					/* Straddles the axis */
	}

		/* ALL ABOVE OR BELOW THE AXIS */

	else {
		stepq = (gtr - les) / nstep;
		if (les <= stepq - eps) {
			les = 0.;
			stepq = gtr / nstep;
		}
	}

		/* SCALE INTERVAL TO PUT 1 <= INTERVAL<10 */

	c = getexpq(&stepq, 10.);
					/* c+1 is the # of places to the left of the decimal. */
	if (stepq > 8.0) {pwr10 = 1; stepq = 1.; c++; cflg = 1; }
	else if (stepq > 6.0 + eps1) {pwr10 = 1; stepq = 8.; }
	else if (stepq > 5.0 + eps1) {pwr10 = 1;  stepq = 6.; }
	else if (stepq > 4.0 + eps1) {pwr10 = 1;  stepq = 5.; }
	else if (stepq > 3.0 + eps1) {pwr10 = 1;  stepq = 4.; }
	else if (stepq > 2.5 + eps1) {pwr10 = 1;  stepq = 3.; }
	else if (stepq > 2.2 + eps1) {pwr10 = 2;  stepq = 2.5; }
	else if (stepq > 2.0 + eps1) {pwr10 = 2;  stepq = 2.2; }
	else if (stepq > 1.8 + eps1) {pwr10 = 1;  stepq = 2.; }
	else if (stepq > 1.5 + eps1) {pwr10 = 2;  stepq = 1.8; }
	else if (stepq > 1.3 + eps1) {pwr10 = 2;  stepq = 1.5; }
	else if (stepq > 1.2 + eps1) {pwr10 = 2;  stepq = 1.3; }
	else if (stepq > 1.0 + eps1) {pwr10 = 2;  stepq = 1.2; }
	else if (stepq <= 1.0 + eps1)      {pwr10 = 1;  stepq = 1.0; }
	stepq *= pow(10., (double)c);		/* scale stepq. */
	if (flag2) {							/* If the axis is straddled */
		hstep = stepq / 2.;
		tempf = lesold / hstep; /* Scaled number of 1/2 steps + remainder */
		temp = (int)(tempf + .49999);
									/* Scaled  rounded number of 1/2 steps */
		les = temp * hstep;			/* Tenative les */
		while (les < (lesold - eps)) {	/* Straddle axis and need fix. */
			les += hstep;				/* shift down 1/2 step */
			if (crossq.flg & AXES_ARE_CROSSED)
				break;
		}
	}
	else if (les > (lesold + eps))
		les -= stepq;

			/* FINISH FOR ZERO INCLUDED */

	tempf = les;
	nstep = 0;
	while (tempf < gtr && stepq != 0.) {
		tempf += stepq;
		nstep++;
	}
	gtr = roundoffq(tempf, eps1);
	if (flag2) {
		if (flag) {
			maxd = gtr;
			mind = ( -les);
		}
		else {
			mind = ( -gtr);
			maxd = les;
		}
	}

		/* FOR ZERO NOT INCLUDED */

	else {
		if (flag) {
			maxd = gtr;
			mind = les;
		}
		else {
			maxd = ( -les);
			mind = ( -gtr);
		}
	}
	if (maxd < ((double)maxq - eps)) {
		nstep++;
		maxd += stepq;
	}
	*maxp = (float)maxd;
	if ((double) * maxp < maxd)
		*maxp *= (float)1.00002;
	*orip = (float)mind;
	if ((double) * orip < maxd)
		*orip = (float)roundoffq((double)mind * 1.00002, eps1);
	*step = (float)roundoffq(stepq, eps1);
	if ((double) * step < stepq)
		*step *= (float)1.00002;

		/* GET THE FORMAT. */

	if (c < 5 && c > - 5) {
		if (c < 0 && cflg)
			c--;
		if (c == 0)
			sprintfq(format, "%%-1.%df", pwr10);
		else if (c < pwr10)
			sprintfq(format, "%%-1.%df", pwr10 - c - 1);
		else
			strcpy(format, "%-1.0f");
	}
	else
		sprintfq(format, "%%-1.%de", pwr10);
}
#if MSQ
#pragma optimize("", on)
#endif
/*************************************************************/
/* returns a rounded value of x based on the exponent determined by
roundq() (above).
*/
double GCLASS roundoffq(
	double x,
	double eps
)
{
	double tempf;
	int c, i, sign = 1;

	if (x == 0.)
		return(0.);
	if (x < 0.) {
		sign = -1;
		x = -x;
	}
	c = getexpq(&x, 10.);		/* get the number of decimal places */
	for (i = 0; i < 4; i++) {			/* Round to 4 sig. digits or less */
		tempf = x - floor(x + eps);		/* Get fractional part */
		if (-eps < tempf && tempf < eps)
			break;						/* If within +/-eps then done */
		x *= 10.;						/* Else add next sig digit */
		c--;							/* Adjust exponent */
	}
	x = sign * floor(x + eps) * pow(10., (double)c);
	return(x);							/* Return the rounded number */
}
/*************************************************************/
/* NORMALIZE TO FIRST DECADE AND RETURN EXPONENT. */
int GCLASS getexpq(double *nptr, double nbase)
{
	double num;
	int expo = 0, sign = 1;

	num = *nptr;			   		/* Put number into local variable */
	if (num == 0.)			   		/* Do nothing if zero */
		return(0);
	if (num < 0.) {			   		/* Handle negative numbers */
		num = -num;
		sign = -1;
	}
	while (num >= nbase) {			/* Make number less than ten */
		num /= nbase;
		expo++;
	}
	while (num < 1.) {				/* Make number at least one */
		num *= nbase;
		expo--;
	}
	*nptr = sign * num;				/* Pass back the normalized number */
	return(expo);					/* Return the exponent */
}

