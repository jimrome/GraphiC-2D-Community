/* (c) 1987-1993 by Scientific Endeavors Corporation.
   All rights reserved.
Version 7.0 7/16/1993
****************************************************************/
/* This module contains the routines for processing panels. The method is:
1)  Store all of the vectors for a given panel and it's holes in a self
    counting array.
2)  After the panel definition is finished, clip the panel against all of
    the clipping boundaries.
3)  If the panel exceeds the limit allowed by the output device, split the
    panel in half repeatedly until it is small enough.
4)  Send the panel to the device.
****************************************************************/
#include <graphic.h>
#include <pclp.h>
#ifndef max
#define max(a,b)    (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a,b)    (((a) < (b)) ? (a) : (b))
#endif

#ifdef PANEL		/* exclude all panel filling code if not defined */

#define CLIP_RIGHT 1
#define CLIP_BOTTOM 2
#define CLIP_LEFT 3
#define CLIP_TOP 4
#define X_IS_INSIDE (x >= xl_cropq && x <= xh_cropq)
#define Y_IS_INSIDE (y >= yl_cropq && y <= yh_cropq)

struct sort {
	int index;
	int perimeter;
};

int G_CDECL _Optlink com_piecesq(const void *arg1, const void *arg2);
int G_CDECL _Optlink comppdpq(const void *arg1, const void *arg2);
int G_CDECL _Optlink compoutq(const void *arg1, const void *arg2);
int G_CDECL _Optlink compcentq(const void *arg1, const void *arg2);

/***************************************************************************
This routine receives the input array and returns it clipped. */
int GCLASS pan_clipq(PPGXYPAIRS xyp, int *index, int *npanels)
{
	int *x = NULL;
	int *y = NULL;
	PGXYPAIRS pxyp;
	PGXYPAIRS newxyp;
	PGXYPAIRS tempxyp;
	int n_arrays, ret_val;
	struct edge_pts *pan_pieces;
	int new_np;
	int i, k, m;
	int npts, new_npts;		/* Number of points in panel arrays. */
	int np;
	int hole_flg;
	int old_size;
	int new_np1;
	int win_num;
	int dir_flg;
	int xls = 0, yls = 0, xhs = 0, yhs = 0;
	int n_clusters;		/* Number of panel clusters in the input array. */
						/* A cluster is an outside and its holes. */
	G_PROCESS_ID

	npts = *index;
	newxyp = (PGXYPAIRS)gpcalloc(npts, sizeof(struct GXYPAIRSQ));
	if (newxyp == (PGXYPAIRS)NULL) {
		*index = -1;
		messageq(ERRQ, 31);		/* Out of Memory */
		return(0);
	}
	G_MEMCPY(newxyp, *xyp, npts * sizeof(struct GXYPAIRSQ));
	tempxyp = NULL;
	new_np = *npanels;
	x = NULL;
	y = NULL;
	pan_pieces = NULL;

	if (win_numq) {		/* Save the crop limits. */
		xls = xl_cropq;
		yls = yl_cropq;
		xhs = xh_cropq;
		yhs = yh_cropq;
	}
	win_num = win_numq + 1;

		/* FOR EACH WINDOW */

	for (i = 0; i < win_num; i++) {
		old_size = 0;		/* Reset counters for each window. */
		np = new_np;
		new_np = 0;
		new_npts = 0;
		if (i > 0) {
			xl_cropq = win_structq[i - 1].xl;
			yl_cropq = win_structq[i - 1].yl;
			xh_cropq = win_structq[i - 1].xh;
			yh_cropq = win_structq[i - 1].yh;
		}

		n_clusters = arrangepq(&newxyp, &npts, &np);

		for (m = 0; m < n_clusters; m++) {
			n_arrays = 0;
			hole_flg = 0;
			/* CLIP EACH PANEL. */
			for (;;) {
			    if (x != NULL)
					gpcfree((void **) &x);
				if (y != NULL)
					gpcfree((void **) &y);
			/* if dir_flg == 1 we are doing an outside. */
				dir_flg = newxyp[old_size].y;
				hole_flg += dir_flg;
				if (hole_flg > 1)
					break;
				npts = newxyp[old_size].x;
				old_size++;
				x = (int *)gpcalloc((npts + 1), sizeof(int));
				y = (int *)gpcalloc((npts + 1), sizeof(int));
				if (x == NULL || y == NULL) {
					n_clusters = 0;
					messageq(ERRQ, 31);	/* Not enough memory to clip panel */
					goto break_out;
				}
				for (k = 0; k < npts; k++) {
					x[k] = newxyp[old_size].x;
					y[k] = newxyp[old_size++].y;
				}
				ret_val = clip_panq(x, y, npts, &pan_pieces,
					i, n_arrays, dir_flg);
				if (ret_val < 0) {
					n_clusters = 0;
					np = 0;
					messageq(ERRQ, 31);	/* Not enough memory to clip panel */
					goto break_out;
				}
				n_arrays += ret_val;
				if (old_size >= *index)
					break;
			}
			if (n_arrays > 0) {
				new_np1 = newpanq(&x, &y, n_arrays, pan_pieces, &npts, i);
				for (k = 0; k < n_arrays; k++) {
					gpcfree((void **) &pan_pieces[k].x);
					gpcfree((void **) &pan_pieces[k].y);
				}
				gpcfree((void **) &pan_pieces);
				if (new_np1 < 0) {	/* Memory allocation failure. */
					n_clusters = 0;
					messageq(ERRQ, 31);	/* Not enough memory to clip panel */
					goto break_out;
				}
				new_np += new_np1;
				pxyp = (PGXYPAIRS)gpcrealloc(tempxyp,
					(size_t)((new_npts + npts) * sizeof(struct GXYPAIRSQ)));
				if (pxyp == NULL) {
					n_clusters = 0;
					messageq(ERRQ, 31);	/* Not enough memory to clip panel */
					goto break_out;
				}
				tempxyp = pxyp;
				for (k = 0; k < npts; k++) {
					tempxyp[new_npts].x = x[k];
					tempxyp[new_npts++].y = y[k];
				}
			}
		}
		if (new_npts > 0) {
			pxyp = (PGXYPAIRS)gpcrealloc(newxyp,
				(size_t)(new_npts * sizeof(struct GXYPAIRSQ)));
			if (pxyp == NULL) {
				gpcfree((void **)&newxyp);
				*index = -1;
				np = 0;
				messageq(ERRQ, 31);		/* Out of Memory */
				goto break_out;
			}
			newxyp = pxyp;
			G_MEMCPY(newxyp, tempxyp, (new_npts * sizeof(struct GXYPAIRSQ)));
		}
		*index = new_npts;
		np = new_np;
	}
	if (np > 0)
		n_clusters = arrangepq(&newxyp, &new_npts, &np);
	else
		n_clusters = 0;
	if (new_npts > 0) {
		pxyp = (PGXYPAIRS)gpcrealloc(*xyp,
			(size_t)(new_npts * sizeof(struct GXYPAIRSQ)));
		if (pxyp == NULL) {
			n_clusters = 0;
			messageq(ERRQ, 31);	/* Not enough memory to clip panel */
			goto break_out;
		}
		*xyp = pxyp;
		*index = new_npts;
		G_MEMCPY(*xyp, newxyp, new_npts * sizeof(struct GXYPAIRSQ));
	}
	else
		(*xyp)[0].x = 0;
break_out:
	*npanels = np;
	gpcfree((void **) &x);
	gpcfree((void **) &y);
	gpcfree((void **) &newxyp);
	gpcfree((void **) &tempxyp);
	gpcfree((void **) &pan_pieces);
	if (win_numq) {		/* Restore the crop limits. */
		xl_cropq = xls;
		yl_cropq = yls;
		xh_cropq = xhs;
		yh_cropq = yhs;
	}
	return(n_clusters);
}
/********************************************************************/
/* This routine takes a complete polygon and clips it against the
currently defined clip boundary. The polygon is broken up into separate
arrays of visible pieces. On return the pan_pieces structure array
contains the the pieces and the number of pieces is returned.
The variable start_in is used to indicate that the first point of the
polygon is inside the clip rectangle. */

