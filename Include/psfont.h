/* (c) 1984-1993 by Scientific Endeavors Corporation.
   All rights reserved.
Version 7.0 PostScript/Truetype extensions written by David Dillow.
**************************************************************/

void SetPSKerning(char onoff, char string);
void SetPSMaxError(float newerror, char string);
void GetPSWidest(int ifont, int *w, unsigned char *char_num);
int GetPSCharWidth(int ifont, unsigned char char_num);
void SetPSforOS2(void);
void FreePSMemory(int n);
int LoadPSFont(int n, char *filename);
void findkernpair(int lastchar, int nextchar, float *xoff);
int getEncodingq(char *buffer, FILE *file);
void getFontMatrixq(char *buffer, FILE *file, int fontnumber);
int getCharStringsq(char *buffer, FILE *file, int fontnumber);
int getSubrsq(char *buffer, FILE *file, int fontnumber);
int findkeyq(char *string);
unsigned char decryptq(unsigned char cipher, unsigned short int *r);
int get_nextq(FILE *file, char *buffer, char *terminator);
int get_bytesq(FILE *file, char *buffer, int number);
void getAFMinfoq(int fontnumber, char *name);
void handlePSFontq(int last, int n, int action, float *wid, int *top, int *bottom);
void draw_bezierq(float x1, float y1,float x2, float y2, float x3, float y3, float x4, float y4);
void PSdrawq(float x1, float y1);
void multiplymatrixq(float *m1, float *m2);
void calcFudgeq(int n);
int find_encodingq(char *buffer);
int getTypeval(float *operands, int *op_ptr, Uchar *current_charstr, int *char_ptr, int *value);
void SetPSFull(char onoff, char string);

/* PostScript Message Codes */

#define PS_ERR 'P'
#define PS_WARN 'p'

#define PS_NOTPSFONT 1
#define PS_INVALIDFONT 2
#define PS_WARNNOAFM 3
#define PS_WARNKERNMEM 4
#define PS_NOSTDENC 5
#define PS_NOENCMEM 6
#define PS_NOMEMORY 7
#define PS_UNSUPPORTED 8
#define PS_ERRORMM 9
