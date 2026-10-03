/* (c) 1984-1993 by Scientific Endeavors Corporation.
   All rights reserved.
Version 7.0 PostScript/Truetype extensions written by David Dillow.
**************************************************************/
#define __TRUETYPE

#include <graphic.h>
#include <psfont.h>
#include <truetype.h>
#include <font_ext.h>

/* external function and variables required from basic GraphiC */

void ReturnXbfYbf(float x, float y, float h, char p);
void Get_angq(float *c, float *s);

void newangq(void);

#ifdef MULTITHREAD
struct GEN_FONT_DATA *GENERAL_dataq[GPC_MAX_THREADS];
#else
struct GEN_FONT_DATA GENERAL_dataq;
#endif

/**************************************************************************/
/* This routine simply allocates memory for the shared data in OS/2       */
void SetTTforOS2(void)
{
#ifdef MULTITHREAD
	G_PROCESS_ID

	tt_font_dataq[instctr] = (struct TT_INFO *)gpcalloc(1,
		sizeof(struct TT_INFO));
#endif
}

#ifdef MULTITHREAD
/**************************************************************************/
/* Free shared memory */
void FreeTTData(int instctr)
{
	gpcfree((void **) &tt_font_dataq[instctr]);
}
#endif

/**************************************************************************/
/* This routine simply frees all memory allocated by the TrueType code    */
void FreeTrueTypeMemory(int n)
{
	G_PROCESS_ID

	if(FontName[n] != NULL)
	{
		gpcfree((void **) &FontName[n]);
		FontName[n] = NULL;
	}

	if(TranslateASCII != NULL)
	{
		gpcfree((void **) &TranslateASCII);
		TranslateASCII = NULL;
	}

	gpcfree((void **) &TTMetrics[n]);
	TTMetrics[n] = NULL;

	gpcfree((void **) &TTCharMap[n].charmap);
	TTCharMap[n].charmap = NULL;

	gpcfree((void **) &TTLoc[n]);
	TTLoc[n] = NULL;

	if(GlyphsInMemory[n])
	{
		gpcfree((void **) &GlyphTable[n]);
		GlyphTable[n] = NULL;
	}

	if(KernTable[n] != NULL)
	{
		gpcfree((void **) &KernTable[n]);
		KernTable[n] = NULL;
	}
}

/**************************************************************************/
/* this routine accepts a 4 character tag inside a long and attemps to    */
/*  find it in the TrueType font file's table directory                                   */
int FindTable(FILE *file, ULONG searchtag, ULONG *offset, ULONG *length)
{
	USHORT numTables, index;
	ULONG tag;
	ULONG Dummy;    /* used to suppress warnings */

	fseek(file, 0, 0);              /* Table directory starts at very beginning of font file */

	Dummy = (ULONG) GetFixed(file);                 /* ignore version number */
	numTables = GetUSHORT(file);            /* find how many tables */
	Dummy = (ULONG) GetUSHORT(file);                /* ignore optimized search variables */
	Dummy = (ULONG) GetUSHORT(file);
	Dummy = (ULONG) GetUSHORT(file);

	Dummy++; /* suppress warning */

	for(index = 0; index < numTables; index++)      /* check each table */
	{
		tag = GetULONG(file);                           /* get table name */

		if(searchtag == tag)                            /* is it the one we want? */
		{
			GetULONG(file);                                 /* yes, ignore checksum */
	    *offset = GetULONG(file);       
			*length = GetULONG(file);
			return TRUE;                    /* tell caller that we found it */
		}
		else
		{
			GetULONG(file);                                 /* nope, this isn't it */
			GetULONG(file);                                 /* skip to next table */
			GetULONG(file);
		}
	}
 
	return FALSE;   /* tell caller that it's not there */
}

