/* (c) 1984-1993 by Scientific Endeavors Corporation.
   All rights reserved.
Version 7.0 PostScript/Truetype extensions written by David Dillow.
**************************************************************/
#define __PSFONT

#include <graphic.h>
#include <psfont.h>
#include <font_ext.h>
#include <stdarg.h>

void ReturnXbfYbf(float x, float y, float h, char p);
void Get_angq(float *c, float *s);

void newangq(void);

#ifdef MULTITHREAD
struct PS_INFO *PS_INFO_dataq[GPC_MAX_THREADS];
#else
struct PS_INFO PS_INFO_dataq;
#endif

/**************************************************************************/
/* This routine simply allocates memory for the shared data in OS/2       */
void SetPSforOS2(void)
{
#ifdef MULTITHREAD
	G_PROCESS_ID

	PS_LOAD_dataq[instctr] = (struct PS_LOAD_DATA *)gpcalloc(1,
		sizeof(struct PS_LOAD_DATA));
#endif
}

#ifdef MULTITHREAD
/**************************************************************************/
/* Free shared memory */
void FreePSData(int instctr)
{
	gpcfree((void **) &PS_LOAD_dataq[instctr]);
}
#endif

/********************************************************************/
void FreePSMemory(int n)
{
	G_PROCESS_ID

	if (FontName[n] != NULL)
		gpcfree((void **) &FontName[n]);

	gpcfree((void **) &charstringsbaseq[n]);
	gpcfree((void **) &charstringsq[n]);
	gpcfree((void **) &PSsubrsbaseq[n]);
	gpcfree((void **) &PSsubrsq[n]);
	if (PS_Kernq[n])
		gpcfree((void **) &PS_KernPairsq[n]);
}

/********************************************************************/
int LoadPSFont(int n, char *filename)
{
	char buffer[150];
	FILE *fontfile;
	char inputbuf[256], term;
	int index, var_flag = 0, error = 0;
	/* flags to make sure everything read only once */
	int gotEncoding = 0, SubrsRead = 0, CharsRead = 0;
	int gotKerned = 0;
	int gotNameAlready = 0;

	G_PROCESS_ID

	fontfile = gfopen(filename, "rb", 0);
	if (fontfile == NULL)		/* error opening fontfile */
		return 0;
        
	fread(inputbuf, 6, 1, fontfile); /* skip header */

	fread(inputbuf, sizeof(char), 2, fontfile); /* get id */

	inputbuf[2] = 0;

	if (strcmp(inputbuf, "%!") != 0) {	/* is it PostScript? */
		fclose(fontfile);
		sprintf(buffer, "Filename: %s", filename);
		gprintq(buffer, 0);
		messageq(PS_ERR, PS_NOTPSFONT);
		return 0;
	}

	eexecq = 0;		/* reset flags and counters */
	gotEncoding = 0;
	eexec_countq = 256;
	eexec_keyq = 55665U;
	lenIV = 4;

	while (eexec_countq) {	/* till end of data */
		if (error || feof((FILE *) fontfile)) {
			eexec_countq = 0;
			sprintf(buffer, "Filename: %s", filename);
			gprintq(buffer, 0);
			messageq(PS_ERR, PS_INVALIDFONT);
			error = 1;
		}

		if (error)
			break;

		get_nextq(fontfile,inputbuf,&term);

		if (strcmp(inputbuf,"eexec") == 0) {
			/* entering encrypted area */

			while (!eexecq) {
				if (fread(inputbuf, 1,1,fontfile) != 1) {
				/* out of data! */
					error = 1;
					sprintf(buffer, "Filename: %s", filename);
					gprintq(buffer, 0);
					messageq(PS_ERR, PS_INVALIDFONT);
					break;
				}

				/* find marker (two 0 bytes) */
				if (inputbuf[0] == 0) {
					if (fread(inputbuf, 1,1,fontfile) != 1) {
						sprintf(buffer, "Filename: %s", filename);
						gprintq(buffer, 0);
						messageq(PS_ERR, PS_INVALIDFONT);
						error = 1;
						break;
					}
					if (inputbuf[0] == 0)
						eexecq = 1;
				}
			}

			for (index = 0; index < 4; index++) {
				/*read 4 random byte header */
				if (fread(inputbuf, sizeof(char), 1, fontfile) != 1) {
					error = 1;
					break;
				}
				decryptq(inputbuf[0], &eexec_keyq);
			}
			continue;
		}

		if (var_flag) {
			switch(findkeyq(inputbuf)) {	/* action based on keyword */
				case 1:
					if (!gotEncoding) {	/* get encoding vector */
						error = getEncodingq(inputbuf, fontfile);
						if (!error)
							getAFMinfoq(n, filename);
					}
					gotEncoding = 1;
					var_flag = 0;
					break;
				case 2:
					error = get_nextq(fontfile, inputbuf, &term);

					if (inputbuf[0] != '1' && inputbuf[0] != 'g') {
						/* check fonttype */
						sprintf(buffer, "Filename: %s", filename);
						gprintq(buffer,0);
						messageq(PS_ERR, PS_INVALIDFONT);
						error = 1;
						break;
					}
					var_flag = 0;
					break;
				case 3:	/* get scaling matrix */
					getFontMatrixq(inputbuf, fontfile, n);
					var_flag = 0;
					break;
				case 4:	/* get main instruction lists */
					if (CharsRead) break;
					error = getCharStringsq(inputbuf, fontfile, n);
					CharsRead = 1;
					var_flag = 0;
					break;
				case 5: /* get subroutines */
					if (SubrsRead) break;
					error = getSubrsq(inputbuf, fontfile, n);
					SubrsRead = 1;
					var_flag = 0;
					break;
				case 6:	/* get header length */
					error = get_nextq(fontfile, inputbuf, &term);

					sscanf(inputbuf, "%i", &lenIV);
					var_flag = 0;
					break;
				case 7:	/* check if filled */
					error = get_nextq(fontfile, inputbuf, &term);
					if (inputbuf[0] == '2')
						FontTypeq[n] = PSOUTLINE;
					var_flag = 0;
					break;
				case 8:	/* is it a kerned font? */
					if (gotKerned)
						break;
					error = get_nextq(fontfile, inputbuf, &term);
					if (inputbuf[0] == 'f')
						PS_Kernq[n] = 0;
					else
						PS_Kernq[n] = 1;
					gotKerned = 1;
					var_flag = 0;
					break;
				case 9:	/* Multiple Master Fonts are not supported */
					messageq(PS_ERR, PS_ERRORMM);
					error = 1;
					break;
				case 10:
					{
                     	char namebuf[128];
						namebuf[0] = '\0';

						if(gotNameAlready)
							break;
						else
							gotNameAlready = 1;

						error = get_nextq(fontfile, inputbuf, &term);
						error = get_nextq(fontfile, inputbuf, &term);

						while(strcmp(inputbuf, "readonly") != 0 && !error)
						{
							if(namebuf[0] != '\0')
								strcat(namebuf, " ");

							strcat(namebuf, inputbuf);

							error = get_nextq(fontfile, inputbuf, &term);
						}

						FontName[n] = (char *) gpcalloc(strlen(namebuf) + 1, 1);

						if(FontName[n] != NULL)
							strcpy(FontName[n], namebuf);
					}
					break;
				default:
					var_flag = 0; /* ignore keyword */
					break;
			}
		}

		if (term == '/') {	/* keyword is next */
			var_flag = 1;
			continue;
		}
	}

	if (!error)
		calcFudgeq(n);

	fclose(fontfile); /* close file */

	if (error)
		return 0;

	return 1;	/* go to next font */
}
/**************************************************************************/
/* getAFMinfoq() gets the Cap height and kerning info from a font's AFM */
/* file. If there is no file, kerning is not done. */
void getAFMinfoq(int fontnumber, char *name)
{
	FILE *afmfile;
	int index, number, char1, char2, offset;
	char buffer[256];
	char filename[128];

	G_PROCESS_ID

	PS_Kernq[fontnumber] = 0;	/* zero flags */
	workingCap = 0;

	strcpy(filename, name);		/* build filename */
	index = 0;

	while (filename[index] != 0 && filename[index] != '.')
		index++;

	strcpy(&filename[index], ".afm");

	afmfile = gfopen(filename, "r", 0);
	if (afmfile == NULL) {
		sprintf(buffer, "Filename: %s", filename);
		gprintq(buffer, 0);
		messageq(PS_WARN, PS_WARNNOAFM);
		return;
	}

	while (!feof(afmfile)) {
		fscanf(afmfile, "%s", buffer);

		if (strcmp(buffer, "StartKernPairs") == 0) {
			/* look for beginning */
			PS_Kernq[fontnumber] = 1;
			break;
		}

		if (strcmp(buffer, "CapHeight") == 0)	/* get height of font */
			fscanf(afmfile, "%i", &workingCap);
	}

	/* did we find data or are not supposed to get it? */
	if (!PS_Kernq[fontnumber]) {
		PS_Kernq[fontnumber] = 0;	/* no data loaded */
		fclose(afmfile);
		return;
	}

	fscanf(afmfile, "%i", &number);	/* get number of kern pairs */

	PS_KernPairsq[fontnumber] =
		(struct kernpair *) GPC_ALLOC(&hkernpairsq[fontnumber],
				number + 1, sizeof(struct kernpair));

	if (PS_KernPairsq[fontnumber] == NULL) {
		fclose(afmfile);		/* no room for kern data */
		PS_Kernq[fontnumber] = 0;
		messageq(PS_WARN, PS_WARNKERNMEM);
		return;
	}

	for (index = 0; index < number; index++) {
		fscanf(afmfile, "%s", buffer);

		if (strcmp(buffer, "EndKernPairs") == 0) {	/* did we hit the end? */
			number = index;
			break;
		}

		if (strcmp(buffer, "KPX") != 0) {
			index--;	/* skip this one. */
			continue;
		}

		fscanf(afmfile, "%s", buffer);	/* get first character name */

		char1 = find_encodingq(buffer);	/* find its number */
		if (char1-- == 0) {	/* is it known? */
			index--;	/* nope, reduce list. */
			number--;
			continue;
		}

		fscanf(afmfile, "%s", buffer);	/* get 2nd char */

		char2 = find_encodingq(buffer);
		if (char2-- == 0) {		/* is it known? */
			index--;	/* nope, reduce list */
			number--;
			continue;
		}

		fscanf(afmfile, "%i", &offset);	/* read in offset */

		PS_KernPairsq[fontnumber][index].char1 = char1;	/* store kern info */
		PS_KernPairsq[fontnumber][index].char2 = char2;
		PS_KernPairsq[fontnumber][index].offset = offset;
	}

	fclose(afmfile);

	/* compress list to save space */
	PS_KernPairsq[fontnumber] = (struct kernpair *)
			GPC_REALLOC(&hkernpairsq[fontnumber], PS_KernPairsq[fontnumber],
			((number + 1) * sizeof(struct kernpair)));

	if (PS_KernPairsq[fontnumber] == NULL) {
		PS_Kernq[fontnumber] = 0;
		return;
	}

	PS_KernPairsq[fontnumber][number].char1 = -1;	/* set up end */
	PS_KernPairsq[fontnumber][number].char2 = 0;		/*  of list marker */
	PS_KernPairsq[fontnumber][number].offset = 0;

	return;
}

