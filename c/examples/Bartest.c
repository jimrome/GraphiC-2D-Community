/*==============================================================================
 * GraphiC Classic C Suite: Bartest
 *
 * How to Compile & Run (from GraphiC repository root):
 *   make examples
 *   ./bin/Bartest
 *
 * Direct Command-Line Compilation:
 *   clang -O2 -IInclude -DGPC_PORTABLE=1 c/examples/Bartest.c lib/libgraphic.a -lm -o bin/Bartest
 *   ./bin/Bartest
 *============================================================================*/

/****************************************************************************
	(c) 1984-1993 by Scientific Endeavors Corporation.
	All rights reserved.

	This program plots a bar chart with a legend.
****************************************************************************/
#include <graphic.h>                           /* Include all needed files */

#if TCQ                                 /* Set stack for Borland (Turbo) C */
extern unsigned _stklen = 0x3000;
#endif

static int tarray[24] = {                           /* Shading for 24 bars */
	1, 93, 1, 93, 1, 93, 1, 93, 1, 93, 1, 93,
	1, 93, 1, 93, 1, 93, 1, 93, 1, 93, 1, 93
};
static int carray[24] = {       /* Color array for the pattern filled bars */
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};
static char *xstring[14] = {                       /* Strings for x labels */
	"", "\310Jan.", "\310Feb.", "\310Mar.", "\310Apr.",
	"\310May", "\310June", "\310July", "\310Aug.",
	"\310Sep.", "\310Oct.", "\310Nov.", "\310Dec.", ""
};
static float sales[24] = {                 /* y vector contains sales data */
	6.931f, 10.170f, 5.370f, 13.105f, 4.590f, 13.576f, 3.465f, 17.204f,
	5.655f, 13.401f, 4.26f, 11.789f, 7.702f, 10.157f, 5.28f, 7.66f,
	8.155f, 12.897f, 3.735f, 13.318f, 3.925f, 10.785f, 4.085f, 12.612f
};
/****************************************************************************
	Main program
****************************************************************************/
void GPC_MAIN(void)
{	float x[24];
	int i, j = 0, nxdiv, npts;

	nxdiv=13;                             /* Desired # of x-axis divisions */
                             /* Should be one more than the number of bars */
	npts=24;      /* Number of bars to be drawn. Stacked bar counts as two */
	for(i = 0; i < npts; i++){                            /* Fill x vector */
		j++;
		x[i++] = (float)(j);
		x[i] = (float)(j);
	}

	bgnplot(1, 'g', "bar.tkf");                  /* GraphiC initialization */
	startplot(BRT_WHITE);
	metricunits(0);                                 /* Plots are in inches */
	font(3, "Swiss.fnt", '\310', "Swissbld.fnt", '\311',  /* Select outline fonts */
		"News.fnt", '\312');
	page(9.0f, 6.884f);                      /* Size of page and plot area */
	area2d(7.6f, 5.3f);
	color(GREEN);                                       /* Legend is green */
	legpos(2, 6.0f, 4.5f, 1);            /* Two entries at 6.0, 4.5 inches */
	legend(2, "\312gadgets", 1, .15f, 1);
	legend(2, "\312widgets", 93, .15f, 1);
	box();                                   /* Draw a box around the page */
	color(GREEN);                    /* Plot and axis titles will be green */
	heading("\311Sales");
	xname("\312""1988");
	yname("\312Thousands");

                                /* Manually scaled axes with string labels */
	xlab(1, xstring);
	graf("%1.2f", 0.f, 1.f, 13.f, "%1.1f", 0.f, 3.f, 18.f);
	xlab(0, xstring);
	bar(nxdiv, x, sales, npts, .6f, 2, carray, tarray);       /* Bar chart */
	color(BLACK);                           /* Put date-time stamp on plot */
	dateit((Uchar)'\310');
	endplot();                                    /* Plot closing routines */
	stopplot();
}
