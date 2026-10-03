/* (c) 1984-1995 by Scientific Endeavors Corporation.
   All rights reserved.
User calls bgnplot at start of plotting session. This file also contains
endplot(), stopplot(), initialq(), page(), area2d() and other files required
for all types of plots.
Version 7.0 8/7/92
Version 7.1 23-May-95    Moified to incorporate BPLTW.C extensions. LB
**************************************************************/

/* GraphiC AXIS VARIABLE NAMING DEFINITIONS
PIXEL VARIABLES (0<=horiz. coord <=MAXRESXMO) (0<=vert. coord<=MAXRESYMO)
xbaseq - x-location of lower left corner of plot area
ybaseq - y-location of lower left corner of plot area
xphysorq - x-location of lower left corner of grid
yphysorq - y-location of lower left corner of grid
xpageq - length of plot in x-direction
ypageq - length of plot in y-direction
xlenq - length of x-axis
ylenq - length of y-axis
xzeroq - x = 0.0 location if crossed axes, else = xphysorq
yzeroq - y = 0.0 location if crossed axes, else = yphysorq
tkhtq - height of a tick mark

VARIABLES IN PLOT COORDINATES
xorigq - value of x variable at left end of x-axis
yorigq - value of y variable at bottom of y-axis
xstepq - change in x variable between ticks
ystepq - change in y variable between ticks
xmaxq - value of x variable at right end of x-axis
ymaxq - value of y variable at top of y-axis

VARIABLES IN INCHES
xphysq - x distance from (xbase, ybase) to (xphysorq, yphysorq)
yphysq - y distance from (xbase, ybase) to (xphysorq, yphysorq)

**************************************************************/
#include <graphic.cmn>
#include <bplt.h>

#if GPC_WIN
#include "gpcwdefs.h" /* GraphiC menu defs and prototypes for Windows */
#include "gpcwin.h"

/* GPCWIN specific external variables */
extern GPCWIN_DATA gpcwin_data;	/* GPCWIN data structure */
extern char szAppName[128];

#endif

void save_pointerq(void);
#if GPC_CPP
/***************************************************************************
 * GraphiC++ class constructor initializes global/static variables         *
 ***************************************************************************/
GRAPHIC::GRAPHIC(void)
{
                                                      /* Zero entire class */
	memset(this, 0, sizeof(GRAPHIC));
                                                       /* Global Variables */
	leg_flgq = LEG_OFF;
                                                               /* CNTPLT.C */
#ifdef PANEL
	first_time = 1;
#endif
	cntclrbq = 1;
                                                               /* CURSOR.C */
	rowsetq = 12;
	colsetq = 0;
                                                                /* D3PLT.C */
	baseq = ( -1);
	z3axisq = (float)10.0;
	surtopq = 1;
	surbotq = 1;
                                                                /* R3PLT.C */
	resetc = 1;
	tol = (float)3.;
                                                                 /* TPLT.C */
#if GPC_UNX
	from_file = TRUE;
#endif
}
#endif

