/*	Copyright 1984-1993 by Scientific Endeavors Corporation.
	All rights reserved.

	This file contains tekq() which responds to user input and directs the
	program to take appropriate action.
Version 7.0 3/5/93
*******************************************************************/
#include <graphic.h>
#if GPC_OS2
#include <g_menus.h>
#endif
#include <tplt.h>

/* Allow 32K for program usage and pointer memory */
#define PTR_ALLOW 0x10000

#if !GPC_CPP
#if GPC_UNX
extern char userq;
#endif

void convertq(void);
void PSformatq(void);
void read_configq(char *name, int mode);
#endif
#if GPC_WIN
#include "gpcwin.h"             	/* Global variables for GraphiC/Win */
#include "gpcwdefs.h"				/* Menu defs and prototypes for Windows */
extern GPCWIN_DATA gpcwin_data;			/* Global data structure in GPCWIN.H */
/* External function declarations */
extern void TYPE GPCWINPrintTKF(HWND);			/* GraphiC windows driver */
#endif
/*************************************************************/
void GCLASS tekq(
	char asc)	/*  = 0 from PLAY, = 1 from GraphiC, >1 for hardcopy mode */
{
#if !(GPC_WIN)
	int i;
#endif
#if !(GPC_OS2 || GPC_WIN)
	char rotsav;
	static int pfnumber = 0;	/* Binary printer file extension */
	static char fdflag = 0;
	int rdfhq = -1;		/* FILE HANDLE FOR REDIRECTED PRINTER FILE */
	int abort = 0;
#endif
	int teksav;
	static int stflag = 0;
	char pchoice[80];		/* Used to hold printer choice config file name */
	char menu_flag = 0;
	static char fdnameq[81];
	G_PROCESS_ID

	if (asc == 3)
		NCOLORQ = 3;	/* assumes multi colors in batch mode printing */
	ascgq = asc;
	teksav = notekq;
	notekq = 1;
#if !(GPC_OS2 || GPC_WIN)
	rotsav = rotate_flgq;
	rotate(0);
#endif
#if GPC_WIN
	bintekq = TRUE;				/* Set flag to indicate in tekq */
#endif
	g_prntq.filedump = 0;
	abortq = 0;
	if (!stflag) {
		settekq();			/* Put default values into cvalq[index][H, L, S] */
		stflag = 1;			/* for TEK color codes. */
	}
	g_prntq.gz = &g_zq;
	g_zq.yfact = g_zq.xfact = 1.f;				/*  x1 MAGNIFICATION */
	freememq();				/* Free font memory */
	for (;; ) {
#if GPC_UNX
		from_file = TRUE;
#endif
		if (ascq == ' ') {		/* REPLAY PRESENT PLOT */
			g_zq.zoomit = 0;
			scrnpltq();
			menu_flag = 0;
		}
		if (ascgq == FROM_PLAY && ascq == 13) {			/* Play a new plot. */
			if (abortq == 'e')
				break;
#if GPC_UNX
			if (userq)
				from_file = FALSE;
#endif
			g_zq.zoomit = 0;
			tkf_offsetq += tkf_incrementq;
			scrnpltq();
			menu_flag = 0;
		}
		if (ascq == 's' || ascq == 'S') {
			g_scq.gmode = 't';
			f_tableq((char *)&g_scq);
			menu_flag = 0;
			if (g_iq[PRINTER].device_num == 32) {
				messageq(ERRQ, 30);	/* Color separation not supported */
									/* when outputting TKF commands.*/
			}
			else {
				g_prntq.separate = (char)(2);
				messageq(MESGQ,21);	/* Color sep. message */
				messageq(MESGQ,14);		/* Next option */
				ascq = gtacharq();
			}
		}
		if (ascq == 'c' || ascq == 'C') {				/* CHANGE FORMATS */
			convertq();
			has_beepedq = 1;
			menu_flag = 0;
			g_prntq.filedump = 0;
			ascq = gtacharq();
		}
		if (ascq == 'v' || ascq == 'V') {		/* CHANGE PostScript FORMAT */
			g_scq.gmode = 't';
			f_tableq(&g_scq);
			menu_flag = 0;
			PSformatq();
			g_prntq.filedump = 0;
			messageq(MESGQ,14);		/* Next option */
			ascq = gtacharq();
		}
		if (ascq == 'p' || ascq == 'P') {
			g_scq.gmode = 't';
			f_tableq(&g_scq);
			menu_flag = 0;
			messageq(MESGQ, 15);			/* Printer/Plotter files menu */
			do
				printer_choice = (gtacharq() - '0');
			while (printer_choice < 1 || printer_choice > 4);
			sprintfq(pchoice, "Using PRINTER%d.PRN", printer_choice);
			gprintq(pchoice, 1);
			messageq(MESGQ, 14);		/* Next option */
			ascq = gtacharq();
		}
		if (ascq == 'f' || ascq == 'F') {
			g_scq.gmode = 't';
			f_tableq(&g_scq);
			menu_flag = 0;
			messageq(MESGQ, 23);		/* Enter filename */
			GPC_GETS("", fdnameq, 81);
			if (fdnameq[0] != '\0') {
				g_prntq.filedump = 1;
			}
			else {
				g_prntq.filedump = 0;
			}
			messageq(MESGQ, 14);		/* Next option */
			ascq = gtacharq();
		}
		if (ascq == 'm' || ascq == 'M' || ascq == 'l' || ascq == 'L') {
#if GPC_OS2
			if (abortq == 'e')
				break;
			if (asc == 3)
				scrnpltq();
			i = print_dataq.prompt;
			print_dataq.prompt = 0;
			convert_flgq = IDM_FILE_PRINT;
			convertq();
			print_dataq.prompt = i;
			goto tekret;
#elif GPC_WIN
			GPCWINPrintTKF(g_iq[VIDEO].hWnd);	/* Routine to print TKF plot */
			goto tekret;
#else
			sprintfq(pchoice, "PRINTER%d.PRN", printer_choice);
			i = 0;
			switch(ascq) {
/* The values of i are:
		bit 0 -> on for full page
		bit 1 -> on to rotate left 90 degrees in driver
		bit 2 -> on to rotate right 90 degrees in driver
		bit 3 -> on to rotate 180 degrees in driver
		bit 4 -> on for high resolution dump
The printer space is defined so that the lower right is 0, 0 so that the printed
page looks like the screen rotated left 90 degrees assuming x is MAXRESXMO.
*/
				case 'm':		/* portrait mode */
					i = R_90;
					break;
				case 'M':
					i = R_90 | HI_RES;
					break;
				case 'l':		/* landscape mode */
					i = FULL_PAGE;
					break;
				case 'L':
					i = FULL_PAGE | HI_RES;
					break;
			}
			if (g_iq[PRINTER].type != FLUSH_PRINTER)
				g_iq[PRINTER].type = 0;
			g_iq[PRINTER].type |= i;
			g_iq[PRINTER].prtinfo = &g_prntq;
			read_configq(pchoice, PRINTER);

			if (g_iq[PRINTER].device_num < 0){
				messageq(MESGQ, 14);		/* Next option */
				ascq = gtacharq();
				continue;
			}

			g_scq.gmode = 't';
			f_tableq(&g_scq);
			menu_flag = 0;
			if (g_iq[PRINTER].device_num < 1
				|| g_iq[PRINTER].device_num > MAX_PRINTER) {
				messageq(MESGQ, 22);	/* No printer assigned */
				messageq(MESGQ, 14);		/* Next option */
				ascq = gtacharq();
				continue;
			}
			gdriverq(PRINTER);							/* sets ftableq */
			if (ftableq == NULL) {
				gpcBeep(600, 50);
				init_videoq(2, g_iq[VIDEO].type);
				messageq(ERRQ, 4);        /* Could not load printer driver */
				messageq(MESGQ, 14);		/* Next option */
				ascq = gtacharq();
				continue;
			}
			if (g_iq[PRINTER].type & FLUSH_PRINTER)
				g_prntq.filedump = fdflag;
			/* OPEN REDIRECT FILE IF NEEDED */
			if (g_prntq.filedump && g_prntq.separate <= 2) {
				if (g_prntq.print_number == 0) {
					g_fopenq.append = 0;
					pfnumber++;
				}
				else {
					g_fopenq.append = 1;
				}
				if (g_prntq.pcom == 6 &&
					!(g_iq[PRINTER].type & FLUSH_PRINTER)) {
					sprintfq(stringq, "%s.%d", tkfnameq, pfnumber);
					strcpy(fdnameq, stringq);
					printf("\nPrinter output to file %s\n", fdnameq);
				}
				g_fopenq.filename = fdnameq;
				g_fopenq.filehandle = rdfhq;
				rdfhq = f_tableq((char *)&g_fopenq);
				if (rdfhq == -1) {
					messageq(ERRQ, 2);		/* Error opening redirect file */
					g_prntq.filedump = 0;
				}
			}
			/* INITIALIZE THE PRINTER AND FILL THE DATA STRUCTURE g_prntq */
			abortq = 0;
			i = g_iq[PRINTER].auxmode;
			if ((i & 0x0200) == 0) {
				/* GET AVAILABLE MEMORY */
				g_prntq.memory_free = HowMuchMem() - PTR_ALLOW;
				if (g_prntq.memory_free <= 0) {	/* Not enough memory */
					abort = 'm';
					abortq = 'q';
				}
			}
			else
				g_prntq.memory_free = 0L;
			if (!abortq) {
				g_prntq.ncolors = NCOLORQ;
				abortq = (char)f_tableq((char *)&g_iq[PRINTER]);
			}
			if (abortq) {
				if (abortq != 'q' && !(g_iq[PRINTER].type & FLUSH_PRINTER)) {
					gpcBeep(600, 50);
					messageq(ERRQ, 5);	/* Could not initialize printer driver */
				}
			}
			else {
				if (!g_iq[PRINTER].vector && g_prntq.separate <= 1) {
					if (!printer_memq(g_prntq.nbuff, g_prntq.row,
						g_prntq.col)) {               /* Allocation failed */
							messageq(MESGQ, 14);		/* Next option */
							ascq = gtacharq();
							continue;
					}
				}
				if (g_prntq.separate <= 1)
					set_time(-1);
				printingq = 1;
				for (i = 0; i < g_prntq.npasses; i++) {
					tkf_incrementq = 0;
					if (!reproq())
						break;
					/* process printer pass */
					abort = (char)f_tableq((char *)&g_dumpq);
					if (abort == 'q') {
						abortq = abort;
						break;
					}
				}
				printingq = 0;
				f_tableq((char *)&g_quitq);	/* close the driver */
				if (g_prntq.separate > 0 && abortq != 'q'){
					if (g_prntq.separate < 0x30) {
						g_prntq.separate += 0x10;
						continue;
					}
				}
				g_fopenq.append = 0;
				g_prntq.separate = 0;
				fdflag = g_prntq.filedump;
				g_prntq.filedump = 0;
				gpcBeep(440, 50);
				if (!g_iq[PRINTER].vector)
					bfreeq(g_prntq.row, g_prntq.nbuff);
			}
			/* CLOSE REDIRECT FILE */
			if (rdfhq != -1) {
				if (f_tableq((char *)&g_fcloseq) == -1)
					messageq(WARNQ, 1);	/* Error closing redirect file */
				rdfhq = -1;
			}
			/* RELOAD THE VIDEO DRIVER */
			init_videoq(2, g_iq[VIDEO].type);
			if (abortq == 'q') {
				if (abort == 'm')
					messageq(MESGQ, 25);	/* Out of memory for dump */
				messageq(MESGQ, 53);		/* Print Aborted */
			}
			else if (!(g_iq[PRINTER].type & FLUSH_PRINTER))
				messageq(MESGQ, 16);		/* Print Made */
			g_iq[PRINTER].type = 0;
			if (ascgq > 1) {
				if (abortq == 'q') {
					messageq(MESGQ, 29);
					ascq = gtacharq();
				}
				goto tekret;
			}
			if (abortq != 'q') {
				gprintq(get_time(-1), 0);
				messageq(MESGQ, 17);		/* Print time */
			}
			messageq(MESGQ, 14);			/* Next option */
			ascq = gtacharq();
#endif
		}
		if (ascq == 'z' || ascq == 'Z' || ascq == 'w' || ascq == 'W') {
			do_zoomq();
			menu_flag = 0;
		}
		if (g_iq[dev_typeq].in_play) {	/* PLAY only */
			if (ascq == 'n' || ascq == 'N') {
				g_scq.gmode = 't';
				f_tableq(&g_scq);
				menu_flag = 0;
				ascq = new_fileq();
				if (ascq == 'q')
					ascq = 32;
				g_scq.gmode = 'g';
				f_tableq(&g_scq);
				continue;
			}
			if (('1' <= ascq) && (ascq <= '9') && !ascgq) {	/*skip n plots*/
				skipq(ascq - '0');
				ascq = 13;
			}
		}

		if (ascq == 'x' || ascq == 'X')
			goto tekret;

		if (ascq == 'q' || ascq == 'Q')
			goto tekret;
		if (ascq == 13) {
#if !(GPC_OS2 || GPC_WIN)
			rotate(rotsav);
#endif
			if (ascgq || abortq == 'e')
				goto tekret;
			abortq = 0;
		}
		if (asc_testq()) {
			if (!menu_flag){
#if 1 - GPC_GUI
				prnt_msgq();			/* List proper responses. */
#endif
				menu_flag = 1;
			}
			if (ascgq > 1)
				goto tekret;
			ascq = gtacharq();
		}
		else
			menu_flag = 0;
	}
tekret:
#if !GPC_UNX
	if (abortq != 'q' && ascq != 'q')
		font(-1);			/* Reload font memory */
#endif
	g_zq.zoomit = 0;
	ascgq = FROM_PLAY;
	notekq = teksav;
#if !(GPC_OS2 || GPC_WIN)
	rotate(rotsav);
#endif
}
/************************************************************************/
void GCLASS do_zoomq(void)
{
	G_PROCESS_ID

#if GPC_WIN
	ascq = gtacharq();
#else
	if (g_zq.zoomit & 1 || g_scq.gmode == 't') {
		g_zq.zoomit = 2;
		tkf_incrementq = 0;
		scrnpltq();
	}
	has_beepedq = 0;
	zcursonq();
	/* SAVE THESE FACTORS FOR REINITIALIZATION ON SECOND DUMP */
	g_zq.zoomit = 1;
	if (ascq == 'w' || ascq == 'W')
		g_zq.zoomit += 2;
	scrnpltq();
#endif
}
/************************************************************************/
/* This routine returns a '1' if ascq has been set to an invalid value. */
char GCLASS asc_testq(void)
{
	G_PROCESS_ID

	switch(ascq) {
		case '\r':
		case ' ':
		case 'c':
		case 'C':
		case 'f':
		case 'F':
		case 'l':
		case 'L':
		case 'm':
		case 'M':
		case 'p':
		case 'P':
		case 's':
		case 'S':
		case 'v':
		case 'V':
		case 'w':
		case 'W':
		case 'x':
		case 'X':
		case 'z':
		case 'Z':
			return(0);
	}
	if (ascgq == FROM_PLAY && ascq >= '0' && ascq <= '9') {
		if (g_iq[dev_typeq].in_play)
			return(0);
	}
	return(1);
}
/**************************************************************/
void GCLASS prnt_msgq(void)
{
	G_PROCESS_ID

	g_scq.gmode = 't';
	f_tableq(&g_scq);
	messageq(MESGQ, 1);			/* choices: c,f,l,L,m,M */
	if (ascgq == FROM_PLAY)
		messageq(MESGQ, 2);		/* n */
	messageq(MESGQ, 3);			/* p,q,s,v,w,z */
	if (ascgq == FROM_PLAY)
		messageq(MESGQ, 4);		/* 1-9 */
	messageq(MESGQ, 6);			/* CR,space */
}

/****************************************************************/
#ifdef SetDefaultPrinter
#undef SetDefaultPrinter
#endif
void TYPE GCLASS SetDefaultPrinter(int prnt_number)
{
	G_PROCESS_ID

	printer_choice = prnt_number;
}
/****************************************************************/
void TYPE GCLASS penchg(char mode)
{
	G_PROCESS_ID

	g_iq[PRINTER].penchg = mode;
}
