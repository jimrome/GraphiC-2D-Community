/* (c) 1984-1993 by Scientific Endeavors Corporation.
   All rights reserved.
Version 7.0 3/3/93
*************************************************************/
#include <graphic.h>
#include <insert.h>

#define TEXTQ 3
#define DUMP_CH 5
#define INTEGERQ 6
#define INTCODE 4

#if !GPC_CPP
extern void tekcodeq(char *tptr, int *x, int *y);
void ins_getxy_pairq(int *x, int *y);
void dispatch_pointq(int penup, int x, int y);
extern int decodeq(char *str);
extern void tekfntq(void);
#endif

float mag;		/* the ratio of insert height to picture height */
int center;
/***********************************************************/
/* Insert a plot from one TKF file into another plot and another
TKF file. EXECUTIVE ROUTINE */

void TYPE GCLASS insert(char *readfile, int picnum, float llx, float lly,
	float ysize, int irot)
{
	int i, ch;
	char buff[80];
	G_PROCESS_ID

	dashf(1);
	center = (int)llx;
	insertq = (char)(irot + 1);
	infileq = gfopen(readfile, "rb", 1);	/* OPEN READ FILE */
	if (infileq == NULL) {
		sprintfq(buff, "Filename: %s", readfile);
		gprintq(buff, 0);
		messageq(ERRQ, 2);		/* Error opening TKF file */
		return;
	}
	if (picnum == 0) {
		messageq(ERRQ, 66);
		return;
	}
	for (i = 1; i < picnum; i++) {
		for (;; ) {
			ch = fgetc(infileq);
			if (ch == -1) {
				messageq(ERRQ, 53);	/* EOF before desired picture */
				return;
			}
			if (ch == '\007') {
				gpcBeep(1000, 30);
				break;	/* BELL IS END OF PLOT */
			}
		}
	}
	g_pq.border = 1;
	g_zq.yshift = intopix(lly);
	mag = ysize / pixtoin(topq);
/* The following code segment is to support old style TKF files which do
not contain the rotate command. If the rotate command is found in the file
the factors are re-set. */

	if ((rotate_flgq & 2) && !(irot & 1))
		g_zq.xfact = mag * ((float)MAXRESXMO / (float)MAXRESYMO);
	else if(!(rotate_flgq & 2) && (irot & 1))
		g_zq.xfact = mag * ((float)MAXRESYMO / (float)MAXRESXMO);
	else
		g_zq.xfact = mag;
	g_zq.yfact = g_zq.xfact;
	if (llx == (float) -1.) {
		if (!irot)
			g_zq.xshift = (int)((acrossq - MAXRESXMO * g_zq.xfact) * .5);
		else
			g_zq.xshift = (int)((acrossq - MAXRESYMO * g_zq.xfact) * .5);
	}
	else
		g_zq.xshift = intopix(llx);
/* End of compatibility code. */

	g_zq.zoomit = 3;
	ascgq = 0;
	file_processq();
	fseek(infileq, 0L, SEEK_END);
	gfclose(infileq);	/* CLOSE INSERT FILE */
	insertq = 0;
	g_zq.zoomit = 0;
	g_zq.xshift = g_zq.yshift = 0;
}
/***********************************************************************/
/* file_precessq() reconstructs the tek file to be inserted */
void GCLASS file_processq(void)
{
	int ch, i = 0, retval = 0;
	int x, y;
	Uchar escmode = 0;
	char lpenup = 1;
	/* KEEP TRACK OF TEKTRONIX INTERPRETATION MODE */
	static char tkfmd = 't';	/* 't' = text; 'g' = graphics */
	G_PROCESS_ID

	for (;; ) {			/* Stay in loop until a bell or EOF. */
		if (newchq != 0) {	/* Value was picked up as previous terminator. */
			ch = newchq;
			newchq = i = 0;
			if (ch == EOF)
				break;
		}
		else {
			if ((ch = fgetc(infileq)) == EOF)
				break;
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
			retval = tek_processq(i - 1);	/* Process the code. */
			escmode = 0;			/* Get out of ESC mode. */
			i = 0;				/* reset string counter */
			continue;			/* get next character */
		}
		if (ch < 32) {
			cmd_strq[0] = (char)ch;
			cmd_strq[1] = 0;
			retval = tek_processq(0);		/* Process a single character. */
			if (retval == 1) {				/* Signals end of plot (bell). */
				return;
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
}
/*************************************************************
tek_processq(mode) is used by file_precessq to process the escape sequences in
a .tkf file. ESC commands that are not needed for inserting a picture
are ignored. */
int GCLASS tek_processq(int mode)
{
	int retval = 0;
	int index = 0, index1;
	int x, y;
	static int GTINDEXQ = 7;
	G_PROCESS_ID

	switch(mode) {
		case 0:							/* A control character */
			switch(cmd_strq[0]) {
				case 7:										/* -BELL- */
					retval = 1;		/* Signal a bell. */
					break;
				case 29:							/* -GS- */
					break;
			}
			break;
		case 1:
			index = cmd_strq[1];
			if (index == 12) {							/* -ESC FormFeed- */
				ccolorq('g', g_lq.color);		/* Initialize line color and style */
				dashf(1);
				break;
			}
			if (index >= '`' && index <= 'g') {			/* -ESC (lc char)- */
				dashf((char)(index - '_'));
										/* Smallest value of DASHQ is 1. */
				break;
			}
			else if (index >= 'h' && index <= 'o') {
				if (index == 'i')
					dashf(9);
				else
					dashf((char)(index - 'g'));
				break;
			}
			else if (index >= 'p' && index <= 'w') {
				dashf((char)(index - 'o'));
				break;
			}
			break;
		case 2:
			if (cmd_strq[1] == 'I') {	/* -IV- Portrait or Landscape. */
				if (cmd_strq[2] == 'V') {
					/* set flag to 0 to get page size. */

					ingetseq(INTCODE);	/* Throw away port number. */
					ingetseq(INTCODE);	/* Throw away */
					ins_getxy_pairq(&x, &y);
					ins_getxy_pairq(&x, &y);
					if (y > x) {
						if (!(rotate_flgq & 2))
							g_zq.xfact = mag *
								((float)MAXRESY / (float)MAXRESX);
						if (center == (float) -1.)
							g_zq.xshift =
								(int)((acrossq -
								MAXRESYMO * g_zq.xfact) * .5);
					}
					else {
						if (rotate_flgq & 2)
							g_zq.xfact = mag *
								((float)MAXRESX / (float)MAXRESY);
						if (center == (float) -1.)
							g_zq.xshift = (int)((acrossq -
							MAXRESXMO * g_zq.xfact) * .5);
					}
					g_zq.yfact = g_zq.xfact;
				}

			}
			if (cmd_strq[1] == 'L') {
				switch(cmd_strq[2]) {
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
						panfilq = 1;
						ins_getxy_pairq(&x, &y);
						dispatch_pointq(1, x, y);
						g_pq.border = (char)(ingetseq(INTEGERQ) - '0');
						g_pq.b_color = g_lq.color;
						break;
					case 'E':									/* -LE- */
						if (panfilq != 0) {
							if (g_pq.nsides > 2)
								panelq();
								gpcfree((void **) &g_pq.xyp);
								panproc();
						}
						panfilq = 0;
						g_pq.nsides = 0;	/* g_pq.nsides is set to 0 in
											pan_prepq() but needs to be set
											here also because file conversions
											do not use panelq() */
						break;
					case 'F':									/* -LF- */
						ins_getxy_pairq(&x, &y);
						dispatch_pointq(1, x, y);
						if (newchq)
							retval = newchq;
						break;
					case 'G':									/* -LG- */
						ins_getxy_pairq(&x, &y);
						dispatch_pointq(0, x, y);
						if (newchq)
							retval = newchq;
						if (!panfilq)
							ldrawq(g_lq.ptx, g_lq.pty);
						break;
					case 'T':									/* -LT- */
						gtextq |= 2;
						getseq(TEXTQ, &x);
						g_tq.str = stringq;
						g_tq.px = x0q;		/* put in starting point */
						g_tq.py = y0q;
						/* IF HARDWARE CHARACTER SUPPORT, CALL DRIVER */
						if (g_iq[dev_typeq].hwchar)
							f_tableq((void *)&g_tq);
						else
						   tekfntq();
						gtextq &= 0x0d;
						break;
				}
				break;
			}
			if (cmd_strq[1] == 'M') {
				index = ingetseq(INTCODE);
				switch(cmd_strq[2]) {
					case 'C':									/* -MC- */
				/* THERE ARE THREE INTEGERQ ARGUMENTS; ONLY THE SECOND
						(THE HEIGHT) IS USED BY THE 4105. */
				/* FONTS ARE 5 PELS WIDE + 2 FOR A GAP, & 6 PELS HIGH. */
					/* GET THE SECOND INTEGERQ. */
						clookq = 1;
						if (!commaq)
							g_tq.height = index;	/* height in pixels */
						else
							commaq = 0;
								/* 'commaq' means a default ', ' was sent. */
						clookq = 0;
						index = ingetseq(INTCODE);		/* Get the second integer. */
						ingetseq(INTCODE);		/* Get the last integer. */
						break;
					case 'L':									/* -ML- */
						ccolorq('g', cl_valq[index]);
						break;
					case 'P':									/* -MP- */
						NCOLORQ++;	/* INCREMENT COLOR COUNTER */
						if (index <= 0) {
							g_pq.pattern = ( -cl_valq[ - index]);
						}
						else {
							g_pq.pattern = index;
						}
						break;
					case 'R':									/* -MR- */
						index1 = ingetseq(INTCODE);
						/* On return, newchq != 0 means no second integer. */
						if (!newchq)
							index = (int)((double)index *
								pow((double)2., (double)index1) + 44.);
						index = (int)((index / 90) * 90);
						while (index < 0)
							index += 360;
						if (index > 270)
							index = 270;
						g_tq.angle = index;
						break;
					case 'T':									/* -MT- */
						if (GTINDEXQ != index) {
							GTINDEXQ = index;
							g_tq.color = cl_valq[GTINDEXQ];
							NCOLORQ++;
						}
						gtextq |= 2;
						break;
					case 'V':									/* -MV- */
						dashf((Uchar)(index + 1));
						break;
				}
				break;
			}
			if (cmd_strq[1] == 'R') {
				if (cmd_strq[2] == 'A') {							/* -RA- */
					break;
				}
			}
			ingetseq(DUMP_CH);	/* Not a recognized code. Dump remainder. */
			break;
	}
	return(retval);
}
/**************************************************************/
/* ingetseq() returns an integer if not in TEXT or XY_PAIR modes unless a ', '
is encountered in which case it sets the global variable 'commaq' to
1. In XY_PAIR it decodes the tek code sequence and calls tekcodeq() which
puts the results in the array  g_pq.xyp[].(x, y).
(These same vectors are used for panel sides.) In TEXT mode ingetseq() loads
a text string into stringq[]. */
int GCLASS ingetseq(int mode)
{
	char finish = 0, start = 1;
	int i = 0, ch;
	int value = 0;
	G_PROCESS_ID

	for (;; ) {			/* Stay in the loop until terminator. */
		if (finish)
			break;
		if (newchq) {			/* Next character was passed from reproq. */
			ch = newchq;
			newchq = 0;
		}
		else {
			ch = fgetc(infileq);
			if (ch == EOF || ch < 32) {
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
				if (ch >= 64)
					continue;
				stringq[i] = 0;
				return(decodeq(stringq));
			case TEXTQ:								/* TEXT mode */
				if (start && ch > '@') continue;	/* First a coded length */
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
/********************************************************************
ins_getxy_pairq() gets an x,y coordinate from the TEK information and returns
them to the caller.
********************************************************************/
void ins_getxy_pairq(int *x, int *y)
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
			ch = fgetc(infileq);
			if (ch == EOF || i >= 6) {
				messageq(ERRQ, 3000);
				return;
			}
			tkf_incrementq++;
		}
		str[i++] = (char)ch;
		if (ch >= 64 && ch < 96) {		/* Finished code sequence. */
			str[i] = 0;
			tekcodeq(str, x, y);			/* Decode and put in array. */
			break;;
		}
	}
}