/*************************************************************/
/* INITIALIZATION OF VARIABLES */
void GCLASS initialq(void)
{
	int i;
	G_PROCESS_ID

		/* SET DEFAULT VALUES */
	notekq = 1;
	stringq = (char *)gpcalloc(256, sizeof(char));
	cmd_strq = (char *)gpcalloc(256, sizeof(char));
	tkfnameq = NULL;	/* Zero the buffer for the tkf name */

		/* SET TEK DEFAULTS TO IBM DEFAULTS */
	itotekq[0] = 0;
	itotekq[1] = 4;
	itotekq[2] = 3;
	itotekq[3] = 5;
	itotekq[4] = 2;
	itotekq[5] = 6;
	itotekq[6] = 8;
	itotekq[7] = 15;
	itotekq[8] = 14;
	itotekq[9] = 11;
	itotekq[10] = 9;
	itotekq[11] = 10;
	itotekq[12] = 13;
	itotekq[13] = 12;
	itotekq[14] = 7;
	itotekq[15] = 1;
		/* Array of PostScript Graftext fonts */
	PSfntarrayq[0] = 4;
	PSfntarrayq[1] = -1;
	PSfntarrayq[2] = -1;
	PSfntarrayq[3] = -1;
	PSfntarrayq[4] = -1;
	PSfntarrayq[5] = -1;
	PSfntarrayq[6] = -1;
	PSfntarrayq[7] = -1;
	PSfntarrayq[8] = -1;
	PSfntarrayq[9] = -1;
	PSlinewq = 1;

	/* SET IBM DEFAULTS TO TEK DEFAULTS. */
	cl_valq[0] = 0;
	cl_valq[1] = 15;
	cl_valq[2] = 4;
	cl_valq[3] = 2;
	cl_valq[4] = 1;
	cl_valq[5] = 3;
	cl_valq[6] = 5;
	cl_valq[7] = 14;
	cl_valq[8] = 6;
	cl_valq[9] = 10;
	cl_valq[10] = 11;
	cl_valq[11] = 9;
	cl_valq[12] = 13;
	cl_valq[13] = 12;
	cl_valq[14] = 8;
	cl_valq[15] = 7;

		/* FUNCTION PACKET INITIALIZATION */
	for (i = 0; i < 3; i++) {
		g_iq[i].gfunc = INITIALIZE;		/* Function code */
		g_iq[i].first = 1;
		g_iq[i].max_psides = 0;	/* 0 = no limit */
		if (i == 0) {
			g_iq[i].vmax = 479;	/* 0=video 1=printer */
			g_iq[i].hmax = 639;	/* resolutions */
		}
		else {
			g_iq[i].vmax = MAXRESYMO;	/* 0=video 1=printer */
			g_iq[i].hmax = MAXRESXMO;	/* resolutions */
		}
		g_iq[i].in_play = 0;
	}
#if GPC_OS2
	g_iq[VIDEO].bBmpbsflag = 1;
#endif

	g_scq.gfunc = SCRNTYPE;
	g_scq.gmode = 't';

	g_clsq.gfunc = CLEAR;
	g_clsq.color = WHITE;

	g_dotq.gfunc = DRAW_DOT;

	g_lq.gfunc = G_LINE;				/* Function code */
	g_lq.pattern = 1;			/* Line style */
	g_lq.penup = 1;				/* penup for simultaneous plots */
	g_lq.color = 0;
	g_lq.npts = 0;				/* Number of points for polyline. */

	g_pq.gfunc = PANL;
	g_pq.pattern = 0;
	ftableq = (GPC_LIBPROC)f_tableq;

	g_tq.gfunc = G_TEXT;		/* Function code */
	g_tq.height = 61;	/* Height in VIRTUAL pixels */
	g_tq.linesp = 92;	/* Space between lines */
	g_tq.space = 53;	/* Space increment between characters */

	g_cq.gfunc = G_CHAR;

	g_quitq.gfunc = QUIT;

	g_fopenq.gfunc = OPEN_FILE;
	g_fopenq.filehandle = -1;

	g_fcloseq.gfunc = CLOSE_FILE;

	g_fwriteq.gfunc = WRITE_FILE;

	g_cposq.gfunc = CURSOR;

	g_dumpq.gfunc = DUMP_PRINT;

	g_erq.gfunc = ERASE_CHAR;

	g_get_charq.gfunc = GET_CHAR;

	g_print_stringq.gfunc = PRINT_STRING;

	g_page_sizeq.gfunc = SIZE_PAGE;

#if GPC_WIN
	g_rdq.gfunc = REDRAW;
#endif

	ascq = 0;					/*ZOOM/PRINT OPTION VARIABLE*/
	symhtq = (float)30.0;		/*MULTIPLIER FOR 11 PIXEL SYMBOL HEIGHT*/
	labscalq = (float)1.0;		/*SCALING FACTOR FOR TEXT*/
#ifdef METRIC
	for (i = 0; i < 3; i++)
		g_iq[i].metric = 1;			/* Set to 1 if metric units */
	pix_convertq = 358.32f;		/* METRIC UNITS */
#else
	for (i = 0; i < 3; i++)
		g_iq[i].metric = 0;			/* Set to 0 if inch units */
	pix_convertq = 910.12f;		/* INCH UNITS */
#endif
	crossq.flg = X_AXES_ON_BOTTOM |
		Y_AXES_ON_LEFT;		/* Assume axes are at the plot edges. */
	crossq.x = crossq.y = 0.0f;
	acrossq = MAXRESXMO;
	topq = MAXRESYMO;
	titloffq = (int)((float).0122 * MAXRESXMO);
	surface_clrq = NO_COLOR;
	/* Turn on axes and labels. */
	axis_flgq = GPC_XAXIS | GPC_YAXIS | GPC_XLAB | GPC_YLAB;
	xh_cropq = MAXRESXMO;
	yh_cropq = MAXRESYMO;
	g_prntq.print_number = 0;		/* Assume print paper is at TOF */
	nxfineq = 1;
	nyfineq = 1;
	gflagq = 1;						/* Signals OK to do a scrntypeq('g'). */
	SetDefaultPrinter(1);			/* Initialize for PRINTER1.PRN */
	x_log_baseq = 10.0;
	y_log_baseq = 10.0;
	GPCSetBeepDefaults(GPC_BEEP_FREQUENCY, GPC_BEEP_DURATION);
	xlspcq = 60;
	ylspcq = 60;
	zlspcq = 60;
	tkhtq = 64;
	labspq = 80;
	borderq = 80;
}
/***********************************************************/
void TYPE GCLASS bgnplot(char mp, char sc, char *tekfile)
{
#if GPC_UNX
	char string1[6], string2[6];
#endif

#if GPC_OS2
	static char registered = 0;
#ifdef MULTITHREAD
	int instctr;

	if (!get_instanceq(-3)) {
		DosEnterCritSec();
		if (instanceq == 0)
#if TCQ
			atexit((void (_USERENTRY *)()) GPCFreeResourcesq);
#else
			atexit(GPCFreeResourcesq);
#endif
		GPCInstanceq(-1);
		instanceq++;
		DosExitCritSec();
	}
	instctr = get_instanceq(-1);
#else

	if (!registered)
#if TCQ
		atexit((void (_USERENTRY *)()) GPCFreeResourcesq);
#else
		atexit(GPCFreeResourcesq);
#endif
	registered = 1;
	instanceq++;
#endif
	/* Control the exit behavior. If sc == 'c' then GraphiC will not
	use the DosExit(EXIT_PROCESS, 0) API to kill the application. The
	System Menu will cause the Window thread to close and free up the
	Window resources but no other action will be taken. */
	if (sc == 'c' || sc == 'C')
		Set_Exitq(0);
	else
		Set_Exitq(1);
	GetProfileInfoq(GPC_PRINTER);
#endif

	initialq();
	if (sc == 't')
		mp = 0;
	getfilenameq(tekfile);
	init_videoq(0, mp);
	LANDSCAPE;
	g_tq.gz = &g_zq;			/* Assign zoom structure to GTEXT */
	g_prntq.gz = &g_zq;		/* And to printer structure */
	NewFile(tekfile);

#if GPC_UNX
#ifndef TKF_OUT
	if (!notekq) {

/* Open two pipes. tkf_pipe will be used to send tkf bytes from
tkbfillq() to reproq() and asc_pipe will be used to send ascq back
from play to endplot. */

		if(pipe(tkf_pipe)<0)
			messageq(FATALQ,100);
		sprintfq(string1,"%d",tkf_pipe[0]);
		if(pipe(asc_pipe)<0)
			messageq(FATALQ,101);
		sprintfq(string2,"%d",asc_pipe[1]);
		switch(fork()){
			case -1:
				messageq(FATALQ,102);
			case 0:
				close(tkf_pipe[1]);
				close(asc_pipe[0]);
				execlp("play","play", "user", string1, string2,
					tekfile, (char *)0);
				messageq(FATALQ,103);
			default:
				close(tkf_pipe[0]);
				close(asc_pipe[1]);
				break;
		}
	}
#endif
#endif
	fontinitq();
	page(9.0f, 6.884f);
	area2d(9.0f, 6.884f);

	/* If GPC_AUTOPREVIEW or GPC_AUTO_OPEN is set, launch companion HTML viewer immediately at plot start */
	if (getenv("GPC_AUTOPREVIEW") != NULL || getenv("GPC_AUTO_OPEN") != NULL) {
		svgopen(NULL);
	}
}
/*************************************************************/

