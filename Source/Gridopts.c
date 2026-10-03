/* (c) 1993-1995  by Scientific Endeavors Corp.
This module contains all of the routines that are used to set up the plot
gridding options.
V 7.0 3/3/93
*************************************************************************/
#include <graphic.h>

/**************************************************************/
/* Enable/disable the drawing of the x and/or y axes with or without
labels. Bit wise comparisons are done on the varible axis_flgq to
determine action.
	bit 0 - all axes and labels if FALSE		GPC_AXESON	= 0x00
    bit 0 - no axes or labels if TRUE			GPC_AXESOFF	= 0x01
    bit 1 - x-axis with no labels				GPC_XAXIS	= 0x02
    bit 2 - y-axis with no labels				GPC_YAXIS	= 0x04
    bit 3 - x-axis labels						GPC_XLAB	= 0x08
    bit 4 - y-axis labels						GPC_YLAB	= 0x10
*/
void TYPE GCLASS axesoff(int noaxes)
{
	G_PROCESS_ID

	if (noaxes == GPC_AXESON)
		/* Axes with labels */
		axis_flgq = GPC_XAXIS | GPC_YAXIS | GPC_XLAB | GPC_YLAB;
	else
		axis_flgq = noaxes;
}
/**************************************************************/
/* ENABLE DRAWING A GRID ON THE GRAPHS */
void TYPE GCLASS grid(int gridon)
{
	xgrid(gridon);
	ygrid(gridon);
}
/**************************************************************/
/* ENABLE DRAWING A GRID ON THE X-AXIS ONLY */
void TYPE GCLASS xgrid(int gridon)
{
	G_PROCESS_ID

	xgrid_flgq = gridon;
}
/**************************************************************/
/* ENABLE DRAWING A GRID ON THE GRAPHS */
void TYPE GCLASS ygrid(int gridon)
{
	G_PROCESS_ID

	ygrid_flgq = gridon;
}

/* fon is the line style. In order to have ticks
only, -1 is used. */
/************************************************************/
/* SUBDIVIDE THE GRID INTO nx X-DIVISIONS AND ny Y-DIVISIONS */
void TYPE GCLASS fgrid(int fon, int ndiv)
{
	xfgrid(fon, ndiv);
	yfgrid(fon, ndiv);
}
/************************************************************/
/* SUBDIVIDE THE GRID INTO nx X-DIVISIONS */
void TYPE GCLASS xfgrid(int fon, int nx)
{
	G_PROCESS_ID

	nxfineq = nx + 1;
	xfgrid_flgq = fon;
}
/************************************************************/
/* SUBDIVIDE THE GRID INTO ny Y-DIVISIONS */
void TYPE GCLASS yfgrid(int fon, int ny)
{
	G_PROCESS_ID

	nyfineq = ny + 1;
	yfgrid_flgq = fon;
}
/**********************************************************/
/* IF FRAMEQ == 1, DRAW A FRAME AROUND PLOT AREA */
void TYPE GCLASS frame(char frameon, char ftick)
{
	G_PROCESS_ID

	if (frameon) {
		frame_flgq = DRAW_FRAME;
		if (ftick)
			frame_flgq |= DRAW_FRAME_TICKS;
	}
	else
		frame_flgq = 0;
}
/**************************************************************/
/* CROSSED AXES - PARAMETER SETTING */
/* crossq.flg bits are used as follows:
	All bits on -->	 User requested crossed axes. Bits are turned off when
	axes are on the edge of the plot. The bit wise tests are used to
	determine the following:
	Bit 1 --> Axes are crossed. Bits 2 or 3 may also be on meaning the
	          axes may not be crossed inside the plot area.
	    2 --> X axis is on the bottom edge.
	    3 --> Y axis is on the left edge.
	    4 --> User requested crossed axes.
*/
void TYPE GCLASS cross(char iscross)
{
	G_PROCESS_ID

	if (iscross)
		crossq.flg = USER_WANTS_CROSSED_AXES;
	else
		crossq.flg = X_AXES_ON_BOTTOM | Y_AXES_ON_LEFT;
}
/*******************************************************/
void TYPE GCLASS tickout(int is_out)
{
	G_PROCESS_ID

	if (is_out)
		tkhtq = ( - gabs(tkhtq));
	else
		tkhtq = gabs(tkhtq);
}
/*******************************************************/
void TYPE GCLASS tickht(float height)
{
	G_PROCESS_ID

	tkhtq = (int)((float)intopix(height) *
		((float)((xpageq > ypageq) ?
			xpageq : ypageq) / (float)MAXRESXMO));
}
