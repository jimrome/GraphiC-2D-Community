#ifndef PCLP_H_INCLUDED

#ifdef PANEL
/************** Function Declarations *********************/
int pan_clipq(PPGXYPAIRS xyp, int *index, int *npanels);
int clip_panq(int *xpan, int *ypan, int plines,
	struct edge_pts **pan_pieces,
	int win_num, int n_arrays, int dir_flg);
int newpanq(int **x, int **y,
	int n_pieces, struct edge_pts *pieces, int *n, int dir);
void bounding_boxq(PGXYPAIRS xyp, int num_panels,
	int *xl, int *yl, int *xh, int *yh);
int save_pointq(int x, int y);
int connectq(int side1, int side2, PGXYPAIRS corner, int dir);
int crossing_testq(int *x, int *y, int plines);
int arrangepq(
	PPGXYPAIRS xypi,/* Pointer to closed polygon pieces */
	int *dim,				/* # array elements */
	int *np					/* Number of pieces */
	);
int pan_on_clip_regionq(PGXYPAIRS xyp);
int ccw_testq(PGXYPAIRS xyp);
void rev_polyq(
	PGXYPAIRS xyp	/* Pointer to self-counting closed polygon */
);
int hole_testq(
	PGXYPAIRS xy1,	/* Pointer to closed (outer) polygon */
	PGXYPAIRS xy2	/* Pointer to closed (outer) polygon */
);

int find_sideq(int perimeter);
void split_holesq(int *pnumber);

#endif

#define PCLP_H_INCLUDED
#endif
