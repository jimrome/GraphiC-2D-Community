/* Copyright 1984-1993 by Scientific Endeavors Corp
	All rights reserved.
Version 7.0 4-23-93
PostScript/Truetype extensions written by David Dillow.
***************************************************************/
#include <graphic.h>
#include <truetype.h>
#include <psfont.h>
#include <font_ext.h>

/**************************************************************/
/* FONT SELECTION ROUTINE                                     */
/* The first parameter is an integer that tells the number of */
/* fonts to be loaded. The minimum number is 1.               */
/* for each font, a pair of arguments is required:            */
/* the font name (char *), and its initializing character,    */
/* which is an integer due to default character promotion.    */
/**************************************************************/
void P_TYPE GCLASS font(int nfont, ...)
{
	int i, n, len, LoadedTable = 0;
	char *buffer, ext[5];
#ifdef MULTITHREAD
	static int n_fonts_loaded[256];
#else
	static int number_of_fonts_loaded;
#endif
	va_list mark;
	static int Font_save[FNTNUM];
	G_PROCESS_ID

		/* OBTAIN THE VARIABLE NUMBER OF ARGUMENTS */
	if (nfont >= 0) {	/* NEW FONT LIST */
		va_start(mark, nfont);	/* initialize va_argmacros */
		if (nfont > FNTNUM) {
			messageq(WARNQ, 32);	/* Too many fonts. Raise FNTNUM */
			nfont = FNTNUM;
		}
		for (i = 0; i < nfont; i++) {
			fntptr[i] = va_arg(mark, char *);
				/* parse pathname for extension */
			len = strlen(fntptr[i]) - 4;
			if (len <= 0)
				buffer = "invalid";
			else
				buffer = fntptr[i] + len;
			ext[0] = buffer[0];
			ext[1] = (char)toupper(buffer[1]);
			ext[2] = (char)toupper(buffer[2]);
			ext[3] = (char)toupper(buffer[3]);
			ext[4] = '\0';
			if (strcmp(ext,".PFB") == 0)
				Font_save[i] = PSFILLED;  /* is it Type 1? */
			else if (strcmp(ext, ".TTF") == 0)
				Font_save[i] = TRUETYPE; /* is it TrueType? */
			else if (strcmp(ext, ".FNT") == 0)
				Font_save[i] = REGULARFONT;  /* no */
			else {
				messageq(WARNQ, 46);
				continue;
			}
			escsym[i] = (Uchar)(va_arg(mark, int));
		}
		va_end(mark);	/* RESET va_arg MACROS */
	}

	else {	/* RELOAD THE PREVIOUS FONT LIST */
		if (number_of_fonts_loaded == 0)
			return;		/* No fonts to reload. */
		nfont = number_of_fonts_loaded;
	}

	freememq();
	NUM_OF_FONTS = nfont;

	for(n = 0; n < nfont; n++)
		FontTypeq[n] = Font_save[n];

	/* LOOP THROUGH EACH FONT; READ AND STORE DATA */
	for (n = 0; n < nfont; n++) {		/* OPEN FOR READ ONLY */
		switch(FontTypeq[n])
		{
			case REGULARFONT:
				if (!(load_fontsq(fntptr[n], &g_fontq[n]))) {
#ifdef GPC_COMMUNITY
					if (load_fontsq("Swiss.fnt", &g_fontq[n]) || load_fontsq("swiss.fnt", &g_fontq[n])) {
						fprintf(stderr, "[GraphiC Community] Note: Hershey stroke font '%s' is omitted from Community Edition. Using outline 'Swiss.fnt'.\n", fntptr[n]);
					} else
#endif
					if (load_fontsq("Simplex.fnt", &g_fontq[n]) || load_fontsq("simplex.fnt", &g_fontq[n])) {
						fprintf(stderr, "[GraphiC] Note: Font '%s' not found. Using 'Simplex.fnt'.\n", fntptr[n]);
					} else if (load_fontsq("Swiss.fnt", &g_fontq[n]) || load_fontsq("swiss.fnt", &g_fontq[n])) {
						fprintf(stderr, "[GraphiC] Note: Font '%s' not found. Using 'Swiss.fnt'.\n", fntptr[n]);
					} else {
						messageq(ERRQ, 39);	/* Could not load fonts */
						/* Safe dummy font initialization to prevent NULL dereference / SIGSEGV */
						g_fontq[n].fntht = 21;
						g_fontq[n].maxwidth = 16;
						g_fontq[n].const_width = 16;
						for (i = 0; i < 169; i++) {
							g_fontq[n].char_dataq[i].width = 16;
							g_fontq[n].char_dataq[i].length = 0;
							g_fontq[n].char_dataq[i].strokes = (char *)gpcalloc(2, sizeof(char));
							g_fontq[n].char_dataq[i].asc = 16;
							g_fontq[n].char_dataq[i].dsc = 5;
							g_fontq[n].char_dataq[i].lext = 0;
							g_fontq[n].char_dataq[i].rext = 0;
						}
					}
				}
				break;
			case TRUETYPE:
				switch(LoadTrueTypeFont(n, fntptr[n]))
				{
					case TTOK:
						g_fontq[n].fntht = PSHGT;
						LoadedTable = 1;
						break;
					case TTINVALIDFONT:
						messageq(TT_ERR, TT_ERRINVALID);
						messageq(ERRQ, 39);
						n--;
						NUM_OF_FONTS = --nfont;
						for(i = n + 1; i < nfont; i++)
						{
							fntptr[i] = fntptr[i+1];
							FontTypeq[i] = FontTypeq[i+1];
							escsym[i] = escsym[i+1];
						}
						LoadedTable = 1;
						break;
					case TTOUTOFMEMORY:
						messageq(TT_ERR, TT_ERRMEMORY);
						messageq(ERRQ, 39);
						n--;
						NUM_OF_FONTS = --nfont;
						for(i = n + 1; i < nfont; i++)
						{
							fntptr[i] = fntptr[i+1];
							FontTypeq[i] = FontTypeq[i+1];
							escsym[i] = escsym[i+1];
						}
						LoadedTable = 1;
						break;
					case TTBADNOMESSAGE:
						n--;
						NUM_OF_FONTS = --nfont;
						for(i = n + 1; i < nfont; i++)
						{
							fntptr[i] = fntptr[i+1];
							FontTypeq[i] = FontTypeq[i+1];
							escsym[i] = escsym[i+1];
						}
						break;
				}
				break;
			case PSFILLED:
			case PSOUTLINE:
				if(!LoadPSFont(n, fntptr[n])) {
					messageq(ERRQ, 39);
					n--;
					NUM_OF_FONTS = --nfont;
					for(i = n; i < nfont; i++) {
						fntptr[i] = fntptr[i+1];
						FontTypeq[i] = FontTypeq[i+1];
						escsym[i] = escsym[i+1];
					}
				}
				else
					g_fontq[n].fntht = PSHGT;
				break;
		}
	}
	number_of_fonts_loaded = NUM_OF_FONTS;

	for(n = i = 0; n < nfont; n++)
		i += ((FontTypeq[n] == TRUETYPE) ? (1) : (0));

	if(LoadedTable && !i)
		UnLoadTranslateTable();
	fntchg(escsym[0]);
}

