#ifndef CNTAUX_H_INCLUDED

#ifdef MULTITHREAD
struct CNT_AUX {
#endif

G_STATIC PSTORCNT *cnt_s;
G_STATIC int n_levels;		/* The number of contour levels */
G_STATIC int x_min;			/* The pixel limits of the plot */
G_STATIC int x_max;
G_STATIC int y_min;
G_STATIC int y_max;
G_STATIC int pca;			/* The number of line elements in a contour */
G_STATIC int tog;			/* Toggle for sharing cnt_s */
G_STATIC int tog_tem;		/* Temporary tog for connecting cnts */
G_STATIC int cnt_counta;	/* The # of contours at the upper level */
G_STATIC int count[2];		/* The # of contours at the current level */
G_STATIC int max_pos;		/* The maximum perimeter value */
G_STATIC int cnr[4];		/* The perimeter value at the corners */
G_STATIC int x_cnr[4];		/* The pixel corner values */
G_STATIC int y_cnr[4];
G_STATIC char direction;	/* Indicates the direction of tracing. */
G_STATIC int sense;			/* The direction to the next border cnt */
G_STATIC int ind;			/* The index for the current contour */
G_STATIC int old_pos;		/* The perim position before connecting */
G_STATIC int pos;			/* The perimeter position of the new contour */
G_STATIC int del_cw;		/* The perimeter distance to next contour */
G_STATIC int del_ccw;
G_STATIC int c_index;		/* Index for the pattern */

#ifdef MULTITHREAD
} *cnt_aux[GPC_MAX_THREADS];

extern char cnt_clr_methodq[];
extern int *cntclrq[];
extern int cntclrbq[];

#define cnt_s cnt_aux[instctr]->cnt_s
#define n_levels cnt_aux[instctr]->n_levels
#define x_min cnt_aux[instctr]->x_min
#define x_max cnt_aux[instctr]->x_max
#define y_min cnt_aux[instctr]->y_min
#define y_max cnt_aux[instctr]->y_max
#define pca cnt_aux[instctr]->pca
#define tog cnt_aux[instctr]->tog
#define tog_tem cnt_aux[instctr]->tog_tem
#define cnt_counta cnt_aux[instctr]->cnt_counta
#define count cnt_aux[instctr]->count
#define max_pos cnt_aux[instctr]->max_pos
#define cnr cnt_aux[instctr]->cnr
#define x_cnr cnt_aux[instctr]->x_cnr
#define y_cnr cnt_aux[instctr]->y_cnr
#define direction cnt_aux[instctr]->direction
#define sense cnt_aux[instctr]->sense
#define ind cnt_aux[instctr]->ind
#define old_pos cnt_aux[instctr]->old_pos
#define pos cnt_aux[instctr]->pos
#define del_cw cnt_aux[instctr]->del_cw
#define del_ccw cnt_aux[instctr]->del_ccw
#define c_index cnt_aux[instctr]->c_index

#define cnt_clr_methodq cnt_clr_methodq[instctr]
#define cntclrq cntclrq[instctr]
#define cntclrbq cntclrbq[instctr]

#elif !GPC_CPP

extern char cnt_clr_methodq;
extern int *cntclrq;
extern int cntclrbq;
#endif

char next_cntq(void);
char otherq(void);
void cnr_procq(void);
void storeq(void);
int get_posq(int, int);
void edge_fixq(void);
void cnt_loadq(int *XC, int *YC, int pc, int cnt_num,
	int cell, int start, int end);
int get_senseq(float **z_grid, int row, int col, int xdim,
	int ydim, int root);

#define CNTAUX_H_INCLUDED
#endif