/* User calls startplot() to initialize next plot. */
void TYPE GCLASS startplot(int bcolor)
{
	unsigned char action;
	G_PROCESS_ID

	if (win_numq == 0) {
		if (!notekq) {
			action = (unsigned char)SEEK_TEK_FILE;
			tkbfillq((char *)&action);
		}
		ccolorq('b', bcolor);		/* Background color before erase */
		strcpy(cmd_strq, "\033\014");
		gprosq(1);
		strcpy(cmd_strq, "\035"); /* erase graphics area: page command */
		gprosq(0);

		/* Put initial information at the start of each picture into the
		TKF file to ensure that on re-play the proper attributes are
		displayed. The attributes are set to the initial value at the
		beginning of each plot. */

		ccolorq('g', g_lq.color);
		ccolorq('p', g_pq.pattern);
		dashfq(g_lq.pattern);
		tlineq(g_lq.thick);
	}
	if (win_numq > 0)
		win_procq();		/* Scale this picture to previous window. */
#if GPC_WIN
	drawflgq = TRUE;        /* set drawing flag */
#endif
}
/*************************************************************/
/*User calls endplot() between graphs. It initializes variables for
new axes, clears the screen and puts the proper messages in TEK. It
does not close the file.*/
int TYPE GCLASS endplot(void)
{
	int asc;
	unsigned char action;
#if GPC_UNX
	char buff[1];
	int index = 0;
#endif
	G_PROCESS_ID

	gpc_flush_svg();
	wplane_flgq = 0;
	plot3q = 0;
	if (leg_flgq != LEG_OFF) {
		gpcfree((void **) &styleq);
		gpcfree((void **) &symclrq);
		leg_flgq = LEG_OFF;
	}
	right_flgq = 0;

	if (win_numq > 0)
		win_boxq();
	if (win_numq == 0) {
		if (sim_flgq > 0) {
			simuplot(0);
			messageq(ERRQ, 24);		/* Need to call simuplot(0) */
		}
		action = (unsigned char)END_TKF_PLOT;
		tkbfillq((char *)&action);
		if (!notekq)
			tkf_inc_saveq = ftell(tkf_pointerq);
#if GPC_WIN
		if (bitbflgq && !IsIconic(g_iq[VIDEO].hWnd)) {
			g_rdq.FromBitmap = 1;	/* Redraw using bitmap */
			f_tableq(&g_rdq);
		}

		drawflgq = FALSE;	/* done drawing for now	*/
		if (NeedRedraw == TRUE) {
			NeedRedraw = FALSE;
			PostMessage(g_iq[VIDEO].hWnd, WM_COMMAND, IDM_REDRAW, 0L);
		}
#endif

#if GPC_UNX
		/* For UNIX, the placing of the bell symbol in the TKF file tells
		play to send ascq back to endplot - when it is ready for the
		next picture or to end the sesson. */
		index=0;
		while(1){
			read(asc_pipe[0],buff,1);
			ascq=(int)buff[0];
			if(ascq=='q' || ascq==ENTER || index++==100)
				break;
		}
#else
		asc = ascq;
		if (!ascq)
			ascq = gtacharq();
		if (ascq != 'q' && ascq != 'Q' &&
			ascq != 'x' && ascq != 'X' &&
			ascq != 13) {
			if (!notekq) {
				if (asc > 13)		/* hardcopy() was called. */
					tekq(2);
				else if (asc != 13) /* hardcopy('n') was not called. */
					tekq(1);
			}
		}
#endif
#if GPC_WIN
		GPCWINEnableMenu(FALSE);
		if (ascq == 'q' || ascq == 'Q')	{
			ascq = asc;
			return(1);
		}
    	else	{
			ascq = asc;
			return(0);
		}
#else
		if (ascq == 'x' || ascq == 'X') {
			stopplot();
			return(1);
		}
		if (ascq == 'q' || ascq == 'Q') {
			stopplot();
#if GPC_OS2
			DosExit(EXIT_PROCESS, 0);
#else
			exit(0);
#endif
		}
		ascq = asc;
#endif
	}

	return(0);
}
/*************************************************************/

