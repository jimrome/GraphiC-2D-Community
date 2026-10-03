/* (c) 1984-1993 by Scientific Endeavors Corporation.
   All rights reserved.
The routines for plotting characters from files of stroke tables.
Version 7.0 1/1/91
PostScript/Truetype extensions written by David Dillow.
**************************************************************/
#define __INFNTPLT
#include <graphic.h>
#include <font_ext.h>
#include <fntplt.h>
#include <truetype.h>
#include <psfont.h>

#define TITLE_SIZE (float)273.		/* .3 inches */
#define XNAM_SIZE (float)182.		/* .2 inches */
#define YNAM_SIZE (float)182.		/* .2 inches */
#define LABEL_SIZE (float)136.5		/* .15 inches */

#include <stdarg.h>

void SetFontdata(int TTChar);

void strsizq(int x, int y, char *string, float height, int ang,
	int action, int *wid, int *top, int *bottom);
void angsetq(int ang);
void rotateq(void);
void newangq(void);
void fboxq(int x, int y);
void tabitq(int action);
void supq(int action, int *top);
void subq(int action, int *bottom);
void slantq(int action);
void wideq(void);
void tfntq(void);
void set_font_paramq(int font_num, float height);
void chkifontq(char *string);
/**************************************************************/
/* FONT INITIALIZATION. ALLOCATES SPACE FOR FONTS ABOVE THE
BUFFER USED FOR THE GRAPHIC DUMPS */
void fontinitq(void)
{
	int index;

#ifdef MULTITHREAD
	G_PROCESS_ID

	if (fnt_dataq[instctr] == NULL) {
		fnt_dataq[instctr] = (struct f_dataq *)gpcalloc(1,
			sizeof(struct f_dataq));
		GENERAL_dataq[instctr] = (struct GEN_FONT_DATA *)gpcalloc(1,
			sizeof(struct GEN_FONT_DATA));
		PS_INFO_dataq[instctr] = (struct PS_INFO *)gpcalloc(1,
			sizeof(struct PS_INFO));
		TT_Char_infoq[instctr] = (struct TT_SHARE_INFO *)gpcalloc(1,
			sizeof(struct TT_SHARE_INFO));

		SetTTforOS2();
		SetPSforOS2();
	}
	memset((struct f_dataq *)fnt_dataq[instctr], '\0',
		sizeof(struct f_dataq));
#endif
	set_x_default = 1;
	set_y_default = 1;
	set_title_default = 1;
/* SPECIAL CHARACTERS */
	clrsymq = '|';			/* SYMBOL FOR CHANGING COLORS*/
	widsymq = '^';			/* SYMBOL FOR WIDENING CHARACTERS*/
	thicksymq = '#';  		/* SYMBOL FOR THICKENING CHARACTERS*/
	supsymq = '[';			/* SYMBOLS FOR SUPER-SUB-SCRIPTS*/
	subsymq = ']';			/* SYMBOLS FOR SUPER-SUB-SCRIPTS*/
	nullsymq = '@';		/* SYMBOLS FOR SUPER-SUB-SCRIPTS*/

	xlabwid = ylabwid = xlspcq;	/* LABEL WIDTH*/
	tabspcq = 5;			/* TAB*/
	xfacq = (float)1.0;
	yfacq = (float)1.0;
	widfacq = (float)1.0;	/* FACTOR FOR CHANGING CHARACTER WIDTH*/
	scrshftq = (float)0.6;	/* SCRIPT SHIFT*/
	scrhtq = (float).75;	/* SCRIPT HEIGHT*/
	line_sp = (float)1.6;	/* LINE SPACING IN TEXT - RATIO TO HEIGHT*/
	ytopq = (float)6.884;	/* TOP OF PAGE - INCHES (CM)*/
	NUM_OF_FONTS = 0;
	xnameq.string = NULL;
	xnameq.string = NULL;
	anameq.string = NULL;
	bnameq.string = NULL;
	cnameq.string = NULL;

	for(index = 0; index < FNTNUM; index++)
	{
		escsym[index] = (unsigned char)'\310';

		SetTTKerning(0, escsym[index]);
		SetTTFull(1, escsym[index]);
		SetTTMaxError(1, escsym[index]);

		SetPSMaxError(1, escsym[index]);
		SetPSFull(1, escsym[index]);
		SetPSKerning(0, escsym[index]);

		FontTypeq[index] = REGULARFONT;

		escsym[index] = '\0';
	}

	SetTTUseMemory(1);
	TTUnicodeq = 0;
    TTUnicodeSymbolq = '~';

	font(1, "simplex.fnt", '\310');
}

/************************************************************/
void TYPE fntchg(Uchar sym)
{
	char i;
	G_PROCESS_ID

	for (i = 0; i < FNTNUM; i++) {
		if (escsym[i] == sym) {
			ifontq = i;
			break;
		}
	}
}
/************************************************************/
/* CENTERS A LINE OF TEXT */
void TYPE ctline(char *string, float height)
{
	int angle;
	float wid, x, y;
	G_PROCESS_ID

	angle = 0;
	wid = strwidth(string, height);				/* Unscaled width */
	ytopq -= line_sp * height;
	y = ytopq;
	if (sclflgq)		/* Called as if full page and is to Scale to virtual
					   page as defined by the last call to page() */
		x = pixtoin((acrossq >> 1)) - wid;
	else			/* Shifted in referance to page boundarys. Size and
					   location are absolute. */
		x = pixtoin((xpageq >> 1)) - wid;
	prtfnt(x, y, string, height, angle);
}
/************************************************************/
/* Left justifies a line of text */
void TYPE ltline(char *string, float height)
{
	int angle;
	float x, y;
	G_PROCESS_ID

	angle = 0;
	strwidth(string, height);				/* Unscaled width to set up
											correct values for fboxq */
	/* SPACE DOWN A LINE */
	ytopq -= line_sp * height;
	x = xleftq;
	y = ytopq;
	prtfnt(x, y, string, height, angle);
}
/************************************************************/
/* RIGHT JUSTIFIES A LINE OF TEXT */
void TYPE rtline(char *string, float height)
{
	int angle;
	float wid, x, xoff, y;
	G_PROCESS_ID

	angle = 0;
	wid = strwidth(string, height);				/* Unscaled width */
	xoff = wid + wid + xrightq;
	ytopq -= line_sp * height;
	y = ytopq;
	if (sclflgq)		/* Called as if full page and is to Scale to virtual
						   page as defined by the last call to page() */
		x = pixtoin(acrossq) - xoff;
	else			/* Shifted in referance to page boundarys. Size and
						   location are absolute. */
		x = pixtoin(xpageq) - xoff;
	prtfnt(x, y, string, height, angle);
}
/*****************************************************************/
/* PLOT THREE STRINGS AT THE LEFT, CENTER, AND RIGHT OF THE LINE */
/* THE HEIGHTS OF EACH STRING CAN BE SPECIFIED INDEPENDENTLY     */