/**************************************************************************/
/* This routine attempts to load the font into memory. It also loads all  */
/*  variables and tables necessary to allow the font to be displayed      */
int LoadTrueTypeFont(int number, char *filename)
{
	ULONG offset, length;
	USHORT temp, indexToLocFormat, index, numberOfHMetrics;
	USHORT NumNameRecords, StringTableOffset, foundName;

	struct NameRec
	{
		USHORT PlatformID;
		USHORT EncodingID;
		USHORT LanguageID;
		USHORT NameID;
		USHORT Offset;
		USHORT Length;
	} *NameRecord;

	int top = 0, bot = 0;

	FILE *fontfile;

	BYTE buffer[255];

	G_PROCESS_ID

	if(!LoadTranslateTable())
		return TTBADNOMESSAGE;

	fontfile = gfopen(filename, "rb", 1);
	if (fontfile == NULL)                   /* Error opening font file */
		return TTINVALIDFONT;

	/* Must have a glyph table to be a font! */

	if(!FindTable(fontfile, (ULONG) TABLENAME('g','l','y','f'), &GlyphTableOffset[number],
										&length))
	{
		fclose(fontfile);
		return TTINVALIDFONT;
	}

	GlyphsInMemory[number] = 0;     /* Default to glyphs NOT in memory */

	if(TTStoreq && length > 0)
	{
		GlyphTable[number] = (BYTE *)gpcalloc((size_t) length + 4, 1);

		if(GlyphTable[number] != NULL)
		{
			fseek(fontfile, GlyphTableOffset[number], 0);
			if(fread(GlyphTable[number], 1, length, fontfile) == length)
				GlyphsInMemory[number] = 1;
			else
			{
				gpcfree((void **) &GlyphTable[number]);
				GlyphTable[number] = NULL;
			}
		}
	}

	/* find the mapping tables, then load one */

	if(!FindTable(fontfile, (ULONG) TABLENAME('c','m','a','p'), &offset, &length))
	{
		if(GlyphsInMemory[number])
			gpcfree((void **) &GlyphTable[number]);
		fclose(fontfile);
		return TTINVALIDFONT;
	}

	switch(GetCharMap(fontfile, offset, length, number))
	{
		case TTOK:
			break;
		case TTINVALIDFONT:
			if(GlyphsInMemory[number])
				gpcfree((void **) &GlyphTable[number]);
			return TTINVALIDFONT;
		case TTOUTOFMEMORY:
			if(GlyphsInMemory[number])
				gpcfree((void **) &GlyphTable[number]);
			return TTINVALIDFONT;
	}

	/* find the maximum profile table */

	if(!FindTable(fontfile, (ULONG) TABLENAME('m','a','x','p'), &offset, &length))
	{
		if(GlyphsInMemory[number])
			gpcfree((void **) &GlyphTable[number]);
		gpcfree((void **) &TTCharMap[number].charmap);
		fclose(fontfile);
		return TTINVALIDFONT;
	}

	fseek(fontfile, offset + sizeof(Fixed), 0);     /* ignore version */

	numGlyphs[number] = GetUSHORT(fontfile);        /* find out how many glyphs in font */
	maxPoints[number] = GetUSHORT(fontfile);        /* maximum points in a single glyph */

	fseek(fontfile, sizeof(USHORT), 1);                     /* skip max contours */

	temp = GetUSHORT(fontfile);                                     /* maximum point in a composite glyph */

	if(temp > maxPoints[number])                            /* use the biggest one for our maximum number */
		maxPoints[number] = temp;

	/* find the horizontal header table */

	if(!FindTable(fontfile, (ULONG) TABLENAME('h','h','e','a'), &offset, &length))
	{
		if(GlyphsInMemory[number])
			gpcfree((void **) &GlyphTable[number]);
		gpcfree((void **) &TTCharMap[number].charmap);
		fclose(fontfile);
		return TTINVALIDFONT;
	}

    /* skip to desired item */

	fseek(fontfile, offset + sizeof(Fixed) + 6 * sizeof(FWord) + sizeof(uFWord) + 8 * sizeof(TTSHORT), 0);

	/* find out how many character metrics fields we have */

	numberOfHMetrics = GetUSHORT(fontfile);

	/* find horizontal metrics table */

	if(!FindTable(fontfile, (ULONG) TABLENAME('h','m','t','x'), &offset, &length))
	{
		if(GlyphsInMemory[number])
			gpcfree((void **) &GlyphTable[number]);
		gpcfree((void **) &TTCharMap[number].charmap);
		fclose(fontfile);
		return TTINVALIDFONT;
	}

	/* load table into memory for faster access */

	if(!LoadTable(fontfile, offset, length))
	{
		if(GlyphsInMemory[number])
			gpcfree((void **) &GlyphTable[number]);
		gpcfree((void **) &TTCharMap[number].charmap);
		fclose(fontfile);
		return TTOUTOFMEMORY;
	}

	/* allocate space for metrics (one for each glyph) */

	TTMetrics[number] = (struct longHorMetric *) gpcalloc(numGlyphs[number], sizeof(struct longHorMetric));

	if(TTMetrics[number] == NULL)
	{
		if(GlyphsInMemory[number])
			gpcfree((void **) &GlyphTable[number]);
		gpcfree((void **) &TTCharMap[number].charmap);
		fclose(fontfile);
		return TTOUTOFMEMORY;
	}

	/* get numberofHMetrics full metrics from the table */
	 
	for(index = 0; index < numberOfHMetrics; index++)
	{
		TTMetrics[number][index].advanceWidth = GetTableuFWord();
		TTMetrics[number][index].lsb = GetTableFWord();
	}

	/* the rest of the metrics are monospaced (same advance width) */

	for( ; index < numGlyphs[number]; index++)
	{
		TTMetrics[number][index].advanceWidth = TTMetrics[number][numberOfHMetrics - 1].advanceWidth;
		TTMetrics[number][index].lsb = GetTableFWord();
    }

	/* unload table from memory */

	FinishTable();

	/* find the font header table */

	if(!FindTable(fontfile, (ULONG) TABLENAME('h','e','a','d'), &offset, &length))
	{
		if(GlyphsInMemory[number])
			gpcfree((void **) &GlyphTable[number]);
		gpcfree((void **) &TTCharMap[number].charmap);
	gpcfree((void **) &TTMetrics[number]);
		fclose(fontfile);
		return TTINVALIDFONT;
	}

	/* skip to desired item */

	fseek(fontfile, offset + 2 * sizeof(Fixed) + 2 * sizeof(ULONG) + sizeof(USHORT), 0);

    unitsPerEm[number] = GetUSHORT(fontfile);   /* get the grid size */

	fseek(fontfile, offset + 2 * sizeof(Fixed) + 2 * sizeof(ULONG) + 4 * sizeof(USHORT) + 4 * sizeof(FWord)
						+ sizeof(TTSHORT) + 16, 0);

    indexToLocFormat = GetSHORT(fontfile);              /* we need to know haw the offsets are stored */

	/* find the glyph offset table */

	if(!FindTable(fontfile, (ULONG) TABLENAME('l','o','c','a'), &offset, &length))
	{
		if(GlyphsInMemory[number])
			gpcfree((void **) &GlyphTable[number]);
		gpcfree((void **) &TTCharMap[number].charmap);
	gpcfree((void **) &TTMetrics[number]);
		fclose(fontfile);
		return TTINVALIDFONT;
	}

	if(!LoadTable(fontfile, offset, length))        /* load table into memory for speed */
	{
		if(GlyphsInMemory[number])
			gpcfree((void **) &GlyphTable[number]);
		gpcfree((void **) &TTCharMap[number].charmap);
		gpcfree((void **) &TTMetrics[number]);
		fclose(fontfile);
		return TTOUTOFMEMORY;
	}

	/* allocate space for offset table */

	TTLoc[number] = (ULONG *) gpcalloc(numGlyphs[number] + 1, sizeof(ULONG));

    /* if there's no space return an error */

	if(TTLoc[number] == NULL)       /* really should give a warning here */
	{
		if(GlyphsInMemory[number])
			gpcfree((void **) &GlyphTable[number]);
		gpcfree((void **) &TTCharMap[number].charmap);
		gpcfree((void **) &TTMetrics[number]);
	fclose(fontfile);
		return TTOUTOFMEMORY;
    }

	if(indexToLocFormat)
	{
		/* offsets are stored as ULONG's */

	    for(index = 0; index < numGlyphs[number] + 1; index++)
		{
			TTLoc[number][index] = GetTableULONG();
		}
	}
	else
	{
		/* offsets are stored as USHORT's */

		for(index = 0; index < numGlyphs[number] + 1; index++)
		{
			/* multiply by two to get true offset */

			TTLoc[number][index] = ((ULONG) GetTableUSHORT()) << 1;
		}
	}

	FinishTable();  /* unload table */

    /* find the naming table */

	if(!FindTable(fontfile, (ULONG) TABLENAME('n','a','m','e'), &offset, &length))
	{
		gpcfree((void **) &TTLoc[number]);
		if(GlyphsInMemory[number])
			gpcfree((void **) &GlyphTable[number]);
		gpcfree((void **) &TTCharMap[number].charmap);
		gpcfree((void **) &TTMetrics[number]);
		fclose(fontfile);
		return TTINVALIDFONT;
	}

	fseek(fontfile, offset + sizeof(USHORT), 0);

	NumNameRecords = GetUSHORT(fontfile);   /* get number of names */
	StringTableOffset = GetUSHORT(fontfile);

	NameRecord = (struct NameRec *) gpcalloc(NumNameRecords, sizeof(struct NameRec));

	if(NameRecord == NULL)
	{
	gpcfree((void **) TTLoc[number]);
		if(GlyphsInMemory[number])
			gpcfree((void **) &GlyphTable[number]);
		gpcfree((void **) &TTCharMap[number].charmap);
	gpcfree((void **) &TTMetrics[number]);
		fclose(fontfile);
		return TTOUTOFMEMORY;
	}

	for(index = 0; index < NumNameRecords; index++) /* load the names */
	{
		NameRecord[index].PlatformID = GetUSHORT(fontfile);
		NameRecord[index].EncodingID = GetUSHORT(fontfile);
		NameRecord[index].LanguageID = GetUSHORT(fontfile);
		NameRecord[index].NameID = GetUSHORT(fontfile);
		NameRecord[index].Length = GetUSHORT(fontfile);
		NameRecord[index].Offset = GetUSHORT(fontfile);
	}

	foundName = 0;

	for(index = NumNameRecords; index != 0; index--)
	{
		if(NameRecord[index - 1].NameID != 4)   /* looking for full name */
			continue;

		switch(NameRecord[index - 1].PlatformID)
		{
			case 0: /* Apple Unicode */
				foundName = 1;
				break;
			case 1: /* Macintosh Script Manager */
				if(NameRecord[index - 1].EncodingID == 0 &&     /* Roman encoding */
					NameRecord[index - 1].LanguageID == 0)  /* English language */
					foundName = 1;
				break;
			case 2: /* ISO */
				if(NameRecord[index - 1].EncodingID == 0)
					foundName = 1;
				break;
			case 3: /* MicroSoft */
				if(NameRecord[index - 1].EncodingID == 1 &&             /* UGL encoding */
					NameRecord[index - 1].LanguageID == 0x0409)     /* US English */
					foundName = 1;
				break;
			default:
				break;
		}

		if(foundName)
			break;
	}

	if(foundName)
	{
		index--;

		fseek(fontfile, offset + StringTableOffset + NameRecord[index].Offset, 0);

		if(NameRecord[index].Length > 254)
			NameRecord[index].Length = 254;

		fread(buffer, NameRecord[index].Length, 1, fontfile);
		buffer[NameRecord[index].Length] = '\0';

		if(NameRecord[index].PlatformID == 3)
		{
			unsigned int PlaceMark, InsertIndex;
			int UnicodeChar;

			InsertIndex = 0;

			for(PlaceMark = 0; PlaceMark < NameRecord[index].Length;
									 PlaceMark += 2)
			{
				UnicodeChar = (buffer[PlaceMark] << 8) |
								(buffer[PlaceMark + 1]);

				buffer[InsertIndex++] = (char) UnicodeChar;
			}

			buffer[InsertIndex] = '\0';
		}

		FontName[number] = (char *) gpcalloc(NameRecord[index].Length + 1, 1);

		if(FontName[number] != NULL)
			strcpy(FontName[number], (char *)buffer);
	}
	else
		FontName[number] = NULL;

	gpcfree((void **) &NameRecord);

	/* try to find kerning information */

	if(FindTable(fontfile, (ULONG) TABLENAME('k','e','r','n'), &offset, &length))
	{
		if(TTStoreq && length > 0)
		{
			KernTable[number] = (BYTE *)gpcalloc((size_t) length + 4, 1);

			if(KernTable[number] != NULL)
			{
				fseek(fontfile, offset, 0);
				if (fread(KernTable[number], 1, length, fontfile) != length)
				{
					gpcfree((void **) &KernTable[number]);
					KernTable[number] = NULL;
				}
			}
		}
	}

	fclose(fontfile);

	TTFudgeFactor[number] = 1.f;
	ifontq = number;
	fnthtq = 1;
	heightq = unitsPerEm[number];
	cosq = 1.f;
	sinq = 0.f;

	TrueTypeOut(-10, ('X' - 32), 'c', &w1, &top, &bot);

	if (top > 0)
		TTFudgeFactor[number] = (float) unitsPerEm[number] / top;
	else
		TTFudgeFactor[number] = 1.f;

	return TTOK;    /* finished with this font */
}