/*User calls stopplot() at end of plotting session.*/
void TYPE GCLASS stopplot(void)
{
	unsigned char action;
	G_PROCESS_ID

#if GPC_OS2
#ifdef MULTITHREAD
	if (!get_instanceq(-3))
#else
	if (!instanceq)
#endif
		return;
#endif
	if (!notekq) {
		action = (unsigned char)CLOSE_TEK_FILE;
		tkbfillq((char *)&action);
	}
	if (horizq.upper != (int *)NULL) {
		gpcfree((void **) &horizcq);
		gpcfree((void **) &(horizq.upper));
		gpcfree((void **) &(horizq.lower));
	}
	freememq();
	gpcfree((void **) &stringq);
	gpcfree((void **) &cmd_strq);
	gpcfree((void **) &tkfnameq);
	gpcfree((void **) &g_lq.xyp);

	if (g_prntq.print_number) {
		ascq = 'm';
		g_iq[PRINTER].type = FLUSH_PRINTER;
		tekq(2);
	}
	gterminateq();
	if (gchkmemq()) {
		gfreeallq();				/* Detected memory that was */
		messageq(ERRQ, 8);			/* allocated but not freed  */
	}
#if GPC_UNX
	 if (float_err_savq != SIG_ERR)
		signal(SIGFPE, float_err_savq);	/* restore Float handler */
	if (ctrl_c_savq != SIG_ERR)
		signal(SIGINT, ctrl_c_savq);	/* restore Ctrl-C handler */
#ifndef TKF_OUT
	close(tkf_pipe[1]);
	close(asc_pipe[0]);
#endif
#endif
	DESTROY_GPC_INSTANCE
}
/*************************************************************/
void TYPE GCLASS hardcopy(char type)
{
	G_PROCESS_ID

	if (type == 0 || type == '0')
		ascq = 0;
	else if (type == 'n' || type == 'N')
		ascq = 13;
	else
		ascq = type;
}

