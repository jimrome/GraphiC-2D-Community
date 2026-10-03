/*********************************************************************
Copyright 1990-1993 by Scientific Endeavors Corporation. All rights reserved.

Header file for GraphiC device driver interface packet structures.
*********************************************************************/

#if defined(__APPLE__) || defined(__linux__) || defined(__unix__) || defined(_WIN64) || defined(__x86_64__) || defined(__aarch64__)
#ifndef G_FAR
#define G_FAR
#endif
#ifndef G_CDECL
#define G_CDECL
#endif
#ifndef B_CDECL
#define B_CDECL
#endif
#ifndef far
#define far
#endif
#ifndef near
#define near
#endif
#ifndef cdecl
#define cdecl
#endif
#ifndef PASCAL
#define PASCAL
#endif
#ifndef pascal
#define pascal
#endif
#ifndef WINAPI
#define WINAPI
#endif
#ifndef APIENTRY
#define APIENTRY
#endif
#elif GPC_UNX || GPC_OS2

#define G_FAR
#define G_CDECL

#if TCQ
#define B_CDECL cdecl
#else
#define B_CDECL
#endif

#else		/* Not OS/2 or UNIX */
#ifdef WIN32
#define G_FAR
#else
#define G_FAR far
#endif

#if GPC_WIN
#define G_CDECL cdecl
#else
#if !WCQ
#define G_CDECL cdecl
#else
#define G_CDECL
#endif
#endif

#endif

#if GPC_EX2
#include <phapi.h>	/* 286-extender files */
#endif

#if GPC_OS2
#include <os2.h>
#endif

#if 1 - IBMQ
#define _Optlink
#endif

#if GPC_WIN
#include <windows.h>
#endif

struct GXYPAIRSQ {
	int x;
	int y;
};

/* Data structure for the device contexts and window sizeing */
#if GPC_UNX && defined(USE_X11)		
struct DevContext {
  XtAppContext	app_context;
  Pixmap		graf_pixmap;
  Window		graf_win;
  Display *		display;
  int			screen;
  GC			gc;
  float			xfactq, yfactq;
  int 			pic_xoffset, pic_yoffset;
  unsigned int	width, height;
};
#define DEVCONTEXT struct DevContext *DvC;
#else
#define DEVCONTEXT void *DvC;
#endif


/* PACKET FOR X-LEVEL CROSSINGS OF PANEL BOUNDARIES */

struct panelsq{					/* Contains the x roots. There is one */
	int index;					/* for each occupied y level.index is the */
	int G_FAR *xp;				/* number of roots, xp points to the left */
	int G_FAR *xp1;				/* edge of an x plateau and xp1 to the */
 								/* right edge. */
};

/* Zoom control parameters for zooming and shrinking */

struct GZOOMQ {
	int zoomit;
	int xshift;
	int yshift;
	float xfact;
	float yfact;
};

/* Printer port and mode control */

struct GPRINTQ {
	int pcom;							/* output serial port */
	int lpt;							/* parallel port number (0-2)*/
	char separate;						/* color separation flag */
	char filedump;						/*	=1 -> output to file */
	int npasses;						/* The number of passes needed */
	int ncolors;						/* Number of colors in the plot */
	int row;							/* The number of rows (in bytes) */
	int col;							/* The number of columns (in bits) */
	int nbuff;							/* The number of print buffers needed */
	int width_100;						/* Page width in 100ths of an inch */
	int height_100;						/* Page height in 100ths of an inch */
	int l_margin_100;					/* Left margin in 100ths of an inch */
	int t_margin_100;					/* Right margin in 100ths of an inch */
	int p_length_100;					/* Length of the long side of plot */
	int print_number;
	long memory_free;					/* Amount of far memory free */
	struct GZOOMQ G_FAR *gz;				/* zoom structure */
};

/*  Packet for GINIT function  */

