#ifndef SIMPLT_H_INCLUDED

typedef struct plotparmblk {	/* STRUCTURE OF PLOT INFORMATION FOR */
	int xphysor, yphysor;		/* SIMULTANEOUS PLOTTING             */
	int xzero, yzero;
	int xpage, ypage;
	int xbase, ybase;
	unsigned int plot_type;
	int xlen, ylen;
	float xorig, yorig;
	float xmax, ymax;
	float xfact_plt;			/* pixels per x user unit on plot */
	float yfact_plt;			/* pixels per y user unit on plot */
	int linclr, linsty, linthk;
	int panclr, border, brdclr;
	int pattern_save;
	int lcolor_save;
	int x0, y0;
	int pfx, pfy;
	int hxold, hyold, lyold, lsxold, lsyold;
	float symht;				/* symbol height */
	int symnum;					/* symbol number */
	char penup;
	char dot;
	float lablht;
	int tkht;
	FILE *tkf_pointer_save;
#if GPC_OS2 || GPC_WIN
	GPCHANDLES gpch;
#endif
} PLOTPARMBLK;

typedef PLOTPARMBLK *PPLOTPARMBLK;

/************************ variable definitions *********************/
#if GPC_CPP
G_STATIC FILE **tmpfilesq;				 	/* TEMP FILE POINTERS */
G_STATIC char **tmpnamesq;					/* TEMP FILE NAMES */
G_STATIC FILE **tkf_pointer_save;
G_STATIC int plotnumq;						/* INDEX VARIABLE */
G_STATIC PPLOTPARMBLK plotparamsq;
#else
static FILE **tmpfilesq = NULL;
static char **tmpnamesq = NULL;
static FILE **tkf_pointer_save = NULL;
static int plotnumq = 0;
static PPLOTPARMBLK plotparamsq;
#endif

/************************ function declarations ********************/
void saveplotq(void);

#define SIMPLT_H_INCLUDED
#endif