/**************************************************************************/
/* find_encodingq() searches the encoding vector for the name in buffer */
/*  and returns its number in the array + 1, 0 means that it was not there */
int find_encodingq(char *buffer)
{
	int index = 0, answer = 0;

	G_PROCESS_ID

	if (strcmp(buffer,"") == 0)		/* Check for null string */
		return 0;

	for (index = 0; index < 223; index++) {
		if (encoding[index] == NULL)
			continue;

		if (G_STRCMP((char *) buffer, (char *)encoding[index]) == 0) {						/* search encoding vector for name */
			answer = index + 1;	/* return number + 1 */
			break;
		}
	}

	return answer;	/* 0 == could not find it */
}

/**************************************************************************/
/* getEncodingq() reads the encoding vector in from font file, or from */
/* STDENCOD.POS if the font has standard encoding. */
int getEncodingq(char *buffer, FILE *file)
{
	int index, number, temp, pass;
	unsigned long place, amount = 0;
	char term;
	FILE *encodingf;

	G_PROCESS_ID

	get_nextq(file, buffer, &term);	    /* see if its Standard Encoding */

	if (strcmp(buffer,"StandardEncoding") == 0) {
		encodingf = gfopen("stdencod.pos", "r", 0);
		if (encodingf == NULL) {
			messageq(PS_ERR, PS_NOSTDENC);
			return 1;	/* 1 indicates error */
		}

		place = ftell(encodingf);	/* save place */
		amount = 0;
		number = 0;
		temp = 0;

		while (!feof(encodingf)) {	/* get amount of mem needed */
			fscanf(encodingf, "%i%s", &temp, buffer);
			amount += (strlen(buffer) + 1);
			number++;
		}

		/* allocate space for pointers to the data */
		encoding = (char * *)
			GPC_ALLOC(&h1encoding, 224, sizeof(char * *));

		if (encoding == NULL) {
			fclose(encodingf);
			messageq(PS_ERR, PS_NOENCMEM);
			return 1;
		}

		/* allocate data space */
		encodingbase = (char *)
			GPC_ALLOC(&h2encoding, (unsigned int)amount + 1, 1);

		if (encodingbase == NULL) {
			fclose(encodingf);
			messageq(PS_ERR, PS_NOENCMEM);
			return 1;
		}

		fseek(encodingf, place, SEEK_SET);	/* restore place */
		amount = 0;

		for (index = 0; index < number; index++) {
			/* get position, name */
			fscanf(encodingf, "%i%s", &temp, buffer);

			amount += (strlen(buffer) + 1);	/* calculate pointer to name */
			temp -= 32;

			/* move name to data space */
			encoding[temp] = encodingbase + (unsigned int) amount - (strlen(buffer) + 1);
			G_STRCPY(encoding[temp], (char *) buffer);
		}

		fclose(encodingf);

		return 0;			/* We're done here for standard encoding*/
	}

	sscanf(buffer,"%i", &number); /* get number of vectors */

	/* allocate space for pointers */
	encoding = (char * *)
		GPC_ALLOC(&h1encoding, 224, sizeof(char * *));

	if (encoding == NULL) {
		messageq(PS_ERR, PS_NOENCMEM);
		return 1;
	}

	place = ftell(file);	/* save spot in file */
	pass = 2; 	/* make two passes */

	while (pass) {
		fseek(file, place, SEEK_SET);	/* restore spot */

		if (pass == 1) {	/* storage pass? */
			/* yes- allocate data space */
			encodingbase = (char *)
				GPC_ALLOC(&h2encoding, (unsigned int)amount + 1, 1);

			if (encodingbase == NULL) {
				messageq(PS_ERR, PS_NOENCMEM);
				return 1;
			}
		}

		amount = 0;
		buffer[0] = 0;

		for (index = 0; index < number; index++) {
			if (strcmp(buffer, "readonly") == 0 || strcmp(buffer, "def") == 0)
				break;	/* end of vectors? */

			while (strcmp(buffer, "dup") != 0) /* find start of nect vector */
				get_nextq(file, buffer, &term);

			get_nextq(file, buffer, &term);	/* get number of vector */

			sscanf(buffer, "%i", &temp);

			temp -= 32;   /* convert it to a pointer into encoding array */

			if (temp < 0 || temp > 223) {	/* range check encoding */
				for (temp = 0; temp < 4; temp++) {
					/* out of range, skip to next */
					get_nextq(file, buffer, &term);
				}
				continue;
			}

			get_nextq(file, buffer, &term);	/* skip '/' terminator */
			get_nextq(file, buffer, &term);

			amount += (strlen(buffer) + 1); /* add storage space required */

			if (pass == 1) {	/* storage pass? */
				encoding[temp] = encodingbase + (unsigned int) amount - (strlen(buffer) + 1);
				G_STRCPY((char *) encoding[temp], (char *) buffer); /* store it */
			}

			get_nextq(file, buffer, &term); /* skip end of def */
			get_nextq(file, buffer, &term);
		}

		pass--;	/* next pass */
	}

	return 0;
}

