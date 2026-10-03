/* (c) 1987-1993 by Scientific Endeavors Corporation.
   All rights reserved.
Version 7.0 3/2/1993
****************************************************************/
#include <graphic.h>
#include <limits.h>
#include <panfil.h>

#ifdef PANEL		/* exclude all panel filling code if not defined */

#define TESTSPLIT 0		/* =1 for testing panel splitting */

#if !GPC_CPP
extern int arrangepq(PPGXYPAIRS xypi, int *dim, int *np);
extern int pan_clipq(PPGXYPAIRS xyp, int *index, int *npanels);
extern void bounding_boxq(PGXYPAIRS xyp, int num_panels,
	int *xl, int *yl, int *xh, int *yh);
extern void split_holesq(int *pnumber);
extern int crossing_testq(int *x, int *y, int plines);
#endif


/**************************************************************
Called from panel(), gpros() in tplt.c and strsizq() in fntplt.c.
Calls fix_panq() to clip the panel then calls fix_panq() multiple
times as needed to take care of a window.

************************************************************/
#define x(i) g_pq.xyp[i].x
#define y(i) g_pq.xyp[i].y
void GCLASS panelq(void)
{
	int npts;
	int index;
	int nsides;
	int i;
	int *xpan;
	int *ypan;
	int x0, y0;
	int x1, y1;
	int x1c, y1c;
	G_PROCESS_ID

	if (g_pq.nsides < 3) {
		g_pq.nsides = 0;
		return;
	}
	if (g_pq.npanels == 0)
		vmaxq = g_iq[dev_typeq].vmax;
	hardware_pattern = 0;
	if (g_iq[dev_typeq].vector & ALL_POLYGONS)
		hardware_pattern = 1;
	else if (g_iq[dev_typeq].vector & POLYGONS_WITHOUT_PATTERNS) {
		if (!(g_pq.pattern > 0 && g_pq.pattern < 50))
			hardware_pattern = 1;
	}
	nsides = g_pq.nsides;
	g_pq.nsides = 0;
/* Make sure the panel is closed */
	if (g_pq.xyp[nsides - 1].x == g_pq.xyp[0].x &&
		g_pq.xyp[nsides - 1].y == g_pq.xyp[0].y)
		nsides--;
	else {							/* Make room for the closing point. */
		g_pq.xyp[nsides].x = g_pq.xyp[0].x;
		g_pq.xyp[nsides].y = g_pq.xyp[0].y;
	}

	npts = nsides + 1;
				/* Remove repeating points. */
	npts = RemoveDuplicatePoints(g_pq.xyp, npts);

	if (npts < 4)
		return;

	nsides = npts - 1;
	index = 0;
	x0 = x(0);
	y0 = y(0);
	for (i = 1; i < npts; i++) {
		x1 = x(i);
		y1 = y(i);
		index = cropq(x0, y0, x1, y1, &x0, &y0, &x1c, &y1c);
		if (index != 4)
			break;
		x0 = x1;
		y0 = y1;
	}
	if (index == 4) {
	/* Completely outside so see if we can throw it away. */
		xpan = (int *)gpcalloc(npts, sizeof(int));
		ypan = (int *)gpcalloc(npts, sizeof(int));
		if (xpan == NULL || ypan == NULL) {
			gpcfree((void **) &xpan);
			gpcfree((void **) &ypan);
			return;
		}
		for (i = 0; i < npts; i++) {
			xpan[i] = x(i);
			ypan[i] = y(i);
		}
		if (crossing_testq(xpan, ypan, npts - 1)) {
			gpcfree((void **) &xpan);	/* We can throw it away. */
			gpcfree((void **) &ypan);
			return;
		}
		gpcfree((void **) &xpan);
		gpcfree((void **) &ypan);
	}

	index = g_pq.npts;
	npts = g_pq.npts + nsides + 2;
	g_pq.save_xyp = (PGXYPAIRS)gpcrealloc(g_pq.save_xyp,
		(size_t)(npts * sizeof(struct GXYPAIRSQ)));
	g_pq.save_xyp[index].x = nsides + 1;
	g_pq.save_xyp[index].y = 0;
	index++;
	G_MEMCPY(&g_pq.save_xyp[index], g_pq.xyp,
		(nsides + 1) * sizeof(struct GXYPAIRSQ));
	g_pq.npts = npts;
	g_pq.npanels++;
}
#undef x
#undef y
/************************************************************/
/* Called from TEKINT.C so we need to be sure that the notekq flag is
properly set. */

