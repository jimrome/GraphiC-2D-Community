#ifndef TEKINT_H_INCLUDED

#if GPC_UNX
int from_file;
int first_line;
#elif GPC_WIN
int is_background_proc(void);
#endif

int decodeq(char *str);
void tekcodeq(char *tptr, int *x, int *y);
void getclrq(int index);
int get_tkf_charq(void);
void getxy_pairq(int *x, int *y);
void dispatch_pointq(int penup, int x, int y);
void tekfntq(void);
void magniq(int x, int y, int *xmg_ptr, int *ymg_ptr);
void rotatewq(int newseg);
long longmakeq(Uchar array[4]);

#define TEKINT_H_INCLUDED
#endif