#define FIRST_POINT_VISIBLE ((win_num == 0 && start_in) || (win_num > 0 && !start_in))

int GCLASS clip_panq(int *xpan, int *ypan, int plines,
	struct edge_pts **lp,
	int win_num,
	int n_arrays,
	int dir_flg)
{
	int i, j = 0, k, n;
	struct edge_pts *lp_p = NULL;
	int temp, tsave = 0;
	char out = 0;
	int start_in = 0;
	int x0, y0, x1, y1, x0c, y0c, x1c, y1c;
	G_PROCESS_ID

	if (xpan[plines - 1] == xpan[0] && ypan[plines - 1] == ypan[0])
		plines--;
	else {							/* Make room for the closing point. */
		xpan[plines] = xpan[0];
		ypan[plines] = ypan[0];
	}

/* A panel with the first vector starting on the crop boundary going out
of sight is a special case. We handle this situation by rotating the
vectors in the array so that the first vector does not start on the line.
If the call to cropq() below returns a 6 and we are cropping to the page
area or if cropq() returns a 0 and we are cropping to a window and the
first point is on the window boundary we need to change the arrays. */

	x0 = xpan[0];
	y0 = ypan[0];
	x1 = xpan[1];
	y1 = ypan[1];
	temp = cropq(x0, y0, x1, y1, (int *)&x0c, (int *)&y0c,
		(int *)&x1c, (int *)&y1c);
	if ((!win_num && temp == 6) || ((win_num && temp == 0) &&
		(x0 == xl_cropq || x0 == xh_cropq ||
		y0 == yl_cropq || y0 == yh_cropq))) {
		for (i = 0; i < plines; i++) {
			xpan[i] = xpan[i + 1];
			ypan[i] = ypan[i + 1];
		}
		x0 = xpan[i] = x1;
		y0 = ypan[i] = y1;
	}

/* Count the number of border intersections */
	for (i = 1; i <= plines; i++) {
		x1 = xpan[i];
		y1 = ypan[i];
		temp = cropq(x0, y0, x1, y1, (int *)&x0c, (int *)&y0c,
			(int *)&x1c, (int *)&y1c);
		if (i == 1) {
			if (temp > 1)
				out = 1;
			if (temp == 0 || temp == 2)
				start_in = 1;
		}
		if (temp == 2 && tsave == 5)
			temp = 3;
		switch(temp) {
			case 0:
				if (out)
					j++;
				break;
			case 1:
			case 2:
				j++;
				break;
			case 4:
			case 5:
			case 6:
				if (!out)
					j++;
				break;
			case 3:
				j += 2;
				break;
		}
		if (temp > 1)
			out = 1;
		else
			out = 0;
		x0 = x1;
		y0 = y1;
		tsave = temp;
	}

	if (start_in == 1 && out == 1) {
	/* The last vector of a panel ended on the crop boundary with the
	beginning point outside of the crop region. In order for this to happen
	the first vector had to start on the line headed in. Since cropq()
	returns a 0 in the latter case, no crossings were counted but one is
	needed. */
		j++;
		if (!win_num)	/* true if the panel started on the border. */
			start_in = 0;
		else
			out = 0;
	/* Set start_in to 0 so that the first point will be recorded as an
	in-bound point */
	}

	if (j == 0) {
/****************************************************************
If j == 0 no boundary crossings occured, so the panel is either all
out or all in. In this case the variable `temp' is either 0 or 4.

If we are clipping to the outside page:
    if the return value from cropq() indicates inside, the entire panel
        is visible and should be saved complete.
    if the return value from cropq() indicates outside, the entire page is
        the panel and only the corners of the page make up the panel.

If we are clipping to a `window':
    if the return value from cropq() indicates inside, the entire panel
        is hidden.
    if the return value from cropq() indicates outside, the entire panel
        is visible and the window makes a hole. The entire panel needs to
        be saved and the corners of the window need to be saved as another
        panel.
*******************************************************************/
		if (temp == 0) {	/* All in. */
			if (win_num)	/* Cropping to a window. */
				return(0);
			else {			/* Cropping to the outside page. */
				lp_p = (struct edge_pts *)gpcrealloc(*lp,
					(n_arrays + 1) * sizeof(struct edge_pts));
				if (lp_p == NULL) {
					gpcfree((void **) lp);
					return(-1);
				}
				*lp = lp_p;
				lp_p[n_arrays].in_point = -(n_arrays + 1);
				lp_p[n_arrays].out_point = -(n_arrays + 2);
				lp_p[n_arrays].x = (int *)gpcalloc(plines + 2, sizeof(int));
				lp_p[n_arrays].y = (int *)gpcalloc(plines + 2, sizeof(int));
				lp_p[n_arrays].x[0] = plines + 1;
				lp_p[n_arrays].y[0] = dir_flg + 3;
				for (i = 1; i <= plines + 1; i++) {
					lp_p[n_arrays].x[i] = xpan[i - 1];
					lp_p[n_arrays].y[i] = ypan[i - 1];
				}
				return(1);
			}
		}
		else {				/* All out. */
			k = 0;
			if (win_num) {	/* Cropping to a window so save complete panel. */
				lp_p = (struct edge_pts *)gpcrealloc(*lp,
					(n_arrays + 1) * sizeof(struct edge_pts));
				if (lp_p == NULL) {
					gpcfree((void **) lp);
					return(-1);
				}
				*lp = lp_p;
				lp_p[n_arrays].in_point = -(n_arrays + 1);
				lp_p[n_arrays].out_point = -(n_arrays + 2);
				lp_p[n_arrays].x = (int *)gpcalloc(plines + 2, sizeof(int));
				lp_p[n_arrays].y = (int *)gpcalloc(plines + 2, sizeof(int));
				lp_p[n_arrays].x[0] = plines + 1;
				for (i = 1; i <= plines + 1; i++) {
					lp_p[n_arrays].x[i] = xpan[i - 1];
					lp_p[n_arrays].y[i] = ypan[i - 1];
				}
				n_arrays++;
				k++;
			}
/* if the panel is completely outside the crop region, check to see if the
panel encloses the entire area. This is done by projecting a line out from
the edge and counting the number of times the panel boundary crosses the
projected line. If the number of crossings is even the panel does not show
and does not need further processing. */
			if (!crossing_testq(xpan, ypan, plines)) {
				/* panel encloses entire area so save the corners of the
					clip region as a panel. */
				lp_p = (struct edge_pts *)gpcrealloc(*lp,
					(n_arrays + 1) * sizeof(struct edge_pts));
				if (lp_p == NULL) {
					gpcfree((void **) lp);
					return(-1);
				}
				*lp = lp_p;
				lp_p[n_arrays].in_point = -(n_arrays + 1);
				lp_p[n_arrays].out_point = -(n_arrays + 2);
				lp_p[n_arrays].x = (int *)gpcalloc(6, sizeof(int));
				lp_p[n_arrays].y = (int *)gpcalloc(6, sizeof(int));
				lp_p[n_arrays].x[0] = 5;
				lp_p[n_arrays].y[0] = -1;
				lp_p[n_arrays].x[1] = xl_cropq;
				lp_p[n_arrays].y[1] = yl_cropq;
				lp_p[n_arrays].x[2] = xh_cropq;
				lp_p[n_arrays].y[2] = yl_cropq;
				lp_p[n_arrays].x[3] = xh_cropq;
				lp_p[n_arrays].y[3] = yh_cropq;
				lp_p[n_arrays].x[4] = xl_cropq;
				lp_p[n_arrays].y[4] = yh_cropq;
				lp_p[n_arrays].x[5] = xl_cropq;
				lp_p[n_arrays].y[5] = yl_cropq;
				return(k + 1);
			}
			else
				return(k);
		}
	}

	/* Any time the panel vectors started in the
	visable region. Therefore, the number of arrays to be connected is one
	more than half of the number of crossings. */

	if (FIRST_POINT_VISIBLE)
		n = (j / 2) + 1;
	else
		n = j / 2;
	j = k = 0;
	if (!(FIRST_POINT_VISIBLE))
		j = -1;	/* j gets incremented to 0 at first inbound point */

			/* ALLOCATE MEMORY FOR TEMPORARY ARRAYS */

	lp_p = (struct edge_pts *)gpcrealloc(*lp,
		(n + n_arrays) * sizeof(struct edge_pts));
	if (lp_p == NULL) {
		gpcfree((void **) lp);
		return(-1);
	}
	*lp = lp_p;

	for (i = n_arrays; i < (n_arrays + n); i++) {
		lp_p[i].x = (int *)gpcalloc((unsigned)64, sizeof(int));
		lp_p[i].y = (int *)gpcalloc((unsigned)64, sizeof(int));
		if (lp_p[i].x == (int *)NULL ||
			lp_p[i].y == (int *)NULL) {
			while (i >= 0) {
				gpcfree((void **) &lp_p[i].x);
				gpcfree((void **) &lp_p[i].y);
				i--;
			}
			gpcfree((void **) lp_p);
			return(-1);
		}
	}

	n = j + n_arrays;
	x0 = xpan[0];
	y0 = ypan[0];

	/* Search for crop boundary intersections. The variable `out' was set
	to the contition of the last vector in the first crop() for loop. */

	if (FIRST_POINT_VISIBLE) {
		/* load first point */
		lp_p[n].x[1] = x0;
		lp_p[n].y[1] = y0;
		k = 2;
		if (win_num)
			out = 1;
	}
	for (i = 1; i <= plines; i++) {
		x1 = xpan[i];
		y1 = ypan[i];
		temp = cropq(x0, y0, x1, y1, (int *)&x0c, (int *)&y0c,
			(int *)&x1c, (int *)&y1c);
		if (temp == 2 && tsave == 5)
			temp = 3;
		if (!win_num) {
			if ((temp == 1 || temp == 3) || (temp == 0 && out)) {
				n++;
				lp_p[n].in_point = save_pointq(x0c, y0c);
				lp_p[n].x[1] = x0c;
				lp_p[n].y[1] = y0c;
				k = 2;
			}
			if (temp == 2 || temp == 3) {	/* outbound point */
				lp_p[n].x[0] = k;
				lp_p[n].y[0] = 0;
				lp_p[n].out_point = save_pointq(x1c, y1c);
			}
			if (temp == 6 && !out) {
				lp_p[n].x[0] = k - 1;
				lp_p[n].y[0] = 0;
				lp_p[n].out_point = save_pointq(x0c, y0c);
			}
			if (temp < 4) {
				lp_p[n].x[k] = x1c;
				lp_p[n].y[k] = y1c;
				k = newvbufsq((void **)(&(lp_p[n].x)),
					(void **)(&(lp_p[n].y)), k, sizeof(int));
			}
		}
		else {
			if (temp == 0 && out) {				/* last temp was a 5 */
				lp_p[n].x[0] = k - 1;
				lp_p[n].y[0] = 0;
				lp_p[n].out_point = save_pointq(x0c, y0c);
			}
			if (temp == 1 || temp == 3) {
				lp_p[n].x[k] = x0c;
				lp_p[n].y[k] = y0c;
				lp_p[n].x[0] = k;
				lp_p[n].y[0] = 0;
				lp_p[n].out_point = save_pointq(x0c, y0c);
			}
			if (temp == 2 || temp == 3) {
				/* inside point */
				n++;
				lp_p[n].x[1] = x1c;
				lp_p[n].y[1] = y1c;
				lp_p[n].in_point = save_pointq(x1c, y1c);
				k = 2;
			}
			if (temp == 6 && !out) {			/* becoming visable */
				n++;
				lp_p[n].x[1] = x0;
				lp_p[n].y[1] = y0;
				lp_p[n].in_point = save_pointq(x0c, y0c);
				k = 2;
			}
			if (temp > 1) {
				lp_p[n].x[k] = x1;
				lp_p[n].y[k] = y1;
				k = newvbufsq((void **)(&(lp_p[n].x)),
					(void **)(&(lp_p[n].y)), k, sizeof(int));
			}
		}
		if (temp > 1)
			out = 1;
		else
			out = 0;
		x0 = x1;
		y0 = y1;
		tsave = temp;
	}
	if (FIRST_POINT_VISIBLE) {		/* Last point is also visible. */
		temp = lp_p[n_arrays].x[0] + k;
		lp_p[n].x = (int *)gpcrealloc(lp_p[n].x,
			(unsigned)temp * sizeof(int));
		lp_p[n].y = (int *)gpcrealloc(lp_p[n].y,
			(unsigned)temp * sizeof(int));
		if (lp_p[n].x == NULL || lp_p[n].y == NULL) {
			for (temp = 0; temp <= n; temp++) {
				gpcfree((void **) &lp_p[temp].x);
				gpcfree((void **) &lp_p[temp].y);
			}
			gpcfree((void **) lp);
			return(-1);
		}
				*lp = lp_p;
		for (i = 2; i <= lp_p[n_arrays].x[0]; i++) {
			lp_p[n].x[k] = lp_p[n_arrays].x[i];
			lp_p[n].y[k++] = lp_p[n_arrays].y[i];
		}
		lp_p[n].x[0] = k - 1;
		lp_p[n].y[0] = 0;
		lp_p[n_arrays].x = (int *)gpcrealloc(lp_p[n_arrays].x,
			(unsigned)k * sizeof(int));
		lp_p[n_arrays].y = (int *)gpcrealloc(lp_p[n_arrays].y,
			(unsigned)k * sizeof(int));
		if (lp_p[n_arrays].x == NULL || lp_p[n_arrays].y == NULL) {
			for (temp = 0; temp <= n_arrays; temp++) {
				gpcfree((void **) &lp_p[temp].x);
				gpcfree((void **) &lp_p[temp].y);
			}
			gpcfree((void **) lp);
			return(-1);
		}
		G_MEMCPY(lp_p[n_arrays].x, lp_p[n].x, k * sizeof(int));
		G_MEMCPY(lp_p[n_arrays].y, lp_p[n].y, k * sizeof(int));
		lp_p[n_arrays].in_point = lp_p[n].in_point;
		gpcfree((void **) &lp_p[n].x);
		gpcfree((void **) &lp_p[n].y);
		n--;
	}
	return((n + 1) - n_arrays);	/* return the # of additional arrays. */
}
/***************************************************************************/
/* This routine takes the pieces structure and assembles them into
complete panel(s).
Returns:    -1 -- Memory allocation failure.
          >= 0 -- Number of complete panels to be added to the panel list.
*/

