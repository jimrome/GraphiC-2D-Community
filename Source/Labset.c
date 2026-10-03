/* (c) 1984-1993 by Scientific Endeavors Corporation.
   All rights reserved.
This file contains routines for managing user specified label attributes.

Version 7.0 1/2/92
**************************************************************/
#include <graphic.h>

/*************************************************************
These functions are called to put string labels at the x, y or z tick
marks. The user makes a structure of the form : int flag;
string1[10]; string2[10]; --- , and initializes it with flag = 1 to
turn on the option and with the strings desired. He calls
xlab(st) or ylab(st) with a pointer to the structure as argument. */

/***************************************************************
User called routine to set up x axis string labels. The arguments are just
passed to xlabq(). */
void TYPE GCLASS xlab(char on_off, char *strlab[])
{
	xlabq(on_off, strlab, 0);
}

/***************************************************************
User called routine to set up y axis string labels. The arguments are just
passed to ylabq(). */
void TYPE GCLASS ylab(char on_off, char *strlab[])
{
	ylabq(on_off, strlab, 0);
}

/***************************************************************
User called routine to set up z axis string labels. The arguments are just
passed to zlabq(). */
void TYPE GCLASS zlab(char on_off, char *strlab[])
{
	zlabq(on_off, strlab, 0);
}

/**************************************************************/
void TYPE GCLASS xLabAng(int angle)
{
	x_lab_angq(angle, 0);
}

/**************************************************************/
void TYPE GCLASS yLabAng(int angle)
{
	y_lab_angq(angle, 0);
}

/**************************************************************/
void TYPE GCLASS zLabAng(int angle)
{
	z_lab_angq(angle, 0);
}

/*#####################################################################*/
/* GraphiC makes internal calls to these routines to retrieve the user's
input. If on_off is 0 or strlab is NULL the global flag lab_flgq has the
bit marking x axis labeling options turned off. The address of the pointer
containing the strings is passed in so that graphic can free the memory at
the end of the program. */

char * GCLASS xlabq(int on_off, char **strlab, int action)
{
	G_PROCESS_ID

	if (action <= 0) {		/* call to load array */
		if (!on_off || *strlab == NULL) {
			lab_flgq &= ~X_STRING_LABELS;
			return(NULL);
		}
		if (action != -1)	/* action = -1 if from an internal call. */
			lab_flgq |= X_STRING_LABELS;
		xstrlbq = strlab;
		return(xstrlbq[0]);
	}
	else if (action > 0)	/* Internal call */
		return(xstrlbq[action - 1]);
	return(NULL);
}
/************************************************************
If on_off is 0 or strlab is NULL the global flag lab_flgq has the
bit marking y axis labeling options turned off. */
char * GCLASS ylabq(int on_off, char **strlab, int action)
{
	G_PROCESS_ID

	if (action <= 0) {
		if (!on_off || *strlab == NULL) {
			lab_flgq &= ~Y_STRING_LABELS;
			return(NULL);
		}
		if (on_off != -1)
			lab_flgq |= Y_STRING_LABELS;
		ystrlbq = strlab;
		return(ystrlbq[0]);
	}
	if (action > 0)
		return(ystrlbq[action - 1]);
	return(NULL);
}
/************************************************************
If on_off is 0 or strlab is NULL the global flag lab_flgq has the
bit marking z axis labeling options turned off. */
char * GCLASS zlabq(int on_off, char **strlab, int action)
{
	G_PROCESS_ID

	if (action <= 0) {
		if (!on_off || *strlab == NULL) {
			lab_flgq &= ~Z_STRING_LABELS;
			return(NULL);
		}
		if (on_off != -1)
			lab_flgq |= Z_STRING_LABELS;
		zstrlbq = strlab;
		return(zstrlbq[0]);
	}
	else if (action > 0)
		return(zstrlbq[action - 1]);
	return(NULL);
}
/**************************************************************/
int GCLASS x_lab_angq(int angle, int action)
{
	G_PROCESS_ID

	if (action)
		return(x_angq);
	else {
		if (angle != DEFAULT_INT) {
			while (angle >= 360)
				angle -= 360;
			while (angle < 0)
				angle += 360;
			if (angle > 180)
				angle -= 360;
		}
		x_angq = angle;
	}
	return(0);
}
/**************************************************************/
int GCLASS y_lab_angq(int angle, int action)
{
	G_PROCESS_ID

	if (action)
		return(y_angq);
	else {
		if (angle != DEFAULT_INT) {
			while (angle >= 360)
				angle -= 360;
			while (angle < 0)
				angle += 360;
			if (angle > 180)
				angle -= 360;
		}
		y_angq = angle;
	}
	return(0);
}
/**************************************************************/
int GCLASS z_lab_angq(int angle, int action)
{
	G_PROCESS_ID

	if (action)
		return(z_angq);
	else {
		if (angle != DEFAULT_INT) {
			while (angle >= 360)
				angle -= 360;
			while (angle < 0)
				angle += 360;
			if (angle > 180)
				angle -= 360;
		}
		z_angq = angle;
	}
	return(0);
}
/******************************************
upright() sets the angle for the y axis labels to -90 which results in the
y axis labels being upright. */
void TYPE GCLASS upright(char uplab)
{
	if (uplab)
		y_lab_angq(-90, 0);
	else
		y_lab_angq(0, 0);
}
/********************************************************************
Use this routine to establish the locations of the tick marks along
the X axis. The array passed in is in plot units.
********************************************************************/
void TYPE XTickPos(int on_off, GTICK_POS *array)
{
	XTickPosq(on_off, array, 0);
}
/********************************************************************
Use this routine to establish the locations of the tick marks along
the Y axis. The array passed in is in plot units.
********************************************************************/
void TYPE YTickPos(int on_off, GTICK_POS *array)
{
	YTickPosq(on_off, array, 0);
}
/********************************************************************
Use this routine to establish the locations of the tick marks along
the Z axis. The array passed in is in plot units.
********************************************************************/
void TYPE ZTickPos(int on_off, GTICK_POS *array)
{
	ZTickPosq(on_off, array, 0);
}

