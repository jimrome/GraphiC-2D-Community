#ifndef GRAPHIC_H_INCLUDED
#define GRAPHIC_H_INCLUDED

#if defined(GPC_PORTABLE) || defined(__APPLE__) || defined(__linux__) || defined(_WIN32)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdarg.h>
#include <ctype.h>
#endif

/* The following comment is for using PC-Lint (Gimpel Software) with GraphiC */
/*lint -e??? */
#ifdef U6000			/* Needs to be first for pragma to work. */
#pragma chars (signed)
#endif

#include <comp.h>
#include <user.h>
#include <missings.h>

#if GPC_WIN || GPC_OS2
#define GPC_GUI 1
#else
#define GPC_GUI 0
#endif

#if !GPC_GUI && defined(__cplusplus)
#define GPC_CPP 0
#else
#define GPC_CPP 0
#endif

#if GPC_CPP
#define GCLASS GRAPHIC::
#define PMF_GCLASS this->*
#define AMF_GCLASS & GCLASS
#else
#define GCLASS
#define PMF_GCLASS
#define AMF_GCLASS
#endif

#if (!GPC_OS2 || GPC_CPP) && defined(MULTITHREAD)
#undef MULTITHREAD
#endif

#if defined(MAKEDLL)	/* If creating or using a DLL */

#if GPC_OS2					/* If using OS/2 */
#define TYPE APIENTRY
#define P_TYPE G_CDECL APIENTRY

#elif GPC_WIN				/* If Using Windows */

#if MSQ + TCQ					/* if Microsoft or Borland */
#define TYPE WINAPI
#else							/* Else if Wactom */
#define TYPE _export WINAPI
#endif							/* end Watcom */

#if !defined(WIN32)
#define P_TYPE _export cdecl
#else
#define P_TYPE cdecl
#endif

#define FTYPE TYPE	/* For functions in driver import lib. */

#endif						/* end Windows */

#if WCQ
#define W_CDECL cdecl
#else
#define W_CDECL
#endif

#else					/* Not making a DLL */

#if GPC_WIN
#define FTYPE WINAPI	/* For functions in driver import lib. */
#endif

#define TYPE
#define P_TYPE G_CDECL
#define W_CDECL
#endif					/* end if defined(MAKEDLL) */

#if GPC_OS2
#define ATYPE G_SYSTEM
#else
#define ATYPE TYPE
#endif

#ifdef MULTITHREAD
#define GPC_MAX_THREADS 256
extern int g_query_process_id(int action);
#define G_PROCESS_ID int instctr = get_instanceq(-1);
#define gpcAssociate(x) ((PGPCHANDLES)x)->instance = get_instanceq(-1) + 1
#define DESTROY_GPC_INSTANCE get_instanceq(-2);
#else
#define G_PROCESS_ID
#define gpcAssociate(x)
#define DESTROY_GPC_INSTANCE
#endif

#if defined(MULTITHREAD) || GPC_CPP
#define G_EXTERN	/* Variables are structure elements so not extern. */
#define G_STATIC
#else
#define G_EXTERN extern
#define G_STATIC static
#endif

#if GPC_WIN

#define CURSON_MODE        1
#define GET_CURSOR_RESULTS 2

#if WCQ && defined(__386__) && !defined(WIN32)
#define WIN32
#endif

#ifndef NOCOMM
#define NOCOMM
#endif

#ifdef GPC_MFC
#define SCOPE ::
#else
#include "windows.h"    /* include for Windows version */
#define SCOPE
#endif

#if !defined(WIN32)

#if !defined(WIN16)
	#define WIN16	TRUE
#endif

	#define WIN31

	#define TCHAR	char
	#define TEXT(x)	x

	#define INT		int
	#define UINT	WORD
	#define APIENTRY PASCAL
	#define G_MEMCPY _fmemcpy
	#define G_STRCMP _fstrcmp
	#define G_STRCPY _fstrcpy
#else
	#define WIN16	FALSE
	#define G_MEMCPY memcpy
	#define G_STRCMP strcmp
	#define G_STRCPY strcpy
#endif

#if GPC_FOR
#define GPC_MAIN APPLICATION
#else
#define GPC_MAIN Application
#endif
#define SET_LPLOTQ if (!prntflgq && !convflgq) lplotq = tkf_incrementq
#define GPC_PUTS GPCWINMessageBox
#define GPC_SPUTS GPCWINprintstr
#define GPC_GETS GPCWINGetInput
#define BITMAP_ON GPCBitmapStore(1);
#define BITMAP_OFF GPCBitmapStore(0);
#define sprintfq wsprintf

#elif GPC_OS2

#define G_MEMCPY memcpy
#define G_STRCMP strcmp
#define G_STRCPY strcpy
#define SET_LPLOTQ
#define GPC_PUTS GPCMessageBox
#define GPC_SPUTS GPCprintstr
#define GPC_GETS GPCGetInput
#define GPC_MAIN main
#define BITMAP_ON GPCBitmapStore(1);
#define BITMAP_OFF GPCBitmapStore(0);
#define sprintfq sprintf

#if TCQ + WCQ
#define G_SYSTEM _syscall
#else
#define G_SYSTEM _System
#endif
#ifdef GRAPHIC_CMN_INCLUDED
#define INCL_DOSPROCESS
#endif

#else			/* Not Windows or OS/2 */

#define G_MEMCPY memcpy
#define G_STRCMP strcmp
#define G_STRCPY strcpy
#define SET_LPLOTQ
#define GPC_PUTS puts
#define GPC_SPUTS puts
#define GPC_GETS(prompt, buffer, len) { printf(prompt); fgets(buffer, len, stdin); \
	buffer[strlen(buffer) - 1] = '\0';}	/* Remove the newline char. */
#define BITMAP_ON
#define BITMAP_OFF
#define sprintfq sprintf
#if WCQ
#define GPC_MAIN main
#else
#define GPC_MAIN G_CDECL main
#endif
#endif

#include "g_macros.h"
#if GPC_UNX && defined(USE_X11)
#include <gvideo.h>
#endif
#include <drvpkt.h>

#define base d3base

