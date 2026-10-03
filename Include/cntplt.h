#ifndef CNTPLT_H_INCLUDED
#define CNTPLT_H_INCLUDED

#ifdef MULTITHREAD
struct gpcCntq {
#endif

G_STATIC int row;
G_STATIC int col;
G_STATIC char formc[LABLEN];
G_STATIC char formtc[LABLEN];
G_STATIC float **z_grid;
G_STATIC unsigned char **cell;
G_STATIC int *xc;
G_STATIC int *yc;
G_STATIC float *glevel;
G_STATIC int *xi;
G_STATIC int *yi;
G_STATIC float zk;
G_STATIC int xdim;
G_STATIC int ydim;
G_STATIC int seg_count;			/* Counts all contour elements this level. */
G_STATIC char no_lab;
G_STATIC unsigned int entry;
G_STATIC int pc;
G_STATIC int pmc;
G_STATIC struct labpos pmcs[10];
G_STATIC int lab_index;
G_STATIC int xlc;
G_STATIC int ybc;
G_STATIC int xrc;
G_STATIC int ytc;
G_STATIC int lab_allow;
G_STATIC int lev_num;
G_STATIC int nlevels;
G_STATIC int intpart;
G_STATIC int decpart;
G_STATIC float lab_hgt;
G_STATIC int lab_width;
G_STATIC int lab_shft;
G_STATIC int left_allow;
G_STATIC int rt_allow;
G_STATIC int *sizeptrq;
G_STATIC float **getx;
G_STATIC float **gety;
G_STATIC float save_level;

#ifdef PANEL
G_STATIC int cnt_count;			/* Counts contours this level. */
G_STATIC char first_time;		/* Sends initialization to set_paramq */
G_STATIC int start_s;			/* Uphill direction at the starting edge */
G_STATIC int end_s;			/* Uphill direction at the ending edge. */
#endif

#ifdef MULTITHREAD
} *gpcCntq[GPC_MAX_THREADS];

char stackq[GPC_MAX_THREADS];
char cnt_clr_methodq[GPC_MAX_THREADS];
int *line_styleq[GPC_MAX_THREADS];			/* Pointer to the line style array. */
int *cntclrq[GPC_MAX_THREADS];				/* Pointer to the color array */
int cntclrbq[GPC_MAX_THREADS];				/* Draw a border around shaded contours? */

#define row gpcCntq[instctr]->row
#define col gpcCntq[instctr]->col
#define formc gpcCntq[instctr]->formc
#define formtc gpcCntq[instctr]->formtc
#define z_grid gpcCntq[instctr]->z_grid
#define cell gpcCntq[instctr]->cell
#define xc gpcCntq[instctr]->xc
#define yc gpcCntq[instctr]->yc
#define glevel gpcCntq[instctr]->glevel
#define xi gpcCntq[instctr]->xi
#define yi gpcCntq[instctr]->yi
#define zk gpcCntq[instctr]->zk
#define xdim gpcCntq[instctr]->xdim
#define ydim gpcCntq[instctr]->ydim
#define seg_count gpcCntq[instctr]->seg_count
#define no_lab gpcCntq[instctr]->no_lab
#define entry gpcCntq[instctr]->entry
#define pc gpcCntq[instctr]->pc
#define pmc gpcCntq[instctr]->pmc
#define pmcs gpcCntq[instctr]->pmcs
#define lab_index gpcCntq[instctr]->lab_index
#define xlc gpcCntq[instctr]->xlc
#define ybc gpcCntq[instctr]->ybc
#define xrc gpcCntq[instctr]->xrc
#define ytc gpcCntq[instctr]->ytc
#define lab_allow gpcCntq[instctr]->lab_allow
#define lev_num gpcCntq[instctr]->lev_num
#define nlevels gpcCntq[instctr]->nlevels
#define intpart gpcCntq[instctr]->intpart
#define decpart gpcCntq[instctr]->decpart
#define lab_hgt gpcCntq[instctr]->lab_hgt
#define lab_width gpcCntq[instctr]->lab_width
#define lab_shft gpcCntq[instctr]->lab_shft
#define left_allow gpcCntq[instctr]->left_allow
#define rt_allow gpcCntq[instctr]->rt_allow
#define sizeptrq gpcCntq[instctr]->sizeptrq
#define getx gpcCntq[instctr]->getx
#define gety gpcCntq[instctr]->gety
#define save_level gpcCntq[instctr]->save_level
#define cnt_count gpcCntq[instctr]->cnt_count
#define first_time gpcCntq[instctr]->first_time
#define start_s gpcCntq[instctr]->start_s
#define end_s gpcCntq[instctr]->end_s

#define stackq stackq[instctr]
#define cnt_clr_methodq cnt_clr_methodq[instctr]
#define line_styleq line_styleq[instctr]
#define cntclrq cntclrq[instctr]
#define cntclrbq cntclrbq[instctr]
#else

char stackq;
char cnt_clr_methodq;
int *line_styleq;
int *cntclrq;
int cntclrbq;

#endif

void pconvq(int x, int y, int *xp, int *yp);
void z_prepq(float);
int startq(void);
void traceq(void);
void getrtq(int side);
void cell_markq(void);
void diag_rtq(void);
void draw_contq(void);
void drawitq(void);
void cntlabq(void);
int getlabspaceq(void);
void put_labelsq(void);
int getpmcq(void);
void load_edge_contourq(void);
void get_start_endq(void);

#endif
