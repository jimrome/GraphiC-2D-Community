#ifndef COLORMAP_H_INCLUDED
#define COLORMAP_H_INCLUDED

/* Scientific Colormap IDs */
#define GPC_COLORMAP_VIRIDIS  0
#define GPC_COLORMAP_PLASMA   1
#define GPC_COLORMAP_INFERNO  2
#define GPC_COLORMAP_MAGMA    3
#define GPC_COLORMAP_COOLWARM 4
#define GPC_COLORMAP_RAINBOW  5
#define GPC_COLORMAP_TURBO    6
#define GPC_NUM_COLORMAPS     7

#ifdef __cplusplus
extern "C" {
#endif

/* Pack R, G, B [0..255] into GraphiC 24-bit RGB color integer (0x01RRGGBB) */
int TYPE GCLASS gpc_rgb(int r, int g, int b);

/* Evaluate colormap at normalized position t in [0.0, 1.0] -> returns 0x01RRGGBB */
int TYPE GCLASS gpc_colormap_eval(int colormap_id, float t);

/* Query colormap at a specific scalar value between minv and maxv -> returns 0x01RRGGBB */
int TYPE GCLASS gpc_get_color(int colormap_id, float value, float minv, float maxv);

/* Query colormap RGB components [0..255] at a specific scalar value */
void TYPE GCLASS gpc_get_rgb(int colormap_id, float value, float minv, float maxv, int *r, int *g, int *b);

/* Continuous scatter plot routine: plots points with individual colormap colors */
void TYPE GCLASS gpc_scatter(float *x, float *y, float *z, int dim,
                             float minv, float maxv, int symnum, float symheight,
                             int colormap_id);

/* Continuous gradient color key / color bar with tick marks, labels, and title */
void TYPE GCLASS ColorKeyGradient(float minv, float maxv, int colormap_id,
                                  float legllx, float leglly, float legurx, float legury,
                                  int nticks, char *format, char fontc, float height,
                                  int angle);

/* Single font loader helper (callable from Fortran, Python, and Java) */
void TYPE GCLASS gpc_load_font(const char *fontname, char esc);

#ifdef __cplusplus
}
#endif

#endif /* COLORMAP_H_INCLUDED */