struct GINITQ {					 		/* 1 */
	int gfunc;							/* Function code */
	unsigned int auxmode;			 	/* Display mode ACOLORQ or CPRINTQ */
	int device_num;				 		/* COLORQ or PRINTQ */
	int type;							/* print or convert */
	int vmax;							/* 0=video 1=printer */
	int hmax;							/* resolutions */
	char G_FAR *info_str; 				/* needed strings (e.g. PS font)*/
	char G_FAR *path_str;
	struct GPRINTQ G_FAR *prtinfo;		/* NEEDED INFO FOR PRINTERS */
	int in_play;
	int first;
	int vector;							/* 0=raster device; 1=vector device */
	int hwchar;							/* 0=no hardware character support in driver */
	int max_psides;						/* Maximum number of panel sides */
	int G_FAR *fnt_array;				/* Array of used PS fonts */
	int metric;							/* Set to 1 if metric units */
	int penchg;							/* Allows user to change pens. */
#if GPC_OS2 || GPC_WIN
	int xpos;
	int ypos;
	BOOL bBmpbsflag;
#endif
#if GPC_OS2
	void *init_params;
#elif GPC_WIN
	float lmargin;						/* for HPGL conversion LB */
	float tmargin;	
	float xlength;		
	WORD xinvert;      
	WORD yinvert;
	HWND hWnd;                          /* Window handle for driver */
#ifdef GPC_MFC
	HINSTANCE hInst;					/* Instance handle for driver */
	HDC hDC;
#else
	HANDLE hInst;						/* Instance handle for driver */
#endif
	HCURSOR hcursor;					/* Cursor handle for default cursor */      
	HCURSOR hXwcursor;					/* Cursor handle for custom cursor */      
#endif
	DEVCONTEXT
};

/* Packet for GSCRTYPE function */

struct GSCRNTYPEQ	{					/* 2 */
	int gfunc;							/* Function code */
	int gmode;							/* Display mode 't', 'g'*/
	int mouse;							/* 1=has mouse */
	DEVCONTEXT
};

/*  Packet for GCLEAR function	*/

struct GCLEARQ {						/* 3 */
	int gfunc;							/* Function code */
	int color;							/* Screen color */
	int BACKQ;							/* Translated screen color. */
	unsigned char G_FAR * G_FAR *tkb[8];/* pointer to printer buffer memory */
	DEVCONTEXT
};

/*  Packet for GDOT function  */

struct GDOTQ {							/* 4 */
	int gfunc;							/* Function code */
	int px, py;							/* Coordinates to complement */
	int color;							/* Color */
	int size;							/* thickness of dot */
	int logical_mode;					/* Logical mode: AND, OR, XOR,... */
};

/*  Packet for GLINE functions  */

struct GLINEQ {						/* 5 */
	int gfunc;						/* Function code */
	int pfx, pfy;					/* From point */
	int ptx, pty;					/* To point */
	int color;						/* Color */
	int thick;						/* Thickness */
	int pattern;					/* Line style */
	int logical_mode;				/* Logical mode: AND, OR, XOR,... */
	int action;
	int penup;						/* Penup flag */
	int npts;						/* Length of next item (several pieces) */
	int start_count;				/* Offset where to put count byte */
	int font_char;					/* True for drawing fonts. */
	struct GXYPAIRSQ G_FAR *xyp;	/* Pointer to self-counting xy vector */
	DEVCONTEXT
};

/*  Packet for the GPANEL function  */

struct GPANELQ {						/* 6  or 16 */
	int gfunc;							/* Function code */
	int nsides;							/* Number of sides */
	int pattern;						/* Fill pattern */
	int border;							/* 0=no border;  1=draw border */
	int b_color;						/* color for border */
	struct GXYPAIRSQ G_FAR *xyp;		/* x-y vertex pairs */
	struct GXYPAIRSQ G_FAR *save_xyp;	/* accumulated x-y vertex pairs */
	struct GXYPAIRSQ G_FAR *save_bord;	/* x-y vertex pairs before hole splitting */
	int npts;							/* # elements in save_xyp */
	int bnpts;							/* # elements in save_borb */
	int npanels;					 	/* # panels in save_xyp */
	int max_npts;
	int noccupied;						/* Number of occupied scan lines */
	int start;							/* lowest y level in panel */
	int end;							/* highest y level in panel */
	/* DATA STRUCTURES TO BE ALLOCATED BY MAIN PROGRAM */
	struct panelsq G_FAR *panl;
	/*	MEMORY TO SAVE EACH PANEL PIECE */
	int G_FAR *xscale;
	int G_FAR *yscale;
	int G_FAR *xcount;
	int G_FAR *tran;
	DEVCONTEXT
};

/*  Packet for GTEXT function  */

struct GTEXTQ {							/* 7 */
	int gfunc;							/* Function code */
	int px, py;							/* Text location (lower left corner) */
	char *str;							/* Text string */
	int tlen;							/* Text length */
	int color;							/* Text color */
	int height;							/* Height in 4096 pixels */
	int angle;							/* Rotation angle in degrees */
	int linesp;							/* Space between lines */
	int space;							/* Space increment between characters */
	int font_number;					/* The PostScript font number */
	struct GZOOMQ G_FAR *gz; 				/* zoom structure */
	DEVCONTEXT
};