int GCLASS newpanq(int **x, int **y,
	int n_pieces, struct edge_pts *pieces, int *n,
	int dir)
{
	int i, j, k, m, n_arrays;
	int close_panel;
	int is_cropped = 0;
	int break_out;
	int side1, side2;
	int start_side = 0, end_side = 0;
	int temp, in_point, out_point;
	int start = 0, end;
	int *used;
	int n_panels;
	int index;
	int n_holes;
	int *final_order;
	int npts = 0;
	struct GXYPAIRSQ corners[4];
	struct sort *ins, *outs;
	int *xp, *yp;

	index = 0;
	npts = 0;
	outs = (struct sort *)gpcalloc(n_pieces + 1, sizeof(struct sort));
	ins = (struct sort *)gpcalloc(n_pieces + 1, sizeof(struct sort));
	used = (int *)gpcalloc(n_pieces + 1, sizeof(int));
	final_order = (int *)gpcalloc(n_pieces + 1, sizeof(int));
	if (outs == NULL ||
		ins == NULL ||
		used == NULL ||
		final_order == NULL) {
		*n = index;
		gpcfree((void **) &ins);
		gpcfree((void **) &outs);
		gpcfree((void **) &used);
		gpcfree((void **) &final_order);
		return(-1);
	}
	n_holes = n_pieces;
	for (j = 0; j < n_pieces; j++) {
		if (pieces[j].y[0] > 2)
			n_holes--;
		ins[j].index = j;
		ins[j].perimeter = pieces[j].in_point;
		outs[j].index = j;
		outs[j].perimeter = pieces[j].out_point;
		npts += (pieces[j].x[0] + 1);
		if (pieces[j].in_point < 0 &&
			pieces[j].out_point < 0)
			index++;
	}
	if (index != n_pieces)
		is_cropped = 1;

	qsort((void *)outs, n_pieces, sizeof(struct sort), com_piecesq);
	qsort((void *)ins, n_pieces, sizeof(struct sort), com_piecesq);

	if (is_cropped) {
		for (j = 0; j < n_pieces; j++) {
			if (outs[j].perimeter < 0)
				continue;
			break;
		}
	}
	if (j < n_pieces) {
		if ((!dir && (outs[j].perimeter < ins[j].perimeter)) ||
			(dir && (outs[j].perimeter > ins[j].perimeter))) {
			for (i = 0; i < j; i++)
				used[i] = ins[i].index;
			for (i = j + 1; i < n_pieces; i++)
				used[i] = ins[i - 1].index;
			used[j] = ins[i - 1].index;
			for (i = 0; i < n_pieces; i++)
				ins[i].index = used[i];
		}
	}
	memset(used, '\0', n_pieces * sizeof(int));
	j = 0;
	i = 0;
	k = 0;
	end = 1;
	n_panels = 0;
	break_out = n_pieces * n_pieces;	/* Safety valve. */
	while (k < n_pieces) {
		for (i = 0; i < n_pieces; i++) {
			m = outs[i].index;
			if (used[m])
				continue;
			temp = ins[i].index;
			if (end) {
				end = 0;
				start = m;		/* set start to the begining array */
			}
			if (temp == start) {
				end = 1;
				final_order[k++] = -(n_pieces + m);
				n_panels++;
				used[m] = 1;
				continue;
			}
			used[m] = 1;
			final_order[k++] = m;
			i = 0;
			while(outs[i].index != temp && i < n_pieces)
				i++;
			i--;
		}
		if (j > break_out)
			break;
		j++;
	}

	index = 1;
	n_arrays = 0;
	close_panel = 1;
	end = 0;
	start = 0;
	side1 = -1;
	side2 = -1;
	for (i = 0; i < n_pieces; i++) {
		m = final_order[i];
		close_panel = 0;
		if (m < 0) {				/* This array completes the panel. */
			close_panel = 1;
			m = -(m + n_pieces);
			if (pieces[m].y[0] == -1 && is_cropped && n_holes > 1) {
				gpcfree((void **) &pieces[m].x);
				gpcfree((void **) &pieces[m].y);
				n_panels--;
				continue;
			}
			in_point = pieces[m].in_point;
			if (in_point >= 0)
				side2 = find_sideq(in_point);
			if (n_arrays == 0) {
				start_side = pieces[m].in_point;
				out_point = pieces[m].out_point;
				if (out_point >= 0)
					side1 = find_sideq(out_point);
			}
			end_side = pieces[m].out_point;
		}
		else {
			if (pieces[m].y[0] == -1 && is_cropped) {
				gpcfree((void **) &pieces[m].x);
				gpcfree((void **) &pieces[m].y);
				n_panels--;
				continue;
			}
			if (n_arrays == 0) {
				start_side = pieces[m].in_point;
				out_point = pieces[m].out_point;
				if (out_point >= 0)
					side1 = find_sideq(out_point);
			}
			else {
				in_point = pieces[m].in_point;
				if (in_point >= 0)
					side2 = find_sideq(in_point);
			}
			n_arrays++;
		}
		if (n_arrays > 0 && side1 >= 0 && side2 >= 0) {
			temp = connectq(side1, side2, corners, dir);
		}
		else
			temp = 0;
		npts = pieces[m].x[0];
		xp = (int *)gpcrealloc(*x, (index + npts + temp + 2) * sizeof(int));
		yp = (int *)gpcrealloc(*y, (index + npts + temp + 2) * sizeof(int));
		if (xp == NULL || yp == NULL)
			return(-1);
		*x = xp;
		*y = yp;
		for (j = 0; j < temp; j++) {
			(*x)[index] = corners[j].x;
			(*y)[index++] = corners[j].y;
			end++;
		}
		for (j = 1; j <= npts; j++) {
			(*x)[index] = pieces[m].x[j];
			(*y)[index++] = pieces[m].y[j];
			end++;
		}
		gpcfree((void **) &pieces[m].x);
		gpcfree((void **) &pieces[m].y);
		if (close_panel) {
			if (start_side < 0) {		/* Close panel. */
				if (((*x)[start + 1] != (*x)[index - 1]) ||
					((*y)[start + 1] != (*y)[index - 1])) {
					(*x)[index] = (*x)[start + 1];
					(*y)[index++] = (*y)[start + 1];
					end++;
				}
			}
			else {
				side1 = find_sideq(end_side);
				side2 = find_sideq(start_side);
				temp = connectq(side1, side2, corners, dir);
				xp = (int *)gpcrealloc(*x, (index + temp + 2) * sizeof(int));
				yp = (int *)gpcrealloc(*y, (index + temp + 2) * sizeof(int));
				if (xp == NULL || yp == NULL)
					return(-1);
				*x = xp;
				*y = yp;
				for (j = 0; j < temp; j++) {
					(*x)[index] = corners[j].x;
					(*y)[index++] = corners[j].y;
					end++;
				}
				(*x)[index] = (*x)[start + 1];
				(*y)[index++] = (*y)[start + 1];
				end++;
			}
			(*x)[start] = end;
			(*y)[start] = 0;
			start = index;
			index++;				/* Make room for the count byte. */
			end = 0;
			side1 = -1;
			side2 = -1;
			start_side = pieces[m].in_point;
			n_arrays = 0;
		}
		out_point = pieces[m].out_point;
		if (out_point >= 0)
			side1 = find_sideq(out_point);
	}
	if (close_panel)
		index--;
	else
		messageq(ERRQ, NEWQ);
	*n = index;
	gpcfree((void **) &ins);
	gpcfree((void **) &outs);
	gpcfree((void **) &used);
	gpcfree((void **) &final_order);
	return(n_panels);
}
/*******************************************************************
This routine loads struct crossings with information about where the
point x,y crosses a crop boundary and with which array number the point is
associated. The variable dis is the total number of pixels the crossing
is from the upper right corner in a clockwise direction. start = 1
indicates the point is starting a visable array. */
int GCLASS save_pointq(int x, int y)
{
	unsigned int distance;
	G_PROCESS_ID

	if ((x == xh_cropq) && Y_IS_INSIDE)				/* right side */
		distance = yh_cropq - y;

	else if ((y == yl_cropq) && X_IS_INSIDE)			/* bottom */
		distance = (yh_cropq - yl_cropq) + (xh_cropq - x);

	else if ((x == xl_cropq) && Y_IS_INSIDE)			/* left side */
		distance = (yh_cropq - yl_cropq) +
			(xh_cropq - xl_cropq) + (y - yl_cropq);

	else { 												/* top */
		distance = (yh_cropq - yl_cropq) * 2;
		distance += (xh_cropq - xl_cropq);
		distance += (x - xl_cropq);
	}
	return(distance);
}
/******************************************************************/
/* Used to project a line out from between the first two points to test
if a connection is needed. The test is done by looking at the vectors
of a panel and counting the number of times the projected line is crossed.
If the line is crossed an odd number of times a connection is needed. */
int GCLASS crossing_testq(int *x, int *y, int plines)
{
	int i = 0, j = 0, iscross = 0;
	int x0, y0, x1, y1;
	float slope;
	char higher = 0, lower = 0;
	G_PROCESS_ID

	x0 = x[0];
	y0 = y[0];
	for (i = 1; i <= plines; i++) {
		x1 = x[i];
		y1 = y[i];
		/* The vector lies along the test line and would cause a divide by
	   zero error below. */
		if (y0 == yl_cropq && y1 == yl_cropq) {
			x0 = x1;
			y0 = y1;
			continue;
		}
		/* Here the vector stradles the test line so we determine the
		location of the crossing. If the crossing is outside of the crop
		region we count it */
		if ((y0 <= yl_cropq && y1 >= yl_cropq) ||
			(y0 >= yl_cropq && y1 <= yl_cropq)) {
			slope = (float)(x1 - x0) / (float)(y1 - y0);
			iscross = (int)((slope * (float)(yl_cropq - y0))
				+ (float).5) + x0;
			if (iscross > xh_cropq) {
				if (y0 > yl_cropq || y1 > yl_cropq)
					higher++;
				if (y0 < yl_cropq || y1 < yl_cropq)
					lower++;
			}
		}
		if (higher && lower) {
			higher--;
			lower--;
			j++;
		}
		x0 = x1;
		y0 = y1;
	}
	return(1 - (j%2));
}
/**********************************************************************/
/* Compare integer elements of an array. MUST be cdecl. */
/**********************************************************************/

