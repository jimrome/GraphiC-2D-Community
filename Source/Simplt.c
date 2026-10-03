/********************************************************************
    (c) 1991 - 1993 by Scientific Endeavors Corporation.
    All rights reserved.

	This file contains the routines for simultaneous plotting.
Version 7.0 3/4/93
********************************************************************/

/************************ includes/defines *************************/
#include <graphic.h>
#include <simplt.h>

#define PUT_HANDLES		4
/********************************************************************
    FUNCTION NAME: simuplot
    ARGUMENTS: numplots -- the number of simultaneous plots
    PURPOSE: Allocate space and set flag or
		     free space and reset flag
********************************************************************/
void TYPE GCLASS simuplot(int numplots)
{
	int i, j;
	int ch;
	unsigned char action;
	G_PROCESS_ID

	if (numplots > 1) {
		sim_flgq = numplots;
		plotparamsq = (PPLOTPARMBLK)gpcalloc(numplots,
			sizeof(PLOTPARMBLK));
		if (plotparamsq == (void *)NULL) {
			sim_flgq = 0;
			messageq(ERRQ, 1);		/* Out of Memory */
			return;
		}
		if (!notekq) {
			tmpfilesq = (FILE **)gpcalloc((unsigned)numplots,
				sizeof(FILE *));
			tkf_pointer_save = (FILE **)gpcalloc((unsigned)numplots,
				sizeof(FILE *));
			tmpnamesq = (char **)gpcalloc((unsigned)numplots,
				sizeof(char *));
			if (tmpfilesq == NULL || tmpnamesq == NULL) {
				gpcfree((void **) &tmpnamesq);
				gpcfree((void **) &tmpfilesq);
				gpcfree((void **) &plotparamsq);
				sim_flgq = 0;
				messageq(ERRQ, 1);		/* Out of Memory */
				return;
			}
			for (i = 0; i < numplots; i++)
				tkf_pointer_save[i] = tkf_pointerq;
			for (i = 0; i < numplots; i++) {
				tmpnamesq[i] = NULL;
				tmpnamesq[i] = (char *)gpcalloc((unsigned)L_tmpnam,
					sizeof(char));
				if (tmpnamesq[i] == NULL) {
					tkf_pointerq = tkf_pointer_save[0];
					messageq(ERRQ, 1);		/* Out of Memory */
				}
				tmpnam((char *)tmpnamesq[i]);
				tmpfilesq[i] = gfopen((char *)tmpnamesq[i], "w+b", 1);
				if (tmpfilesq[i] == NULL) {
					for (j = i - 1; j >= 0; j--) {
						gfclose((FILE *)tmpfilesq[j]);
						remove((char *)tmpnamesq[j]);
						gpcfree((void **) &tmpnamesq[i]);
					}
					gpcfree((void **) &tmpnamesq);
					gpcfree((void **) &tmpfilesq);
					gpcfree((void **) &plotparamsq);
					sim_flgq = 0;
					messageq(ERRQ, 2);		/* Error opening temp file */
					return;
				}
                                       /* Initialize line and panel colors */
				tkf_pointerq = (FILE *)tmpfilesq[i];
				ccolorq('g', g_lq.color);
				ccolorq('p', g_pq.pattern);
			}
			tkf_pointerq = (FILE *)tmpfilesq[0];
		}
		plot_initq();
		for (plotnumq = 0; plotnumq < numplots; plotnumq++)
			saveplotq();
		plotnumq = 0;
	}
	else if (sim_flgq) {
		if (!notekq) {
			tkf_pointerq = tkf_pointer_save[0];
			for (i = 0; i < sim_flgq; i++) {
				fflush((FILE *)tmpfilesq[i]);
				fseek((FILE *)tmpfilesq[i], 0L, SEEK_SET);
				while ((ch = fgetc((FILE *)tmpfilesq[i])) != EOF)
					fputc(ch, tkf_pointer_save[i]);
				if (i > 0) {
					if (tkf_pointerq != tkf_pointer_save[i]) {
						tkf_pointerq = tkf_pointer_save[i];
						action = END_TKF_PLOT;
						tkbfillq((char *)&action);
						gfclose(tkf_pointerq);
					}
					gfclose((FILE *)tmpfilesq[i]);
					remove((char *)tmpnamesq[i]);
					gpcfree((void **) &tmpnamesq[i]);
					tkf_pointerq = tkf_pointer_save[i];
					tkf_pointer_save[i] = NULL;
				}
			}
			tkf_pointerq = tkf_pointer_save[0];
			tkf_pointer_save[0] = NULL;
			gfclose((FILE *)tmpfilesq[0]);
			remove((char *)tmpnamesq[0]);
			gpcfree((void **) &tmpnamesq[0]);
			gpcfree((void **) &tmpnamesq);
			gpcfree((void **) &tmpfilesq);
		}
		gpcfree((void **) &plotparamsq);
		sim_flgq = 0;
		plotnumq = 0;
	}
}