/*  Packet for GCHAR function (matrix font)  */

struct GCHARQ {
	int gfunc;							/* 8 */
	int character;						/* Character to output */
	int row, col;						/* Text location (lower left corner) */
	int color;							/* Text color */
};

/*  Packet for function-only requests:	*/

struct GFONLYQ {
	int gfunc;							/* Function code */
};

/* Packet to signal termination */

struct GTERMINATEQ {					/* 9 */
	int gfunc;
	DEVCONTEXT
};

/* Packet for openfile() function  */

struct GFILEOPENQ {						/* 10 */
	int gfunc;
	char G_FAR *filename;
	int filehandle;						/* (Returned) */
	int append;							/* Should we append to the old file? */
};

/* Packet for closefile() */

struct GFILECLOSEQ {					/* 11 */
	int gfunc;
	int filehandle;
};

/* Packet for wrtfile() */

struct GWRITEFILEQ {
	int gfunc;							/* 12 */
	char G_FAR *wstr;						/* ptr to the string to be written */
	int fh;								/* file handle */
};

/*  Packet for GMARKER function  */

struct GMARKQ {
	int gfunc;							/* Function code */
	int color;							/* Color */
	int size;							/* Size in 4096 pixels */
	int px, py;							/* Location */
};

/* Current position for cursor functions */

struct CURPOS {							/* 13 */
	int gfunc;
	int color;
	int x;
	int y;
	int xsav[2];
	int ysav[2];
	int mode;
	int on_off;
};

/* Dump print buffer */

struct GDUMPQ {							/* 14 */
	int gfunc;
};

/*	GERCHARQ Packet to erase matrix font area */

struct GERCHARQ {						/* 17 */
	int gfunc;
	int row;  							/* row of character */
	int frcol;							/* from column */
	int tocol;							/* to column */
};

struct G_GET_CHAR {						/* 18 */
	int gfunc;
	int action;
};

struct G_PRINT_STRING {					/* 19 */
	int gfunc;
	int type;
	char G_FAR *string;
};

struct G_PAGE_SIZE {
	int gfunc;							/* 20 */
	int xLeft;						/* Used to set the page dimensions. */
	int yBottom;
	int xRight;
	int yTop;
};

#if GPC_WIN

struct PRINT_INFO {
	char szOpnName[128];
	float fpixconv;
	BOOL bprntscl; 
	BOOL bprtwhtbk; 
	BOOL bprtwhtln;
};

struct G_REDRAWQ {
	int gfunc;
	int resize;
	int FromBitmap;
	int hmax;
	int vmax;
};

struct AUX_INFO {
	int action_flag;
	char *buffer;
};

typedef struct PRINT_DATA {
	int l_margin_100;
	int t_margin_100;
	int p_length_100;
	int prompt;
	int metric;
	int FillPage;
	int PrntBackground;
	int setup;
	int Portrait_mode;
	char printer[128];
} PRINT_DATA;

typedef struct CONV_ARGS {
	int size;
	unsigned char *tkf_buff;
	char *conv_name;
	struct GZOOMQ g_z;
	PRINT_DATA *print_data;
	struct AUX_INFO aux_info;
	int instctr;
} CONV_ARGS;

typedef struct GPCHANDLES {
	HWND hWndq;          	/* global window handle */
	HANDLE hpalq;			/* global handle for palette */
	HPALETTE hPal;   		/* handle for palette */
	LOGPALETTE *phPal;      /* pointer to logical palette */
	HBRUSH hBrush;          /* brush to clear screen  */
	HPEN hSolidPen;         /* pen handle             */
	HINSTANCE hInstq;       /* handle for instance of program */
	HDC hOC;				/* current output context */
	HDC hWinDC;             /* handle for display context */
	HDC hMemDC;          	/* memory device context */
	HBITMAP hBitmapq;		/* Bitmap for clipboard */
	HBITMAP hBitcopq;		/* backup Bitmap for window */
	HBITMAP hOldBitmapq;	/* previous backup Bitmap for window */
	HCURSOR hXhairw;		/* custom xhair cursor handle */
} GPCHANDLES;
typedef GPCHANDLES *PGPCHANDLES;

#endif

#if GPC_OS2

struct G_REDRAW {
	int gfunc;
	int resize;
	int menustate;
};

#define TIFF_SIZE 38610

struct AUX_INFO {
	int action_flag;
	char *buffer;
};

