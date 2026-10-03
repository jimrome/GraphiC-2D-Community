#ifndef PANFIL_H_INCLUDED

#ifdef MULTITHREAD
struct G_PANEL_DATA {
#endif
G_STATIC int hardware_pattern;
G_STATIC int panelnumber;
G_STATIC int vmaxq;
G_STATIC int plns_glb;
#ifdef MULTITHREAD
};

struct G_PANEL_DATA *panel_data[GPC_MAX_THREADS];
#define pan_data panel_data[instctr]

#define hardware_pattern	pan_data->hardware_pattern
#define panelnumber			pan_data->panelnumber
#define vmaxq				pan_data->vmaxq
#define plns_glb			pan_data->plns_glb
#endif

void panproc2q(void);
void split_in_halfq(int xll, int yll, int xur, int yur);
void freeallq(int mode);
int clip_halfq(int dim, PGXYPAIRS xyp, int *num_panels,
	int *n_clusters, int xlq, int ylq, int xhq, int yhq);
int load_panq(void);
void figure_8_testq(struct GPANELQ *gp);
int split_panelq(struct GPANELQ *gp, int start_index);
int RemoveDuplicatePoints(PGXYPAIRS xyp, int npts);

#define PANFIL_H_INCLUDED
#endif