/**************************************************************************/
/* getFontMatrixq() loads the transformation matrix for the font */
void getFontMatrixq(char *buffer, FILE *file, int fontnumber)
{
	char term;
	int index = 0;

	G_PROCESS_ID

	get_nextq(file, buffer, &term);

	while (term != ']') {	/* read in transform matrix */
		get_nextq(file, buffer, &term);
		sscanf(buffer, "%f", &FontMatrix[fontnumber][index]);
		index++;
	}
	return;
}

/**************************************************************************/
/* calcFudgeq() calculates the scaling needed to make PS fonts print at */
/* maeasured size */
void calcFudgeq(int n)
{
	int top = 0, bottom = 0;
	float wid = 0.f;

	G_PROCESS_ID

	fnthtq = PSHGT;

	ifontq = n;

	FontFudge[n] = 1;

	if (workingCap)
		top = workingCap;
	else
	{
		heightq = 1 / FontMatrix[n][3];
		heightq /= fnthtq;

		xbf = ybf = 0;

		handlePSFontq(-10, ('X' - 32), 'c', &wid, &top, &bottom);
	}

	if (top != 0)
		FontFudge[n] = (1 / FontMatrix[n][3]) / top;
}

/**************************************************************************/
/* getCharstringsq() reads in the character drawing instructions */
int getCharStringsq(char *buffer, FILE *file, int fontnumber)
{
	int temp, bytes, nostore, loop = 1;
	unsigned short old_eexec;
	long place, amount = 0;
	char term;
	char *charbuffer;

	G_PROCESS_ID

	get_nextq(file, buffer, &term); /* skip next */

	charstringsq[fontnumber] = (Uchar * *)
		GPC_ALLOC(&h1charstrq[fontnumber], 225, sizeof(Uchar *));

	if (charstringsq[fontnumber] == NULL)
	{
		messageq(PS_ERR, PS_NOMEMORY);
		return 1;
	}

	place = ftell(file);	/* save our place */
	old_eexec = eexec_keyq;

	for (;;)	/* until we've read all charstrings */
	{
		nostore = 0;	/* default to store character */

		/* get to next def or end of defs */
		while (term != '/' && strcmp(buffer, "end") != 0)
			get_nextq(file, buffer, &term);

		if (strcmp(buffer, "end") == 0)
		{
			if (loop)    /* after first time thru, alloc memory */
			{
				fseek(file, place, SEEK_SET);	/*restore our place */
				loop = 0;
				eexec_keyq = old_eexec;
				charstringsbaseq[fontnumber] = (Uchar *)
					GPC_ALLOC(&h2charstrq[fontnumber], (unsigned int)amount + 1, 1);
				amount = 0;
				if (charstringsbaseq[fontnumber] == NULL)
				{
					messageq(PS_ERR, PS_NOMEMORY);
					return 1;
				}
				buffer[0] = 0;
				continue;
			}
			else
				break; /* no more */
		}

		get_nextq(file, buffer, &term); /* get name */

		temp = find_encodingq(buffer); /* get number */

		if (temp == 0)
		{
			if (strcmp(buffer,".notdef") != 0)   /* is it .notdef? */
				nostore = 1;	/* no- go on to next */
			temp = 225; /* yes, store at last spot */
		}

		temp--;	/* compensate for "not found error" from
					find_encodingq */

		get_nextq(file, buffer, &term);  /* get number of bytes */

		sscanf(buffer, "%i", &bytes);
		if (!nostore)
			amount += (bytes - lenIV); /* calc storage requirement */

		get_nextq(file, buffer, &term);  /* skip RD */

		/* allocate a buffer for reading */
		charbuffer = (char *) gpcalloc(1, bytes);
		if (charbuffer == NULL)
		{
			messageq(PS_ERR, PS_NOMEMORY);
			return 1;
		}

		get_bytesq(file, charbuffer, bytes); /* read in instructions */

		if (!loop && !nostore)	/* if on storage pass & we are to save char */
		{
			charstringsq[fontnumber][temp] = charstringsbaseq[fontnumber] +
								(unsigned int) amount - (bytes - lenIV);

			/* move charstring to final place, skipping random number header */
			G_MEMCPY(charstringsq[fontnumber][temp],
						(Uchar *) &charbuffer[lenIV], (bytes - lenIV));
		}

		gpcfree((void * *) &charbuffer); /* free buffer */
	}

	GPC_FREE(&h2encoding, &(encodingbase));
	GPC_FREE(&h1encoding, &encoding);

	return 0;
}

