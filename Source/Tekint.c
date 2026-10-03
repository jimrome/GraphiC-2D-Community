/*	(c) 1984-1993 by Scientific Endeavors Corporation.
	All rights reserved.
Mainly the switch for full feature plotting. Includes the date
label facility.
Version 7.0 3/5/93
Modified for spawning from a user program 1/3/92
**************************************************************/
#include <graphic.h>
#include <tekint.h>
#include "g_font.h"

#define TEXTQ 3
#define DUMP_CH 5
#define INTEGERQ 6
#define INTCODE 4

#if GPC_WIN
#define PM_UPDATESTATUS       WM_USER
#define GPC_BACKGROUND_PROC if (!(is_background_proc())) break
extern void FTYPE GethAbortWnd(HWND *hwnd);
#else
#define GPC_BACKGROUND_PROC
#endif

#if !GPC_CPP
#if GPC_UNX
extern int tkf_pipe, asc_pipe;
extern char userq;
extern int redrawq;
int first_line;
#endif

#if GPC_WIN
#include <gpcwin.h>
extern GPCWIN_DATA gpcwin_data;
#endif
#endif

/************************************************************************
scrnpltq() uses reproq to print one graph in the screen buffer starting at
tkf_offsetq. At the end of the plot, tkf_incrementq is returned to allow
updating tkf_offsetq.*/
void GCLASS scrnpltq(void)
{
	G_PROCESS_ID
#if GPC_UNX
	char buf[1];
	redrawq = 0;
#endif

	g_prntq.filedump = 0;
#if GPC_WIN
	GPCWINEnableMenu(FALSE);
	drawflgq = TRUE;
	g_rdq.FromBitmap = 0;	/* Redraw without using bitmap */
	f_tableq(&g_rdq);
#else
	if (!reproq())
		return;
#endif
	if (ascgq == PLAY_HC || ascgq == GRAF_HC)	/* Hardcopy */
		return;
	if (g_zq.zoomit != 2 && !has_beepedq) {
		ascq = gtacharq();
#if GPC_UNX
		if (userq) {
			buf[0] = ascq;
			write(asc_pipe, buf, 1);
		}
	}
	if (userq) {
		if (ascq == 'z')
			from_file = TRUE;
		else
			from_file = FALSE;
#endif
	}
	has_beepedq = 1;
}
/***********************************************************************/
/* reproq() reconstructs the tek file and puts the results in the proper
 buffers. */
