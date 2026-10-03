#ifndef RFIT_H_INCLUDED

G_STATIC float aa, bb, s[5];
G_STATIC int nitsq, nnbrsq;
G_STATIC float *res;
G_STATIC float *temf;
G_STATIC float *wq;
G_STATIC float *xxq;
G_STATIC float *yyq;
G_STATIC float *xwq;
G_STATIC float *ya;

void getwtq(float *x, int i, int npts);
void getrq(float *s);
void dorobq(float *x, float *y, int npts);
void medianq(float *medp);
void h1sortq(float *x, int n);
void h2sortq(float *x, float *y, int n);

#define RFIT_H_INCLUDED
#endif