void GCLASS panprocq(void)
{
	int notek_save;
	G_PROCESS_ID

	notek_save = notekq;
	notekq = 1;		/* Do not put panel in TKF file. */
	panproc();
	notekq = notek_save;
}
/*********************************************************************/
/* Break up panels to reduce sides if necessary */
/* define TESTSPLIT 1 and set max_psides in driver to debug panel
splitting on the screen. */

void TYPE GCLASS panproc(void)
{
	int xl, yl, xh, yh;
	int i, j, count;
	int bordsav;
	G_PROCESS_ID

	if (g_pq.npanels == 0)
		return;

	/* NULL the border array */
	g_pq.save_bord = (struct GXYPAIRSQ *)NULL;
	figure_8_testq(&g_pq);
	panelnumber = g_pq.npanels;
	if ((arrangepq(&g_pq.save_xyp, &g_pq.npts, &panelnumber)) <= 0)
		return;

	pan_clipq(&g_pq.save_xyp, &g_pq.npts, &panelnumber);
	if (panelnumber == 0) {
  		gpcfree((void **) &g_pq.save_xyp);
  		g_pq.npts = 0;
  		g_pq.nsides = 0;
  		g_pq.npanels = 0;
  		return;
	}

	g_pq.npanels = panelnumber;

	j = 0;
	g_pq.max_npts = 0;
	for (i = 0; i < panelnumber; i++) {
		count = g_pq.save_xyp[j++].x;
		if (count > g_pq.max_npts)
			g_pq.max_npts = count;
		j += count;
	}
	gpcfree((void **) &g_pq.xyp);

	/* CHECK FOR PANEL-SPLITTING OPTIONS */
	bordsav = g_pq.border;
	if (g_iq[dev_typeq].vector & NO_HOLES && hardware_pattern) {
		g_pq.save_bord = (struct GXYPAIRSQ *)gpcalloc(g_pq.npts,
			sizeof(struct GXYPAIRSQ));
		if(g_pq.save_bord != NULL) {
			G_MEMCPY(g_pq.save_bord, g_pq.save_xyp,
				g_pq.npts * sizeof(struct GXYPAIRSQ));
			g_pq.bnpts = g_pq.npts;
		}
		g_pq.border = 0;
		split_holesq(&panelnumber);
	}
#if TESTSPLIT 	/* Testing only */
	if (g_iq[dev_typeq].max_psides == 0 ||
		g_pq.npts < g_iq[dev_typeq].max_psides)
		panproc2q();
#else
	if ((g_iq[dev_typeq].max_psides == 0)
		|| (g_pq.npts < g_iq[dev_typeq].max_psides)
		|| (g_iq[dev_typeq].vector == 0)
		|| !hardware_pattern) {
			panproc2q();
		}
#endif
	else {
		g_pq.border = 0;
		bounding_boxq(g_pq.save_xyp, g_pq.npanels, &xl, &yl, &xh, &yh);
		split_in_halfq(xl, yl, xh, yh);
	}
	g_pq.border = bordsav;		/* Restore original border */
	if (g_pq.border) {
		g_pq.gfunc = PANPROC;
		g_pq.border = 2;
		f_tableq((struct GPANELQ *)&g_pq);
		g_pq.border = 1;
	}
	g_pq.npts = 0;
	g_pq.npanels = 0;
	gpcfree((void **) &g_pq.save_xyp);
	gpcfree((void **) &g_pq.save_bord);
#ifndef DEMO
	if (!notekq) {
		tkbfillq("\33LE\35");
		if (lcolor_saveq != g_pq.b_color)
			ccolorq('g', lcolor_saveq);
	}
#endif
}
/*********************************************************************/
/* Process a panel that has few enough sides to be processed in the driver */
void GCLASS panproc2q(void)
{
	int i, j = 0, k;
	int start = 0, end = 0, s_tog = 0, e_tog = 1;
	static int *local_xcount;		/* Pointer to y-crossing array */
	static int *local_tran;			/* Translates y to occupied y. */
	struct panelsq *local_panl;			/* Contains the x roots. */
	G_PROCESS_ID


/* THIS LOOP PROCESSES EACH PANEL PIECE AND CALLS THE DRIVER */
	if (!hardware_pattern) {
			/* ALLOCATE NEEDED MEMORY FOR DRIVER */
		g_pq.xcount = (int *)gpcalloc((size_t)(vmaxq + 1),
			sizeof(int));
		if (g_pq.xcount == (int *)NULL) {
			freeallq(0);
			return;
		}
	}

	for (i = 0; i < panelnumber; i++) {
/* Load the panel structure with the individual panel parts and send them
to the driver. */
		g_pq.gfunc = PANL;
		g_pq.b_color = g_lq.color;
		g_pq.nsides = g_pq.save_xyp[end].x;
		end++;
		start = end;
		end += g_pq.nsides;
		g_pq.nsides--;
		g_pq.xyp = &g_pq.save_xyp[start];
		load_panq();
		if (!f_tableq(&g_pq))
			g_pq.save_xyp[start - 1].y = -2;	/* Printer dumps need to know
											   which panels will not show
											   for the pass being
											   processed. */
		if ((g_iq[dev_typeq].vector & NO_HOLES) && hardware_pattern) {
			g_pq.gfunc = PANPROC;
			f_tableq(&g_pq);
		}
	}
	g_pq.xyp = NULL;
	if ((g_iq[dev_typeq].vector & NO_HOLES) && hardware_pattern) {
		panelnumber = 0;
		plns_glb = 0;
		g_pq.nsides = 0;
		return;
	}
	j = 0;
	start = 0;
	end = vmaxq;
	if (hardware_pattern) {
		g_pq.gfunc = PANPROC;
		f_tableq(&g_pq);
		panelnumber = 0;
		plns_glb = 0;
		g_pq.nsides = 0;
		return;
	}
	/* CALCULATE NEEDED MEMORY FOR DRIVER AND ALLOCATE IT */
	g_pq.tran = (int *)gpcalloc((size_t)(vmaxq + 1), sizeof(int));
	if (g_pq.tran == (int *)NULL) {
		freeallq(0);
		return;
	}
	local_tran = g_pq.tran;
	local_xcount = g_pq.xcount;			/* Use local variables for speed. */
	for (i = 0; i < vmaxq; i++) {
		if (local_xcount[i] == 0) {		/* xcount is loaded in the driver */
			if (!e_tog) {				/* e_tog is used as a signal to set */
				e_tog = 1;				/* end to the highest y level in the */
				end = i;				/* panel(s) being processed. */
			}
			local_tran[i] = ( -1);
			continue;
		}
		if (!s_tog) {
			s_tog = 1;
			start = i;
		}
		if (e_tog)
			e_tog = 0;
		local_tran[i] = j++;		/* Translate to occupied-y space. */
	}
	if (!e_tog)						/* panel is at the bottom */
		end = i;
	/* Now, j is the number of scan lines occupied by the panel */

	plns_glb = g_pq.noccupied = j;

	g_pq.start = start;
	g_pq.end = end;
	if (j > 2) {		/* Panel is > 1 pixel high in output space */
		g_pq.panl = (struct panelsq *)gpcalloc((size_t)(j + 2),
			sizeof(struct panelsq));
		if (g_pq.panl == (void *)NULL) {
			messageq(ERRQ, 31);		/* Not enough memory for panel. */
			freeallq(0);
			return;
		}
		local_panl = g_pq.panl;
		for (i = start; i < end; i++) {
			k = local_tran[i];
			if (k == -1)
				continue;
			local_panl[k].xp = (int *)gpcalloc(
				(size_t)((local_xcount[i]) + 2), sizeof(int));
			local_panl[k].xp1 = (int *)gpcalloc(
				(size_t)((local_xcount[i]) + 2), sizeof(int));
			if (local_panl[k].xp == (int *)NULL ||
				local_panl[k].xp1 == (int *)NULL) {
				messageq(ERRQ, 31);		/* Not enough memory for panel. */
				freeallq(1);
				return;
			}
		}
		g_pq.xscale = (int *)gpcalloc((size_t)(g_pq.max_npts + 1),
			sizeof(int));
		g_pq.yscale = (int *)gpcalloc((size_t)(g_pq.max_npts + 1),
			sizeof(int));
		if (g_pq.xscale == (int *)NULL ||
			g_pq.yscale == (int *)NULL) {
			gpcfree((void **) &g_pq.xscale);
			gpcfree((void **) &g_pq.yscale);
			messageq(ERRQ, 31);		/* Not enough memory for panel. */
			freeallq(1);
			return;
		}
	}

	g_pq.gfunc = PANPROC;
	f_tableq(&g_pq);
	if (plns_glb > 2) {
		gpcfree((void **) &g_pq.xscale);
		gpcfree((void **) &g_pq.yscale);
		freeallq(1);
	}
	else
		freeallq(0);
}