#define gabs(x) ((x)<0?-(x):(x))
#define messageq(mesgtype,mesgnum) (gmessageq(mesgtype,mesgnum,__FILE__,__LINE__))
#define FULLPAGE pixtoin(acrossq + 1), pixtoin(topq + 1)
#ifdef TESTMEM
#define gpcalloc(x, y) gpcallocq(x, y, __FILE__, __LINE__)
#define gpcrealloc(x, y) gpcreallocq(x, y, __FILE__, __LINE__)
#define gpcfree(ptr) gpcfreeq((ptr), __FILE__, __LINE__)
#endif
#define LANDSCAPE rotate(0);
#define PORTRAIT rotate(1);
#define sym3d(x, y, z, sym) d3symbol(x, y, z, sym, 0)

	/* SCALING MACROS FOR USING LONG FACTORS INSTEAD OF FLOATS */
#define XSCALE_PAGE(x) ((x) * xfact_pgq)
#define YSCALE_PAGE(y) ((y) * yfact_pgq)
#define XSCALE_PLOT(x) ((x) * xfact_pltq)
#define YSCALE_PLOT(y) ((y) * yfact_pltq)
#define ZSCALE_PLOT(z) ((z) * zfact_pltq)

#define TOP_SURFACE_COLOR (surface_clrq & 0x00FF)
#define BOTTOM_SURFACE_COLOR ((surface_clrq & 0xFF00) >> 8)

/* GRAPHIC CONSTANTS */
#define LABLEN 31				/* Max length of label strings */
#define MAX_PRINTER 35			/* Last printer selection available. */
#define FNTNUM 4
#define TKBUFFSIZE 0x3000
#define MAX_TIMER 5             /* Number of timers */
#define GPC_BEEP_FREQUENCY 440
#define GPC_BEEP_DURATION 50
#define SAVE_SEGMENT 1

#ifdef PI
#undef PI
#endif
#define PI 3.141592654
#define PI_OVER_180 .017453293
#define PI_OVER_TWO 1.570796
#define TWO_PI 6.2831853
#define SQRT_OF_3 1.732051
#define NO_INT (-32767)
#ifdef DEGREES_PER_RADIANS
#undef DEGREES_PER_RADIANS
#endif
#define DEGREES_PER_RADIANS 57.2958
#ifdef BASE_E
#undef BASE_E
#endif
#define BASE_E 2.718282
#ifdef DEFAULT_INT
#undef DEFAULT_INT
#endif
#define DEFAULT_INT NO_INT

/***************** Grpahic's window attribute flags *****************/
/* Windows in GraphiC's nomenclature are areas that are protected from
being drawn in unless they are active. */
#define GPC_IS_GWIN			0x01	/* Window to contain a plot */
#define GPC_HAS_BORDER		0x02	/* Window has a border */
#define GPC_NO_BORDER		0x04	/* No border around the window */
#define GPC_IS_BAR			0x08	/* Window generated from bar() */

/************************* Axes Identifiers *************************/
#define GPC_X_AXIS			0x01
#define GPC_Y_AXIS			0x02
#define GPC_Z_AXIS			0x04

/************************ Axes options flags ************************/
        /*************** Used with axis_flgq ********************/
#define GPC_AXESON			0x00	/* Default -- axes with labels */
#define GPC_AXESOFF			0x01	/* No axes or labels */
#define GPC_XAXIS			0x02	/* Draw x axis */
#define GPC_YAXIS			0x04	/* Draw y axis */
#define GPC_XLAB			0x08	/* Label ticks on x axis */
#define GPC_YLAB			0x10	/* Label ticks on y axis */
#define GPC_AUTOSCAL		0x20	/* Auto scaled axes. */

        /*************** Used with lab_flgq ********************/
#define X_STRING_LABELS		0x01	/* User specified x axis string labels */
#define Y_STRING_LABELS		0x02	/* User specified y axis string labels */
#define Z_STRING_LABELS		0x04	/* User specified z axis string labels */
#define X_TICK_POS			0x08	/* User specified x tick positions */
#define Y_TICK_POS			0x10	/* User specified y tick positions */
#define Z_TICK_POS			0x20	/* User specified z tick positions */
#define REVERSE_INDEX		0x40	/* Label array indexes need reversing */
#define	DRAW_FRAME			0x01	/* Draw a frame around the plot */
#define DRAW_FRAME_TICKS	0x02	/* Draw tick marks around the frame */

/************************ Axes attribute flags *********************/
#define AXES_ARE_CROSSED	0x01
#define X_AXES_ON_BOTTOM	0x02
#define X_AXES_ON_TOP		0x04
#define Y_AXES_ON_LEFT		0x08
#define Y_AXES_ON_RIGHT		0x10
#define USER_WANTS_CROSSED_AXES	0x20
#define USER_SET			0x40
#define X_ON_BOTTOM (crossq.flg	& X_AXES_ON_BOTTOM)
#define X_ON_TOP (crossq.flg & X_AXES_ON_TOP)
#define Y_ON_LEFT (crossq.flg & Y_AXES_ON_LEFT)
#define Y_ON_RIGHT (crossq.flg & Y_AXES_ON_RIGHT)
#define USER_SET_CROSS_LOC (crossq.flg & USER_SET)

#define LEFT_AXIS_DRAWN		0x02	/* Used with right-hand y axes */
#define DRAWING_RIGHT_AXIS	0x04	/* in yupq() for the right-hand y axis */

/************************ Error messages flags *********************/
#define WARNQ				'W'		/* Lowest level warning */
#define ERRQ				'E'		/* More serious problem, continue ? */
#define FATALQ				'F'		/* Program will crash, exit */
#define MESGQ				'M'		/* Text message for user information */
#define NEWQ				'N'		/* For development purposes */

/************************ Scaling units flags **********************/
#define USER_UNITS			0x01	/* location input is in user units */
#define INCH_UNITS			0x02	/* location input is in inches */
#define PIXEL_UNITS			0x04	/* location is in pixels */

#define VIDEO 0					/* Device driver types */
#define PRINTER 1
#define CONVERT 2

#define OPEN_TEK_FILE 200		/* TKF file handeling flags */
#define CLOSE_TEK_FILE 201
#define DUMP_TEK_FILE 202
#define SEEK_TEK_FILE 203
#define MARK_TEK_FILE 204
#define END_TKF_PLOT 205

#define BOTTOM		0X01		/* Attributes for contour plotter cells */
#define TOP			0X02
#define EAST		0X04
#define WEST		0X08

