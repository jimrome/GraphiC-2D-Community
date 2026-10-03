#ifndef FNTPLT_H_INCLUDED
#define FNTPLT_H_INCLUDED

#ifdef MULTITHREAD
struct f_dataq {
#endif

G_STATIC char set_x_default;
G_STATIC char set_y_default;
G_STATIC char set_title_default;
G_STATIC char newseg;			/* Start of new segment. */
G_STATIC char sclflgq;
G_STATIC char skewq;			/* SKEWED CHARACTERS*/
G_STATIC char widenq;			/* CHANGE CHARACTER WIDTH*/
G_STATIC char fillq;			/* FLAG FOR FILLING FONTS*/
G_STATIC int font_patternq;		/* FLAG FOR FILLING FONTS*/
G_STATIC int font_border_colorq;/* FLAG FOR FILLING FONTS*/
/* SPECIAL CHARACTERS */
G_STATIC char clrsymq;			/* SYMBOL FOR CHANGING COLORS*/
G_STATIC char widsymq;			/* SYMBOL FOR WIDENING CHARACTERS*/
G_STATIC char thicksymq;		/* SYMBOL FOR THICKENING CHARACTERS*/
G_STATIC char supsymq;			/* SYMBOLS FOR SUPER-SUB-SCRIPTS*/
G_STATIC char subsymq;			/* SYMBOLS FOR SUPER-SUB-SCRIPTS*/
G_STATIC char nullsymq;			/* SYMBOLS FOR SUPER-SUB-SCRIPTS*/
G_STATIC char use_allq;

G_STATIC int flagq;
G_STATIC int xinc;
G_STATIC int yinc;
G_STATIC int angq;
G_STATIC int lwid;
G_STATIC int wsavfq;
G_STATIC int tsavfq;
G_STATIC int bsavfq;
G_STATIC int rhoq;
G_STATIC int scrlevel;			/* super/subscript nesting level */
G_STATIC int xhome;
G_STATIC int yhome;				/* bottom right of character to be scripted */
G_STATIC int tfontq;			/* WIDTH OF FONT LINE*/
G_STATIC int fboxonq;			/* FONT BOX ON?*/
G_STATIC int thickq;
G_STATIC int xlabwid;			/* LABEL WIDTH*/
G_STATIC int ylabwid;			/* LABEL WIDTH*/
G_STATIC int ifontq;			/* ACTIVE FONT NUMBER*/
G_STATIC int fnthtq;			/* Pel height for the active font */
G_STATIC int char_spc[FNTNUM];	/* PIXEL SPACING BETWEEN CHARACTERS*/
G_STATIC int nullflag;
G_STATIC int shiftq;
G_STATIC int twoscr;
G_STATIC int tabspcq;			/* TAB*/
G_STATIC int scripton;
G_STATIC int scrtype ;
G_STATIC char const_spaceq[FNTNUM];
G_STATIC float heightq;
G_STATIC int charspcq;			/*PIXEL SPACING BETWEEN CHARACTERS*/
G_STATIC float hgtwdq;
G_STATIC float dtheta;
G_STATIC float sinq;
G_STATIC float cosq;
G_STATIC float xbf;
G_STATIC float ybf;
G_STATIC float w1;
G_STATIC float w2;
G_STATIC float w3;
G_STATIC float xsav;
G_STATIC float ysav;
G_STATIC float tw;
G_STATIC float xfacq;
G_STATIC float yfacq;
G_STATIC float fboxsizq;
G_STATIC float skewfacq;		/* SLANT FACTOR*/
G_STATIC float widfacq;			/* FACTOR FOR CHANGING CHARACTER WIDTH*/
G_STATIC float scrshftq;		/* SCRIPT SHIFT*/
G_STATIC float scrhtq;			/* SCRIPT HEIGHT*/
G_STATIC float line_sp;			/* LINE SPACING IN TEXT - RATIO TO HEIGHT*/
G_STATIC float ytopq;			/* TOP OF PAGE - INCHES (CM)*/
G_STATIC float xrightq;			/* RIGHT MARGIN - INCHES (CM)*/
G_STATIC float xleftq;			/* LEFT MARGIN*/
G_STATIC double angd;
#ifdef MULTITHREAD
};