/* 12/20/85 modified to allow for insert of another picture into TKF file*/
int GCLASS reproq(void)
{
#if GPC_UNX
	Uchar buffer[1];
	int tally = 0;
#endif
	int ch, i = 0, retval = 0;
	int x, y;
	char asc;
	Uchar escmode = 0;
	int lpenup = 1;
	int tkmod;
	/* KEEP TRACK OF TEKTRONIX INTERPRETATION MODE */
	char tkfmd = 't';	/* 't' = text; 'g' = graphics */
						/* tkfmd needs to be initialized each entry into
						   reproq(). */
	G_PROCESS_ID

	g_lq.action = 1;
	if (g_lq.xyp != NULL)
		gpcfree((void **) &g_lq.xyp);
	g_lq.xyp = (struct GXYPAIRSQ *)gpcalloc(64, sizeof(struct GXYPAIRSQ));
	g_lq.npts = 1;
	replayq = 1;
#if GPC_UNX
	first_line = 1;		/* Used in loading seg_struct. */
#endif
	fseek(tkf_pointerq, tkf_offsetq, SEEK_SET);	/* USE READING FILE */
	tkf_incrementq = 0;
	tkmod = g_prntq.filedump;
	asc = ascgq;
	abortq = 0;
	gflagq = 1;
	for (;; ) {			/* Stay in loop until a bell or EOF. */

		GPC_BACKGROUND_PROC;

		if (newchq != 0) {	/* Value was picked up as previous terminator. */
			ch = newchq;
			newchq = i = 0;
		}
		else {
			ch = get_tkf_charq();
		}
		if (ch == EOF)
			abortq = 'e';
		if (abortq == 'e')
			break;

		if (tkmod && abortq == 'q') {
			replayq = 0;
			g_lq.action = 0;
			return(1);
		}
		/* GraphiC TOSSES ANY TEXT INFORMATION */
		if (tkfmd == 't' && !escmode) {
			if (ch == 10 || ch == 13 || ch == 32 || isalnum(ch))
				continue;
		}
		if (i < 0) {				/* Signals cmd_strq ready for decode. */
			if (g_lq.xyp == NULL)
				g_lq.xyp = (struct GXYPAIRSQ *)gpcalloc(64,
					sizeof(struct GXYPAIRSQ));
			tekcodeq(cmd_strq, &x, &y);
			dispatch_pointq(lpenup, x, y);
			lpenup = 0;
			i = 0;
		}
		if (ch == 31) {						/* US -> into text mode */
			tkfmd = 't';
			continue;
		}
		if (ch == 29) {
			tkfmd = 'g';
			lpenup = 1;
		}
		if (ch == 27) {
			if (!escmode) {					/* Go into ESC mode. */
				if (g_lq.npts > 2) {
					g_lq.xyp[0].x = g_lq.npts - 1;
					f_tableq(&g_lq);
					g_lq.npts = 1;
					gpcfree((void **) &g_lq.xyp);
				}
				escmode = 1;
				i = 0;
				cmd_strq[i++] = 27;
				continue;
			}
			newchq = 27;					/* Restore ESC of new sequence. */
		}
		if (escmode) {
			cmd_strq[i++] = (char)ch;
			if (i < 4 && ch >= 'A' && ch <= 'Z')
				continue;	/* Get ESC code. */
			if (i == 4) {
				newchq = ch;				/* Save the discarded character. */
				cmd_strq[3] = 0;			/* Finish with the string. */
				i--;
			}
			else
				cmd_strq[2] = 0;
			retval = gprosq(i - 1);	/* Process the code. */
			escmode = 0;			/* Get out of ESC mode. */
			i = 0;				/* reset string counter */
			continue;			/* get next character */
		}
		if (ch < 32) {
			cmd_strq[0] = (char)ch;
			cmd_strq[1] = 0;
			retval = gprosq(0);		/* Process a single character. */
			if (retval == 1) {				/* Signals end of plot (bell). */
				SET_LPLOTQ;	/* Store length of tkf plot (WIN) */
				for (;;) {
			 		ch = get_tkf_charq();
					if (ch != 0x16) {		/* Count padding characters. */
						tkf_incrementq--;
						break;
					}
#if GPC_UNX
					if (userq && tally++ == 11)
						break;
#endif
				}
				if (asc == 3)
					fseek(tkf_pointerq, tkf_offsetq + tkf_incrementq, SEEK_SET);
				if (ch == EOF)
#if GPC_UNX
				if (!userq)
#endif
					abortq = 'e';
				if (g_lq.npts > 2) {
					f_tableq(&g_lq);
					gpcfree((void **) &g_lq.xyp);
				}
				replayq = 0;
				g_lq.action = 0;
				return(1);
			}
			if (retval > 1)
				newchq = retval;		/* Save character returned. */
			continue;
		}
		if (ch == 31)
			continue;	/* Discard non-ASCII characters. */
		cmd_strq[i++] = (char)ch;
		if (ch >= 64 && ch < 96) {
			cmd_strq[i] = 0;
			i = ( -i);
		}
	}
	SET_LPLOTQ;	/* Store length of tkf plot (WIN) */
	replayq = 0;
	if (g_lq.npts > 2) {
#if GPC_WIN
		if (!bCancelq)
#endif
			f_tableq(&g_lq);
		gpcfree((void **) &g_lq.xyp);
	}
	g_lq.action = 0;
	if (abortq == 'e' && gflagq)
		return(0);
	return(1);
}
/*********************************************************************/
int get_tkf_charq(void)
{
	int ch;
#if GPC_UNX
	Uchar buffer[1];
#endif
	G_PROCESS_ID

#if GPC_UNX
	if (!from_file) {
		read(tkf_pipe, (char *)buffer, 1);
		ch = (int)buffer[0];
	}
	else
#endif
	ch = fgetc(tkf_pointerq);
	tkf_incrementq++;
	return(ch);
}
/*************************************************************
Decodes a single TEK encoded number. */
int GCLASS decodeq(char *str)
{
	int value = 0, i = 0;
	while (str[i] >= 64)
		value = (value << 6) + str[i++] - 64;
	if (str[i] >= 48)
		value = (value << 4) + str[i] - 48;
	else {
		value = (value << 4) + str[i] - 32;
		value = -value;
	}
	return(value);
}
/*************************************************************
gprosq(mode) is used by reproq to process the escape sequences in
a .tkf file. */
/* Tektronix sends a TG escape sequence for each color index.
Format is ESC TG 14 index-# hue lightness saturation. (The 1 is the surface
number (always 1) and the 4 is the number of values in the array (always 4).)
All numerical values are in code as follows:. The number is divided into 6 bit
groups with a least-significant 4 bit group. Each group is sent as an ASCII
character. Each 6 bit group has 64 added. If the number is positive, the 4 bit
group has 48 added, if negative it has 32 added. The individual color
assignments are:
ESC RA for the background, ESC MT for text and ESC ML for graphics. Each
sequence is followed by an index defined by a previous ESC TG. Indices are in
code. Our procedure is to convert IBM colors first to the values corresponding
to the TEK default and then to TEK hue - brightness - saturation values before
encoding for the disk file. Here, the disk file numbers are converted back to
index numbers which are retranslated to IBM values. */
int GCLASS gprosq(int mode)
{
	int i = 0, j = 0, k = 0;
	int x, y;
	int retval = 0;
	int index = 0, index1, ind = 3;
	static int GTINDEXQ = 7;
	unsigned char action;
	G_PROCESS_ID

	if (!notekq)
		tkbfillq(cmd_strq);
	switch(mode) {
		case 0:							/* A control character */
			switch(cmd_strq[0]) {
				case 7:										/* -BELL- */
					if (g_lq.npts > 2) {
						g_lq.xyp[0].x = g_lq.npts - 1;
						f_tableq(&g_lq);
						g_lq.npts = 1;
						gpcfree((void **) &g_lq.xyp);
					}
					if (!adding_flgq) {
						if(!notekq) {
							action = (unsigned char)MARK_TEK_FILE;
							tkbfillq((char *)&action);
						}
						if (!printingq)
							gpcBeep(beep_frequencyq, beep_durationq);
						gflagq = 1;
#if GPC_UNX
						gline(1);		/* Dump the seg structs. */
#endif
						retval = 1;		/* Signal a bell. */
					}
					break;
				case 29:									/* -GS- */
					if (g_lq.npts > 2) {
						g_lq.xyp[0].x = g_lq.npts - 1;
						f_tableq(&g_lq);
						g_lq.npts = 1;
						gpcfree((void **) &g_lq.xyp);
					}
					penup_flgq = 1;
					break;
			}
			break;
		case 1:
			index = cmd_strq[1];
			if (index == 12) {							/* -ESC FormFeed- */
				NCOLORQ = 0;
				if (gflagq) {
					g_scq.gmode = 'g';
					f_tableq(&g_scq);
					f_tableq((char *)&g_clsq);
					g_lq.pattern = 1;
				}
				gflagq = 0;
				has_beepedq = 0;
				break;
			}
			if (index >= '`' && index <= 'g') {			/* -ESC (lc char)- */
				g_lq.pattern = (index - '_');
				penup_flgq = 1;
											/* Smallest value of DASHQ is 1. */
				break;
			}
			else if (index >= 'h' && index <= 'o') {
				if (index == 'i')
					g_lq.pattern = 9;
				else
					g_lq.pattern = (index - 'g');
				penup_flgq = 1;
				break;
			}
			else if (index >= 'p' && index <= 'w') {
				g_lq.pattern = (index - 'o');
				penup_flgq = 1;
				break;
			}
			break;
		case 2:
			if (cmd_strq[1] == 'G') {
				if (cmd_strq[2] == 'T')
					g_lq.thick = (char)getseq(INTCODE, &ind);
													/* -GT-  Thicken curve */
				if (cmd_strq[2] == 'U')
					g_lq.thick = 0;				/* -GU-  Normal thickness */
				penup_flgq = 1;
				break;
			}
			if (cmd_strq[1] == 'I') {	/* -IV- Portrait or Landscape. */
				if (cmd_strq[2] == 'V') {
					/* set flag to 0 to get page size. */

					getseq(INTCODE, &ind);	/* Throw away port number. */
					getseq(INTCODE, &ind);	/* Throw away */
					getxy_pairq(&x, &y);
					getxy_pairq(&x, &y);
					if (y > x)
						rotate(1);
					else
						rotate(0);
				}
			}
			if (cmd_strq[1] == 'L') {
				switch(cmd_strq[2]) {
#ifdef PANEL
					case 'P':									/* -LP- */
						if (panfilq) { /* Proc. previous panel. */
							panelq();
							gpcfree((void **) &g_pq.xyp);
							g_pq.nsides = 0;
						}
						g_pq.xyp =
							(struct GXYPAIRSQ *)gpcalloc(64,
								sizeof(struct GXYPAIRSQ));
						if (g_pq.xyp == NULL) {
							messageq(WARNQ, 2);	/* Out of Memory */
							panfilq = 0;
						}
						else
							panfilq = 1;
						getxy_pairq(&x, &y);
						dispatch_pointq(1, x, y);
						ind = 8;
						g_pq.border = (char)(getseq(INTEGERQ, &ind) - '0');
						g_pq.b_color = g_lq.color;
						break;
					case 'E':									/* -LE- */
						if (g_pq.nsides >= 3 && panfilq != 0) {
							panelq();
							gpcfree((void **) &g_pq.xyp);
							panprocq();
						}
						panfilq = 0;
						g_pq.nsides = 0;	/* g_pq.nsides is set to 0 in
											pan_prepq() but needs to be set
											here also because file conversions
											do not use panelq() */
						break;
#endif
					case 'F':									/* -LF- */
						getxy_pairq(&x, &y);
						dispatch_pointq(1, x, y);
						if (newchq)
							retval = newchq;
						break;
					case 'G':									/* -LG- */
						getxy_pairq(&x, &y);
						dispatch_pointq(0, x, y);
						if (newchq)
							retval = newchq;
						if (!panfilq)
							ldrawq(g_lq.ptx, g_lq.pty);
						break;
					case 'T':									/* -LT- */
#ifndef DEMO
						gtextq |= 2;
						getseq(TEXTQ, &ind);
						g_tq.str = stringq;
						g_tq.px = x0q;		/* put in starting point */
						g_tq.py = y0q;
						/* IF HARDWARE CHARACTER SUPPORT, CALL DRIVER */
						if (g_iq[dev_typeq].hwchar)
							f_tableq((void *)&g_tq);
						else
						   tekfntq();
						gtextq &= 0x0d;
#endif
						break;
				}
				break;
			}
			if (cmd_strq[1] == 'M') {
				index = getseq(INTCODE, &ind);
				switch(cmd_strq[2]) {
					case 'C':									/* -MC- */
				/* THERE ARE THREE INTEGERQ ARGUMENTS; ONLY THE SECOND
						(THE HEIGHT) IS USED BY THE 4105. */
				/* FONTS ARE 5 PELS WIDE + 2 FOR A GAP, & 6 PELS HIGH. */
						g_tq.space = index;
					/* GET THE SECOND INTEGERQ. */
						/* height in pixels */
						g_tq.height = getseq(INTCODE, &ind);
						/* Get last integer. */
						g_tq.linesp = getseq(INTCODE, &ind);
						g_tq.linesp = (g_tq.height * 6) / 5;
						g_tq.space = (g_tq.height * 5) / 7;
						break;
					case 'F':                                   /* -MF- */
					    /* Select GrafText font (4107 command) */
                        /* Keep track of fonts used */
                        for (j = 0; j < 10; j++) {
							k = PSfntarrayq[j];
							if (k == -1) {	/* Blank spot, insert font */
								PSfntarrayq[j] = index;
								break;
							}
							else if (k == index) {	/* Already there */
								break;
							}
                        }
						/* Put into gtext packet */
						g_tq.font_number = index;
						break;
					case 'L':									/* -ML- */
						if(index <= 15)
							index = cl_valq[index];
						set_clrq('g', index);
						penup_flgq = 1;
						break;
					case 'P':									/* -MP- */
						if (NCOLORQ < 3)
							NCOLORQ++;	/* INCREMENT COLOR COUNTER */
						if (index <= 0) {
							g_pq.pattern = -cl_valq[-index];
						}
						else {
							g_pq.pattern = index;
						}
						break;
					case 'R':									/* -MR- */
						index1 = getseq(INTCODE, &ind);	/* On return, newchq
											!= 0 means no second integer. */
						if (!newchq)
							index = (int)((double)index *
								pow((double)2., (double)index1) + 44.);
						index = (int)((index / 90) * 90);
						if (index < 0)
							index += 360;
						if (index > 270)
							index = 270;
						g_tq.angle = index;
						break;
					case 'T':									/* -MT- */
						if (GTINDEXQ != index) {
							GTINDEXQ = index;
							g_tq.color = (index <= 15) ?
								(int)cl_valq[GTINDEXQ] : GTINDEXQ;
							if (NCOLORQ < 3)
								NCOLORQ++;	/* INCREMENT COLOR COUNTER */
						}
						break;
					case 'V':									/* -MV- */
						g_lq.pattern = (index + 1);
						penup_flgq = 1;
						break;
				}
				break;
			}
			if (cmd_strq[1] == 'R') {
				if (cmd_strq[2] == 'A') {							/* -RA- */
									/* Skip the first value (surface #). */
					getseq(INTCODE, &ind);
					index = getseq(INTCODE, &ind);
					/* Allow colors other than the 16 IBM ones */
					set_clrq('b', (int)(index < 16 ?
						(int)cl_valq[index] : index));
					gflagq = 1;		/* Set flag for new screen type; */
					break;
				}
			}
			if (cmd_strq[1] == 'T' && cmd_strq[2] == 'G') {			/* -TG- */
				i = 0;
				getseq(INTCODE, &ind);					/* Throw away surface
															number. */
				getseq(INTCODE, &ind);					/* Throw away # of
															values. */
				index = getseq(INTCODE, &ind);				/* Next value is
															the index. */
				clookq = 1;								/* Expect commaqs as
															default. */
				while (i < 3) {
					if (commaq) {				/* 'commaq' means a default ',
												was sent. */
						commaq = 0;
						i++;
						continue;
					}
					cvalq[index][i++] = getseq(INTCODE, &ind);
				}
				clookq = 0;
				getclrq(index);			/* Puts IBM color value in cl_valq. */
				break;
			}
			getseq(DUMP_CH, &ind); /* Not a recognized code. Dump remainder. */
			break;
	}
	return(retval);
}
/*********************************************************************/
/*
tekcodeq() decodes a tkf code set and returns the results to the
calling routine The extrabyte has been extended to obtain 8192
resolution as follows:
bit  8   7   6   5   4   3   2   1
     x0  1   1   y0  y2  y1  x2  x1
*/