/**************************************************************/
/* Free up all the memory allocated for the previous panel */

void GCLASS freeallq(int mode)
{
	int i;
	G_PROCESS_ID

	g_pq.nsides = 0;
	panelnumber = 0;
	gpcfree((void **) &g_pq.xcount); /* Free crossing-count */
	gpcfree((void **) &g_pq.tran); /* and translation arrays. */
	if (!mode) {
		plns_glb = 0;
		return;
	}
	if (g_pq.panl != (void *)NULL) { /* Release arrays */
		for (i = 0; i < plns_glb; i++) {
			gpcfree((void **) &g_pq.panl[i].xp);
			gpcfree((void **) &g_pq.panl[i].xp1);
		}
	}
	gpcfree((void **) &g_pq.panl); /* Release panl structures. */
	plns_glb = 0;
}

/*********************************************************************/
/* Split a panel recursively so that it has few enough sides to be
processed in the driver. The original panel structure quantities must
always be in the panel structure when this routine is called. */
void GCLASS split_in_halfq(int xll, int yll, int xur, int yur)
{
	int dx, dy;
	int xl[2], yl[2], xu[2], yu[2];
	int newdim, n_clusters, leftdim;
	int j, k, n, np, npsav;
	/* Storage for the g_pq elements we must retain */
	PGXYPAIRS clipped_xyp_sav, xycs;
	int npts_sav, npanels_sav;
	G_PROCESS_ID

	/* SPLIT THE LONG DIRECTION INTO TWO */
	dx = xur - xll;
	dy = yur - yll;
	if (dx > dy) {
		xl[0] = xll;
		xu[0] = xl[1] = xll + dx/2;
		xu[1] = xur;
		yl[0] = yl[1] = yll;
		yu[0] = yu[1] = yur;
	}
	else {
		xl[0] = xl[1] = xll;
		xu[0] = xu[1] = xur;
		yl[0] = yll;
		yu[0] = yl[1] = yll + dy/2;
		yu[1] = yur;
	}
	/* CLIP PANELS ON EACH HALF-SPACE */
	npts_sav = g_pq.npts;	/* Save the panel quantities */
	npanels_sav = g_pq.npanels;
	clipped_xyp_sav = g_pq.save_xyp;

	for (k = 0; k < 2; k++) {		/* For each half... */
		n_clusters = 0;
		newdim = clip_halfq(g_pq.npts, clipped_xyp_sav, &g_pq.npanels,
			&n_clusters, xl[k], yl[k], xu[k], yu[k]);
		if (newdim <= 0 || n_clusters == 0) {
			gpcfree((void **) &g_pq.save_xyp);
			g_pq.save_xyp = clipped_xyp_sav;
			g_pq.npts = npts_sav;
			g_pq.npanels = npanels_sav;
			if (newdim >= 0)
				continue;
			else
				return;		/* Failure in clipping routine */
		}
		if (newdim <= g_iq[dev_typeq].max_psides) {
#if TESTSPLIT
g_lq.xyp = (PGXYPAIRS)gpcalloc(6, sizeof(struct GXYPAIRSQ));
g_lq.color = 0;
g_lq.xyp[0].x = 5;
g_lq.xyp[1].x = xl[k];
g_lq.xyp[1].y = yl[k];
g_lq.xyp[2].x = xu[k];
g_lq.xyp[2].y = yl[k];
g_lq.xyp[3].x = xu[k];
g_lq.xyp[3].y = yu[k];
g_lq.xyp[4].x = xl[k];
g_lq.xyp[4].y = yu[k];
g_lq.xyp[5].x = xl[k];
g_lq.xyp[5].y = yl[k];
g_lq.npts = 5;
f_tableq(&g_lq);
gpcfree((void **) &g_lq.xyp);
#endif
			/* Success! */
			if (newdim - g_pq.npanels > 3) {	/* Process the panel */
				/*	Here we must process each cluster (an outside together
					with its holes) separately to preserve the winding-
					rule fill properties.
			   	*/
				npsav = g_pq.npanels;
				xycs = g_pq.save_xyp;	/* Pointer to array start */
				leftdim = newdim;
				for (j = 0; j < n_clusters; j++) {
					n = 0;				/* no panels and no points */
					np = 0;
					/* Find the end of the first cluster */
			/* Since n is an index and newdim is a count we need
			   to test n against leftdim - 1. */
					while (n < leftdim - 1) {
						n += (g_pq.save_xyp[n].x + 1);
						np++;
						if (np == npsav)
							break;
						if (g_pq.save_xyp[n].y )
							break;
					}
					g_pq.npts = n;
					g_pq.npanels = panelnumber = np;
					npsav -= np;
					leftdim -= n;
					/* Pass in the new clipped half */
					panproc2q();
					g_pq.npts = 0;
					g_pq.npanels = 0;
					g_pq.save_xyp += n;
				}
				g_pq.save_xyp = xycs;
			}
			/* Else, do nothing, too few sides. */
		}
		/* FREE MEMORY ALLOCATED IN THE CLIPPER */
		gpcfree((void **) &g_pq.save_xyp);

		/* RESTORE THE PANEL STRUCTURE QUANTITIES */
		g_pq.save_xyp = clipped_xyp_sav;
		g_pq.npts = npts_sav;
		g_pq.npanels = npanels_sav;

		/* FAILURE. SPLIT THE HALF IN TWO PIECES */
		if (newdim > g_iq[dev_typeq].max_psides) {
			split_in_halfq(xl[k], yl[k], xu[k], yu[k]);
		}
	}
}

