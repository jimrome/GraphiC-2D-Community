/* (c) 1984-1993 by Scientific Endeavors Corporation.
   All rights reserved.
This file is used to generate axes with grid lines, tick marks and
labels.
Version 7.0 6/14/93
**************************************************************/
#include <graphic.h>
#include <axesdraw.h>

#define X_ON_EDGE (X_ON_TOP || X_ON_BOTTOM)
#define Y_ON_EDGE (Y_ON_LEFT || Y_ON_RIGHT)
#define X_END xphysorq + xlenq
#define Y_END yphysorq + ylenq

#if MSQ
#pragma optimize("", off)
#endif
/**************************************************************
Plots and labels the x axis. */
int GCLASS xupq(int nxstep, char *xformat, int npix, float orig,
	int start, PLABFUNC lab, int lab_flag)
{
	float temp;
	int on_edge;
	int lab_spc;
	int above;
	int shift;
	char *xl;
	int xtick, i, j = 0;
	int width;
	int xfgrid_step, xdraw;
	int xp, yp;
	int ang;
	G_PROCESS_ID

				/* DRAW X-AXIS */
	xp = X_END;
	yp = Y_END;
	for (i = 0; i <= 4; i++) {		/*DRAW AXIS */
		if (!X_ON_BOTTOM && (axis_flgq & GPC_XAXIS)) {
			lineq(xphysorq, yzeroq + i, xp, yzeroq + i);
			lineq(xphysorq, yzeroq - i, xp, yzeroq - i);
		}

							/*DO BOTTOM BORDER OR X AXIS */
		if ((X_ON_BOTTOM && (axis_flgq & GPC_XAXIS)) || frame_flgq)
			lineq(xphysorq - i, yphysorq - i, xp, yphysorq - i);

		if (frame_flgq) {		/*DO TOP BORDER*/
			lineq(xphysorq - 4, yp + i, xp + 4, yp + i);
		}
	}

			/* LAY DOWN GRID LINES */
	if (xfgrid_flgq && nxfineq)
		xfgrid_step = npix / nxfineq;
	xtick = start;

	for (i = 0; i <= nxstep; i++) {
		if (lab_flag & X_TICK_POS)
			xtick = XTickPosq(0, (GTICK_POS *)NULL, i + 1);

		/* If there is a frame or a right hand Y axis and
			the current position is at the right end of the axis. */
		if ((frame_flgq || right_flgq) && xtick + 5 >= X_END) {
			if (tkhtq < 0)		/* If the tick points out. */
				tickdrwq(1, xtick, xp, yp);
			break;	/* Axis or frame edge will be drawn here */
		}

		/* If there is a frame and
			the current position is not at the left end of the axis or
			any time there is not a frame. */
		if ((frame_flgq && xtick - 4 > xphysorq) || (!frame_flgq)) {
			if (xgrid_flgq && (xtick < xzeroq - 4 || xtick
				 > xzeroq + 4)) {
				if (xgrid_flgq > 0) {
					dashf(xgrid_flgq);
					lineq(xtick, yphysorq, xtick, yp);
					dashf(1);
				}
			}
			tickdrwq(1, xtick, xp, yp);
		}

		/* or any time the ticks point out. */
		else if (tkhtq < 0)
			tickdrwq(1, xtick, xp, yp);

			/* MAKE FINE GRID */
		if (xfgrid_flgq && i < nxstep) {
			xdraw = xtick;
			tkhtq /= 2;
			for (j = 1; j < nxfineq; j++) {
				xdraw += xfgrid_step;
				if (xdraw < xzeroq - 4 || xdraw > xzeroq + 4) {
					if (xfgrid_flgq > 0) {
						dashf(xfgrid_flgq);
						lineq(xdraw, yphysorq, xdraw, yp);
						dashf(1);
					}
				}
				tickdrwq(1, xdraw, xp, yp);
			}
			tkhtq *= 2;
		}
		if (!(lab_flag & X_TICK_POS))
			xtick += npix;
		if (xtick - 5 > X_END)
			break;
	}
	if (!(axis_flgq & GPC_XLAB))
		return(0);

				/* LABEL X-AXIS */
	if (X_ON_TOP)
		above = 1;
	else
		above = 0;
	xtick = start;
	temp = orig;

	for (i = 0; i <= nxstep; i++) {
		if (lab_flag & X_TICK_POS) {
			xtick = XTickPosq(0, (GTICK_POS *)NULL, i + 1);
			temp = ((float)(xtick - xphysorq) / xfact_pltq) + orig;
		}
		if (lab_flag & X_STRING_LABELS) {
			if (lab_flag & REVERSE_INDEX)
				xl = (PMF_GCLASS lab)(0, NULL, (nxstep - i) + 1);
			else
				xl = (PMF_GCLASS lab)(0, NULL, i + 1);
		}
		else {
			temp = (float)roundoffq((double)temp, .0001);
			xl = (char *)gpcalloc((size_t)LABLEN, sizeof(char));
			kprintfq(xl, xformat, temp);
		}
		if (xl[0] == '\0' || xl == NULL) {	/* Empty string. */
			if (lab_flag & X_TICK_POS)
				xtick = XTickPosq(0, (GTICK_POS *)NULL, i + 1);
			else
				xtick += npix;
			if (!(lab_flag & X_STRING_LABELS))
				gpcfree((void **)&xl);
			else {
				if (lab_flag & REVERSE_INDEX)
					temp -= xstepq;
				else
					temp += xstepq;
			}
			continue;
		}
		width = strwidtq(xl, lablhtq);
		ang = x_lab_angq(0, 1);
		if (ang == DEFAULT_INT)
			ang = 0;

		shift = 0;
		on_edge = 1;
		if (!X_ON_EDGE) {
			on_edge = -1;
			if ((Y_ON_LEFT || frame_flgq) && i == 0)
				shift = 1;
			else if ((Y_ON_RIGHT || frame_flgq) && i == nxstep)
				shift = 2;
			if (!Y_ON_EDGE)
				shift |= 4;
		}
		if (((wplane_flgq & V1_Z_PLANE) && i == nxstep) ||
			((wplane_flgq & V2_Z_PLANE) && i == 0)) {
			on_edge = -1;
			if (i == 0)
				shift = 1;
			else
				shift = 2;
		}
		lab_spc = get_lab_posq(ang, lablhtq, above, on_edge, &xp, &yp,
			width, xlspcq, xphysorq, xtick, xzeroq, i, shift);
		if (above)
			yp += yzeroq;
		else
			yp = yzeroq - yp;
		if (!X_ON_EDGE)
			yp -= (tkhtq != 0) ? gabs(tkhtq) : 0;
		fntpltq(xp, yp, xl, lablhtq, ang);
		if (!(ang == 0 || ang == 180)) {
			if (j < lab_spc)
				j = lab_spc;
		}
		else
			j = (int)lablhtq + xlspcq + ((tkhtq < 0) ? -tkhtq : 0);
		if (!(lab_flag & X_TICK_POS))
			xtick += npix;
		if (!(lab_flag & X_STRING_LABELS)) {
			gpcfree((void **)&xl);
			temp += xstepq;
		}
		if (xtick - 5 > X_END)
			break;
	}
	lab_wid_setq(GPC_X_AXIS, j);
	return(j);
}
/********************************************************
Plots and labels the y axis. */
int GCLASS yupq(int nystep, char *yformat, int npix, float orig,
	int start, PLABFUNC lab, int lab_flag)
{
	float temp;
	int on_edge;
	int lab_spc;
	int above;			/* false if the top of label is closest to axes. */
	int shift;
	char *yl;
	int ytick, i, j = 0;
	int width, ang;
	int yfgrid_step, ydraw;
	int xp, yp;
	G_PROCESS_ID

				/* DRAW Y-AXIS */
	xp = X_END;
	yp = Y_END;
	if (right_flgq & LEFT_AXIS_DRAWN)
		right_flgq |= DRAWING_RIGHT_AXIS;
	else {
		for (i = 0; i <= 4; i++) {
			if (!Y_ON_LEFT && (axis_flgq & GPC_YAXIS)) {
				lineq(xzeroq + i, yphysorq, xzeroq + i, yp);
				lineq(xzeroq - i, yphysorq, xzeroq - i, yp);
			}
							/* Draw left border or y axis */
			if ((Y_ON_LEFT && (axis_flgq & GPC_YAXIS)) || frame_flgq)
				lineq(xphysorq - i, yphysorq - i, xphysorq - i, yp);
		}
	}

	            /*** DRAW RIGHT HAND Y-AXIS OR RIGHT BORDER **/
	if ((right_flgq & LEFT_AXIS_DRAWN) || frame_flgq) {
		for (i = 0; i <= 4; i++)
			lineq(xp + i, yphysorq - 4, xp + i, yp);
	}

			/* LAY DOWN GRID LINES */
	if (yfgrid_flgq && npix)
		yfgrid_step = npix / nyfineq;
	ytick = start;

	for (i = 0; i <= nystep; i++) {
		if (lab_flag & Y_TICK_POS)
			ytick = YTickPosq(0, (GTICK_POS *)NULL, i + 1);

		/* If there is a frame and
			the current position is at the top of the axis. */
		if (frame_flgq && ytick + 5 >= Y_END) {
			if (tkhtq < 0)		/* If the tick points out. */
				tickdrwq(0, ytick, xp, yp);
			break;
		}

		/* If there is a frame and
			the current position is not at the bottom or
			any time there is not a frame. */
		if ((frame_flgq && ytick - 4 > yphysorq) || !frame_flgq) {
			if (ytick > yzeroq + 4 || ytick < yzeroq - 4) {
				if (ygrid_flgq > 0) {
					dashf(ygrid_flgq);
					if (!(right_flgq & DRAWING_RIGHT_AXIS))
						lineq(xphysorq, ytick, xp, ytick);
					dashf(1);
				}
			}
			tickdrwq(0, ytick, xp, yp);
		}

		/* or any time the ticks point out. */
		else if (tkhtq < 0)
			tickdrwq(0, ytick, xp, yp);

			/* MAKE FINE GRID */
		if (yfgrid_flgq && ytick < Y_END) {
			ydraw = ytick;
			tkhtq /= 2;
			for (j = 1; j < nyfineq; j++) {
				ydraw += yfgrid_step;
				if (yfgrid_flgq > 0 && ytick < Y_END) {
			   		dashf(yfgrid_flgq);
					lineq(xphysorq, ydraw, xp, ydraw);
					dashf(1);
				}
				tickdrwq(0, ydraw, xp, yp);
			}
			tkhtq *= 2;
		}

		if (!(lab_flag & Y_TICK_POS))
			ytick += npix;
		if (ytick - 5 > Y_END)
			break;
	}

	if (!(axis_flgq & GPC_YLAB)) {
		if (right_flgq && (!(right_flgq & LEFT_AXIS_DRAWN)))
			right_flgq |= LEFT_AXIS_DRAWN;
		else
			right_flgq &= ~LEFT_AXIS_DRAWN;
		return(0);
	}

			/* LABEL Y-AXIS */
	ytick = start;
	temp = orig;
	j = 0;

	if (Y_ON_RIGHT || (right_flgq & DRAWING_RIGHT_AXIS))
		above = 0;
	else
		above = 1;
	for (i = 0; i <= nystep; i++) {
		if (lab_flag & Y_TICK_POS) {
			ytick = YTickPosq(0, (GTICK_POS *)NULL, i + 1);
			temp = ((float)(ytick - yphysorq) / yfact_pltq) + orig;
		}
		else if (i == 0 && (wplane_flgq == 0x10 || wplane_flgq == 0x20)) {
		/* Do not print bottom Z axis label */
			ytick += npix;
			temp += ystepq;
			continue;
		}
		temp = (float)roundoffq((double)temp, .0001);
		if (lab_flag & Y_STRING_LABELS) {
			if (lab_flag & REVERSE_INDEX)
				yl = (PMF_GCLASS lab)(0, NULL, (nystep - i) + 1);
			else
				yl = (PMF_GCLASS lab)(0, NULL, i + 1);
		}
		else {
			yl = (char *)gpcalloc((size_t)LABLEN, sizeof(char));
			kprintfq(yl, yformat, temp);
		}
		if (yl[0] == '\0' || yl == NULL) {	/* Empty string. */
			if (!(lab_flag & Y_TICK_POS))
				ytick += npix;
			if (!(lab_flag & Y_STRING_LABELS))
				gpcfree((void **)&yl);
			else {
				if (lab_flag & REVERSE_INDEX)
					temp -= ystepq;
				else
					temp += ystepq;
			}
			continue;
		}
		width = strwidtq(yl, lablhtq);

		ang = y_lab_angq(0, 1);
		if (ang == DEFAULT_INT)
			ang = 0;
		shift = 0;
		on_edge = 1;
		if (!(lab_flag & Y_TICK_POS)) {
			if (Y_ON_EDGE && X_ON_BOTTOM && i == 0)
				shift = 1;
			else if (Y_ON_EDGE && X_ON_TOP && i == nystep)
				shift = 2;
			else if (!Y_ON_EDGE) {
				on_edge = -1;
				if ((X_ON_BOTTOM || frame_flgq) && i == 0)
					shift = 1;
				else if ((X_ON_TOP || frame_flgq) && i == nystep)
					shift = 2;
				if (!X_ON_EDGE)
					shift |= 8;		/* shift down at axes crossing. */
			}
		}
		if (i == 0)
			ydraw = yphysorq;
		else
			ydraw = yphysorq + ylenq;
		lab_spc = get_lab_posq(ang, lablhtq, above, on_edge, &yp, &xp,
			width, ylspcq, ydraw, ytick, yzeroq, i, shift);
		if (above)
			xp = xzeroq - xp;
		else if (right_flgq & DRAWING_RIGHT_AXIS)
			xp += (xphysorq + xlenq);
		else
			xp += xzeroq;
		if (!Y_ON_EDGE)
			xp -= (tkhtq != 0) ? gabs(tkhtq) : 0;
		fntpltq(xp, yp, yl, lablhtq, ang + 90);
		if (!(ang == 0 || ang == 180)) {
			if (j < lab_spc)
				j = lab_spc;
		}
		else
			j = (int)lablhtq + ylspcq + ((tkhtq < 0) ? -tkhtq : 0);
		if (!(lab_flag & Y_TICK_POS))
			ytick += npix;
		if (!(lab_flag & Y_STRING_LABELS)) {
			gpcfree((void **)&yl);
			temp += ystepq;
		}
		if (ytick - 5 > Y_END)
			break;
	}
	lab_wid_setq(GPC_Y_AXIS, j);
	if (right_flgq && (!(right_flgq & LEFT_AXIS_DRAWN)))
		right_flgq |= LEFT_AXIS_DRAWN;
	else
		right_flgq &= ~LEFT_AXIS_DRAWN;
	return(j);
}
/*************************************************************
This routine draws tickmarks */
void GCLASS tickdrwq(int x_y, int pos, int xp, int yp)
{
	G_PROCESS_ID

	if (tkhtq == 0)
		return;
	if (x_y) {			/* for x-axis */
		if (axis_flgq & GPC_XAXIS) {
			if (X_ON_TOP)
				lineq(pos, yzeroq, pos, yzeroq - tkhtq);
			else if (!X_ON_BOTTOM)
				lineq(pos, yzeroq + tkhtq, pos, yzeroq - tkhtq);
			else
				lineq(pos, yzeroq, pos, yzeroq + tkhtq);
		}
		if (frame_flgq & DRAW_FRAME_TICKS) {
			if (!X_ON_BOTTOM)
				lineq(pos, yphysorq, pos, yphysorq + tkhtq);
			lineq(pos, yp, pos, yp - tkhtq);
		}
	}
	else {				/* for y-axis */
		if (axis_flgq & GPC_YAXIS) {
			if (Y_ON_RIGHT)
				lineq(xzeroq, pos, xzeroq - tkhtq, pos);
			else if (!Y_ON_LEFT)
				lineq(xzeroq + tkhtq, pos, xzeroq - tkhtq, pos);
			else {
				if (!(right_flgq & DRAWING_RIGHT_AXIS))
					lineq(xzeroq, pos, xzeroq + tkhtq, pos);
				else
					lineq(xp, pos, xp - tkhtq, pos);
			}
		}
		if (frame_flgq & DRAW_FRAME_TICKS) {
			if (!right_flgq)
				lineq(xp, pos, xp - tkhtq, pos);
			if (!Y_ON_LEFT)
				lineq(xphysorq, pos, xphysorq + tkhtq, pos);
		}
	}
}
/******************************************************************/
int GCLASS get_lab_posq(
	int ang,			/* angle relative to axis. */
	float lablht,		/* label height. */
	int above,			/* true if the labels are on top of the axis. */
	int on_edge,		/* 1 for low edge, 2 for high edge. */
	int *xpos,			/* return value along the axis. */
	int *ypos,			/* return value perpendicular to the axis. */
	int width,			/* string width. */
	int spc,			/* label spacing. */
	int org,
	int tick,			/* location of label in pixels. */
	int zero,			/* location of the axes crossing point in pixels. */
	int count,			/* n'th label. */
	int shift_flg)		/* shift label at edges yes/no. */
{
	double angle;
	int itemp;
	int shift;
	int xp, yp;
	int retval;
	float sin_fact;
	float cos_fact;
	G_PROCESS_ID;

	xp = 0;
	shift = 0;
	if (!(ang % 90)) {
		if (ang == 0 || ang == 180)
			itemp = width;
		else
			itemp = (int)(lablht / (float)2.);
		shift = shift_flg & 3;
		if (shift_flg > 3)
			org = zero;
		if ((shift_flg & 4) &&
			((tick + itemp) > zero) &&
			((tick - itemp) < zero) &&
			shift == 0)
				shift = 1;
		else if ((shift_flg & 8) &&
			((tick + itemp) > zero) &&
			((tick - itemp) < zero) &&
			shift == 0)
				shift = 2;

		if (shift == 1) {		/* at left edge */
			/* shift right along axis. */
			if (ang == 0 || ang == -90) {
				xp = tick - (spc * on_edge);
				if (xp > (org - (spc * on_edge)))
					xp = org - (spc * on_edge);
			}
			else if (ang == 180 || ang == 90) {
				xp = tick + (2 * itemp) - (spc * on_edge);
				if ((xp - (2 * itemp)) > (org - (spc * on_edge)))
					xp = (org - (spc * on_edge)) + (2 * itemp);
			}
		}
		else if (shift == 2) {	/* at right edge */
			/* shift left along axis. */
			if (ang == 0 || ang == -90) {
				xp = tick - (2 * itemp) + (spc * on_edge);
				if ((xp + (itemp * 2)) < (org + (spc * on_edge)))
					xp = (org + (spc * on_edge)) - (itemp * 2);
			}
			else if (ang == 180 || ang == 90) {
				xp = tick + (spc * on_edge);
				if (xp < (org + (spc * on_edge)))
					xp = org + (spc * on_edge);
			}
		}
		else {
			if (ang == 0 || ang == -90)
				xp = tick - itemp;
			else
				xp = tick + itemp;
		}
		yp = spc + ((tkhtq < 0) ? -tkhtq : 0);
		if (above) {
			if (ang == 0 || ang == 90)
				shift = 0;
			if (ang == 180)
				shift = (int)lablht;
			else if (ang == -90)
				shift = width * 2;
		}
		else {
			if (ang == 180 || ang == -90)
				shift = 0;
			if (ang == 0)
				shift = (int)lablht;
			else if (ang == 90)
				shift = width * 2;
		}
		*ypos = yp + shift;
		*xpos = xp;
		retval = ((tkhtq < 0) ? -tkhtq : 0) + spc;
		if (ang == 0 || ang == 180)
			retval += (int)(lablht * (float)1.3 + (float).5);
		else
			retval += (width * 2) + spc;
	}
	else {
		angle = (double)ang / DEGREES_PER_RADIANS;
		shift = ((tkhtq < 0) ? -tkhtq : 0);
		sin_fact = (float)sin(angle);
		cos_fact = (float)cos(angle);
		if (count == 0 && shift_flg)
			shift_flg = (int)(gabs(cos(angle) * (lablht / (float)2.)));
		else
			shift_flg = 0;
		retval = (int)((fabs(sin_fact) * (float)(width * 2)) +
			(fabs(cos_fact) * lablht) + ((fabs(sin_fact) *
			(float)(spc + shift))));

		if (ang > 0 && ang < 90) {
			if (above) {
				xp = tick + (int)(sin_fact * lablht / (float)2.);
				yp = (int)(sin_fact * (float)(spc + shift));
			}
			else {
				xp = (tick - (int)(cos_fact * (float)(width * 2))) +
					(int)(sin_fact * lablht / (float)2.);
				yp = retval;
			}
		}
		else if (ang > 90 && ang < 180) {
			if (above) {
				xp = tick + (int)(sin_fact * lablht / (float)2.);
				yp = (int)(sin_fact * (float)(spc + shift)) -
					(int)(cos_fact * lablht);
			}
			else {
				xp = (tick - (int)(cos_fact * (float)(width * 2))) +
					(int)(sin_fact * lablht / (float)2.);
				yp = (int)(sin_fact * (float)(width * 2) +
					(sin_fact * (float)(spc + shift)));
			}
		}
		else if (ang < 0 && ang > -90) {
			if (above) {
				xp = (tick - (int)(cos_fact * (float)(width * 2))) +
					(int)(sin_fact * lablht / (float)2.);
				yp = -((int)(sin_fact * (float)(width * 2) +
					(sin_fact * (float)(spc + shift))));
			}
			else {
				xp = tick + (int)(sin_fact * lablht / (float)2.);
				yp = (int)(cos_fact * (float)(spc + shift)) +
					(int)(cos_fact * lablht);
			}
		}
		else {
			if (above) {
				xp = (tick - (int)(cos_fact * (float)(width * 2))) +
					(int)(sin_fact * lablht / (float)2.);
				yp = -((int)(sin_fact * (float)(width * 2) +
					(cos_fact * lablht) + (sin_fact * (float)(spc + shift))));
			}
			else {
				xp = tick + (int)(sin_fact * lablht / (float)2.);
				yp = -((int)(sin_fact * (float)(spc + shift)));
			}
		}
		*xpos = xp + shift_flg;
		*ypos = yp;
	}
	return(retval);
}
/******************************************************************/
/* This routine sets the linear axes cross point according to the user
specified location. The units are in plot units. */
void TYPE GCLASS CrossAt(float x, float y)
{
	G_PROCESS_ID

	crossq.x = x;
	crossq.y = y;
	cross(1);
	crossq.flg |= USER_SET;
}