/********************************************************************
    FUNCTION NAME: saveplotq
    ARGUMENTS: none
    PURPOSE: Save all global parameters in a structure
********************************************************************/
void GCLASS saveplotq(void)
{
	G_PROCESS_ID

	if (plotnumq < sim_flgq) {
		plotparamsq[plotnumq].xphysor = xphysorq;
		plotparamsq[plotnumq].yphysor = yphysorq;
		plotparamsq[plotnumq].xzero = xzeroq;
		plotparamsq[plotnumq].yzero = yzeroq;
		plotparamsq[plotnumq].xpage = xpageq;
		plotparamsq[plotnumq].ypage = ypageq;
		plotparamsq[plotnumq].xbase = xbaseq;
		plotparamsq[plotnumq].ybase = ybaseq;
		plotparamsq[plotnumq].plot_type = plot_typeq;
		plotparamsq[plotnumq].xlen = xlenq;
		plotparamsq[plotnumq].ylen = ylenq;
		plotparamsq[plotnumq].xorig = xorigq;
		plotparamsq[plotnumq].yorig = yorigq;
		plotparamsq[plotnumq].xmax = xmaxq;
		plotparamsq[plotnumq].ymax = ymaxq;
		plotparamsq[plotnumq].xfact_plt = xfact_pltq;
		plotparamsq[plotnumq].yfact_plt = yfact_pltq;
		plotparamsq[plotnumq].linclr = g_lq.color;
		plotparamsq[plotnumq].linsty = g_lq.pattern;
		plotparamsq[plotnumq].linthk = g_lq.thick;
		plotparamsq[plotnumq].panclr = g_pq.pattern;
		plotparamsq[plotnumq].border = g_pq.border;
		plotparamsq[plotnumq].brdclr = g_pq.b_color;
		plotparamsq[plotnumq].pattern_save = pattern_saveq;
		plotparamsq[plotnumq].lcolor_save = lcolor_saveq;
		plotparamsq[plotnumq].x0 = x0q;
		plotparamsq[plotnumq].y0 = y0q;
		plotparamsq[plotnumq].pfx = g_lq.pfx;
		plotparamsq[plotnumq].pfy = g_lq.pfy;
		plotparamsq[plotnumq].hxold = hxoldq;
		plotparamsq[plotnumq].hyold = hyoldq;
		plotparamsq[plotnumq].lyold = lyoldq;
		plotparamsq[plotnumq].lsxold = lsxoldq;
		plotparamsq[plotnumq].lsyold = lsyoldq;
		plotparamsq[plotnumq].symnum = symcntq;
		plotparamsq[plotnumq].symht = symhtq;
		plotparamsq[plotnumq].penup = penup_flgq;
		plotparamsq[plotnumq].dot = dotq;
		plotparamsq[plotnumq].lablht = lablhtq;
		plotparamsq[plotnumq].tkht = tkhtq;
#if GPC_OS2 || GPC_WIN
		gpcHandles(GET_HANDLES, &(plotparamsq[plotnumq].gpch));
#endif
	}
}

/********************************************************************
    FUNCTION NAME: context
    ARGUMENTS: plot -- index of the plot context to be restored
    PURPOSE: Restore global parameters
********************************************************************/
void TYPE GCLASS context(int plot)
{
	G_PROCESS_ID

	if (plot < 0 || plot >= sim_flgq) {
		messageq(ERRQ, 28);                      /* Invalid context number */
		return;
	}
	if (plot != plotnumq) {
		saveplotq();
		if (!notekq)
			tkf_pointerq = (FILE *)tmpfilesq[plot];
		xphysorq = plotparamsq[plot].xphysor;
		yphysorq = plotparamsq[plot].yphysor;
		xzeroq = plotparamsq[plot].xzero;
		yzeroq = plotparamsq[plot].yzero;
		xpageq = plotparamsq[plot].xpage;
		ypageq = plotparamsq[plot].ypage;
		xbaseq = plotparamsq[plot].xbase;
		ybaseq = plotparamsq[plot].ybase;
		plot_typeq = plotparamsq[plot].plot_type;
		xlenq = plotparamsq[plot].xlen;
		ylenq = plotparamsq[plot].ylen;
		xorigq = plotparamsq[plot].xorig;
		yorigq = plotparamsq[plot].yorig;
		xmaxq = plotparamsq[plot].xmax;
		ymaxq = plotparamsq[plot].ymax;
		xfact_pltq = plotparamsq[plot].xfact_plt;
		yfact_pltq = plotparamsq[plot].yfact_plt;
		g_lq.color = plotparamsq[plot].linclr;
		g_lq.pattern = plotparamsq[plot].linsty;
		g_lq.thick = plotparamsq[plot].linthk;
		g_pq.pattern = plotparamsq[plot].panclr;
		g_pq.border = plotparamsq[plot].border;
		g_pq.b_color = plotparamsq[plot].brdclr;
		pattern_saveq = plotparamsq[plot].pattern_save;
		lcolor_saveq = plotparamsq[plot].lcolor_save;
		x0q = plotparamsq[plot].x0;
		y0q = plotparamsq[plot].y0;
		g_lq.pfx = plotparamsq[plot].pfx;
		g_lq.pfy = plotparamsq[plot].pfy;
		hxoldq = plotparamsq[plot].hxold;
		hyoldq = plotparamsq[plot].hyold;
		lyoldq = plotparamsq[plot].lyold;
		lsxoldq = plotparamsq[plot].lsxold;
		lsyoldq = plotparamsq[plot].lsyold;
		symcntq = plotparamsq[plot].symnum;
		symhtq = plotparamsq[plot].symht;
		g_lq.penup = penup_flgq = plotparamsq[plot].penup;
		dotq = plotparamsq[plot].dot;
		lablhtq = plotparamsq[plot].lablht;
		tkhtq = plotparamsq[plot].tkht;
		plotnumq = plot;
#if GPC_OS2 || GPC_WIN
		gpcHandles(PUT_HANDLES, &(plotparamsq[plot].gpch));
#endif
	}
}
/********************************************************************
********************************************************************/
void save_pointerq(void)
{
	G_PROCESS_ID

	tkf_pointer_save[plotnumq] = tkf_pointerq;
	tkf_pointerq = (FILE *)tmpfilesq[plotnumq];
}