void TYPE lcrline(char *lstr, float hl, char *cstr, float hc,
	char *rstr, float hr)
{
	int angle;
	float wid, x, y, hmax = (float)0.0;
	G_PROCESS_ID

	angle = 0;

	if (hmax < hl)				/* Find maximum height */
		hmax = hl;
	if (hmax < hc)
		hmax = hc;
	if (hmax < hr)
		hmax = hr;
	ytopq -= line_sp * hmax;	/* Allow for biggest font */
	y = ytopq;

	/* LEFT STRING */
	if (*lstr != '\0') {
		wid = strwidth(lstr, hl); 	/* Unscaled width */
		x = xleftq;
		prtfnt(x, y, lstr, hl, angle);
	}
	/* CENTER STRING */
	if (*cstr != '\0') {
		wid = strwidth(cstr, hc);	/* Unscaled width */
		if (sclflgq) {
			x = (float).5 * pixtoin(acrossq) - wid;
		}
		else {
			x = pixtoin((xpageq >> 1)) - wid;
		}
		prtfnt(x, y, cstr, hc, angle);
	}
	/* RIGHT STRING */
	if (*rstr != '\0') {
		wid = strwidth(rstr, hr);	/* Unscaled width */
		if (sclflgq) {
			x = pixtoin(acrossq) - 2.f * wid - xrightq;
		}
		else {
			x = pixtoin(xpageq) - wid - wid - xrightq;
		}
		prtfnt(x, y, rstr, hr, angle);
	}
}
/************************************************************/
/* PRINT A STRING USING THE FONTS AT AN OFFSET OF XINCH, YINCH FROM THE
LOWER LEFT CORNER OF THE PLOT */
void TYPE prtfnt(float xinch, float yinch, char *string, float height,
	int angle)
{
	int x, y;
	G_PROCESS_ID

/* CONVERT HEIGHT OF CHARACTER IN INCHES TO MULTIPLIER
   OF THE PIXEL FONT HEIGHT */
   /* If setscale() has not been called, the scaling factors xfacq and
   yfacq are 1. */
	height = (pix_convertq * height) * yfacq;
	x = (int)((pix_convertq * xinch) * xfacq) + xbaseq;
	y = (int)((pix_convertq * yinch) * yfacq) + ybaseq;
	fntpltq(x, y, string, height, angle);
}
/**********************************************************************/
/* PLOT A STRING IN A PLOT WITH THE LOWER LEFT CORNER AT A COORDINATE
ON THE PLOT */
/* If the plot is rotated, the string will also rotate */
void TYPE pltfnt(float x, float y, char *string, float height, int angle)
{
	int xt, yt;
	float ht;
	G_PROCESS_ID

	ht = (pix_convertq * height) * yfacq;
	if (!psetflgq && !wplane_flgq)
		psetq();
	usertopix(x, y, 0, &xt, &yt);
	fntpltq(xt, yt, string, ht, angle);
}
/**********************************************************************/
/* PLOT FONT STRING AT POSITION x, y. height IS THE HEIGHT MULTIPLIER, */
/* ang IS THE ANGLE AT WHICH THE LETTERS ARE PLOTTED. */
/* Revised 8-5-84 by J. A. Rome */
/* SUPER/SUB SCRIPTS ADDED 10/19/84 JAR */
/* WIDTH EXPANSION AND SKEW ADDED 8/24/85 JAR */
/* Speed-up and circular plot. Uses newangq() and circsetq(). 8/16/85 GGK */
/* height is now in pixels. 11/28/90 */
void fntpltq(int x, int y, char *string, float height, int ang)
{
	int width, top, bottom, notek_sav, angle;
	unsigned int i;
	char res[5];
	char ch_width[5];
	char spacing[5];
	char plotsav;
	G_PROCESS_ID

	plotsav = plot3q;
	plot3q = 0;						/* FLAG USED IN LPLT WHEN IN 3D MODE */
	if (gtextq == 1) {				/* Literal text into TKF file. */
		i = 200;
		while (i == 200) {
			angle = ang;
			i = 0;
			lmoveq(x, y);
			encodeq((int)(height), res);
			encodeq((int)(height * (float).75), ch_width);
			encodeq((int)(height * (float).25), spacing);
										/* Height is in pixels (43/pel). */
			sprintf(cmd_strq, "\33MC%s%s%s", ch_width, res, spacing);
			gprosq(2);
			encodeq(itotekq[g_lq.color], res);	/* text color */
			sprintf(cmd_strq, "\33MT%s", res);
			gprosq(2);
			totekq(x, y, 1);		 	/* Not done in lmoveq(). */
			tkbfillq(cmd_strq);
			if (rotate_flgq & 1) /* Rotation must be done AFTER the move */
				angle -= 90;
			while (angle < 0)
				angle += 360;
			encodeq(angle, res);
			sprintf(cmd_strq, "\33MR%s0", res);
			tkbfillq(cmd_strq);
			if (rotate_flgq & 1) /* Rotation must be done AFTER the move */
				angle += 90;
			while (angle >= 360)
				angle -= 360;
			notek_sav = notekq;
			notekq = 1;
			encodeq(angle, res);
			sprintf(cmd_strq, "\33MR%s0", res);
			gprosq(2);
			notekq = notek_sav;
			i = strlen(string);
			if (i > 200) {
				messageq(WARNQ, 42);	/* String is too long and will */
				i = 200;				/* be printed on multiple lines. */
				*(string + i) = '\0';
				y -= (int)(line_sp * height);
			}
			encodeq((int)i, res);	/* res is index code string. */
			sprintf(cmd_strq, "\33LT%s%s", res, string);
			if (i == 200)	/* advance pointer to remainder of string. */
				string += 201;
			gprosq(2);
		}
		return;
	}
	strsizq(x, y, string, height, ang, 'p', &width, &top, &bottom);
	plot3q = plotsav;
}
/**************************************************************/
/* TO CALCULATE THE HALF-WIDTH OF A STRING OF FONTED CHARACTERS */
/* Revised 8-5-84   J. A. Rome */
/* SUPER/SUBSCRIPTS 10-20-84 JAR */
/* ELONGATED AND SKEWED FONTS 8/24/85 JAR */
/* COMBINED WITH FNTPLTQ 9/1/85 JAR */
int strwidtq(char *string, float height)
{
	int width, top, bottom;
	int tfont;	/* Need dummys to preserve entry values */
	char twide, tskew;
	int thick;
	G_PROCESS_ID

	thick = thickq;
	tfont = ifontq;
	twide = widenq;
	tskew = skewq;
	strsizq(0, 0, string, height, 0, 'c', &width, &top, &bottom);
	thickq = thick;
	ifontq = tfont;		/* REPLACE OLD VALUES */
	widenq = twide;
	skewq = tskew;
	return(width);
}
/*********************************************************************/
/* FONT ADMINISTRATION ROUTINE. PLOTS AND SIZES FONTS */
void strsizq(int x, int y, char *string, float height, int ang,
	int action, int *wid, int *top, int *bottom)
{
	char firstflg = 0, TTCharIsUnicodeq;
	int n, j, nstroke, bk, lastchar = -10;
	int dashsav;
	int txt_border_color;
	int thicksav;
	int i;
	int offset = 0;
	int width, ichar;
	int xon;
	int ch, reset, tsize, nchar;
	float lex, rex = 0.f;
	char buf[3];
	signed char *strbuf;
	G_PROCESS_ID

	if (*string == '\0') {
		*wid = 0;
		return;
	}
	dashsav = g_lq.pattern;
	txt_border_color = g_lq.color;
	thicksav = g_lq.thick;
	reset = 1;
	if (action == 'c') {
		*wid = 0;
		*top = 0;
		*bottom = 0;
	}
	else
		dashf(1);
	w1 = 0;
	xbf = (float)x;
	ybf = (float)y;
	angsetq(ang);
	twoscr = 0;
	scripton = 0;
	scrlevel = 0;
	scrtype = 0;
	i = 0;
	j = 0;
	xon = 1;
	set_font_paramq(ifontq, height);
	if (action == 'c') {			/* DEFAULT HEIGHT */
		*top = (int)height;
		tsavfq = 0;
		bsavfq = 0;
	}

	/* CHARACTER LOOP */
	while ((ch = (Uchar)string[j]) != '\0') {
		TTCharIsUnicodeq = 0;
		j++;
		bk = 0;
		for (n = 0; n < NUM_OF_FONTS; n++) {
			if (ch == (int)escsym[n]) {
				if (reset) {		/* prepare for next script */
					reset = 0;
					w2 = w3 = 0.0f;
					shiftq = 0;
				}
				set_font_paramq(n, height);

				if(FontTypeq[n] == PSFILLED || FontTypeq[n] == PSOUTLINE || FontTypeq[n] == TRUETYPE)
					lastchar = -10; /* reset last character */

				bk = 1;
				break;
			}
		}
		if (bk)
			continue;
		if ((ch == nullsymq) && (!nullflag)) {
			if (!use_allq) {
				nullflag = 1;
				continue;
			}
		}
		else if (ch == '\t') {
			tabitq(action);
			continue;
		}
		else if (!nullflag && !use_allq) {
			if (ch == supsymq) {
				supq(action, top);
				continue;
			}
			else if (ch == subsymq) {
				subq(action, bottom);
				continue;
			}
			else if (ch == '`') {
				slantq(action);
				continue;
			}
			else if (ch == widsymq) {
				wideq();
				continue;
			}
			else if (ch == thicksymq) {
				tfntq();
				continue;
			}
			else if (ch == clrsymq) {
				buf[0] = string[j++];
				if ((buf[1] = string[j++]) == clrsymq)
					buf[1] = '\0';
				else {
					buf[2] = '\0';
					j++;
				}
				ch = atoi(buf);
				color((char)ch);
				continue;
			}
			else if (ch == TTUnicodeSymbolq && TTUnicodeq) {
				ch = (unsigned char) string[j] | ((unsigned char) string[j+1] << 8);
				j += 2;
				TTCharIsUnicodeq = 1;
			}
		}

		/* Transparent UTF-8 multi-byte decoding for TrueType fonts */
		if (FontTypeq[ifontq] == TRUETYPE && (ch >= 0xC2)) {
			if (ch <= 0xDF && ((unsigned char)string[j] & 0xC0) == 0x80) {
				ch = ((ch & 0x1F) << 6) | ((unsigned char)string[j++] & 0x3F);
				TTCharIsUnicodeq = 1;
			}
			else if (ch >= 0xE0 && ch <= 0xEF &&
			         ((unsigned char)string[j] & 0xC0) == 0x80 &&
			         ((unsigned char)string[j+1] & 0xC0) == 0x80) {
				ch = ((ch & 0x0F) << 12) |
				     (((unsigned char)string[j] & 0x3F) << 6) |
				     ((unsigned char)string[j+1] & 0x3F);
				j += 2;
				TTCharIsUnicodeq = 1;
			}
			else if (ch >= 0xF0 && ch <= 0xF7 &&
			         ((unsigned char)string[j] & 0xC0) == 0x80 &&
			         ((unsigned char)string[j+1] & 0xC0) == 0x80 &&
			         ((unsigned char)string[j+2] & 0xC0) == 0x80) {
				ch = ((ch & 0x07) << 18) |
				     (((unsigned char)string[j] & 0x3F) << 12) |
				     (((unsigned char)string[j+1] & 0x3F) << 6) |
				     ((unsigned char)string[j+2] & 0x3F);
				j += 3;
				TTCharIsUnicodeq = 1;
			}
		}
		else if ((FontTypeq[ifontq] == REGULARFONT) &&
			((ch < ' ') || (ch > 199) || ch == 127)) {
				sprintf(buf, "%c", ch);
				gprintq(buf, 0);
				messageq(WARNQ, 26); /* Non-ASCII character */
				ch = '*';
		}
		if (scripton && scrlevel == 0) {
			scripton = 0;	/*OUT OF SCRIPT*/
			/* USE FARTHEST EXTENSION OF SCRIPT FOR NEW BASE */
			if (action == 'p') {
				if (twoscr) {
					if (((xsav - xhome) * (xsav - xhome) + (ysav - yhome)
						* (ysav - yhome)) > ((xbf - xhome) * (xbf - xhome)
						+ (ybf - yhome) * (ybf - yhome))) {
						xbf = xsav;
						ybf = ysav;
					}
					twoscr = 0;
				}
			}
			else {
				reset = 1;		/* RESET WIDTHS NEXT TIME THRU */
				if (twoscr) {
					w1 = (w2 > w3) ? (w1 + w2) : (w1 + w3);
				}
				else {
					w1 = (scrtype > 0) ? (w1 + w2) : (w1 + w3);
				}
				scrtype = 0;
				twoscr = 0;
			}
		}
		if (nullflag)
			nullflag = 0;
		n = ch - 32;
		if(FontTypeq[ifontq] == TRUETYPE) {
			SetFontdata(TTCharIsUnicodeq);

			TrueTypeOut(lastchar, n, action, &w1, top, bottom);

			lastchar = n;
			rex = 0.f;
			continue;
        }
        if(FontTypeq[ifontq] == PSFILLED ||
			FontTypeq[ifontq] == PSOUTLINE) {
			SetFontdata(TTCharIsUnicodeq);

			handlePSFontq(lastchar, n, action, &w1, top, bottom);

			rex = 0; /* no extenders on PS font */
			lastchar = n;	/* save char for Kerning */
			continue;
		}
		nstroke = g_fontq[ifontq].char_dataq[n].length;
		strbuf = (signed char *)g_fontq[ifontq].char_dataq[n].strokes;
		if (strbuf == NULL || nstroke <= 0) {
			continue;
		}
		nchar = strbuf[0];
				/* SET WIDTH AND DESCENDERS */
		lex = heightq * g_fontq[ifontq].char_dataq[n].lext;
		rex = heightq * g_fontq[ifontq].char_dataq[n].rext;
		width = g_fontq[ifontq].char_dataq[n].width;
		/* ADD ANY EXTRA INTER-CHARACTER SPACING */
		lwid = width + charspcq;
		if (widenq)
			lwid = (int)((float)lwid * widfacq);
		if (const_spaceq[ifontq]) {
			offset = (g_fontq[ifontq].const_width - width) / 2;
			lwid += offset * 2;
		}
		hgtwdq = heightq * lwid + lex + rex;
		if (action == 'p') {
			if (flagq)
				newangq();
			if (fboxonq && !firstflg) {
				xbf += lex * cosq;
				ybf += lex * sinq;
				firstflg = 1;
			}
			if (nchar == '}' && fillq) {
				panfilq = 1;
				g_pq.nsides = 0;
				if (g_pq.xyp != NULL)
					gpcfree((void **) &g_pq.xyp);
				g_pq.xyp = (PGXYPAIRS)gpcalloc(64,
					sizeof(struct GXYPAIRSQ));
				if (g_pq.xyp == (void *)NULL) {
					messageq(WARNQ, 2);		/* Out of Memory */
					panfilq = 0;
				}
			}
			else {
				panfilq = 0;
				if (g_lq.xyp != NULL)
					gpcfree((void **) &g_lq.xyp);
				g_lq.xyp = (PGXYPAIRS)gpcalloc(64,
					sizeof(struct GXYPAIRSQ));
				if (g_lq.xyp == (void *)NULL) {
					messageq(WARNQ, 2);		/* Out of Memory */
					g_lq.action = 0;
					g_lq.npts = 0;
				}
				else {
					g_lq.font_char = 1;
					g_lq.npts = 1;
					g_lq.action = 1;
					g_lq.start_count = 0;
				}
			}
			if (thickq)
				tline(tfontq);
			for (i = 1; i <= nstroke; i++) {
				ichar = nchar;				/* nchar == (next character). */
					/* Look ahead for segment end. */
				if(i < nstroke)
					nchar = strbuf[i];
				/* CHECK FOR BOTH FILLED AND UNFILLED FONTS */
				/* Multi-piece fonts */
				if (ichar == '{' || ichar == '}') {
												/* PEN UP OR 1ST CHARACTER */
					newseg = 1;
					if (panfilq && g_pq.nsides > 2) {
						g_pq.border = 1;
						if (fillq == 1) {
							g_pq.b_color = txt_border_color;
							g_pq.pattern = ( -g_lq.color);
						}
						else {
							g_pq.b_color = font_border_colorq;
							g_pq.pattern = font_patternq;
							if (font_border_colorq != g_lq.color) {
								color(font_border_colorq);
							}
						}
						panelq();
						gpcfree((void **) &g_pq.xyp);
						if (nchar == '}') {			/* SEPARATE SEGMENT */
							panproc();
						}
						g_pq.xyp = (PGXYPAIRS)gpcalloc(64,
							sizeof(struct GXYPAIRSQ));
						if (g_pq.xyp == (void *)NULL) {
							messageq(WARNQ, 2);		/* Out of Memory */
							panfilq = 0;
						}
					}
					continue;
				}
				else {
					if (xon) {
						xinc = ichar + offset;
						xon = 0;
						continue;
					}
					else {
						yinc = ichar;
						xon = 1;
						rotateq();
						continue;
					}
				}
			}
			xbf += lwid * heightq * cosq;
			ybf += lwid * heightq * sinq;
			if (panfilq && g_pq.nsides > 2) {
				g_pq.border = 1;
				if (fillq == 1) {
					g_pq.b_color = txt_border_color;
					g_pq.pattern = ( -g_lq.color);
				}
				else {
					g_pq.b_color = font_border_colorq;
					g_pq.pattern = font_patternq;
					if (font_border_colorq != g_lq.color) {
						color(font_border_colorq);
					}
				}
				panelq();
				gpcfree((void **) &g_pq.xyp);
				panproc();
				panfilq = 0;
			}
			else if (g_lq.action) {
				if (g_lq.npts > 2) {
					g_lq.xyp[g_lq.start_count].x =
						(g_lq.npts - g_lq.start_count) - 1;
					f_tableq(&g_lq);
				}
				g_lq.action = 0;
				g_lq.npts = 0;
				g_lq.start_count = 0;
			}
			if (g_pq.xyp != NULL)
				gpcfree((void **) &g_pq.xyp);
			if (g_lq.xyp != NULL)
				gpcfree((void **) &g_lq.xyp);
		}
		else if (action == 'c') {
			if (FontTypeq[ifontq] == TRUETYPE) {
				SetFontdata(TTCharIsUnicodeq);
				TrueTypeOut(lastchar, n, action, &w1, top, bottom);
				lastchar = n;
				rex = 0.f;
				continue;
			}
			if(FontTypeq[ifontq] == PSFILLED ||
				FontTypeq[ifontq] == PSOUTLINE) {
				SetFontdata(TTCharIsUnicodeq);
				handlePSFontq(lastchar, n,action,&w1,top,bottom);
				rex = 0.f; /* no extenders on PS font */
				lastchar = n;	/* save char for Kerning */
				continue;
			}

				/* CALCULATE STRING SIZES */
			tsize = g_fontq[ifontq].char_dataq[n].dsc;	/* DESCENDERS */
			if (tsize && (scrlevel <= 0)) {
				tsize = (int)((float)tsize * heightq);
				if ((bsavfq - tsize) < *bottom)
					*bottom = bsavfq - tsize;
			}
			tsize = g_fontq[ifontq].char_dataq[n].asc;	/* ASCENDERS */
			if (tsize && (scrlevel >= 0)) {	/* ASCENDERS */
				tsize = (int)((float)tsize * heightq);
				if ((tsavfq + tsize) > *top)
					*top = tsavfq + tsize;
			}
			/* WIDTH */
			tw = heightq * (width + charspcq + (offset * 2));
			if (widenq)
				tw = tw * widfacq;			/* EXPAND */
			w1 += tw;
		}
	}
			/* END OF INDIVIDUAL CHARACTER LOOP */
	if (action == 'c') {		/* FIND SIZES */
		if (FontTypeq[ifontq] == REGULARFONT && skewq) {		/* ADD THE PIECE THAT STICKS OUT */
			tw = height * skewfacq;
			if (widenq)
				tw = tw * widfacq;
			w1 += tw;
		}
		*wid = (((int)(w1 - charspcq * heightq)) + (int)rex) >> 1;
		wsavfq = *wid;
		tsavfq = *top;
		bsavfq = *bottom;
		/* In this pass if called from ctline() ltline(), or rtline()
		   the parameters for fboxq are calculated but not scaled. The
		   scaling for the letters is done after this pass. The box
		   dimensions are scaled in fboxq(). */
	}
	g_lq.font_char = 0;
	if (g_lq.color != txt_border_color)
		color(txt_border_color);
	if (thickq)
		tline(thicksav);
	if (action == 'p' && fboxonq)
		fboxq(x, y);
	dashf(dashsav);
}
/**************************************************************/
void angsetq(int ang)
{
	G_PROCESS_ID

	angq = ang;
	if (flagq)
		angq = 1;	/* Force use of sinq and cosq in rotateq(). */
	angd = 0.01745329252 * (double)ang;
	switch(ang) {
		case 0:
			sinq = (float)0.;
			cosq = (float)1.;
			break;
		case 90:
			sinq = (float)1.;
			cosq = (float)0.;
			break;
		case - 90:
			sinq = (float) - 1.;
			cosq = (float)0.;
			break;
		case 180:
			sinq = (float)0.;
			cosq = (float) - 1.;
			break;
		case - 180:
			sinq = (float)0.;
			cosq = (float) - 1.;
			break;
		default:
			sinq = (float)sin(angd);
			cosq = (float)cos(angd);
			break;
	}
}
/**************************************************************/
/* TO CALCULATE THE HALF-WIDTH OF A STRING IN INCHES */
float TYPE strwidth(char *in_string, float height)
{
	float fht;
	int wid;
	G_PROCESS_ID

	fht = pix_convertq * height;
	wid = strwidtq(in_string, fht);
	return(pixtoin(wid));
}
/**************************************************************/
/*ROTATE THE CHARACTER AND PLOT THE SEGMENT*/
/* Revised 8-24-85 by JAR, skew and width added */
void rotateq(void)
{
	int xtemp, ytemp;
	float xf;
	G_PROCESS_ID

	xf = (float)xinc;
	if (skewq)
		xf += yinc * skewfacq;
	if (widenq)
		xf = xf * widfacq;
	if (angq == 0) {
		xtemp = (int)(xbf + heightq * xf + (float).5);
		ytemp = (int)(ybf + heightq * yinc + (float).5);
	}
	else if (angq == 90) {
		xtemp = (int)(xbf - heightq * yinc + (float).5);
		ytemp = (int)(ybf + heightq * xf + (float).5);
	}
	else if (angq == - 90) {
		xtemp = (int)(xbf + heightq * yinc + (float).5);
		ytemp = (int)(ybf - heightq * xf + (float).5);
	}
	else if (angq == 180 || angq == - 180) {
		xtemp = (int)(xbf - heightq * xf + (float).5);
		ytemp = (int)(ybf - heightq * yinc + (float).5);
	}
	else {
		xtemp = (int)(xbf + heightq * (xf * cosq - yinc * sinq) + (float).5);
		ytemp = (int)(ybf + heightq * (xf * sinq + yinc * cosq) + (float).5);
	}
	if (newseg) {
		lmoveq(xtemp, ytemp);
		newseg = 0;
		return;
	}
	if (!panfilq || wplane_flgq != 0)
		ldrawq(xtemp, ytemp);
	else {
		g_pq.xyp[g_pq.nsides].x = xtemp;
		g_pq.xyp[g_pq.nsides].y = ytemp;
		g_pq.nsides = newbufsq(&g_pq.xyp, g_pq.nsides);
	}
}
/**************************************************************/
/* These routines place labels on the plot. The plot is centered in the
available space in psetq() in bplt.c and the axis is shrunk there if the
allowances for plot and labels exceed the space. The allowances are set in
the initialization of the g_spacesq structure. They are as follows:
Starting at an axis, there is a space tkht if the ticks are out, then a
space x/ylspc, then 1.3*lablht (labels + descenders) or 2*lablen if
the labels are upright, then labsp to the top of the axis name. An
allowance of 1.3 * ?namhtq is made for the axes names and there is an
additional allowance for clearance to the edge equal to borderq.
Finally, there is an extra allowance where appropriate of lablen for label
extension past the ends of an axis. From the top of the plot to the
heading is labsp + 0.3*titlhtq, for descenders, to the title, then
there is the additional allowance of titlht plus border. */
/* X-AXIS NAMING ROUTINE */
void TYPE GCLASS xname(char *xlabel)
{
	int fsave;
	G_PROCESS_ID

	if (xlabel == NULL)
		return;
	fsave = ifontq;
	chkifontq(xlabel);
	if (set_x_default)	/* page() called but xnamht() not */
		x_namhtq = XNAM_SIZE * labscalq;
	xnameq.string = strdupq(xlabel);
	xnameq.dup_flag = 1;
	if (xnameq.string == NULL) {
		messageq(WARNQ, 2);				/* Out of memory */
		xnameq.string = xlabel;
		xnameq.dup_flag = 0;
	}
	xnameq.ifont = ifontq;
	xnameq.fillflag = fillq;
	if (fillq == 2) {
		xnameq.color = font_patternq;
		xnameq.bcolor = font_border_colorq;
	}
	else
		xnameq.color = g_lq.color;
	ifontq = fsave;
}
/**************************************************************/
/* Y-AXIS NAMING ROUTINE */
void TYPE yname(char *ylabel)
{
	int fsave;
	G_PROCESS_ID

	if (ylabel == NULL)
		return;
	fsave = ifontq;
	chkifontq(ylabel);
	if (set_y_default)	/* page() called but ynamht() not */
		y_namhtq = YNAM_SIZE * labscalq;
	ynameq.string = strdupq(ylabel);
	ynameq.dup_flag = 1;
	if (ynameq.string == NULL) {
		messageq(WARNQ, 2);				/* Out of memory */
		ynameq.string = ylabel;
		ynameq.dup_flag = 0;
	}
	ynameq.ifont = ifontq;
	ynameq.fillflag = fillq;
	if (fillq == 2) {
		ynameq.color = font_patternq;
		ynameq.bcolor = font_border_colorq;
	}
	else
		ynameq.color = g_lq.color;
	ifontq = fsave;
}
/**************************************************************/
/* A-AXIS NAMING ROUTINE */
void TYPE aname(char *alabel)
{
	int fsave;
	G_PROCESS_ID

	if (alabel == NULL)
		return;
	fsave = ifontq;
	chkifontq(alabel);
	if (set_x_default)	/* page() called but xnamht() not */
		x_namhtq = XNAM_SIZE * labscalq;
	anameq.string = strdupq(alabel);
	anameq.dup_flag = 1;
	if (anameq.string == NULL) {
		messageq(WARNQ, 2);				/* Out of memory */
		anameq.string = alabel;
		anameq.dup_flag = 0;
	}
	anameq.ifont = ifontq;
	anameq.fillflag = fillq;
	if (fillq == 2) {
		anameq.color = font_patternq;
		anameq.bcolor = font_border_colorq;
	}
	else
		anameq.color = g_lq.color;
	ifontq = fsave;
}
/**************************************************************/
/* B-AXIS NAMING ROUTINE */
void TYPE bname(char *blabel)
{
	int fsave;
	G_PROCESS_ID

	if (blabel == NULL)
		return;
	fsave = ifontq;
	chkifontq(blabel);
	if (set_x_default)	/* page() called but xnamht() not */
		x_namhtq = XNAM_SIZE * labscalq;
	bnameq.string = strdupq(blabel);
	bnameq.dup_flag = 1;
	if (bnameq.string == NULL) {
		messageq(WARNQ, 2);				/* Out of memory */
		bnameq.string = blabel;
		bnameq.dup_flag = 0;
	}
	bnameq.ifont = ifontq;
	bnameq.fillflag = fillq;
	if (fillq == 2) {
		bnameq.color = font_patternq;
		bnameq.bcolor = font_border_colorq;
	}
	else
		bnameq.color = g_lq.color;
	ifontq = fsave;
}
/**************************************************************/
/* C-AXIS NAMING ROUTINE */
void TYPE cname(char *clabel)
{
	int fsave;
	G_PROCESS_ID

	if (clabel == NULL)
		return;
	fsave = ifontq;
	chkifontq(clabel);
	if (set_x_default)	/* page() called but xnamht() not */
		x_namhtq = XNAM_SIZE * labscalq;
	cnameq.string = strdupq(clabel);
	cnameq.dup_flag = 1;
	if (cnameq.string == NULL) {
		messageq(WARNQ, 2);				/* Out of memory */
		cnameq.string = clabel;
		cnameq.dup_flag = 0;
	}
	cnameq.ifont = ifontq;
	cnameq.fillflag = fillq;
	if (fillq == 2) {
		cnameq.color = font_patternq;
		cnameq.bcolor = font_border_colorq;
	}
	else
		cnameq.color = g_lq.color;
	ifontq = fsave;
}
/**************************************************************/
/* PLOT TITLING ROUTINE */
void TYPE heading(char *title)
{
	int fsave;
	G_PROCESS_ID

	if (title == NULL)
		return;
	fsave = ifontq;
	chkifontq(title);
	set_titlhtq();
	titleq.spacing = (int)((float)0.3 * titlhtq);
	titleq.string = strdupq(title);
	titleq.dup_flag = 1;
	if (titleq.string == NULL) {
		messageq(WARNQ, 2);				/* Out of memory */
		titleq.string = title;
		titleq.dup_flag = 0;
	}
	titleq.ifont = ifontq;
	titleq.fillflag = fillq;
	if (fillq == 2) {
		titleq.color = font_patternq;
		titleq.bcolor = font_border_colorq;
	}
	else
		titleq.color = g_lq.color;
	ifontq = fsave;
}
/**************************************************************/
void labels2q(void)
{
	int font_save;
	int xp, yp, width;
	int tick;
	char fillsave;
	int color_save;
	int fontbcolor;
	int fontpat;
	G_PROCESS_ID

	fillsave = fillq;
	fontbcolor = font_border_colorq;
	fontpat = font_patternq;
	font_save = ifontq;
	tick = ((tkhtq < 0) ? -tkhtq : 0);
	color_save = g_lq.color;

/* X axis label */
	if (xnameq.string != NULL) {
		ifontq = xnameq.ifont;
		fillq = (char)xnameq.fillflag;
		if (fillq == 2) {
			font_border_colorq = xnameq.bcolor;
			font_patternq = xnameq.color;
		}
		else
			color(xnameq.color);
		width = strwidtq(xnameq.string, x_namhtq);
		if (Y_ON_LEFT || Y_ON_RIGHT || X_ON_TOP || X_ON_BOTTOM)
			xp = xphysorq + (xlenq >> 1) - width;
		else if (gabs(xmaxq) > gabs(xorigq))
			xp = xphysorq + xlenq - 2 * width - labspq;
		else
			xp = xphysorq + labspq;
		if (X_ON_TOP) {
			yp = yphysorq + ylenq + tick + xnameq.spacing + xlabwid;
		}
		else {
			yp = yzeroq - (X_ON_BOTTOM ? tick : gabs(tkhtq)) -
				xnameq.spacing - (int)x_namhtq - xlabwid;
		}
		fntpltq(xp, yp, xnameq.string, x_namhtq, 0);
		if (xnameq.dup_flag)
			gpcfree((void **) &(xnameq.string));
		xnameq.string = NULL;
		xlabwid = xlspcq;
	}

/* Y axis labels */
	if (ynameq.string != NULL) {
		ifontq = ynameq.ifont;
		fillq = (char)ynameq.fillflag;
		if (fillq == 2) {
			font_border_colorq = ynameq.bcolor;
			font_patternq = ynameq.color;
		}
		else
			color(ynameq.color);
		width = strwidtq(ynameq.string, y_namhtq);
		if (Y_ON_LEFT || Y_ON_RIGHT || X_ON_TOP || X_ON_BOTTOM)
			yp = yphysorq + (ylenq >> 1) - width;
		else if (gabs(ymaxq) > gabs(yorigq))
			yp = yphysorq + ylenq - 2 * width - labspq;
		else
			yp = yphysorq + labspq;
		if (Y_ON_RIGHT || (right_flgq & DRAWING_RIGHT_AXIS))
			xp = xphysorq + xlenq + ylabwid + (int)(y_namhtq + .5) +
				ynameq.spacing;
		else if (!(Y_ON_LEFT))
			xp = xzeroq + (int)y_namhtq + borderq + tkhtq + ynameq.spacing;
		else
			xp = xzeroq - ylabwid - (Y_ON_LEFT ? tick : gabs(tkhtq)) -
				ynameq.spacing;
		fntpltq(xp, yp, ynameq.string, y_namhtq, 90);
		if (ynameq.dup_flag)
			gpcfree((void **) &(ynameq.string));
		ynameq.string = NULL;
		ylabwid = xlspcq;
	}

/* A axis labels */
	if (anameq.string != NULL) {
		ifontq = anameq.ifont;
		fillq = (char)anameq.fillflag;
		if (fillq == 2) {
			font_border_colorq = anameq.bcolor;
			font_patternq = anameq.color;
		}
		else
			color(anameq.color);
		width = strwidtq(anameq.string, x_namhtq);
		xp = xphysorq + (tri_lenq >> 2) -
			(xlabwid + xlspcq + (int)lablhtq) - width;
		yp = yphysorq +
			((int)(((float)tri_lenq * SQRT_OF_3)) >> 2) + (xlabwid / 2) -
			(int)(((float)width * SQRT_OF_3) / 2.);
		fntpltq(xp, yp, anameq.string, x_namhtq, 60);
		if (anameq.dup_flag)
			gpcfree((void **) &(anameq.string));
		anameq.string = NULL;
	}

/* B axis labels */
	if (bnameq.string != NULL) {
		ifontq = bnameq.ifont;
		fillq = (char)bnameq.fillflag;
		if (fillq == 2) {
			font_border_colorq = bnameq.bcolor;
			font_patternq = bnameq.color;
		}
		else
			color(bnameq.color);
		width = strwidtq(bnameq.string, x_namhtq);
		xp = xphysorq + (tri_lenq >> 1) - width;
		yp = yphysorq - ylspcq - labspq -
			(int)((float)1.3 * (float)(x_namhtq + lablhtq) + (float)0.5);
		fntpltq(xp, yp, bnameq.string, x_namhtq, 0);
		if (bnameq.dup_flag)
			gpcfree((void **) &(bnameq.string));
		bnameq.string = NULL;
	}

/* C axis labels */
	if (cnameq.string != NULL) {
		ifontq = cnameq.ifont;
		fillq = (char)cnameq.fillflag;
		if (fillq == 2) {
			font_border_colorq = cnameq.bcolor;
			font_patternq = cnameq.color;
		}
		else
			color(cnameq.color);
		width = strwidtq(cnameq.string, x_namhtq);
		xp = xphysorq + ((3 * tri_lenq) >> 2) +
			(xlabwid + xlspcq + (int)lablhtq);
		yp = yphysorq +
			((int)(((float)tri_lenq * SQRT_OF_3)) >> 2) + (xlabwid / 2) +
			(int)(((float)width * SQRT_OF_3) / 2.);
		fntpltq(xp, yp, cnameq.string, x_namhtq, -60);
		if (cnameq.dup_flag)
			gpcfree((void **) &(cnameq.string));
		cnameq.string = NULL;
	}

/* Heading label */
	if (titleq.string != NULL) {
		ifontq = titleq.ifont;
		fillq = (char)titleq.fillflag;
		if (fillq == 2) {
			font_border_colorq = titleq.bcolor;
			font_patternq = titleq.color;
		}
		else
			color(titleq.color);
		width = strwidtq(titleq.string, titlhtq);
		if (set_title_default && width > 0) {
			int max_w = (tri_plotq ? tri_lenq : xlenq);
			if (max_w <= 0)
				max_w = (int)((float)xpageq * 0.88f);
			if ((width << 1) > max_w && max_w > 0) {
				float scale = (float)max_w / (float)(width << 1);
				float min_scale = 0.35f;
				if (scale < min_scale && (width << 1) * min_scale <= (int)((float)xpageq * 0.88f))
					scale = min_scale;
				titlhtq *= scale;
				titleq.spacing = (int)((float)0.3 * titlhtq);
				width = strwidtq(titleq.string, titlhtq);
			}
		}
		if (tri_plotq)
			xp = xphysorq + (tri_lenq >> 1) - width;
		else
			xp = xphysorq + (xlenq >> 1) - width;
	/* yp = bottom of the y axis
			+ the length of the y axis
			+ the distance the y labels extend past the end
			+ the value set by the user or 1/3 the title height.
	*/
		yp = yphysorq + ylenq + titloffq +
			titleq.spacing;
		if (set_title_default && (yp + (int)titlhtq) > (ybaseq + ypageq - 40)) {
			int y_avail = (ybaseq + ypageq - 40) - (yphysorq + ylenq + titloffq);
			if (y_avail > 20) {
				titlhtq = (float)y_avail / 1.35f;
				titleq.spacing = (int)((float)0.3 * titlhtq);
				width = strwidtq(titleq.string, titlhtq);
				yp = yphysorq + ylenq + titloffq + titleq.spacing;
				if (tri_plotq)
					xp = xphysorq + (tri_lenq >> 1) - width;
				else
					xp = xphysorq + (xlenq >> 1) - width;
			}
		}
		fntpltq(xp, yp, titleq.string , titlhtq, 0);
		if (titleq.dup_flag)
			gpcfree((void **) &(titleq.string));
		titleq.string = NULL;
	}
	fillq = fillsave;
	font_border_colorq = fontbcolor;
	font_patternq = fontpat;
	ifontq = font_save;
	color(color_save);
}
/*************************************************************
SET OR RESET FLAG FOR CURVED FNTPLT */
void circsetq(int flag, int r)
{
	G_PROCESS_ID

	flagq = flag;
	if (flag)
		rhoq = r + r;
}
/****************************
CALL FROM FNTPLT TO GET NEW ANGLE
newangq allows the angle of plot to change between characters. */
void newangq(void)
{
	static float dthold = 0;
	G_PROCESS_ID

	dtheta = hgtwdq / rhoq;
	if (flagq == 1) {
		angd -= dtheta;
		flagq++;
	}
	else
		angd -= (dthold + dtheta);
	dthold = dtheta;
	sinq = (float)sin(angd);
	cosq = (float)cos(angd);
}
/*************************************************************/
/* draw a tight fitting box around a string */
void fboxq(int x, int y)
{
	int j, width, bot, top;
	G_PROCESS_ID

	j = intopix((float)(fboxsizq * xfacq));
	width = wsavfq * 2;
	bot = (int)((float)bsavfq * yfacq);
	top = (int)((float)tsavfq * yfacq);
	lmoveq(x - j, y + bot - j);
	ldrawq(x + width + j, y + bot - j);
	ldrawq(x + width + j, y + top + j);
	ldrawq(x - j, y + top + j);
	ldrawq(x - j, y + bot - j);
}
/*************************************************************
Get the angular half-width of a string. r is in pixels. */
float angwidq(char *string, float height, float r)
{
	float fwidth;
	int width;
	width = strwidtq(string, height);
	fwidth = (float)width / r;
	return(fwidth);
}
/*************************************************************
Puts text on a circle of radius r, starting at r, theta and centered at
x, y. Pixel units. */
void TYPE circtxt(float x, float y, float r, char *string, float hgt, float angle)
{
	int intx, inty, intr;
	G_PROCESS_ID

	hgt = (pix_convertq * hgt) * xfacq;
	if (sclflgq) {
		intx = (int)((pix_convertq * x) * xfacq) + xbaseq;
		inty = (int)((pix_convertq * y) * yfacq) + ybaseq;
	}
	else {
		intx = intopix(x);
		inty = intopix(y);
	}
	intr = (int)(((pix_convertq * r) * yfacq) + (hgt * (float)3));
	circtxtq(intx, inty, intr, string, hgt, angle);
}
/*************************************************************
Puts text on a circle of radius r, starting at r, theta and centered at
x, y. Pixel units. */
void circtxtq(int xpix, int ypix, int r, char *string, float hgt, float angle)
{
	int phi;
	float pi = (float)3.14159;

	phi = (int)(angle + (float).5 - (float)90);
	xpix += (int)((double)r * cos((double)(angle * pi / 180.)));
	ypix += (int)((double)r * sin((double)(angle * pi / 180.)));
	circsetq(1, r);		/* Sets flag for angle change in fntplt. */
	fntpltq(xpix, ypix, string, hgt, phi);
	circsetq(0, 0);		/* Reset fntplt for linear text. */
}
/*************************************************************
The following 5 routines are used by strsizq() above. */
void tabitq(int action)
{
	float twid;
	int top = 0, bot = 0;
	G_PROCESS_ID

	if(FontTypeq[ifontq] == TRUETYPE) {
		SetFontdata(0);

		TrueTypeOut(-10, 0, 'c', &twid, &top, &bot);
		hgtwdq = twid * tabspcq;
	}
	else if(FontTypeq[ifontq] == PSFILLED || FontTypeq[ifontq] == PSOUTLINE) {
		SetFontdata(0);

		handlePSFontq(-10,0,'c', &twid, &top, &bot);
		hgtwdq = twid * tabspcq;
	}
	else {
		lwid = tabspcq * (g_fontq[ifontq].char_dataq[0].width + charspcq);
		hgtwdq = heightq * lwid;
	}

	if (action == 'p') {
		if (flagq)
			newangq();
		xbf += hgtwdq * cosq;
		ybf += hgtwdq * sinq;
	}
	else {
		if (scrtype < 0)
			w3 += hgtwdq;
		else if (scrtype == 0)
			w1 += hgtwdq;
		else
			w2 += hgtwdq;
	}
}
/*****************/
void GCLASS supq(int action, int *top)
{
	G_PROCESS_ID

	scrlevel++;
	if (scripton == 0) {	/* record base position */
		xhome = (int)xbf;
		yhome = (int)ybf;
		scripton = 1;
		scrtype = 1;
		w2 = (float)0.0;
	}
	else if (scrlevel == 1) {	/*WAS A SUBSCRIPT NOW*/
		twoscr = 1;				/*ADD SUPERSCRIPT */
		/*RECORD EXTENSION FOR WHEN EXITING SCRIPTS */
		xsav = xbf;
		ysav = ybf;
		xbf = (float)xhome;
		ybf = (float)yhome;
		scrtype = 1;
		w2 = (float)0.0;
	}
	/* CHANGE BASELINE AND HEIGHT */
	if (scrlevel > 0) {	/*SUPERSCRIPT GOING UP */
		shiftq = (int)((fnthtq * heightq) * scrshftq);
		heightq = heightq * scrhtq;
	}
	else {	/*SUBSCRIPT GOING UP */
		if (scrhtq != 0)
			heightq = heightq / scrhtq;	/*BIGGER */
		shiftq = (int)((fnthtq * heightq) * scrshftq);
	}
	if (action == 'c') {
		bsavfq += shiftq;
		tsavfq = bsavfq + (int)(fnthtq * heightq);	/*NEW TOP NEEDED*/
		if (tsavfq > *top)
			*top = tsavfq;
	}
	xbf -= shiftq * sinq + (float).5;
	ybf += shiftq * cosq + (float).5;
}
/*******************/
void GCLASS subq(int action, int *bottom)
{
	G_PROCESS_ID

	scrlevel--;
	if (scripton == 0) {	/* record base position */
		xhome = (int)xbf;
		yhome = (int)ybf;
		scripton = 1;
		scrtype = -1;
		w3 = (float)0.0;
	}
	else if (scrlevel == -1) {	/*WAS A SUPERSCRIPT NOW*/
		twoscr = 1;					/*ADD SUBSCRIPT */
		/*RECORD EXT. FOR WHEN EXITING SCRIPTS */
		xsav = xbf;
		ysav = ybf;
		xbf = (float)xhome;	/*START AT END OF ITEM TO BE SCRIPTED */
		ybf = (float)yhome;
	}
	/* CHANGE BASELINE AND HEIGHT */
	if (scrlevel < 0) {	/*SUBSCRIPT GOING DOWN */
		shiftq = (int)((fnthtq * heightq) * (-scrshftq));
		heightq = heightq * scrhtq;
	}
	if (scrlevel >= 0) {	/* SUPERSCRIPT GOING DOWN */
		if (scrhtq != 0)
			heightq = heightq / scrhtq;	/*BIGGER */
		shiftq = (int)((fnthtq * heightq) * (-scrshftq));
	}
	if (action == 'c') {
		bsavfq += shiftq;
		if (bsavfq < *bottom)
				*bottom = bsavfq;	/* NEW BOTTOM */
	}
	xbf -= shiftq * sinq;
	ybf += shiftq * cosq;
}
/****************/
void slantq(int action)
{
	G_PROCESS_ID

	if(FontTypeq[ifontq] != REGULARFONT) {
		if(skewq) skewq = 0;	/* PS handler will do calcs */
		else skewq = 1;
		return;
	}

	if (skewq) {
		skewq = 0;
		/* ADD THE PIECE THAT STICKS OUT */
		tw = heightq * fnthtq * skewfacq;
		if (widenq)
			tw = tw * widfacq;
		if (action == 'c') {
			if (scrtype < 0)
				w3 += tw;
			else if (scrtype == 0)
				w1 += tw;
			else
				w2 += tw;
		}
		else {	/* MOVE THE BASE OF THE NEXT LETTER */
			xbf += tw * cosq;
			ybf += tw * sinq;
		}
	}
	else
		skewq = 1;
}
/****************/
void wideq(void)
{
	G_PROCESS_ID

	widenq = (char)(1 - widenq);
}
/************************************************************/
/* SET LEFT MARGIN FOR TEXT PLOTS */
void TYPE lmargin(float marginch)
{
	G_PROCESS_ID

	xleftq = marginch;
}
/************************************************************/
/* SET TOP MARGIN IN TEXT PLOTS */
void TYPE tmargin(float marginch)
{
	G_PROCESS_ID

	if (sclflgq) {
		ytopq = pixtoin(topq) - marginch;
	}
	else {
		/* ytopq is the number pf inches from the bottom of the
			area defined by the call to page() */
		ytopq = pixtoin(ypageq) - marginch;
	}
}
/************************************************************/
/* SET RIGHT MARGIN IN TEXT PLOTS */
void TYPE rmargin(float marginch)
{
	G_PROCESS_ID

	xrightq = marginch;
}
/***********************************************************/
void TYPE fbox(int boxon, float size)
{
	G_PROCESS_ID

	fboxonq = boxon;
	fboxsizq = size;
}
/***********************************************************
setscale(fac) allows the user to decide if the font routines will
scale with the page size or if the sizes will be absolute.
Fac of 0 means no scaling. */
void TYPE setscale(char fac)
{
	G_PROCESS_ID

	sclflgq = fac;
	if (fac == 1) {
		xfacq = (float)xfact_pgq;
		yfacq = (float)yfact_pgq;
	}
	else
		xfacq = yfacq = (float)1.0;
}
/*************************************************************
Sets the font width in inches (or cm). Reset it with 0.0 */
void TYPE tifont(float wid)
{
	int w;
	G_PROCESS_ID

	w = intopix(wid);
	if (w > 21) {
		messageq(WARNQ, 12);		/* Max line width is 21 */
		w = 21;
	}
	tfontq = w;
}
/*************************************************************
Sets the font width . Reset it with zero. */
void TYPE tfont(int wid)
{
	G_PROCESS_ID

	if (wid > 21) {
		messageq(WARNQ, 12);		/* Max line width is 21 */
		wid = 21;
	}
	tfontq = wid;
}
/*************************************************************/
void tfntq(void)
{
	G_PROCESS_ID

	thickq = 1 - thickq;
	if (!thickq)
		tline(0);
}
/***********************************************************/
/* DEFINE WHETHER FONTS SHOULD BE FILLED, AND IF SO, WITH WHICH PATTERN
This routine is included to maintain compatibility with
version 6 of GraphiC. */

