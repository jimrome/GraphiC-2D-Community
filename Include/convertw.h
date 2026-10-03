#ifndef CONVERTW_H_INCLUDED

/**** FUNCTION DECLARATIONS *********************************/
void convertq();
#ifdef GPC_MFC
HINSTANCE LoadFileConversionLibrary(char *Driver);
#else
HANDLE LoadFileConversionLibrary(char *Driver);
#endif
void PS_convq(void);
void PSformatq(void);
void pic_convq(void);
void cgm_convq(void);
void gem_convq(void);
void TIFF_convq(void);
void scodl_convq(void);
int TIFF_makeq(char *str);
void hp_convq(void);
void BMConvq(HWND hWnd);
void WMFConvq(HWND hWnd);

/**** EXTERNAL FUNCTION DECLARATIONS *********************************/
#if !GPC_CPP
extern void BMConvq(HWND hWnd);
extern void WMFConvq(HWND hWnd);
extern void ReplaceExt(LPSTR, LPSTR);
#endif

#define CONVERTW_H_INCLUDED
#endif