/**************************************************************************/
/* Translates our ASCII code to the corresponding Unicode character       */
int OurASCIItoUnicode(int CharIn)
{
	G_PROCESS_ID

	if(CharIn < 128)                /* codes under 128 are the same */
		return CharIn;

	return TranslateASCII[CharIn - 128];
}

/**************************************************************************/
/* Find the character Char in the mapping table and returns the glyph     */
/*  corresponds to it. 0 is the "missing glyph"                                                   */    
USHORT MapCharToGlyph(USHORT Char)
{
	USHORT index = 0;

	G_PROCESS_ID

	if(TTCharMap[ifontq].format == 0)       /* simple 256 char (1 byte) mapping */
	{
		if(Char > 255)                                  /* only 256 glyphs */
			return (USHORT) 0;

		return TTCharMap[ifontq].charmap[Char]; /* return the glyph from array */
    }

	while(TTCharMap[ifontq].endCount[index] != 0xFFFF)      /* 0xFFFF is the end sentinel */
	{
		if(TTCharMap[ifontq].endCount[index] < Char)    /* if the Char is higher than the end of segment */
		{
			index++;                                                                        /* continue to next segment */
			continue;
		}

		if(TTCharMap[ifontq].startCount[index] > Char)  /* if the Char is lower than the beginning of the segment */
			return (USHORT) 0;                                                      /* there is no corresponding glyph */

		if(TTCharMap[ifontq].idRangeOffset[index] != 0) /* do specific mappings */
		{
			USHORT glyph = *(TTCharMap[ifontq].idRangeOffset[index] / 2 +
						(Char - TTCharMap[ifontq].startCount[index]) +
						&TTCharMap[ifontq].idRangeOffset[index]);
			if(glyph != 0)
				glyph = (USHORT)((glyph + TTCharMap[ifontq].idDelta[index]) & 0xFFFF);
			return glyph;
		}
		else    /* these are listed in the TrueType specs */
		{
			return (USHORT) ((Char + TTCharMap[ifontq].idDelta[index]) & 0xFFFF);
		}
	}

	return (USHORT) 0;      /* Char has no corresponding glyph */
}

/**************************************************************************/
/* This is the main TrueType handler. It is called from strsizq in TTFNTPLT.C */
void TrueTypeOut(int lastChar, int Char, int action, float *width, int *top, int *bottom)
{
	USHORT GlyphID, numPointsinGlyph = 0;
	float advanceWidth, kernWidth;
	struct TTPoint *points;

	G_PROCESS_ID

	Char += 32;             /* get actual ASCII character code */

	if(!TTCharIsUnicodeq)                           /* if it is an ASCII character */
		Char = OurASCIItoUnicode(Char); /*  translate it to Unicode */

	GlyphID = MapCharToGlyph((USHORT) Char);        /* translate Unicode char to glyph index */

	if(action == 'p' && flagq)                      /* if we are to plot it, advance rotation counters, if on a circle */
	{
		newangq();
		Get_angq(&cosq, &sinq);
	}

	/* allocate space for the points of the glyph */
	{
		USHORT npts_alloc = (maxPoints[ifontq] < 1024) ? 1024 : (maxPoints[ifontq] * 2);
		points = (struct TTPoint *) gpcalloc(npts_alloc, sizeof(struct TTPoint));
	}

	/* if there isn't enough space, warn user */

	if(points == NULL)
	{
		messageq(TT_WARN, TT_WARNMEMORY);
		return;
	}

	/* loaf glyph into point structure */

	GetGlyph(&numPointsinGlyph, points, GlyphID, &advanceWidth, top, bottom);

	/* advance the width of the character */

	*width += advanceWidth;

	if(action == 'p') {		/* if we are plotting... */
			/* if there is kerning to do */
		if(TTkernq[ifontq] && !flagq && lastChar != -10) {
			/* do it */
			kernWidth = GetTTKerningWidth((USHORT)(lastChar + 32), (USHORT)Char);
			xbf += kernWidth * cosq;
			ybf += kernWidth * sinq;
		}

		if(numPointsinGlyph)    /* make sure the are points to plot */
			OutputGlyph(&numPointsinGlyph, points);

		xbf += advanceWidth * cosq;     /* advance character position */
		ybf += advanceWidth * sinq;
		hgtwdq = advanceWidth;
	}
	else {
		if(TTkernq[ifontq] && lastChar != -10)  /* just add kerning */
			*width -= GetTTKerningWidth((USHORT)(lastChar + 32), (USHORT)Char);
	}

	gpcfree((void **) &points);

	ReturnXbfYbf(xbf, ybf, hgtwdq, panfilq);
}