void TYPE fillfont(int fill)
{
	G_PROCESS_ID

	fontfill(fill, g_lq.color, g_lq.color);
}
/***********************************************************/
/* DEFINE WHETHER FONTS SHOULD BE FILLED, AND IF SO, WITH WHICH PATTERN */
/*
	fill = 0 -> Don't fill
	fill = 1 -> Fill using present colors
	fill = 2 -> Use these colors
*/
void TYPE fontfill(int fill, int fill_pattern, int border_color)
{
	G_PROCESS_ID

	fillq = (char)fill;
	font_patternq = fill_pattern;
	font_border_colorq = border_color;
}
/**********************************************************/
/* ROUTINES FOR CHANGING THE CHARACTER ATTRIBUTES */
void TYPE skew(float slant)
{
	G_PROCESS_ID

	skewfacq = slant;
}
/**********************************************************/
void TYPE widen(float fact)
{
	G_PROCESS_ID

	widfacq = fact;
}
/************************************************************/
/*ROUTINES FOR SUPER/SUBSCRIPT PARAMETERS */
void TYPE supsym(char ch)
{
	G_PROCESS_ID

	supsymq = ch;
}
/**********************************************************/
void TYPE subsym(char ch)
{
	G_PROCESS_ID

	subsymq = ch;
}
/************************************************************/
/* Set the symbol to flag special characters for printing.
The default is @ */
void TYPE nullsym(char ch)
{
	G_PROCESS_ID

	nullsymq = ch;
}
/**********************************************************/
void TYPE scrht(float height)
{
	G_PROCESS_ID

	scrhtq = height;
}
/**********************************************************/
void TYPE scrshft(float shift)
{
	G_PROCESS_ID

	scrshftq = shift;
}
/**********************************************************/
void TYPE tabspace(int tspace)
{
	G_PROCESS_ID

	tabspcq = tspace;
}
/************************************************************/
/* TITLE HEIGHT PARAMETER SETTING ROUTINES */
void TYPE titlht(float inch)
{
	G_PROCESS_ID

	titlhtq = labscalq * intopix(inch);	/* titlht now is in pixels. */
	set_title_default = 0;
}
/**********************************************************/
/* AXIS NUMBER HEIGHT PARAMETER SETTING ROUTINES */
void TYPE numht(float inch)
{
	G_PROCESS_ID

	lablhtq = labscalq * (pix_convertq * inch); /* lablht is in (float)pixels. */
}
/**************************************************/
/* AXIS TITLE HEIGHT PARAMETER SETTING ROUTINES */
void TYPE axnamht(float inch)
{
	xnamht(inch);
	ynamht(inch);
}
/**************************************************/
/* X-AXIS TITLE HEIGHT PARAMETER SETTING ROUTINES */
void TYPE xnamht(float inch)
{
	G_PROCESS_ID

	x_namhtq = labscalq * intopix(inch); /* x_namhtq is in pixels. */
	set_x_default = 0;
}
/**************************************************/
/* Y-AXIS TITLE HEIGHT PARAMETER SETTING ROUTINES */
void TYPE ynamht(float inch)
{
	G_PROCESS_ID

	y_namhtq = labscalq * intopix(inch); /* y_namht is in pixels. */
	set_y_default = 0;
}
/**************************************************/
/* TRI-AXIS TITLE HEIGHT PARAMETER SETTING ROUTINES */
void TYPE trinamht(float inch)
{
	xnamht(inch);
}
/*********************************************************/
/* PARAMETER-SETTING FOR SPACES BETWEEN CHARACTERS */
void TYPE charspc(int width)
{
	int i;
	G_PROCESS_ID

	for (i = 0; i < NUM_OF_FONTS; i++) {
		char_spc[i] = (width * g_fontq[i].fntht) / 42;
	}
}
/*********************************************************/
/*Called from psetq() in POSITION.C to set the label width after finding the
longest label. The labwid value is then used to locate the axis name */
void GCLASS lab_wid_setq(int axis, int width)
{
	G_PROCESS_ID

	switch(axis) {
		case GPC_X_AXIS:
			xlabwid = width;
		break;
		case GPC_Y_AXIS:
			ylabwid = width;
		break;
	}
}

