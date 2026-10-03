#ifndef D3PLT_H_INCLUDED

G_STATIC GPC_FUNQ fnq;

/* THESE NON STATIC VARIABLES ARE USED WITH BOXFILL.C */
float v1localq, v2localq;	/* Global base coordinates */
int ilocalq, jlocalq;		/* Global base indices */
int v1ptsq, v2ptsq;			/* Number of mesh points */
float delta1, delta2;		/* Deltas in v1 and v2 coordinates */
int i1pts, i2pts;			/* Number of sub-mesh points */
int xcalq, ycalq;			/* Picture plane coordinates */
float clpmaxq, clpminq;		/* For clipping extreme z values */
char matrix3q;				/* Matrix mode */
#if GPC_CPP
char tplot3q;				/* Used with plot3q	to set hidden l. */
#else
char tplot3q = 0;
#endif
int cindex3q;				/* Used to indicate filled surface */

G_STATIC float **zmatq;	/* Global pointer to z matrix */
G_STATIC float zstepq;				/* Global z step */
G_STATIC float v1factq, v2factq;		/* Scaling factors user to pixel */
G_STATIC Uint fixq, sclfixq;			/* Inhibit scaling. */
G_STATIC float aq, bq, cq;			/* View coordinates */
G_STATIC int a3c;						/* COLOR USED FOR AXES - set in graf3d() */
G_STATIC int usrvuq;					/* For chosing cartesian or angle view */
G_STATIC char axesdrawn;				/* Is 1 after the axes are drawn. */
G_STATIC float v2m, v1m;				/* Pixel values at max ends */
G_STATIC float v2max, v1max, v2orig, v1orig, v2step, v1step; /* In v1-v2 space */
G_STATIC char *v2name = NULL;
G_STATIC char *v1name = NULL;
G_STATIC float d3barsq;				/* Flag for bar drawing */
G_STATIC float bxtopixq;
G_STATIC int zviewq;

#if GPC_CPP
G_STATIC int baseq;			/* Flag for drawing base */
G_STATIC char *x3nam;
G_STATIC char *y3nam;
G_STATIC char *z3nam;			/* Label string ptrs. */
G_STATIC float z3axisq;
char surtopq;					/*SHOW 3-D SURFACES*/
char surbotq;					/*SHOW 3-D SURFACES*/
#else
G_STATIC int baseq = ( -1);
G_STATIC char *x3nam = NULL;
G_STATIC char *y3nam = NULL;
G_STATIC char *z3nam = NULL;
G_STATIC float z3axisq = (float)10.0;
char surtopq = 1;
char surbotq = 1;
#endif

void surfacq(GPC_FUNQ, float **, int, float, int, float);
void plot3wq(void);
void getindq(int i, int j, int *k, int *m);
void autoscq(void);
void d3consq(float scale3);
void d3confq(float x, float y, float z, float *xmaxp,
	float *ymaxp, float *xminp, float *yminp);
void baseitq(GPC_FUNQ fun, float **zmat);
void d3axesq(void);
void ax3labq(void);
void plbl3q(float side, float max, float orig, float step,
	char *name, int axis);
void hinitq(float v1, float v2);
void bar3q(void);
void drawbarq(int i, int j, float v1, float v2, float bar1, float bar2);
float pcalcq(void);
void boxfillq(void);
void set_projectionq(void);
int xupq(int nxstep, char *xformat, int npix, float orig,
	int start, PLABFUNC lab, int lab_flag);
int yupq(int nxstep, char *xformat, int npix, float orig,
	int start, PLABFUNC lab, int lab_flag);

#define D3PLT_H_INCLUDED
#endif