int G_CDECL com_piecesq(const void *arg1, const void *arg2)
{
	struct sort *x, *y;
	int tempx, tempy;

	x = (struct sort *)arg1;
	y = (struct sort *)arg2;

	tempx = (*x).perimeter;
	tempy = (*y).perimeter;
	if (tempx > tempy)
		return(1);
	if (tempx == tempy)
		return(0);
	return(-1);
}

/*********************************************************************/

/******************************************************************/
/* Split an incoming panel into its pieces. Arrange each piece    */
/* so that the outside comes first and is connected to each hole. */
/* Each composite panel runs CCW and obeys the winding rule.      */
/* Load the new values for np and dim.                            */
/* Return the number of outside panels. Return -1 for an error.   */
/******************************************************************/
int GCLASS arrangepq(
	PPGXYPAIRS xyppi,/* Pointer to closed polygon pieces */
	int *dimi,				/* # array elements */
	int *npi				/* Number of pieces */
)
{
	struct pan_data *pdp;
	int i, j, k, n, ctr, size, rv;
	int xl, yl, xh, yh;
	int isj, isk;
	int npleft;
	int dim, np;
	int nouts;
	PGXYPAIRS xypi;
	PGXYPAIRS xynew;	/* New output array */
	int xyni = 0;		/* Index for new output array */

	np = *npi;
	if (np == 0)
		return(0);
	if (np == 1) {
		if (ccw_testq(*xyppi) != 1)	/* Be sure it is CCW */
			rev_polyq(*xyppi);
		xyppi[0] -> y = 1;			/* Flag as an outside */
		return(1);
	}
	dim = *dimi;
	xypi = *xyppi;	/* Local pointer to the original panel */
	/* ALLOCATE THE MEMORY FOR THE PIECE STRUCTURES */
	/* Allocate one structure per panel piece */
	pdp = (struct pan_data *)gpcalloc(np, sizeof(struct pan_data));
	if (pdp == (struct pan_data *)NULL) {
		messageq(ERRQ, 31);		/* Out of Memory */
		return(-1);
	}
	/* ALLOCATE MEMORY FOR NEW OUTPUT ARRAYS */
	xynew = (PGXYPAIRS)gpcalloc((size_t)64, sizeof(struct GXYPAIRSQ));
	if (xynew == (PGXYPAIRS)NULL) {
		gpcfree((void **) &pdp);
		messageq(ERRQ, 31);	/* Not enough memory */
		return(-1);
	}

	/* ALLOCATE THE SUB-MEMORY IN EACH STRUCTURE */
	for (j = 0; j < np; j++) {
		/* Need one element for each of OTHER panel pieces */
		pdp[j].outs = 	/* list of other panels for which this is a hole */
			(int *)gpcalloc(np - 1, sizeof(int));
		if (pdp[j].outs == (int *)NULL) {
			messageq(ERRQ, 31);	/* Not enough memory */
			for(k = j - 1; k >= 0; k-- ) {
				gpcfree((void **) &pdp[k].outs);
			}
			gpcfree((void **) &pdp);
			return(-1);
		}
	}
	/* FILL IN THE DATA FOR EACH STRUCTURE */
	ctr = 0;
	for (j = 0; j < np; j++) {	/* Each piece */
		size = xypi[ctr].x;	/* Size of panel piece */
		pdp[j].xyp = (PGXYPAIRS)&(xypi[ctr].x);
		for(k = 0; k < np-1; k++)
			pdp[j].outs[k] = -1;		/* Initialize to -1 */
		/* Must use near addresses for return values */
		bounding_boxq(pdp[j].xyp, 1, &xl, &yl, &xh, &yh);
		pdp[j].xl = xl;
		pdp[j].yl = yl;
		pdp[j].xh = xh;
		pdp[j].yh = yh;
		pdp[j].nholes = 0;
		pdp[j].nouts = 0;
		ctr += size + 1;
	}
	/* SORT ALL THE PANEL STRUCTURES BY ymax DESCENDING */
	/* IF TWO PIECES HAVE SAME yu, SORT BY SMALLEST xl,yl AND BIGGEST xu */
	qsort((void *)(pdp), (size_t)(np), sizeof(struct pan_data), comppdpq);

	/*
		Now we discard pairs of panels that lie on the clip region.
	*/
	while (np > 1) {
		if (!pan_on_clip_regionq(pdp[0].xyp))
			break;
		if (!pan_on_clip_regionq(pdp[1].xyp))
			break;
		gpcfree((void **)&pdp[0].outs);
		gpcfree((void **)&pdp[1].outs);
		for (i = 0, j = 2; j < np; i++, j++) {
			memcpy((void *)&pdp[i], (void *)&pdp[j],
				sizeof(struct pan_data));
		}
		np -= 2;
		dim -= 12;
	}
	*npi = np;

	/*	Start with the outermost panel (biggest yu)
		and test all others to see if they are holes in it.
	*/
	for(j = 0; j < np - 1; j++) {
		for(k = j + 1; k < np; k++) {
			rv = hole_testq( pdp[j].xyp, pdp[k].xyp );
			if (rv) {	/* Put kth panel in hole array */
				/* Tell j that it has another hole */
				pdp[j].nholes++;
				/* Tell k that j is its outside */
				pdp[k].outs[pdp[k].nouts++] = j;
			}
		}
	}

	/*	At this point, each panel piece has the number of other panels
		that are its holes as well as a list of panels that are its
		outsides. The problem is to eliminate concentric holes
		and make new output arrays in which the holes are connected to
		their correct outer pieces.
		The strategy is to peel off the outer pieces until none are left.
	*/
	npleft = *npi;
	nouts = 0;
	np = 0;
	while (npleft > 0) {
		/*	FIRST OUTPUT OUTSIDE PANELS WITH NO HOLES */
		for(j = 0; j < *npi; j++) {
			if (pdp[j].nholes == 0 && pdp[j].nouts == 0) {
				np++;
				if (ccw_testq(pdp[j].xyp) != 1)	/* Be sure it is CCW */
					rev_polyq(pdp[j].xyp);
				for(k = 0; k <= pdp[j].xyp[0].x; k++) {
					/* This includes the vertex counter */
					xynew[xyni].x = pdp[j].xyp[k].x;
					if (k == 0) {
						xynew[xyni].y = 1;	/* An outside */
						nouts++;
					}
					else {
						xynew[xyni].y = pdp[j].xyp[k].y;
					}
					xyni = newbufsq(&xynew, xyni);
				}
				npleft--;
				/* Set counters negative so we don't output this again */
				pdp[j].nholes = pdp[j].nouts = -1;
			}
		}
		/* NOW OUTPUT THE NON-CONCENTRIC PANELS TOGETHER WITH THEIR HOLES */
		for(j = 0; j < *npi; j++) {		/* j is the outside index */
			if (pdp[j].nholes > 0 && pdp[j].nouts == 0) {
				/* IT IS AN OUTSIDE WITH HOLES */
				np++;
				if (ccw_testq(pdp[j].xyp) != 1)	/* Be sure it is CCW */
					rev_polyq(pdp[j].xyp);
				/* PUT THE OUTSIDE PANEL INTO THE OUTPUT ARRAY */
				for(k = 0; k <= pdp[j].xyp[0].x; k++) {
					/* This includes the vertex counter */
					xynew[xyni].x = pdp[j].xyp[k].x;
					if (k == 0) {
						xynew[xyni].y = 1;	/* An outside */
						nouts++;
					}
					else {
						xynew[xyni].y = pdp[j].xyp[k].y;
					}
					xyni = newbufsq(&xynew, xyni);
				}
				npleft--;
				pdp[j].nholes = pdp[j].nouts = -1;
				/* NOW FIND THE HOLES */
				for(k = j + 1; k < *npi; k++) {
					/* IF ONLY ONE OUT, IT IS A PROPER HOLE */
					if (pdp[k].nouts == 1) {
						/* k is a hole */
						/* MUST KEEP OUTS SORTED SO THAT -1'S AT END */
						if (pdp[k].outs[0] == j) {	/* It is j's hole */
							np++;
							if (ccw_testq(pdp[k].xyp) == 1)	/* Be sure it is CW */
								rev_polyq(pdp[k].xyp);
							for(i = 0; i <= pdp[k].xyp[0].x; i++) {
								/* This includes the vertex counter */
								xynew[xyni].x = pdp[k].xyp[i].x;
								if (i == 0) {
									xynew[xyni].y = 0;	/* A hole */
								}
								else {
									xynew[xyni].y = pdp[k].xyp[i].y;
								}
								xyni = newbufsq(&xynew, xyni);
							}
							pdp[k].nouts = -1;
							pdp[k].nholes = -1;	/* No concentric holes */
							npleft--;

							/*
								IF k IS IN THE LIST OF OUTS, REMOVE
								BOTH j AND k FROM OTHER LISTS OF OUTS
								SINCE THEY ARE CONCENTRIC RINGS.
							*/
							for(n = 0; n < *npi; n++) {
								/* FOR EACH PANEL */
								ctr = pdp[n].nouts;
								isj = -1;
								isk = -1;
								for(i = 0; i < ctr; i++) {
									/* LOOK FOR A j OR k IN THE outs */
									/* If k is there, j has to be there also! */
									if (pdp[n].outs[i] == j ) {
										isj = i;
									}
									else if (pdp[n].outs[i] == k ) {
										isk = i;
									}
								}
								if (isk != -1) {	/* If a k is there... */
									pdp[n].outs[isk] = -1;
									pdp[n].nouts--;
									if (isj != -1) {
										pdp[n].outs[isj] = -1;
										pdp[n].nouts--;
									}
								}
							}
						}
					}
				}	/* End of the hole search */
				/* MUST SORT THE outs ARRAYS, DESCENDING */
				for (i = 0; i < *npi; i++) {
					if (pdp[i].nouts > 0) {
						qsort((void *)(pdp[i].outs), (size_t)(*npi - 1),
							sizeof(int), compoutq);
					}
				}
				/* Now the out arrays have used entries(-1) at end */
			}	/* Done with each hole */
		}	/* Done with each outside */
	}	/* End of while() */

	/* FREE LOCAL MEMORY */
	for(k = 0; k < *npi; k++ ) {
		gpcfree((void **) &pdp[k].outs);
	}
	gpcfree((void **) &pdp);
	/* NOW FREE THE INPUT VECTOR MEMORY */
	gpcfree((void **) xyppi);
	/* AND REASSIGN THE NEW VECTORS TO IT */
	*xyppi = xynew;
	*npi = np;		/* Return the dimensions */
	*dimi = dim;
	return(nouts);
}

