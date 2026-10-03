#ifndef LOGPLT_H_INCLUDED

void axprepq(int axis, int ndiv, float *x, float *y, int npts);
void logscq(int axis, float orig1, float max1, char *format);
void lgaxisq(int axis, double pixold);
void tickq(int axis, int size, int pos);
void axlabq(int axis, int number, int pos, int type, int count);
int xupq(int nxstep, char *xformat, int npix, float orig,
	int start, PLABFUNC lab, int lab_flag);
int yupq(int nxstep, char *xformat, int npix, float orig,
	int start, PLABFUNC lab, int lab_flag);

#define LOGPLT_H_INCLUDED
#endif