typedef struct PRINT_DATA {
	int l_margin_100;
	int t_margin_100;
	int p_length_100;
	int prompt;
	int metric;
	int FillPage;
	int PrntBackground;
	int setup;
	int Portrait_mode;
	char printer[128];
} PRINT_DATA;

typedef struct CONV_ARGS {
	int size;
	PVOID base_mem;
	unsigned char *tkf_buff;
	char *conv_name;
	struct GZOOMQ g_z;
	PRINT_DATA *print_data;
	struct AUX_INFO aux_info;
	char *file_path;
	HWND hWndFrame;
	HEV g_print_wait;
} CONV_ARGS;

typedef struct GPCHANDLES {
/********************** WINDOW HANDLES ********************/
	HWND hWndFrame;
	HWND hWndClient;		/* client window */
	HMQ hmq;
	HAB hab;				/* PM anchor block handle */
	HDC hdc_Screen;			/* Screen handles */
	HPS hps_Screen;
	HDC hdc_Bitmap;			/* Bit map handles for screen saves */
	HPS hps_Bitmap;
	HBITMAP hbm_Bitmap;
	HWND hwndHelp;
	int instance;
} GPCHANDLES;
typedef GPCHANDLES *PGPCHANDLES;

#define GPC_SCREEN		1
#define GPC_PRINTER		2

#define WM_USER_TEXT			WM_USER + 1
#define WM_USER_SET_MENU		WM_USER + 2
#define WM_USER_FILEOPEN		WM_USER + 3
#define WM_USER_SET_TITLE		WM_USER + 4
#define WM_USER_GET_TITLE		WM_USER + 5
#define WM_USER_SETFOCUS		WM_USER + 6
#define WM_USER_QUIT			WM_USER + 7
#define WM_USER_SET_Y_LONG		WM_USER + 8
#define WM_USER_GPC				WM_USER + 9

#endif

#define UPRIGHT 1

#define FULL_PAGE 1			/* assignments for g_iq.type used for printers */
#define L_90 2
#define R_90 4
#define RL_180 8
#define HI_RES 16
#define PROMPT 32
#define FLUSH_PRINTER 64

#define INITIALIZE		1
#define SCRNTYPE		2
#define CLEAR			3
#define DRAW_DOT		4
#define G_LINE			5
#define PANL			6
#define G_TEXT			7
#define G_CHAR			8
#define QUIT			9
#define OPEN_FILE		10
#define CLOSE_FILE		11
#define WRITE_FILE		12
#define CURSOR			13
#define DUMP_PRINT		14
#define SET_PRINT_MEM	15
#define PANPROC			16
#define ERASE_CHAR		17
#define GET_CHAR		18
#define PRINT_STRING	19
#define SIZE_PAGE		20
#define REDRAW			21

#define SOLID					1
#define DOTTED					2
#define DOT_DASH				3
#define SHORT_DASH				4
#define LONG_DASH				5
#define LONG_DASH_THREE_DOTS	6
#define LONG_DASH_SHORT_DASH	7
#define MEDIUM_DASH				8
#define FINE_DOT				9

#define	MAXRESX		8192
#define	MAXRESY		6266
#define MAXRESXMO	8191
#define	MAXRESYMO	6265
#define CURSORSIZE	106

#define RNDHALF (float).49999
#define RNDONE (float).99999

#if GPC_UNX
extern int *ftableq;
extern void (*ctrl_c_savq)();
extern void (*float_err_savq)();
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
extern char tkf_fileq[];
extern char tkf_input;
#endif

#define FROM_PLAY 0
#define GRAF_HC   2
#define PLAY_HC   3

#define ENTER 13

/* Panel-filling capabilities and requirements */
#define VECTOR_DEVICE             0x01
#define POLYGONS_WITHOUT_PATTERNS 0x02
#define ALL_POLYGONS              0x04
#define NO_HOLES                  0x08

/* Cursor size parameters for the video drivers */
#define CUREND		106
#define CURSTART	30

/* Flag values for passing handle information. */
#define GET_HANDLES		1
#define GIVE_HANDLES	2

/* color definitions */
#define BLACK		0
#define BLUE		1
#define GREEN		2
#define CYAN		3
#define RED			4
#define MAGENTA		5
#define BROWN		6
#define WHITE		7
#define DRK_GRAY	8
#define LGT_BLUE	9
#define LGT_GREEN	10
#define LGT_CYAN	11
#define LGT_RED		12
#define LGT_MAGENTA	13
#define YELLOW		14
#define BRT_WHITE	15