/*********************************************************************
Set the spacing between the given axis and the axis name. This function
would ordinarily be used only when not using the default labeling
routines. The parameter axis should be one of the defined (in GRAPHIC.H)
constants GPC_X_AXIS, GPC_Y_AXIS, GPC_Z_AXIS. */

void TYPE GCLASS SetNameSpace(int axis, float factor)
{
	int width;
	G_PROCESS_ID

	switch(axis) {
		case GPC_X_AXIS:
			width = (int)(xlspcq * factor);
			xnameq.spacing = width;
		break;
		case GPC_Y_AXIS:
			width = (int)(ylspcq * factor);
			ynameq.spacing = width;
		break;
	}
}
/*********************************************************************
Set the spacing between the given axis and the tick labels.
The parameter axis should be one of the defined (in GRAPHIC.H)
constants GPC_X_AXIS, GPC_Y_AXIS, GPC_Z_AXIS. */

void TYPE GCLASS SetLabelSpace(int axis, float factor)
{
	int *lspcq = NULL;
	G_PROCESS_ID

	switch(axis) {
		case GPC_X_AXIS:
			lspcq = &xlspcq;
		break;
		case GPC_Y_AXIS:
			lspcq = &ylspcq;
		break;
		case GPC_Z_AXIS:
			lspcq = &zlspcq;
		break;
	}
	if (lspcq != NULL)
		*lspcq = (int)(*lspcq * factor);
	else
		messageq(WARNQ, 49);
}

