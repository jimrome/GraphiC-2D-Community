/*	Copyright 1993 by Scientific Endeavors Corporation.
	All rights reserved.

	This file contains statistical functions that may be called by
	GraphiC users.
	Version 7.0
*/

#include <graphic.h>

int G_CDECL _Optlink compfq(const void *arg1, const void *arg2);

/*********************************************************************/
/* Return the average of x */
float TYPE GCLASS mean(float *x, int xdim)
{
	int i;
	float sum = 0.0f;

	if (xdim == 0)
		return(*x);
	for (i = 0; i < xdim; i++) {
		sum += *(x + i);
	}
	return(sum/(float)xdim);
}
/*********************************************************************/
/* Calculate the median of a vector */
float TYPE GCLASS median(float *x, int xdim)
{
	qsort(&x, (size_t)xdim, sizeof(float), compfq);	/* Sort array */
	if (xdim % 2) {	/* Odd number of elements */
		return(*(x + (xdim/2)));	/* array starts at 0!! */
	}
	else {
		return(0.5f * (*(x + (xdim/2)) + *(x + (xdim/2) - 1)));
	}
}
/*********************************************************************/
/* Used for qsort comparisons of floats */
int G_CDECL compfq(const void *arg1, const void *arg2)
{
	float *one, *two;

	one = (float *)arg1;
	two = (float *)arg2;
	if (*one < *two)
		return(-1);
	if (*one > *two)
		return(1);
	return(0);
}

/*********************************************************************/
/* Calculate the variance of x: s^2 = SUM((x - mean(x))^2)/(n-1)
This incorporates Bessel's correction which replaces n by n-1 in the
denominator.
*/
float TYPE GCLASS variance(float *x, int xdim)
{
	float m, sum = 0.0f, temp;
	int j;

	m = mean(x, xdim);
	for (j = 0; j < xdim; j++) {
		temp = *(x + j) - m;
		sum += temp * temp;
	}
	xdim--;
	if (xdim == 0)
		xdim++;
	return(sum/(float)xdim);
}

/*********************************************************************/
/* Calculate the standard deviation of x, s = sqrt(variance) */
float TYPE GCLASS StdDev(float *x, int xdim)
{
	return((float)(sqrt( (double)variance(x, xdim) ) ));
}