#define GPC_FILL_CONTOURS_NO_BORDERS	0x0000
#define GPC_FILL_CONTOURS_WITH_BORDERS	0x0001
#define GPC_CONTOUR_LINE_ATTRIBUTES		0x0002

#define X_ON_RIGHT	0X08
#define X_ON_LEFT	0X10

#define UPPER		0X10		/* Masks for contour cell entry on diagonal */
#define LOWER		0X20

#ifdef NO_DATA
#undef NO_DATA
#endif
#define NO_DATA (-FLT_MAX)		/* Provide a means to signal no data */

/******************** Flags for surface plot attributes *************/
#define FIRST_QUAD		0x01	/* viewers position relative to plot */
#define SECOND_QUAD		0x02
#define THIRD_QUAD		0x04
#define FORTH_QUAD		0x08
#define NO_COLOR		0x00	/* 3-D surface colors not requested */
#define SET_TOP_COLOR	0x01	/* Draw vectors in top color */
#define SET_BOTTOM_COLOR 0x02	/* Draw vectors in bottom color */
#define POSITIVE_X (FIRST_QUAD | FORTH_QUAD)
#define POSITIVE_Y (FIRST_QUAD | SECOND_QUAD)

#define GPC_DRAW_MESH		  0x01 /* Wire mesh drawn with filled surface. */
#define GPC_FILL_ON_CONTOURS  0x02 /* Surface colors in bands at contours. */
#define GPC_COLOR_ON_X		  0x04 /* Surface colors in bands in x. */
#define GPC_COLOR_ON_Y		  0x08 /* Surface colors in bands in y. */

#ifdef ON
#undef ON
#endif
#ifdef OFF
#undef OFF
#endif
#ifdef TRUE
#undef TRUE
#endif
#ifdef FALSE
#undef FALSE
#endif
#define TRUE 1
#define FALSE 0
#define ON 1
#define OFF 0

#if GPC_DOS
#define GRAPHIC_ENV "GPC"
#elif GPC_UNX
#define GRAPHIC_ENV "GPC"
#elif GPC_OS2
#define GRAPHIC_ENV "GPCOS2"
#elif GPC_EX2
#define GRAPHIC_ENV "GPC286"
#elif GPC_WIN
#define GRAPHIC_ENV "GPCWIN"     /* Windows version */
#else
#define GRAPHIC_ENV "GPC"
#endif

/* definitions for plot_typeq */
#define LINEAR_PLOT		0x0001
#define XLOG_PLOT		0x0002
#define YLOG_PLOT		0x0004
#define POLAR_PLOT		0x0008
#define POLAR_LOG_PLOT	0x0010
#define ERF_PLOT		0x0020
#define SMITH_PLOT		0x0040
#define	D3_PLOT			0x0080
#define LOG_BASE_X		0x0100		/* User specified log base for x. */
#define LOG_BASE_Y		0x0200		/* User specified log base for y. */

/* Legend status */
#define LEG_OFF			0
#define LEG_ON			1
#define LEG_DISABLED	2

#define XY_PLANE	1
#define XZ_PLANE	2
#define YZ_PLANE	3
#define Y_Z_PLANE	0x04
#define V1_V2_PLANE 0x08
#define	V1_Z_PLANE 0x10
#define	V2_Z_PLANE 0x20

/************************* Flags used in zooming. ***************/
#define GPC_ZOOM_OUT 2
#define LL_CORNER_SET 4
#define GPC_BUTTON1_DOWN 8
#define GPC_BUTTON2_DOWN 0x10
#define GPC_QUIT 0x20

/************ TYPE DEFINITIONS ***************/
typedef struct GXYPAIRSQ *PGXYPAIRS;
typedef struct GXYPAIRSQ **PPGXYPAIRS;
typedef struct TICK_POS GTICK_POS;
typedef unsigned char Uchar;
typedef unsigned int Uint;
typedef unsigned long Ulong;

typedef float (W_CDECL *GPC_FUNQ)(float, float);

#if GPC_DOS
typedef int (cdecl *GPC_LIBPROC)(void far *,int);
typedef GPC_LIBPROC (G_CDECL *GD_INIT)(Uint sp, Uint ss);

#elif GPC_EX2
typedef int (far _pascal _loadds _export *GPC_LIBPROC)(void far *, int);

#elif GPC_WIN
typedef int (far APIENTRY *GPC_LIBPROC)(void far *, int);

#elif GPC_OS2
typedef MRESULT (*PUSER_FUN)(HWND, USHORT, MPARAM, MPARAM);
typedef int (* G_SYSTEM GPC_LIBPROC)(void *, int);

#elif GPC_UNX
typedef int (*GPC_LIBPROC)( void *, int );
#else
typedef int (*GPC_LIBPROC)( void *, int );
#endif

#ifndef sign
#define sign(x) (((x)<0)? -1:1)
#endif
#ifndef min
#define min(a,b) (((a)<(b))?(a):(b))
#endif
#ifndef max
#define max(a,b) (((a)>(b))?(a):(b))
#endif

/************ COMPILER INCLUDE FILES ***************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <float.h>
#include <ctype.h>
#include <math.h>

#if !GPC_PORTABLE
#if IBMQ
#include <io.h>
#include <conio.h>
#include <malloc.h>
#include <memory.h>
#include <fcntl.h>
#include <stdarg.h>
#endif

#if MSQ
#include <conio.h>
#include <dos.h>
#include <io.h>
#include <malloc.h>
#include <memory.h>
#include <stdarg.h>
#endif

#if TCQ
#include <alloc.h>
#include <dos.h>
#include <io.h>
#include <mem.h>
#include <process.h>
#include <stdarg.h>
#endif

#if WCQ
#include <conio.h>
#include <dos.h>
#include <fcntl.h>
#include <io.h>
#include <malloc.h>
#include <stdarg.h>
#include <stddef.h>
#include <process.h>
#endif
#endif /* !GPC_PORTABLE */

#if GPC_UNX || GPC_PORTABLE || GPC_MAC || defined(__APPLE__) || defined(__linux__) || defined(__unix__)
/*#define _ALL_SOURCE */
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>
#include <math.h>
#include <float.h>
#include <string.h>
#include <signal.h>
#if !defined(_MSC_VER) && !defined(_WIN32)
#include <unistd.h>
#elif defined(__MINGW32__)
#include <unistd.h>
#endif
#if defined(USE_X11)
#include <sys/ipc.h>
#include <sys/sem.h>
#endif
#include <errno.h>
#include <sys/stat.h>
#endif