/*********************************************************************
Set the spacing between the top of the Y Axis and the bottom of the
plot title. The default is to be one third of the title height. If this
function is used the value passed in will be added to the default.
*********************************************************************/

void TYPE GCLASS SetTitleSpace(float factor)
{
	G_PROCESS_ID

	titleq.spacing = (int)(TITLE_SIZE * .3f * factor);
}

/************************************************************/
/* SET LINESPACING IN TEXT PLOTS */
void TYPE linesp(
	float ratio		/* spacing is height*ratio. Default = 1.6 */
)
{
	G_PROCESS_ID

	line_sp = ratio;
}
/*************************************************************/
/* Sets the flag use_allq to allow the user to print a string without
having to put an '@' in front of the special characters. Returns the
present state so that a routine can save and restore the flag. */

char TYPE symoff(char flag)
{
	G_PROCESS_ID

	char temp = use_allq;
	use_allq = flag;

	return(temp);
}
/**************************************************************/
/* User can put a label anywhere on the plot. It is automatically placed
in a close-fitting window. Position units can be user, inch, or pixel. */
void TYPE putlabel(char *string, float xpos, float ypos, float height,
	int border, int units)
{
    GPCPutLabel(string, xpos, ypos, height, border, units, g_lq.color);
}
/**************************************************************/
/* User can put a label anywhere on the plot. It is automatically placed
in a close-fitting window. Position units can be user, inch, or pixel. */
void TYPE GPCPutLabel(char *string, float xpos, float ypos, float height,
	int border, int units, int bk_color)
{
	char ch;
	int i = 0, j = 0, xpix, ypix, ypix_sav;
	char buf[240];
	int width, max_wid = 0;
	int allow = 15, ydel;
	int LL_X, LL_Y, UR_X, UR_Y;
	G_PROCESS_ID

	height = pix_convertq * height;
	ydel = (int)(height * line_sp);
	switch(units) {
		case USER_UNITS:
			usertopix(xpos, ypos, 0, &xpix, &ypix);
			ypix_sav = ypix;
			break;
		case INCH_UNITS:
			xpix = intopix(xpos);
			ypix_sav = ypix = intopix(ypos);
			break;
		default:
			xpix = (int)xpos;
			ypix_sav = ypix = (int)ypos;
	}
	for (;; ) {
		ch = string[i++];
		if (ch != '\n' && ch != '\0') {   /* Parse the string for CRLFs. */
			buf[j++] = string[i - 1];
			if (j >= 239) {
				messageq(WARNQ, 33);		/* Buffer overflow */
				i--;
				string[i] = '\0';		/* Terminate string. */
				continue;				/* Process what we can. */
			}
		}
		else {
			buf[j] = '\0';
			width = strwidtq(buf, height);
			if (width > max_wid)
				max_wid = width;
			if (ch != '\0')
				ypix -= ydel;
			j = 0;
		}
		if (ch == '\0')
			break;
	}
	LL_X = xpix - allow;
	LL_Y = ypix - allow + bsavfq;
	UR_X = xpix + (max_wid << 1) + allow;
	UR_Y = ypix_sav + tsavfq + allow;
	/* border == 1 for border, 2 for no border */
	if (border == 1)
	    border = GPC_HAS_BORDER;
	else
	    border = GPC_NO_BORDER;

	if (bk_color != g_lq.color) {
		panfilq = 1;
		g_pq.nsides = 4;
		if (g_pq.xyp == NULL) {
			g_pq.xyp = (struct GXYPAIRSQ *)gpcalloc(5,
				sizeof(struct GXYPAIRSQ));
			if (g_pq.xyp == NULL)
				messageq(WARNQ, 2);		/* Out of Memory */
		}
		if (g_pq.xyp != NULL) {
		    g_pq.xyp[0].x = LL_X;
		    g_pq.xyp[0].y = LL_Y;
		    g_pq.xyp[1].x = UR_X;
		    g_pq.xyp[1].y = LL_Y;
		    g_pq.xyp[2].x = UR_X;
		    g_pq.xyp[2].y = UR_Y;
		    g_pq.xyp[3].x = LL_X;
		    g_pq.xyp[3].y = UR_Y;
		    g_pq.xyp[4].x = LL_X;
		    g_pq.xyp[4].y = LL_Y;
		    g_pq.pattern = bk_color;
            g_pq.border = 0;
			panelq();
			gpcfree((void **) &g_pq.xyp);
			panproc();
			g_pq.nsides = 0;
		}
	}
	j = i = 0;
	ypix = ypix_sav;
	for (;; ) {
		ch = string[i++];
		if (ch != '\n' && ch != '\0') {   /* Parse the string for CRLFs. */
			buf[j++] = string[i - 1];
		}
		else {
			buf[j] = '\0';
			width = strwidtq(buf, height);
			fntpltq(xpix, ypix, buf, height, 0);
			if (ch != '\0')
				ypix -= ydel;
			j = 0;
		}
		if (ch == '\0')
			break;
	}
    win2q(LL_X, LL_Y, UR_X, UR_Y, border);
}
/*********************************************************/
/* Sets labscalq, the scaling factor for partial-page-sized plots. */

