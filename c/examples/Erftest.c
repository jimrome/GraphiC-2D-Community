/*==============================================================================
 * GraphiC Classic C Suite: Erftest
 *
 * How to Compile & Run (from GraphiC repository root):
 *   make examples
 *   ./bin/Erftest
 *
 * Direct Command-Line Compilation:
 *   clang -O2 -IInclude -DGPC_PORTABLE=1 c/examples/Erftest.c lib/libgraphic.a -lm -o bin/Erftest
 *   ./bin/Erftest
 *============================================================================*/

/****************************************************************************
	(c) 1984-1993 by Scientific Endeavors Corporation.
	All rights reserved.

	This program plots a single curve on an error function grid.
****************************************************************************/

#include <graphic.h>                           /* Include all needed files */

#if TCQ                                 /* Set stack for Borland (Turbo) C */
extern unsigned _stklen = 0x3000;
#endif

float xvals[ ] = {                                          /* Data values */
	0.37f, 2.51f, 8.01f, 18.09f, 39.01f, 63.01f,
	84.49f, 94.76f, 98.31f, 99.80f, 100.f
};
float yvals[ ] = {
	51.0f, 52.0f, 53.0f, 54.0f, 55.0f, 56.0f,
	57.0f, 58.0f, 59.0f, 60.0f, 61.0f
};

/****************************************************************************
	Main program
****************************************************************************/
void GPC_MAIN(void)
{
	int npts=11;

	bgnplot(1, 'g', "erf.tkf");                  /* GraphiC initialization */
	startplot(DRK_GRAY);

	metricunits(0);                        /* Ensure scaling in inch units */
	page(9.0f, 6.884f);                      /* Size of page and plot area */
	area2d(7.6f, 5.4f);

	color(YELLOW);                          /* Yellow axis and plot titles */
	xname("Cumulative Percent");
	yname("Head Circumference");
	heading("Head Sizes of 1071 Boys");

	color(GREEN);                      /* Green axes with solid grid lines */
	grid(1);
	fgrid(1, 10);
	erfscales(10, yvals, 11);

	color(RED);                                  /* Red curve with symbols */
	tcurve(4);
	sympick(12);
	erfcurve(xvals, yvals, npts, 1);
	tcurve(0);

	endplot();                                 /* GraphiC closing routines */
	stopplot();
}