#ifndef ERR
#define ERR (-1)
#endif

#ifdef SetDefaultPrinter
#undef SetDefaultPrinter
#endif

#if !GPC_WIN && !defined(_MSC_VER)
char * TYPE itoa(int value, char *str, int radix);
char * TYPE strupr(char *str);
char * TYPE strlwr(char *str);
#endif

#ifndef UCHAR
#define UCHAR unsigned char
#endif
#ifndef SHORT
#define SHORT short
#endif

/*********************** Structure Definitions *****************/
struct crossdatq {	/* Used to determine axis crossing options */
	int flg;
	float x;
	float y;
};

struct GDRQ {
	char fname[128];
	char ProcName[128];
};

union pointersq{
	long pointr;
	char *ptrc;
	Uint *ptri;
};

struct DATE_TIMEQ {
	int year;
	int month;
	int day;
	int hour;
	int minute;
	int second;
};

struct G_WIN {
	int type;
	int xl;
	int yl;
	int xh;
	int yh;
	int border_color;
};

struct ADDITIONSQ {
	int left;
	int right;
	int below;
	int x_right;
	int x_left;
	int x_top;
	int x_bottom;
	int y_right;
	int y_left;
	int y_top;
	int y_bottom;
};

struct HORIZON {
	int offset;			/* Offset from the page edge to the workbox edge */
	int width;			/* Length of horizon arrays. */
	int *upper;			/* Array of upper horizon values. */
	int *lower;			/* Array of lower horizon values. */
};

struct TitlDatq {
	char *string;
	char dup_flag;
	int ifont;
	int color;
	int bcolor;
	int fillflag;
	int xpos;
	int ypos;
	int spacing;
};

struct LINESTART {
	int x;
	int y;
	int penup;
};

struct CHAR_DATA {
	int width;
	int length;
	char *strokes;
	int dsc;
	int asc;
	int lext;
	int rext;
};

struct G_FONT {
	int fntht;			/* Pel heights for each loaded font */
	int maxwidth;		/* Width of the widest character */
	int width_set;		/* Character whose width is used for spacing */
	int const_width;	/* The constant width in pels */
	struct CHAR_DATA char_dataq[169];	/* 168 characters per font */
};

struct D_3_CONVERSION_FACTORS {
	float xvu;
	float yvu;
	float zvu;
	int xview;
	int yview;
	float x3conv;
	float y3conv;
	float z3conv;
	int xpos;
	int ypos;
	float d1;			/*PARAMETERS FOR PICTURE-PLANE CONVERSIONS*/
	float d2;			/*PARAMETERS FOR PICTURE-PLANE CONVERSIONS*/
	float d12;			/*PARAMETERS FOR PICTURE-PLANE CONVERSIONS*/
};

struct PATCHQ {
	int attr;
	int x;
	int y;
};

struct PATCH_LISTQ{
	int index;
	struct PATCHQ *patch;
};

struct VECTOR_SAV {
	struct VECTOR_SAV *next;
	int x;
	int y;
};

struct TICK_POS {
	int nsteps;
	float *tickpos;
};

struct HORIZ_CLR {
	int x;
	int y;
	int next_index;
};

typedef struct {	/* One of these for every contour at a level */
	int start;			/* Location of line start in "perimeter" units. */
	int end;			/* Location of line end in "perimeter" units. */
	int number;			/* The number of elements in the contour. */
	int *x;				/* The coordinates of the vertices. */
	int *y;
	int start_sense;	/* A 1 means rising clockwise. */
	int end_sense;
	char cell;			/* Is 0 if contour is open, 2 if closed. */
	char used;			/* Is 0 if not used this level. */
} STORCNT;
typedef STORCNT *PSTORCNT;

struct labpos {
	int lpmc;
	double dist;
};

#ifdef TESTMEM		/* Located in USER.H */
struct ELEM {
	void *dataptr;                           /* Pointer to allocated block */
	struct ELEM *next;              /* Pointer to next element of the list */
	char file[9];
	int line_num;
	size_t amount;
	long test_pattern;
};
#endif

struct edge_pts {
	int *x;
	int *y;
	int in_point;
	int out_point;
};

struct pan_data {		 	/* Data structure for each panel piece */
	PGXYPAIRS xyp;			/* Pointer to this piece (with size) */
	int nholes;				/* Number of holes so far */
	int *outs; 				/* Pointer to an array of outside indices */
	int nouts;				/* Number of holes so far */
	int xl, yl, xh, yh;		/* Bounding box for piece */
};

