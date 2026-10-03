#ifndef TPLT_H_INCLUDED

#if GPC_UNX
#ifdef GPC_CPP
char from_file;
#else
char from_file = TRUE;
#endif
#endif

void do_zoomq(void);
char asc_testq(void);
void prnt_msgq(void);

#define TPLT_H_INCLUDED
#endif