/****************************************************************
cltotkfq() takes a color value and places an ESC TG sequence with specified
index in the disk file. Color values are TEK default. The translations are
done with the arrays itotekq[] and tektoiq[] defined in graphic.cmn */
void GCLASS cltotkfq(void)
{
	G_PROCESS_ID

	strcpy(cmd_strq, "\33TG140000");
	gprosq(2);
	strcpy(cmd_strq, "\33TG1410F40");
	gprosq(2);
	strcpy(cmd_strq, "\33TG142G8C2F4");
	gprosq(2);
	strcpy(cmd_strq, "\33TG143O0C2F4");
	gprosq(2);
	strcpy(cmd_strq, "\33TG1440C2F4");
	gprosq(2);
	strcpy(cmd_strq, "\33TG145R<C2F4");
	gprosq(2);
	strcpy(cmd_strq, "\33TG146C<C2F4");
	gprosq(2);
	strcpy(cmd_strq, "\33TG147K4C2F4");
	gprosq(2);
	strcpy(cmd_strq, "\33TG148I6C2F4");
	gprosq(2);
	strcpy(cmd_strq, "\33TG149M2C2F4");
	gprosq(2);
	strcpy(cmd_strq, "\33TG14:P>C2F4");
	gprosq(2);
	strcpy(cmd_strq, "\33TG14;T:C2F4");
	gprosq(2);
	strcpy(cmd_strq, "\33TG14<A>C2F4");
	gprosq(2);
	strcpy(cmd_strq, "\33TG14=E:C2F4");
	gprosq(2);
	strcpy(cmd_strq, "\33TG14>0B10");
	gprosq(2);
	strcpy(cmd_strq, "\33TG14?0C<0");
	gprosq(2);
}

