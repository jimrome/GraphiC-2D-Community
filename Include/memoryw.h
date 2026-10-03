#ifndef MEMORYW_H_INCLUDED

/********************* variable definitions ************************/
#ifdef TESTMEM
#ifdef __cplusplus
G_STATIC struct ELEM *top;
#else
G_STATIC struct ELEM *top = NULL;
#endif
#endif

#define MEMORYW_H_INCLUDED
#endif

