#ifndef SMOOTH_H_INCLUDED

G_STATIC int ng_out, nxg_out, nyg_out;

void splintq(float *xp, float *yp, float *y2p, int n, float *x1p,
	float *y1p);
int splineq(float *xp, float *yp, int n, float yp0, float ypn,
	float *y2p);
int splie2q(float *xap, float **zap, int m, int n,
	float * *z2ap);
int splin2q(float *xap, float *yap, float * *zap,
	float * *z2ap, int m, int n, float *x1p, float *y1p,
	float * *zn);

#define SMOOTH_H_INCLUDED
#endif
