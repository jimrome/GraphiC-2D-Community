/* Copyright 1984-1993 by Scientific Endeavors Corporation.
   All rights reserved.
This file contains cropq, which is required both by tek and the line drawing
routines.
Version 7.0 3/20/92
*******************************************************************/
#include <graphic.h>
#include <bothplt.h>

/**************************************************************/
/* Sets the global variables x0q and y0q for the beginning
   of the line segment. */
void GCLASS lmoveq(int x0, int y0)
{
	G_PROCESS_ID

	if (sim_flgq) {
		if (x0q == x0 && y0q == y0) {
			if (g_lq.action)
				g_lq.action = 2;
			return;
		}
	}
	x0q = x0;
	y0q = y0;
	penup_flgq = 1;
	if (g_lq.action)
		g_lq.action = 2;
}
/***********************************************************************
Cropq() clips the plot so that only the portion in the graphic window
is sent to the screen buffer and to the tek converter.
Cropq() has been rewritten to use bit coded masks for line end and root
information. 7/26/91 GGK.
Crop returns:
		0 if both ends in bounds
		1 if start is outside and end is inside
		2 if end is outside and start is inside
		3 both are outside but segment crosses window
		4 both are outside  -- no intercepts
		5 first point outside second point on border (needed to make penup)
		6 first point on border and second point outside.
*/
#define S_MASK 0x8000				/* Masks the sign bit. */
#define TOP_MASK 0X8				/* Mask the proper bits of start_out, */
#define BOT_MASK 0X4				/* end_out and root. */
#define RT_MASK 0X2
#define LEFT_MASK 0X1

