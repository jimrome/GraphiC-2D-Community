/* Copyright 1984-1995 by Scientific Endeavors Corporation. All rights

reserved. GGK 9/2/88 Line handling has been modified to send only
individual vector ends. There are two line handling routines: lmoveq() and
ldrawq(). lmoveq() sets the global variables x0q and y0q in
pixel space to the start of a line. It sets penup_flgq to 1, indicating
that rescaling of start values is necessary.

ldrawq() routes 3-d procedures and passes 2-d procedures to ldraw2q()
after rotating the variables for rotated pictures and cropping. This
routine sends the line information to the device drivers for processing.

9/11/86 ldrawq has been modified to blank a window specified by a call to
window(lower left x, y, upper right x, y). User's call to window() sets
the flag wflagq.

ldrawq() modified to accept multiple windows 9/5/90 GGK. There are three
user functions that use these windows. The first arises from a call to
gwindow(). The result is a blank area that is filled by an automatically
scaled subsequent picture. Multiple calls to gwindow() are supported. The
second arises from a call to put_label(string, position). The result is a
blank area sized to fit the (multi-line) string. A box is drawn around the
area after the plot has been made. The third is the legend-maker, which
takes care of its own border. win_numq takes the place of wflagq, but
holds the number of windows currently open. The executive program for
setting windows is win2q(coordinates, type). The program for drawing
borders is win_boxq() and win_procq() is called in  startplot to set
scaling for plot-in-window.

Version 7.1 4/10/95
********************************************************************/
#include <graphic.h>
#include <lplt.h>

#define NO_PLANE 0
#define IN_CLEAR 1
#define TO_HIDE 2
#define EMERGE 4
#define HIDDEN 8
#define HD_TOP 16
#define HD_BOTTOM 32
#define NO_HIDE 0
#define REM_HIDDEN 1
#define DOT_BASE 2
#define HORIZ_SET 4

#if !GPC_CPP
#if GPC_UNX
extern int tkf_pipe[];
#endif

#if GPC_WIN
#include <gpcwin.h>
extern GPCWIN_DATA gpcwin_data;
#endif
#endif