void scale_fontsq(void)
{
	G_PROCESS_ID

	labscalq = (xpageq > ypageq) ?
		(float)xpageq / (float)MAXRESXMO :
		(float)ypageq / (float)MAXRESXMO;
	symhtq = ((float)10. * labscalq);
	lablhtq = LABEL_SIZE * labscalq;
	set_x_default = 1;
	set_y_default = 1;
	set_title_default = 1;
}
/***************************************************************************
This routine loads the g_fntq packet with the proper values for font
scaling */
void set_font_paramq(int font_num, float height)
{
	G_PROCESS_ID

	ifontq = font_num;
	fnthtq = g_fontq[font_num].fntht;
	if (fnthtq <= 0) fnthtq = 21;
	charspcq = char_spc[font_num];
	heightq = height / (float)fnthtq;	/* heightq = pixels/pel */
}
/********************************************************************
    FUNCTION NAME: chkifontq
    ARGUMENTS: string - string for which ifontq is to checked
    PURPOSE: Check to see if font parameters should be set
********************************************************************/
void chkifontq(char *string)
{
	int i;
	G_PROCESS_ID

	if ((Uchar)(*string) > (Uchar)127) {
		for (i = 0; i < FNTNUM; i++) {
			if (escsym[i] == (Uchar) * string)
				ifontq = i;
		}
	}
}
/*********************************************************************/
/* Select a font for GrafText (PostScript only) */
void TYPE GTfont(int font_number)
{
	char res[5];
	G_PROCESS_ID

	if (font_number < 0 || font_number > 29)
		return;
	if (gtextq == 1) {		/* Literal text into TKF file. */
		encodeq(font_number, res);
		sprintf(cmd_strq, "\33MF%s", res);
		gprosq(2);
	}
}
/**********************************************************************/
/* Set a flag to turn on constant character spacing to the width of the
character char_width. The constant spacing will be turned on only for
the current active font. The argument char_width has the following values:
	 -1 --> Use the widest character in the font for constant spacing.
	  0 --> Use proportional spacing (default).
	>31 --> Use the width of the input character for constant spacing.
This routine must be called AFTER the call to font(). */