/*********************************************************************/
/* These routines are used for reducing the number of sides in a panel */
/*********************************************************************/
/***********************************************************/
/* CLIP THE PANEL USING THE LIANG-BARSKY ALGORITHM (COMM. ACM
VOL. 26, 868 (1983). (corrected in vol 27, #4, p383) */

int GCLASS clip_halfq(int dim, PGXYPAIRS xyp, int *num_panels,
int * n_clusters, int xlq, int ylq, int xhq, int yhq)		/* Clipping values */

{
	int xls, yls, xhs, yhs;
	G_PROCESS_ID

	xls = xl_cropq;
	yls = yl_cropq;
	xhs = xh_cropq;
	yhs = yh_cropq;

	xl_cropq = xlq;
	yl_cropq = ylq;
	xh_cropq = xhq;
	yh_cropq = yhq;

	g_pq.save_xyp = (PGXYPAIRS)gpcalloc(dim,
		sizeof(struct GXYPAIRSQ));
	G_MEMCPY(g_pq.save_xyp, xyp, dim * sizeof(struct GXYPAIRSQ));
	*n_clusters = pan_clipq(&g_pq.save_xyp, &dim, num_panels);

	xl_cropq = xls;
	yl_cropq = yls;
	xh_cropq = xhs;
	yh_cropq = yhs;

	return(dim);
}
/*********************************************************************/
/**********************************************************************/
/* Dummy routines to provide panelq() and panproc() if PANEL is not   */
/* defined in USER.H.                                                 */
/**********************************************************************/
#else /* #ifdef PANEL */

