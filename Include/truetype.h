#ifndef TRUETYPE_H
#define TRUETYPE_H
/* (c) 1984-1993 by Scientific Endeavors Corporation.
   All rights reserved.
Version 7.0 PostScript/Truetype extensions written by David Dillow.
**************************************************************/

#include <stdint.h>

#define BYTE uint8_t				/* define our own types */
#define TTCHAR int8_t
#define USHORT uint16_t
#define TTSHORT int16_t
#define ULONG uint32_t
#define TTLONG int32_t
#define Fixed int32_t
#define FWord int16_t
#define uFWord uint16_t
#define F2Dot14 int16_t
#define F26Dot6 int32_t
#define HFixed int32_t

/* macros to conver from one type to another */

#define HFixedToFloat(x) ((float) ((float)(x) / (1L << 19)))
#define FloatToHFixed(x) ((HFixed) ((x) * (1L << 19)))
#define F26Dot6ToHFixed(x) ((HFixed) ((x) << 13))
#define SHORTToHFixed(x) ((HFixed) ((HFixed) (x) << 19))
#define HFixedToSHORT(x) ((TTSHORT) ((x) >> 19))

#define FixedToFloat(x) ((float) ((float)(x) / (1L << 16)))
#define FloatToFixed(x) ((Fixed) ((x) * (1L << 16)))
#define F2Dot14ToFloat(x) ((float) ((float)(x) / (1 << 14)))
#define FloatToF2Dot14(x) ((F2Dot14) ((x) * (1 << 14)))
#define F26Dot6ToFloat(x) ((float) ((float)(x) / (1 << 6)))
#define FloatToF26Dot6(x) ((F26Dot6) ((x) * (1 << 6)))

#define FixedToF26Dot6(x) ((F26Dot6) ((x) >> 10))
#define F26Dot6ToFixed(x) ((Fixed) ((x) << 10))

#define SHORTToF26Dot6(x) ((F26Dot6) ((TTLONG)(x) << 6))
#define F26Dot6ToSHORT(x) ((TTSHORT) ((TTLONG)(x) >> 6))

#define SHORTToFixed(x) ((Fixed) ((TTLONG)(x) << 16))
#define FixedToSHORT(x) ((TTSHORT) ((TTLONG)(x) >> 16))

#define F2Dot14ToF26Dot6(x) ((Fixed) ((TTLONG)(x) >> 8))
#define F26Dot6ToF2Dot14(x) ((F2Dot14) ((TTLONG)(x) << 8))

/* macros to read in the different types from disk */

#define GetBYTE(file) Get8Bits(file)
#define GetCHAR(file) (TTCHAR) Get8Bits(file)
#define GetUSHORT(file) Get16Bits(file)
#define GetSHORT(file) (TTSHORT) Get16Bits(file)
#define GetULONG(file) Get32Bits(file)
#define GetTTLONG(file) (TTLONG) Get32Bits(file)
#define GetFixed(file) (Fixed) Get32Bits(file)
#define GetFWord(file) (FWord) Get16Bits(file)
#define GetuFWord(file) (uFWord) Get16Bits(file)
#define GetF2Dot14(file) (F2Dot14) Get16Bits(file)

/* macros to retrieve the types from memory */

#define GetTableBYTE Get8Table
#define GetTableCHAR (TTCHAR) Get8Table
#define GetTableUSHORT Get16Table
#define GetTableSHORT (TTSHORT) Get16Table
#define GetTableULONG Get32Table
#define GetTableTTLONG (TTLONG) Get32Table
#define GetTableFixed (Fixed) Get32Table
#define GetTableFWord (FWord) Get16Table
#define GetTableuFWord (uFWord) Get16Table
#define GetTableF2Dot14 (F2Dot14) Get16Table

/* utility macro to convert 4 char arguments to a ULONG */

#define TABLENAME(a,b,c,d) (((ULONG)(uint8_t)(a) << 24) | ((ULONG)(uint8_t)(b) << 16) | ((ULONG)(uint8_t)(c) << 8) | ((ULONG)(uint8_t)(d)))

/* TrueType return codes */

#define TTOK 1
#define TTOUTOFMEMORY 2
#define TTINVALIDFONT 3
#define TTBADNOMESSAGE 4