/**************************************************************************
 * Returns a 1 if the panel piece in the given xyp array lies on the clip *
 * region. Otherwise, returns a 0.                                        *
 **************************************************************************/
int GCLASS pan_on_clip_regionq(PGXYPAIRS xyp)
{
	int i, corner, prev_corner = 0, direction = 0, next_corner = 0;
	G_PROCESS_ID

	if (xyp[0].x != 5)                                /* Is it four-sided? */
		return(0);
	for (i = 1; i <= 5; i++) {
		if (xyp[i].x == xl_cropq && xyp[i].y == yl_cropq) /* Which corner? */
			corner = 1;
		else if (xyp[i].x == xh_cropq && xyp[i].y == yl_cropq)
			corner = 2;
		else if (xyp[i].x == xh_cropq && xyp[i].y == yh_cropq)
			corner = 3;
		else if (xyp[i].x == xl_cropq && xyp[i].y == yh_cropq)
			corner = 4;
		else
			return(0);
		if (i > 1) {
			if (i > 2) {
				if (corner != next_corner)   /* Is it the expected corner? */
					return(0);
			}
			else if (gabs(corner - prev_corner) == 1)
				direction = corner - prev_corner;   /* Calculate direction */
			else
				return(0);
			next_corner = corner + direction;   /* Set the expected corner */
			if (next_corner < 1)
				next_corner = 4;
			else if (next_corner > 4)
				next_corner = 1;
		}
		prev_corner = corner;
	}
	return(1);
}