/* The following functions size the page and plotting area. They are set
up to generate pictures on a VIRTUAL pixel grid. The VIRTUAL size is
defined in GRAPHIC.H. The coordinate (0, 0) is at the bottom left corner
of the plot, even when rotated. Rotations are done in lineq. All user
input is in inches.
*************************************************************/
/* DETERMINE THE SIZE AND ROTATION OF THE PLOT. THE LONGER DIMENSION
IS ALWAYS PUT HORIZONTALLY ON THE SCREEN. */
void TYPE GCLASS page(float hinch, float vinch)
{
	G_PROCESS_ID

	xpageq = intopix(hinch);
	if (xpageq > acrossq) {
		xpageq = acrossq;
		messageq(WARNQ, 4);	/* X was chosen to long. Has been reduced. */
	}
	ypageq = intopix(vinch);
	if (ypageq > topq) {
		ypageq = topq;
		messageq(WARNQ, 5);	/* Y was chosen to long. Has been reduced. */
	}
			/* CENTER PLOT IN AREA */
	if (!win_numq) {
		xbaseq = (acrossq - xpageq) / 2;
		ybaseq = (topq - ypageq) / 2;
	}
	/* SET SCALING FACTORS TO THE PAGE SIZE */
	xfact_pgq = (float)xpageq / (float)acrossq;
	yfact_pgq = (float)ypageq / (float)topq;
	plot_initq();		/* Reset plot-specific parameters */
	scale_fontsq();
}
/**************************************************************/
/* DETERMINE THE SIZE AND LOCATION OF THE GRAPHICS SUBPLOT */
/* Does not include space for labels. */
void TYPE GCLASS area2d(float xinch, float yinch)
{
	G_PROCESS_ID

	xlenq = intopix(xinch);
	ylenq = intopix(yinch);
	right_flgq = 0;
}
/*********************************************************************/
/* plot_initq() initializes all parameters at the start of a plot */

void GCLASS plot_initq(void)
{
	G_PROCESS_ID

	titleq.string = NULL;
	xnameq.string = NULL;
	ynameq.string = NULL;
/* title and name heights are set to 0 so no space will be allowed for
them unless the user has asked for the space */
	titlhtq = (float)0.;			/* No spacing will be allowed */
	x_namhtq = (float)0.;			/* when calculating x and y */
	y_namhtq = (float)0.;			/* when calculating x and y */
	lablhtq = (float)(MAXRESX / 60);	/* .15 inches */

	add_inq.below = 0;
	add_inq.right = 0;
	add_inq.left = 0;
	xlspcq = (int)XSCALE_PAGE(60.f);
	ylspcq = (int)YSCALE_PAGE(60.f);
	zlspcq = (int)YSCALE_PAGE(60.f);
	tkhtq = (int)XSCALE_PAGE(64.f);
	labspq = (int)XSCALE_PAGE(80.f);
	borderq = (int)XSCALE_PAGE(80.f);
	if (crossq.flg & USER_WANTS_CROSSED_AXES)	/* user requested crossed axes */
		crossq.flg = 0x0f;
	crossq.x = crossq.y = 0.0f;	/* Reset the crossing points to 0.0 */
	plot_typeq = 0;
	psetflgq = 0;
	leg_flgq = LEG_OFF;
	x_angq = DEFAULT_INT;
	y_angq = DEFAULT_INT;
	z_angq = DEFAULT_INT;
	xphysq = (float)-1000.0;		/* User has not shifted the plot. */
	yphysq = (float)-1000.0;
	SetNameSpace(GPC_X_AXIS, 1.f);
	SetNameSpace(GPC_Y_AXIS, 1.f);
	SetTitleSpace(1.f);
}
/**************************************************************/
/* Search the win_structq arrays for windows that were called from gwindow.
When one is found, scale the proper parameters to place the next picture
in the window. Change the window type to 0 to prevent reuse. */
void GCLASS win_procq(void)
{
	int temp, temp1, temp2;
	int i, j, xl, xh, yl, yh;
	struct G_WIN *winmem;
	G_PROCESS_ID

	if (win_numq == 0)
		return;
	for (i = 0; i < win_numq; i++) {
		if (win_structq[i].type == GPC_IS_GWIN) {		/* It is a gwindow call. */
			win_structq[i].type = 0;
			xl = win_structq[i].xl;
			xh = win_structq[i].xh;
			yl = win_structq[i].yl;
			yh = win_structq[i].yh;
			temp = (short)(((long)xlenq * (xh - xl)) / xpageq);
			temp1 = (short)(((long)ylenq * temp) / xlenq);
			if (temp1 > (temp2 = (int)((long)ylenq * (yh - yl) / ypageq))) {
				xlenq = (int)(((long)xlenq * temp2) / ylenq);
				ylenq = temp2;
			}
			else {
				xlenq = temp;
				ylenq = temp1;
			}
			xpageq = xh - xl;
			ypageq = yh - yl;
			xbaseq = xl;
			ybaseq = yl;
			page(pixtoin(xpageq), pixtoin(ypageq));
			area2d(pixtoin(xpageq) * (float).85,
				pixtoin(ypageq) * (float).85);
			win_numq--;
			for (j = i; j < win_numq; j++) {
				G_MEMCPY((void *)&win_structq[j],
					(void *)&win_structq[j + 1], sizeof(struct G_WIN));
			}
			break;
		}
		i--;
	}
	if (win_numq > 0) {
		winmem = (struct G_WIN *)gpcrealloc(win_structq,
			win_numq * sizeof(struct G_WIN));
			if(winmem == NULL) {
				messageq(ERRQ, 1);				/* Out of Memory */
				gpcfree((void **) &win_structq);
				win_numq = 0;
			}
			else
				win_structq = winmem;
	}
	else {
		gpcfree((void **) &win_structq);
		gpcfree((void **) &line_start);
		gpcfree((void **) &line_end);
	}
}
/* The following two routines are provided to allow the user to match the
dimensions he chooses in EQUIP with that of the dimensions for graphic.
Since we are providing only one perameter for setting the size The aspect
ratio of the picture must be accounted for. The macro FULLPAGE can be
used as an argument to page() to set the page size equal to the plot
dimension. */
/**********************************************************************/
/* This routine sets the pixels / (inch or cm) based on the plot length
so that the user can match plot dimensions to the actual plot output.
The default is 9 inches giving 910 pixels per inch. */

