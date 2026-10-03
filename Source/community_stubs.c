/*
 * community_stubs.c
 * GraphiC 2D Community Edition - Professional Feature Stubs
 *
 * Provides runtime notices when GraphiC Professional routines
 * (3D surfaces, contours, Smith charts, polar/ternary plots, advanced typography)
 * are invoked in the free Community Edition.
 */

#include <stdio.h>
#include <stdlib.h>
#include <graphic.h>

static void gpc_pro_feature_notice(const char *feature) {
    static int count = 0;
    fprintf(stderr, "\n[GraphiC Community Edition] Notice: \"%s\" is a GraphiC Professional feature.\n", feature);
    if (count++ < 3) {
        fprintf(stderr, "  Free Community Edition includes core 2D plotting, outline Hershey fonts,\n"
                        "  and native TrueType text. 3D surfaces, contours, Smith charts, polar/ternary\n"
                        "  plots, and advanced typography controls require GraphiC Professional 2026.\n"
                        "  Upgrade or view full documentation at: https://jamesrome.com\n\n");
    }
}

void TYPE surmat(float *zmat, int xdim, int ydim)
{
    gpc_pro_feature_notice("surmat");
    
}

void TYPE surmat2(float **zmat, int xdim, int ydim)
{
    gpc_pro_feature_notice("surmat2");
    
}

void TYPE surmatr(float *xp, float *yp, float *zp, int npts, float xmin, float xmax, int nx, float ymin, float ymax, int ny)
{
    gpc_pro_feature_notice("surmatr");
    
}

void TYPE survis(char *str)
{
    gpc_pro_feature_notice("survis");
    
}

void TYPE surfun(GPC_FUNQ fun, int ixpts, float xdel, int iypts, float ydel)
{
    gpc_pro_feature_notice("surfun");
    
}

void TYPE box3d(void)
{
    gpc_pro_feature_notice("box3d");
    
}

void TYPE curv3d(float *pa, float *pb, float *pc, int npnts)
{
    gpc_pro_feature_notice("curv3d");
    
}

void TYPE d3bars(float barson)
{
    gpc_pro_feature_notice("d3bars");
    
}

void TYPE d3base(int isbase, int color)
{
    gpc_pro_feature_notice("d3base");
    
}

void TYPE d3grid(int subdiv, int label)
{
    gpc_pro_feature_notice("d3grid");
    
}

void TYPE d3head(char *title, int place)
{
    gpc_pro_feature_notice("d3head");
    
}

void TYPE d3line(float xf, float yf, float zf, float xt, float yt, float zt)
{
    gpc_pro_feature_notice("d3line");
    
}

void TYPE d3pltfnt(float x, float y, float z, char *string, float height, int angle)
{
    gpc_pro_feature_notice("d3pltfnt");
    
}

void TYPE graf3d(float xorig, float xstp, float xmax, float yorig, float ystp, float ymax, float zorig, float zstp, float zmax)
{
    gpc_pro_feature_notice("graf3d");
    
}

void TYPE plane3d(char wplane, float pos, char hide)
{
    gpc_pro_feature_notice("plane3d");
    
}

void TYPE volm3d(float x3axis, float y3axis, float z3axis)
{
    gpc_pro_feature_notice("volm3d");
    
}

void TYPE vuabs(float xvu, float yvu, float zvu)
{
    gpc_pro_feature_notice("vuabs");
    
}

void TYPE vuangl(float phi, float theta, float radius)
{
    gpc_pro_feature_notice("vuangl");
    
}

void TYPE x3name(char *xnam)
{
    gpc_pro_feature_notice("x3name");
    
}

void TYPE y3name(char *ynam)
{
    gpc_pro_feature_notice("y3name");
    
}

void TYPE z3name(char *znam)
{
    gpc_pro_feature_notice("z3name");
    
}

void TYPE nohide(int visible)
{
    gpc_pro_feature_notice("nohide");
    
}

void TYPE tcolor(Uchar value)
{
    gpc_pro_feature_notice("tcolor");
    
}

void TYPE bcolor(Uchar value)
{
    gpc_pro_feature_notice("bcolor");
    
}

void TYPE sclfix(int isfixed)
{
    gpc_pro_feature_notice("sclfix");
    
}

void TYPE d3colorL(int *array, int dim, char mesh)
{
    gpc_pro_feature_notice("d3colorL");
    
}

void TYPE d3colorN(float *values, int *array, int dim, char mesh)
{
    gpc_pro_feature_notice("d3colorN");
    
}

void TYPE polar(int mode)
{
    gpc_pro_feature_notice("polar");
    
}

