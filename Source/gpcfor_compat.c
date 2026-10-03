/* (c) 1984-1995 Scientific Endeavors Corporation
 * Modernized Fortran 77 / F90 ABI Compatibility Shim
 * Implements pass-by-reference entry points for both lowercase trailing underscore
 * (GNU/POSIX convention) and uppercase symbols (classic Windows/VMS convention).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graphic.h"

/* Helper to convert Fortran fixed-length string with trailing spaces to C null-terminated string */
static void f77_to_c_str(char *dst, const char *src, int src_len, int max_dst)
{
    int len;
    if (src == NULL || src_len <= 0) {
        dst[0] = 0;
        return;
    }
    len = src_len;
    while (len > 0 && ((unsigned char)src[len - 1] <= ' ')) {
        len--;
    }
    if (len >= max_dst) len = max_dst - 1;
    memcpy(dst, src, len);
    dst[len] = 0;
}

/* BGNPLOT */
void bgnplot_(char *mp, char *sc, char *tekfile, int tekfile_len)
{
    char c_file[512];
    f77_to_c_str(c_file, tekfile, tekfile_len, sizeof(c_file));
    bgnplot(*mp, *sc, c_file);
}
void BGNPLOT(char *mp, char *sc, char *tekfile, int tekfile_len)
{
    bgnplot_(mp, sc, tekfile, tekfile_len);
}

/* STARTPLOT */
void startplot_(int *bcolor)
{
    startplot(*bcolor);
}
void STARTPLOT(int *bcolor)
{
    startplot(*bcolor);
}

/* ENDPLOT */
int endplot_(void)
{
    return endplot();
}
int ENDPLOT(void)
{
    return endplot();
}

/* STOPPLOT */
void stopplot_(void)
{
    stopplot();
}
void STOPPLOT(void)
{
    stopplot();
}

/* PAGE */
void page_(float *width, float *height)
{
    page(*width, *height);
}
void PAGE(float *width, float *height)
{
    page(*width, *height);
}

/* AREA2D */
void area2d_(float *xlen, float *ylen)
{
    area2d(*xlen, *ylen);
}
void AREA2D(float *xlen, float *ylen)
{
    area2d(*xlen, *ylen);
}

/* PHYSOR */
void physor_(float *x, float *y)
{
    physor(*x, *y);
}
void PHYSOR(float *x, float *y)
{
    physor(*x, *y);
}

/* OREL */
void orel_(float *dx, float *dy)
{
    orel(*dx, *dy);
}
void OREL(float *dx, float *dy)
{
    orel(*dx, *dy);
}

/* BOX */
void box_(void)
{
    box();
}
void BOX(void)
{
    box();
}

/* COLOR */
void color_(int *col)
{
    color(*col);
}
void COLOR(int *col)
{
    color(*col);
}

/* DASHF */
void dashf_(int *style)
{
    dashf(*style);
}
void DASHF(int *style)
{
    dashf(*style);
}

/* SYMPICK */
void sympick_(int *sym)
{
    sympick(*sym);
}
void SYMPICK(int *sym)
{
    sympick(*sym);
}

/* GRAF */
void graf_(char *xform, float *xmin, float *xstep, float *xmax,
           char *yform, float *ymin, float *ystep, float *ymax,
           int xform_len, int yform_len)
{
    char c_xf[64], c_yf[64];
    f77_to_c_str(c_xf, xform, xform_len, sizeof(c_xf));
    f77_to_c_str(c_yf, yform, yform_len, sizeof(c_yf));
    graf(c_xf, *xmin, *xstep, *xmax, c_yf, *ymin, *ystep, *ymax);
}
void GRAF(char *xform, float *xmin, float *xstep, float *xmax,
          char *yform, float *ymin, float *ystep, float *ymax,
          int xform_len, int yform_len)
{
    graf_(xform, xmin, xstep, xmax, yform, ymin, ystep, ymax, xform_len, yform_len);
}

/* HEADING */
void heading_(char *title, int title_len)
{
    char c_t[256];
    f77_to_c_str(c_t, title, title_len, sizeof(c_t));
    heading(c_t);
}
void HEADING(char *title, int title_len)
{
    heading_(title, title_len);
}

/* XNAME */
void xname_(char *xlab, int xlab_len)
{
    char c_x[256];
    f77_to_c_str(c_x, xlab, xlab_len, sizeof(c_x));
    xname(c_x);
}
void XNAME(char *xlab, int xlab_len)
{
    xname_(xlab, xlab_len);
}

/* YNAME */
void yname_(char *ylab, int ylab_len)
{
    char c_y[256];
    f77_to_c_str(c_y, ylab, ylab_len, sizeof(c_y));
    yname(c_y);
}
void YNAME(char *ylab, int ylab_len)
{
    yname_(ylab, ylab_len);
}

/* CURVE */
void curve_(float *x, float *y, int *npts, int *sym)
{
    curve(x, y, *npts, *sym);
}
void CURVE(float *x, float *y, int *npts, int *sym)
{
    curve(x, y, *npts, *sym);
}