/**********************************************************************/
/* Compare the ymax elements of the panel data arrays. MUST be cdecl. */
/**********************************************************************/
/* rv < 0 means arg1 comes before arg2 */
int G_CDECL comppdpq(const void *arg1, const void *arg2)
{
	int rv;

	/* We want the outside to be first! */
	/* SORT BY ymax, BIGGEST FIRST */
	rv = ((struct pan_data *)arg2) -> yh -
		((struct pan_data *)arg1) -> yh;
	if (rv == 0) {	/* Sort in increasing xmin if same y */
		rv = ((struct pan_data *)arg1) -> xl -
			((struct pan_data *)arg2) -> xl;
		if (rv == 0) {	/* Biggest xh first */
			rv = ((struct pan_data *)arg2) -> xh -
				((struct pan_data *)arg1) -> xh;
			if (rv == 0)	/* Finally, by smallest yl first */
				rv = ((struct pan_data *)arg1) -> yl -
					((struct pan_data *)arg2) -> yl;
		}
	}
	return(rv);
}
/*********************************************************************/

/**********************************************************************/
/* Compare the outs elements of the panel data arrays. MUST be cdecl. */
/**********************************************************************/

int G_CDECL compoutq(const void *arg1, const void *arg2)
{
	int *x, *y;

	x = (int *)arg2;
	y = (int *)arg1;

	return(*x - *y);
}