/***********************************************************************
lineq(x0, y0, x1, y1) stores the vector plot information, in tektronix format,
in a buffer. The buffer is automatically dumped to disk when full and at the
close of a plotting session. It also puts the graph on the screen. Lines are
cropped at the edges of the graphic window
If xhair = 1, no information is put into the tkf file & lines are xor'ed
with the existing screen. If notekq = 1, no information is put into
the tkf file but lines are drawn normally.
**************************************************************/
/* lineq(x0, y0, x, y) calls ldrawq(). It is used for isolated line segments. */
void GCLASS lineq(int x0, int y0, int x1, int y1)
{
	lmoveq(x0, y0);
	ldrawq(x1, y1);
}
/**************************************************************/
/* The line drawing routine for unrotated variables. */
void GCLASS ldrawq(int x1, int y1)
{
	int temp;
	int xsav, ysav;
	int xsav3d, ysav3d;
	int x0, y0;
	int k, nvectors;
	int x0c, y0c, x1c, y1c;
	int xls, yls, xhs, yhs;
	int i, j = 0;
	G_PROCESS_ID

	if (wplane_flgq) {
		projected_lineq(x1, y1);
		return;
	}
	xsav = x1;
	ysav = y1;
	if (g_prntq.filedump == 0) {
		temp = cropq(x0q, y0q, x1, y1, &x0c, &y0c,
			&x1c, &y1c);
														/* Crop to border. */
		switch(temp) {
			case 1:				/* entering */
			case 3:				/* passes completely through */
				lmoveq(x0c, y0c);
				break;
			case 2:				/* exiting */
				break;
			case 4:				/* all out */
			case 5:				/* entering with end point on the line */
			case 6:				/* exiting with end point on the line */
				lmoveq(x1, y1);
				return;
		}
		x0 = x0c;
		y0 = y0c;
		x1 = x1c;
		y1 = y1c;

/* If windowing, we call cropq() again with the border-cropped segment.
This crop will set new values of x0q and y0q if the line passes into the
window from outside. */

		if (plot3q & REM_HIDDEN) {
			xsav3d = xsav;
			ysav3d = ysav;
			remhidq(x1, y1);
			x0q = xsav3d;
			y0q = ysav3d;
			return;
		}
		else if (win_numq > 0) {
											/* Recrop to exclude windows.*/
			xls = xl_cropq;
			yls = yl_cropq;
			xhs = xh_cropq;
			yhs = yh_cropq;
			nvectors = 1;
			k = 0;
			line_start[k].x = x0;	/* Initialize start and end points */
			line_start[k].y = y0;
			line_end[k].x = x1;
			line_end[k].y = y1;
			for (i = 0; i <= win_numq; i++)		/* Initialize penups to 0 */
				line_start[i].penup = 0;
			for (i = 0; i < win_numq; i++) {	/* For each window */
				if (win_structq[i].type == 0)
					continue;
				xl_cropq = win_structq[i].xl;
				yl_cropq = win_structq[i].yl;
				xh_cropq = win_structq[i].xh;
				yh_cropq = win_structq[i].yh;
			/* The number of vectors always starts at one but can increase
			if a line goes completely through a window. The following for
			loop checks all pieces which are generated against each
			window. */
				for (j = 0; j < nvectors; j++) {
					x0 = line_start[j].x;
					y0 = line_start[j].y;
					x1 = line_end[j].x;
					y1 = line_end[j].y;
					temp = cropq(x0, y0, x1, y1, &x0c, &y0c, &x1c, &y1c);

					/* Adjust the end points to reflect clipped values */
					switch (temp) {
						case 0:		/* Line is not visible */
							line_start[j].penup = 2;
							continue;
						case 1:
							line_end[j].x = x0c;
							line_end[j].y = y0c;
							break;
						case 2:
							line_start[j].x = x1c;
							line_start[j].y = y1c;
							line_start[j].penup = 1;
							break;
						case 3:
				/* Here the number of vectors gets increased. */
							line_end[j].x = x0c;
							line_end[j].y = y0c;
				/* Use k for the index so as not to over write earlier
				vector */
							k++;
							line_start[k].x = x1c;
							line_start[k].y = y1c;
							line_start[k].penup = 1;
							line_end[k].x = x1;
							line_end[k].y = y1;
							break;
						case 4:
						case 5:
							break;
						case 6:
							line_start[j].penup = 1;
							break;
					}
				}
				/* Set nvectors so that all vectors will
				   be clipped to the remaining windows */
				nvectors = k + 1;
			}
			for (i = 0; i < nvectors; i++) {
				if (line_start[i].penup == 2)
					continue;			/* completely obscured by a window */
				if (line_start[i].penup) {
					lmoveq(line_start[i].x, line_start[i].y);
					line_start[i].penup = 0;
				}
				x1 = line_end[i].x;
				y1 = line_end[i].y;
	  			ldraw2q(x1, y1);
			}
			xl_cropq = xls;
			yl_cropq = yls;
			xh_cropq = xhs;
			yh_cropq = yhs;
		}
		else
			ldraw2q(x1, y1);
	}
	else
		ldraw2q(x1, y1);
	x0q = xsav;
	y0q = ysav;
}
/*************************************************************
All vectors filter through this routine. gprosq() is called with
the vector ends. It is in gprosq() that information about the line
is put in the TKF file. gprosq() also calls the routine which processes
the line. */
void GCLASS ldraw2q(int x1, int y1)
{
	G_PROCESS_ID

	g_lq.penup = penup_flgq;
	if (penup_flgq) {
		totekq(x0q, y0q, 1);
		tkbfillq(cmd_strq);
		if (g_lq.action == 2) {	/* Doing a polyline */
			if (g_lq.font_char) {
				if (g_lq.npts > 2) {
					g_lq.xyp[g_lq.start_count].x =
						(g_lq.npts - g_lq.start_count) - 1;
					g_lq.start_count = g_lq.npts;
					g_lq.npts = newbufsq((PPGXYPAIRS)&g_lq.xyp, g_lq.npts);
				}
				g_lq.xyp[g_lq.npts].x = g_lq.pfx;
				g_lq.xyp[g_lq.npts].y = g_lq.pfy;
				g_lq.npts = newbufsq((PPGXYPAIRS)&g_lq.xyp, g_lq.npts);
			}
			else {
				if (g_lq.npts > 2) {

					x0q = g_lq.pfx;		/* preserve the new starting point. */
					y0q = g_lq.pfy;		/* .pf? are changed by the driver. */

					g_lq.xyp[0].x = g_lq.npts - 1;
					f_tableq(&g_lq);
					g_lq.xyp = (PGXYPAIRS)gpcrealloc((PGXYPAIRS)g_lq.xyp,
						64 * sizeof(struct GXYPAIRSQ));

					g_lq.pfx = x0q;		/* Restore the starting point. */
					g_lq.pfy = y0q;
				}
				g_lq.xyp[1].x = g_lq.pfx;
				g_lq.xyp[1].y = g_lq.pfy;
				g_lq.npts = 2;
			}
			g_lq.action = 1;
		}
	}
	totekq(x1, y1, 0);
	tkbfillq(cmd_strq);
	if (!g_lq.action)
		f_tableq(&g_lq);
	else {
		g_lq.xyp[g_lq.npts].x = g_lq.ptx;
		g_lq.xyp[g_lq.npts].y = g_lq.pty;
		g_lq.npts = newbufsq((PPGXYPAIRS)&g_lq.xyp, g_lq.npts);
	}
	g_lq.pfx = g_lq.ptx;
	g_lq.pfy = g_lq.pty;
	x0q = x1;
	y0q = y1;
	penup_flgq = 0;
}
/*************************************************************/
/*totekq(x, y) translates position on a MAXRESX x MAXRESY grid to
tektronix format and stores the code in a buffer. The buffer is dumped to
disk when full. The MAXRES grid size is defined in GRAPHIC.H */
/*********************************************************************/
void G_CDECL GCLASS totekq(int x, int y, char penup)
{
#define	MASK5	0x1f
#define	MASK3	0x07
	int hx, hy, ly, lsx, lsy;
	int i = 0, temp;
	int extra = 0;
	G_PROCESS_ID

	if (rotate_flgq & 1) {
		temp = x;
		x = y;
		y = MAXRESYMO - temp;
	}
	if (penup) {
		g_lq.pfx = x;
		g_lq.pfy = y;
	}
	else {
		g_lq.ptx = x;
		g_lq.pty = y;
	}
	if (notekq)
		return;
	if (g_lq.logical_mode)
		return;
	i = 0;
	if (penup) {	/* Start with a clean slate. */
		hxoldq = ( -1);
		hyoldq = ( -1);
		lyoldq = ( -1);
		lsxoldq = ( -1);
		extra = ( -1);
	}
	lsx = 0;
	lsy = 0;
	if (penup)
		cmd_strq[i++] = 29;	/* A GS */
	if ((lsx = (x & MASK3)) != lsxoldq) {
		lsxoldq = lsx;		/* If lsx has changed, save and flag. */
		extra = 1;
	}
	if ((lsy = (y & MASK3)) != lsyoldq) {
		lsyoldq = lsy;		/* If lsy has changed, save and flag. */
		extra = 1;
	}
	/* Get most significant 5 bits out of 13 */
	if ((hy = ((y >> 8) & MASK5) + 0x20) != hyoldq) {
		cmd_strq[i++] = (Uchar)hy; /* If new hy, send. */
		hyoldq = hy;
	}
	/********************************************
		The extra byte is arranged as follows:
		bit		7	6	5	4	3	2	1	0
				x0	1	1	y0	y2	y1	x2	x1
	*/
	if (extra) {
		cmd_strq[i++] = (Uchar)(0x60
			+ ((lsx & 1) << 7) + ((lsx & 6) >> 1)
			+ ((lsy & 1) << 4) + ((lsy & 6) << 1));
												/* If extra, assemble and */
	}
	/* Get most significant 5 bits out of 13 */
	hx = (x >> 8) + 0x20;			  	/* send. */
	if (((ly = ((y >> 3) & MASK5) + 0x60) != lyoldq) || hx != hxoldq || extra) {
		cmd_strq[i++] = (Uchar)ly;	  	/* If hx or extra or ly, send ly. */
		lyoldq = ly;
		extra = 0;
	}
	if (hx != hxoldq) {	/* If new hx, send it. */
		cmd_strq[i++] = (Uchar)hx;
		hxoldq = hx;
	}
	cmd_strq[i++] = (Uchar)(((x >> 3) & MASK5) + 0x40);
												/* Always send lx to finish. */
	cmd_strq[i] = '\0';
#undef	MASK5
#undef	MASK3
}
/***********************************************************************
remhidq() accepts the cropped line segment and tracks it in VIRTUAL space. It
tracks the segment pixel by pixel and resets y horizons. The routine has
been rewritten (3/91) to work properly with color-patch shading. The
previous version did not allow for complicated interaction between the
patch sides and the horizons. The new version adds auxiliary horizons to
allow vertical multi-pixel steps to be handled properly. There are two
upper horizons and two lower horizons. Pairs track together unless there
is a multi-pixel vertical step. Such a step lowers the auxiliary upper
horizon or the lower primary horizon on decent or raises the auxiliary
lower horizon or the primary upper horizon on ascent. A subsequent
traversal of the region between the two respective horizons is treated as
a contact with the horizon for all steps at that x value.
The need for the auxiliary horizons is felt on major y multiple steps.
If the y increment is positive, the main bottom or the auxiliary top
horizon is set, as appropriate, at the start of a constant-x rise. The
relationship to old horizons is checked after each single-pixel y
step. If a horizon is passed the status is changed. If the increment
is finished without reaching the old horizon, the other horizon is
set to the end of the step. The step may break through the opposite
horizon, in which case the opposite horizons must be adjusted.
Define status:
	HIDDEN	  -> lower than lower top and higher than upper bottom
	IN_CLEAR  -> at or above lower top or at or below upper bottom
For color patch operations, points at transition are marked:
	EMERGE	->	was hidden
	TO_HIDE	->	was clear - now hidden
Keep status of previous point.
Get status of first point. Call it old_status.
Determine new status.
If new status is HIDDEN and old status is not, if there has been no
change in x, update auxiliary horizons.
(The auxiliary horizons will be used in htraceq() for constructing
color patches.)
On a transition from HIDDEN, set line start at new point with attribute
EMERGE.
On a transition to HIDDEN, set line end at old point with attribute
TO_HIDE.
There are several major modes of operation of remhidq(). They are not
all mutually exclusive:
	1. Ignore horizons. (all bits off)			(plot3q == NO_HIDE(0))
	2. Use dotted lines to draw through hidden regions.
												(plot3q & DOT_BASE(2))
	3. Remove hidden lines.						(plot3q & REM_HIDDEN(1))
	4. Set horizons.							(plot3q & SET_HORIZ(4))
2/16/91 GGK
*/