void TYPE setspacing(int char_width, unsigned char fnt_select)
{
	int i, ifont;
	G_PROCESS_ID

	ifont = 1000;
	for (i = 0; i < FNTNUM; i++) {
		if (escsym[i] == fnt_select) {
			ifont = i;
			break;
		}
	}
	if (ifont == 1000) {
		messageq(WARNQ, 34);	/* Input fnt_select not valid, using current
							   active font. */
		ifont = ifontq;
	}
	if (char_width == 0)	/* Turn off constant spacing. */
		const_spaceq[ifont] = 0;
	else {
		const_spaceq[ifont] = 1;
		if (char_width == -1)	/* User wants width of widest char. */
		{
			switch(FontTypeq[ifont])
			{
				case TRUETYPE:
				case PSFILLED:
				case PSOUTLINE:
					gpcGetWidestChar(escsym[ifont], (unsigned char *) &i, &g_fontq[ifont].const_width);
					break;
				case REGULARFONT:
					g_fontq[ifont].const_width = g_fontq[ifont].maxwidth;
					break;
			}
		}
		else
		{
			switch(FontTypeq[ifont])
			{
				case TRUETYPE:
				case PSFILLED:
				case PSOUTLINE:
					gpcGetCharWidth(escsym[ifont], (unsigned char) (char_width - 32), &g_fontq[ifont].const_width);
					break;
				case REGULARFONT:
					g_fontq[ifont].const_width =
						g_fontq[ifont].char_dataq[char_width - 32].width;
					break;
			}
		}
	}
}
/*******************************************************************/
/* THIS ROUTINE RETURNS THE WIDEST CHARACTER AND IT'S WIDTH */
void TYPE gpcGetWidestChar(unsigned char fnt_select,
	unsigned char *char_num,
	int *width)
{
	int i, n, ifont = 0;
	G_PROCESS_ID

	*width = 0;
	for (n = 0; n < NUM_OF_FONTS; n++) {
		if (fnt_select == escsym[n])
			ifont = n;
	}

	switch(FontTypeq[ifont])
	{
		case TRUETYPE:
			GetTTWidest(ifont, width, char_num);
			break;
		case PSFILLED:
		case PSOUTLINE:
			GetPSWidest(ifont, width, char_num);
			break;
		case REGULARFONT:
			for (i = 0; i < 168; i++) {
				if (g_fontq[ifont].char_dataq[i].width > *width) {
					*width = g_fontq[ifont].char_dataq[i].width;
					*char_num = (char)(i + 32);
				}
			}
			break;
	}
}
/*******************************************************************/
/* THIS ROUTINE RETURNS THE WIDTH OF A SPECIFIED CHARACTER */
void TYPE gpcGetCharWidth(unsigned char fnt_select,
	unsigned char char_num,
	int *width)
{
	int n, ifont = 0;
	G_PROCESS_ID

	*width = 0;
	for (n = 0; n < NUM_OF_FONTS; n++) {
		if (fnt_select == escsym[n])
			ifont = n;
	}
	switch(FontTypeq[ifont])
	{
		case TRUETYPE:
			*width = GetTTCharWidth(ifont, (unsigned char) (char_num - 32));
			break;
		case PSFILLED:
		case PSOUTLINE:
			*width = GetPSCharWidth(ifont, (unsigned char) (char_num - 32));
			break;
		case REGULARFONT:
			*width = g_fontq[ifont].char_dataq[(int)(char_num - 32)].width;
			break;
	}
}
/*********************************************************************/
/* Set default title height */
void set_titlhtq(void)
{
	G_PROCESS_ID

	if (set_title_default)	/* page() called but titlht() not */
		titlhtq = TITLE_SIZE * labscalq;
}
/*************************************************************************/
/* Returns the current active font selection character. Used to get the
active font so that it can be restored after a change. */

