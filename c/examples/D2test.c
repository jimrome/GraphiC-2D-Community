/*==============================================================================
 * GraphiC Classic C Suite: D2test
 *
 * How to Compile & Run (from GraphiC repository root):
 *   make examples
 *   ./bin/D2test
 *
 * Direct Command-Line Compilation:
 *   clang -O2 -IInclude -DGPC_PORTABLE=1 c/examples/D2test.c lib/libgraphic.a -lm -o bin/D2test
 *   ./bin/D2test
 *============================================================================*/

/****************************************************************************
	(c) 1984-1993 by Scientific Endeavors Corporation.
	All rights reserved.

	This program plots a single curve on a set of 2-D linear axes.
	Except for color() and grid() this simple example program uses only
	the minimum calls needed to produce a GraphiC plot.
****************************************************************************/

#include <graphic.h>                           /* Include all needed files */
#define NPTS 301

#if TCQ                                 /* Set stack for Borland (Turbo) C */
extern unsigned _stklen = 0x3000;
#endif

static int carray[ ] = {                  /* Colors to be used on the bars */
	1, 2, 3, 4, 5, 6, 2, 3, 4, 5, 6, 1,
	3, 4, 5, 6, 1, 2, 4, 5, 6, 1, 2, 3
};
static int tarray[ ] = {                    /* Shading styles for the bars */
	1, 2, 3, 7, 8, 11, 12, 13, 1, 2, 3, 7,
	7, 8, 11, 12, 13, 1, 2, 3, 7, 8, 11, 12
};
static char *xstring[ ] = {                   /* Strings for x-axis labels */
	"", "\310Apr.", "\310May", "\310June", "\310July",
	"\310Aug.", "\310Sep.", "\310Oct.", "\310Nov.",
	"\310Dec.", "\310Jan.", "\310Feb.", "\310Mar.", ""
};
static float sales[ ] = {                        /* y values for bar chart */
	.4f, 4.17f, .566f, 4.63f, .505f, 5.55f, .881f, 5.27f, .666f, 5.89f, 1.414f, 6.48f,
	1.4f, 6.95f, 1.05f, 8.13f, 2.04f, 8.65f, 1.62f, 8.3f, 2.13f, 9.05f, 3.88f, 9.77f
};

	float W_CDECL zfun(float, float);