/**************************************************************************/
/* This is the main Glyph loading function. It handles the calculations   */
/*  for composite glyphs, and distrubes actual loading to GetSingleGlyph  */
/*  It allows the glyph table to be in memory, or just uses the disk.     */
void GetGlyph(USHORT *numPoints, struct TTPoint *points, USHORT GlyphID, float *advanceWidth, int *top, int *bottom)
{
    FILE *file;
	ULONG GlyphOffset, GlyphLength, currentLocation;
	TTSHORT numberOfContours;
	float Width_Offset, Scale, TempScale, TTMatrix[4], NewMatrix[4], TempMatrix[4];
	USHORT flags, nextGlyph, GlyphMetrics, CombinedArg;
	TTSHORT argument1, argument2;
	BYTE GlyphMetricsSet;
	int TTtemptop, TTtempbottom;

	G_PROCESS_ID

	TTtemptop = TTtempbottom = 0;   /* tempory variables to find extents of the character */

	file = NULL;            /* set it to NULL, we may not need it (if glyphs are in memory) */

	GlyphOffset = TTLoc[ifontq][GlyphID];                                   /* calculate offset and length */
	GlyphLength = TTLoc[ifontq][GlyphID + 1] - GlyphOffset; /*  of glyph */

	if(GlyphLength == 0)    /* if there is no glyph description, return */
	{
	/* the scale is the height in pixels divided by the number of units in the font */
	/*  multiplyied by our modifier to get a full height (no space at top of square) */

		Scale = ((heightq * fnthtq * ((TTfullq[ifontq]) ? (TTFudgeFactor[ifontq]) : (1)))
						 / unitsPerEm[ifontq]);

	/* calculate the advance width, including the widening factor and extra spacing */

		if(const_spaceq)
		{
			*advanceWidth = constwidthq * Scale *
							((widenq) ? (widfacq) : (1.0f)) + charspcq;
		}
		else
		{
			*advanceWidth = TTMetrics[ifontq][GlyphID].advanceWidth * Scale *
							((widenq) ? (widfacq) : (1.0f)) + charspcq;
		}

		return;
	}

	if(const_spaceq) {
		Width_Offset = 
			(constwidthq - TTMetrics[ifontq][GlyphID].advanceWidth) / 2;
	}
	else {
		Width_Offset = charspcq;
	}

	if(GlyphsInMemory[ifontq])
	{
		SetTable(GlyphTable[ifontq]);   /* set table pointers to our glyph table */
		Tableseek(GlyphOffset, 0);              /* position to our glyph */
	}
	else
	{
		file = gfopen(fntptr[ifontq], "rb", 1); /* load our glyph as a table */
		if(!LoadTable(file, GlyphOffset + GlyphTableOffset[ifontq], GlyphLength))
		{
			fclose(file);
			return;
		}
	}

	numberOfContours = GetTableSHORT();     /* get the number of contours in the glyph */
	Tableseek(4 * sizeof(FWord), 1);        /* skip bounding box */
	currentLocation = TableTell();          /* save our spot */

	if(!GlyphsInMemory[ifontq])
		FinishTable();                                  /* free table if reading from disk */
										/* (GetSingleGlyph loads the actual glyph) */

   *numPoints = 0;      /* start with no points */

	GlyphMetrics = GlyphID; /* defualt to the original Glyph's Metrics */
	GlyphMetricsSet = 0;    /* they matrics have not been offically set yet */

	/* the scale is the height in pixels divided by the number of units in the font */
	/*  multiplyied by our modifier to get a full height (no space at top of square) */
	 
	Scale = ((heightq * fnthtq * ((TTfullq[ifontq]) ? (TTFudgeFactor[ifontq]) : (1)))
					 / unitsPerEm[ifontq]);

	/* load the scale into our transformation matrix, along with rotation info */

	TTMatrix[0] = TTMatrix[3] = Scale * cosq;
	TTMatrix[1] = Scale * sinq;
	TTMatrix[2] = -TTMatrix[1];

	if(widenq)
	{
	/* exagerate the X scale */
		TTMatrix[0] = TTMatrix[0] * widfacq;
	}

	if(skewq)
	{
		/* shift the points over based on the y value of height */
		TTMatrix[2] = skewfacq * Scale;
	}

	if(numberOfContours < 0)
	{
		/* composite glyph (more than one base glyph) */

		do
		{
			NewMatrix[0] = TTMatrix[0];     /* load temporary matrix */
			NewMatrix[1] = TTMatrix[1];
			NewMatrix[2] = TTMatrix[2];
			NewMatrix[3] = TTMatrix[3];

			if(!GlyphsInMemory[ifontq])     /* read glyph into memory, if necessary */
			{
				if(!LoadTable(file, GlyphOffset + GlyphTableOffset[ifontq], GlyphLength))
				{
					fclose(file);
					return;
		}
			}

			Tableseek(currentLocation, 0);  /* restore our spot */

			flags = GetTableUSHORT();               /* get the flags for this glyph */
			nextGlyph = GetTableUSHORT();   /* get the component glyph ID */

			if(flags & ARG_1_AND_2_ARE_WORDS)       
			{
				argument1 = GetTableSHORT();    /* directly read the arguments */
				argument2 = GetTableSHORT();
			}
			else
			{
				CombinedArg = GetTableUSHORT(); /* read arguments ((a1 << 8) | a2) */
				argument2 = (TTCHAR) (CombinedArg & 0xFF); /* strip off arg1 */
				argument1 = (TTCHAR) (CombinedArg >> 8);        /* lose arg2 to leave arg1 */
			}

			if(flags & WE_HAVE_A_SCALE)
			{
				TempScale = F2Dot14ToFloat(GetTableF2Dot14());  /* read the scale */
				NewMatrix[0] = TempScale * TTMatrix[0]; /* incorporate it in our TTMatrix */
				NewMatrix[3] = TempScale * TTMatrix[3];
		}
			else if(flags & WE_HAVE_AN_X_AND_Y_SCALE)
			{
				NewMatrix[0] = F2Dot14ToFloat(GetTableF2Dot14()) * TTMatrix[0]; /* incorporate X scale */
				NewMatrix[3] = F2Dot14ToFloat(GetTableF2Dot14()) * TTMatrix[3]; /* incorporate Y scale */
			}
			else if(flags & WE_HAVE_A_TWO_BY_TWO)
			{
				TempMatrix[0] = F2Dot14ToFloat(GetTableF2Dot14());      /* load in matrix */
				TempMatrix[1] = F2Dot14ToFloat(GetTableF2Dot14());
				TempMatrix[2] = F2Dot14ToFloat(GetTableF2Dot14());
				TempMatrix[3] = F2Dot14ToFloat(GetTableF2Dot14());

				/* multiply new matrix with our matrix */

				NewMatrix[0] = (TTMatrix[0] * TempMatrix[0]) +
								(TTMatrix[1] * TempMatrix[2]);
				NewMatrix[1] = (TTMatrix[0] * TempMatrix[1]) +
								(TTMatrix[1] * TempMatrix[3]);
				NewMatrix[2] = (TTMatrix[2] * TempMatrix[0]) +
								(TTMatrix[3] * TempMatrix[2]);
				NewMatrix[3] = (TTMatrix[2] * TempMatrix[1]) +
								(TTMatrix[3] * TempMatrix[3]);
			}

			currentLocation = TableTell();  /* save our spot */

			if(!GlyphsInMemory[ifontq])
				FinishTable();          /* remove table if all glyphs not in memory */

			if(!GlyphMetricsSet)
			{
				/* if metrics haven't been set, make sure the metrics are of the widest glyph */

				if(TTMetrics[ifontq][nextGlyph].advanceWidth >
					TTMetrics[ifontq][GlyphMetrics].advanceWidth)
				{
					GlyphMetrics = nextGlyph;
				}
			}

			/* load in the component glyph */

			GetSingleGlyph(file, numPoints, points, nextGlyph, *numPoints, &TTtemptop, &TTtempbottom,
								(BYTE) (flags & ARGS_ARE_XY_VALUES), argument1, argument2, NewMatrix, TTMatrix, Width_Offset);

			if(flags & USE_MY_METRICS)
			{
				GlyphMetrics = nextGlyph;       /* set metrics to current glyph */
				GlyphMetricsSet = 1;            /* metrics have been officially set */
			}

		} while(flags & MORE_COMPONENTS);       /* until we're out of components */
    }
    else        
	/* get the simple glyph */
		GetSingleGlyph(file, numPoints, points, GlyphID, 0, &TTtemptop, &TTtempbottom, 1, 0, 0, TTMatrix, TTMatrix, Width_Offset);

	/* calculate the advance width, including the widening factor and extra spacing */
	if(const_spaceq)
	{
		*advanceWidth = constwidthq * Scale *
							((widenq) ? (widfacq) : (1.0f)) + charspcq;
	}
	else
	{
		*advanceWidth = TTMetrics[ifontq][GlyphMetrics].advanceWidth * Scale *
							((widenq) ? (widfacq) : (1.0f)) + charspcq;
	}

	/* if the character is skewed, add enough to move past the top peice */

	if(skewq)
		*advanceWidth += TTtemptop * skewfacq;

	/* set the top and bottom extents of the character if needed */

	if(scrlevel <= 0) {
		if((bsavfq + abs(TTtempbottom)) > *bottom)
			*bottom = bsavfq + abs(TTtempbottom);

		if((tsavfq + (int) newheight + TTtemptop) > *top)
			*top = tsavfq + (int) newheight + TTtemptop;
	}

	if(!GlyphsInMemory[ifontq])     /* close the font file if we opened it */
	fclose(file);
}

