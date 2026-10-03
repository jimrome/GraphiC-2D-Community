#if defined(__cplusplus) && !GPC_CPP
extern "C" {
#endif


   /* ARROW.C */
void TYPE uvector(float xfrom, float yfrom, float xto, float yto,
   float ltow, float size, char *type);
void TYPE vector(float xfrom, float yfrom, float xto, float yto,
   float ltow, float size, char *type);

   /* AXESDRAW.C */
void TYPE CrossAt(float x, float y);

   /* BARPLT.C */
void TYPE bar(int nxdiv, float *x, float *y, int npts,
   float bwid, int mode, int *car, int *tpe);
#define barchart(nbars, x, y, npts, w, c, t) bar(nbars, x, y, npts, w, 0, c, t)
void TYPE histogram(float *x, float *y, int
 npts, int type, int *colors);

   /* BAUX???.C */
void init_videoq(int in_play, int mp);
int TYPE f_tableq(void *);
int TYPE ciq(void);
int TYPE ci_echoq(void);
unsigned int TYPE cstsq(void);
int TYPE ci_scq(void);
int TYPE ci_sc1q(void);
void ATYPE scr_aputsq(char *string, int attribute);
void gdriverq(int type);
void gterminateq(void);
void TYPE gpcBeep(int freq, int duration);
void get_dtq(struct DATE_TIMEQ *dt);
int new_fileq(void);
void openoffq(char *fname);
void getfilenameq(char *fname);
int TYPE gfclose(FILE *file);
void TYPE set_time(int timer);
char * TYPE get_time(int timer);
void ATYPE blank(Uchar attr);
FILE * TYPE gfopen(char *filenam, char *mode, size_t bigbuff);
FILE * TYPE gpc_tmpfile(void);
void ATYPE rowcolq(int row, int col);
void TYPE gpcTextMode(int mode);
#if GPC_OS2
void TYPE GPCMessageBox(char *string);
void TYPE GPCGetInput(char *prompt, char *string, int nbytes);
void ATYPE GPCprintstr(char *string);
void TYPE gpcGetClipData(int *, float *, float *, float *);
int TYPE RespondToMenu(SHORT action);
int TYPE get_instanceq(int action);
void TYPE GPCSetParentWindow(HWND);
void TYPE GPCSetModuleHandle(HMODULE hmod);
HWND TYPE GPCWINCreate(HWND *, HMODULE, HWND, ULONG, LONG, PCHAR,
	PCHAR, PUSER_FUN);
void Set_Exitq(int on_off);
void GetProfileInfoq(int);
#ifdef MULTITHREAD
void GPCInstanceq(int);
#endif
#endif
int load_fontsq(char *filename, struct G_FONT *font_info);
unsigned int gtacharq(void);
void TYPE screentype(int mode);

#if GPC_OS2 || GPC_WIN
void TYPE gpcHandles(int action, PGPCHANDLES gw);
void TYPE GPCSetWindowTitle(HWND hwnd, char *title);
void TYPE GPCBitmapStore(BOOL on_off);
#endif

#if GPC_WIN
void TYPE GPCWINYieldControl(void);
void TYPE GPCWINMessageBox(char *buf);
void TYPE GPCWINGetInput(char *prompt, char *buf, int nbytes);
void TYPE GPCWINGetClipData(int *numd, float *data1,
   float *data2, float *data3);
void TYPE GPCWINtextwnd(int iflag);
BOOL TYPE GPCWINprintstr(LPSTR output_string);
int TYPE GPCWINRegister(HANDLE hInstance, WNDPROC GPCWndProc);
HWND TYPE GPCWINCreate(HWND hWnd, HANDLE hInstance, HWND hOwnerWnd,
	int nCmdShow, HANDLE *hAccTable, char *title);
void TYPE GPCWINCommProc(HWND hWnd, UINT wParam, LONG lParam);
void TYPE GPCWINEnableMenu(BOOL bmenu);
void TYPE GPCWINCurkeyq(UINT wParam);
void TYPE GPCWINCharinq(UINT wParam);
long TYPE GPCWINSysCommand(HWND hWnd, UINT message, UINT wParam, LONG lParam);
void TYPE GPCWINReset(void);
void TYPE GPCWINStart(HWND hWnd);
void TYPE GPCWINClose(HWND hWnd);
void TYPE GPCWINDestroy(HWND hWnd);
void TYPE GPCWINSizeProc(HWND hWnd, UINT wParam);
void TYPE GPCWINPaintProc(HWND hWnd);
void TYPE GPCWINActProc(HWND hWnd, UINT wParam, LONG lParam);
void TYPE GPCWINLButtProc(HWND hWnd, LONG lParam);
void TYPE GPCWINMouseMProc(HWND hWnd, LONG lParam);
void TYPE GPCWINRButtProc(HWND hWnd);
void TYPE GPCWINCancel(void);
void TYPE GPCWINReDraw(HWND hWnd, UINT wParam, LONG lParam);
#endif

   /* BOTHPLT.C */
void lmoveq(int x0, int y0);
int cropq(int x0, int y0, int x1, int y1, int *x0p, int *y0p,
   int *x1p, int *y1p);
void TYPE rotate(char isrot);
void TYPE GPCSetBeepDefaults(int frequency, int duration);

   /* BOXFILL.C */
void TYPE d3color(int *array, int dim, char mesh);
void TYPE d3colorL(int *array, int dim, char mesh);
void TYPE d3colorN(float *values, int *array, int dim, char mesh);

   /* BPLT.C */
void TYPE bgnplot(char mp, char sc, char *tekfile);
void TYPE startplot(int bcolor);
int TYPE endplot(void);
void TYPE stopplot(void);
void TYPE hardcopy(char type);
void TYPE page(float hinch, float vinch);
void TYPE area2d(float xinch, float yinch);
void plot_initq(void);
void TYPE PlotLength(float dimension);
void TYPE PixPerUnit(float pixels);
void TYPE NewFile(char *NewName);
void TYPE svgcrop(float margin_inches);
void gpc_set_svg_crop(float margin_inches);
int TYPE svgopen(char *filepath);
int TYPE svgpush(char *url, char *filepath);
const char *gpc_get_svg_path(void);
const char *gpc_get_html_path(void);
int gpc_is_svg_active(void);
void gpc_flush_svg(void);

   /* CIRCLES.C */
void TYPE fillellipse(int onoff, int pattern, int border);
void TYPE ellipse(float xc, float yc, float xr, float yr,
   float rotation, float th1, float th2, char degrees, int units);
void ellipseq(int xc, int yc, int xr, int yr,
   float rotation, float th1, float th2);
void TYPE uradlin(float x, float y, float r1, float r2, float th);
void TYPE uradlinp(float x, float y, float r1, float r2,
	float th, int axis);
void TYPE radlin(float xt, float y, float r1, float r2, float th);
void radlinq(int xt, int y, int r1, int r2, float th);

   /* CNTAUX.C */
void set_paramq(int xmin, int xmax, int ymin, int ymax,
   unsigned int nlevels);
void seg_loadq(int *xc, int *yc, int pc, char seg_num, char cell,
   char start, char end);
void get_shapeq(int cntcount, int lev_num, char abort);

   /* CNTPLT.C */
void TYPE contour(GPC_FUNQ fun, float *x, int nx,
   float *y, int ny, float orig, float step, float maxq, int *lnstyle,
   int labint);
void TYPE conmat(float *zmat, float *x, int nx, float *y, int ny,
   float orig, float step, float maxq, int *lnstyle, int labint);
void TYPE conmat2(float **zmat, float *x, int nx, float *y, int ny,
   float orig, float step, float maxq, int *lnstyle, int labint);
void TYPE lcontour(GPC_FUNQ fun, float *x, int nx,
   float *y, int ny, float *level, int nlevel, int *lnstyle,
   int labint, char *formt);
void TYPE lconmat(float *zmat, float *x, int nx, float *y, int ny,
   float *level, int *lnstyle, int nlevel, int labint, char *formt);
void TYPE lconmat2(float **zmat, float *x, int nx, float *y, int ny,
   float *level, int *lnstyle, int nlevel, int labint, char *formt);

void contq(float **zmat, GPC_FUNQ fun, float *x,
   int nx, float *y, int ny, float orig, float step, float maxq,
   float *level, int *lnstyle, int nlevel, int labint, char *formt,
   int type);
float funq(void);
void TYPE stack(char stacked);
void TYPE polar(int mode);
float TYPE GetContour(int *size, float **x, float **y, float level);
void put_contourq(int size, int *x, int *y);
void P_TYPE contcolor(int *array, int border, ...);
void TYPE contcolor_fixed(int *colors, int bord, int *style);

   /* COLOR.C */
void TYPE HLStoRGB(int h, int l, int s, int *r, int *g, int *b);
void TYPE RGBtoHLS(int r, int g, int b, int *h, int *l, int *s);
void TYPE RGBtoCMY(int r, int g, int b, int * c, int * m, int *y);
void TYPE RGBtoCMYK(int r, int g, int b, int * c, int * m, int *y, int *k);
void CMYto4q(int c, int m, int y, int square[4]);
void TYPE GRCtoIBM(int i, int rgb[3], int *IBMcolor);
int TYPE RGBtoIBM(int rgb[3]);
int TYPE RGBtoGPC(int r, int g, int b, int *IBMcolor);

   /* COLORKEY.C */
void TYPE ColorKeyL(float minv, float maxv,
	int *intensity, int nintensity, float legllx, float leglly,
	float legurx, float legury, int labint, char *format, char fontc,
	float height, int angle);
void TYPE ColorKeyN(float *values, int *intensity,
	int nintensity, float legllx, float leglly,	float legurx,
	float legury, int labint, char *format, char fontc,	float height,
	int angle);

   /* CONMATR */
void TYPE conmatrNS(float *xp, float *yp, float *zp, int npts, float xmin,
   float xmax, int nx, float ymin, float ymax, int ny, float zorig,
   float zstep, float zmax, int *lnstyle, int labint);
void TYPE conmatr(float *xp, float *yp, float *zp, int npts, float xmin,
   float xmax, int nx, float ymin, float ymax, int ny, float zorig,
   float zstep, float zmax, int *lnstyle, int labint);

   /* CURSOR.C */
void TYPE curson(int row, int col, float **fx, float **fy, int *find);
void TYPE curvefollow(int row, int col, float *x, float *y, int npts);
void TYPE putsgq(char *string, int irow, int icol);
void TYPE lclrq(int irow, int minl, int maxl);
int TYPE GetDataPoint(float *, float *);

   /* CURVES.C */
void TYPE curve(float *x, float *y, int npts, int sym);
void TYPE errorbar(float x, float xmin, float xmax, float y, float ymin,
   float ymax);
void TYPE staircase(float *x, float *y, float *xnew,
   float *ynew, int *npts, int mode);
void TYPE scatplot(char dot);

   /* D3AUX.C */
void TYPE curv3d(float *pa, float *pb, float *pc, int npnts);
void TYPE panel3d(float *pa, float *pb, float *pc, int npnts,
   int pattern, int border);
void TYPE d3symbol(float x, float y, float z, int sym, int connect);
void TYPE d3line(float xf, float yf, float zf, float xt, float yt, float zt);
void TYPE d3pltfnt(float x, float y, float z, char *string,
   float height, int angle);
void TYPE box3d(void);
void TYPE d3scale(float x, float y, float z, int *xp, int *yp);
void TYPE WaterfallCurve(float *x, float y, float *z, int npts);
void TYPE WaterfallPlot(float *x, float *y, float **z, int nx, int ny);

   /* D3PLT.C */
void TYPE surfun(GPC_FUNQ fun, int ixpts, float xdel,
   int iypts, float ydel);
void TYPE surmat2(float **zmat, int xdim, int ydim);
void TYPE surmatc2(float **zmat, int xdim, int ydim, int ixpts, int iypts);
void TYPE surmat(float *zmat, int xdim, int ydim);
void TYPE surmatc(float *zmat, int xdim, int ydim, int ixpts, int iypts);
void TYPE volm3d(float x3axis, float y3axis, float z3axis);
void TYPE vuabs(float xvu, float yvu, float zvu);
void TYPE vuangl(float phi, float theta, float radius);
void TYPE view(float a, float b, float c);
void TYPE graf3d(float xorig, float xstp, float xmax, float yorig,
   float ystp, float ymax, float zorig, float zstp, float zmax);
void TYPE survis(char *str);
void TYPE nohide(int visible);
void TYPE sclfix(int isfixed);
void TYPE d3base(int isbase, int color);
void TYPE x3name(char *xnam);
void TYPE y3name(char *ynam);
void TYPE z3name(char *znam);
void TYPE d3head(char *title, int place);
void TYPE d3grid(int subdiv, int label);
void TYPE tcolor(Uchar value);
void TYPE bcolor(Uchar value);
void TYPE plane3d(char wplane, float pos, char hide);
void TYPE d3bars(float barson);
void TYPE GPCHorizonInit(void);

   /* D4PLT.C */
void TYPE Plot4dPnt(float x, float y, float z, float vol, float vmax, int sym);
void TYPE Plot4d(float *x, float *y, float *z, float *v, int np,
	float vmax, int sym);

   /* ERFPLT.C */
void TYPE erfcurve(float *x, float *y, int npts, int sym);
void TYPE erfscales(int nydiv, float *y, int npts);
void TYPE erfgrid(float ymin, float ystep, float ymax, char *yformat);

   /* FNTLOAD.C */
void P_TYPE font(int nfont, ...);
void freememq(void);

   /* FNTPLT.C */
void fontinitq(void);
void TYPE fntchg(Uchar sym);
void TYPE ctline(char *string, float height);
void TYPE ltline(char *string, float height);
void TYPE rtline(char *string, float height);
void TYPE lcrline(char *lstr, float hl, char *cstr, float hc,
   char *rstr, float hr);
void TYPE prtfnt(float xinch, float yinch, char *string, float height,
   int angle);
void TYPE pltfnt(float x, float y, char *string, float height, int angle);
void fntpltq(int x, int y, char *string, float height, int ang);
int strwidtq(char *string, float height);
float TYPE strwidth(char *string, float height);
void TYPE xname(char *xlabel);
void TYPE yname(char *ylabel);
void TYPE aname(char *alabel);
void TYPE bname(char *blabel);
void TYPE cname(char *clabel);
void TYPE heading(char *title);
void labels2q(void);
float angwidq(char *string, float height, float r);
void TYPE circtxt(float x, float y, float r, char *string, float hgt,
   float angle);
void circtxtq(int xpix, int ypix, int r, char *string, float hgt,
   float angle);
void TYPE lmargin(float marginch);
void TYPE rmargin(float marginch);
void TYPE tmargin(float marginch);
void TYPE fbox(int boxon, float size);
void TYPE setscale(char fac);
void TYPE tifont(float wid);
void TYPE tfont(int wid);
void TYPE fillfont(int fill);
void TYPE fontfill(int fill, int fill_pattern, int border_color);
void TYPE skew(float slant);
void TYPE widen(float fact);
void TYPE supsym(char ch);
void TYPE subsym(char ch);
void TYPE nullsym(char ch);
void TYPE scrht(float height);
void TYPE scrshft(float shift);
void TYPE tabspace(int tspace);
void TYPE titlht(float inch);
void TYPE numht(float inch);
void TYPE axnamht(float inch);
void TYPE xnamht(float inch);
void TYPE ynamht(float inch);
void TYPE trinamht(float inch);
void TYPE charspc(int width);
void lab_wid_setq(int axis, int width);
void TYPE SetNameSpace(int axis, float factor);
void TYPE SetLabelSpace(int axis, float factor);
void TYPE SetTitleSpace(float factor);
void TYPE linesp(float ratio);
char TYPE symoff(char flag);
void TYPE GPCPutLabel(char *string, float xpos, float ypos, float height,
	int border, int units, int color);
void TYPE putlabel(char *string, float xpos, float ypos, float height,
   int border, int units);
void scale_fontsq(void);
void TYPE GTfont(int font_number);
void TYPE setspacing(int on_off, unsigned char fnt_select);
void TYPE gpcGetWidestChar(unsigned char fnt_select,
  unsigned char *char_num, int *width);
void TYPE gpcGetCharWidth(unsigned char fnt_select,
   unsigned char char_num, int *width);
void set_titlhtq(void);
unsigned char get_current_fontq(void);

   /* GETXY */
void getxyq(float v1, float v2, float *px, float *py, int units);

   /* GRIDOPTS.C */
void TYPE axesoff(int noaxes);
void TYPE grid(int gridon);
void TYPE xgrid(int gridon);
void TYPE ygrid(int gridon);
void TYPE fgrid(int fon, int ndiv);
void TYPE xfgrid(int fon, int nx);
void TYPE yfgrid(int fon, int ny);
void TYPE frame(char frameon, char ftick);
void TYPE cross(char iscross);
void TYPE tickout(int is_out);
void TYPE tickht(float height);

   /* INSERT.C */
void TYPE insert(char *readfile, int picnum, float llx, float lly,
   float ysize, int irot);

   /* LABSET.C */
void TYPE xlab(char on_off, char **strlab);
void TYPE ylab(char on_off, char **strlab);
void TYPE zlab(char on_off, char **strlab);
void TYPE xLabAng(int angle);
void TYPE yLabAng(int angle);
void TYPE zLabAng(int angle);
char *xlabq(int on_off, char **strlab, int action);
char *ylabq(int on_off, char **strlab, int action);
char *zlabq(int on_off, char **strlab, int action);
int x_lab_angq(int angle, int action);
int y_lab_angq(int angle, int action);
int z_lab_angq(int angle, int action);
void TYPE upright(char uplab);
void TYPE XTickPos(int on_off, GTICK_POS *array);
void TYPE YTickPos(int on_off, GTICK_POS *array);
void TYPE ZTickPos(int on_off, GTICK_POS *array);
int XTickPosq(int on_off, GTICK_POS *array, int action);
int YTickPosq(int on_off, GTICK_POS *array, int action);
int ZTickPosq(int on_off, GTICK_POS *array, int action);

   /* LEGEND.C */
void TYPE legpos(int entries, float xleg, float yleg, int border);
void TYPE legend(int marker, char *string, int style, float strhgt,
   int scolor);
void TYPE autolegend(char on_off);

   /* LINEATTR.C */
void TYPE dashf(int type);
void dashfq(int type);
void TYPE color(int col);
void ccolorq(char mode, int value);
int TYPE RGBcolor(int r, int g, int b);
int closest_indexq(char *array, int value, int end);
void TYPE tiline(float wid);
void TYPE tline(int width);
void tlineq(int width);

   /* LINES.C */
void TYPE line(int xfrom, int yfrom, int xto, int yto, int newcurv);
void TYPE uline(float xfrom, float yfrom, float xto, float yto, int newcurv);
void TYPE linch(float x1, float y1, float x2, float y2, char nucrv);

   /* LINPLT.C */
void TYPE graf(char *xformat, float xori, float xste, float xma,
   char *yformat, float yori, float yste, float yma);
void TYPE scales(int nxdiv, int nydiv, float *x, float *y, int npts);

   /* LOGPLT.C */
void TYPE xlgscle(int nydiv, float *x, float *y, int npts);
void TYPE ylgscle(int nxdiv, float *x, float *y, int npts);
void TYPE lglgscle(float *x, float *y, int npts);
void TYPE xlog(float xorig, float xmax, char *format, float yorig,
   float ystep, float ymax);
void TYPE ylog(char *format, float xorig, float xstep, float xmax,
   float yorig, float ymax);
void TYPE loglog(float xorig, float xmax, float yorig, float ymax);
void TYPE SetLogBase(double x_base, double y_base);
double glogq(double n, int is_x_or_y);

   /* LPLT.C */
void lineq(int x0, int y0, int x1, int y1);
void ldrawq(int x1, int y1);
void G_CDECL totekq(int x, int y, char penup);
void d3lineq(float v10, float v20, float z0, float v1,
   float v2, float z);
void d3convq(float x, float y, float z, int *xp, int *yp);
void d3rndq(float orig, float step, float max,
   int *pint, int *pdec, char *format);
void tkbfillq(char *string);
void TYPE gwindow(float x0, float y0, float x1, float y1);
void TYPE GPCClipRectangle(float x0, float y0, float x1, float y1,
	int units, int border);
void windowq(int x0, int y0, int x1, int y1);
void win2q(int x0, int y0, int x1, int y1, int type);
void win_boxq(void);
void encodeq(int value, char *resp);
void TYPE TKFSegmentStart(void);
void TYPE TKFSegmentEnd(int action);

   /* MATSOLVE */
void TYPE matsolve(double **a, int n, double *b);
void TYPE matinverse(double **a, double **y, int n);

   /* MEMORY(?).C */
#ifdef TESTMEM
void * TYPE gpcallocq(size_t nobj, size_t size, char *file, int line);
void * TYPE gpcreallocq(void *ptr, size_t nobj, char *file, int line);
void TYPE gpcfreeq(void **ptr, char *file, int line);
#else
void * TYPE gpcalloc(size_t nobj, size_t size);
void * TYPE gpcrealloc(void *ptr, size_t nobj);
void TYPE gpcfree(void **ptr);
#endif
void gfreeallq(void);
int gchkmemq(void);
int printer_memq(int nbuf, int row, int col);
void bfreeq(int row, int nbuf);
int newbufsq(struct GXYPAIRSQ **xpanq, int index);
int newvbufsq(void **x, void **y, int index, int size);
int newabufsq(void **x, int index, int size);
void **TYPE dim2(int row, int col, unsigned int size);
void **TYPE free2(void ***pa);
long TYPE HowMuchMem(void);
char *strdupq(char *string);
#if GPC_OS2
void GPCFreeResourcesq(void);
#endif

   /* MESSAGES.C */
void TYPE gmessageq(char mesgtype, int mesgnum, char *filename,
   int linno);
void gprintq(char *string, char position);
void TYPE plotadd(void);

   /* PANFIL.C */
void panprocq(void);
void TYPE panproc(void);
void panelq(void);

   /* PATCHPLT.C */
void TYPE patchmat(float *zmat, float *x, int nx, float *y, int ny,
   float minv, float maxv, int type, int *intensity, int nintensity,
   int border, float legllx, float leglly, float legurx, float legury,
   int labint, char *format, char font, float height, int nsmear);
void TYPE PatchMatL(float *zmat, float *x, int nx, float *y,
	int ny, float *minv, float *maxv, int type,
	int *intensity, int nintensity, int nsmear);
void TYPE PatchMatN(float *zmat, float *x, int nx, float *y,
	int ny, float *values, int type,
	int *intensity, int nintensity, int nsmear);

   /* PIEPLT.C */
void TYPE pieplot(int dim, float *seg, int *color, float *offset,
   char *caption[], float size, int border, int txtclr);

   /* PLUMEPLT.C */
void TYPE PlumePlotL(float *x, float *y, float *z, int dim,
	float *minv, float *maxv, int symnum, float symheight,
	int *intensity, int nintensity);
void TYPE PlumePlotN(float *x, float *y, float *z, int dim,
	float *values, int symnum, float symheight,
	int *intensity, int nintensity);

   /* POLPLT.C */
void TYPE polgrid(float rmax, float rmin, int mode, char *larray[],
   int dim, int ntheta, int col);
void TYPE polcurve(float *r, float *theta, unsigned int dim, int sym);
void TYPE wedge(float rmin, float rmax, float thmin, float thmax,
   int pattern, int border);

   /* POSITION.C */
void set_spacingq(int nxstep, char *xformat, int nystep, char *yformat);
void psetq(void);

   /* SMOOTH.C */
void TYPE spline(float *x, float *y, int n, int n1, float s1, float s2,
   float *xn, float *yn);
void TYPE spline3(float *x, float *y, float *z, int nx, int ny, int n1x,
   int n1y, float *xn, float *yn, float *zn);
int TYPE spline3_2(float *x, float *y, float **z,
   int nx, int ny, int n1x, int n1y, float *xn, float *yn,
   float **zn);

   /* R3PLT.C */
int TYPE InterpolateZ(float *xp_in, float *yp_in, float *zp_in,
	int npts, float xmin, float xmax, int nxg, float ymin,
	float ymax, int nyg, float **z_out);
void TYPE settolerance(float tolerance);

   /* RFIT */
void TYPE rfit(float *x, float *y, int npts, int nits, float sfact, int nout,
   float *xout, float *yout);

   /* ROUNDOFF.C */
void TYPE minmax(float *minq, float *maxq, float *xy, int npts);
void roundq(int nstep, float *orip, float *step, float *maxp,
   char *format);
double roundoffq(double x, double eps);
int getexpq(double *nptr, double base);
   
   /* SIMPLT.C */
void TYPE simuplot(int);
void TYPE context(int);

   /* SMEAR.C */
int TYPE smear(float *m1, int nx, int ny, int nsmear);

   /* SMITH.H */
void TYPE smdraw(Uchar lcolor);
void TYPE plotrx(float r, float x);
void TYPE plotrho(float rho, float theta);
void TYPE smove(float dell);
void TYPE getrx(float *r, float *x);
void TYPE getrho(float *rho, float *theta);
void TYPE smithcir(float u, float v, float radius);

   /* STATFUNS.C */
float TYPE mean(float *x, int xdim);
float TYPE median(float *x, int xdim);
float TYPE variance(float *x, int xdim);
float TYPE StdDev(float *x, int xdim);
void TYPE BoxPlot(float x, float *y, int ydim, int outlier,
   int symnum, float width);
int TYPE crosscorrelate(float *x, float *y, float *cvec,
   int dim, int avg); 

   /* SURMATR */
void TYPE surmatr(float *xp, float *yp, float *zp, int npts, float xmin,
   float xmax, int nx, float ymin, float ymax, int ny);
void TYPE surmatrS(float *xp, float *yp, float *zp, int npts, float xmin,
   float xmax, int nx, float ymin, float ymax, int ny);

   /* SVC.C */
void TYPE pbox(void);
void TYPE box(void);
void TYPE physor(float xphys, float yphys);
void TYPE orel(float xorel, float yorel);
void TYPE pgshift(float xinch, float yinch);
void TYPE acrop(char flag);
void acropq(char flag);
void TYPE rightax(char yright);
void TYPE ticurve(float width);
void TYPE tcurve(int width);
void TYPE DateItXY(float xinch, float yinch, unsigned char fontt);
void TYPE dateit(Uchar font);
void TYPE plotabsolute(int set_flag);
void TYPE gtext(char mode);
void TYPE usertopix(float x_in, float y_in, float z_in, int *x_out, int *y_out);
void TYPE usertoinch(float x_in, float y_in, float *x_out, float *y_out);
void TYPE pixtouser(int x_in, int y_in, float *x_out, float *y_out);
void TYPE metricunits(int onoff);
float fudgeitq(float z, float diff);
int zvalueq(float *values, int dim, float tval);
void TYPE panelinch(float *x, float *y, int nlines, int pcolor, int border);
void TYPE panel(float *x, float *y, int nlines, int pcolor, int border);
void TYPE curvfill(float *x, float *y, int npts, float *x1, float *y1,
   int npts1, int pcolor, int border);
void kprintfq(char *xl, char *format, float value);
float TYPE pixtoin(int pixels);
int TYPE intopix(float inches);

   /* SYMBOLS.C */
void TYPE symbol(float x, float y, int symnum);
void sympltq(int x, int y, int symcnt);
void TYPE symht(float height);
void TYPE sympick(int symbol);
void TYPE syminch(float x, float y, int symnum);

   /* TEKINT.C */
void scrnpltq(void);
int reproq(void);
int gprosq(int mode);
void settekq(void);
int getseq(Uchar mode, int *ind);
void skipq(int n);
void set_clrq(char mode, int color);

   /* TPLT.C */
void tekq(char asc);
#ifdef SetDefaultPrinter
#undef SetDefaultPrinter
#endif
void TYPE SetDefaultPrinter(int prnt_number);
void TYPE penchg(char mode);

   /* TRIANGLE.C */
void TYPE trimat(float *m1, float *m2, float *z, int dim, int dim1,
   float zmin, float zstep, float zmax, int *lnstyle, int bcolor);
void TYPE trifun(GPC_FUNQ fun, int dim, float zmin,
   float zstep, float zmax, int *lnstyle, int bcolor);
void tri_convq(int x, int y, int *xp, int *yp);
void tri_angq(int bcolor);

   /* ZCURSOR.C */
void zcursonq(void);
void cursorq(struct CURPOS *cpos);
void GetPosBuq(int *bstatus, int *hmpos, int *vmpos);
void SetCposq(int horizontal, int vertical);

#if defined(__cplusplus) && !GPC_CPP
};
#endif
