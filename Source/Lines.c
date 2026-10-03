/*
	(c) 1984-1996 by Scientific Endeavors Corporation.
	All rights reserved.

	Miscellaneous line drawing routines.
	Version 7.0 12/12/91
********************************************************/
#include <graphic.h>

/****************************************************************/
/* USER ROUTINE TO DRAW A LINE IN PIXELS (MAXRESX x MAXRESY space ) */
void TYPE GCLASS line(
	int xfrom,	/* starting x value */
	int yfrom,	/* starting y value */
	int xto,		/* ending x value */
	int yto,		/* ending y value */
	int newcurv	/* 1 = start a new curve;   0 = continue an old curve */
)
{
	if (newcurv & 1)
		lmoveq(xfrom, yfrom);
	ldrawq(xto, yto);
}
/*************************************************************
USER ROUTINE TO DRAW A LINE IN USER (PLOT) UNITS */
void TYPE GCLASS uline(
	float xfrom,		/* starting x value */
	float yfrom,		/* starting y value */
	float xto,			/* ending x value */
	float yto,			/* ending y value */
	int newcurv		/* 1 = start a new curve; 0 = continue an old curve */
)
{
	int x0, y0, x1, y1;

	usertopix(xfrom, yfrom, 0.0f, (int *)&x0, (int *)&y0);
	usertopix(xto, yto, 0.0f, (int *)&x1, (int *)&y1);
	line(x0, y0, x1, y1, newcurv);
}

/*************************************************************
ALLOWS USER TO DRAW LINES USING INCH (OR CM) COORDINATES. */
void TYPE GCLASS linch(float x1, float y1, float x2, float y2, char newcurv)
{
	int xa, ya, xb, yb;
	G_PROCESS_ID

	xa = intopix(x1);
	ya = intopix(y1);
	xb = intopix(x2);
	yb = intopix(y2);
	line(xa, ya, xb, yb, newcurv);
}