/************************ ALL GLOBAL DATA ***********************/
#if GPC_CPP
class GRAPHIC {
	public:
	GRAPHIC(void);

#elif defined(MULTITHREAD)
struct G_GLOBAL_DATA {
#endif

/************************ BOOLEAN FLAGS *************************/
G_EXTERN char gtextq;
G_EXTERN char psetflgq;
G_EXTERN char size_flagq;
G_EXTERN char demoq;
G_EXTERN char leg_flgq;
G_EXTERN char panfilq;
G_EXTERN int pattern_saveq;
G_EXTERN int lcolor_saveq;
G_EXTERN char crop_flgq;
G_EXTERN char in_plbl3q;
G_EXTERN char penup_flgq;
G_EXTERN char rotate_flgq;
G_EXTERN char mouse_flgq;
G_EXTERN char has_beepedq;
G_EXTERN char adding_flgq;

/********************** BIT FLAGS ****************************/
G_EXTERN char frame_flgq;
G_EXTERN char right_flgq;
G_EXTERN char viewers_quadq;

G_EXTERN int ascq;					/*ZOOM/PRINT OPTION VARIABLE*/
G_EXTERN float symhtq;		/*MULTIPLIER FOR 11 PIXEL SYMBOL HEIGHT*/
G_EXTERN float labscalq;		/*SCALING FACTOR FOR TEXT*/
G_EXTERN char *cmd_strq;	   		/*COMMAND STRING FOR GPROSQ*/
G_EXTERN char *stringq;			/*MISC. STRING PASSER*/
G_EXTERN char cl_valq[16];	/*IBM COLOR VALUE CORRESPONDING TO A TEK INDEX*/

G_EXTERN float pix_convertq;	/* METRIC UNITS */


G_EXTERN int itotekq[16];

G_EXTERN Uint PSlinewq;				/* WIDTH FOR POSTSCRIPT LInes */
G_EXTERN int PSfntarrayq[10];

G_EXTERN int NCOLORQ;				/*THE NUMBER OF PLOT COLORS USED */
G_EXTERN int win_numq;				/* WINDOW HANDLER FLAG */
G_EXTERN char dev_typeq;				/* 0=VIDEO, 1=PRINTER, 2=CONVERT */

G_EXTERN struct G_WIN *win_structq;
G_EXTERN struct ADDITIONSQ add_inq;

/************ UNINITIALIZED GLOBAL VARIABLES ***********/

G_EXTERN unsigned int plot_typeq; 		/* bit field axis types */
/*  0 -> linear;   1 -> xlog;  2 -> ylog;   3-> polar;  4 -> polar log;
   	5 -> error fn; 6 -> Smith Chart;   7 -> Bar chart  */
G_EXTERN int axis_flgq;			/* flag which controls which axes get drawn */
G_EXTERN int xgrid_flgq;
G_EXTERN int ygrid_flgq;
G_EXTERN int xfgrid_flgq;
G_EXTERN int yfgrid_flgq;
G_EXTERN struct crossdatq crossq;
G_EXTERN int wplane_flgq;
G_EXTERN char *tkfnameq;			/* TKF file name root */
G_EXTERN FILE *tkf_pointerq;					/*WRITE FILE*/
G_EXTERN FILE *tofileq;			   	/*OFFSET FILE*/
G_EXTERN long tkf_offsetq;
G_EXTERN long tkf_incrementq;
G_EXTERN long tkf_inc_saveq;
G_EXTERN FILE *tkf_pointer_saveq;
G_EXTERN int notekq;
G_EXTERN int sim_flgq;
G_EXTERN int xpageq;		/* pixel size of virtual page in the x dir */
G_EXTERN int ypageq;		/* pixel size of virtual page in the y dir */
G_EXTERN int xbaseq;				/* pixel location of left edge of virtual page */
G_EXTERN int ybaseq;				/* pixel location of lower edge of virtual page */
G_EXTERN int acrossq;		/* pixel size of full page in the x dir */
G_EXTERN int topq;		/* pixel size of full page in the y dir */

/********************** SCALING FACTORS ******************************/
G_EXTERN float xfact_pgq;			/* 4096 / (x page size) */
G_EXTERN float yfact_pgq;			/* 3133 / (y page size) */
G_EXTERN float xfact_pltq;			/* pixels per x user unit on plot */
G_EXTERN float yfact_pltq;			/* pixels per y user unit on plot */
G_EXTERN float zfact_pltq;
G_EXTERN double x_log_baseq;
G_EXTERN double y_log_baseq;

G_EXTERN int xlenq;
G_EXTERN int ylenq;
G_EXTERN int zlenq;
G_EXTERN float x3axisq;		/*LENGTHS OF THE AXES IN FLOATING PIXELS*/
G_EXTERN float y3axisq;		/*LENGTHS OF THE AXES IN FLOATING PIXELS*/
G_EXTERN int xphysorq;
G_EXTERN int yphysorq;
G_EXTERN int xzeroq;
G_EXTERN int yzeroq;
G_EXTERN float xphysq;
G_EXTERN float yphysq;
G_EXTERN float xorigq;
G_EXTERN float yorigq;
G_EXTERN float zorigq;
G_EXTERN float xstepq;
G_EXTERN float ystepq;
G_EXTERN float xmaxq;
G_EXTERN float ymaxq;
G_EXTERN float zmaxq;
G_EXTERN int nxfineq;			/* number of fine x divisions */
G_EXTERN int nyfineq;			/* number of fine y divisions */
G_EXTERN int borderq;			/* space allowance between edge and labels */
G_EXTERN int titloffq;
G_EXTERN int xlspcq;			/* space between axes and labels on x_axis */
G_EXTERN int ylspcq;			/* space between axes and labels on y_axis */
G_EXTERN int zlspcq;			/* space between axes and labels on z_axis */
G_EXTERN int tkhtq;				/* height for tick marks */
G_EXTERN int labspq;			/* label spacing */

G_EXTERN int xl_cropq;			/* Crop boundary used by cropq(). */
G_EXTERN int yl_cropq;
G_EXTERN int xh_cropq;
G_EXTERN int yh_cropq;

G_EXTERN float lablhtq;			/* Height of the tick labels. */
G_EXTERN float titlhtq;			/* Height of the tick marks. */
G_EXTERN float x_namhtq;		/* x axis name height. */
G_EXTERN float y_namhtq;		/* y axis name height. */
G_EXTERN char plot3q;			/* Flag to control hidden line actions., */
G_EXTERN struct HORIZON horizq;	/* 3-D UPPER AND LOWER HORIZONS */
G_EXTERN GTICK_POS *x_tickposq;
G_EXTERN GTICK_POS *y_tickposq;
G_EXTERN GTICK_POS *z_tickposq;

/* Structure containing the values for converting (x,y,z) in 3-D space
to (x,y) in screen space. */
G_EXTERN struct D_3_CONVERSION_FACTORS d3_convertq;

/* HORIZON SEPARATING TOP FROM BOTTOM OF SURFACE*/
G_EXTERN int *horizcq;

G_EXTERN int surface_clrq;		/* TOP AND BOTTOM SURFACE COLORS */
G_EXTERN char d3_clrflgq;		/* SET TOP OR BOTTOM COLOR ? */

/* Value of the perpenticular plane when projecting a 2-D plane. */
G_EXTERN float axposq;
G_EXTERN int tcurveq;			/* Line thickness for curves */

G_EXTERN int x0q;				/* Global line starting end */
G_EXTERN int y0q;

G_EXTERN int n_legq;			/* index into arrays of legend entries. */
G_EXTERN int strcntq;
G_EXTERN int *styleq;			/* LINE AND SYMBOL STYLES FOR LEGEND */
G_EXTERN int *symclrq;			/* COLORS FOR LEGEND ENTRIES */

G_EXTERN int hxoldq;			/* TKF encoding variables */
G_EXTERN int hyoldq;
G_EXTERN int lyoldq;
G_EXTERN int lsxoldq;
G_EXTERN int lsyoldq;

G_EXTERN struct GDRQ gdrq[3];   /* DRIVER NAMES AND LOCATIONS */
G_EXTERN int lab_flgq;			/* Indicates user string labels. */

/* PARAMETERS FOR POLAR PLOTS */
G_EXTERN int pixrq;			/* Number of pixels in one radial division */
G_EXTERN float r_polminq;	/* Minimum value of log polar plot */
G_EXTERN float rfact_polq;	/* Number of pixels per user unit */
G_EXTERN int x0_polq;		/* Pixel coordinates of polar origin */
G_EXTERN int y0_polq;

/* TRIANGLE PLOTS */
G_EXTERN int tri_lenq;
G_EXTERN float tri_factq;
G_EXTERN char tri_plotq;
/* POLAR CONTOURS */
G_EXTERN int polarq;

/* PLAY.C */
G_EXTERN int abortq;

/* LABSET.C */
G_EXTERN char **xstrlbq;
G_EXTERN char **ystrlbq;
G_EXTERN char **zstrlbq;
G_EXTERN int x_angq;
G_EXTERN int y_angq;
G_EXTERN int z_angq;

/* LEGEND.C */
G_EXTERN int xlegq;
G_EXTERN int ylegq;
G_EXTERN int legwidq;
G_EXTERN int leg_txtclr;
G_EXTERN int iq;
G_EXTERN int drawb;
G_EXTERN int *curve_thickness;

/* LPLT.C */
G_EXTERN struct LINESTART *line_start;
G_EXTERN struct GXYPAIRSQ *line_end;

/* MESSAGES.C */
G_EXTERN char in_gmessageq;
G_EXTERN char *mesgptrq;
G_EXTERN char mesgposq;

/* SVC.C */
G_EXTERN char dotq;
G_EXTERN char filled_ellipseq;
G_EXTERN int ellipse_patternq;
G_EXTERN char ellipse_borderq;
G_EXTERN int symcntq;
G_EXTERN int symfillq;

/* TEKINT */
G_EXTERN int printingq;
G_EXTERN int y00txtq;
G_EXTERN int y11txtq;
G_EXTERN char replayq;
G_EXTERN int newchq;
G_EXTERN char clookq;
G_EXTERN char commaq;
G_EXTERN char gflagq;                      /* Signals OK to do a scrntypeq('g'). */
G_EXTERN int xtextq;
G_EXTERN int ytextq;
G_EXTERN int numtq;
G_EXTERN int dentxtq;
G_EXTERN int x0txtq;
G_EXTERN int x1txtq;
G_EXTERN int ydtxtq;
G_EXTERN unsigned char hxq;
G_EXTERN unsigned char hyq;
G_EXTERN unsigned char lyq;
G_EXTERN unsigned char extraq;
G_EXTERN unsigned char extrabq;
G_EXTERN char insertq;
G_EXTERN int cvalq[30][3];
G_EXTERN int beep_frequencyq;
G_EXTERN int beep_durationq;

/* TPLT.C */
G_EXTERN int printer_choice;
G_EXTERN char ascgq;

G_EXTERN struct TitlDatq titleq;
G_EXTERN struct TitlDatq xnameq;
G_EXTERN struct TitlDatq ynameq;
G_EXTERN struct TitlDatq anameq;
G_EXTERN struct TitlDatq bnameq;
G_EXTERN struct TitlDatq cnameq;
G_EXTERN int NUM_OF_FONTS;
G_EXTERN char *fntptr[FNTNUM];	/*FONT NAMES*/
G_EXTERN unsigned char escsym[FNTNUM];
G_EXTERN struct G_FONT g_fontq[FNTNUM]; /* STORAGE FOR FONT SIZE DATA */

