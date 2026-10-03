/*	(c) 1993 by Scientific Endeavors Corp.
	All rights reserved.

	Routines for drawing symbols.
	Vesrion 7.0 3/22/92
**************************************************/
#include <graphic.h>

#define SYMMAX 18

struct symtab {
	char *tab;
	int symwidth;
	int symht;
};
static struct symtab symbols[] = {
	{"AAKAKKAKAA", 11, 11},								/* 0  Square */
	{"EAGAIBJCKEKGJIIJGKEKCJBIAGAEBCCBEA", 11, 11},		/* 1  Circle */
	{"FAKKAKFA", 11, 11},								/* 2  Del */
	{"AGLG*GAGM", 13, 13},								/* 3  + */
	{"AAKK*KAAK", 11, 11},								/* 4  x */
	{"HAOHHOAHHA", 15, 15},								/* 5  diamond */
	{"AAKAFKAA", 11, 11},								/* 6   */
	{"AACCEDGDICKA*AKCIEHGHIIKK*FAFK", 11, 11},			/* 7  Stick figure */
	{"DAHA*DKHK*ADAH*KDKH*AFKF*FAFK", 11, 11},			/* 8  + with feet. */
	{"AAKA", 11, 1},									/* 9  Horizontal line */
	{"AAAK", 1, 11},									/* 10 Vertical line */
	{"AATATOAOAA", 20, 15},								/* 11 Rectangular box */
	{"AAAA", 1, 1},										/* 12 A dot */
	{"AABABBABAA", 2, 2},								/* 13 A big dot */
	{"AACACCACAA", 3, 3}								/* 14 A bigger dot */
	};

/*******************************************************************/
/* USER-CALLABLE SYMBOL PLOTTER. X, Y ARE USER (PLOT) UNITS. */
void TYPE GCLASS symbol(
	float x,	/* x location of symbol */
	float y,	/* y location of symbol */
	int symnum	/* desired symbol */
)
{
	int x0, y0;
	G_PROCESS_ID

	if (symnum < 0 || symnum > SYMMAX) {
		messageq(ERRQ, 22);		/* Invalid symbol style */
		return;
	}
	if (!crop_flgq)
		acropq(1);
	else
		acropq(0);
	if (plot_typeq &
	 (LINEAR_PLOT | XLOG_PLOT | YLOG_PLOT | POLAR_PLOT | POLAR_LOG_PLOT)) {
		usertopix(x, y, 0.0f, &x0, &y0);
		sympltq(x0, y0, symnum);
	}
	acropq(0);
}
/*******************************************************************/
/* PLOT SYMBOLS CENTERED ON X, Y IN PIXEL (MAXRESX x MAXRESY) SPACE */
void GCLASS sympltq(
	int x,		/* x location of symbol */
	int y,		/* y location of symbol */
	int symcnt	/* desired symbol */
)
{
	char *p, mflag;
	int dsh;
	int thicksav;
	static int pat;
	int symw, symh, symend = 0;
	int xtemp, ytemp;
	int action_save, npts_save;
	G_PROCESS_ID

	action_save = g_lq.action;
	npts_save = g_lq.npts;
	g_lq.action = 0;
	g_lq.npts = 0;
	dsh = g_lq.pattern;
	thicksav = g_lq.thick;
	dashf(1);

	if (tcurveq != 0)
		tline(0);
	if (symcnt >= 16 && symcnt != 20)	/* DOTs */
		symcnt -= 4;
	else if (symcnt > 10) {				/* FILLED SYMBOLS */
		if (symfillq < 4) {
			symfillq = 3;
			pat = (g_lq.color < 16 ? -g_lq.color : g_lq.color);
		}
		/* PICK THE CORRECT OUTLINE NUMBER (0-10) */
		/* NOTE: symcnt == 20 from legend for panel fills */
		symcnt -= 11;
		if (symcnt > 2 && symcnt != 9)
			symcnt += 2;
		else if (symcnt == 9) {			/* Panel-filled legend */
			symcnt = 11;
			pat = symfillq - 20;			/* Was set to 20 + pattern #. */
		}
		panfilq = 1;
		g_pq.nsides = 0;
		if (g_pq.xyp == NULL) {
			g_pq.xyp = (struct GXYPAIRSQ *)gpcalloc(64,
				sizeof(struct GXYPAIRSQ));
			if (g_pq.xyp == NULL) {
				messageq(WARNQ, 2);		/* Out of Memory */
				panfilq = 0;
				symfillq = 0;
			}
		}
	}
	p = symbols[symcnt].tab;
	symw = (symbols[symcnt].symwidth) >> 1;
	symh = (symbols[symcnt].symht) >> 1;
	mflag = 1;
	for (;;) {
		if (*(p + 2) == '\0') {
			symend = 1;
			if (symfillq == 1)
				symfillq = 2;
		}
		if (*p == '*') {
			p++;
			mflag = 1;
		}
		xtemp = (int)((float)((*(p++) - 'A') - symw) * symhtq) + x;
		ytemp = (int)((float)((*(p++) - 'A') - symh) * symhtq) + y;
		if (mflag) {
			if (symfillq > 0 && !wplane_flgq) {
										/* We're making a filled symbol. */
				g_pq.xyp[g_pq.nsides].x = xtemp;
				g_pq.xyp[g_pq.nsides].y = ytemp;
				g_pq.nsides = newbufsq(&g_pq.xyp, g_pq.nsides);
			}
			else
				lmoveq(xtemp, ytemp);
			mflag = 0;
		}
		else {
			if (symfillq > 0 && !wplane_flgq) {
										/* We're making a filled symbol. */
				g_pq.xyp[g_pq.nsides].x = xtemp;
				g_pq.xyp[g_pq.nsides].y = ytemp;
				g_pq.nsides = newbufsq(&g_pq.xyp, g_pq.nsides);
			}
			else
				ldrawq(xtemp, ytemp);
		}
		if (symend == 1)
			break;
	}
	if (symfillq > 0) {					/* We're finished with contour. */
		symfillq = 0;
		g_pq.pattern = pat;
		g_pq.border = 0;
		panelq();
		gpcfree((void **) &g_pq.xyp);
		panproc();
		g_pq.nsides = 0;
	}
	g_lq.action = action_save;
	g_lq.npts = npts_save;
	panfilq = 0;
	if (tcurveq != 0)
		tline(thicksav);
	lmoveq(x, y);
	dashf(dsh);
}
/**************************************************************/
/* MULTIPLY HEIGHT OF A SYMBOL */
void TYPE GCLASS symht(
	float height	/* Height of symbol in inches (cm) */
)
{
	G_PROCESS_ID

	symhtq = (float)(XSCALE_PAGE((intopix(height)) / 10));
}
/**************************************************************/
/* Select the symbol to be used on the next curve */
void TYPE GCLASS sympick(
	int nsymbol	/* symbol number for the next curve */
)
{
	G_PROCESS_ID

	symcntq = nsymbol;
}
/***************************************************************
ALLOWS USER TO POSISTION A SYMBOL IN INCH (OR CM) COORDINATES. */
void TYPE GCLASS syminch(float x, float y, int symnum)
{
	int sx, sy;
	G_PROCESS_ID

	crop_flgq = 1;		/* crop to page not area2d */
	sx = intopix(x);
	sy = intopix(y);
	sympltq(sx, sy, symnum);
}
