#ifndef INSERT_H_INCLUDED

FILE *infileq;

void file_processq(void);
int tek_processq(int mode);
int ingetseq(int mode);
void ins_magniq(int x, int y, int *xmg_ptr, int *ymg_ptr);

#define INSERT_H_INCLUDED
#endif