#define MASKX	0x0003
#define MASKY	0x000c
#define MASKX0	0x0080
#define MASKY0	0x0010

void GCLASS tekcodeq(char *tptr, int *x, int *y)
{
	int i = 0;
	static unsigned int hx, hy, ly, extra, extrab = 0;
	unsigned int ch;

	for (;; ) {
		ch = (unsigned int)tptr[i++];
		if (ch >= 64 && ch < 96) {		/* We're through.  It is lo-x */
			extra = 0;					/* The most likely is put first */
			*x = (((hx << 5) + (ch & 0x1f)) << 3) + ((extrab & MASKX) << 1)
				+ ((extrab & MASKX0) >> 7);
									   	/* for speed. */
			*y = (((hy << 5) + (ly & 0x1f)) << 3) + ((extrab & MASKY) >> 1)
				+ ((extrab & MASKY0) >> 4) ;
			break;
		}
		else if (ch > 95) {				/* Low y or extra */
			extra++;					/* If two of these, first is extra. */
			if (extra == 2)
				extrab = ly;			/* ly is really extra byte. */
			/* Don't AND ly because it could be an extra byte */
			ly = (Uint)(ch);			/* Update ly if extra sent. */
		}
		else if (extra) {					/* If last char was ly or extra */
			hx = (Uint)(ch & 0x1f);
			extra = 0;
		}
		else							/* ch is between 32 and 63 */
			hy = (Uint)(ch & 0x1f);
		continue;
	}
}
#undef MASKX
#undef MASKY
#undef MASKX0
#undef MASKY0
/********************************************************************
dispatch_pointq() takes an x,y pair and either loads the panel array or
sends the point to the appropriate line routine.
********************************************************************/
void dispatch_pointq(int penup, int x, int y)
{
	G_PROCESS_ID

	if (g_zq.zoomit & 1 && !(gtextq & 2))	/* Need magnified values */
		magniq(x, y, &x, &y);
	if (panfilq) {
		g_pq.xyp[g_pq.nsides].x = x;
		g_pq.xyp[g_pq.nsides].y = y;
	/* CHECK FOR END OF BUFFER, AND REALLOCATE IF NECESSARY */
		g_pq.nsides = newbufsq(&g_pq.xyp, g_pq.nsides);
	}
	else {
		if (penup)
			lmoveq(x, y);
		else
			ldrawq(x, y);
	}
}
/*************************************************************
getclrq() translates TEK color values into EGA values. */
void GCLASS getclrq(int index)
{
	int temp1, temp2, temp3;
	G_PROCESS_ID

	temp1 = cvalq[index][0];			/* Hue */
	temp2 = cvalq[index][1];			/* Lightness */
	temp3 = cvalq[index][2];			/* Saturation */
	if (temp3 < 10) {					/* Low saturation */
		if (temp2 < 10) *(cl_valq + index) = 0;
		else if (temp2 >= 10 && temp2 < 35) *(cl_valq + index) = 8;
		else if (temp2 >= 35 && temp2 < 80) *(cl_valq + index) = 7;
		else if (temp2 > 80) *(cl_valq + index) = 15;
		return;
	}
	if (temp1 >= 345 || temp1 < 15) *(cl_valq + index) = 1;
	else if (temp1 >= 15 && temp1 < 45) *(cl_valq + index) = 13;
	else if (temp1 >= 45 && temp1 < 75) *(cl_valq + index) = 5;
	else if (temp1 >= 75 && temp1 < 105) *(cl_valq + index) = 12;
	else if (temp1 >= 105 && temp1 < 135) *(cl_valq + index) = 4;
	else if (temp1 >= 135 && temp1 < 165) *(cl_valq + index) = 6;
	else if (temp1 >= 165 && temp1 < 195) *(cl_valq + index) = 14;
	else if (temp1 >= 195 && temp1 < 225) *(cl_valq + index) = 10;
	else if (temp1 >= 225 && temp1 < 255) *(cl_valq + index) = 2;
	else if (temp1 >= 255 && temp1 < 285) *(cl_valq + index) = 11;
	else if (temp1 >= 285 && temp1 < 315) *(cl_valq + index) = 3;
	else if (temp1 >= 315 && temp1 < 345) *(cl_valq + index) = 9;
}
/**************************************************************
Sets Tektronix color values. See Set Surface Color Map in Tektronix
4106 manual for the TEK default values. */
void GCLASS settekq(void)
{
	int i;
	G_PROCESS_ID

	for (i = 2; i < 14; i++)
		cvalq[i][1] = 50;
	for (i = 2; i < 14; i++)
		cvalq[i][2] = 100;
	for (i = 0; i < 3; i++)
		cvalq[0][i] = 0;
	cvalq[1][0] = 0;
	cvalq[4][0] = 0;
	cvalq[14][0] = 0;
	cvalq[15][0] = 0;
	cvalq[2][0] = 120;
	cvalq[3][0] = 240;
	cvalq[5][0] = 300;
	cvalq[6][0] = 60;
	cvalq[7][0] = 180;
	cvalq[8][0] = 150;
	cvalq[9][0] = 210;
	cvalq[10][0] = 270;
	cvalq[11][0] = 330;
	cvalq[12][0] = 30;
	cvalq[13][0] = 90;
	cvalq[1][1] = 100;
	cvalq[14][1] = 33;
	cvalq[15][1] = 66;
	cvalq[1][2] = 0;
	cvalq[14][2] = 0;
	cvalq[15][2] = 0;
}
/********************************************************************
getxy_pairq() gets an x,y coordinate from the TEK information and returns
them to the caller.
********************************************************************/
void getxy_pairq(int *x, int *y)
{
	
	int ch;
	int i = 0;
	char str[6];
	G_PROCESS_ID

	for (;; ) {			/* Stay in the loop until terminator. */
		if (newchq) {			/* Next character was passed from reproq. */
			ch = newchq;
			newchq = 0;
		}
		else {
			ch = get_tkf_charq();
			if (ch == EOF || i >= 6) {
				messageq(ERRQ, 3000);
				return;
			}
		}
		str[i++] = (char)ch;
		if (ch >= 64 && ch < 96) {		/* Finished code sequence. */
			str[i] = 0;
			tekcodeq(str, x, y);			/* Decode and put in array. */
			break;;
		}
	}
}
/**************************************************************/
/* getseq() returns an integer if not in TEXT or XY_PAIR modes unless a ', '
is encountered in which case it sets the global variable 'commaq' to
1. In XY_PAIR it decodes the tek code sequence and calls tekcodeq() which
puts the results in the array  g_pq.xyp[].(x, y).
(These same vectors are used for panel sides.) In TEXT mode getseq() loads
a text string into stringq[]. */
int GCLASS getseq(Uchar mode, int *ind)
{
	char finish = 0, start = 1;
	int i = 0, ch;
	int value = 0;
	G_PROCESS_ID
#if GPC_UNX
	Uchar buf[1];
#endif

	for (;; ) {			/* Stay in the loop until terminator. */
		if (finish)
			break;
		if (newchq) {			/* Next character was passed from reproq. */
			ch = newchq;
			newchq = 0;
		}
		else {
			if (replayq) {
				ch = get_tkf_charq();
			}
			else
				ch = cmd_strq[(*ind)++];
			if (ch == EOF || ch < 32) {
				if (replayq)
					newchq = ch;		/* Termination of sequence */
				finish = 1;
			}
		}
		if (i == 0 && newchq != 0)
			return(0);		/* No string to process. */
		if (newchq == 0)
			stringq[i++] = (Uchar)ch;		/* Not yet terminated */
		switch(mode) {
			case INTEGERQ:
				if (ch < '0' || ch > '9') {		/* No integer is present. */
					newchq = ch;				/* Save the character. */
					return(0);
				}
				return(ch);
			case INTCODE:								/* INTCODE mode */
				commaq = 0;		/* 'commaq' means a default ', ' was sent. */
				if (ch == ',' && clookq) {
										/* clookq means a default expected. */
					commaq = 1;
					return(0);
				}
				if (ch >= 64) continue;
				stringq[i] = 0;
				return(decodeq(stringq));
			case TEXTQ:								/* TEXT mode */
				if (start && ch > '@')
					continue;	/* First a coded length */
				else if (start) {
					stringq[i] = 0;
					value = decodeq(stringq);
					start = 0;
					i = 0;
					continue;
				}
				if (ch < 32 || ch > 127) {
										/* throw away non ASCII characters */
					i--;
					value--;
					continue;
				}
				if (i < value) {
					if (i == 80) {					/* Limit of 80 chars. */
						stringq[i] = 0;
						return(80);
					}
					continue;
				}
				else {
					stringq[i] = 0;
					return(value);
				}
			case DUMP_CH:			/* Dump unrecognized code. */
				if (newchq == 0)
					continue;
				return(0);
		}
	}
	return(0);
}
/***********************************************************/
/* Skip n pictures in a TKF file */
void GCLASS skipq(int n)
{
	int ch, i;
	long ofposq;
	Uchar array[4];

	G_PROCESS_ID

	if (tofileq != NULL) {	/* USE OFFSET FILE */
		/* SKIP TO PLOT #n POSITION */
		if (!fseek(tofileq, 4L * ((long)n - 1), SEEK_SET)) {
			fread(array, sizeof(char), 4, tofileq);
			ofposq = longmakeq(array);
			fseek(tkf_pointerq, ofposq, SEEK_SET);
			tkf_offsetq = ofposq;
			tkf_incrementq = 0;
			return;
		}
	}
	for (i = 0; i < n; i++) {
		for (;; ) {
			ch = fgetc(tkf_pointerq);
			tkf_incrementq++;
			if (ch == -1) {
				messageq(ERRQ, 11);		/* Can't skip. At end of file. */
				return;
			}
			if (ch == '\007') {
				gpcBeep(1000, 30);
				break;	/* BELL IS END OF PLOT */
			}
		}
	}
	while ((ch = fgetc(tkf_pointerq)) == '')	/* skip padding */
		tkf_incrementq++;
	if (ch == -1) {
		messageq(ERRQ, 11);		/* Can't skip. At end of file. */
		return;
	}
	tkf_offsetq += tkf_incrementq;
	tkf_incrementq = 0;
}
/**************************************************************
magniq(x, y, xmg_ptr, ymg_ptr) stretches the x and y coordinates so that a
magnified plot can be made.*/
void GCLASS magniq(int x, int y, int *xmg_ptr, int *ymg_ptr)
{
	long xl, yl;
	G_PROCESS_ID

	xl = x;
	yl = y;
	if (g_zq.zoomit == 3) {
//		if (insertq == 2) {
//			xl = (long)((MAXRESYMO - y) * g_zq.xfact) + g_zq.xshift;
//			yl = (long)(x * g_zq.yfact) + g_zq.yshift;
//		}
//		else {
			xl = (long)(x * g_zq.xfact) + g_zq.xshift;
			yl = (long)(y * g_zq.yfact) + g_zq.yshift;
//		}
	}
	else {
		xl = (long)((x - g_zq.xshift) * g_zq.xfact); /* (x-offset)*magnification */
		yl = (long)((y - g_zq.yshift) * g_zq.yfact); /* (y-offset)*magnification */
	}
	if (xl > 32766) xl = 32766;
	if (yl > 32766) yl = 32766;
	if (xl < - 32767) xl = ( -32767);
	if (yl < - 32767) yl = ( -32767);
	*xmg_ptr = (int)xl;
	*ymg_ptr = (int)yl;
}
/********************************************************************/
void GCLASS set_clrq(char mode, int ncolor)
{
	G_PROCESS_ID

	switch(mode) {
		case 'g':
			g_lq.color = ncolor;
			if (NCOLORQ < 3)
				NCOLORQ++;	/* INCREMENT COLOR COUNTER */
			break;
		case 't':
			g_tq.color = ncolor;
			break;
		case 'b':
			g_clsq.color = ncolor;
			break;
	}
}
#ifndef DEMO
/*********************************************************************/
/* These routines do graftext for non-hardware character devices     */
/* PLOT FONT STRING AT POSITION x,y. height IS THE HEIGHT MULTIPLIER, */
/* ang IS THE ANGLE AT WHICH THE LETTERS ARE PLOTTED. 0, +/-90 DEGREES*/
/* GrafText is always upright (unrotated) */
void GCLASS tekfntq(void)
{
	int x , y;
	int hgt, space;
	char *string;
	int n, i, j;
	unsigned char ch;
	char *p;
	int noteksav;
	int dshsav;
	int colsav;
	int newsegt;
	G_PROCESS_ID

	noteksav = notekq;
	dshsav = g_lq.pattern;
	colsav = g_lq.color;

	notekq = 1;
	g_lq.color = g_tq.color;
	x = g_tq.px;
	y = g_tq.py;
	string = (char *)g_tq.str;
	hgt = g_tq.height;	/* In pixel space */
	if (hgt < 20) {
		numtq = hgt;
		dentxtq = 7;
	}
	else {
		numtq = hgt / 7;
		dentxtq = 1;
	}
	g_lq.pattern = 1;
	i = 0;
	xtextq = x;
	ytextq = y;
	newsegt = 1;
	while ((ch = string[i++]) != 0) {
		if ((ch < ' ') || (ch > 127)) continue;
		n = ch - 32;
		if ((n >= 0) && (n <= 94)) {  /*good char location*/
				/* SET WIDTH AND DESCENDERS */
			ydtxtq = tek[n].descend;
			p = tek[n].tab;
		}
		else
			continue;
		for (j = 0; j <= 50; j++) {
			x0txtq = x1txtq;
			y00txtq = y11txtq;
			if (*p == 127) {					/* END OF LETTER */
				newsegt = 1;
				break;
			}
			else if (*p == '*') {				/* PENUP */
				p++;
				x0txtq = *(p++);
				y00txtq = *(p++);
				newsegt = 1;
			}
			x1txtq = *(p++);
			y11txtq = *(p++);
			if (j != 0) {
				rotatewq(newsegt);
				newsegt = 0;
			}
		}
		/* UPDATE X,Y */
		/* MOVE GRAPHIC CURSOR */
		space = g_tq.space;
		switch(g_tq.angle) {	/* Check for needed magnification */
			case 0:
			case 360:
				xtextq += (g_zq.zoomit & 1) ?
					(int)(space * g_zq.xfact) : space;
				break;
			case - 270:
			case 90:
				ytextq += (g_zq.zoomit & 1) ?
					(int)(space * g_zq.yfact) : space;
				break;
			case -180:
			case 180:
				xtextq -= (g_zq.zoomit & 1) ?
					(int)(space * g_zq.xfact) : space;
				break;
			case -90:
			case 270:
				ytextq -= (g_zq.zoomit & 1) ?
					(int)(space * g_zq.yfact) : space;
				break;
		}
	}
	if (g_lq.npts > 2) {
		g_lq.xyp[0].x = g_lq.npts - 1;
		f_tableq(&g_lq);
		g_lq.npts = 1;
	}
	g_lq.pattern = dshsav;
	g_lq.color = colsav;
	notekq = noteksav;
}
/**************************************************************/
/* ROTATE THE CHARACTER AND PLOT THE GrafText SEGMENT*/
/*
NOTE: The start of the text string has been magnified
(if necessary) when the position was decoded. Thus, only magnify
the character strokes. Don't shift them.
*/

