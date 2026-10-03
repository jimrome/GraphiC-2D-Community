/* (c) 1984-1993 by Scientific Endeavors Corporation.
   All rights reserved.
Version 7.0 PostScript/Truetype extensions written by David Dillow.
**************************************************************/

#define FNTNUM 4
#define PSHGT 20
#define REGULARFONT 0
#define PSFILLED 1
#define PSOUTLINE 2
#define TRUETYPE 3

struct GEN_FONT_DATA {
	char skew;			/* SKEWED CHARACTERS*/
	char widen;			/* CHANGE CHARACTER WIDTH*/
	char fill;			/* FLAG FOR FILLING FONTS*/
	int font_pattern;		/* FLAG FOR FILLING FONTS*/
	int font_border_color;/* FLAG FOR FILLING FONTS*/
	/* SPECIAL CHARACTERS */
	int flag;
	int ang;
	int scrlev;			/* super/subscript nesting level */
	int thick;
	int tfont;			/* WIDTH OF FONT LINE*/
	int ifont;			/* ACTIVE FONT NUMBER*/
	int fntht;			/* Pel height for the active font */
	int tsav;
	int bsav;
	char const_space;
	int constwidth;
	float height;
	int charspc;			/*PIXEL SPACING BETWEEN CHARACTERS*/
	float hgtwd;
	float sin;
	float cos;
	float xb;
	float yb;
	float w1;
	float newht;
	float skewfac;		/* SLANT FACTOR*/
	float widfac;			/* FACTOR FOR CHANGING CHARACTER WIDTH*/
	int txt_border_color;
	int TTCharIsUnicodeq;	/* Flag to indicate that next character is in Unicode */
};

#ifdef MULTITHREAD
extern struct GEN_FONT_DATA *GENERAL_dataq[GPC_MAX_THREADS];
#define gen_data (*GENERAL_dataq[instctr])
#else
extern struct GEN_FONT_DATA GENERAL_dataq;
#define gen_data GENERAL_dataq
#endif

#ifndef __INFNTPLT

#define skewq gen_data.skew
#define widenq gen_data.widen
#define fillq gen_data.fill
#define font_patternq gen_data.font_pattern
#define font_border_colorq gen_data.font_border_color
#define flagq gen_data.flag
#define angq gen_data.ang
#define scrlevel gen_data.scrlev
#define thickq gen_data.thick
#define tfontq gen_data.tfont
#define ifontq gen_data.ifont
#define fnthtq gen_data.fntht
#define tsavfq gen_data.tsav
#define bsavfq gen_data.bsav
#define const_spaceq gen_data.const_space
#define heightq gen_data.height
#define charspcq gen_data.charspc
#define hgtwdq gen_data.hgtwd
#define sinq gen_data.sin
#define cosq gen_data.cos
#define xbf gen_data.xb
#define ybf gen_data.yb
#define w1 gen_data.w1
#define newheight gen_data.newht
#define xfacq gen_data.xfac
#define yfacq gen_data.yfac
#define skewfacq gen_data.skewfac
#define widfacq gen_data.widfac
#define txt_border_color gen_data.txt_border_color
#define TTCharIsUnicodeq gen_data.TTCharIsUnicodeq
#define constwidthq gen_data.constwidth

#endif

struct PS_LOAD_DATA
{
int eexecq, lenIV;
unsigned short int eexec_keyq, eexec_countq;
};

#ifdef MULTITHREAD
#ifdef __PSFONT
struct PS_LOAD_DATA *PS_LOAD_dataq[GPC_MAX_THREADS];
#endif
#define psl_data (*PS_LOAD_dataq[instctr])
#else
#ifdef __PSFONT
struct PS_LOAD_DATA PS_LOAD_dataq;
#endif
#define psl_data PS_LOAD_dataq
#endif

#ifndef __INFNTPLT

#define eexecq psl_data.eexecq
#define lenIV psl_data.lenIV
#define eexec_keyq psl_data.eexec_keyq
#define eexec_countq psl_data.eexec_countq

#endif

#ifdef __TRUETYPE
struct TT_INFO
{
	/* These structures hold info on the character to glyph mapping and
		on the metrics (advancewidth) of each glyph */

	struct cmapFormat4 TTCharMap[FNTNUM];
	struct longHorMetric *TTMetrics[FNTNUM];

	float TTFudgeFactor[FNTNUM];				/* Multiply by this amount to make characters full height */
	int LastContourX, LastContourY;			/* Global storage of first point in current contour */
	USHORT unitsPerEm[FNTNUM];					/* Granularity of character grid (used to scale glyphs) */
	USHORT maxPoints[FNTNUM];					/* Maximum number of points in a Glyph */
	USHORT numGlyphs[FNTNUM];					/* Number of Glyphs in each font */
	ULONG *TTLoc[FNTNUM];					/* Index to glpyh definitions relative to glyph table */
	ULONG GlyphTableOffset[FNTNUM];				/* Offset of Glyph table in font file */
	TTCHAR GlyphsInMemory[FNTNUM];				/* Flag to indicate if Glyphs are in memory */
	BYTE *GlyphTable[FNTNUM];				/* Address of Glyph table, if loaded */
	BYTE *KernTable[FNTNUM];				/* Address of kerning table, if loaded */

