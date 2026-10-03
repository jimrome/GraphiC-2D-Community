#ifndef ARROW_H_INCLUDED

G_STATIC float sn, cs, xfrom, yfrom, xto, yto, hlx, hly, hwx, hwy;
G_STATIC int int2x, int2y, int3x, int3y, t0, t1;

void vectorq(int x0, int y0, int x1, int y1, float ltow, float size,
	char *type);
void arodrawq(int end, int type);

#define ARROW_H_INCLUDED
#endif