/* TrueType Message Codes */

#define TT_ERR 'T'
#define TT_WARN 't'

#define TT_ERRMEMORY 1
#define TT_ERRINVALID 2
#define TT_WARNMEMORY 3
#define TT_NOTRANSLATE 4
#define TT_BADTRANSLATE 5
#define TT_NOCLOSETRANSLATE 6	/* warning */

/* Point definition flags */

#define ONCURVE 1
#define XISSHORT 2
#define YISSHORT 4
#define REPEATFLAG 8
#define XISSAME 16
#define POSXSHORT 16
#define YISSAME 32
#define POSYSHORT 32

/* composite glyph definition flags */

#define ARG_1_AND_2_ARE_WORDS 1
#define ARGS_ARE_XY_VALUES 2
#define ROUND_XY_TO_GRID 4
#define WE_HAVE_A_SCALE 8
#define TT_COMPOSITE_RESERVED 16
#define MORE_COMPONENTS 32
#define WE_HAVE_AN_X_AND_Y_SCALE 64
#define WE_HAVE_A_TWO_BY_TWO 128
#define WE_HAVE_INSTRUCTIONS 256
#define USE_MY_METRICS 512 

/* struct to describe a point */

struct TTPoint
{
	TTSHORT x, y;
	float ScaledX, ScaledY;
	BYTE flags, endpoint;
};

/* structure to hold info on the mapping tables */

struct cmapFormat4
{
	BYTE format;
	USHORT segCount;
	USHORT mapsize;
	USHORT *endCount;
	USHORT *startCount;
	USHORT *idDelta;
	USHORT *idRangeOffset;
	USHORT *glyphIdArray; 

	USHORT *charmap;
};

/* struct to hold info on character metrics */

struct longHorMetric
{
	uFWord advanceWidth;
	FWord lsb;
};

/* function prototypes */

int FindTable(FILE *file, ULONG identifier, ULONG *offset, ULONG *length);
int LoadTrueTypeFont(int number, char *filename);
BYTE Get8Bits(FILE *file);
USHORT Get16Bits(FILE *file);
ULONG Get32Bits(FILE *file);
void GetSingleGlyph(FILE *file, USHORT *numPoints, struct TTPoint *points, USHORT GlyphID,
						USHORT currentPoint, int *top, int *bottom, BYTE offsets, TTSHORT matchedPoint1, TTSHORT matchedPoint2, float *NewMatrix, float *TMatrix, float Width_Offset);
void OutputGlyph(USHORT *numPoints, struct TTPoint *points);
int HandleQSpline(USHORT *numPoints, struct TTPoint *points, USHORT firstIndex, BYTE newContour);
void GetGlyph(USHORT *numPoints, struct TTPoint *points, USHORT GlyphID, float *advanceWidth, int *top, int *bottom);
void DrawQSpline(float qAX, float qAY, float qBX, float qBY, float qCX, float qCY);
void TrueTypeOut(int lastChar, int Char, int action, float *width, int *top, int *bottom);
float GetTTKerningWidth(USHORT left, USHORT right);
void FreeTrueTypeMemory(int n);
int LoadTable(FILE *file, ULONG offset, ULONG length);
void FinishTable(void);
void Tableseek(TTLONG offset, USHORT direction);
BYTE Get8Table(void);
USHORT Get16Table(void);
ULONG Get32Table(void);
ULONG TableTell(void);
void SetTable(BYTE *newTable);
int GetCharMap(FILE *fontfile, ULONG offset, ULONG length, int number);
int HandleFormat0(int number, ULONG offset);
int HandleFormat4(int number, ULONG offset, ULONG length);
FWord GetTTKernDisk(USHORT left, USHORT right);
FWord GetTTKernMemory(USHORT left, USHORT right);
void SetTTforOS2(void);
void SetTTUseMemory(char onoff);
void SetTTFull(char onoff, char string);
void SetTTKerning(char onoff, char string);
int GetTTCharWidth(int ifont, unsigned char char_num);
void GetTTWidest(int ifont, int *w, unsigned char *char_num);
int LoadTranslateTable(void);
void SetTTMaxError(float newerror, char string);
void UnLoadTranslateTable(void);

#endif