/**************************************************************************/
/* getSubrsq() reads in subrs for font */
int getSubrsq(char *buffer, FILE *file, int fontnumber)
{
	int index, number, temp, bytes, loop = 2;
	long amount, place;
	unsigned short old_eexec;
	char term;
	char *subrbuffer;

	G_PROCESS_ID

	get_nextq(file, buffer, &term); /*get number of routines*/

	sscanf(buffer, "%i", &number);

	/* allocate space for pointers to routines */
	PSsubrsq[fontnumber] = (Uchar * *)
		GPC_ALLOC(&h1PSsubrsq[fontnumber], number, sizeof(Uchar *));

	if (PSsubrsq[fontnumber] == NULL)
	{
		messageq(PS_ERR, PS_NOMEMORY);
		return 1;
	}

	place = ftell(file);	/* save place */
	old_eexec = eexec_keyq;

	while (loop)
	{
		fseek(file, place, SEEK_SET);	/* restore spot */
		eexec_keyq = old_eexec;
		amount = 0;

		for (index = 0; index < number; index++)
		{
			while (strcmp(buffer,"dup") != 0)  /* skip to next subr */
				get_nextq(file, buffer, &term);

			get_nextq(file, buffer, &term); /* get subr number */

			sscanf(buffer, "%i", &temp);

			get_nextq(file, buffer, &term); /* get length of subr */

			sscanf(buffer, "%i", &bytes);
			amount += (bytes - lenIV);	/* calc mem needed */

			get_nextq(file, buffer, &term);  /* skip RD */

			/* allocate read buffer */
			subrbuffer = (char *)
				gpcalloc((size_t) bytes,1);

			if (subrbuffer == NULL)
			{
				messageq(PS_ERR, PS_NOMEMORY);
				return 1;
			}

			get_bytesq(file, subrbuffer, bytes); /* read in subroutine */

			if (loop == 1)	/* is storage pass */
			{
				PSsubrsq[fontnumber][temp] = PSsubrsbaseq[fontnumber] + (unsigned int) amount -
												(bytes - lenIV);

				/* move subr to final place, skip header */
				G_MEMCPY(PSsubrsq[fontnumber][temp],
							(Uchar *) &subrbuffer[lenIV], (bytes - lenIV));
			}

			gpcfree((void * *) &subrbuffer);	/* free read buffer */
		}

		loop--;	/* next pass */
		if (loop) /* on storage pass now */
		{
			PSsubrsbaseq[fontnumber] = (Uchar *)
				GPC_ALLOC(&h2PSsubrsq[fontnumber], (unsigned int)amount, 1);

			if (PSsubrsbaseq[fontnumber] == NULL)
			{
				messageq(PS_ERR, PS_NOMEMORY);
				return 1;
			}
		}
	}

	return 0;
}

/**************************************************************************/
/* findkeq() returns a number for the important keywords */
int findkeyq(char *string)
{
	/* simply return a number based on name */

	if (strcmp(string,"Encoding") == 0) return 1;
	if (strcmp(string,"FontType") == 0) return 2;
	if (strcmp(string,"FontMatrix") == 0) return 3;
	if (strcmp(string,"CharStrings") == 0) return 4;
	if (strcmp(string,"Subrs") == 0) return 5;
	if (strcmp(string,"lenIV") == 0) return 6;
	if (strcmp(string,"PaintType") == 0) return 7;
	if (strcmp(string,"isFixedPitch") == 0) return 8;
	if (strcmp(string,"$Blend") == 0) return 9;
	if (strcmp(string,"FullName") == 0) return 10;

	/* 0 indicates not found */
	return 0;
}

/**************************************************************************/
/* decryptq() decrypts the encoding of the font */
unsigned char decryptq(unsigned char cipher, unsigned short int *r)
{
	unsigned char plain;

	plain = (char)(cipher ^ (unsigned char)(*r >> 8));
	*r = (unsigned short)((cipher + *r) * 52845U + 22719);
	return plain;
}

/**************************************************************************/
/* get_nextq() returns the next token in the file, and its terminator */
int get_nextq(FILE *file, char *buffer, char *terminator)
{
	int loop = 1, index = 0;
	static char lastread;

	G_PROCESS_ID

	/*get next token from fontfile */

	while (loop && eexec_countq)  /* while there is data */
	{
		if (fread(&buffer[index],sizeof(char),1,file) != 1)  /* get a char */
		{
			buffer[0] = 0;		/*EOF- zero buffer */
			terminator = 0;		/*let it fall thru to font()*/
			return 1;			/*font() will catch it at next loop */
		}

		if (buffer[index] == '0' && lastread == '0') {
				eexec_countq--; /* 512  '0's at EOF */
		}
		lastread = buffer[index];

		if (eexecq) /* do we need to decryptq? */
		{
			buffer[index] = decryptq(buffer[index], &eexec_keyq);
		}

		switch(buffer[index])
		{
			case ' ':	/* whitespace */
			case '\t':
			case '\r':
			case '\n':
				if (index) /* is it leading? */
				{
					*terminator = buffer[index];
					buffer[index] = 0;      /* no */
					loop = 0;
				}
				break; /* ignore leading whitespace */
			case '(':
			case ')':
			case '<': /* terminators */
			case '>':
			case '[':
			case ']':
			case '{':
			case '}':
			case '/':
			case '%':
				/* store it and return */
				*terminator = buffer[index];
				buffer[index] = 0;
				loop = 0;
				break;
			default:
				index++; /* regular character, just store it */
		}
	}

	return 0;
}

/**************************************************************************/
/* get_bytesq() reads in a binary string and decrypts them */
int get_bytesq(FILE *file, char *buffer, int number)
{
	int index;
	unsigned short int charstring_key = 4330;

	G_PROCESS_ID

	if (fread(buffer, 1, number, file) != (size_t)number)   /* read bytes */
	{
		fclose(file);
		messageq(PS_ERR, PS_INVALIDFONT);
		return 1;
	}

	for (index = 0; index < number; index++)
	{
		buffer[index] = decryptq(buffer[index], &eexec_keyq);
		buffer[index] = decryptq(buffer[index], &charstring_key);
	}

	return 0;
}

