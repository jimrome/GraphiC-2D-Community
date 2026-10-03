/* (c) 1984-1993 by Scientific Endeavors Corporation.
   All rights reserved.
Version 7.0 2/26/93
*************************************************************/

#include <graphic.h>
#include <arrow.h>

/**************************************************************/

void GCLASS vectorq(int x0, int y0, int x1, int y1, float ltow,
	float size, char *type)
{
	float headlen, headwid, vl;
	int dsh;
	float dx, dy;
#ifndef PANEL
	float tempx, tempy, templx, temply;
	int i;
#endif
	G_PROCESS_ID

	if (x0 == x1 && y0 == y1) {
		messageq(WARNQ, 27);	/* Can not plot a 0 length vector */
		return;
	}
	xfrom = (float)x0;
	xto = (float)x1;
	yfrom = (float)y0;
	yto = (float)y1;

	/* Parse type string:
	   Supports:
	   - 'N' or 'n' -> No head (plain line segment, full length)
	   - Classic SEC 2-digit format:
	       type[0]: '0'=solid filled, '1'=open barbed, '2'=outline triangle
	       type[1]: '0'=no head, '1'=head at to, '2'=heads at both ends
	   - Letter codes:
	       'F' or 'f' -> Filled head (style 0)
	       'O' or 'o' -> Open head (style 1)
	       'H' or 'h' -> Hollow outline head (style 2)
	       'A' or 'a' -> Arrow (standard filled head at to, style 0, placement 1)
	*/
	if (type == (char *)NULL || type[0] == '\0') {
		t0 = 0; /* Default: filled head */
		t1 = 1; /* at destination */
	}
	else if (type[0] == 'N' || type[0] == 'n' ||
	         (type[1] != '\0' && (type[1] == 'N' || type[1] == 'n' || type[1] == '0')) ||
	         (type[0] == '0' && type[1] == '0')) {
		t0 = 0;
		t1 = 0; /* No head: plain line segment */
	}
	else if (type[0] == 'F' || type[0] == 'f') {
		t0 = 0;
		t1 = (type[1] == '2' || type[1] == 'B' || type[1] == 'b') ? 2 : 1;
	}
	else if (type[0] == 'O' || type[0] == 'o') {
		t0 = 1;
		t1 = (type[1] == '2' || type[1] == 'B' || type[1] == 'b') ? 2 : 1;
	}
	else if (type[0] == 'H' || type[0] == 'h') {
		t0 = 2;
		t1 = (type[1] == '2' || type[1] == 'B' || type[1] == 'b') ? 2 : 1;
	}
	else if (type[0] == 'A' || type[0] == 'a') {
		t0 = 0;
		t1 = 1;
	}
	else if (type[0] >= '0' && type[0] <= '9') {
		t0 = type[0] - '0';
		t1 = (type[1] >= '0' && type[1] <= '9') ? (type[1] - '0') : 1;
	}
	else {
		t0 = 0;
		t1 = 1;
	}

	dsh = g_lq.pattern;
	dashf(1);	/* TURN OFF DOTTED LINES */
	dx = xto - xfrom;
	dy = yto - yfrom;
	vl = (float)sqrt((double)(dx * dx + dy * dy));
	if (vl != (float)0.0) {
		sn = dy / vl;	/* ROTATION ANGLES */
		cs = dx / vl;
	}
	else {
		sn = (float)1.0;
		cs = (float)0.0;
	}
	headlen = intopix(size) * xfact_pgq;
	headwid = headlen / ltow;
	hlx = headlen * cs;
	hly = headlen * sn;
	hwx = ( - headwid * sn);
	hwy = headwid * cs;

		/* DRAW THE BODY OF THE VECTOR. */

	if (t1 == 0) {
		/* NO HEAD: Draw full-length straight line segment */
		lineq((int)xfrom, (int)yfrom, (int)xto, (int)yto);
	}
#ifdef PANEL
	else if (t0 > 1)
#else
	else if (t0 != 1)
#endif
		lineq((int)xfrom, (int)yfrom, (int)xto, (int)yto);
											/* FULL LENGTH VECTOR */
	else {		/* CHOP OFF THE ENDS FOR OPEN BARBED HEAD */
		if (t1 >= 2)
		    lineq((int)(xfrom + hlx), (int)(yfrom + hly), (int)(xto - hlx),
		   		(int)(yto - hly));
		else
			lineq((int)xfrom, (int)yfrom, (int)(xto - hlx), (int)(yto - hly));
	}
			/*DRAW THE HEADS AND TAILS */

	if (t1 != 0) {
		arodrawq(1, t0);				/* DRAW THE HEAD */
		if (t1 > 1)
			arodrawq(0, t0); /* DRAW TAIL */
#ifndef PANEL
		if (t0 == 0) {	/* FILL IT IN */
			t0 = 2; /* Stop crossing the end. */
			templx = (float).25 * hlx;
			temply = (float).25 * hly;
			tempx = (float).25 * hwx;
			tempy = (float).25 * hwy;
			for (i = 0; i < 3; i++) {
				xto -= templx;
				yto -= temply;
				hlx -= templx;
				hly -= temply;
				hwx -= tempx;
				hwy -= tempy;
				arodrawq(1, t0);
				if (t1 == 2) {
					xfrom += templx;
					yfrom += temply;
					arodrawq(0, t0);
				}
			}
		}
#endif
	}
	dashf(dsh);	/*RESTORE USER'S LINE TYPE */
}
/**********************************************************
DRAW AN ARROW HEAD */

