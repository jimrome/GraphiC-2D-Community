#ifndef BPLT_H_INCLUDED

void initialq(void);
void cltotkfq(void);
void win_procq(void);

#if GPC_UNX
char *getenv();
int asc_pipe[2], tkf_pipe[2];
#endif

#define BPLT_H_INCLUDED
#endif