void GCLASS rotatewq(int newseg)
{
	static int xa, ya, xb, yb;
	static int xfm, yfm, xto, yto;
	int x0c, y0c, xtoc, ytoc;
	int temp;
	int den2;
	G_PROCESS_ID

	den2 = dentxtq >> 1;
	if (newseg) {
		xa = (numtq * x0txtq + den2) / dentxtq;
		ya = (numtq * (y00txtq - ydtxtq) + den2) / dentxtq;
		if (g_zq.zoomit & 1)	{		/* Need magnified values */
			xa = (int)(xa * g_zq.xfact);	 /* x*magnification */
			ya = (int)(ya * g_zq.yfact);	 /* y*magnification */
		}
	}
	else {
		xa = xb;
		ya = yb;
	}
	xb = (numtq * x1txtq + den2) / dentxtq;
	yb = (numtq * (y11txtq - ydtxtq) + den2) / dentxtq;
	if (g_zq.zoomit & 1)	{		/* Need magnified values */
		xb = (int)(xb * g_zq.xfact);	 /* x*magnification */
		yb = (int)(yb * g_zq.yfact);	 /* y*magnification */
	}
	switch(g_tq.angle) {
		case 0:
		case 360:
			xfm = xtextq + xa;
			yfm = ytextq + ya;
			xto = xtextq + xb;
			yto = ytextq + yb;
			break;
		case - 270:
		case 90:
			xfm = xtextq - ya;
			yfm = ytextq + xa;
			xto = xtextq - yb;
			yto = ytextq + xb;
			break;
		case -180:
		case 180:
			xfm = xtextq - xa;
			yfm = ytextq - ya;
			xto = xtextq - xb;
			yto = ytextq - yb;
			break;
		case -90:
		case 270:
			xfm = xtextq + ya;
			yfm = ytextq - xa;
			xto = xtextq + yb;
			yto = ytextq - xb;
			break;
	}

	if (newseg) {
		lmoveq(xfm, yfm);
	}
	if ((xfm > xh_cropq) || (xfm < xl_cropq) || (xto > xh_cropq)
		 || (xto < xl_cropq) || (yfm > yh_cropq) || (yfm < yl_cropq)
		 || (yto > yh_cropq) || (yto < yl_cropq)) {

		temp = cropq(xfm, yfm, xto, yto, &x0c, &y0c, &xtoc, &ytoc);

		if (temp == 1 || temp == 3 || newseg)
			lmoveq(x0c, y0c);
		if (temp < 4)
			ldrawq(xtoc, ytoc);
	}
	else {
		ldrawq(xto, yto);
	}
}
#endif
/************************************************************/
/* Take a long and return a 4-byte array that contains
the bytes from lowest signifigant to highest */
long GCLASS longmakeq(Uchar array[4])
{
	int i;
	long value = 0L;

	for(i = 0; i < 4; i++) {
		value = value << 8;
		value |= (long)array[3 - i];
	}
	return(value);
}
/***************************************************************************
 * is_background_proc()                                                    *
 *                                                                         *
 * This GraphiC/Win specific function periodically allows Windows          *
 * to have control of the cpu for other applications. It also              *
 * refreshes the window with a BitBlt if needed and updates the            *
 * gas gauge for conversions.                                              *
 ***************************************************************************/