void GCLASS panelq(void)
{
	messageq(WARNQ, 23);	/* Tell the user how to do panel filling */
}
void TYPE GCLASS panproc(void)
{
}

#endif
#ifdef MULTITHREAD
void GCLASS pan_init(int action, int counter)
{
	if (action) {
		panel_data[counter] = (struct G_PANEL_DATA *)gpcalloc(1,
			sizeof(struct G_PANEL_DATA));
	}
	else
		gpcfree((void **) &panel_data[counter]);
}
#endif

/**************************************************************
load_panq fills the cmd_strq with the proper information to process a
panel. The panel information is already loaded in the g_pq structure and
gets put into the .TKF file from a call in gprosq(). */

int GCLASS load_panq(void)
{
	int i;
	char local_string[10];
	int value;
	char res[5];
	G_PROCESS_ID

	lcolor_saveq = g_lq.color;
	if (lcolor_saveq != g_pq.b_color) {
		ccolorq('g', g_pq.b_color);
	}
	if (g_pq.pattern < 0)
		value = ( -itotekq[ -g_pq.pattern]);
	else
		value = g_pq.pattern;
	if (value != pattern_saveq) {
		NCOLORQ++;
		tkbfillq("\33MP");
		encodeq(value, res);
		tkbfillq(res);
		pattern_saveq = value;
	}
	totekq(g_pq.xyp[0].x, g_pq.xyp[0].y, 1);	/* put first point in string */
	g_pq.xyp[0].x = g_lq.pfx;
	g_pq.xyp[0].y = g_lq.pfy;
	strcpy(local_string, "\33L");
	strcat(local_string, cmd_strq);
	local_string[2] = 'P';		/* overwrite the penup put in by cmd_strq */
	i = strlen(cmd_strq) + 2;
	if (g_pq.border)			/* put border int TKF file */
		local_string[i++] = '1';
	else
		local_string[i++] = '0';
	local_string[i] = '\0';		/* terminate the string */
	tkbfillq(local_string);		/* put panel in TKF file */
	for (i = 1; i <= g_pq.nsides; i++) {
		totekq(g_pq.xyp[i].x, g_pq.xyp[i].y, 0);
		g_pq.xyp[i].x = g_lq.ptx;
		g_pq.xyp[i].y = g_lq.pty;
		tkbfillq(cmd_strq);
	}
	return(1);
}
/***************************************************************************
This routine adds the angles oof all the sides in a polygon to determine
if the panel is figure 8 shaped. If so the resulting angle should be very
near 0. Otherwise the angle should be plus or minus 2 * PI. */
#if MSQ
#pragma optimize("",off)
#endif
#if TCQ
#pragma option -Od
#endif
#define x(i) gp -> save_xyp[i].x
#define y(i) gp -> save_xyp[i].y
void GCLASS figure_8_testq(struct GPANELQ *gp)
{
	double a, b, c;		/* the three sides of a triangle. */
	double angle;		/* the acumulated clockwise angle. */
	double newang;		/* the angle for the present set of points. */
	double cos_b;		/* the cos of the angle newang. */
	int dx, dy, dx1, dy1;	/* storage variables for the deltas. */
	int start;			/* start index of each piece. */
	int x1, y1;			/* the second point of a given polygon.
						   used to calculate the closing angle. */
	int inc;			/* + or - depending on the change of direction. */
	int i, j, npts;		/* counters. */

	angle = 0.;
	i = 0;
	while (i < gp -> npts) {
		start = i;
		npts = x(i);		/* Get the number of points this piece. */
		i++;				/* advance to first point this piece. */
		if (npts < 5) {		/* cannot cross with only 3 sides. */
			i += npts;
			continue;
		}
		/* calculate the first side. */
		dx1 = x(i + 1) - x(i);	/* first deltas. */
		dy1 = y(i + 1) - y(i);
		i++;

		c = sqrt((double)((long)dx1 * (long)dx1) +
			(double)((long)dy1 * (long)dy1));
		x1 = x(i);		/* save second point. */
		y1 = y(i);

		for (j = 1; j < npts - 1; j++) {
		/* calculate the length of the next side. */
			dx = x(i + 1) - x(i);
			dy = y(i + 1) - y(i);
			a = sqrt((double)((long)dx * (long)dx) +
				(double)((long)dy * (long)dy));

			/* determine wether to add or subtract this angle. */
			inc = sign(((long)dx1 * (long)dy) - ((long)dy1 * (long)dx));
			dx1 = dx;	/* update dx1 and dy1. */
			dy1 = dy;
		/* calcualte the length of the side bewteen the first and last
		   points of this trio. This side is not part of the polygon but
		   is used to find the angle between the other two sides. */
			dx = x(i - 1) - x(i + 1);
			dy = y(i - 1) - y(i + 1);
			i++;

		/* Get length. */
			b = sqrt((double)(((long)dx * (long)dx) +
				((long)dy * (long)dy)));

		/* calculate the cosine of the angle. */
			if (a == 0 || c == 0)
				continue;
			cos_b = ((a * a + c * c) - (b * b)) / (2 * a * c);

		/* if the angle is not 0, find the angle. */
			if (gabs(1 - gabs(cos_b)) > .000001) {
				if (gabs(cos_b) > 1)
					newang = 12345;
				else
					newang = PI - acos(cos_b);
			}
			else {
				if (cos_b > 0)
					newang = PI;
				else
					newang = 0;
			}
			angle += (newang * inc);
			c = a;
		}

	/* Now calculate the angle at the closing point. Same logic as above.*/
		dx = x1 - x(i);
		dy = y1 - y(i);
		a = sqrt((double)((long)dx * (long)dx) +
			(double)((long)dy * (long)dy));

		inc = sign(((long)dx1 * (long)dy) - ((long)dy1 * (long)dx));

		dx1 = dx;
		dy1 = dy;
		dx = x(i - 1) - x1;
		dy = y(i - 1) - y1;

		b = sqrt((double)(((long)dx * (long)dx) +
			((long)dy * (long)dy)));
		if (a != 0 && c != 0) {
			cos_b = ((a * a + c * c) - (b * b)) / (2 * a * c);
			if (gabs(1 - gabs(cos_b)) > .000001)
				newang = PI - acos(cos_b);
			else {
				if (cos_b > 0)
					newang = PI;
				else
					newang = 0;
			}
		}
		angle += (newang * inc);
		i++;
	/* Use .01 as the test value to allow for rounding errors. */
		inc = (int)((gabs(angle) / PI) + .5);
		if (inc == 0  || (inc % 2)) {
			if (split_panelq(gp, start))
				i = start + npts + 1;
			else
				i = start;
		}
		angle = 0;
	}
}
#undef x
#undef y
#if MSQ
#pragma optimize("",on)
#endif
/***********************************************************************
This routine will contain the code to divide the panel at its crossing
point. */