/*********************************************************************/
/*	Make a box plot of a vector of y's at a given x.
	outlier = 1 -> plot lowest 10% and highest 10% of data points
	          0 -> don't plot them
	symnum = symbol number to use for these points
	width = width of the box in inches
	When the percentages don't hit an exact data point, we linearly
	interpolate the y position between the two adjacent points.
**********************************************************************/
void TYPE GCLASS BoxPlot(float x, float *y, int ydim, int outlier, int symnum,
	float width)
{
	float ten, twenty_five, fifty, seventy_five, ninety, delta;
	int centerx;
	int left, right, top1, top2, bottom1, bottom2, halfw;
	int j, k, m;
	int dash_sav, thick_sav;
	char crop_sav;
	G_PROCESS_ID

	if (ydim < 4)                                   /* Too few data points */
		return;

	dash_sav = g_lq.pattern;                       /* Save line attributes */
	thick_sav = g_lq.thick;
	dashf(1);

	crop_sav = crop_flgq;                     /* Set cropping to axis area */
	if (!crop_flgq && !wplane_flgq) {
		crop_flgq = 1;
		acropq(1);
	}
	else
		acropq(0);

	qsort(y, (size_t)ydim, sizeof(float), compfq);           /* Sort array */

                                                /* First plot the outliers */
	k = ydim / 10;
	if (outlier) {
		for (j = 0, m = ydim - 1; j < k; j++, m--) {
			symbol(x, y[j], symnum);
			symbol(x, y[m], symnum);
		}
		if (ydim % 10) {
			symbol(x, y[j], symnum);
			symbol(x, y[m], symnum);
		}
	}

                                /* Calculate the 10th and 90th percentiles */
	delta = .1f * (float)(ydim % 10);
	ten = y[k] + (y[k + 1] - y[k]) * delta;
	k = ydim - 1 - k;
	ninety = y[k] - (y[k] - y[k - 1]) * delta;

                                      /* And the 25th and 75th percentiles */
	k = ydim / 4;
	delta = .25f * (float)(ydim % 4);
	twenty_five = y[k] + (y[k + 1] - y[k]) * delta;
	k = ydim - 1 - k;
	seventy_five = y[k] - (y[k] - y[k - 1]) * delta;

                                            /* And now the 50th percentile */
	k = ydim / 2;
	delta = .5f * (float)(ydim % 2);
	fifty = y[k] + (y[k + 1] - y[k]) * delta;

                                                      /* Draw the whiskers */
	usertopix(x, ninety, 0.0f, &centerx, &top1);
	usertopix(x, seventy_five, 0.0f, &centerx, &top2);
	usertopix(x, twenty_five, 0.0f, &centerx, &bottom1);
	usertopix(x, ten, 0.0f, &centerx, &bottom2);
	halfw = intopix(width) >> 2;
	left = centerx - halfw;
	right = centerx + halfw;
	lineq(left, top1, right, top1);
	lineq(centerx, top1, centerx, top2);
	lineq(centerx, bottom1, centerx, bottom2);
	lineq(left, bottom2, right, bottom2);

                                                           /* Draw the box */
	halfw = intopix(width) >> 1;
	left = centerx - halfw;
	right = centerx + halfw;
	lmoveq(left, bottom1);
	ldrawq(left, top2);
	ldrawq(right, top2);
	ldrawq(right, bottom1);
	ldrawq(left, bottom1);

	                                                    /* Draw the median */
	tline(5);
	usertopix(x, fifty, 0.0f, &centerx, &top2);
	lineq(left, top2, right, top2);

	dashf(dash_sav);                            /* Restore line attributes */
	tline(thick_sav);

	crop_flgq = crop_sav;                       /* Restore cropping region */
	acropq(0);
}
/*********************************************************************/
/*
Calculate the cross-correlation function for two functions. If they
are the same, the auto-correlation function will be calculated.
Assumptions:
	Each function is given at equally-spaced ordinates 0,,t,,dim.
	The functions are assumed to be periodic with period dim.
	x[0] == x[dim]; y[0] == y[dim];
The cross-correlation function is defined as


             SUM  (x  - <x>)(y - <y>)
              t     t+k       t
rho (k) = ___________________________________
  xy                         2            2
          SQRT { SUM(x - <x>) SUM(y - <y>) }
                      t            t

<x> = average of x. If avg == 0, the average will not be subtracted
from the vector(s).
The output vector, cvec, will range from 0 to k.
Be sure to save the returned address and free the allocated storage
when you are finished.
The correlation coefficient is cvec[0], and ranges from -1.0 to 1.0.
1.0 means the functions are fully correlated.
-1.0 means they are anticorrelated.
The function returns 1 if one vector was all zeros, or if memory allocation
failed. It returns 0 upon succcess.
*/
int TYPE GCLASS crosscorrelate(float *x, float *y, float *cvec, int dim, int avg)
{
	float xav = 0.0f, yav = 0.0f, sumx, sumy;
	int c_cross = 1, i, k;
	float *tptr;
	float temp;
	float *tx;
	float *ty;

	if (dim <= 2)
		return(1);
	if (x == y)
		c_cross = 0;

	/* ALLOCATE MEMORY */
	tx = (float *)gpcalloc((size_t)(2 * dim), sizeof(float));
	if (c_cross)
		ty = (float *)gpcalloc((size_t)dim, sizeof(float));
	if (tx == (float *)NULL || ty == (float *)NULL) {
		gpcfree((void **) &tx);
		gpcfree((void **) &ty);
		messageq(ERRQ, 57);
		return(1);
	}
	if (avg) {
		/* FIRST FIND THE AVERAGE OF EACH FUNCTION */
		for (i = 0; i <= c_cross; i++) {
			temp = (float)0.0;
			tptr = (i == 0) ? x : y;
			for (k = 0; k < dim; k++) {
				temp += *(tptr + k);
			}
			temp /= (float)dim;
			if (i == 0)
				xav = temp;
			else
				yav = temp;
		}
	}
	else {
		xav = yav = (float)0.0;
	}
	/* REPLICATE x IN THE tx ARRAY */
	for (i = 0; i < 2; i++ ) {
		for (k = 0; k < dim; k++) {
			tx[k + dim * i] = x[k] - xav;
		}
	}

	/* REPLICATE y ARRAY */
	if (c_cross) {
		for (k = 0; k < dim; k++) {
			ty[k] = y[k] - yav;
		}
	}

	/* CALCULATE NORMALIZATIONS */
	sumx = sumy = (float)0.0;
	for (k = 0; k < dim; k++) {
		sumx += tx[k] * tx[k];
	}
	if (c_cross) {
		for (k = 0; k < dim; k++) {
			sumy += ty[k] * ty[k];
		}
	}
	if (c_cross)
		sumx = (float)sqrt((double)(sumx * sumy));
	/* Now sumx has the normalization for both cases */

	/* CALCULATE THE CORRELATION FUNCTION */
	for (k = 0; k < dim; k++) {
		temp = (float)0.0;
		tptr = (i == 0) ? tx : ty;
		for (i = 0; i < dim; i++) {
			temp += tx[k + i] * tptr[i];
		}
		if (sumx == 0.0f) {
			gpcfree((void **) &tx);
			gpcfree((void **) &ty);
			return(1);
		}
		cvec[k] = temp / sumx;
	}

	gpcfree((void **) &tx);
	gpcfree((void **) &ty);
	return(0);
}
