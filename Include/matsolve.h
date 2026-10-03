#ifndef MATSOLVE_H_INCLUDED

void lu_dcompq(double **a, int n, int *index);
void lu_bksubq(double **a, int n, int *index, double *b);

#define MATSOLVE_H_INCLUDED
#endif