	int TTkernq[FNTNUM];			/* Kerning switch - default: no kerning */
	int TTfullq[FNTNUM];            /* Height is inches / (pointsize / 72) switch - default: height is inches */
	int TTStoreq;			/* Attempt to store glyphs in memory switch - default: store glyphs in memory */

	int *TranslateASCII;	/* Table to translate ASCII to UniCode */
	float TTMaxError[FNTNUM];
};

#ifdef MULTITHREAD
#ifdef __TRUETYPE
struct TT_INFO *tt_font_dataq[GPC_MAX_THREADS];
#endif
#define ttf_data (*tt_font_dataq[instctr])
#else
#ifdef __TRUETYPE
struct TT_INFO tt_font_dataq;
#endif
#define ttf_data tt_font_dataq
#endif

#define TTCharMap ttf_data.TTCharMap
#define TTMetrics ttf_data.TTMetrics
#define TTFudgeFactor ttf_data.TTFudgeFactor
#define LastContourX ttf_data.LastContourX
#define LastContourY ttf_data.LastContourY
#define unitsPerEm ttf_data.unitsPerEm
#define maxPoints ttf_data.maxPoints
#define numGlyphs ttf_data.numGlyphs
#define TTLoc ttf_data.TTLoc
#define GlyphTableOffset ttf_data.GlyphTableOffset
#define GlyphsInMemory ttf_data.GlyphsInMemory
#define GlyphTable ttf_data.GlyphTable
#define KernTable ttf_data.KernTable
#define TTkernq ttf_data.TTkernq
#define TTfullq ttf_data.TTfullq
#define TTStoreq ttf_data.TTStoreq
#define TranslateASCII ttf_data.TranslateASCII
#define TTMaxError ttf_data.TTMaxError

#endif

struct kernpair
{
	int char1, char2, offset;
};

struct PS_INFO
{
	int workingCap;	/* Caps height */
	int PS_Kernq[FNTNUM], FontTypeq[FNTNUM];
	float Matrix[6], FontFudge[FNTNUM];	/* Hold transform matrix */
	int FindBBox, temptop, tempbot;  /* flags */
	float FontMatrix[FNTNUM][6];	/* Font transformation matrix */
	char ** encoding, *encodingbase;
	unsigned char **PSsubrsq[FNTNUM], *PSsubrsbaseq[FNTNUM];
	unsigned char **charstringsq[FNTNUM], *charstringsbaseq[FNTNUM];
	struct kernpair *PS_KernPairsq[FNTNUM];
	char PSfullq[FNTNUM];
	float PSMaxError[FNTNUM];
	char *FontName[FNTNUM];
};

#ifdef MULTITHREAD
extern struct PS_INFO *PS_INFO_dataq[GPC_MAX_THREADS];
#define psi_data (*PS_INFO_dataq[instctr])
#else
extern struct PS_INFO PS_INFO_dataq;
#define psi_data PS_INFO_dataq
#endif

#define workingCap psi_data.workingCap
#define PS_Kernq psi_data.PS_Kernq
#define FontTypeq psi_data.FontTypeq
#define Matrix psi_data.Matrix
#define FontFudge psi_data.FontFudge
#define FindBBox psi_data.FindBBox
#define temptop psi_data.temptop
#define tempbot psi_data.tempbot
#define FontMatrix psi_data.FontMatrix
#define encoding psi_data.encoding
#define encodingbase psi_data.encodingbase
#define PSsubrsq psi_data.PSsubrsq
#define PSsubrsbaseq psi_data.PSsubrsbaseq
#define charstringsq psi_data.charstringsq
#define charstringsbaseq psi_data.charstringsbaseq
#define PS_KernPairsq psi_data.PS_KernPairsq
#define PSfullq psi_data.PSfullq
#define PSMaxError psi_data.PSMaxError
#define FontName psi_data.FontName

struct TT_SHARE_INFO
{
	int TTUnicodeq;			/* Allow Unicode sequences in strings - default: no Unicode allowed */
	int TTUnicodeSymbolq;	/* Default escape to insert Unicode into string - ~, low byte, high byte */
};

#ifdef __INFNTPLT
#ifdef MULTITHREAD
struct TT_SHARE_INFO *TT_Char_infoq[GPC_MAX_THREADS];
#define tci_data (*TT_Char_infoq[instctr])
#else
struct TT_SHARE_INFO TT_Char_infoq;
#define tci_data TT_Char_infoq
#endif

#define TTUnicodeq tci_data.TTUnicodeq
#define TTUnicodeSymbolq tci_data.TTUnicodeSymbolq

#endif