/*********************************************************************/

#if MSQ
#pragma optimize("", off)
#endif
/*****************************************************************/
/* Test a panel for being CW (return -1) or CCW (return 1)       */
/* If the two sides leading to the maximum y-value coincide, the */
/* test fails and 0 is returned. (Case not allowed in GraphiC.   */
/*****************************************************************/
int GCLASS ccw_testq(
	PGXYPAIRS xyp	/* Pointer to closed self-counting polygon */
)
{
	int j, ymax, jmax, dim;
	double xm, x, xp, ym, y, yp, dms, dps;
	double t1, t2;
	double xl, yl, xn;	/* Larger and clipped values */

	dim = xyp[0].x;
	xyp++;	/* Get past the counter */
	/* First find the maximum y value */
	ymax = xyp[0].y;
	jmax = 0;
	for(j = 1; j < dim; j++) {
		if (xyp[j].y > ymax) {
			ymax = xyp[j].y;
			jmax = j;
		}
	}
	/*	The strategy is to clip the longer edge adjacent to the maximum
		so that both sides are the same length. Then the direction can
		found by comparing x-values.
	*/
	x = xyp[jmax].x;
	y = xyp[jmax].y;
	xp = xyp[jmax + 1].x;	/* jmax can't be last value */
	yp = xyp[jmax + 1].y;
	if (jmax > 0) {
		xm = xyp[jmax - 1].x;
		ym = xyp[jmax - 1].y;
	}
	else {
		/* dim - 1 is same as first */
		xm = xyp[dim - 2].x;
		ym = xyp[dim - 2].y;
	}
	t1 = (double)(x - xm);
	t2 = (double)(y - ym);
	dms = t1 * t1 + t2 * t2;
	t1 = (double)(x - xp);
	t2 = (double)(y - yp);
	dps = t1 * t1 + t2 * t2;
	if (dps > dms) {
		xl = x - xp; yl = y - yp;
		/* dms is the smaller distance squared */
		if (xl != 0.0) {
			/* This is really dx */
			xn = sqrt(dms / ( 1.0 + (yl * yl) / (xl * xl)) );
			if (xp > x)
				xn = x + xn;
			else
				xn = x - xn;
		}
		else
			xn = x;

		if (xn == xm)
			return(0);	/* Pathelogical case */
		if (xn < xm)
			return(1);	/* CCW */
		else
			return(-1);	/* CW */
	}
	else {
		xl = x - xm; yl = y - ym;
		/* dps is the smaller distance squared */
		if (xl != 0.0) {
			/* This is really dx */
			xn = sqrt(dps / ( 1.0 + (yl * yl) / (xl * xl)) );
			if (xm > x)
				xn = x + xn;
			else
				xn = x - xn;
		}
		else
			xn = x;

		if (xn == xp)
			return(0);	/* Pathelogical case */
		if (xn < xp)
			return(-1);	/* CW */
		else
			return(1);	/* CCW */
	}
}
/*********************************************************************/

/******************************************************************/
/* Reverse the order of a closed polygon                          */
/******************************************************************/
void GCLASS rev_polyq(
	PGXYPAIRS xyp	/* Pointer to self-counting closed polygon */
)
{
 	int j, dim;
	struct GXYPAIRSQ txy;

	dim = xyp[0].x;
	xyp++;	/* Get past the counter */
	for(j = 1; j < dim/2; j++) {
		txy.x = xyp[dim - j - 1].x;
		txy.y = xyp[dim - j - 1].y;
		xyp[dim - j - 1].x = xyp[j].x;
		xyp[dim - j - 1].y = xyp[j].y;
		xyp[j].x = txy.x;
		xyp[j].y = txy.y;
	}
}
/*********************************************************************/

/******************************************************************/
/* Test to see if polygon 2 is a hole of polygon 1.               */
/* Return 1 if it is inside, 0 otherwise.                         */
/* Assumes that ymax1 >= ymax2                                    */
/******************************************************************/
int GCLASS hole_testq(
	PGXYPAIRS xy1,	/* Pointer to self-counting
							closed (outer) polygon */
	PGXYPAIRS xy2	/* Pointer to self-counting
							closed (inner?) polygon */
)
{
	int j, xmin, ymax;
	int dim_1, dim_2;	/* Number of vertices in the two panels */
	int ncross = 0;	/* +2 for a full crossing, +1 for a vertex */
	int xpi, ypi, xi, yi;
	int dy1, dy2;
	int up = 0;	/* # of upward kisses to the line */
	double xp, x, yp, y, xmn, ymx, xs;

	dim_1 = xy1[0].x;
	dim_2 = xy2[0].x;
	/* Increment counters to get past the size */
	xy1++;	xy2++;
	/* First find the maximum y value of xp2 and the leftmost x at ymax */
	xmin = xy2[0].x;
	ymax = xy2[0].y;
	for(j = 1; j < dim_2; j++) {
		if (xy2[j].y > ymax) {
			xmin = xy2[j].x;
			ymax = xy2[j].y;
		}
		else if (xy2[j].y == ymax) {
			if (xy2[j].x < xmin)
				xmin = xy2[j].x;
		}
	}
	xmn = (double)xmin;
	ymx = (double)ymax;


	/*	Now check each edge of panel 1 for intersection with a line
		from xmin, ymax to -infinity, ymax
	*/
	xp = xpi = xy1[0].x;
	yp = ypi = xy1[0].y;
	dy1 = 0;
	dy2 = 0;
	for(j = 1; j < dim_1; j++) {
		x = xp;
		y = yp;
		xi = xpi;
		yi = ypi;
		xp = xpi = xy1[j].x;
		yp = ypi = xy1[j].y;
		if (ypi != yi) {	/* There may be an intersection */
			/* CHECK FOR RIGHT Y RANGE */
			if (ypi > ymax && yi > ymax)
				continue;
			if (ypi < ymax && yi < ymax)
				continue;
			/* SOLVE FOR THE X-INTERSECTION OF THE LINE AT
				YMAX GOING TO LEFT EDGE OF SCREEN */
			xs = x + ( ((xp - x) * (ymx - y)) / (yp - y) );
			if (xs < xmn) {
				if (yi == ymax || ypi == ymax) {
					if (yi == ymax) {
						if (ypi > ymax)
							up += 1;
						else
							up -= 1;
					}
					if (ypi == ymax) {
						if (yi < ymax)
							up += 1;
						else
							up -= 1;
					}
				}
				else
					ncross += 2;
			}
			else if (xs == xmn) {
	/* If a line touches the test line:
	   If it returns in the direction from which it came, do'nt count it.
	   If it continues on the other side, count it. */
				if (dy1 != 0) {		/* second time on the line. */
					dy2 = (int)(yp - y);
					if (sign(dy1) == sign(dy2))
						ncross += 2;
					dy1 = 0;
					dy2 = 0;
				}
				else	/* First time the point is on the line. */
					dy1 = (int)(yp - y);
			}
		}
		else if (ypi != ymax)
			continue;	/* Different levels */
		else if (xmin > xi && xmin > xpi)
			continue;	/* Next to each other, no intersection */
		else {	/* Same level. Must happen on two more segs */
			 continue;
		}
	}
	ncross += abs(up);	/* Will be 0 for a kiss */
	/* Answer had better come out even!! */
	ncross /= 2;	/* The real crossing count */
	if (ncross %2)	/* Odd number means enclosed */
		return(1);
	return(0);
}
/*********************************************************************/
/* Find the bounding box of np panels in the self-counting panel structure */
void GCLASS bounding_boxq(PGXYPAIRS xyp, int np, int *xl, int *yl, int *xh, int *yh)
{
	int i, j, k;
	int npts;

	*xl = *xh = xyp[1].x;
	*yl = *yh = xyp[1].y;
	k = 0;
	j = 0;
	do {
		npts = xyp[j++].x;
		for (i = 0; i < npts; i++, j++) {
			*xl = min(*xl, xyp[j].x);
			*xh = max(*xh, xyp[j].x);
			*yl = min(*yl, xyp[j].y);
			*yh = max(*yh, xyp[j].y);
		}
	}
	while (++k < np);
}

