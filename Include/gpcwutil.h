/************************************************************************
 *  GPCWUTIL.H		(c) 1991 Scientific Endeavors Corp.
 *				All rights reserved.
 *	   		   Developed by BIT, Inc.
 *
 *
 *  Header file for GPCWIN Utilities.  Provides
 *  function prototypes and constants for the following functions:
 *
 *  FindDIBBits()		- Sets pointer to the DIB bits
 *  DIBWidth()			- Gets the DIB width
 *  DIBHeight()			- Gets the DIB height
 *  DIBNumColors()		- Calculates number of colors in the DIB's color table
 *  PaletteSize()		- Calculates the buffer size required by a palette
 *  CreateDIBPalette()	- Creates a palette from a DIB
 *  DIBToBitmap()		- Creates a bitmap from a DIB
 *  BitmapToDIB()		- Creates a DIB from a bitmap
 *  PalEntriesOnDevice()- Gets the number of palette entries
 *  GetSystemPalette()	- Gets the current palette
 *  PrintDIB()       	- Prints a DIB
 *  CopyWindowToDIB()   - Copies a window to a DIB
 *  CopyScreenToDIB()   - Copies entire screen to a DIB
 *  CopyWindowToBitmap()- Copies a window to a standard Bitmap
 *  CopyScreenToBitmap()- Copies entire screen to a standard Bitmap
 *  SaveDIB()           - Saves the specified dib in a file
 *  LoadDIB()           - Loads a DIB from a file
 *  DestroyDIB()        - Deletes DIB when finished using it
 *
 *************************************************************************/

#define IDC_ENTIRESCREEN            311
#define IDC_PARTIALSCREEN           312
#define IDC_CLIENTAREAONLY          302
#define IDC_ENTIREWINDOW            301

/* Handle to a DIB */
#define HDIB HANDLE

/* Print Area selection */
#define PW_WINDOW        1
#define PW_CLIENT        2

/* Print Options selection */
#define PW_BESTFIT       1
#define PW_STRETCHTOPAGE 2
#define PW_SCALE         3

/* Function prototypes */
HDIB   	CopyWindowToDIB(HWND, WORD);
HDIB   	CopyScreenToDIB(LPRECT);
HBITMAP	CopyWindowToBitmap(HWND, WORD);
HBITMAP	CopyScreenToBitmap(LPRECT);
WORD    PrintDIB(HDIB, HDC, LPSTR);
WORD   	SaveDIB(HDIB, LPSTR);
HDIB   	LoadDIB(LPSTR);
WORD   	DestroyDIB(HDIB);

/* DIB constants */
#define PALVERSION   0x300

/* DIB macros */
#define WIDTHBYTES(bits)	(((bits) + 31) / 32 * 4)
#define IS_WIN30_DIB(lpbi)  ((*(LPDWORD)(lpbi)) == sizeof(BITMAPINFOHEADER))

/* function prototypes */
LPSTR    	FindDIBBits(LPSTR lpbi);
DWORD		DIBWidth(LPSTR lpDIB);
DWORD		DIBHeight(LPSTR lpDIB);
WORD     	DIBNumColors(LPSTR lpbi);
WORD     	PaletteSize(LPSTR lpbi);
HPALETTE 	CreateDIBPalette(HDIB hDIB);
HBITMAP  	DIBToBitmap(HDIB hDIB, HPALETTE hPal);
HDIB     	BitmapToDIB(HBITMAP hBitmap, HPALETTE hPal);
int			PalEntriesOnDevice(HDC hDC);
HPALETTE 	GetSystemPalette(void);

enum {
      ERR_MIN = 0,		       /* All error #s >= this value	  */
      ERR_NOT_DIB = 0,		       /* Tried to load a file, NOT a DIB!*/
      ERR_MEMORY,		       /* Not enough memory!		  */
      ERR_READ, 		       /* Error reading file!		  */
      ERR_LOCK, 		       /* Error on a GlobalLock()!	  */
      ERR_OPEN, 		       /* Error opening a file! 	  */
      ERR_CREATEPAL,		       /* Error creating palette.	  */
      ERR_GETDC,		       /* Couldn't get a DC.              */
      ERR_CREATEDDB,		       /* Error create a DDB.		  */
      ERR_STRETCHBLT,		       /* StretchBlt() returned failure.  */
      ERR_STRETCHDIBITS,	       /* StretchDIBits() returned failure.*/
      ERR_SETDIBITSTODEVICE,	       /* SetDIBitsToDevice() failed.	  */
      ERR_STARTDOC,		       /* Error calling StartDoc().	  */
      ERR_NOGDIMODULE,		       /* Couldn't find GDI module in memory.*/
      ERR_SETABORTPROC, 	       /* Error calling SetAbortProc().   */
      ERR_STARTPAGE,		       /* Error calling StartPage().	  */
      ERR_NEWFRAME,		       /* Error calling NEWFRAME escape.  */
      ERR_ENDPAGE,		       /* Error calling EndPage().	  */
      ERR_ENDDOC,		       /* Error calling EndDoc().	  */
      ERR_SETDIBITS,		       /* Error calling SetDIBits().	  */
      ERR_FILENOTFOUND, 	       /* Error opening file in GetDib()  */
      ERR_INVALIDHANDLE,	       /* Invalid Handle		  */
      ERR_DIBFUNCTION,		       /* Error on call to DIB function   */
      ERR_MAX			       /* All error #s < this value	  */
     };


void DIBError (int ErrNo);