/**************************************************************************/
/* This routine loads a single, simple glyph into the point array. It     */
/*  will load the glyph from disk if the glyphs are not in memory. The    */
/*  currentPoint parameter allows the routine to build composite glyphs   */
/*  appending the current glyph to the last one, starting at currentPoint.*/ 
void GetSingleGlyph(FILE *file, USHORT *numPoints, struct TTPoint *points, USHORT GlyphID,
						USHORT currentPoint, int *top, int *bottom, BYTE offsets, TTSHORT matchedPoint1,
						TTSHORT matchedPoint2, float *NewMatrix, float *TTMatrix, float Width_Offset)
{
	ULONG GlyphOffset, GlyphLength;
	TTSHORT CurrentX, CurrentY;
	BYTE repeatCount, repeatNumber, repeatedFlag;
	TTSHORT numberOfContours, sindex;
	USHORT *endPtsOfContours, index;
	float ScaledOffsetX, ScaledOffsetY;
	TTSHORT OffsetX, OffsetY;

	G_PROCESS_ID

	GlyphOffset = TTLoc[ifontq][GlyphID];                                   /* calculate glyph offset and length */
    GlyphLength = TTLoc[ifontq][GlyphID + 1] - GlyphOffset;

    if(GlyphsInMemory[ifontq])
		Tableseek(GlyphOffset, 0);      /* locate the beginning of glyph */
	else
    {
		if(!LoadTable(file, GlyphOffset + GlyphTableOffset[ifontq], GlyphLength))       /* load glyph */
			return;
	}

    numberOfContours = GetTableSHORT(); /* get the number of contours */

	Tableseek(4 * sizeof(FWord), 1);        /* skip bounding box */

	/* allocate a scratch array to store the endpoints of the contours */

	endPtsOfContours = (USHORT *) gpcalloc(numberOfContours, sizeof(USHORT));

	if(endPtsOfContours == NULL)
	{
		messageq(TT_WARN, TT_WARNMEMORY);       /* bail if no memory */
		return;
	}

	/* read in the endpoints and decide how many points in glyph */

	for(sindex = 0; sindex < numberOfContours; sindex++)
	{
		if(((endPtsOfContours[sindex] = GetTableUSHORT()) + currentPoint)
			> *numPoints)
			*numPoints = (USHORT)(endPtsOfContours[sindex] + currentPoint);
	}

	(*numPoints)++; /* actual number of points in glyph is the last endpoint */

	Tableseek((long) GetTableUSHORT(), 1);  /* skip instructions */

	repeatCount = 0;                /* setup to read flags */
	index = currentPoint;

	while(index < *numPoints)       /* one flag for each point */
	{
		points[index].flags = repeatedFlag = GetTableBYTE();    /* get a flag */
		index++;                                                                                                /* index next point */

		if(points[index - 1].flags & REPEATFLAG)        /* if the flag is repeated */             
		{
			repeatNumber = GetTableBYTE();                  /* find how many times to repeat */

			for(repeatCount = 0; repeatCount < repeatNumber; repeatCount++)
			{
				points[index].flags = repeatedFlag;     /* give point the repeated flags */
				index++;                                                        /* index next point */
			}
		}
	}

	CurrentX = (SHORT) Width_Offset;
	CurrentY = 0;   /* first point if relative to origin */

	for(index = currentPoint; index < *numPoints; index++)
	{
		if(!(points[index].flags & XISSHORT) && (points[index].flags & XISSAME))
		{
			points[index].x = CurrentX;     /* X value doesn't change from last X */

			continue;
		}

		if(points[index].flags & XISSHORT)
		{
			/* relative move is expressed as a unsigned byte */

			points[index].x = (TTSHORT) GetTableBYTE();

			/* set the sign of the move */

			if(!(points[index].flags & POSXSHORT))
				points[index].x *= (TTSHORT)-1;
		}
		else
		{
			/* relative move is a TTSHORT */

			points[index].x = GetTableSHORT();
		}

		/* update current point and make the stored point absolute */

		CurrentX = (points[index].x += CurrentX);
	}

	for(index = currentPoint; index < *numPoints; index++)
	{
		if(!(points[index].flags & YISSHORT) && (points[index].flags & YISSAME))
		{
			points[index].y = CurrentY;     /* Y value doesn't change from last Y */

			continue;
		}

		if(points[index].flags & YISSHORT)
		{
			/* relative move is expressed as a unsigned byte */

			points[index].y = (TTSHORT) GetTableBYTE();

			/* set the sign of the move */

			if(!(points[index].flags & POSYSHORT))
				points[index].y *= (TTSHORT)-1;
		}
		else
		{
			/* relative move is a TTSHORT */

			points[index].y = GetTableSHORT();
		}

		/* update current point and make the stored point absolute */

	   CurrentY = (points[index].y += CurrentY);
	}

	/* mark the endpoints as such */

	for(sindex = 0; sindex < numberOfContours; sindex++)
	{
		points[endPtsOfContours[sindex] + currentPoint].endpoint = 1;
	}

	/* we are finished with the scratch array */

	gpcfree((void **) &endPtsOfContours);

	if(!GlyphsInMemory[ifontq])
		FinishTable();  /* unload array if we loaded it */

	if(offsets)     /* calculate the offsets to be added to incoming glyph (support for comosite glyphs) */
	{
		OffsetX = matchedPoint1;        /* parameters are actual offsets */
		OffsetY = matchedPoint2;
	}
	else
	{
		OffsetX = (TTSHORT)(points[matchedPoint2].x - points[matchedPoint1].x);    /* parameters are points to be matched */       
		OffsetY = (TTSHORT)(points[matchedPoint2].y - points[matchedPoint1].y);
    }

	/* scale offsets by the base matrix */

	ScaledOffsetX = (OffsetX * TTMatrix[0]) +
						(OffsetY * TTMatrix[2]);
	ScaledOffsetY = (OffsetX * TTMatrix[1]) +
						(OffsetY * TTMatrix[3]);

	for(index = currentPoint; index < *numPoints; index++)
	{
		CurrentX = points[index].x;     /* original X and Y */
		CurrentY = points[index].y;

		/* scale x and y coordinates and add offsets to them */

		points[index].ScaledX = ((float)CurrentX * NewMatrix[0]) +
									((float)CurrentY * NewMatrix[2]) + ScaledOffsetX;
		points[index].ScaledY = ((float)CurrentX * NewMatrix[1]) +
									((float)CurrentY * NewMatrix[3]) + ScaledOffsetY;

		/* update extent counters */

		if((SHORT) (points[index].ScaledY) > *top)
			*top = (SHORT) (points[index].ScaledY);

		if((SHORT) (points[index].ScaledY) < *bottom)
			*bottom = (SHORT) (points[index].ScaledY);
	}
}