int GCLASS cropq(int x0, int y0, int x1, int y1, int *x0p, int *y0p,
	int *x1p, int *y1p)
{
	int temp;
	int xl;
	int yl;
	int xh;
	int yh;
	int low_x, low_y, high_x, high_y;
	Uchar start_out = 0, end_out = 0, crosses, need_one=0;
	Uchar root = 0, invert = 0;
	int ret_value = 0;
	G_PROCESS_ID

	xl = xl_cropq;
	yl = yl_cropq;
	xh = xh_cropq;
	yh = yh_cropq;
	*x0p = x0;				/* Tenatively -- */
	*y0p = y0;
	*x1p = x1;
	*y1p = y1;

/*  CONSTRUCT OUTCODES FOR THE VECTOR ENDS. */

	start_out |= (char)(((yh - y0) & S_MASK) >> 12);	/* Above the top */
	start_out |= (char)(((y0 - yl) & S_MASK) >> 13);	/* Below the bottom */
	start_out |= (char)(((xh - x0) & S_MASK) >> 14);	/* To the right */
	start_out |= (char)(((x0 - xl) & S_MASK) >> 15);	/* To the left */
	end_out |= (char)(((yh - y1) & S_MASK) >> 12);	/* Above the top */
	end_out |= (char)(((y1 - yl) & S_MASK) >> 13);	/* Below the bottom */
	end_out |= (char)(((xh - x1) & S_MASK) >> 14);	/* To the right */
	end_out |= (char)(((x1 - xl) & S_MASK) >> 15);	/* To the left */

/* GET OUT FAST WHEN POSSIBLE. */

	if (!(start_out | end_out))					/* Both ends inside */
		return(0);
	if (start_out & end_out)	/* Both ends are above, below, rt. or left. */
		return(4);							/* No window intersection */

/*  IF CAN'T GET OUT FAST, FIND THE BORDER INTERSECTIONS. */

/* Note: rootq() has fast return for perpendicular crossings and for
points on the axis. No special horizontal or vertical processing needed.
*/
	crosses = (Uchar)(start_out ^ end_out);/* 'On' bits signal a crossing. */
	if (!start_out || !end_out)
		need_one = 1;				/* flag for needing only one root */

	if (crosses & LEFT_MASK) {		/* Line crosses left border. */
			root |= (Uchar)rootq(y0, x0, y1, x1, xl, yl, yh, &low_y);
		if(root && need_one)
			goto root_skip;					/* Need only one root. */
	}
	if (crosses & RT_MASK) {
		root |= (Uchar)(rootq(y0, x0, y1, x1, xh, yl, yh, &high_y) << 1);
		if (root > 2 || (root && need_one))	/* Have found the roots. */
			goto root_skip;
	}
	if (crosses & BOT_MASK) {
		root |= (Uchar)(rootq(x0, y0, x1, y1, yl, xl, xh, &low_x) << 2);
		if (root > 4 || (root && need_one))
			goto root_skip;
	}
	if (crosses & TOP_MASK)
		root |= (Uchar)(rootq(x0, y0, x1, y1, yh, xl, xh, &high_x) << 3);

root_skip:

	if (!root)					/* There are no window crossings. */
		return(4);
	if (!need_one && ((root_countq(root)) < 2))
		return(4);

		/* LINES TO BE CLIPPED */

/* Note: Use *x0p,*y0p for first crossing. Fix later if wrong way. */

	if (!start_out) 			/* Start is inside - *x0p & *y0p already set. */
		ret_value = 2;
	else if (!end_out) {			/* End is inside. */
		*x0p = x1;					/* Always assume first end is start. */
		*y0p = y1;
		invert = 1;				/* Flag to fix wrong assumption later */
		ret_value = 1;
	}
	if (root & LEFT_MASK) {		/* Left side of window is crossed. */
		if (ret_value > 0) {		/* Already have one root. */
			*x1p = xl;			/* Could be wrong end, but will be fixed. */
			*y1p = low_y;
			goto crop_ret;		/* One point is in - ret_value set above. */
		}
		*x0p = xl;					/* Assume start at left. */
		*y0p = low_y;
		if (x0 > x1)
			invert = 1;			/* Start not at left - fix later. */
		ret_value = 3;			/* First is out - both must be out. */
	}
	if (root & BOT_MASK) {
		if (ret_value > 0) {
			*x1p = low_x;
			*y1p = yl;
			goto crop_ret;
		}
		*x0p = low_x;				/* Assume start at bottom. */
		*y0p = yl;
		if (y0 > y1)
			invert = 1;				/* Start not at bottom */
		ret_value = 3;
	}
	if (root & RT_MASK) {
		if (ret_value > 0) {
			*x1p = xh;
			*y1p = high_y;
			goto crop_ret;
		}
		*x0p = xh;					/* Assume start at right. */
		*y0p = high_y;
		if (x0 < x1)
			invert = 1;				/* Start not at right */
		ret_value = 3;
	}
	if (root & TOP_MASK) {			/* Must already have first root. */
		*x1p = high_x;
		*y1p = yh;
	}

crop_ret:

	if (invert) {				/* Fix for wrong start assumption. */
		temp = *x0p;
		*x0p = *x1p;
		*x1p = temp;
		temp = *y0p;
		*y0p = *y1p;
		*y1p = temp;
	}

	if (*x0p == *x1p && *y0p == *y1p)	/* Single point - must be border. */
		if (ret_value < 3)				/* Could be corner point! */
			return(4 +ret_value);		/* Returns 5 if start is out. */

	return(ret_value);
}
/*************************************************************
Calculates all intercepts. Choice of arguments adapts for either a horizontal
or a vertical axis. Modified to treat border points 7/26/91. GGK
*/
Uchar GCLASS rootq(int a, int b, int c, int d, int linerq, int min, int max,
int *rp)
{
	long delac, delbd;

	delac = (long)c - a;					/* delta 'x' */
	delbd = (long)d - b;					/* delta 'y' */
	if (delbd == 0)
		return(0);				/* Segment parallel to 'x' axis */
	if (delac == 0							/* Crosses 'x' axis at rt angle */
		|| linerq == b) {						/* or point is on axis. */
		if (min <= a && a <= max) {		/* Crossing is in range. */
			*rp = a;
			return(1);
		}
		return(0);
	}
	*rp = (int)(a + (((linerq - b) * delac) + (delbd / 2)) / delbd);
														/* Get intercept. */
	if (min <= *rp && *rp <= max) {	/* Is it in bounds? */
		return(1);
	}
	return(0);
}
int GCLASS root_countq(int root)
{
	int i = 0;
	if (root & 1)
		i++;
	if (root & 2)
		i++;
	if (root & 4)
		i++;
	if (root & 8)
		i++;
	return(i);
}
/************************************************************/
/* ROTATE PRINTED PAGE */
void TYPE GCLASS rotate(
	char isrot	/* 0 = upright (landscape);  1 = rotated (portrait) */
)
{
	G_PROCESS_ID

	if (isrot) {
		rotate_flgq = 1;
		topq = yh_cropq = MAXRESXMO;
		acrossq = xh_cropq = MAXRESYMO;
	}
	else {
		rotate_flgq = 0;
		topq = yh_cropq = MAXRESYMO;
		acrossq = xh_cropq = MAXRESXMO;
	}

#if GPC_GUI
#if GPC_OS2
	print_dataq.Portrait_mode = isrot;
#endif
	rotate_flgq = 0;
	if (isrot && g_iq[VIDEO].hmax > g_iq[VIDEO].vmax) {
		int temp;
		temp = g_iq[VIDEO].xpos;
		g_iq[VIDEO].xpos = g_iq[VIDEO].ypos;
		g_iq[VIDEO].ypos = temp;
		temp = g_iq[VIDEO].hmax;
		g_iq[VIDEO].hmax = g_iq[VIDEO].vmax;
		g_iq[VIDEO].hmax = temp;
	}
	strcpy(cmd_strq, "\33IV00");
	tkbfillq(cmd_strq);
	totekq(0, 0, 1);
	tkbfillq(cmd_strq + 1);
	totekq(acrossq, topq, 1);
	tkbfillq(cmd_strq + 1);
	g_page_sizeq.xRight = acrossq;
	g_page_sizeq.yTop = topq;
	f_tableq(&g_page_sizeq);
#endif
	if (isrot)
		rotate_flgq |= 2;
}


/***************************************************************************
 * GPCSetBeepDefaults(int frequency, int duration)                         *
 *                                                                         *
 * This function set the default beep frequency and duration for           *
 * the end of plot bell. The defaults are set initially by initialq        *
 * in bplt?.c. Each time bgnplot is called the defaults will be reset      *
 * to the values defined in GRAPHIC.H.                                     *
 * The following lines in GRAPHIC.H set the defaults:                      *
 * #define GPC_BEEP_FREQUENCY 440                                          *
 * #define GPC_BEEP_DURATION 50                                            *
***************************************************************************/
void TYPE GCLASS GPCSetBeepDefaults(int frequency, int duration)
{
	G_PROCESS_ID

	beep_frequencyq = frequency;
	beep_durationq = duration;
}
