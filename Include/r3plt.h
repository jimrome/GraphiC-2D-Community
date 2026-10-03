#ifndef R3PLT_H_INCLUDED

G_STATIC int **cellr;		/* Will be an array of PTS_PER_CELL arrays,
							each having a member for each cell. Each member
							points to a data set lying within the cell. */
int off;									/* offsets */
int sector;								/* search sector */
int resetc;							/* Reset getoffq() & arbq() */
int first;
#if GPC_CPP
G_STATIC float tol;
#else
G_STATIC float tol = (float)3.;
#endif

int getoffq(int i, int j, int nx, int ny);
int arbq(int tind, float *xp, float *yp, float *zp,
	float *xp1, float *yp1, float *zp1, float xg, float yg);
int solveq(float *x, float *y, float *z, float xg, float yg, float *ansp);
int chkdisq(float *xp, float *yp);

#define R3PLT_H_INCLUDED
#endif