/* VOLM3D */
void volm3d_(float *xlen, float *ylen, float *zlen)
{
    volm3d(*xlen, *ylen, *zlen);
}
void VOLM3D(float *xlen, float *ylen, float *zlen)
{
    volm3d(*xlen, *ylen, *zlen);
}

/* VUABS */
void vuabs_(float *x, float *y, float *z)
{
    vuabs(*x, *y, *z);
}
void VUABS(float *x, float *y, float *z)
{
    vuabs(*x, *y, *z);
}

/* GRAF3D */
void graf3d_(float *xmin, float *xstep, float *xmax,
             float *ymin, float *ystep, float *ymax,
             float *zmin, float *zstep, float *zmax)
{
    graf3d(*xmin, *xstep, *xmax, *ymin, *ystep, *ymax, *zmin, *zstep, *zmax);
}
void GRAF3D(float *xmin, float *xstep, float *xmax,
            float *ymin, float *ystep, float *ymax,
            float *zmin, float *zstep, float *zmax)
{
    graf3d_(xmin, xstep, xmax, ymin, ystep, ymax, zmin, zstep, zmax);
}

/* SURMAT */
void surmat_(float *zmat, int *nx, int *ny)
{
    survis("top");
    surmat(zmat, *nx, *ny);
}
void SURMAT(float *zmat, int *nx, int *ny)
{
    surmat_(zmat, nx, ny);
}

/* CONMAT */
void conmat_(float *zmat, float *x, int *nx, float *y, int *ny,
             float *orig, float *step, float *maxq, int *styles, int *labint)
{
    conmat(zmat, x, *nx, y, *ny, *orig, *step, *maxq, styles, *labint);
}
void CONMAT(float *zmat, float *x, int *nx, float *y, int *ny,
            float *orig, float *step, float *maxq, int *styles, int *labint)
{
    conmat_(zmat, x, nx, y, ny, orig, step, maxq, styles, labint);
}

/* SMDRAW */
void smdraw_(int *grid_col)
{
    smdraw(*grid_col);
}
void SMDRAW(int *grid_col)
{
    smdraw(*grid_col);
}

/* PLOTRX */
void plotrx_(float *r, float *x)
{
    plotrx(*r, *x);
}
void PLOTRX(float *r, float *x)
{
    plotrx(*r, *x);
}

/* POLGRID */
void polgrid_(float *rmax, float *rmin, int *mode, int *col)
{
    polgrid(*rmax, *rmin, *mode, NULL, 3, 8, *col);
}
void POLGRID(float *rmax, float *rmin, int *mode, int *col)
{
    polgrid_(rmax, rmin, mode, col);
}

/* POLCURVE */
void polcurve_(float *angles, float *radii, int *npts, int *sym)
{
    polcurve(angles, radii, *npts, *sym);
}
void POLCURVE(float *angles, float *radii, int *npts, int *sym)
{
    polcurve(angles, radii, *npts, *sym);
}

/* XLOG */
void xlog_(float *xmin, float *xmax, char *xform,
           float *ymin, float *ystep, float *ymax, int xform_len)
{
    char c_xf[64];
    f77_to_c_str(c_xf, xform, xform_len, sizeof(c_xf));
    xlog(*xmin, *xmax, c_xf, *ymin, *ystep, *ymax);
}
void XLOG(float *xmin, float *xmax, char *xform,
          float *ymin, float *ystep, float *ymax, int xform_len)
{
    xlog_(xmin, xmax, xform, ymin, ystep, ymax, xform_len);
}

/* YLOG */
void ylog_(char *yform, float *xmin, float *xstep, float *xmax,
           float *ymin, float *ymax, int yform_len)
{
    char c_yf[64];
    f77_to_c_str(c_yf, yform, yform_len, sizeof(c_yf));
    ylog(c_yf, *xmin, *xstep, *xmax, *ymin, *ymax);
}
void YLOG(char *yform, float *xmin, float *xstep, float *xmax,
          float *ymin, float *ymax, int yform_len)
{
    ylog_(yform, xmin, xstep, xmax, ymin, ymax, yform_len);
}

/* SVGOPEN */
int svgopen_(char *filepath, int filepath_len)
{
    char c_file[512];
    f77_to_c_str(c_file, filepath, filepath_len, sizeof(c_file));
    return svgopen(c_file);
}
int SVGOPEN(char *filepath, int filepath_len)
{
    return svgopen_(filepath, filepath_len);
}

/* SVGPUSH */
int svgpush_(char *url, char *filepath, int url_len, int filepath_len)
{
    char c_url[1024];
    char c_file[512];
    f77_to_c_str(c_url, url, url_len, sizeof(c_url));
    f77_to_c_str(c_file, filepath, filepath_len, sizeof(c_file));
    return svgpush(c_url, c_file);
}
int SVGPUSH(char *url, char *filepath, int url_len, int filepath_len)
{
    return svgpush_(url, filepath, url_len, filepath_len);
}