#if GPC_WIN
int GCLASS is_background_proc(void)
{
	static int backcnt,backcnt2;
	WORD nPercentCq;
    float fPercentCq;
    HWND hwndtmp;

	if (tkf_incrementq == 0) {
		backcnt = 0;
		backcnt2 = 0;
	}

	if (bCancelq)	{
		gpcBeep(1000, 30);	/* Bell sound at plot end */
		return(0);	/* Return 0 to exit for loop in reproq */
	}

	/* Background processing for Windows	*/
	if (backcnt >= 10) {
		backcnt = 0;
		GPCWINYieldControl();  /* Give Windows control */
	}
	backcnt++;

	if ((backcnt2 >= 5*Wupdatfreq)&&((convflgq)||(prntflgq))) { /* Update gasgauge */
		backcnt2 = 0;
		if (lplotq > 0) {
			fPercentCq = (float)((100.0*tkf_incrementq)/lplotq);
			nPercentCq = (WORD)fPercentCq;
		}
		else
			nPercentCq = 0;
		if (convflgq) { /* converting */
			hwndtmp = hConvDlg;
		}
		else {	/* printing */
			if (hAbortDlgWnd == (HWND)NULL)
				GethAbortWnd(&hAbortDlgWnd);
			hwndtmp = hAbortDlgWnd;
		}
		SendMessage(hwndtmp, PM_UPDATESTATUS, nPercentCq, 0L);
	}
	backcnt2++;
	return(1);	/* Return 1 to remain in for loop in reproq */
}
#endif