/****************************************************************************
	Main program
****************************************************************************/
void GPC_MAIN(void)
{
	double x1a;
	float pi = (float)PI;
	float x3axis, y3axis, z3axis, xvu, yvu, zvu, xdel, ydel;
	float xorig, xstp, xmax, yorig, ystp, ymax, zorig, zstp, zmax;
	int i, j, nxdiv, nydiv, npts, ixpts, iypts;
	float *x, *y, *xa, *ya, *za;

	/* Use GraphiC macros to allocate memory */
	x = (float *)gpcalloc(301, sizeof(float));
 	y = (float *)gpcalloc(301, sizeof(float));
	xa = (float *)gpcalloc(200, sizeof(float));
	ya = (float *)gpcalloc(200, sizeof(float));
	za = (float *)gpcalloc(200, sizeof(float));
	if (x == (void *)NULL || y == (void *)NULL ||
		xa == (void *)NULL || ya == (void *)NULL ||
		za == (void *)NULL) {
		GPC_PUTS("D2TEST: Could not allocate data arrays");
		goto EndOfApp;                     /* Free up memory before return */
	}

	bgnplot(1, 'g', "d2.tkf");                   /* GraphiC initialization */
	startplot(WHITE);
	metricunits(0);                                 /* Plots are in inches */

/****************************** FIRST PLOT *********************************/

	font(4, "Swiss.fnt", '\310', "Swissbld.fnt", '\311',  /* Select outline fonts */
		"Swissitl.fnt", '\312', "Newsgrm.fnt", '\313');
	page(4.5f, 3.442f);                      /* Size of page and plot area */
	pgshift(0.0f, 3.442f);               /* In upper left corner of screen */
	area2d(3.5f, 2.5f);
	color(BLACK);                                 /* Black box around plot */
	box();
	nxdiv = 5;                            /* Desired # of x-axis divisions */
	nydiv = 6;                            /* Desired # of y-axis divisions */
	npts = 301;                     /* Number of points in x and y vectors */
	for(i = 0; i < npts; i++) {
		x[i] = 20.f * (i * .01f - 1.5f);
		if((x[i] <= 1.e-8f) && (x[i] >= -1.e-8f))
			y[i] = 1.0f;
		else
			y[i] = (float)sin((double)x[i]) / x[i];
	}
	color(RED);                                /* Red axis and plot titles */
	xname("\313wt");
	yname("\312Amplitude");
	heading("\311Pulse Spectrum");

	color(BLACK);             /* Black crossed axes with dotted grid lines */
	cross(1);
	grid(2);
	scales(nxdiv, nydiv, x, y, npts);

	color(MAGENTA);                       /* Plot magenta symbol and label */
	symbol(8.f, .71f, 0);
	pltfnt(9.2f, .7f, "\310sin(x)/x", .12f, 0);

	color(LGT_BLUE);                             /* Bright blue curve with */
	curve(x, y, npts, 20);                      /* symbols every 20 points */

	color(BLACK);                                 /* Black date-time stamp */
	dateit((Uchar)'\310');
	errorbar(-14.f, -18.f, -12.f, .65f, .5f, .8f);

/****************************** SECOND PLOT ********************************/

	ixpts = 2;                                 /* Two segments across cell */
	iypts = 2;
	x3axis = 2.f;                                  /* x-axis twice as long */
	y3axis = 1.f;
	z3axis = 1.f;
	xvu = 5.15f;                             /* Viewing angle for the plot */
	yvu = 7.33f;
	zvu = 5.f;
	xorig = -pi;                                /* Sets left end of x-axis */
	xstp = 1.f;
	xdel = pi / 15.f;                           /* Sets the size of x step */
	xmax = pi;                                 /* x ranges from -pi to +pi */
	yorig = 0.f;                           /* Sets the lower end of y-axis */
	ystp = 1.0f;
	ydel = pi / 15.f;                           /* Sets the size of y step */
	ymax = pi;                                    /* y ranges from 0 to pi */
	zorig = -1.f;                                         /* z starts at 0 */
	zstp = .5f;                                     /* Sets size of z step */
	zmax = 1.f;                                   /* z ranges from -1 to 1 */
	page(4.5f, 3.442f);                      /* Size of page and plot area */
	pgshift(4.5f, 3.442f);                     /* In upper right of screen */
	area2d(3.5f, 2.5f);

	color(BLACK);                                 /* Black box around plot */
	box();

	x3name("\310X-axis");                                    /* Axis names */
	y3name("\310Y-axis");
	z3name("\310Z-axis");

	volm3d(x3axis, y3axis, z3axis);      /* Set axis lengths and viewpoint */
	vuabs(xvu, yvu, zvu);

	graf3d(xorig, xstp, xmax, yorig, ystp, ymax, zorig, zstp, zmax);

	base(1, 0);                                              /* Black base */
	tcolor(BLUE);                                /* Top of surface is blue */
	bcolor(MAGENTA);                        /* Botom of surface is magenta */
	surfun(zfun, ixpts, xdel, iypts, ydel);

	color(GREEN);                                      /* Green plot title */
	d3head("\311sin(y) * cos(y-x)", 't');

/****************************** THIRD PLOT *********************************/

	nxdiv = 13;                           /* Desired # of x-axis divisions */
	nydiv = 5;                            /* Desired # of y-axis divisions */
	npts = 24;                      /* Number of points in x and y vectors */

	font(4, "Swiss.fnt", '\310', "Swissbld.fnt", '\311',   /* Select outline fonts */
		"News.fnt", '\312', "Newsgrm.fnt", '\313');
	page(4.5f, 3.442f);                      /* Size of page and plot area */
	pgshift(4.5f, 0.f);                        /* In lower right of screen */
	area2d(3.5f, 2.5f);

	color(BLACK);                                 /* Black box around plot */
	box();

	j = 0;

	for(i = 0; i < npts; i++){
		j++;
		x[i++] = (float)j;
		x[i] = (float)j;
	}

	color(LGT_GREEN);                 /* Bright green axis and plot titles */
	fntchg((Uchar)'\312');
	xname("1984-1985");
	yname("Hundreds");
	heading("\311Sales");

	color(BLUE);                                    /* Blue axes and ticks */
	cross(0);
	grid(0);
	fgrid(0,0);
	xlab(1, xstring);                             /* Turn on string labels */
	graf("", 0.f, 1.f, 13.f, "%-1.1f", 0.f, 3.f, 15.f);
	xlab(0, xstring);

	bar(nxdiv, x, sales, npts, .6f, 1, carray, tarray);       /* Bar chart */

/****************************** FOURTH PLOT ********************************/

	nxdiv = 10;
	nydiv = 6;
	npts = 200;

	page(4.5f, 3.442f);                      /* Size of page and plot area */
	pgshift(0.0f, 0.0f);                        /* In lower left of screen */
	area2d(3.5f, 2.7f);

	color(BLACK);                                 /* Black box around plot */
	box();
	legpos(2, 5.3f, 5.1f, 1);                     /* Legend with 2 entries */
	legend(1, "\310Amplitude", 1, .125f, 14);
	legend(1, "Phase", 4, .125f, 4);

	for (i = 1; i <= npts; i++) {
		xa[i-1] = (float)i / 20.f;
		x1a = xa[i-1] * xa[i-1];
		ya[i-1] = (float)(1. / sqrt((1. - x1a) * (1. - x1a) +
			(.5 * xa[i-1]) * (.5 * xa[i-1])));
		if (i == 20)
			za[i-1] = pi / 2.f;
		else
			za[i-1] = (float)-atan(.5 * xa[i-1] / (1. - x1a));
		if (za[i-1] > 0)
			za[i-1] -= pi;
	}

	color(MAGENTA);                        /* Magenta axis and plot titles */
	xname("\312Frequency");
	yname("\312Amplitude");
	heading("\311Filter Response");

	color(BLACK);                      /* Black axes with upright y labels */
	upright(1);
	grid(0);
	fgrid(-1, 5);
	frame(1, 0);               /* Draw a frame around plot with tick marks */
	rightax(1);                     /* Second curve with right-hand y-axis */
	xlog(0.05f, 10.0f, "%-1.2f", 0.0f, 0.4f, 2.4f);

	curve(xa, ya, npts, 0);                  /* Draw curve with no symbols */

	color(MAGENTA);                          /* 2nd y-axis name is magenta */
	yname("\312Phase in Radians");

	color(BLACK);                            /* Right-hand y-axis is black */
	frame(0,0);                                                /* No frame */
	fgrid(0,0);
	xlog(0.05f, 10.0f, "%-1.1f", -3.6f, 0.6f, 0.0f);

	curve(xa, za, npts, 0);              /* Draw the curve without symbols */

	endplot();                                       /* Terminate the plot */
	stopplot();                                    /* Close files and quit */
EndOfApp:
	gpcfree((void **)&x);                  /* free allocated global memory */
	gpcfree((void **)&y);
	gpcfree((void **)&xa);
	gpcfree((void **)&ya);
	gpcfree((void **)&za);
}

/****************************************************************************
	Function for 3-D plot
****************************************************************************/
float W_CDECL zfun(float x, float y)
{
	return(float)(sin((double)y) * cos((double)(y - x)));
}
