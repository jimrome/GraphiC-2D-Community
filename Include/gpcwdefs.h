/*************************************************************************
  FILE:  gpcwdefs.h     (c) 1994 by Scientific Endeavors Corp.
   				All rights reserved.
		Developed by Baylor Information Technology, Inc.


  PURPOSE:
     Macros and prototypes for Windows port of GraphiC 6.0.

  ORIGINAL:
	 24-Dec-1990  L.R. Baylor	Original for use with GraphiC6.0 for Windows 3.0
	  2-Sep-1994  L.R. Baylor   Updated for version 7.0a.
	 16-Jan-1995  L.R. Baylor	Updated for version 7.0d.
         27-Feb-1996  L.R. Baylor       Updated for version 7.1
*************************************************************************/

#define TRADITIONAL 1
#define ENHANCED    2
#define PLACEABLE   4

/* private message to inform "cancel" modeless	*/
/* dialog box that the % done has changed		*/
#define PM_UPDATESTATUS       WM_USER
#define IDC_METAENHANCED	569
#define PM_CLEARGASGAUGE      WM_USER + 1

/* This message starts the first picture. */
#define WM_USER_START         WM_USER + 2

/* This message performs redraw of current picture. */
#define WM_USER_REDRAW        WM_USER + 3

/* This message performs redraw of current picture. */
#define WM_USER_CANCEL        WM_USER + 4

/* This message performs redraw of current picture. */
#define WM_USER_MAIN          WM_USER + 5

/* This defines the first available message number. */
#define WM_GPC_USER           WM_USER + 6


/*  Help menu items  */
#define IDM_ABOUT 100
#define IDM_HELP_INDEX     131
#define IDM_HELP_KEYBOARD  132
#define IDM_HELP_HELP      133
#define IDD_WINDOWSMODE    214
#define IDD_FREEMEM 	   215
#define IDD_RESOURCES	   216
#define IDS_MODE_STANDARD  0x231
#define IDS_MODE_ENHANCED  0x232
#define IDS_MODE_WLO	   0x233
#define IDS_MODE_UNDEF	   0x234
#define IDS_INTEL          0x249
#define IDS_INTEL386       0x240
#define IDS_INTEL486       0x241
#define IDS_INTELP5	   0x242
#define IDS_MIPS	   0x243
#define IDS_ALPHA	   0x246
#define IDS_PPC 	   0x245

/*  GraphiCW menu items  */
#define IDM_CURSON 199
#define IDM_OPEN 191
#define IDM_SAVE 192
#define IDM_DRAWNEXT 101
#define IDM_REDRAW 102
#define IDM_SAVEAS 103
#define IDM_PRINT 104
#define IDM_PRINTSETUP 116
#define IDD_PRINTDEVICE 107
#define IDD_PRINTPORT   108
#define IDD_PRINTTITLE  109
#define IDC_PERCENTAGE  303
#define IDM_PREFERENCE 117
#define IDC_WHITEBACK 118
#define IDC_WHITELINE 119
#define IDC_PRNTSCL 121
#define IDC_SAVECONFIG 120
#define IDC_BMPBS 122
#define IDM_STOP 125
#define IDM_EXIT 105
#define IDC_FILENAME 400
#define IDC_EDIT 401
#define IDC_FILES 402
#define IDC_PATH 403
#define IDC_DIRS 404

#define IDM_ZOOMIN 201
#define IDM_ZOOMOUT 202
#define IDM_SKIP 194

/*  Convert menu items  */
#define IDC_CONVERT 560
#define IDD_GASGAUGE 561
#define IDD_PGASGAUGE 562

#define IDM_PIC 501
#define IDM_GEM 502
#define IDM_HPGL 503
#define IDM_POSTSCR 504
#define IDM_TIFF 505
#define IDM_META 506
#define IDM_BM 507
#define IDM_CGM 508
#define IDM_SCODL 509

#define IDC_PSFILENAME 510
#define IDC_PSEDIT 511
#define IDC_PSCOLOR 512
#define IDC_PSTIFFIN 513
#define IDC_PSt 514
#define IDC_PSr 515
#define IDC_PSl 516
#define IDC_PSLINEW 517
#define IDC_PSWEDIT 518
#define IDC_PSFONT 519
#define IDC_PSLEVEL1 592
#define IDC_PSLEVEL2 593
#define IDC_PSLEVEL 594
#define IDC_PSFEDIT 569
#define IDC_PO 570
#define IDC_PO2 576
#define IDC_PSLISTBOX 571
#define IDC_PSNOSEP 572
#define IDC_PSRGB 573
#define IDC_PSMYC 574
#define IDC_PSSEP 575


#define IDC_PICFILENAME 520
#define IDC_PICEDIT 521

#define IDC_GEMFILENAME 530
#define IDC_GEMEDIT 531

#define IDC_CGMFILENAME 430
#define IDC_CGMEDIT 431

#define IDC_SCODLFILENAME 433
#define IDC_SCODLEDIT 432

#define IDC_BMFILENAME 535
#define IDC_BMEDIT 536

#define IDC_TIFFFILENAME 540
#define IDC_TIFFEDIT 541
#define IDC_TIFF1 542
#define IDC_TIFF2 543
#define IDC_TIFF3 544
#define IDC_TIFF4 545

#define IDC_HPGLFILENAME 550
#define IDC_HPGLEDIT 551
#define IDC_HPGLL 552
#define IDC_HPGLM 553
#define IDC_HPGLLMAR 554
#define IDC_HPGLLMARE 555
#define IDC_HPGLTMAR 556
#define IDC_HPGLTMARE 557
#define IDC_HPGLXLEN 558
#define IDC_HPGLXLENE 559
#define IDC_HPGLXINV 588
#define IDC_HPGLYINV 589

#define IDC_METAFILENAME 565
#define IDC_METAEDIT 566
#define IDC_METATRADITIONAL 567
#define IDC_RADIOBUTTON1	517
#define IDC_METAPLACEABLE 568

#define IDC_INDAT 581
#define IDC_PROMPT 582

/*  Edit menu items  */
#define IDM_COPY 106
#define IDM_COPYMETA 110

/*  Cursor dialog box items  */
#define IDC_CURSOR 590
#define IDC_CURSORLOC 591

/* GPCWApp definition */
#define IDM_PLOT 198



/*  Prototypes of exported functions  */
BOOL WINAPI About(HWND, UINT, UINT, LONG);
int WINAPI AbortDlg(HWND, UINT, UINT, LONG);
int WINAPI PSDlg(HWND, UINT, UINT, LONG);
int WINAPI PICDlg(HWND, UINT, UINT, LONG);
int WINAPI BMDlg(HWND, UINT, UINT, LONG);
int WINAPI GEMDlg(HWND, UINT, UINT, LONG);
int WINAPI TIFFDlg(HWND, UINT, UINT, LONG);
int WINAPI HPGLDlg(HWND, UINT, UINT, LONG);
int WINAPI METADlg(HWND, UINT, UINT, LONG);
int WINAPI CGMDlg(HWND, UINT, UINT, LONG);
int WINAPI SCODLDlg(HWND, UINT, UINT, LONG);
BOOL WINAPI ConvDlgProc(HWND, UINT, UINT, LONG);
int WINAPI INDATDlg(HWND, UINT, UINT, LONG);
BOOL WINAPI PREFDlg(HWND, UINT, UINT, LONG);
long WINAPI TextWndProc(HWND, UINT, UINT, LONG);
int CALLBACK FileHookProc(HWND, UINT, UINT, LONG);
/* GPCWApp prototype */
long FAR APIENTRY GPCWndProc(HWND, UINT, UINT, LONG);