	/* GRAPHIC FUNCTION PACKETS */
G_EXTERN struct GINITQ g_iq[3];
G_EXTERN struct GSCRNTYPEQ g_scq;
G_EXTERN struct GCLEARQ g_clsq;
G_EXTERN struct GDOTQ g_dotq;
G_EXTERN struct GLINEQ g_lq;
G_EXTERN struct GPANELQ g_pq;
G_EXTERN struct GTEXTQ g_tq;
G_EXTERN struct GCHARQ g_cq;
G_EXTERN struct GTERMINATEQ g_quitq;
G_EXTERN struct GPRINTQ g_prntq;
G_EXTERN struct GFONLYQ g_fonlyq;
G_EXTERN struct GFILEOPENQ g_fopenq;
G_EXTERN struct GFILECLOSEQ g_fcloseq;
G_EXTERN struct GWRITEFILEQ g_fwriteq;
G_EXTERN struct CURPOS g_cposq;
G_EXTERN struct GDUMPQ g_dumpq;
G_EXTERN struct GZOOMQ g_zq;
G_EXTERN struct GERCHARQ g_erq;
G_EXTERN struct G_GET_CHAR g_get_charq;
G_EXTERN struct G_PAGE_SIZE g_page_sizeq;
G_EXTERN struct G_PRINT_STRING g_print_stringq;
#if GPC_OS2
G_EXTERN int convert_flgq;
G_EXTERN struct PRINT_DATA print_dataq;
#endif
#if GPC_WIN
G_EXTERN int convert_flgq;
G_EXTERN struct G_REDRAWQ g_rdq;
#endif


#if GPC_UNX
G_EXTERN int rfposq;
G_EXTERN int rfincq;

#else
G_EXTERN GPC_LIBPROC ftableq;
#endif

/**************** ANSI C / C++ function prototypes **************/
#include <proto.h>
#if GPC_CPP
#include <arrow.h>
#include <axesdraw.h>
#include <bothplt.h>
#include <boxfill.h>
#include <cntaux.h>
#include <cntplt.h>
#include <color.h>
#include <d3plt.h>
#include <erfplt.h>
#include <fntplt.h>
#include <insert.h>
#include <logplt.h>
#include <lplt.h>
#include <matsolve.h>
#include <panfil.h>
#include <pclp.h>
#include <polplt.h>
#include <position.h>
#include <r3plt.h>
#include <rfit.h>
#include <simplt.h>
#include <smith.h>
#include <smooth.h>
#include <tekint.h>
#if GPC_DOS
#include <bauxdos.h>
#include <bplt.h>
#include <convert.h>
#include <cursor.h>
#include <memory1.h>
#include <tplt.h>
#include <zcursor.h>
#elif GPC_EX2
#include <baux286.h>
#include <bplt.h>
#include <convert.h>
#include <cursor.h>
#include <memory2.h>
#include <tplt.h>
#include <zcursor.h>
#endif
#endif

#if defined(MULTITHREAD) || GPC_CPP
};
#endif
#if GPC_OS2
extern int instanceq;
#endif

/************ TYPEDEFS FOR MEMBER FUNCTIONS *********************/
typedef	char * (GCLASS * PLABFUNC)(int, char **, int);

#if GPC_OS2 || !GPC_CPP
/**************** GraphiC function prototypes *******************/
#include <proto.h>
#endif

#if GPC_OS2
extern PVOID G_Base_Memq;
#endif

#ifdef MULTITHREAD
extern struct G_GLOBAL_DATA *g_dataq[];
#define g_datq g_dataq[instctr]

/*
Global variable names are defined as members of an array of data
structures. Each instance of bgnplot() allocates a new array and ties the
instance to the correct offset into the array.
*/

/************************ BOOLEAN FLAGS *************************/
#define gtextq g_datq->gtextq
#define psetflgq g_datq->psetflgq
#define size_flagq g_datq->size_flagq
#define demoq g_datq->demoq
#define leg_flgq g_datq->leg_flgq
#define panfilq g_datq->panfilq
#define pattern_saveq g_datq->pattern_saveq
#define lcolor_saveq g_datq->lcolor_saveq
#define crop_flgq g_datq->crop_flgq
#define in_plbl3q g_datq->in_plbl3q
#define penup_flgq g_datq->penup_flgq
#define rotate_flgq g_datq->rotate_flgq
#define mouse_flgq g_datq->mouse_flgq
#define has_beepedq g_datq->has_beepedq
#define adding_flgq g_datq->adding_flgq

/********************** BIT FLAGS ****************************/
#define frame_flgq g_datq->frame_flgq
#define right_flgq g_datq->right_flgq
#define viewers_quadq g_datq->viewers_quadq

#define ascq g_datq->ascq
#define symhtq g_datq->symhtq
#define labscalq g_datq->labscalq
#define cmd_strq g_datq->cmd_strq
#define stringq g_datq->stringq
#define cl_valq g_datq->cl_valq
#define pix_convertq g_datq->pix_convertq
#define itotekq g_datq->itotekq
#define PSlinewq g_datq->PSlinewq
#define PSfntarrayq g_datq->PSfntarrayq
#define NCOLORQ g_datq->NCOLORQ
#define win_numq g_datq->win_numq
#define dev_typeq g_datq->dev_typeq
#define win_structq g_datq->win_structq
#define add_inq g_datq->add_inq

/************ UNINITIALIZED GLOBAL VARIABLES ***********/

#define plot_typeq g_datq->plot_typeq
#define axis_flgq g_datq->axis_flgq
#define grid_flgq g_datq->grid_flgq
#define xgrid_flgq g_datq->xgrid_flgq
#define ygrid_flgq g_datq->ygrid_flgq
#define xfgrid_flgq g_datq->xfgrid_flgq
#define yfgrid_flgq g_datq->yfgrid_flgq
#define crossq g_datq->crossq
#define wplane_flgq g_datq->wplane_flgq
#define tkf_pointerq g_datq->tkf_pointerq
#define tofileq g_datq->tofileq
#define tkf_offsetq g_datq->tkf_offsetq
#define tkf_incrementq g_datq->tkf_incrementq
#define tkf_inc_saveq g_datq->tkf_inc_saveq
#define tkf_pointer_saveq g_datq->tkf_pointer_saveq
#define sim_flgq g_datq->sim_flgq
#define notekq g_datq->notekq
#define xpageq g_datq->xpageq
#define ypageq g_datq->ypageq
#define xbaseq g_datq->xbaseq
#define ybaseq g_datq->ybaseq
#define acrossq g_datq->acrossq
#define topq g_datq->topq
#define tkfnameq g_datq->tkfnameq

/********************** SCALING FACTORS ******************************/
#define xfact_pgq g_datq->xfact_pgq
#define yfact_pgq g_datq->yfact_pgq
#define xfact_pltq g_datq->xfact_pltq
#define yfact_pltq g_datq->yfact_pltq
#define zfact_pltq g_datq->zfact_pltq
#define x_log_baseq g_datq->x_log_baseq
#define y_log_baseq g_datq->y_log_baseq

#define xlenq g_datq->xlenq
#define ylenq g_datq->ylenq
#define zlenq g_datq->zlenq
#define x3axisq g_datq->x3axisq
#define y3axisq g_datq->y3axisq
#define xphysorq g_datq->xphysorq
#define yphysorq g_datq->yphysorq
#define xzeroq g_datq->xzeroq
#define yzeroq g_datq->yzeroq
#define xphysq g_datq->xphysq
#define yphysq g_datq->yphysq
#define xorigq g_datq->xorigq
#define yorigq g_datq->yorigq
#define zorigq g_datq->zorigq
#define xstepq g_datq->xstepq
#define ystepq g_datq->ystepq
#define xmaxq g_datq->xmaxq
#define ymaxq g_datq->ymaxq
#define zmaxq g_datq->zmaxq
#define nxfineq g_datq->nxfineq
#define nyfineq g_datq->nyfineq
#define borderq g_datq->borderq
#define titloffq g_datq->titloffq
#define xlspcq g_datq->xlspcq
#define ylspcq g_datq->ylspcq
#define zlspcq g_datq->zlspcq
#define tkhtq g_datq->tkhtq
#define labspq g_datq->labspq
#define xl_cropq g_datq->xl_cropq
#define yl_cropq g_datq->yl_cropq
#define xh_cropq g_datq->xh_cropq
#define yh_cropq g_datq->yh_cropq
#define lablhtq g_datq->lablhtq
#define titlhtq g_datq->titlhtq
#define x_namhtq g_datq->x_namhtq
#define y_namhtq g_datq->y_namhtq
#define plot3q g_datq->plot3q
#define horizq g_datq->horizq
#define x_tickposq g_datq->x_tickposq
#define y_tickposq g_datq->y_tickposq
#define z_tickposq g_datq->z_tickposq
#define d3_convertq g_datq->d3_convertq
#define horizcq g_datq->horizcq
#define surface_clrq g_datq->surface_clrq
#define d3_clrflgq g_datq->d3_clrflgq
#define axposq g_datq->axposq
#define n_legq g_datq->n_legq
#define strcntq g_datq->strcntq
#define styleq g_datq->styleq
#define symclrq g_datq->symclrq
#define hxoldq g_datq->hxoldq
#define hyoldq g_datq->hyoldq
#define lyoldq g_datq->lyoldq
#define lsxoldq g_datq->lsxoldq
#define lsyoldq g_datq->lsyoldq

#define gdrq g_datq->gdrq

#define lab_flgq g_datq->lab_flgq
/* PARAMETERS FOR POLAR PLOTS */
#define pixrq g_datq->pixrq
#define r_polminq g_datq->r_polminq
#define rfact_polq g_datq->rfact_polq
#define x0_polq g_datq->x0_polq
#define y0_polq g_datq->y0_polq
#define tcurveq g_datq->tcurveq
#define x0q g_datq->x0q
#define y0q g_datq->y0q
/* TRIANGLE PLOTS */
#define tri_lenq g_datq->tri_lenq
#define tri_factq g_datq->tri_factq
#define tri_plotq g_datq->tri_plotq
/* POLAR CONTOURS */
#define polarq g_datq->polarq

/* BAUX???.C */
#define fullpathq g_datq->fullpathq

/* PLAY.C */
#define abortq g_datq->abortq

/* LABSET.C */
#define xstrlbq g_datq->xstrlbq
#define ystrlbq g_datq->ystrlbq
#define zstrlbq g_datq->zstrlbq
#define x_angq g_datq->x_angq
#define y_angq g_datq->y_angq
#define z_angq g_datq->z_angq

/* LEGEND.C */
#define xlegq g_datq->xlegq
#define ylegq g_datq->ylegq
#define legwidq g_datq->legwidq
#define leg_txtclr g_datq->leg_txtclr
#define iq g_datq->iq
#define drawb g_datq->drawb
#define curve_thickness g_datq->curve_thickness

/* LPLT.C */
#define line_start g_datq->line_start
#define line_end g_datq->line_end

/* MESSAGES.C */
#define in_gmessageq g_datq->in_gmessageq
#define mesgptrq g_datq->mesgptrq
#define mesgposq g_datq->mesgposq

/* SVC.C */
#define dotq g_datq->dotq
#define filled_ellipseq g_datq->filled_ellipseq
#define ellipse_patternq g_datq->ellipse_patternq
#define ellipse_borderq g_datq->ellipse_borderq
#define symcntq g_datq->symcntq
#define symfillq g_datq->symfillq

/* TEKINT */
#define printingq g_datq->printingq
#define y00txtq g_datq->y00txtq
#define y11txtq g_datq->y11txtq
#define replayq g_datq->replayq
#define newchq g_datq->newchq
#define clookq g_datq->clookq
#define commaq g_datq->commaq
#define gflagq g_datq->gflagq
#define xtextq g_datq->xtextq
#define ytextq g_datq->ytextq
#define numtq g_datq->numtq
#define dentxtq g_datq->dentxtq
#define x0txtq g_datq->x0txtq
#define x1txtq g_datq->x1txtq
#define ydtxtq g_datq->ydtxtq
#define hxq g_datq->hxq
#define hyq g_datq->hyq
#define lyq g_datq->lyq
#define extraq g_datq->extraq
#define extrabq g_datq->extrabq
#define insertq g_datq->insertq
#define cvalq g_datq->cvalq
#define beep_frequencyq g_datq->beep_frequencyq
#define beep_durationq g_datq->beep_durationq

/* TPLT.C */
#define printer_choice g_datq->printer_choice
#define ascgq g_datq->ascgq

#define titleq g_datq->titleq
#define xnameq g_datq->xnameq
#define ynameq g_datq->ynameq
#define anameq g_datq->anameq
#define bnameq g_datq->bnameq
#define cnameq g_datq->cnameq
#define fntptr g_datq->fntptr
#define NUM_OF_FONTS g_datq->NUM_OF_FONTS
#define escsym g_datq->escsym
#define g_fontq g_datq->g_fontq
#define ftableq g_datq->ftableq