void TYPE conmat(float *zmat, float *x, int nx, float *y, int ny, float orig, float step, float maxq, int *lnstyle, int labint)
{
    gpc_pro_feature_notice("conmat");
    
}

void TYPE lconmat(float *zmat, float *x, int nx, float *y, int ny, float *level, int *lnstyle, int nlevel, int labint, char *formt)
{
    gpc_pro_feature_notice("lconmat");
    
}

void TYPE lcontour(GPC_FUNQ fun, float *x, int nx, float *y, int ny, float *level, int nlevel, int *lnstyle, int labint, char *formt)
{
    gpc_pro_feature_notice("lcontour");
    
}

void TYPE contcolor_fixed(int *colors, int bord, int *style)
{
    gpc_pro_feature_notice("contcolor_fixed");
    
}

void TYPE ColorKeyL(float minv, float maxv, int *intensity, int nintensity, float legllx, float leglly, float legurx, float legury, int labint, char *format, char fontc, float height, int angle)
{
    gpc_pro_feature_notice("ColorKeyL");
    
}

void TYPE ColorKeyN(float *values, int *intensity, int nintensity, float legllx, float leglly, float legurx, float legury, int labint, char *format, char fontc, float height, int angle)
{
    gpc_pro_feature_notice("ColorKeyN");
    
}

void TYPE smdraw(Uchar lcolor)
{
    gpc_pro_feature_notice("smdraw");
    
}

void TYPE smithcir(float u, float v, float radius)
{
    gpc_pro_feature_notice("smithcir");
    
}

void TYPE smove(float dell)
{
    gpc_pro_feature_notice("smove");
    
}

void TYPE trimat(float *m1, float *m2, float *z, int dim, int dim1, float zmin, float zstep, float zmax, int *lnstyle, int bcolor)
{
    gpc_pro_feature_notice("trimat");
    
}

void TYPE trifun(GPC_FUNQ fun, int dim, float zmin, float zstep, float zmax, int *lnstyle, int bcolor)
{
    gpc_pro_feature_notice("trifun");
    
}

void TYPE polgrid(float rmax, float rmin, int mode, char *larray[], int dim, int ntheta, int col)
{
    gpc_pro_feature_notice("polgrid");
    
}

void TYPE polcurve(float *r, float *theta, unsigned int dim, int sym)
{
    gpc_pro_feature_notice("polcurve");
    
}

void TYPE plotrho(float rho, float theta)
{
    gpc_pro_feature_notice("plotrho");
    
}

void TYPE plotrx(float r, float x)
{
    gpc_pro_feature_notice("plotrx");
    
}

void TYPE pieplot(int dim, float *seg, int *color, float *offset, char *caption[], float size, int border, int txtclr)
{
    gpc_pro_feature_notice("pieplot");
    
}

void TYPE wedge(float rmin, float rmax, float thmin, float thmax, int pattern, int border)
{
    gpc_pro_feature_notice("wedge");
    
}

void TYPE WaterfallPlot(float *x, float *y, float **z, int nx, int ny)
{
    gpc_pro_feature_notice("WaterfallPlot");
    
}

void TYPE rfit(float *x, float *y, int npts, int nits, float sfact, int nout, float *xout, float *yout)
{
    gpc_pro_feature_notice("rfit");
    
}

void TYPE spline(float *x, float *y, int n, int n1, float s1, float s2, float *xn, float *yn)
{
    gpc_pro_feature_notice("spline");
    
}

void TYPE stack(char stacked)
{
    gpc_pro_feature_notice("stack");
    
}

void TYPE erfscales(int nydiv, float *y, int npts)
{
    gpc_pro_feature_notice("erfscales");
    
}

void TYPE erfcurve(float *x, float *y, int npts, int sym)
{
    gpc_pro_feature_notice("erfcurve");
    
}

void TYPE erfgrid(float ymin, float ystep, float ymax, char *yformat)
{
    gpc_pro_feature_notice("erfgrid");
    
}

void TYPE matinverse(double **a, double **y, int n)
{
    gpc_pro_feature_notice("matinverse");
    
}

void TYPE matsolve(double **a, int n, double *b)
{
    gpc_pro_feature_notice("matsolve");
    
}

void TYPE settolerance(float tolerance)
{
    gpc_pro_feature_notice("settolerance");
    
}



#undef d3color
void TYPE d3color(int *array, int dim, char mesh) {
    gpc_pro_feature_notice("d3color");
}

void TYPE WaterfallCurve(float *x, float y, float *z, int npts) {
    gpc_pro_feature_notice("WaterfallCurve");
}

void TYPE WaterfallLines(float *x, float *y, float **zmat, int nx, int ny) {
    gpc_pro_feature_notice("WaterfallLines");
}