#define UPPER_HORIZ(x) (((x) >= 0 && (x) < horizq.width && horiz_upper != NULL) ? (horiz_upper[x] & MAXRESXMO) : 0)
#define LOWER_HORIZ(x) (((x) >= 0 && (x) < horizq.width && horiz_lower != NULL) ? (horiz_lower[x] & MAXRESXMO) : MAXRESYMO)
#define Y_IS_BETWEEN_HORIZONS ((y < UPPER_HORIZ(x)) && (y > LOWER_HORIZ(x)))
#define Y_IS_ON_OR_ABOVE_UPPER(x,y) (y >= UPPER_HORIZ(x))
#define Y_IS_ON_OR_BELOW_LOWER(x,y) (y <= LOWER_HORIZ(x))
void GCLASS remhidq(int x1, int y1)
{
	int dltx, dlty, c, ca, cr, cr2, nc;	/* line tracing variables */
	static char trans_flag;				/* transition flag */
	int *horiz_upper;				/* local pointers to horizons */
	int *horiz_lower;
	int x, y;							/* current x,y pos in loop */
	int x3old, y3old;					/* previous x,y pos in loop */
	char plot3 = 0;						/* local variable for plot3q */
	int x3d, y3d;						/* used for drawing grid lines */
	char ymajor = 0;					/* is x/y less than 1 */
	int xinc, yinc;						/* step increaments 1 or -1 */
	int ystart;							/* save y pos for verticle segs */
	int low_y, high_y;
	G_PROCESS_ID

		/* DO 3-D INITIALIZATION */

	if (penup_flgq)
		trans_flag = HIDDEN;
	x1 -= horizq.offset;
	x1 /= 2;
	x = (x0q - horizq.offset) / 2;
	y = y0q;
	x3old = x;
	y3old = y;
	x3d = x;
	y3d = y;
	plot3 = plot3q;
	horiz_upper = horizq.upper;
	horiz_lower = horizq.lower;
	if (plot3 & DOT_BASE) {
		x3d = x;
		y3d = y;
	}

		/* INITIALIZE LINE DRAWING ROUTINE */

	xinc = sign(x1 - x);		/* Increments for steps. */
	yinc = sign(y1 - y);
	dltx = gabs(x1 - x);
	dlty = gabs(y1 - y);
	nc = (dltx + dlty);			/* Number of steps in segment. */
	if (!nc)
		return;
	if (dlty > dltx) {
		c = dltx << 1;
		cr = dlty;
		ymajor = 1;						/* Y is major step. */
	}
	else {
		c = dlty << 1;
		cr = dltx;
	}
	cr2 = cr << 1;
	ca = 0;					/* save for reset */

		/* TRACE LINE IN PIXEL SPACE */

	if (x < 0) {
		lmoveq((x * 2) + horizq.offset, y);
		trans_flag = IN_CLEAR;
	}
	ystart = y;
	for (;;) {

		/* 3-D HIDDEN LINE REMOVAL - SEGMENTS BELOW UPPER AND ABOVE LOWER
			HORIZONS ARE NOT PLOTTED. */

		if (x >= 0 && x < horizq.width) {
			if (Y_IS_BETWEEN_HORIZONS) {	/* obscured */
				if (trans_flag & IN_CLEAR) {
					/* Wasn't obscured now is */
					if (Y_IS_ON_OR_ABOVE_UPPER(x3old, y3old))
						trans_flag |= HD_TOP;
					else
						trans_flag &= ~HD_TOP;
					if (Y_IS_ON_OR_BELOW_LOWER(x3old, y3old))
						trans_flag |= HD_BOTTOM;
					else
						trans_flag &= ~HD_BOTTOM;
					if (plot3 & DOT_BASE) {
						x3d = x3old;
						y3d = y3old;
					}
					lfinishq((x3old * 2) + horizq.offset, y3old);	/* Draw visible segment */
				}
				trans_flag &= ~IN_CLEAR;
				trans_flag |= HIDDEN;		/* Indicates obscured */
			}
			else {
				if (d3_clrflgq)
					horizcq[x] = y;
				if (Y_IS_ON_OR_ABOVE_UPPER(x, y))
					trans_flag |= HD_TOP;
				else
					trans_flag &= ~HD_TOP;
				if (Y_IS_ON_OR_BELOW_LOWER(x, y))
					trans_flag |= HD_BOTTOM;
				else
					trans_flag &= ~HD_BOTTOM;
				if (trans_flag & HIDDEN) {
					/* Isn't obscured but was */
					if (plot3 & DOT_BASE) {
						lmoveq((x3d * 2) + horizq.offset, y3d);
						/* draw dotted line under surface */
						gridlinq((x * 2) + horizq.offset, y);
						x3d = x;
						y3d = y;
					}
					lmoveq((x * 2) + horizq.offset, y);	/* Start new segment */
				}
				trans_flag &= ~HIDDEN;
				trans_flag |= IN_CLEAR;	/* Indicates not obscured */
			}
		}
		x3old = x;
		y3old = y;

			/* TAKE A STEP IN PIXEL SPACE */

		if (nc-- <= 0)
			break;
		if (ymajor)
			y += yinc;					/* Take a major step. */
		else
			x += xinc;

		ca += c;
		if (ca > cr || (xinc < 0 && ca == cr)) {
			ca -= cr2;			/* if appropriate take a minor step.*/
			if (ymajor)
				x += xinc;
			else
				y += yinc;
			nc--;
		}
		if ((plot3 & HORIZ_SET) && x != x3old) {
			if (ystart > y3old) {
				high_y = ystart;
				low_y = y3old;
			}
			else {
				high_y = y3old;
				low_y = ystart;
			}
			if (x3old >= 0 && x3old < horizq.width && horizq.upper != NULL) {
				if (trans_flag & HD_TOP) {
					if (high_y > (horizq.upper[x3old] & MAXRESXMO))
						horizq.upper[x3old] = high_y;
				}
				if (trans_flag & HD_BOTTOM) {
					if (low_y < (horizq.lower[x3old] & MAXRESXMO))
						horizq.lower[x3old] = low_y;
				}
			}
			ystart = y;
		}
	}
	if (trans_flag & (IN_CLEAR | EMERGE)) {
		if (d3_clrflgq && x1 >= 0 && x1 < horizq.width && horizcq != NULL)
			horizcq[x1] = y1;
		lfinishq((x1 * 2) + horizq.offset, y1);
	}
	else if (plot3 & DOT_BASE) {
		lmoveq((x3d * 2) + horizq.offset, y3d);
		gridlinq((x1 * 2) + horizq.offset, y1);
	}
	x0q = (x1 * 2) + horizq.offset;
	y0q = y1;
}
/*************************************************************/
/* y-values are compared with horizcq[] to determine the color.
If y > horizcq[] line is top color. y<horizcq[] line is bottom
color. If the line is on the edge ?clrflgq determines the color. */
void GCLASS lfinishq(int x, int y)
{
	int wplansav;
	char plot3sav;
	G_PROCESS_ID

	wplansav = wplane_flgq;
	wplane_flgq = NO_PLANE;
	if (surface_clrq) {	/* user selected top and bottom colors */
		if ((d3_clrflgq & SET_BOTTOM_COLOR) ||
			(!(d3_clrflgq & SET_TOP_COLOR) &&
				y < horizcq[(x - horizq.offset) >> 1])) {
			if (g_lq.color != (int)BOTTOM_SURFACE_COLOR) {
				if (g_lq.npts > 2) {
					g_lq.xyp[0].x = g_lq.npts - 1;
					f_tableq(&g_lq);
					g_lq.xyp = (PGXYPAIRS)gpcrealloc((PGXYPAIRS)g_lq.xyp,
						64 * sizeof(struct GXYPAIRSQ));
					g_lq.npts = 1;
					lmoveq(x0q, y0q);
				}
				color(BOTTOM_SURFACE_COLOR);
			}
		}
		else if ((d3_clrflgq & SET_TOP_COLOR) ||
			(!(d3_clrflgq & SET_BOTTOM_COLOR) &&
				y > horizcq[(x - horizq.offset) >> 1])) {
			if (g_lq.color != TOP_SURFACE_COLOR) {
				if (g_lq.npts > 2) {
					g_lq.xyp[0].x = g_lq.npts - 1;
					f_tableq(&g_lq);
					g_lq.xyp = (PGXYPAIRS)gpcrealloc((PGXYPAIRS)g_lq.xyp,
						64 * sizeof(struct GXYPAIRSQ));
					g_lq.npts = 1;
					lmoveq(x0q, y0q);
				}
				color(TOP_SURFACE_COLOR);
			}
		}
		d3_clrflgq = 0;
	}
	plot3sav = plot3q;
	plot3q = 0;
	ldrawq(x, y);
	plot3q = plot3sav;
	wplane_flgq = wplansav;;
}
/*****************************************************************/
/* This routine takes 2-D pixels and translates them to a 3-D coordinate
system and calls d3lineq() which then transforms the 3-D points to the
correct view port coordinates and calls ldraw2q() to draw the line. The
arguments to the call to d3lineq() are: coordinate of right side	from
										coordinate of left side		from
										coordinate of z				from
										coordinate of right side	to
										coordinate of left side		to
										coordinate of z				to.
V1_? represents the right side values.
V2_? represents the left side values.
Input coordinates are in MAXRESX, MAXRESY space.
The projection plane determines to which of the three from,to coordinates
x and y map. Axes label projections are a special case since we must
ensure readability. Therefore we have the in_plbl3q flag to tell us how to
handle the pixel transformation for fonts. */