void GCLASS arodrawq(int end, int type)
{
	float x, y, delx, dely;
	int intx, inty;
#ifdef PANEL
	struct GXYPAIRSQ *newxyp;
#endif
	G_PROCESS_ID

	if (end) {
		x = xto;
		y = yto;
		delx = hlx;
		dely = hly;
	}
	else {
		x = xfrom;
		y = yfrom;
		delx = ( - hlx);
		dely = ( - hly);
	}
	intx = (int)x;
	inty = (int)y;
	int2x = (int)(x - delx + hwx);
	int2y = (int)(y - dely + hwy);
	int3x = (int)(x - delx - hwx);
	int3y = (int)(y - dely - hwy);
#ifdef PANEL
	if (type == 0) {
		newxyp = (struct GXYPAIRSQ *)gpcrealloc(g_pq.xyp,
			(size_t)(64 * sizeof(struct GXYPAIRSQ)));
		if (newxyp == (void *)NULL) {
			gpcfree((void **) &g_pq.xyp);
			messageq(ERRQ, 1);				/* Out of Memory */
		}
		else {
			g_pq.xyp = newxyp;
			if (wplane_flgq) {
			 	g_pq.nsides = 0;
				panfilq = 1;
				lmoveq(intx, inty);
				ldrawq(int2x, int2y);
				ldrawq(int3x, int3y);
				ldrawq(intx, inty);
			}
			else {
				g_pq.xyp[0].x = g_pq.xyp[3].x = intx;
				g_pq.xyp[0].y = g_pq.xyp[3].y = inty;
				g_pq.xyp[1].x = int2x;
				g_pq.xyp[1].y = int2y;
				g_pq.xyp[2].x = int3x;
				g_pq.xyp[2].y = int3y;
			 	g_pq.nsides = 4;
			}
		 	g_pq.pattern = (- g_lq.color);
		 	g_pq.border = 0;
		 	panelq();
			gpcfree((void **) &g_pq.xyp);
		 	panproc();
			g_pq.nsides = 0;
			panfilq = 0;
			return;
		}
	}
#endif
	lineq(intx, inty, int2x, int2y);
	lineq(intx, inty, int3x, int3y);
	if (type != 2)
		lineq(int2x, int2y, int3x, int3y);
}
/*************************************************************
uvector() draws a vector with the end positions in user units and
scaled with x. */

void TYPE GCLASS uvector(float xf, float yf, float xt, float yt,
	float ltow, float size, char *type)
{
	int x0, y0, x1, y1;
	G_PROCESS_ID

	usertopix(xf, yf, 0.0f, &x0, &y0);
	usertopix(xt, yt, 0.0f, &x1, &y1);
	if (!crop_flgq)
		acropq(1);
	vectorq(x0, y0, x1, y1, ltow, size, type);
	acropq(0);
}
/*************************************************************
vector() accepts inch units and passes them to vector. */

void TYPE GCLASS vector(float xf, float yf, float xt, float yt,
	float ltow, float size, char *type)
{
	int x0, y0, x1, y1;
	G_PROCESS_ID

	x0 = (int)((pix_convertq * xf) * xfact_pgq) + xbaseq;
	y0 = (int)((pix_convertq * yf) * yfact_pgq) + ybaseq;
	x1 = (int)((pix_convertq * xt) * xfact_pgq) + xbaseq;
	y1 = (int)((pix_convertq * yt) * yfact_pgq) + ybaseq;
	vectorq(x0, y0, x1, y1, ltow, size, type);
}