	/* FUNCTION PACKETS */
#define g_iq g_datq->g_iq
#define g_scq g_datq->g_scq
#define g_clsq g_datq->g_clsq
#define g_dotq g_datq->g_dotq
#define g_lq g_datq->g_lq
#define g_pq g_datq->g_pq
#define g_tq g_datq->g_tq
#define g_cq g_datq->g_cq
#define g_quitq g_datq->g_quitq
#define g_prntq g_datq->g_prntq
#define g_fonlyq g_datq->g_fonlyq
#define g_fopenq g_datq->g_fopenq
#define g_fcloseq g_datq->g_fcloseq
#define g_fwriteq g_datq->g_fwriteq
#define g_cposq g_datq->g_cposq
#define g_dumpq g_datq->g_dumpq
#define g_zq g_datq->g_zq
#define g_erq g_datq->g_erq
#define g_get_charq g_datq->g_get_charq
#define g_print_stringq g_datq->g_print_stringq
#define g_page_sizeq g_datq->g_page_sizeq
#define print_dataq g_datq->print_dataq

#define convert_flgq g_datq->convert_flgq
#ifndef GRAPHIC_CMN_INCLUDED
#endif
#endif

#if GPC_UNX
/*
extern int *ftableq;
void (*ctrl_c_savq)();
void (*float_err_savq)();
unsigned sleep();
unsigned alarm();
void exit();
void perror();
char *getenv();
char tkf_fileq[100];
char tkf_input;
int errno;
*/
#endif
/* The following comment is for using PC-Lint (Gimpel Software) with GraphiC */
/*lint +e??? -e935 -e950 -e923 -e932 -e740 -e950 -e915 -e919 -e922 -e912
-e918 -e619 -e925 -e917 -e929 -e934 -e747 -e911 -e766 -e921 -eau -e701 -e703
-e790 -e974 -e942 -e737 -e732 -e123 -e713 -e722 -e775 -e920 -e534 -e928 -e941
-e776 -e702 -e931 -e704 -e926 -e736
*/

#if TCQ
#pragma hdrstop
#endif

#include <colormap.h>

#define svg_crop svgcrop
#define svg_open svgopen
#define svg_push svgpush

#define GRAPHIC_H_INCLUDED
#endif