void GCLASS projected_lineq(int x1, int y1)
{
	float v1_to, v2_to, z0, z1, tempf;
	static float v1_from = (float)0, v2_from = (float)0;
	G_PROCESS_ID

	if (penup_flgq) {
		v1_from = (float)x0q;
		v2_from = (float)y0q;
	}
	v1_to = (float)x1;
	v2_to = (float)y1;
	if (wplane_flgq & V1_V2_PLANE) {			/* x-y plane */
		if (!in_plbl3q) {
			v1_from *= (float)d3_convertq.xview;
			v2_from *= (float)d3_convertq.yview;
			v1_to *= (float)d3_convertq.xview;
			v2_to *= (float)d3_convertq.yview;
			if (viewers_quadq & (SECOND_QUAD | FORTH_QUAD)) {
			/* Y is along the right axis so we need to exchange
			   the x and y coordinates. */
				tempf = v1_from;
				v1_from = v2_from;
				v2_from = tempf;
				tempf = v1_to;
				v1_to = v2_to;
				v2_to = tempf;
			}
		}
		z0 = z1 = axposq;
	}
	else if (wplane_flgq & V1_Z_PLANE) {
	/* The y coordinates map to the z axis and the perpindicular plane
	   constant maps to y. */
		if (!in_plbl3q) {
			v1_from *= (float)d3_convertq.xview;
			v1_to *= (float)d3_convertq.xview;
		}
		z0 = v2_from;
		z1 = v2_to;
		v2_from = v2_to = axposq;
	}
	else {	/* v2 plane, x is parallel to left axis x goes to v2 */
		z0 = v2_from;
		z1 = v2_to;
		if (!in_plbl3q) {
			v1_from *= (float)d3_convertq.xview;
			v1_to *= (float)d3_convertq.xview;
		}
		v2_from = v1_from;
		v2_to = v1_to;
		v1_from = v1_to = axposq;
		if (in_plbl3q) {
			v2_from = -v2_from;
			v2_to = -v2_to;
			v2_from += (float)xlenq;
			v2_to += (float)xlenq;
		}
	}
	/* Calls ldraw2q or loads panel. */
	d3lineq(v1_from, v2_from, z0, v1_to, v2_to, z1);
	v1_from = (float)x1;
	v2_from = (float)y1;
	if (penup_flgq) {
		x0q = x1;
		y0q = y1;
	}
}
/*************************************************************
Accepts 3-d floating pixels and passes 2-d window variables to lineq. */
void GCLASS d3lineq(float v10, float v20, float z0, float v1, float v2, float z)
{
	int xsc, ysc;
	float x, y;
	static int xsav = -1, ysav = -1;
	int wplansav;
	G_PROCESS_ID

	wplansav = wplane_flgq;
	wplane_flgq = NO_PLANE;
	getxyq(v10, v20, &x, &y, PIXEL_UNITS);	/* getxy() for workbox units */
	d3convq(x, y, z0, &xsc, &ysc);
	if (xsav != xsc || ysav != ysc || penup_flgq) {
		if (panfilq) {
			g_pq.xyp[g_pq.nsides].x = xsc;
			g_pq.xyp[g_pq.nsides].y = ysc;
			g_pq.nsides = newbufsq((PPGXYPAIRS)&g_pq.xyp, g_pq.nsides);
		}
		lmoveq(xsc, ysc);
	}
	getxyq(v1, v2, &x, &y, PIXEL_UNITS);
	d3convq(x, y, z, &xsc, &ysc);
	if (panfilq) {
		g_pq.xyp[g_pq.nsides].x = xsc;
		g_pq.xyp[g_pq.nsides].y = ysc;
		g_pq.nsides = newbufsq((PPGXYPAIRS)&g_pq.xyp, g_pq.nsides);
		x0q = xsc;
		y0q = ysc;
		penup_flgq = 0;
	}
	else {
		ldrawq(xsc, ysc);
	}
	xsav = xsc;
	ysav = ysc;
	wplane_flgq = wplansav;
}
/**************************************************************
This function does the conversion from sx, sy, sz coordinates in scaled workbox
units to the window coordinates. sx, sy and sz are the user's x, y, and z
scaled to the lengths of the workbox axes and shifted to have zero at
the origin. */
void GCLASS d3convq(float x, float y, float z, int *xp, int *yp)
{
	float exx, eyy, ezz, zdrawi;
	G_PROCESS_ID

	exx = d3_convertq.xvu - x;
	eyy = d3_convertq.yvu - y;
	ezz = d3_convertq.zvu - z;
	zdrawi = d3_convertq.d1 / (d3_convertq.x3conv * exx + d3_convertq.y3conv
		* eyy + d3_convertq.z3conv * ezz);
	*xp = (int)(d3_convertq.d2 * (d3_convertq.y3conv * exx - d3_convertq.x3conv
		* eyy) * zdrawi) + d3_convertq.xpos;
	*yp = (int)((d3_convertq.z3conv * (d3_convertq.x3conv * exx +
		d3_convertq.y3conv * eyy) - d3_convertq.d12 * ezz)
		* zdrawi) + d3_convertq.ypos;
}
/*************************************************************
Rounding for 3-d labels. User units. */
void GCLASS d3rndq(float orig, float step, float max,
	int *pint, int *pdec, char *format)
{
	double temf;
	long temp;

	temf = log10((double)step);

	if (temf < 0)	/* step size is < 1. */
		*pdec = (int)(temf - .0000001);
	else			/* step size is >= 1. */
		*pdec = (int)(temf + .0000001);

	if (*pdec > 0)	/* step size is >= 1. */
		*pint = (*pdec + 1);
	else
		*pint = ((int)step >= 1) ? 1 : 0;
	*pdec = 5 - *pdec;
	temf = (*pdec);
	temp = (long)(pow(10., temf) * step * 1.000001);
	while (((temp / 10) * 10) == temp) {
		temp = temp / 10;
		(*pdec)--;
	}
	if (*pdec < 0)
		*pdec = 0;
	if (*pdec) {
		if ((*pdec > 2) && (*pint == 0))
			sprintfq(format, "%%-1.3e");
		else
			sprintfq(format, "%%-1.%df", *pdec);
	}
	else if (*pint > 4) {
		if (max > (float)65535. && orig > (float)65535.)
			strcpy(format, "%-ld");
		else if (max > (float)32766. && orig > (float)32766.)
			strcpy(format, "%-u");
	}
	else
		strcpy(format, "%-1.0f");
}
/*********************************************************************/
void GCLASS gridlinq(int x, int y)
{
	Uchar plotsav;
	int dashsav;
	G_PROCESS_ID

	plotsav = plot3q;
	dashsav = g_lq.pattern;
	dashf(2);
	plot3q = NO_HIDE;
	ldraw2q(x, y);
	plot3q = plotsav;
	dashf(dashsav);
}
/*****************************************************************************/
void GCLASS tkbfillq(char *string)
{
	int len, error;
	unsigned char action;
	char ofstr[128], ch, *sptr, *optr;
	long ofposq;
	char *aptr;
	G_PROCESS_ID

#if GPC_WIN
	/* Background processing for Windows */
	if (tkbcntq >= 100) {
		tkbcntq = 0;
		GPCWINYieldControl();
	}
	tkbcntq++;

	if (bitbflgq){/* bitblt to screen if covered but not minimized */
		if (tkbcnt2q >= Wupdatfreq )	{
			tkbcnt2q = 0;
			if (!IsIconic(g_iq[VIDEO].hWnd)) {
				g_rdq.FromBitmap = 1;	/* Redraw using bitmap */
				f_tableq(&g_rdq);
			}
		}
		tkbcnt2q++;
	}

#endif
	action = (unsigned char)*string;
	switch (action) {
		case OPEN_TEK_FILE:
			if (notekq) return;
			string++;
			tkf_pointerq = gfopen(string, "w+b", TKBUFFSIZE);
			getfilenameq(string);
			tofileq = NULL; /* Obsolete .OFF offset files disabled */
		return;

		case CLOSE_TEK_FILE:
			if (tkf_pointerq != NULL) {
				error = gfclose(tkf_pointerq);
				tkf_pointerq = NULL;
				if (error == ERR)
					messageq(WARNQ, 1);	/* Error closing TKF file */
			}
			if (tofileq != NULL) {
				error = gfclose(tofileq);
				tofileq = NULL;
			}
		return;

		case DUMP_TEK_FILE:
			fflush(tkf_pointerq);			/* force a disk dump */
		return;

		case SEEK_TEK_FILE:
			fseek(tkf_pointerq, 0L, SEEK_END);
			tkf_offsetq = ftell(tkf_pointerq);
			tkf_incrementq = 0;
		return;

		case MARK_TEK_FILE:
			if (tofileq != NULL) {
				ofposq = ftell(tkf_pointerq) + 1L;
				aptr = longsplitq(ofposq);
				error = fwrite(aptr, sizeof(char), 4, tofileq);
			}
		return;

		case END_TKF_PLOT:
			if (g_lq.thick != 0) {
				strcpy(cmd_strq, "\33GU");
				gprosq(2);
			}
			cmd_strq[1] = 0;
			cmd_strq[0] = 29;				/* Penup */
			gprosq(0);
			cmd_strq[0] = 7;                /* Bell */
			gprosq(0);
			if (!notekq) {
				tkbfillq("");	/* padding between pictures */
				fflush(tkf_pointerq);			/* force a disk dump */
			}
		return;
	}
	if (notekq)
		return;
	len = strlen(string);

	error = fwrite(string, sizeof(char), len, tkf_pointerq);
	if (error != len)
		messageq(WARNQ, 29);		/* Error writing TKF file */
#if GPC_UNX
#ifdef TKF_OUT
	fputs(string, stdout);
#else
	write(tkf_pipe[1], string, len);
#endif
#endif
	tkf_incrementq += (long)len;
	SET_LPLOTQ;
}
/*************************************************************
Window in inch(cm) units.  Prevents writing in the area bounded by x0, y0
at the lower left and x1, y1 at the upper right as viewed with the picture
upright. */
void TYPE GCLASS gwindow(float x0, float y0, float x1, float y1)
{
	G_PROCESS_ID

	windowq(intopix(x0), intopix(y0), intopix(x1), intopix(y1));
}
/*************************************************************
Window in pixel units. */
void GCLASS windowq(int x0, int y0, int x1, int y1)
{
	G_PROCESS_ID

	if (x0 < 0 || y0 < 0 || y1 > topq || x1 > acrossq) {
		messageq(WARNQ, 15);		/* Window too big, reduced. */
		if (x0 < 0)
			x0 = 0;
		if (y0 < 0)
			y0 = 0;
		if (x1 > acrossq)
			x1 = acrossq;
		if (y1 > topq)
			y1 = topq;
	}
	win2q(x0, y0, x1, y1, GPC_IS_GWIN);
}

