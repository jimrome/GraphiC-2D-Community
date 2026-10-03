/* (c) 1993 by Scientific Endeavors Corp. */

/* The following definitions are provided for backward compatibility. */
#define circle(a,b,c,d,e) ellipse(a,b,c,c,0,d,e,0,INCH_UNITS)
#define ucircle(a,b,c,d,e) ellipse(a,b,c,c,0,d,e,0,USER_UNITS)
#define d3color d3colorL
#define dateit Dateit

/* These general purpose macros can be */
/* used to define any type of pointer. */
#define G_PTR(T,p,h) T *p
#define G_MPTR(M,T,p,h) M T *p
#define G_PTRPTR(T,p,h1,h2) T **p
#define G_MPTRPTR(M,T,p,h1,h2) M T **p
#define G_PTRPTRAQ(T,p,h)  T **p

#define G_P_PTR(T,p,h) T *p
#define G_P_PTRPTR(T,p,h1,h2) T **p

#define G_A_PTR(p,h) *p
#define G_A_PTRPTR(p,ph,h1,h2) **p

#define GPC_ALLOC(h, amt, siz) gpcalloc((amt), (siz))
#define GPC_REALLOC(h, p, amt) gpcrealloc((void *)(p), (amt))
#define GPC_FREE(h, p) gpcfree((void **)(p))
#define GPC_DIM2(h1, h2, amt1, amt2, siz) dim2((amt1), (amt2), (siz))
#define GPC_FREE2(h1, h2, p) free2((void ***)(p))
#define GPC_NEWBUF(h, p, k) newbufsq((struct GXYPAIRSQ **)(p), (k))
#define GPC_NEWVBUF(h1, h2, p1, p2, k, siz) newvbufsq((void **)(p1), (void **)(p2), (k), (siz))

/* Pointer definition macros (for GraphiC 6.1 compatability) */
#define G_PDQ(x,y) G_PTR(double, x, y)
#define G_PFQ(x,y) G_PTR(float, x, y)
#define G_PIQ(x,y) G_PTR(int, x, y)
#define G_PCQ(x,y) G_PTR(char, x, y)
#define G_PUIQ(x,y) G_PTR(unsigned int, x, y)
#define G_PUCQ(x,y) G_PTR(unsigned char, x, y)
#define G_PPFQ(x,y,z) G_PTRPTR(float, x, y, z)
#define G_PGXYPAIRSQ(x,y) G_PTR(struct GXYPAIRSQ, x, y)
#define G_EPCQ(x,y) G_MPTR(extern, char, x, y)
#define G_EPIQ(x,y) G_MPTR(extern, int, x, y)
#define G_SPFQ(x,y) G_MPTR(static, float, x, y)
#define G_SPGXYPAIRSQ(x,y) G_MPTR(static, struct GXYPAIRSQ, x, y)
#define G_SPIQ(x,y) G_MPTR(static, int, x, y)
#define G_SPCQ(x,y) G_MPTR(G_STATIC, char, x, y)
#define G_SPUIQ(x,y) G_MPTR(static, unsigned int, x, y)
#define G_SPUCQ(x,y) G_MPTR(static, unsigned char, x, y)
#define G_SPPUCQ(x,y,z) G_MPTRPTR(static, unsigned char, x, y, z)
#define G_SPPIQ(x,y,z) G_MPTRPTR(static, int, x, y, z)
#define G_SPPFQ(x,y,z) G_MPTRPTR(static, float, x, y, z)
#define G_SPPCQ(x,y,z) G_MPTRPTR(static, char, x, y, z)

#if 1 - GPC_OS2
#define DIST far
#else
#define DIST
#endif
