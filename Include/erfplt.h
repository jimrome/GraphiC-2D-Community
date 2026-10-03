#ifndef ERFPLT_H_INCLUDED

/************************ variables ********************************/
G_STATIC int xend, yend, ypose;

/************************ function declarations ********************/
void ysetq(float ymin, float ystep, float ymax, int *nystep, char *yformat);
void erf_grid_lineq(float percent, int major);
void erf_labelq(float percent, char *format, int justify);
void erf_tickq(int xpos);
int xposiq(float percnt);
double erfq(double percnt);
double inv_erfq(double x);
int yupq(int nxstep, char *xformat, int npix, float orig,
	int start, PLABFUNC lab, int lab_flag);

#define ERFPLT_H_INCLUDED
#endif
