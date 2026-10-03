#ifndef AXESDRAW_H_INCLUDED

void tickdrwq(int x_y, int pos, int xp, int yp);
int get_lab_posq(int ang, float lablht, int above, int on_edge,
	int *xp, int *yp, int width, int spc, int physor, int tick,
	int zero, int count, int shift_flg);

#define AXESDRAW_H_INCLUDED
#endif
