/* (c) 1984-1993 by Scientific Endeavors Corporation.
All rights reserved.
Version 7.0 4/6/92
**************************************************************/
#include <graphic.h>

/*************************************************************/
/* This function relates the universal grid variables to the
input x and y. */

void GCLASS getxyq(float v1, float v2, float *px, float *py, int units)
{
	float xorig, yorig, xmax, ymax;
	G_PROCESS_ID

	switch(units) {
		case USER_UNITS:
			xmax = xmaxq;
			ymax = ymaxq;
			xorig = xorigq;
			yorig = yorigq;
		break;
		case PIXEL_UNITS:
			xmax = x3axisq;
			ymax = y3axisq;
			xorig = 0.0f;
			yorig = 0.0f;
		break;
		default:
			xmax = 0.0f;
			ymax = 0.0f;
			xorig = 0.0f;
			yorig = 0.0f;
		break;
	}
	switch (viewers_quadq) {
		case FIRST_QUAD:
			*px = xmax - v1;
			*py = ymax - v2;
		break;
		case SECOND_QUAD:
			*px = xorig + v2;
			*py = ymax - v1;
		break;
		case THIRD_QUAD:
			*px = xorig + v1;
			*py = yorig + v2;
		break;
		case FORTH_QUAD:
			*px = xmax - v2;
			*py = yorig + v1;
		break;
	}
}