struct f_dataq *fnt_dataq[GPC_MAX_THREADS];
#define fnt_data fnt_dataq[instctr]

#define set_x_default fnt_data->set_x_default
#define set_y_default fnt_data->set_y_default
#define set_title_default fnt_data->set_title_default
#define newseg fnt_data->newseg
#define sclflgq fnt_data->sclflgq
#define skewq fnt_data->skewq
#define widenq fnt_data->widenq
#define fillq fnt_data->fillq
#define font_patternq fnt_data->font_patternq
#define font_border_colorq fnt_data->font_border_colorq
/* SPECIAL CHARACTERS */
#define clrsymq fnt_data->clrsymq
#define widsymq fnt_data->widsymq
#define thicksymq fnt_data->thicksymq
#define supsymq fnt_data->supsymq
#define subsymq fnt_data->subsymq
#define nullsymq fnt_data->nullsymq

/* STORAGE FOR FONT SIZE DATA */
#define flagq fnt_data->flagq
#define xinc fnt_data->xinc
#define yinc fnt_data->yinc
#define angq fnt_data->angq
#define lwid fnt_data->lwid
#define wsavfq fnt_data->wsavfq
#define tsavfq fnt_data->tsavfq
#define bsavfq fnt_data->bsavfq
#define rhoq fnt_data->rhoq
#define scrlevel fnt_data->scrlevel
#define xhome fnt_data->xhome
#define yhome fnt_data->yhome
#define tfontq fnt_data->tfontq
#define fboxonq fnt_data->fboxonq
#define thickq fnt_data->thickq
#define xlabwid fnt_data->xlabwid
#define ylabwid fnt_data->ylabwid
#define ifontq fnt_data->ifontq
#define fnthtq fnt_data->fnthtq
#define char_spc fnt_data->char_spc
#define nullflag fnt_data->nullflag
#define shiftq fnt_data->shiftq
#define twoscr fnt_data->twoscr
#define tabspcq fnt_data->tabspcq
#define scripton fnt_data->scripton
#define scrtype fnt_data->scrtype
#define const_spaceq fnt_data->const_spaceq
#define heightq fnt_data->heightq
#define charspcq fnt_data->charspcq
#define hgtwdq fnt_data->hgtwdq
#define dtheta fnt_data->dtheta
#define sinq fnt_data->sinq
#define cosq fnt_data->cosq
#define xbf fnt_data->xbf
#define ybf fnt_data->ybf
#define w1 fnt_data->w1
#define w2 fnt_data->w2
#define w3 fnt_data->w3
#define xsav fnt_data->xsav
#define ysav fnt_data->ysav
#define tw fnt_data->tw
#define xfacq fnt_data->xfacq
#define yfacq fnt_data->yfacq
#define fboxsizq fnt_data->fboxsizq
#define skewfacq fnt_data->skewfacq
#define widfacq fnt_data->widfacq
#define scrshftq fnt_data->scrshftq
#define scrhtq fnt_data->scrhtq
#define line_sp fnt_data->line_sp
#define ytopq fnt_data->ytopq
#define xrightq fnt_data->xrightq
#define xleftq fnt_data->xleftq
#define angd fnt_data->angd
#define use_allq fnt_data->use_allq

void font_destroyq(int instctr);
#endif

void strsizq(int x, int y, char *string, float height, int ang,
	int action, int *wid, int *top, int *bottom);
void angsetq(int ang);
void rotateq(void);
void circsetq(int flag, int r);
void newangq(void);
void fboxq(int x, int y);
void tabitq(int action);
void supq(int action, int *top);
void subq(int action, int *bottom);
void slantq(int action);
void wideq(void);
void tfntq(void);
void set_font_paramq(int font_num, float height);
void chkifontq(char *string);
void get_extendersq(int *ascender, int *descender);

#endif