void TYPE GCLASS PlotLength(float dimension)
{
	G_PROCESS_ID

	pix_convertq = (float)MAXRESX / dimension;
}
/*********************************************************************/
/* This routine allows the user to specify the plot length by establishing
the scaling factor based on pixels per inch. The default is 910 pixels per
inch or 179.13 pixels per cm, giving a plot length of 9 inches. */

void TYPE GCLASS PixPerUnit(float pixels)
{
	G_PROCESS_ID

	pix_convertq = pixels;
}
/****************************************************************
This routine allows the user to close a TKF file and open a new
one without having to go through stopplot() and bgnplot(). If a
file already exists it will be closed with all of the proper closeout
information. This function can also be used in a program which is
making many plots to allow the post processing features without the
file growing longer with each plot. If the same name is sent each
time the previous picture will be ofer written.
****************************************************************/
void TYPE GCLASS NewFile(char *tekfile)
{
	int i;
	unsigned char action;
	char *title = NULL;
	G_PROCESS_ID

    /* if there is a TKF file close it. */
	if (!notekq && tkf_pointerq != NULL) {
		action = (unsigned char)CLOSE_TEK_FILE;
		tkbfillq((char *)&action);
	}
    /* If a TKF file is requested... */
	if (tekfile != NULL) {
		/* In modernized GraphiC, default to NO TKF creation unless GPC_WRITE_TKF is set */
		if (getenv("GPC_WRITE_TKF") != NULL)
			notekq = 0;
		else
			notekq = 1;
		if (*tekfile == '\0') {
			title = gpcalloc(sizeof(char), 12);
			strcpy(title, "graphic.tkf");
		}
		else {
	        title = gpcalloc(1, strlen(tekfile) + 1);
			for (i = 0; tekfile[i] != '\0'; i++) {
				title[i] = (char)tolower(tekfile[i]);
			}
			if (!strncmp("notek", title, 5)) {
				notekq = 1;
			}
			else {
			    strcpy(title, tekfile);
			}
		}
	}
	else
		notekq = 1;
	if (!notekq) {
		sprintfq(cmd_strq, "%c%s", OPEN_TEK_FILE, title);
		tkbfillq(cmd_strq);
		if (sim_flgq > 0)
			save_pointerq();
		if (tkf_pointerq == NULL)
			notekq = 1;
		else {
			tkbfillq("\33%!0");		/* Not used by GraphiC, put in TKF file
							for portability: set terminal to TEK mode syntax */

			/* PUT COLOR INDEX IN OUTPUT FILE. */
			cltotkfq();
		}
	}
	if (title != NULL) {
	    gpcfree((void **)&title);
	}
#if GPC_WIN
	strcpy(OpenName, tekfile);	/* Put file name in window title LB */
	title = gpcalloc(strlen(tekfile) + strlen(szAppName) + 4,
	    sizeof(char));
	sprintfq(title, "%s - %s", szAppName, tekfile);
	GPCSetWindowTitle(g_iq[VIDEO].hWnd, (LPSTR)title);
	gpcfree((void **)&title);
#endif
}

