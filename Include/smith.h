#ifndef SMITH_H_INCLUDED

G_STATIC float theta;
G_STATIC float thm, x, y, rr, r, th1, th2, delth;
G_STATIC int txtclrq, labcolor, lcolor1;
G_STATIC int xoff, yoff, toff;
G_STATIC float height, fact, page_fact;
G_STATIC float u, v;

void resdrawq(void);
void reactq(Uchar mode);
void smlabelq(int lcolor);
void scirctxtq(float x0, float y0, float r, char *string, float hgt,
	float angle);
void cdrawq(Uchar mode);
void scircleq(float x, float y, float r, float th1, float th2);
void sradlinq(float x, float y, float r1, float r2, float th);

#define SMITH_H_INCLUDED
#endif