int GCLASS find_sideq(int perimeter)
{
	int dx, dy;
	int side;
	G_PROCESS_ID

	dx = xh_cropq - xl_cropq;
	dy = yh_cropq - yl_cropq;
	side = perimeter % ((dx * 2) + (dy * 2));
	if (side > ((dy * 2) + dx))
		side = CLIP_TOP;
	else if (side > dx + dy)
		side = CLIP_LEFT;
	else if (side > dy)
		side = CLIP_BOTTOM;
	else
		side = CLIP_RIGHT;
	return(side);
}

#if MSQ
#pragma optimize("", on)
#endif
/************************************************************/
/* Called from order_crossingsq() to fill in corners between connected
arrays. num is the array number which will be followed by a corner in
do_panelsq() */
int GCLASS connectq(int side1, int side2, struct GXYPAIRSQ *corner, int dir)
{
	int i, xtemp[4], ytemp[4];
	int side1l, side2l, temp = 0;
	G_PROCESS_ID

	if (side1 == side2)
		return(0);
	side1--;
	side2--;
	side1l = side1;
	side2l = side2;
	if (!dir) {
		while (side1l != side2l) {
			side1l--;
			if (side1l < 0)
				side1l = 3;
			temp |= 0x1 << side1l;
		}
	}
	else {
		while (side1l != side2l) {
			temp |= 0x1 << side1l;
			side1l++;
			if (side1l > 3)
				side1l = 0;
		}
	}
	if (temp & 1) {
		/* need lower right */
		xtemp[0] = xh_cropq;
		ytemp[0] = yl_cropq;
	}
	if (temp & 2) {
		/* need lower left */
		xtemp[1] = xl_cropq;
		ytemp[1] = yl_cropq;
	}
	if (temp & 4) {
		/* need upper left */
		xtemp[2] = xl_cropq;
		ytemp[2] = yh_cropq;
	}
	if (temp & 8) {
		/* need upper right */
		xtemp[3] = xh_cropq;
		ytemp[3] = yh_cropq;
	}
	i = 0;
	side1l = side1;
	side2l = side2;
	if (!dir) {
		while (side1l != side2l) {
			side1l--;
			if (side1l < 0)
				side1l = 3;
			corner[i].x = xtemp[side1l];
			corner[i++].y = ytemp[side1l];
		}
	}
	else {
		while (side1l != side2l) {
			corner[i].x = xtemp[side1l];
			corner[i++].y = ytemp[side1l];
			side1l++;
			if (side1l > 3)
				side1l = 0;
		}
	}
	return(i);
}

/*********************************************************************/
/* Split a panel until there are no holes in it and process it */
void GCLASS split_holesq(int *pnumber)
{
	int j, k, npts;
	int xl_crops, xh_crops;
	int xl, xh, yl, yh;	/* Bounds on panel box */
	struct GXYPAIRSQ *xypp = NULL;	/* Place keeper */
	struct GXYPAIRSQ *xypnew = NULL;	/* Pointer to new array */
	struct GXYPAIRSQ *sxyp = NULL;	/* Pointer to saved array */
	int *xhc, xhci = 0;	/* Pointer and index to an array of hole centers */
	int xypnewi = 0;	/* index to xypnew array */
	int newpnum = 0;
	G_PROCESS_ID

	xl_crops = xl_cropq;	/* Save  cropping boundaries */
	xh_crops = xh_cropq;
	/* Allocate necessary memory */
	xhc = (int *)gpcalloc(64, sizeof(int));
	if(xhc == NULL) {
		gpcfree((void **)&sxyp);
		return;
	}
	xypp = g_pq.save_xyp;	/* Mark start of panel piece */
	npts = 0;
	for(j = 0; j < *pnumber; j++) {	/* For each piece */
		xypp +=	npts;
		npts = xypp -> x;
		if(xypp -> y == 0) {	/* It  is a hole */
			bounding_boxq(xypp, 1, &xl, &yl, &xh, &yh);
			xhc[xhci] = (xl + xh) >> 1;	/* SAVE HOLE CENTER */
			xhci = newabufsq((void **)&xhc, xhci, sizeof(int));
		}
		npts++;		/* +1 for the counter */
	}
	if(xhci ==  0)	/* No holes to split! */
		goto split_holes_end;
	xhc[xhci++] = xh_crops;	/* Add right boundary to array */
	
	/* SORT HOLE CENTERS */
	qsort((void *)xhc, (size_t)xhci, sizeof(int), compcentq);
	
	/**************************************************
	The strategy is to clip the panel into vertical stripes cutting
	the center of each hole. The resulting clipped regions
	must be reassembled into a new save_xyp array.
	Each call to the clipping routing changes the array, so
	we must be sure to save the original each time.
	**************************************************/
	for(j = 0; j < xhci; j++) {	/* For each center */
		xh_cropq = xhc[j];
		if(xh_cropq > xl_cropq) {
			sxyp = (struct GXYPAIRSQ *)gpcalloc(g_pq.npts, sizeof(struct GXYPAIRSQ));
			if(sxyp == NULL) {
				goto split_holes_end;
			}
			/* Copy original arrays to scratch array */
			G_MEMCPY(sxyp, g_pq.save_xyp, g_pq.npts * sizeof(struct GXYPAIRSQ));
			npts = g_pq.npts;
			k = *pnumber;
			pan_clipq(&sxyp, &npts, &k);
			newpnum += k;
			/* EXPAND THE SIZE OF THE NEW OUTPUT ARRAY */
			xypp = (struct GXYPAIRSQ *)gpcrealloc(xypnew,
				(xypnewi + npts) * sizeof(struct GXYPAIRSQ) );
			if(xypp == NULL)
				goto split_holes_end;
			xypnew = xypp;
			G_MEMCPY((xypnew + xypnewi), sxyp,	/* Add new panel */
				npts * sizeof(struct GXYPAIRSQ));
			xypnewi += npts;	/* Up new panel pair counter */
			gpcfree((void **)& sxyp);	/* Free scratch memory */
		}
		xl_cropq = xh_cropq;
	}
	/* NOW PUT THE NEW ARRAY BACK INTO THE G_PQ STRUCTURE */
	xypp = (struct GXYPAIRSQ *)gpcrealloc(g_pq.save_xyp,
	 	xypnewi * sizeof(struct GXYPAIRSQ) );
	if(xypp == NULL)
		goto split_holes_end;
	g_pq.save_xyp = xypp;
	G_MEMCPY(g_pq.save_xyp, xypnew, xypnewi * sizeof(struct GXYPAIRSQ));
	*pnumber = newpnum;
	g_pq.npanels = newpnum;
	g_pq.npts = xypnewi;
split_holes_end:
	gpcfree((void **)& sxyp);
	gpcfree((void **)& xypnew);
	gpcfree((void **)& xhc);
	xl_cropq = xl_crops;
	xh_cropq = xh_crops;
}

/**********************************************************************/
/* Compare the centers of the panel hole arrays. MUST be cdecl. */
/**********************************************************************/

int G_CDECL compcentq(const void *arg1, const void *arg2)
{
	int *x, *y;

	x = (int *)arg2;
	y = (int *)arg1;

	return(*y - *x);	/* Ascending order */
}

#endif