unsigned char get_current_fontq(void)
{
	G_PROCESS_ID

	return(escsym[ifontq]);
}

/*************************************************************************/
/* Provide a method to allow TT and PS fonts to return new char position */
void ReturnXbfYbf(float x, float y, float h, char p)
{
	G_PROCESS_ID

	xbf = x;
	ybf = y;
	hgtwdq = h;
	panfilq = p;
}

/*************************************************************************/
/* Provide a method to allow TT and PS fonts to return new char position */
void Get_angq(float *c, float *s)
{
	G_PROCESS_ID

	*s = sinq;
	*c = cosq;
}
/**************************************************************************/
void SetFontdata(int TTChar)
{
	G_PROCESS_ID

	gen_data.skew = skewq;
	gen_data.widen = widenq;
	gen_data.fill = fillq;
	gen_data.font_pattern = font_patternq;
	gen_data.font_border_color = font_border_colorq;
	gen_data.flag = flagq;
	gen_data.ang = angq;
	gen_data.scrlev = scrlevel;
	gen_data.tfont = tfontq;
	gen_data.ifont = ifontq;
	gen_data.fntht = fnthtq;
	gen_data.tsav = tsavfq;
	gen_data.bsav = bsavfq;
	gen_data.const_space = const_spaceq[ifontq];
	gen_data.height = heightq;
	gen_data.charspc = charspcq;
	gen_data.hgtwd = hgtwdq;
	gen_data.sin = sinq;
	gen_data.cos = cosq;
	gen_data.xb = xbf;
	gen_data.yb = ybf;
	gen_data.newht = (float)fnthtq;
	gen_data.skewfac = skewfacq;
	gen_data.widfac = widfacq;
	gen_data.TTCharIsUnicodeq = TTChar;
	gen_data.thick = thickq;
	gen_data.constwidth = g_fontq[ifontq].const_width;
}
/************************************************************/
void UnicodeSymbol(char ch)
{
	G_PROCESS_ID

	TTUnicodeSymbolq = ch;
}
/**************************************************************************/
void SetUseTTUnicode(int onoff)
{
	G_PROCESS_ID

	TTUnicodeq = onoff;
}

#ifdef MULTITHREAD
void FreePSData(int instctr);
void FreeTTData(int instctr);

void font_destroyq(int instctr)
{
	if (fnt_dataq[instctr] != NULL) {
		gpcfree((void **) &fnt_dataq[instctr]);
		gpcfree((void **) &GENERAL_dataq[instctr]);
		gpcfree((void **) &PS_INFO_dataq[instctr]);
		gpcfree((void **) &TT_Char_infoq[instctr]);

		FreePSData(instctr);
		FreeTTData(instctr);
	}
}
#endif