/**************************************************************************/
/* This routine uses the point array to draw the finished glyph. It draws */
/*  the straight lines itself and hands the q-splines off to a subroutine.*/
void OutputGlyph(USHORT *numPoints, struct TTPoint *points)
{
	USHORT index;
	int CurrentX, CurrentY, StartX, StartY;
    BYTE firstLineOfContour = 1;
	BYTE newContour = 1;

	G_PROCESS_ID

	if(fillq)       /* standard panel-fill setup code */
	{
		panfilq = 1;
		g_pq.nsides = 0;
		if (g_pq.xyp != NULL)
			gpcfree((void **) &g_pq.xyp);
		g_pq.xyp = (PGXYPAIRS)gpcalloc(64,
			sizeof(struct GXYPAIRSQ));
		if (g_pq.xyp == (void *)NULL) {
			messageq(WARNQ, 2);             /* Out of Memory */
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
			messageq(WARNQ, 2);             /* Out of Memory */
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

	if(thickq)
		tline(tfontq);

	CurrentX = (int) points[0].ScaledX;   /* set current position to the first point of glyph */
	CurrentY = (int) points[0].ScaledY;

	StartX = (int) xbf;
	StartY = (int) ybf;

	for(index = 0; index < *numPoints; index++)     /* for each point in glyph */
	{
		if(newContour)  /* if we are starting a new contour... */
		{
			LastContourX = (int) points[index].ScaledX;   /* save our spot */
			LastContourY = (int) points[index].ScaledY;

			/* move to new spot (penup) */

			lmoveq((int) (StartX + LastContourX),
					(int) (StartY + LastContourY));
			newContour = 0; /* no longer starting a new contour */
		}

		if(points[index].flags & ONCURVE)
		{
		/* ONCURVE point indicates straight line segment from last point */

			firstLineOfContour = 0; /* no longer the first line */

			CurrentX = (int) points[index].ScaledX;       /* reset poition */
			CurrentY = (int) points[index].ScaledY;

			if(!panfilq || wplane_flgq != 0)
			{
				/* draw a line to it, no panel fill */

				ldrawq((int) (StartX + CurrentX),
							 (int) (StartY + CurrentY));

			}
			else
			{
				/* save position for panel filling code */

				g_pq.xyp[g_pq.nsides].x = (int) (StartX + CurrentX);
				g_pq.xyp[g_pq.nsides].y = (int) (StartY + CurrentY);
				g_pq.nsides = newbufsq(&g_pq.xyp, g_pq.nsides);
			}
		}
		else
		{
			/* it's a curved section, give it to the q-spline handler and reset the current point */

			index = (USHORT)HandleQSpline(numPoints, points, index, firstLineOfContour);

			CurrentX = (int) points[index - 1].ScaledX;   /* index is the last point of the curve */
			CurrentY = (int) points[index - 1].ScaledY;

			firstLineOfContour = 0; /* no longer first line in contour */
		}

		if(points[index].endpoint)      /* if we finished a contour... */
		{
			firstLineOfContour = 1; /* next line is the first line of contour */

			/* close contour to first point */

			if(!panfilq || wplane_flgq != 0)
			{
				ldrawq((int) (StartX + LastContourX),
							 (int) (StartY + LastContourY));

			}
			else
			{
				g_pq.xyp[g_pq.nsides].x = (int) (StartX + LastContourX);
				g_pq.xyp[g_pq.nsides].y = (int) (StartY + LastContourY);
				g_pq.nsides = newbufsq(&g_pq.xyp, g_pq.nsides);
			}

			/* if there is more than one side, save panel */

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

				g_pq.xyp = (PGXYPAIRS)gpcalloc(64,
					sizeof(struct GXYPAIRSQ));
				if (g_pq.xyp == (void *)NULL) {
					messageq(WARNQ, 2);             /* Out of Memory */
					panfilq = 0;
				}
			}
			newContour = 1; /* start a new contour */
		}
	}       /* end of for() loop */

	if(panfilq)     /* fill the panels if we're supposed to */
	{
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

	panfilq = 0;
}
				
/**************************************************************************/
/* This routine interprets a series of points that are off the curve and  */
/*  calculates the control points to be handed to the q-spline drawing    */
/*  routine. If the final point is an endpoint, this routine completes    */
/*  contour using the beginning point of the contour as the final control */
/*  point. If the first point in a contour is off the curve, this routine */
/*  finds the endpoint of the contour and uses it for the starting point. */
int HandleQSpline(USHORT *numPoints, struct TTPoint *points, USHORT firstIndex, BYTE newContour)
{
	int CurrentX, CurrentY;
	float qSplineAX, qSplineAY, qSplineBX, qSplineBY, qSplineCX, qSplineCY;
 
	USHORT index;
	TTCHAR Endpoint = 0;    /* we haven't hit an endpoint yet */

	G_PROCESS_ID

	/* get our bearings- we are called on the first OFF curve point */
	/*  and we need the first control point to be ON the curve */

	index = (USHORT)(firstIndex - 1);

	if(newContour)  /* if this is the first line on the new contour... */
	{
		for(index = firstIndex; index < *numPoints; index++)    /* find the endpoint of the contour */
		{
			if(points[index].endpoint)
				break;
	}       /* must have at least one more endpoint or we have an invalid font */

		CurrentX = (int) points[index].ScaledX;       /* get our starting point */
		CurrentY = (int) points[index].ScaledY;

		index = firstIndex;     /* point to second point in curve */

		LastContourX = CurrentX;        /* save our place */
		LastContourY = CurrentY;
	}
	else
	{
		CurrentX = (int) points[index].ScaledX;       /* load the starting point */
		CurrentY = (int) points[index].ScaledY;
		index++;                                                        /* point to next point */
	}

	/* while we haven't hit an endpoint, have points, and are still off the curve... */

	while(!Endpoint && index < (*numPoints) && !(points[index].flags & ONCURVE))
	{
		qSplineAX = CurrentX;   /* 1st control point */
		qSplineAY = CurrentY;

		qSplineBX = points[index].ScaledX;      /* second control point */
		qSplineBY = points[index].ScaledY;

		index++;        /* bump to next point */

		if(index >= (*numPoints) || (points[index - 1].endpoint))
		{
			qSplineCX = LastContourX;       /* we hit an endpoint */
			qSplineCY = LastContourY;       /* so loop around to starting point */
			Endpoint = 1;                           /* mark it */
			index--;                                        /* and give the handler in OutputGlyph room to work */
		}
		else if(points[index].flags & ONCURVE)
		{
			qSplineCX = points[index].ScaledX;      /* this point is the last one in the curve */
			qSplineCY = points[index].ScaledY;
		}
		else
		{       /* interpolate the point to end at, we still have more points in the curve */
			qSplineCX = (points[index].ScaledX + qSplineBX) / 2;
			qSplineCY = (points[index].ScaledY + qSplineBY) / 2;
		}

		CurrentX = (int) qSplineCX;   /* save our spot for next iteration */
		CurrentY = (int) qSplineCY;

		/* draw the curve */

		DrawQSpline(qSplineAX, qSplineAY, qSplineBX, qSplineBY, qSplineCX, qSplineCY);
	}

	return index;   /* return the spot for OutputGlyph */
}

/**************************************************************************/
/* DDA routine to quickly put a q-spline into TKF.                        */
void DrawQSpline(float qAX, float qAY, float qBX, float qBY, float qCX, float qCY)
{
	float AX, AY, BX, BY;
	float deltaX1, deltaX2, deltaY1, deltaY2;
	float X, Y, OldX, OldY, NewX, NewY;
	float Error;
	int N, step;

	G_PROCESS_ID

	/* derive the coefficients of the q-spline curve equations */

	AX = qAX - (qBX * 2) + qCX;
	AY = qAY - (qBY * 2) + qCY;

	BX = (qBX - qAX) * 2;
	BY = (qBY - qAY) * 2;

	deltaX1 = AX + BX;      /* calculate 1st forward difference of X */
	deltaX2 = AX * 2;       /* calculate 2nd forward difference */

	deltaY1 = AY + BY;  /* calculate 1st forward difference of Y */
	deltaY2 = AY * 2;  /* calculate 2nd forward difference */

	X = qAX;        /* get our starting point */
	Y = qAY;

	/* error = ((step size) ^ 2) * |a| / 8 */
	/* step size is 1 and so drops out */
	/* |a| = (1st coefficient) ^ 2 / 2 */

	Error = (AX * AX + AY * AY) / 16;

	for(N = 1; Error > TTMaxError[ifontq]; N *= 2)
	{
		deltaX2 /= 4;                                           /* subdivide forward differences */
		deltaX1 = (deltaX1 - deltaX2) / 2;

		deltaY2 /= 4;
		deltaY1 = (deltaY1 - deltaY2) / 2;

		/* each division increases step size * 2 and decreases error by factor of 4 */

		Error /= 4;
	}

	/* setup Current position counter */

	OldX = X + 0.5f + xbf;
	OldY = Y + 0.5f + ybf;

	for(step = 0; step < N; step++)
	{
		X += deltaX1;                   /* add differences to get next location */
		deltaX1 += deltaX2;             /* update differences for next iteration */

		Y += deltaY1;
		deltaY1 += deltaY2;

		/* calculate new position */

		NewX = X + 0.5f + xbf;
		NewY = Y + 0.5f + ybf;

		if(NewY == OldY && NewX == OldX)        /* ignore it if it's the same as the last one */
			continue;

		OldX = NewX;    /* save it otherwise */
		OldY = NewY;

		/* draw the line to new position */

		if(!panfilq || wplane_flgq != 0)
		{
			ldrawq((int) NewX, (int) NewY);
		}
		else
		{
			g_pq.xyp[g_pq.nsides].x = (int) NewX;
			g_pq.xyp[g_pq.nsides].y = (int) NewY;
			g_pq.nsides = newbufsq(&g_pq.xyp, g_pq.nsides);
		}
	}
}

/**************************************************************************/
/* This routine finds the amount of kerning required for the given characters */
float GetTTKerningWidth(USHORT leftChar, USHORT rightChar)
{
	FWord value;
	float Scale;
	USHORT left, right;

	G_PROCESS_ID

	left = MapCharToGlyph(leftChar);        /* get left glyph */
	right = MapCharToGlyph(rightChar);      /* get right glyph */

	if(KernTable[ifontq] == NULL)
		value = GetTTKernDisk(left, right);
	else
		value = GetTTKernMemory(left, right);

	if(value == 0)  /* no kerning */
		return (float) 0;

	Scale = heightq * fnthtq * ((TTfullq[ifontq]) ? (TTFudgeFactor[ifontq]) : (1))
				/ unitsPerEm[ifontq];

	return Scale * value;   /* scale kerning amount */
}

/**************************************************************************/
/* This routine gets kerning info from the disk                                                   */
FWord GetTTKernDisk(USHORT left, USHORT right)
{
	FILE *file;
	ULONG offset, length, Dummy;
	USHORT index, index2, nTables, coverage, nPairs;
	USHORT SearchLeft, SearchRight;
	FWord value;

	G_PROCESS_ID

	file = gfopen(fntptr[ifontq], "rb", 0); /* open the file */

	/* find the table */
	if(!FindTable(file, (ULONG) TABLENAME('k', 'e', 'r', 'n'), &offset, &length))
	{
		fclose(file);   /* no table, no kerning */
		return (FWord) 0;
	}

	fseek(file, offset, 0); /* go to the table */

	Dummy = (ULONG) GetUSHORT(file);        /* skip version */
	nTables = GetUSHORT(file);      /* get the number of tables */

	value = 0;      /* amount of kerning */

	for(index = 0; index < nTables; index++)        /* for each table... */
	{
		Dummy = (ULONG) GetUSHORT(file);                /* skip version */
		length = GetUSHORT(file);                               /* get length */
		coverage = GetUSHORT(file);                             /* get info flags */

		if((coverage & 0xFF04) || !(coverage & 0x1))    /* if not format 0 or horizontal... */
		{
			fseek(file, length - 3 * sizeof(USHORT), 1);    /* skip table */
			continue;
		}

		nPairs = GetUSHORT(file);                       /* get the number of kerning pairs */
		Dummy = (ULONG) GetUSHORT(file);        /* skip search parameters */
		Dummy = (ULONG) GetUSHORT(file);
		Dummy = (ULONG) GetUSHORT(file);

		for(index2 = 0; index2 < nPairs; index2++)
		{
			SearchLeft = GetUSHORT(file);   /* get left and right glyphs */
			SearchRight = GetUSHORT(file);

			if(SearchLeft == left && SearchRight == right)  /* are they the ones we want? */
			{
				if(coverage & 0x8)                              /* if this is an override table */
				{
					value = GetFWord(file);         /* the value replaces what we have */
					continue;
				}

				if(coverage & 0x2)                              /* if it's a minimum table */
				{
					Dummy = (ULONG) GetFWord(file);

					if(value < (FWord) Dummy)
						value = (FWord) Dummy;  /* value replaces only if accumilated kerning is lower */
				}
				else
				{
					value += GetFWord(file);        /* accumilate kerning motion */
				}
			}
			else
				Dummy = (ULONG) GetFWord(file); /* skip the kerning amount- not our pair of glyphs */
		}
	}

	fclose(file);

	return value;
}

/**************************************************************************/
/* This routine gets kerning info from the memory                                                 */
FWord GetTTKernMemory(USHORT left, USHORT right)
{
	USHORT length;
	USHORT index, index2, nTables, coverage, nPairs;
	USHORT SearchLeft, SearchRight;
	FWord value;
	ULONG Dummy;

	G_PROCESS_ID

	SetTable(KernTable[ifontq]);

	Dummy = (ULONG) GetTableUSHORT();       /* skip version */
	nTables = GetTableUSHORT();     /* get the number of tables */

	value = 0;      /* amount of kerning */

	for(index = 0; index < nTables; index++)        /* for each table... */
	{
		Dummy = (ULONG) GetTableUSHORT();                       /* skip version */
		length = GetTableUSHORT();                                      /* get length */
		coverage = GetTableUSHORT();                            /* get info flags */

		if((coverage & 0xFF04) || !(coverage & 0x1))    /* if not format 0 or horizontal... */
		{
			Tableseek(length - 3 * sizeof(USHORT), 1);      /* skip table */
			continue;
		}

		nPairs = GetTableUSHORT();      /* get the number of kerning pairs */

		Tableseek(3 * sizeof(USHORT), 1);

		for(index2 = 0; index2 < nPairs; index2++)
		{
			SearchLeft = GetTableUSHORT();  /* get left and right glyphs */
			SearchRight = GetTableUSHORT();

			if(SearchLeft == left && SearchRight == right)  /* are they the ones we want? */
			{
				if(coverage & 0x8)                              /* if this is an override table */
				{
					value = GetTableFWord();                /* the value replaces what we have */
					continue;
				}

				if(coverage & 0x2)                              /* if it's a minimum table */
				{
					Dummy = (ULONG) GetTableFWord();

					if(value < (FWord) Dummy)
						value = (FWord) Dummy;  /* value replaces only if accumilated kerning is lower */
				}
				else
				{
					value += GetTableFWord();       /* accumilate kerning motion */
				}
			}
			else
				Dummy = (ULONG) GetTableFWord();        /* skip the kerning amount- not our pair of glyphs */
		}
	}

	return value;
}

/**************************************************************************/
/* This routine attempts to load a character map from the font file. Only */
/* formats 0 and 4 are supported.                                                                                 */
int GetCharMap(FILE *fontfile, ULONG offset, ULONG length, int number)
{
	USHORT numEncTables;
	TTSHORT index, Format4table, nextTable;
	ULONG cmapLength;

	struct cmapTable
	{
		USHORT PlatformID, EncID;
		ULONG cmapOffset;
	} *tables;

	if(!LoadTable(fontfile, offset, length))/* load table for speed */
		return TTOUTOFMEMORY;

	GetTableUSHORT();                                               /* skip version */

	numEncTables = GetTableUSHORT();                /* get number of tables */

	/* allocate memory for the information structures */

	tables = (struct cmapTable *) gpcalloc(numEncTables, sizeof(struct cmapTable));

	if(tables == NULL)
		return TTOUTOFMEMORY;   /* abort if no memory */

	Format4table = -1;      /* tables haven't been found */
	nextTable = -1;

	for(index = 0; index < (TTSHORT) numEncTables; index++) /* load table info */
	{
		tables[index].PlatformID = GetTableUSHORT();
		tables[index].EncID = GetTableUSHORT();
		tables[index].cmapOffset = GetTableULONG();

		/* Prefer MicroSoft Unicode BMP (Platform 3, Enc 1) */
		if(tables[index].PlatformID == 3 && tables[index].EncID == 1)
			Format4table = index;
		/* Fallback to Apple Unicode (Platform 0) */
		else if(Format4table == -1 && tables[index].PlatformID == 0)
			Format4table = index;
	}

	if(Format4table != -1)  /* did we find Unicode format 4? */
	{
		/* yes, proceed... */

		for(index = 0; index < (TTSHORT) numEncTables; index++) /* find next table */
		{
			if(index == Format4table)
				continue;

			if(tables[index].cmapOffset > tables[Format4table].cmapOffset)
			{
				if(nextTable == -1)
				{
					nextTable = index;
					continue;
				}

				if(tables[index].cmapOffset < tables[nextTable].cmapOffset)
					nextTable = index;
			}
		}

		if(nextTable == -1)
		{
			cmapLength = length - tables[Format4table].cmapOffset;
		}
		else
		{
			cmapLength = tables[nextTable].cmapOffset - tables[Format4table].cmapOffset;
		}

		/* attempt to load the mapping table */

		if(!HandleFormat4(number, tables[Format4table].cmapOffset, cmapLength))
		{
			FinishTable();
			gpcfree((void **) &tables);
			return TTOUTOFMEMORY;
		}
	}
	else
	{
		/* no Unicode table, try Format 0 mapping */

		for(index = 0; index < (TTSHORT) numEncTables; index++)
		{
			Tableseek(tables[index].cmapOffset, 0);

			if(GetTableUSHORT() == 0)
				nextTable = index;
		}

		if(nextTable == -1)     /* no table present */
		{
			FinishTable();
			gpcfree((void **) &tables);
			return TTINVALIDFONT;
		}

		/* attempt to load format 0 table */

		if(!HandleFormat0(number, tables[nextTable].cmapOffset))
		{
			FinishTable();
			gpcfree((void **) &tables);
			return TTOUTOFMEMORY;
		}
	}

	/* clean-up and exit */

	FinishTable();
	gpcfree((void **) &tables);

	return TTOK;
}

/**************************************************************************/
/* This routine loads a format 0 character map. This is a simple 256 byte array. */
int HandleFormat0(int number, ULONG offset)
{
	int index;

	G_PROCESS_ID

	Tableseek(offset + 3 * sizeof(USHORT), 0);      /* Skip header info */

	/* allocate room for character table */

	TTCharMap[number].charmap = (USHORT *) gpcalloc(256, sizeof(USHORT));

	/* return error if no memory */

	if(TTCharMap[number].charmap == NULL)
		return FALSE;
	

	for(index = 0; index < 256; index++)    /* load in mapping table */
	{
		TTCharMap[number].charmap[index] = (USHORT) GetTableBYTE();
	}

	TTCharMap[number].format = 0;   /* set format for future reference */

	return TRUE;
}

/**************************************************************************/
/* Load in a format 4 table. This table maps chars to glyphs using segment */
/*  mapping to a delta value.                                                                                      */
int HandleFormat4(int number, ULONG offset, ULONG length)
{
	unsigned int index, numWords;
	USHORT sub_len;

	G_PROCESS_ID

	Tableseek(offset + sizeof(USHORT), 0); /* skip format (4) */
	sub_len = GetTableUSHORT();            /* get subtable length in bytes */
	GetTableUSHORT();                      /* skip language */

	TTCharMap[number].segCount = (USHORT)(GetTableUSHORT() >> 1);     /* get the segment count times two from memory and */
														/*  store actual segment count */

	GetTableUSHORT();       /* skip search parameters */
	GetTableUSHORT();
	GetTableUSHORT();

	if (sub_len > 14)
		numWords = (sub_len - 14) / sizeof(USHORT);
	else
		numWords = (length > 14) ? (length - 14) / sizeof(USHORT) : 256;

	/* allocate memory for map */
	TTCharMap[number].charmap = (USHORT *) gpcalloc((size_t)numWords + 32, sizeof(USHORT));

	/* return error if there is no memory */
	if(TTCharMap[number].charmap == NULL)
		return FALSE;
    
	/* load in mapping table */
	for(index = 0; index < numWords; index++)
	{
		TTCharMap[number].charmap[index] = GetTableUSHORT();
	}

	/* setup pointers into the different sections of the table */

	/* ending values of each segment */
	TTCharMap[number].endCount = (USHORT *) &TTCharMap[number].charmap[0];

    /* starting values of each segment */
	TTCharMap[number].startCount = (USHORT *) &TTCharMap[number].charmap[TTCharMap[number].segCount + 1];

	/* char to glpyh mapping values */
	TTCharMap[number].idDelta = (USHORT *) &TTCharMap[number].charmap[2 * TTCharMap[number].segCount + 1];
	TTCharMap[number].idRangeOffset = (USHORT *) &TTCharMap[number].charmap[3 * TTCharMap[number].segCount + 1];
	TTCharMap[number].glyphIdArray = (USHORT *) &TTCharMap[number].charmap[4 * TTCharMap[number].segCount + 1];

	/* save format for future reference */
	TTCharMap[number].format = 4;

	return TRUE;
}
/**************************************************************************/
void SetTTKerning(char onoff, char string)
{
	int i, n = 1000;
	G_PROCESS_ID

	for (i = 0; i < FNTNUM; i++) {
		if (escsym[i] == (Uchar) string)
			n = i;
	}

	if(n == 1000)
		return;

	TTkernq[n] = onoff;
}
/**************************************************************************/
void SetTTFull(char onoff, char string)
{
	int i, n = 1000;
	G_PROCESS_ID

	for (i = 0; i < FNTNUM; i++) {
		if (escsym[i] == (Uchar) string)
			n = i;
	}

	if(n == 1000)
		return;

	TTfullq[n] = onoff;
}
/**************************************************************************/
void SetTTUseMemory(char onoff)
{
	G_PROCESS_ID

	TTStoreq = onoff;
}
/**************************************************************************/
int GetTTCharWidth(int ifont, unsigned char char_num)
{
	USHORT GlyphID;

	G_PROCESS_ID

	ifontq = ifont;

	GlyphID = (USHORT)OurASCIItoUnicode(char_num + 32);
	GlyphID = MapCharToGlyph(GlyphID);

	return TTMetrics[ifont][GlyphID].advanceWidth;
}
/**************************************************************************/
void GetTTWidest(int ifont, int *w, unsigned char *char_num)
{
	int i;
	USHORT GlyphID;

	G_PROCESS_ID

	ifontq = ifont;

	for(i = 0; i < 168; i++)
	{
		GlyphID = (USHORT)OurASCIItoUnicode(i + 32);
		GlyphID = MapCharToGlyph(GlyphID);

		if((int) TTMetrics[ifont][GlyphID].advanceWidth > *w)
		{
			*w = TTMetrics[ifont][GlyphID].advanceWidth;
			*char_num = (char) (i + 32);
		}
	}
}
/**************************************************************************/
int LoadTranslateTable(void)
{
	int i;
	short *temp;
	FILE *f;

	G_PROCESS_ID

	if (TranslateASCII == NULL) {
		TranslateASCII = (int *)gpcalloc(sizeof(int), 128);
		if (TranslateASCII == NULL) {
			messageq(TT_ERR, TT_ERRMEMORY);
			return 0;
		}

		/* Default 1:1 identity mapping for ASCII 128..255 (maps to Latin-1 Unicode U+0080..U+00FF) */
		for (i = 0; i < 128; i++)
			TranslateASCII[i] = 128 + i;

		/* If custom unicode.ttf mapping table exists, load it */
		f = gfopen("unicode.ttf", "rb", 0);
		if (f != NULL) {
			temp = (short *)gpcalloc(sizeof(short), 128);
			if (temp != NULL) {
				if (fread(temp, sizeof(short), 128, f) == 128) {
					for (i = 0; i < 128; i++)
						TranslateASCII[i] = (int)temp[i];
				}
				gpcfree((void **) &temp);
			}
			fclose(f);
		}
	}

	return 1;
}
/**************************************************************************/
void UnLoadTranslateTable(void)
{
	G_PROCESS_ID

	gpcfree((void **) &TranslateASCII);
	TranslateASCII = NULL;
}
/**************************************************************************/
void SetTTMaxError(float newerror, char string)
{
	int i, n = 1000;
	G_PROCESS_ID

	for (i = 0; i < FNTNUM; i++) {
		if (escsym[i] == (Uchar) string)
			n = i;
	}

	if(n == 1000)
		return;

	TTMaxError[n] = newerror;
	TTMaxError[n] *= TTMaxError[n];
}
/**************************************************************************/
char *GetFontName(char string)
{
	int i, n = 1000;
	G_PROCESS_ID

	for (i = 0; i < FNTNUM; i++) {
		if (escsym[i] == (Uchar) string)
			n = i;
	}

	if(n == 1000)
		return NULL;

	if(FontTypeq[n] == REGULARFONT || FontName[n] == NULL)
		return fntptr[n];

	return FontName[n];
}
