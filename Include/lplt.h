#ifndef LPLT_H_INCLUDED

#if GPC_WIN
int tkbcntq=0, tkbcnt2q=0;	/* tkf buffer counters */
#endif

void ldraw2q(int x1, int y1);
void remhidq(int x1, int y1);
void lfinishq(int x, int y);
void gridlinq(int x, int y);
void projected_lineq(int x1, int y1);
char *longsplitq(long num);

#define LPLT_H_INCLUDED
#endif
