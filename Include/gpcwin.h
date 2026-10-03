/******************************************************************
*
*  GPCWIN.H 	include file for GPCWIN global data variables
*			(c) 1992-1995 Scientific Endeavors Corp.
*					All rights reserved.
*	 Developed by Baylor Information Technology, Inc.
*
*******************************************************************/

#define MAXNAMESIZE 128

/* GPCWIN data structure definition */
typedef struct _GPCWIN_DATA {

	char OpenName[MAXNAMESIZE];	/* file name for tkf file */

	HACCEL hAccTable;           /* handle for accelerator table */

	HWND hAbortDlgWnd;          /* window handle for about dialog box */
	HWND hConvDlg;              /* window handle for conversion dialog boxes */
	HMENU hMenu;				/* Menu handle                   */
	HCURSOR hSaveCursor;		/* current cursor handle         */
	HCURSOR hHourGlass;			/* handle to hourglass cursor    */
	HCURSOR hCrossCursor;		/* cross cursor handle           */
	HCURSOR hXhairwCursor;		/* custom xhair cursor handle    */
	HCURSOR hArrowCursor;		/* arrow cursor handle           */

	BOOL bCancelq;				/* Cancel boolean flag for dialog boxes */
	BOOL convflgq;      		/* Conversion flag default=FALSE */
	BOOL prntflgq;      		/* Printing flag default=FALSE */
	BOOL prntsclq;      		/* Print scaling flag default=FALSE */
	BOOL Metaflgq;      		/* Metafile Conv flag default=FALSE */
	BOOL MetaStyle;      		/* Metafile style flag default=ENHANCED */
	BOOL copyflgq;      		/* Copy flag TRUE if copying or printing */
	BOOL bitbflgq;      		/* Bitblt flag TRUE if drawing to bitmap */
	BOOL bBmpbsflgq;			/* TRUE if bitmap backing store enabled */
	BOOL bTrack;        		/* TRUE if left button clicked   */
	BOOL newbuttq;       		/* TRUE if new zoom button       */
	BOOL zoominflg;     		/* TRUE if in Zoomin mode        */
	BOOL zoomoutflg;    		/* TRUE if in Zoomout mode       */
	BOOL drawflgq;      		/* TRUE if drawing page          */
	BOOL startflgq;     		/* TRUE if started drawing page  */
	BOOL rotatflgq;     		/* TRUE if started drawing page  */
	BOOL Wcontinueq;    		/* TRUE to continue drawing      */
	BOOL endflgq;       		/* TRUE when ready to exit       */
	BOOL curflgq;       		/* TRUE when in cursor mode 	 */
	BOOL prtwhtbk;       		/* TRUE if printing white bckgrnd*/
	BOOL prtwhtln;       		/* TRUE if printing white lines black*/
	BOOL bSaveConfig;    		/* TRUE if save config to INI file */
	BOOL bintekq;    			/* TRUE if in tekq function     */
	int NeedRedraw;             /* TRUE if window needs to be redrawn */
	int WOrgXq, WOrgYq; 		/* starting window position     */
	int nWidthXq;     			/* starting window width        */
	int OrgX, OrgY;     		/* original cursor position     */
	int PrevX, PrevY;   		/* current cursor position      */
	int Wupdatfreq;          	/* window update frequency 		*/
	long lplotq;            	/* length of tkf plot in bytes 	*/
	int hmpos, vmpos;			/* curson cursor position 		*/
	unsigned int curchq;		/* curson input value 			*/
	int numplotq;               /* number of plots in file      */
	WORD PSt, PSl, PSr;			/* orientation flags for conversions */
	int iWoption;           	/* Bitmap copy option */
} GPCWIN_DATA;

#define curchq gpcwin_data.curchq
#define lplotq gpcwin_data.lplotq
#define numplotq gpcwin_data.numplotq
#define hmpos gpcwin_data.hmpos
#define vmpos gpcwin_data.vmpos
#define curflgq gpcwin_data.curflgq
#define Wupdatfreq gpcwin_data.Wupdatfreq
#define PrevX gpcwin_data.PrevX
#define PrevY gpcwin_data.PrevY
#define OrgX gpcwin_data.OrgX
#define OrgY gpcwin_data.OrgY
#define WOrgXq gpcwin_data.WOrgXq
#define WOrgYq gpcwin_data.WOrgYq
#define nWidthXq gpcwin_data.nWidthXq
#define NeedRedraw gpcwin_data.NeedRedraw
#define startflgq gpcwin_data.startflgq
#define endflgq gpcwin_data.endflgq
#define Wcontinueq gpcwin_data.Wcontinueq
#define bSaveConfig gpcwin_data.bSaveConfig
#define bintekq gpcwin_data.bintekq
#define drawflgq gpcwin_data.drawflgq
#define rotatflgq gpcwin_data.rotatflgq
#define convflgq gpcwin_data.convflgq
#define prntflgq gpcwin_data.prntflgq
#define prntsclq gpcwin_data.prntsclq
#define prtwhtbk gpcwin_data.prtwhtbk
#define prtwhtln gpcwin_data.prtwhtln
#define Metaflgq gpcwin_data.Metaflgq
#define MetaStyle gpcwin_data.MetaStyle
#define copyflgq gpcwin_data.copyflgq
#define bitbflgq gpcwin_data.bitbflgq
#define bBmpbsflgq gpcwin_data.bBmpbsflgq
#define newbuttq gpcwin_data.newbuttq
#define bTrack gpcwin_data.bTrack
#define zoominflg gpcwin_data.zoominflg
#define zoomoutflg gpcwin_data.zoomoutflg
#define bCancelq gpcwin_data.bCancelq
#define hSaveCursor gpcwin_data.hSaveCursor
#define hArrowCursor gpcwin_data.hArrowCursor
#define hXhairwCursor gpcwin_data.hXhairwCursor
#define hCrossCursor gpcwin_data.hCrossCursor
#define hHourGlass gpcwin_data.hHourGlass
#define hSaveCursor gpcwin_data.hSaveCursor
#define hConvDlg gpcwin_data.hConvDlg
#define hMenu gpcwin_data.hMenu
#define hAccTable gpcwin_data.hAccTable
#define hAbortDlgWnd gpcwin_data.hAbortDlgWnd
#define OpenName gpcwin_data.OpenName
#define PSt gpcwin_data.PSt
#define PSr gpcwin_data.PSr
#define PSl gpcwin_data.PSl
#define iWoption gpcwin_data.iWoption

/* external function declarations */
extern void set_handlersq(void);	/* In BAUXWIN.C  */
extern void restore_handlersq(void);

/* external driver functions */
extern void	FTYPE PrintTKF(HWND hWnd, HINSTANCE hInst, struct PRINT_INFO prt_info);
extern void	FTYPE SetupWinDC(HDC hDC);   

extern void GPCWINGetConfig(void);	/* in BAUXWIN.C */
extern void GPCWINSaveConfig(void);	/* in BAUXWIN.C */

extern void convertq();				/* in CONVERTW.C */
extern void initialq(void);			/* initialq in BPLTW.C */