int GCLASS split_panelq(struct GPANELQ *gp, int start_index)
{
	int i, j, k;
	int x0, x1, y0, y1;
	int xp0, xp1, yp0, yp1;
	float slope_0, slope_1;
	int x_cross, y_cross;
	int npts, newnpts;
	int start;
	int shared_point;
	int break_out;
	PGXYPAIRS xyp;
	PGXYPAIRS newxyp;
	G_PROCESS_ID

	xyp = &(gp -> save_xyp[start_index]);
	npts = xyp[0].x;
	xyp++;
	break_out = 0;
	x1 = 0;
	y1 = 0;
	xp0 = 0;
	xp1 = 0;
	yp0 = 0;
	yp1 = 0;
	j = 0;

	x0 = xyp[0].x;
	y0 = xyp[0].y;

	for (i = 1; i < npts - 1; i++) {
		x1 = xyp[i].x;
		y1 = xyp[i].y;
		xp0 = xyp[i + 1].x;
		yp0 = xyp[i + 1].y;
		for (j = i + 2; j < npts; j++) {
			xp1 = xyp[j].x;
			yp1 = xyp[j].y;
			if (xp0 > x0 && xp0 > x1 && xp1 > x0 && xp1 > x1)
				goto next_point;
			if (xp0 < x0 && xp0 < x1 && xp1 < x0 && xp1 < x1)
				goto next_point;
			if (yp0 > y0 && yp0 > y1 && yp1 > y0 && yp1 > y1)
				goto next_point;
			if (yp0 < y0 && yp0 < y1 && yp1 < y0 && yp1 < y1)
				goto next_point;
			break_out = 1;
			break;
next_point:
			xp0 = xp1;
			yp0 = yp1;
			continue;
	/* If there is an intersection, we need to load a new array with xyp[]
	up to i - 1, the intersecting point, then xyp from j to npts - 1. The
	next piece will start at the intersecting point and go from xyp[i]
	through xyp[j] then back to the intersecting point. */
		}
		if (break_out)
			break;
		x0 = x1;
		y0 = y1;
	}
	if (!break_out)
		return(1);
	if (x1 == xp0 && y1 == yp0) {
		shared_point = 1;
		x_cross = x1;
		y_cross = y1;
	}
	else {
		shared_point = 0;
		if (x0 == x1)
			slope_0 = (float)MAXRESX;
		else
			slope_0 = (float)(y1 - y0) / (float)(x1 - x0);
		if (xp0 == xp1)
			slope_1 = (float)MAXRESX;
		else
			slope_1 = (float)(yp1 - yp0) / (float)(xp1 - xp0);
		if (slope_0 == slope_1)
			x_cross = x0;
		else
			x_cross = x0 +
				(int)((float)(yp1 + ((float)(x0 - xp1) * slope_1) - y0)
				/ (slope_0 - slope_1) + .5);
		y_cross = (int)(((float)(x_cross - xp0) * slope_1) + .5) + yp0;
		if (!(((x_cross < x0) && (x_cross > x1)) ||
			((x_cross > x0) && (x_cross < x1)))) {
			break_out = 0;
			goto next_point;
		}
		if (!(((y_cross < y0) && (y_cross > y1)) ||
			((y_cross > y0) && (y_cross < y1)))) {
			break_out = 0;
			goto next_point;
		}
	}
	k = i + (npts - j);		/* Find the number of elements in this piece */
	if (k < 3) {
		break_out = 0;
		goto next_point;
	}
	newxyp = (PGXYPAIRS)gpcalloc(npts + 5, sizeof(struct GXYPAIRSQ));
	xyp--;
	i++;
	k++;

	/* Put in first part of panel. */
	G_MEMCPY(newxyp, xyp, i * sizeof(struct GXYPAIRSQ));

	newxyp[i].x = x_cross;
	newxyp[i].y = y_cross;
	G_MEMCPY(&(newxyp[i + 1]), &(xyp[j + 1]),
		(npts - j) * sizeof(struct GXYPAIRSQ));
	k = RemoveDuplicatePoints(&(newxyp[1]), k);
	if (k < 4) {
		k = -1;
		g_pq.npanels--;
	}
	else
		newxyp[0].x = k;	/* count in the count element. */

	j++;
	k += 2;
	start = k;

	/* Starting point. */
	if (!shared_point) {
		newxyp[k].x = x_cross;
		newxyp[k++].y = y_cross;
	}

	/* Body of panel. */
	G_MEMCPY(&(newxyp[k]), &(xyp[i]),
		(j - i) * sizeof(struct GXYPAIRSQ));

	/* Closing point. */
	k += (j - i);
	if (!shared_point) {
		newxyp[k].x = x_cross;
		newxyp[k++].y = y_cross;
	}

	k = RemoveDuplicatePoints(&(newxyp[start]), k - start);
	newxyp[start - 1].x = k;
	if (k < 4) {
		gp -> npanels--;
		k = -1;
	}
	k += start;
	npts++;			/* Add one for the count element. */

	if (gp -> npts > (npts + start_index)) {
	/* Remove unsplit panel from xyp array. */
		G_MEMCPY(xyp, &(xyp[npts]),
			(gp -> npts - (npts + start_index)) * sizeof(struct GXYPAIRSQ));
		newnpts = gp -> npts - npts;
	}
	else
		newnpts = start_index;
	gp -> save_xyp = (PGXYPAIRS)gpcrealloc(gp -> save_xyp,
		(size_t)((newnpts + k) * sizeof(struct GXYPAIRSQ)));
	gp -> npts = newnpts + k;
	G_MEMCPY(&(gp -> save_xyp[newnpts]),
		newxyp,
		k * sizeof(struct GXYPAIRSQ));
	gpcfree((void **) &newxyp);
	gp -> npanels++;
	return(0);
}
/***********************************************************************/
/* This routine takes an input GXYPAIRSQ array an finds any consecutive
duplicate points and removes them from the array. */

#define x(i) xyp[i].x
#define y(i) xyp[i].y

int GCLASS RemoveDuplicatePoints(PGXYPAIRS xyp, int npts)
{
	int i, k;
	int x1, y1;

	i = 0;
	x1 = x(i);
	y1 = y(i);
	k = 1;
	for (i = 1; i < npts; i++) {
		if (x1 == x(i) && y1 == y(i))
			continue;
		x1 = x(i);
		y1 = y(i);
		if (k != i) {
			x(k) = x1;
			y(k) = y1;
		}
		k++;
	}
	return(k);
}