/* These macros simplify code. They simply transform the point x?, y?
 * by the matrix in Matrix. */
#define tran_x(z) tx##z = ((x##z * Matrix[0]) + (y##z * Matrix[2])) + Matrix[4] + 0.51f
#define tran_y(z) y##z = ((x##z * Matrix[1]) + (y##z * Matrix[3])) + Matrix[5] + 0.51f; \
				x##z = tx##z

/**************************************************************************/
/* This is the main handler for PostScript fonts. It is called from
 * strsizq(). It checks for kerning and adjusts position accordingly.
 * Calculating the width, top, and bottom take the same steps as
 * ploting the character, with the exception of drawing.
 * Accented characters require a larger stack, as they are formed by
 * recursive calls to this routine. */
void handlePSFontq(int lastchar, int n, int action, float *wid,
				 int *top, int *bottom)
{
	float x1, x2, x3, x4, y1, y2, y3, y4, tx1, tx2;
	float Flex_x[8], Flex_y[8]; /*Flex point storage */

	float current_x = 0.f, current_y = 0.f, old_x = 0.f, old_y = 0.f, our_hgt; /* counters */
	Uchar *current_charstr;			/* instruction pointer */
	float operands[24], other_ops[24], width = 0; /* stacks */
	Uchar *subrs[11];
	int op_ptr, oth_ptr, subrs_ptr,char_ptr,value; /* stack pointers */
	int subrs_index[11], Flex_ptr = 0, Flex_hgt, command;
	int to_be_filled = 0, index; /* flags */
	float tmatrix[6], t2matrix[6];	/* temp matricies */
	float xbfsav, ybfsav, kernx;
	int temp = 0;
	char newseg = 1;

	G_PROCESS_ID

	Flex_x[0] = 0.f;
	Flex_y[0] = 0.f;

	if (PS_Kernq[ifontq])	/* do kerning? */
		findkernpair(lastchar, n, &kernx);	/* find kern offset */
	else
		kernx = 0.0f;

	if (n < 0)	/* n must be positive! */
		return;

	/* convert height to pixels */
	our_hgt = fnthtq * heightq *
		((PSfullq[ifontq]) ?  (FontFudge[ifontq]) : (1));

	FindBBox = 0;
	newseg = 1;

	if (action == 'p') {
		if (fillq && FontTypeq[ifontq] != PSOUTLINE) {
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
	}

	if (thickq)
		tline(tfontq);

	/* set up transformation matrix */

	for (index = 0; index < 6; index++)
		tmatrix[index] = FontMatrix[ifontq][index];

	t2matrix[0] = t2matrix[3] = our_hgt;
	t2matrix[1] = t2matrix[2] = 0;
	t2matrix[4] = xbf;
	t2matrix[5] = ybf;

	if (skewq)
		t2matrix[2] = skewfacq * our_hgt;
	if (widenq)
		t2matrix[0] *= widfacq;

	multiplymatrixq(tmatrix, t2matrix);

	current_charstr = charstringsq[ifontq][n];	/* init instruction ptr */

	if (current_charstr == NULL) {	/* if no character here, use .notdef */
		current_charstr = charstringsq[ifontq][224];

		if (current_charstr == NULL) {
			if (g_pq.xyp != NULL)
				gpcfree((void **) &g_pq.xyp);
			if (g_lq.xyp != NULL)
				gpcfree((void **) &g_lq.xyp);

			panfilq = 0;
			return;
		}
	}

	op_ptr = oth_ptr = subrs_ptr = char_ptr = 0;  /* zero ptrs */

	for (;;) {
		command = getTypeval(operands, &op_ptr,	current_charstr,
			&char_ptr, &value); /* get next instruction */

		if (command) {	/* value is a command */
			/* handle it */
			switch(value) {
				case 1:         /* ignore hints */
					/* hstem */
				case 3:
					/* vstem */
					op_ptr = 0;
					break;
				case 4:
					/* vmoveto */
					current_y += operands[0];
					op_ptr = 0;
					if (newseg) {
						old_x = current_x;	/* save x and y for closepath */
						old_y = current_y;
						newseg = 0;
					}
					x1 = current_x;
					y1 = current_y;
					tran_x(1);	/* translate point */
					tran_y(1);
					lmoveq((int) x1, (int) y1); /* go there */
					break;
				case 5:
					/* rlineto */
					x1 = (current_x += operands[0]);
					y1 = (current_y += operands[1]);
					op_ptr = 0;
					PSdrawq(x1, y1);
					break;
				case 6:
					/* hlineto */
					x1 = (current_x += operands[0]);
					y1 = current_y;
					op_ptr = 0;
					PSdrawq(x1, y1);
					break;
				case 7:
					/* vlineto */
					x1 = current_x;
					y1 = (current_y += operands[0]);
					op_ptr = 0;
					PSdrawq(x1, y1);
					break;
				case 8:
					/* rrcurveto */
					x1 = current_x;
					x2 = x1 + operands[0];
					x3 = x2 + operands[2];
					x4 = x3 + operands[4];
					current_x = x4;
					y1 = current_y;
					y2 = y1 + operands[1];
					y3 = y2 + operands[3];
					y4 = y3 + operands[5];
					current_y = y4;
					op_ptr = 0;
					draw_bezierq(x1,y1,x2,y2,x3,y3,x4,y4);
					break;
				case 9:
					/* closepath */
					newseg = 1;   /* reset flags */
					op_ptr = 0;
					if (current_x != old_x || current_y != old_y) {
						x1 = old_x;   /* close path */
						y1 = old_y;
						PSdrawq(x1, y1);
					}
					/* set flags to fill in outline(if filled) */
					if (panfilq && g_pq.nsides > 2) {
						g_pq.border = 1;
						if (fillq == 1) {
							g_pq.b_color = txt_border_color;
							g_pq.pattern = ( -g_lq.color);
						}
						else {
							g_pq.b_color = font_border_colorq;
							g_pq.pattern = font_patternq;
							if (font_border_colorq != g_lq.color)
								color(font_border_colorq);
						}
						panelq();
						to_be_filled = 1;

						gpcfree((void **) &g_pq.xyp);

						g_pq.xyp = (PGXYPAIRS)gpcalloc(64,
							sizeof(struct GXYPAIRSQ));
						if (g_pq.xyp == (void *)NULL) {
							messageq(WARNQ, 2);		/* Out of Memory */
							panfilq = 0;
						}
					}
					break;
				case 10:
					/* callsubr */
					subrs[subrs_ptr] = current_charstr;	/* store current */
					subrs_index[subrs_ptr++] = char_ptr;
					char_ptr = 0;
					current_charstr = PSsubrsq[ifontq][(int)operands[--op_ptr]];
					break;
				case 11:
					/* return */
					current_charstr = subrs[--subrs_ptr];
					char_ptr = subrs_index[subrs_ptr];
					break;
				case 12:
					/* escape to alt command set */
					value = current_charstr[char_ptr++];
					switch(value) {
						case 0:		/* ignore hints */
							/* dotsection */
						case 1:
							/* vstem3 */
						case 2:
							/* hstem3 */
							op_ptr = 0;
							break;
						case 6: {
							/* seac */
							int bchar, achar, fflagqsav;
							float x1, tx1, y1, widsav;
							float adx, ady;

							ybfsav = ybf;	/* save old spot */
							xbfsav = xbf;
							fflagqsav = flagq;

							widsav = *wid;	/* save old width */

							x1 = operands[1] + current_x - operands[0];   /* calc relative */
							y1 = operands[2];				  /*  location of the */
							tran_x(1);						  /*  accent */
							tran_y(1);
							adx = x1;
							ady = y1;

							bchar = (int)operands[3];		/* base char */
							achar = (int)operands[4];		/* accent */

							if (bchar == 245)
								bchar = 240;

							switch(achar) {
								case 193:
								case 194:
								case 195:
								case 196:
								case 197:
								case 198:
								case 199:
								case 200:
									achar += 48;
									break;
								case 202:
								case 203:
									achar += 47;
									break;
								case 205:
								case 206:
								case 207:
								case 208:
									achar += 46;
									break;
							}

							achar -= 32;
							bchar -= 32;

							xbf = adx;	/* adjust place */
							ybf = ady;
							flagq = 0;	/* don't adjust angle yet */

							/* draw accent first */
							handlePSFontq(-10, achar, action, wid, top, bottom);

							ybf = ybfsav;	/* restore place */
							xbf = xbfsav;

							*wid = 0.f;	/* reset width */

							/* draw base character */
							handlePSFontq(-10, bchar, action, wid, top, bottom);

							if (widsav > *wid)
								*wid = widsav;	/* return width of base */
							flagq = fflagqsav;	/* can change angle now */

							/* end of character */

							if (g_pq.xyp != NULL)
								gpcfree((void **) &g_pq.xyp);
							if (g_lq.xyp != NULL)
								gpcfree((void **) &g_lq.xyp);

							panfilq = 0;
							return;
						}
						case 7:
							/* sbw */
							width = hgtwdq = operands[2] * Matrix[0]  + charspcq;
							current_x = operands[0] + kernx;	/* set first point */
							current_y = operands[1];
							op_ptr = 0;

							if (const_spaceq) {
								width += (constwidthq - operands[2]);
								current_x += (constwidthq - operands[2]) / 2;
							}

							temptop = tempbot = 0;

							if (action == 'c') {
								FindBBox = 1;
								panfilq = 0;
								break;
							}

							if (flagq) {
								newangq(); /* get angle */
								Get_angq(&cosq, &sinq);
							}

							/* set up final matrix */
							for (index = 0; index < 6; index++)
								tmatrix[index] = Matrix[index];
							t2matrix[0] = t2matrix[3] = cosq;
							t2matrix[1] = sinq;
							t2matrix[2] = -sinq;
							t2matrix[4] = t2matrix[5] = 0;

							multiplymatrixq(tmatrix, t2matrix);
							break;
						case 12:
							/* div */
							x2 = operands[--op_ptr];
							x1 = operands[--op_ptr];
							operands[op_ptr++] = x1 / x2;
							break;
						case 16: {
							/* callothersubr */
							int other_sub, number, index;

							other_sub = (int)operands[--op_ptr]; /* other sub number */
							number = (int)operands[--op_ptr];	/* number of operands */

							/* move operands to othersub stack */
							for (index = 0; index < number; index++)
								other_ops[oth_ptr++] = operands[--op_ptr];

							switch(other_sub) {
								case 0:
										/* first value is control */
									Flex_hgt = (int)other_ops[--oth_ptr];
									x1 = Flex_x[0];  /* restore old point */
									y1 = Flex_y[0];
									tran_x(1);
									tran_y(1);
									lmoveq((int) x1,(int) y1);
									x1 = Flex_x[1];
									x2 = Flex_x[4];
									y1 = Flex_y[1];
									y2 = Flex_y[4];
									tran_x(1);
									tran_y(1);
									tran_x(2);
									tran_y(2);

		/* if actual distance between curves and lines is greater than */
		/*  control value/100, use curves */
									if (sqrt((double) (((x1-x2) * (x1-x2))
										+ ((y1-y2) * (y1-y2)))) >=
										(double) (Flex_hgt / 100)) {

										/* do curves */
										x1 = Flex_x[0];
										y1 = Flex_y[0];
										x2 = Flex_x[2];
										y2 = Flex_y[2];
										x3 = Flex_x[3];
										y3 = Flex_y[3];
										x4 = Flex_x[4];
										y4 = Flex_y[4];
										draw_bezierq(x1,y1,x2,y2,x3,y3,x4,y4);
										/* second curve */
										x1 = Flex_x[4];
										y1 = Flex_y[4];
										x2 = Flex_x[5];
										y2 = Flex_y[5];
										x3 = Flex_x[6];
										y3 = Flex_y[6];
										x4 = Flex_x[7];
										y4 = Flex_y[7];
										draw_bezierq(x1,y1,x2,y2,x3,y3,x4,y4);
										/* finished drawing curves */
										current_x = other_ops[--oth_ptr];	/* set new current point */
										current_y = other_ops[--oth_ptr];	/*  from absolute coords */
									}
									else {
										current_x = x1 = other_ops[--oth_ptr];	/* set new current point */
										current_y = y1 = other_ops[--oth_ptr];	/* from absolute coords */
										PSdrawq(x1, y1);
									}
									other_ops[oth_ptr++] = current_y;		/* store x,y for */
									other_ops[oth_ptr++] = current_x;		/*  return to charstring */
									break;
								case 1:
									Flex_ptr = 0;             /* reset counter */
									Flex_x[Flex_ptr] = current_x;	  /* first call to Flex */
									Flex_y[Flex_ptr++] = current_y;  /* save initial points */
									break;
								case 2:
									Flex_x[Flex_ptr] = current_x;
									Flex_y[Flex_ptr++] = current_y;
									break;				/* middle calls - save current_point */
								case 3:
									other_ops[oth_ptr - 1] = 3;	/* hint replacement */
									break;					/* ignore and go to subr 3 */
								default:
									/* Unsupported othersub entry */
									messageq(PS_WARN, PS_UNSUPPORTED);
									break;
								}
							}
							break;
						case 17:
							/* pop */
							operands[op_ptr++] = other_ops[--oth_ptr];
							break;
						case 33:
							/* setcurrentpoint*/
							current_x = operands[0];
							current_y = operands[1];
							op_ptr = 0;
							break;
					};
					break;
				case 13:
					/* hsbw */
					width = hgtwdq = operands[1] * Matrix[0] + charspcq;
					current_x = operands[0] + kernx;	/* set current point */
					current_y = 0;
					op_ptr = 0;

					if (const_spaceq) {
						width += (constwidthq - operands[1]);
						current_x += (constwidthq - operands[1]) / 2;
					}

					temptop = tempbot = 0;

					if (action == 'c') {
						panfilq = 0;
                        FindBBox = 1;
						break;
					}

					if (flagq) {
						newangq();	/* get angle */
						Get_angq(&cosq, &sinq);
					}

					/* set transformation matrix */
					for (index = 0; index < 6; index++)
						tmatrix[index] = Matrix[index];
					t2matrix[0] = t2matrix[3] = cosq;
					t2matrix[1] = sinq;
					t2matrix[2] = -sinq;
					t2matrix[4] = t2matrix[5] = 0;

					multiplymatrixq(tmatrix, t2matrix);
					break;
				case 14:
					/* endchar */
					/*  do any filling required */
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

					if (panfilq && to_be_filled)
					{
						panproc();
						panfilq = 0;
					}

					if (g_pq.xyp != NULL)
						gpcfree((void **) &g_pq.xyp);
					if (g_lq.xyp != NULL)
						gpcfree((void **) &g_lq.xyp);

					if (scrlevel <= 0) {
						if (tempbot < *bottom)
							*bottom = temp;
					}
					if (scrlevel >= 0) {
						temp = (int)newheight + temptop;
						if (temp > *top)
							*top = temp;
					}

					if (skewq)
						width += skewfacq * temptop;

					*wid += width;

					if (action == 'c') {
						if (g_pq.xyp != NULL)
							gpcfree((void **) &g_pq.xyp);
						if (g_lq.xyp != NULL)
							gpcfree((void **) &g_lq.xyp);

						panfilq = 0;
						return;
					}
					xbf += width * cosq;
					ybf += width * sinq;

					ReturnXbfYbf(xbf, ybf, hgtwdq, panfilq);
					if (g_pq.xyp != NULL)
						gpcfree((void **) &g_pq.xyp);
					if (g_lq.xyp != NULL)
						gpcfree((void **) &g_lq.xyp);

					panfilq = 0;
					return;
				case 21:
					/* rmoveto */
					current_x += operands[0];
					current_y += operands[1];
					op_ptr = 0;
					if (newseg)
					{
						old_x = current_x; 	/* save old x y for closepath */
						old_y = current_y;
						newseg = 0;
					}
					x1 = current_x;
					y1 = current_y;
					tran_x(1);
					tran_y(1);
					lmoveq((int) x1,(int) y1);
					break;
				case 22:
					/* hmoveto */
					current_x += operands[0];
					op_ptr = 0;
					if (newseg)
					{
						old_x = current_x;	/* save old for closepath */
						old_y = current_y;
						newseg = 0;
					}
					x1 = current_x;
					y1 = current_y;
					tran_x(1);
					tran_y(1);
					lmoveq((int) x1,(int) y1);
					break;
				case 30:
					/* vhcurveto */
					x1 = current_x;
					x2 = x1;
					x3 = x2 + operands[1];
					x4 = x3 + operands[3];
					current_x = x4;
					y1 = current_y;
					y2 = y1 + operands[0];
					y3 = y2 + operands[2];
					y4 = y3;
					current_y = y4;
					op_ptr = 0;
					draw_bezierq(x1,y1,x2,y2,x3,y3,x4,y4);
					break;
				case 31:
					/* hvcurveto */
					x1 = current_x;
					x2 = x1 + operands[0];
					x3 = x2 + operands[1];
					x4 = x3;
					current_x = x4;
					y1 = current_y;
					y2 = y1;
					y3 = y2 + operands[2];
					y4 = y3 + operands[3];
					current_y = y4;
					op_ptr = 0;
					draw_bezierq(x1,y1,x2,y2,x3,y3,x4,y4);
					break;
				default:
					/* unsupported command */
					messageq(PS_WARN, PS_UNSUPPORTED);
					op_ptr = 0;
					break;
			};
		}
	}
}

/**************************************************************************/
/* multiply two matrices, store the result in Matrix. Does NOT do a true
 * matrix multiplication, as xbf and ybf are added straight. */
void multiplymatrixq(float *m1, float *m2)
{
	G_PROCESS_ID

	Matrix[0] = m1[0] * m2[0] + m1[1] * m2[2];
	Matrix[1] = m1[0] * m2[1] + m1[1] * m2[3];
	Matrix[2] = m1[2] * m2[0] + m1[3] * m2[2];
	Matrix[3] = m1[2] * m2[1] + m1[3] * m2[3];
	Matrix[4] = (float)xbf;
	Matrix[5] = (float)ybf;
}

/**************************************************************************/
/* draw_bezierq() draws a bezier curve using a DDA engine. The forward
 * differences are calculated and refined. The curve is then displayed
 * by looping thru each step and adjusting the point accordingly. */
void draw_bezierq(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4)
{
	struct delta
	{
		float d1, d2, d3;
	} dx,dy,fx,fy;

	float error,new_x,new_y;
	float tx1, tx2, tx3, tx4;
	int N, at;

	G_PROCESS_ID

	tran_x(1);    /* must transform points before */
	tran_y(1);    /* using them for calculations */
	tran_x(2);    /* to avoid errors */
	tran_y(2);
	tran_x(3);
	tran_y(3);
	tran_x(4);
	tran_y(4);

	new_x = x1;
	new_y = y1;

	/* calculate forward differences to DDA */

	dx.d1 = x2 - x1;
	dx.d2 = x3 - x2 - dx.d1;
	dx.d3 = x4 - (2 * x3) + x2 - dx.d2;

	dy.d1 = y2 - y1;
	dy.d2 = y3 - y2 - dy.d1;
	dy.d3 = y4 - (2 * y3) + y2 - dy.d2;

	fx.d1 = 3 * (dx.d1 + dx.d2) + dx.d3;
	fx.d2 = 6 * (dx.d2 + dx.d3);
	fx.d3 = 6 * (dx.d3);

	fy.d1 = 3 * (dy.d1 + dy.d2) + dy.d3;
	fy.d2 = 6 * (dy.d2 + dy.d3);
	fy.d3 = 6 * (dy.d3);

	error = (float)(0.75 * max(sqrt(dx.d2 * dx.d2 + dy.d2 * dy.d2),
			sqrt((dx.d2 + dx.d3) * (dx.d2 + dx.d3) +
			(dy.d2 + dy.d3) * (dy.d2 + dy.d3))));

	for (N = 1; error > PSMaxError[ifontq]; N *= 2)  /* refine differences to reduce error */
	{
		fx.d3 = fx.d3 / 8;
		fx.d2 = (fx.d2 / 4) - fx.d3;
		fx.d1 = (fx.d1 - fx.d2) / 2;

		fy.d3 = fy.d3 / 8;
		fy.d2 = (fy.d2 / 4) - fy.d3;
		fy.d1 = (fy.d1 - fy.d2) / 2;

		error /= 4;		/* each pass reduces error by 4 */
	}

	at = 0;

	while (at < N )
	{
		new_x += fx.d1;	/* calculate and draw line */
		fx.d1 += fx.d2;	/* to new point */
		fx.d2 += fx.d3;	/* and update differences */

		new_y += fy.d1;
		fy.d1 += fy.d2;
		fy.d2 += fy.d3;

		if ((new_y - ybf) > temptop) temptop = (int) (new_y - ybf);
		if ((new_y - ybf) < tempbot) tempbot = (int) (new_y - ybf);

		if (FindBBox)
		{
			at++;
			continue;
		}

		if (!panfilq || wplane_flgq != 0) {
			ldrawq((int) new_x, (int) new_y);
		}
		else {
			g_pq.xyp[g_pq.nsides].x = (int) new_x;
			g_pq.xyp[g_pq.nsides].y = (int) new_y;
			g_pq.nsides = newbufsq(&g_pq.xyp, g_pq.nsides);
		}

		at++;

	}
}
/**************************************************************************/
/* Transforms and draws line from current point. */
void PSdrawq(float x1, float y1)
{
	float tx1;

	G_PROCESS_ID

	tran_x(1);	/* transform x,y to tkf space */
	tran_y(1);

	if ((y1 - ybf) > temptop) temptop = (int) (y1 - ybf);
	if ((y1 - ybf) < tempbot) tempbot = (int) (y1 - ybf);

	if (FindBBox)
	{
		return;
	}

	if (!panfilq || wplane_flgq != 0) {
		ldrawq((int) x1, (int) y1);
	}
	else {
		g_pq.xyp[g_pq.nsides].x = (int) x1;
		g_pq.xyp[g_pq.nsides].y = (int) y1;
		g_pq.nsides = newbufsq(&g_pq.xyp, g_pq.nsides);
	}
}

/**************************************************************************/
/* Decode the instruction from PS Type 1 font. */
int getTypeval(float *operands, int *op_ptr,
				Uchar *current_charstr, int *char_ptr, int *value)
{
	long longvalue;

	*value = current_charstr[(*char_ptr)++];

	if (*value >= 32 && *value <= 246)
	{
		*value -= 139;
		operands[(*op_ptr)++] = (float) *value;
		return 0;
	}
	else if (*value >= 247 && *value <= 250)
	{
		*value = ((*value - 247) * 256) + current_charstr[(*char_ptr)++] + 108;
		operands[(*op_ptr)++] = (float) *value;
		return 0;
	}
	else if (*value >= 251 && *value <= 254)
	{
		*value = -((*value - 251) * 256) - current_charstr[(*char_ptr)++] - 108;
		operands[(*op_ptr)++] = (float) *value;
		return 0;
	}
	else if (*value == 255)
	{
		longvalue = current_charstr[*char_ptr] * 16777216l +
					current_charstr[*char_ptr+1] * 65536l +
					current_charstr[*char_ptr+2] * 256 +
					current_charstr[*char_ptr+3];
		*char_ptr = *char_ptr + 4;
		operands[(*op_ptr)++] = (float) longvalue;
		return 0;
	}
	else return 1;
}

/**************************************************************************/
/* Searches for last and next character drawn in kern list. Returns offset */
void findkernpair(int lastchar, int nextchar, float *xoff)
{
	int index = 0;

	G_PROCESS_ID

	if (!PS_Kernq[ifontq]) /* no kerning! */
	{
		*xoff = 0.0f;
		return;
	}

	/* find last character */
	while (PS_KernPairsq[ifontq][index].char1 != -1 && PS_KernPairsq[ifontq][index].char1 != lastchar)
		index++;

	if (PS_KernPairsq[ifontq][index].char1 == -1)
	{
		*xoff = 0.0f;	/* not found */
		return;
	}

	/* find second character */
	while (PS_KernPairsq[ifontq][index].char1 == lastchar && PS_KernPairsq[ifontq][index].char2 != nextchar)
		index++;

	if (PS_KernPairsq[ifontq][index].char1 == lastchar)
	{
		*xoff = (float) PS_KernPairsq[ifontq][index].offset; /* found it */
		return;
	}
	else
	{
		*xoff = 0.0f; /* no kerning required */
		return;
	}
}
/**************************************************************************/
void SetPSFull(char onoff, char string)
{
	int i, n = 1000;
	G_PROCESS_ID

	for (i = 0; i < FNTNUM; i++) {
		if (escsym[i] == (Uchar) string)
			n = i;
	}

	if (n == 1000)
		return;

	PSfullq[n] = onoff;
}
/**************************************************************************/
int GetPSCharWidth(int ifont, unsigned char char_num)
{
	int ttop = 0, tbot = 0;
	float grid, twid = 0;

	G_PROCESS_ID

	fnthtq = PSHGT;
	ifontq = ifont;
	cosq = 1;
	sinq = 0;
	flagq = 0;
	widenq = 0;
	skewq = 0;
	tfontq = 0;
	charspcq = 0;

	grid = (1 / FontMatrix[ifont][0]);
	heightq = (float) (grid / fnthtq);

	handlePSFontq(-10, char_num,'c', &twid, &ttop, &tbot);

	return (int) twid;
}
/**************************************************************************/
void GetPSWidest(int ifont, int *w, unsigned char *char_num)
{
	int tw, i;

	for (i = 0; i < 168; i++)
	{
		if ((tw = GetPSCharWidth(ifont, (unsigned char) i)) > *w)
		{
			*w = tw;
			*char_num = (char) (i + 32);
		}
	}
}
/**************************************************************************/
void SetPSMaxError(float newerror, char string)
{
	int i, n = 1000;
	G_PROCESS_ID

	for (i = 0; i < FNTNUM; i++) {
		if (escsym[i] == (Uchar) string)
			n = i;
	}

	if (n == 1000)
		return;

	PSMaxError[n] = newerror;
}
/**************************************************************************/
void SetPSKerning(char onoff, char string)
{
	int i, n = 1000;
	G_PROCESS_ID

	for (i = 0; i < FNTNUM; i++) {
		if (escsym[i] == (Uchar) string)
			n = i;
	}

	if (n == 1000)
		return;

	PS_Kernq[n] = onoff;
}
