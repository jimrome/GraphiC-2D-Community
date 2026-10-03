/*==============================================================================
 * GraphiC Classic C Suite: Sample
 *
 * How to Compile & Run (from GraphiC repository root):
 *   make examples
 *   ./bin/Sample
 *
 * Direct Command-Line Compilation:
 *   clang -O2 -IInclude -DGPC_PORTABLE=1 c/examples/Sample.c lib/libgraphic.a -lm -o bin/Sample
 *   ./bin/Sample
 *============================================================================*/

/****************************************************************************
	(c) 1984-1993 by Scientific Endeavors Corporation.
	All rights reserved.

	This program plots a single curve on a set of 2-D linear axes. Except for
	color(), sympick(), and grid(), this simple example program uses only
	the minimum calls needed to produce a GraphiC plot.
****************************************************************************/

#include <graphic.h>                           /* Include all needed files */

#if TCQ                                 /* Set stack for Borland (Turbo) C */
extern unsigned _stklen = 0x3000;
#endif

/****************************************************************************
	Main program
****************************************************************************/
void GPC_MAIN(void)
{
	float *x, *y;
	int i, nxdiv, nydiv, npts;
	char TKFName[] = "sample.tkf";

	npts = 301;                      /* The # of points in x and y vectors */

	x = (float *)gpcalloc(npts, sizeof(float));
	y = (float *)gpcalloc(npts, sizeof(float));
	if(x == (float *)NULL || y == (float *)NULL) {
		GPC_PUTS("SAMPLE: Could not allocate data arrays");
		goto EndOfApp;
	}
	
	bgnplot(1, 'g', TKFName);     /* Parameters: 1 - draw plot on screen
                                                    'g' - graphics mode
                                           "sample.tkf" - .TKF file name   */
	startplot(WHITE);
	metricunits(0);                        /* Ensure scaling in inch units */
	font(1, "Swiss.fnt", '\310');              /* Loads outline Swiss font */
	page(9.0f, 6.884f);                              /* Sets the page size */
                          /* This is the same aspect ratio as 4095 by 3132 */
	area2d(7.6f, 5.5f);                       /* Sets the area of the plot */
	for (i = 0; i < npts; i++) {                          /* Generate data */
		y[i] = .3f * i;
		x[i] = (y[i] * y[i]) / 2.f;
	}
	color(BLACK);                  /* Axis names and heading will be black */
	xname("\310X-Axis");
	yname("Y-Axis");
	heading("SAMPLE PLOT");
	grid(9);                /* Draws grid through tick marks, 9 - fine dot */
	nxdiv = 5;                   /* Sets the desired # of x-axis divisions */
	nydiv = 6;                   /* Sets the desired # of y-axis divisions */
	color(GREEN);                                 /* Green axes and labels */
	scales(nxdiv, nydiv, x, y, npts);         /* Draws and scales the axes */
	color(RED);                                               /* Red curve */
	sympick(12);                                  /* Filled circle symbols */
	curve(x, y, npts, 10);
//	curvefollow(0, 0, x, y, npts);
	endplot();         /* Finishes plot and waits for instructions to exit or
                         draw next plot. Resets file pointers and defaults */
	stopplot();          /* Close files, return to text mode, exit program */
EndOfApp:
	gpcfree((void **)&x);
	gpcfree((void **)&y);
}