/********************************************************************
GPCClipRectangle() defines a rectangle which will be protected against
drawing. It is intended to allow a user to protect some portion of the
plot from being drawn over. This call must come AFTER the portion of
the plot that is to be protected is complete.
********************************************************************/
void TYPE GPCClipRectangle(float x0, float y0, float x1, float y1,
	int units, int border)
{
	G_PROCESS_ID

	switch (units) {
		case USER_UNITS:
			{int ix0, iy0, ix1, iy1;
			usertopix(x0, y0, 0.f, &ix0, &iy0);
			usertopix(x1, y1, 0.f, &ix1, &iy1);
			win2q(ix0, iy0, ix1, iy1, border);}
		break;
		case INCH_UNITS:
			win2q(intopix(x0), intopix(y0), intopix(x1),
				intopix(y1), border);
		break;
		case PIXEL_UNITS:
			win2q(intopix(x0),
				intopix(y0),
				intopix(x1),
				intopix(y1),
				border);
		break;
	}
	/* Any other value for units is an error so
	   the function does nothing. */
}
/**************************************************************/
/* Load the window structure with window sides and type. */
void GCLASS win2q(int x0, int y0, int x1, int y1, int type)
{
	int i;
	struct G_WIN *ws;
	struct LINESTART *ls;
	PGXYPAIRS le;
	G_PROCESS_ID

	win_numq++;                      /* win_numq accumulates # of windows. */
	ws = (struct G_WIN *)gpcrealloc(win_structq,
		win_numq * sizeof(struct G_WIN));
	ls = (struct LINESTART *)gpcrealloc(line_start,
		(win_numq + 1) * sizeof(struct LINESTART));
	le = (PGXYPAIRS)gpcrealloc(line_end,
		(win_numq + 1) * sizeof(struct GXYPAIRSQ));
	if (ws == NULL || ls == NULL || le == NULL) {
		win_numq--;
		messageq(ERRQ, 17);    /* Not enough memory for window */
		return;
	}
	win_structq = ws;
	line_start = ls;
	line_end = le;
	i = win_numq - 1;
	win_structq[i].type = type;                      /* load window structure */
	win_structq[i].xl = x0;
	win_structq[i].yl = y0;
	win_structq[i].xh = x1;
	win_structq[i].yh = y1;
	win_structq[i].border_color = g_lq.color;
}
/*********************************************************************/
/* Draws borders around all windows that were not called from gwindow().
It is checked every time in endplot(), and any windows that are processed
there are set to type "0". When all windows have been processed, win_numq
is set to 0. */
void GCLASS win_boxq(void)
{
	int x0, y0, x1, y1;
	int i, j, type;
	G_PROCESS_ID

	dashf(1);
	for (i = win_numq - 1; i >= 0; i--) {
		type = win_structq[i].type;
		if (type != GPC_IS_GWIN) {				/* Proccess this window */
			win_structq[i].type = 0;
			if (type == GPC_HAS_BORDER) {		/* Draw box */
				x0 = win_structq[i].xl;
				y0 = win_structq[i].yl;
				x1 = win_structq[i].xh;
				y1 = win_structq[i].yh;
				color(win_structq[i].border_color);
				lmoveq(x0, y0);
				ldrawq(x0, y1);
				ldrawq(x1, y1);
				ldrawq(x1, y0);
				ldrawq(x0, y0);
			}
		}
	}
	for (i = 0, j = 0; i < win_numq; i++) {
		type = win_structq[i].type;
		if (type == GPC_IS_GWIN)
		/* Move un-proccessed windows to beginning of array */
			G_MEMCPY((void *)&win_structq[j++],
				(void *)&win_structq[i], sizeof(struct G_WIN));
	}
	win_numq = j;
	if (win_numq) {
		win_structq = (struct G_WIN *)gpcrealloc(win_structq,
			win_numq * sizeof(struct G_WIN));
		line_start = (struct LINESTART *)gpcrealloc(line_start,
			(win_numq + 1) * sizeof(struct LINESTART));
		line_end = (PGXYPAIRS)gpcrealloc(line_end,
			(win_numq + 1) * sizeof(struct GXYPAIRSQ));
	}
	else {
		gpcfree((void **) &win_structq);
		gpcfree((void **) &line_start);
		gpcfree((void **) &line_end);
	}
}
/*************************************************************
Returns TEK code. */
void GCLASS encodeq(int value, char *resp)
{
	int temp, i = 0, flag = 0;
	if (value < 0) {
		flag = 1;
		value = ( -value);
	}
	if ((temp = ((Uint)value & 0xfc00) >> 10) > 0)
		*(resp + i++) = (char)(temp + 64);
	if ((temp = ((Uint)value & 0x3f0) >> 4) > 0)
		*(resp + i++) = (char)(temp + 64);
	*(resp + i) = (char)(((Uint)value & 0xf) + 32);
	if (!flag)
		*(resp + i) += (char)16;
	i++;
	*(resp + i) = 0;
	return;
}
/************************************************************/
/* Take a long and return a 4-byte array that contains
the bytes from lowest signifigant to highest */
char * GCLASS longsplitq(long num)
{
	int i;
	static char array[4];

	for(i = 0; i < 4; i++) {
		array[i] = (char)(num & 0x000000ff);
		num = num >> 8;
	}
	return(array);
}
/********************************************************************
The following two routines allow the user to select a point in the
sequence of GraphiC calls to which he can return and have the TKF file
pointer reset. ResetMark() sets a variable to the current file position.
ResetPlot() rewinds the file pointer to the location saved by ResetMark().
These routines allow programs to draw and erase portions of plots without
the information being stored in the TKF file.
********************************************************************/
void TYPE TKFSegmentStart(void)
{
	G_PROCESS_ID

	tkf_pointer_saveq = tkf_pointerq;
	tkf_pointerq = gfopen("tmp_tkf_file", "w+b", TKBUFFSIZE);
	tkf_incrementq = 0;
}
void TYPE TKFSegmentEnd(int action)
{
	FILE *temp_file;
	unsigned int nbytes;
	char *buffer;
	G_PROCESS_ID

	temp_file = tkf_pointerq;
	tkf_pointerq = tkf_pointer_saveq;
	if (action == SAVE_SEGMENT) {
		nbytes = (unsigned int)ftell(temp_file);
		buffer = (char *)gpcalloc(nbytes, sizeof(char));
		fseek(temp_file, 0, SEEK_SET);
		fread(buffer, sizeof(char), nbytes, temp_file);
		fwrite(buffer, sizeof(char), nbytes, tkf_pointerq);
	}
	gfclose(temp_file);
	remove("tmp_tkf_file");
	tkf_incrementq = ftell(tkf_pointerq);
}