/****************************************************************
svgcrop() sets the auto-crop margin in inches for SVG output.
margin >= 0: enable cropping with margin (default 0.25 inches).
margin < 0: disable cropping, returning to full letter canvas.
****************************************************************/
void TYPE GCLASS svgcrop(float margin_inches)
{
	gpc_set_svg_crop(margin_inches);
}

/****************************************************************
svgopen(filepath)
Opens an SVG plot in the user's default browser or image viewer.
If filepath is NULL or empty (""), the last rendered plot SVG
is automatically retrieved and opened.

Returns 0 on success, non-zero error code if unable to open.
****************************************************************/
int TYPE GCLASS svgopen(char *filepath)
{
	const char *target = filepath;
	char cmd[1024];

	/* If plot is NOT currently active, flush to ensure completed SVG is written to disk */
	if (!gpc_is_svg_active()) {
		gpc_flush_svg();
	}

	if (target == NULL || target[0] == '\0') {
		/* Prefer interactive HTML companion viewer with pan/zoom and live auto-update */
		const char *html = gpc_get_html_path();
		if (html != NULL && html[0] != '\0') {
			target = html;
		} else {
			target = gpc_get_svg_path();
		}
	}
	if (target == NULL || target[0] == '\0') {
		fprintf(stderr, "[GraphiC] svgopen: No plot file specified or generated.\n");
		return -1;
	}

#if defined(_WIN32) || (defined(GPC_WIN) && GPC_WIN)
	snprintf(cmd, sizeof(cmd), "start \"\" \"%s\"", target);
	return system(cmd);
#elif defined(__APPLE__)
	snprintf(cmd, sizeof(cmd), "open \"%s\"", target);
	return system(cmd);
#else
	snprintf(cmd, sizeof(cmd), "xdg-open \"%s\" >/dev/null 2>&1 &", target);
	return system(cmd);
#endif
}

/****************************************************************
svgpush(url, filepath)
Pushes an SVG plot over HTTP/HTTPS (POST) to a web endpoint,
dashboard, or live monitoring service.

If url is NULL or empty (""), the environment variable
GPC_SVG_PUSH_URL is checked.
If filepath is NULL or empty (""), the last rendered plot SVG
is automatically used.

Returns 0 on success, non-zero error code on failure.
****************************************************************/
int TYPE GCLASS svgpush(char *url, char *filepath)
{
	const char *target_url = url;
	const char *target_file = filepath;
	char cmd[2048];
	int ret;

	gpc_flush_svg();

	if (target_url == NULL || target_url[0] == '\0') {
		target_url = getenv("GPC_SVG_PUSH_URL");
	}
	if (target_url == NULL || target_url[0] == '\0') {
		fprintf(stderr, "[GraphiC] svgpush: No destination URL specified (or GPC_SVG_PUSH_URL not set).\n");
		return -1;
	}

	if (target_file == NULL || target_file[0] == '\0') {
		target_file = gpc_get_svg_path();
	}
	if (target_file == NULL || target_file[0] == '\0') {
		fprintf(stderr, "[GraphiC] svgpush: No plot file specified or generated.\n");
		return -2;
	}

	/* Use curl utility (standard on macOS, Linux, and Windows 10/11) */
	snprintf(cmd, sizeof(cmd),
		"curl -s -S -X POST -H \"Content-Type: image/svg+xml\" --data-binary \"@%s\" \"%s\" >/dev/null 2>&1",
		target_file, target_url);

	ret = system(cmd);
#if defined(_WIN32)
	int exit_code = ret;
#else
	int exit_code = (ret >= 0 && (ret & 0x7f) == 0) ? ((ret >> 8) & 0xff) : ret;
#endif
	if (exit_code == 0) {
		return 0;
	}

	fprintf(stderr, "[GraphiC] svgpush: curl returned status %d when pushing '%s' to '%s'\n", exit_code, target_file, target_url);
	return exit_code;
}