/********************************************************************
The next 3 routines return the pixel location of the specified tick mark
based on the value of action. if action is > 0 then it is 1 greater
than the offset into the tickpos array. ie value = tickpos[action - 1]
********************************************************************/
int XTickPosq(int on_off, GTICK_POS *array, int action)
{
	G_PROCESS_ID

	if (action <= 0) {		/* call to load array */
		if (!on_off || array == NULL) {
			lab_flgq &= ~X_TICK_POS;
			return(0);
		}
		if (action != -1)	/* action = -1 if from an internal call. */
			lab_flgq |= X_TICK_POS;
		x_tickposq = array;
		return(0);
	}
	else if (action > 0)	/* Internal call */
		return (xphysorq + (int)(XSCALE_PLOT(
			x_tickposq -> tickpos[action - 1] - xorigq) + .5));
	return(0);
}
int YTickPosq(int on_off, GTICK_POS *array, int action)
{
	G_PROCESS_ID

	if (action <= 0) {		/* call to load array */
		if (!on_off || array == NULL) {
			lab_flgq &= ~Y_TICK_POS;
			return(0);
		}
		if (action != -1)	/* action = -1 if from an internal call. */
			lab_flgq |= Y_TICK_POS;
		y_tickposq = array;
		return(0);
	}
	else if (action > 0)	/* Internal call */
		return (yphysorq + (int)(YSCALE_PLOT(
			y_tickposq -> tickpos[action - 1] - yorigq) + .5));
	return(0);
}
int ZTickPosq(int on_off, GTICK_POS *array, int action)
{
	G_PROCESS_ID

	if (action <= 0) {		/* call to load array */
		if (!on_off || array == NULL) {
			lab_flgq &= ~Z_TICK_POS;
			return(0);
		}
		if (action != -1)	/* action = -1 if from an internal call. */
			lab_flgq |= Z_TICK_POS;
		z_tickposq = array;
		return(0);
	}
	else if (action > 0)	/* Internal call */
		return (yphysorq + (int)(YSCALE_PLOT(
			z_tickposq -> tickpos[action - 1] - yorigq) + .5));
	return(0);
}
