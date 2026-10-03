#ifndef BOXFILL_H_INCLUDED

#if GPC_CPP
struct VECTOR_SAV *pt_save_top;
struct VECTOR_SAV *pt_save_bot;
#else
struct VECTOR_SAV *pt_save_top = NULL;
struct VECTOR_SAV *pt_save_bot = NULL;
#endif
struct HORIZ_CLR *patch_horiz_upper;
struct HORIZ_CLR *patch_horiz_lower;
G_STATIC int cdimq;					/* Global size of color array */
G_STATIC int *carrayq;				/* Pointer to color array */
G_STATIC struct PATCH_LISTQ patlistq;
G_STATIC int draw_meshq;
G_STATIC float *zlevels;
G_STATIC int is_visible;
G_STATIC int x_right;

void box_procq(void);
void htraceq(int x0, int y0, int x1, int y1, int attr1, int attr2);
void single_patch_colorq(int *x, int *y, float *z);
void do_patch_colorsq(int *x_in, int *y_in, float *z);
void proc_patchq(int *x, int *y, float *z, int npts);
int TestPatchq(int *x, int *y);
int FillOutPatchq(int *x, int *y, float *z, int *level, int npts,
	int *x1, int *y1);
void GetIntersectionq(int *x, int *y, float *z, int index, float level,
	int *xp, int *yp);
void send_patchq(int *x, int *y, int index, int clr);
void FindStartPointq(int *x, int *level, int count, int *starts);
void remhid3dq(int order_flag);
void hsetq(int x, int y);
void load_patchq(int x, int y, int trans_flag);
void add_pointq(struct VECTOR_SAV **pt_save, int x, int y);
void del_pointq(struct VECTOR_SAV **pt_save, int x);
int get_next_ptq(int x, int top_flag);
void free_listq(void);
void triangulateq(int *x, int *y, float *z);
void do_3d_panelq(void);

#define BOXFILL_H_INCLUDED
#endif

