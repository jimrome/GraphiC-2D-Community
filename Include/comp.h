#ifndef COMP_H
#define COMP_H

/* Compilers */
#if defined(_MSC_VER)
  #ifndef MSQ
    #define MSQ 0
  #endif
#else
  #ifndef MSQ
    #define MSQ 0
  #endif
#endif

#ifndef TCQ
  #define TCQ 0
#endif
#ifndef WCQ
  #define WCQ 0
#endif
#ifndef IBMQ
  #define IBMQ 0
#endif

/* Operating Systems / Targets */
#ifndef GPC_DOS
  #define GPC_DOS 0
#endif
#ifndef GPC_EX2
  #define GPC_EX2 0
#endif
#ifndef GPC_OS2
  #define GPC_OS2 0
#endif

#if defined(__APPLE__)
  #ifndef GPC_MAC
    #define GPC_MAC 1
  #endif
  #ifndef GPC_UNX
    #if defined(USE_X11)
      #define GPC_UNX 1
    #else
      #define GPC_UNX 0
    #endif
  #endif
  #ifndef GPC_WIN
    #define GPC_WIN 0
  #endif
  #ifndef GPC_PORTABLE
    #define GPC_PORTABLE 1
  #endif
#elif defined(_WIN32) || defined(_WIN64)
  #ifndef GPC_MAC
    #define GPC_MAC 0
  #endif
  #ifndef GPC_UNX
    #define GPC_UNX 0
  #endif
  #ifndef GPC_WIN
    #define GPC_WIN 0
  #endif
  #ifndef GPC_PORTABLE
    #define GPC_PORTABLE 1
  #endif
#elif defined(__unix__) || defined(__linux__)
  #ifndef GPC_MAC
    #define GPC_MAC 0
  #endif
  #ifndef GPC_UNX
    #if defined(USE_X11)
      #define GPC_UNX 1
    #else
      #define GPC_UNX 0
    #endif
  #endif
  #ifndef GPC_WIN
    #define GPC_WIN 0
  #endif
  #ifndef GPC_PORTABLE
    #define GPC_PORTABLE 1
  #endif
#else
  #ifndef GPC_MAC
    #define GPC_MAC 0
  #endif
  #ifndef GPC_WIN
    #define GPC_WIN 0
  #endif
  #ifndef GPC_UNX
    #define GPC_UNX 0
  #endif
  #ifndef GPC_PORTABLE
    #define GPC_PORTABLE 1
  #endif
#endif

#if defined(GPC_PORTABLE) && GPC_PORTABLE
  #undef GPC_WIN
  #define GPC_WIN 0
  #undef GPC_OS2
  #define GPC_OS2 0
  #undef GPC_DOS
  #define GPC_DOS 0
  #undef GPC_EX2
  #define GPC_EX2 0
  #undef MSQ
  #define MSQ 0
  #undef TCQ
  #define TCQ 0
  #undef WCQ
  #define WCQ 0
  #undef IBMQ
  #define IBMQ 0
#endif

#endif /* COMP_H */