/************************************************************/
void GCLASS freememq(void)
{
	int n, i;
	G_PROCESS_ID

	if (NUM_OF_FONTS == 0)
		return;

	/* RELEASE FONT MEMORY */
	for (n = 0; n < NUM_OF_FONTS; n++) {
		switch(FontTypeq[n]) {
			case REGULARFONT:
				for (i = 0; i < 168; i++)
					gpcfree((void **) &g_fontq[n].char_dataq[i].strokes);
				break;
			case TRUETYPE:
				FreeTrueTypeMemory(n);
				break;
			case PSFILLED:
			case PSOUTLINE:
				FreePSMemory(n);
				break;
        }
	}
	NUM_OF_FONTS = 0;
}
/********************************************************************/

#define FNTMAX 500
int GCLASS load_fontsq(char *filename, struct G_FONT *font_info)
{
	int temp1 = 0, temp2 = 0;
	int length[168];
	unsigned char rbuf[3];
	FILE *fontfile;
	int i;
	int err;

	fontfile = gfopen(filename, "rb", 0);
	if (fontfile == NULL)			/* Error opening font file */
		return(0);

	/* READ DATA INTO DATA ARRAYS -  THE DATA IN EACH FILE ARE
	ARRANGED AS FOLLOWS: FIRST LINE IS THE FONT HEIGHT,
	THEN ON THE OTHER LINES, WE HAVE THE INFORMATION -
	CHARACTER, WIDTH, STROKE TABLE, DESCENDER, ASCENDER, LEFT
	EXTENSION, RIGHT EXTENSION. ALL QUANTITIES ARE POSITIVE. */

	/* START WITH THE POINTER TABLE */
	err = fread(rbuf, 1, 2, fontfile);
	if (err < 2)
		messageq(WARNQ, 24); /* Can't read pointer table */
	temp2 = (int)((((UCHAR)rbuf[1]) << 8) + rbuf[0]);
	for (i = 0; i < 168; i++) {
		err = fread(rbuf, 1, 2, fontfile);
		if (err < 2)
			messageq(WARNQ, 24); /* Can't read pointer table */
		temp1 = (int)((((UCHAR)rbuf[1]) << 8) + rbuf[0]);
		length[i] = temp1 - temp2;
		font_info -> char_dataq[i].length = length[i];
		font_info -> char_dataq[i].strokes =
			(char *)gpcalloc(length[i], sizeof(char));
		if (font_info -> char_dataq[i].strokes == NULL) {
			while (i >= 0) {
				gpcfree((void **) &font_info -> char_dataq[i].strokes);
				i--;
			}
			messageq(ERRQ, 59);
			return(0);
		}
		temp2 = temp1;
	}

	/* AND THEN THE HEIGHT OF THE FONT */
	err = fread(rbuf, 1, 1, fontfile);
	if (err < 1)
		messageq(ERRQ, 13);				/* Error reading height */
	font_info -> fntht = *rbuf;

	/* LOOP THRU FONT TABLES AND PLACE DATA IN CORRECT PLACES */
	for (i = 0; i < 168; i++) {

				/* DUMP CHAR '|'  AND IGNORE CHARACTER INFORMATION */
		err = fread(rbuf, 1, 3, fontfile);
		if (err < 1)
			messageq(ERRQ, 14);	/* Error reading character */

					/* GET WIDTH */
		err = fread(rbuf, 1, 2, fontfile);
		if (err < 2)
			messageq(ERRQ, 15);	/* Error reading width */
		font_info -> char_dataq[i].width = rbuf[0];		/* width in pels */
					/* STORE THE MAXIMUM CHARACTER WIDTH */
		if (rbuf[0] > (unsigned char)font_info -> maxwidth)
			font_info -> maxwidth = rbuf[0];

					/* GET STROKE TABLE */
		if (length[i] > FNTMAX)
			messageq(ERRQ, 16);	/* Max length is 500 bytes */
		err = fread(font_info -> char_dataq[i].strokes, 1,
			length[i], fontfile);	/*DATA INTO STRBUFF*/
		if (err < length[i])
			messageq(ERRQ, 18);	/* Error reading stroke table */

					/* GET DESCENDER */
		err = fread(rbuf, 1, 3, fontfile);
		if (err < 3)
			messageq(ERRQ, 19);	/* Error reading descender */
		font_info -> char_dataq[i].dsc = rbuf[1];

					/* GET ASCENDER */
		err = fread(rbuf, 1, 2, fontfile);
		if (err < 2)
			messageq(ERRQ, 20);	/* Error reading ascender */
		font_info -> char_dataq[i].asc = rbuf[0];

					/* GET LEFT EXTENDER */
		err = fread(rbuf, 1, 2, fontfile);
		if (err < 2)
			messageq(ERRQ, 21);	/* Error reading left extender */
		font_info -> char_dataq[i].lext = rbuf[0];
		/* GET RIGHT EXTENDER */
		err = fread(rbuf, 1, 1, fontfile);
		font_info -> char_dataq[i].rext = rbuf[0];
	}
	fclose(fontfile);
	return(1);
}
