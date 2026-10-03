/********************************************************************
    FILE: baux_portable.c
    (c) 1984-2026 Scientific Endeavors Corporation / Modernized Port
    PURPOSE: Portable POSIX/C99 platform auxiliary functions for GraphiC.
             Replaces old BAUXDOS.C / BAUXWIN.C with standard ANSI/POSIX C.
********************************************************************/

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <io.h>
#include <process.h>
#ifdef SetDefaultPrinter
#undef SetDefaultPrinter
#endif
#if !defined(__MINGW32__)
#define strtok_r(str, delim, saveptr) strtok_s(str, delim, saveptr)
#endif
#define GPC_PATH_DELIMS ";"
#else
#include <sys/time.h>
#include <unistd.h>
#include <dlfcn.h>
#define GPC_PATH_DELIMS ":;"
#endif
#include <sys/stat.h>
#include <graphic.h>

#define MAX_TIMERS 4
static double timer_start_time[MAX_TIMERS];

/********************************************************************/
/* Store root of filename in tkfnameq                               */
/********************************************************************/
void getfilenameq(char *fname)
{
	int j, k, m;
	char ch;
	G_PROCESS_ID

	if (fname == NULL)
		return;

	j = (int)strlen(fname);
	m = j;
	for (k = j; k > 0; k--) {
		ch = fname[k - 1];
		if (ch == '.') {
			m = k;
			break;
		}
	}
	if (tkfnameq != NULL)
		gpcfree((void **)&tkfnameq);

	tkfnameq = (char *)gpcalloc(m + 1, sizeof(char));
	if (tkfnameq != NULL) {
		strncpy(tkfnameq, fname, m > 1 && fname[m - 1] == '.' ? m - 1 : m);
		tkfnameq[m] = '\0';
	}
}

#if !defined(_WIN32)
#define _GNU_SOURCE
#include <dlfcn.h>
#if defined(__APPLE__)
#include <mach-o/dyld.h>
#endif
#endif

/********************************************************************/
/* Try opening file at dir/filename, with case variations for read  */
/********************************************************************/
static FILE *try_file(const char *dir, const char *filename, const char *mode)
{
	char buff[1024];
	FILE *fp = NULL;

	if (filename == NULL || mode == NULL)
		return NULL;

	if (dir != NULL && dir[0] != '\0') {
		snprintf(buff, sizeof(buff), "%s/%s", dir, filename);
	} else {
		snprintf(buff, sizeof(buff), "%s", filename);
	}

	fp = fopen(buff, mode);
	if (fp != NULL)
		return fp;

	/* If reading, try case variations on case-sensitive filesystems */
	if (mode[0] == 'r') {
		char alt[256];
		size_t len = strlen(filename);
		if (len > 0 && len < sizeof(alt)) {
			/* 1. Titlecase (first char uppercase, rest lower before dot) */
			strcpy(alt, filename);
			alt[0] = (char)toupper((unsigned char)alt[0]);
			for (size_t i = 1; i < len; i++) {
				if (alt[i] == '.') break;
				alt[i] = (char)tolower((unsigned char)alt[i]);
			}
			if (strcmp(alt, filename) != 0) {
				if (dir != NULL && dir[0] != '\0')
					snprintf(buff, sizeof(buff), "%s/%s", dir, alt);
				else
					snprintf(buff, sizeof(buff), "%s", alt);
				fp = fopen(buff, mode);
				if (fp != NULL) return fp;
			}

			/* 2. All lowercase */
			for (size_t i = 0; i < len; i++)
				alt[i] = (char)tolower((unsigned char)filename[i]);
			alt[len] = '\0';
			if (strcmp(alt, filename) != 0) {
				if (dir != NULL && dir[0] != '\0')
					snprintf(buff, sizeof(buff), "%s/%s", dir, alt);
				else
					snprintf(buff, sizeof(buff), "%s", alt);
				fp = fopen(buff, mode);
				if (fp != NULL) return fp;
			}

			/* 3. All uppercase */
			for (size_t i = 0; i < len; i++)
				alt[i] = (char)toupper((unsigned char)filename[i]);
			alt[len] = '\0';
			if (strcmp(alt, filename) != 0) {
				if (dir != NULL && dir[0] != '\0')
					snprintf(buff, sizeof(buff), "%s/%s", dir, alt);
				else
					snprintf(buff, sizeof(buff), "%s", alt);
				fp = fopen(buff, mode);
				if (fp != NULL) return fp;
			}
		}
	}
	return NULL;
}

/********************************************************************/
/* Search up to max_levels parent directories and subdirectories   */
/********************************************************************/
static FILE *check_dir_hierarchy(const char *basedir, const char *filename, const char *mode, int max_levels)
{
	char curdir[1024];
	FILE *fp = NULL;

	if (basedir == NULL || basedir[0] == '\0')
		strcpy(curdir, ".");
	else
		snprintf(curdir, sizeof(curdir), "%s", basedir);

	for (int level = 0; level <= max_levels; level++) {
		/* Try curdir/filename */
		fp = try_file(curdir, filename, mode);
		if (fp != NULL) return fp;

		/* Try subdirectories: fonts, Fonts, Include, Source */
		char subdir[1024];
		snprintf(subdir, sizeof(subdir), "%s/fonts", curdir);
		fp = try_file(subdir, filename, mode);
		if (fp != NULL) return fp;

		snprintf(subdir, sizeof(subdir), "%s/Fonts", curdir);
		fp = try_file(subdir, filename, mode);
		if (fp != NULL) return fp;

		snprintf(subdir, sizeof(subdir), "%s/Include", curdir);
		fp = try_file(subdir, filename, mode);
		if (fp != NULL) return fp;

		snprintf(subdir, sizeof(subdir), "%s/Source", curdir);
		fp = try_file(subdir, filename, mode);
		if (fp != NULL) return fp;

		/* Move up one directory level */
		char nextdir[1024];
		snprintf(nextdir, sizeof(nextdir), "%s/..", curdir);
		strncpy(curdir, nextdir, sizeof(curdir) - 1);
		curdir[sizeof(curdir) - 1] = '\0';
	}
	return NULL;
}

/********************************************************************/
/* Open file helper with GPC environment and multi-level searching  */
/********************************************************************/
FILE * TYPE gfopen(char *filename, char *mode, size_t bigbuff)
{
	FILE *fptr = NULL;

	if (filename == NULL || mode == NULL)
		return NULL;

	/* If write mode or not read, open directly */
	if (mode[0] != 'r') {
		fptr = fopen(filename, mode);
		if (fptr != NULL && bigbuff > 0) {
			if (bigbuff == 1) bigbuff = 0x8000;
			setvbuf(fptr, NULL, _IOFBF, bigbuff);
		}
		return fptr;
	}

	/* 1. Search current directory and up to 5 parent levels */
	fptr = check_dir_hierarchy(".", filename, mode, 5);
	if (fptr != NULL) goto done;

	/* 2. Search environment variables: GRAPHIC, GPC, GPC_FONTS, GRAPHIC_DIR */
	{
		const char *env_vars[] = {"GRAPHIC", "GPC", "GPC_FONTS", "GRAPHIC_DIR", NULL};
		for (int e = 0; env_vars[e] != NULL; e++) {
			char *env_val = getenv(env_vars[e]);
			if (env_val != NULL && env_val[0] != '\0') {
				char pathbuff[1024];
				char *token;
				char *saveptr = NULL;
				strncpy(pathbuff, env_val, sizeof(pathbuff) - 1);
				pathbuff[sizeof(pathbuff) - 1] = '\0';

				token = strtok_r(pathbuff, GPC_PATH_DELIMS, &saveptr);
				while (token != NULL) {
					fptr = check_dir_hierarchy(token, filename, mode, 3);
					if (fptr != NULL) goto done;
					token = strtok_r(NULL, GPC_PATH_DELIMS, &saveptr);
				}
			}
		}
	}

	/* 3. Search relative to the executable path and library path */
#if defined(_WIN32)
	{
		char exepath[1024];
		if (GetModuleFileNameA(NULL, exepath, sizeof(exepath)) > 0) {
			char *slash = strrchr(exepath, '\\');
			if (slash == NULL) slash = strrchr(exepath, '/');
			if (slash != NULL) {
				*slash = '\0';
				fptr = check_dir_hierarchy(exepath, filename, mode, 5);
				if (fptr != NULL) goto done;
			}
		}

		HMODULE hm = NULL;
		if (GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
		                       GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
		                       (LPCSTR)&gfopen, &hm)) {
			if (GetModuleFileNameA(hm, exepath, sizeof(exepath)) > 0) {
				char *slash = strrchr(exepath, '\\');
				if (slash == NULL) slash = strrchr(exepath, '/');
				if (slash != NULL) {
					*slash = '\0';
					fptr = check_dir_hierarchy(exepath, filename, mode, 5);
					if (fptr != NULL) goto done;
				}
			}
		}
	}
#else
	{
		/* Check via dladdr (library or executable) */
		Dl_info dlinfo;
		if (dladdr((void *)gfopen, &dlinfo) && dlinfo.dli_fname != NULL) {
			char libdir[1024];
			strncpy(libdir, dlinfo.dli_fname, sizeof(libdir) - 1);
			libdir[sizeof(libdir) - 1] = '\0';
			char *slash = strrchr(libdir, '/');
			if (slash != NULL) {
				*slash = '\0';
				fptr = check_dir_hierarchy(libdir, filename, mode, 5);
				if (fptr != NULL) goto done;
			}
		}

#if defined(__APPLE__)
		{
			char exepath[1024];
			uint32_t size = sizeof(exepath);
			if (_NSGetExecutablePath(exepath, &size) == 0) {
				char *slash = strrchr(exepath, '/');
				if (slash != NULL) {
					*slash = '\0';
					fptr = check_dir_hierarchy(exepath, filename, mode, 5);
					if (fptr != NULL) goto done;
				}
			}
		}
#elif defined(__linux__)
		{
			char exepath[1024];
			ssize_t len = readlink("/proc/self/exe", exepath, sizeof(exepath) - 1);
			if (len > 0) {
				exepath[len] = '\0';
				char *slash = strrchr(exepath, '/');
				if (slash != NULL) {
					*slash = '\0';
					fptr = check_dir_hierarchy(exepath, filename, mode, 5);
					if (fptr != NULL) goto done;
				}
			}
		}
#endif
	}
#endif

done:
	if (fptr != NULL && bigbuff > 0) {
		if (bigbuff == 1)
			bigbuff = 0x8000;
		setvbuf(fptr, NULL, _IOFBF, bigbuff);
	}
	return fptr;
}

int TYPE gfclose(FILE *file)
{
	if (file != NULL)
		return fclose(file);
	return 0;
}

FILE * TYPE gpc_tmpfile(void)
{
	FILE *fp = tmpfile();
	if (fp != NULL)
		return fp;

#if defined(_WIN32)
	/* Windows fallback when tmpfile() fails (e.g. non-admin permission error on C:\) */
	char temp_path[MAX_PATH];
	char temp_file[MAX_PATH];
	DWORD ret = GetTempPathA(sizeof(temp_path), temp_path);
	if (ret > 0 && ret < sizeof(temp_path)) {
		if (GetTempFileNameA(temp_path, "gpc", 0, temp_file)) {
			fp = fopen(temp_file, "w+bTD");
			if (fp == NULL) {
				fp = fopen(temp_file, "w+b");
			}
			return fp;
		}
	}
	fp = fopen(".gpc_tmp.tmp", "w+bTD");
	if (fp == NULL) {
		fp = fopen(".gpc_tmp.tmp", "w+b");
	}
	return fp;
#else
	/* POSIX fallback using $TMPDIR or /tmp */
	char temp_file[512];
	const char *tmpdir = getenv("TMPDIR");
	if (!tmpdir) tmpdir = "/tmp";
	snprintf(temp_file, sizeof(temp_file), "%s/gpc_tmp_%d_%ld.tmp", tmpdir, (int)getpid(), (long)clock());
	fp = fopen(temp_file, "w+b");
	if (fp != NULL) {
		unlink(temp_file);
	}
	return fp;
#endif
}

/********************************************************************/
/* Open offset file (.OFF) corresponding to .TKF or .SVG            */
/********************************************************************/
void openoffq(char *fname)
{
	(void)fname;
	tofileq = NULL;
}

/********************************************************************/
/* Date & Time fill                                                 */
/********************************************************************/
void get_dtq(struct DATE_TIMEQ *dt)
{
	time_t t = time(NULL);
	struct tm *lt = localtime(&t);
	if (dt != NULL && lt != NULL) {
		dt->year = lt->tm_year + 1900;
		dt->month = lt->tm_mon + 1;
		dt->day = lt->tm_mday;
		dt->hour = lt->tm_hour;
		dt->minute = lt->tm_min;
		dt->second = lt->tm_sec;
	}
}

/********************************************************************/
/* Timer functions                                                  */
/********************************************************************/
static double get_current_time_sec(void)
{
#if defined(_WIN32)
	LARGE_INTEGER freq, counter;
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&counter);
	return (double)counter.QuadPart / (double)freq.QuadPart;
#else
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (double)tv.tv_sec + (double)tv.tv_usec * 1e-6;
#endif
}

void TYPE set_time(int timer)
{
	if (timer >= 0 && timer < MAX_TIMERS)
		timer_start_time[timer] = get_current_time_sec();
}

char * TYPE get_time(int timer)
{
	static char time_str[64];
	double elapsed, rem;
	int hours, mins, secs, tenths;

	if (timer < 0 || timer >= MAX_TIMERS)
		return "00:00:00.0";

	elapsed = get_current_time_sec() - timer_start_time[timer];
	if (elapsed < 0) elapsed = 0;

	hours = (int)(elapsed / 3600.0);
	rem = elapsed - (hours * 3600.0);
	mins = (int)(rem / 60.0);
	rem = rem - (mins * 60.0);
	secs = (int)rem;
	tenths = (int)((rem - secs) * 10.0);

	snprintf(time_str, sizeof(time_str), "%02d:%02d:%02d.%01d", hours, mins, secs, tenths);
	return time_str;
}

/********************************************************************/
/* Beep and terminal cues                                           */
/********************************************************************/
void TYPE gpcBeep(int freq, int duration)
{
	(void)freq;
	if (duration > 0) {
		/* Audible bell */
		fputc('\a', stderr);
		fflush(stderr);
	}
}

void ATYPE blank(Uchar attr)
{
	(void)attr;
	/* In modern terminal, can emit ANSI clear sequence */
	printf("\033[2J\033[H");
	fflush(stdout);
}

void ATYPE rowcolq(int row, int col)
{
	printf("\033[%d;%dH", row + 1, col + 1);
	fflush(stdout);
}

void ATYPE scr_aputsq(char *string, int attribute)
{
	(void)attribute;
	if (string != NULL)
		fputs(string, stdout);
}

int TYPE ciq(void)
{
	return getchar();
}

int TYPE ci_echoq(void)
{
	return getchar();
}

unsigned int TYPE cstsq(void)
{
	return 0;
}

int TYPE ci_scq(void)
{
	const char *nonint = getenv("GPC_NONINTERACTIVE");
	if (nonint != NULL) {
		return 13;
	}
#if !defined(_WIN32)
	if (!isatty(0))
		return 13;
#else
	if (!_isatty(0))
		return 13;
#endif
	int ch = getchar();
	if (ch == EOF)
		return 13;
	if (ch == '\n' || ch == '\r')
		return 13;
	return ch;
}

int TYPE ci_sc1q(void)
{
	return 0;
}

void TYPE gpcTextMode(int mode)
{
	(void)mode;
}

void TYPE screentype(int mode)
{
	(void)mode;
}

int new_fileq(void)
{
	return 'q';
}

void gdriverq(int type)
{
	(void)type;
}

void gterminateq(void)
{
	f_tableq(&g_quitq);
}

void init_videoq(int in_play, int mp)
{
	int i;
	char *envptr;

	dev_typeq = (char)VIDEO;
	envptr = getenv("GRAPHIC");
	if (envptr == NULL)
		envptr = getenv("GPC");
	if (envptr == NULL)
		envptr = ".";

	for (i = 0; i < 3; i++) {
		g_iq[i].path_str = envptr;
		g_iq[i].fnt_array = PSfntarrayq;
		g_iq[i].in_play = 0;
		g_iq[i].vector = ALL_POLYGONS | VECTOR_DEVICE;
	}

	g_iq[VIDEO].type = mp;
	g_iq[VIDEO].in_play = in_play;
	g_iq[VIDEO].vmax = MAXRESYMO;
	g_iq[VIDEO].hmax = MAXRESXMO;

	f_tableq(&g_iq[VIDEO]);
}

unsigned int gtacharq(void)
{
	int ch = ci_scq();
	if (ch == 0x4800 || ch == 0x4b00 || ch == 0x4d00 || ch == 0x5000)
		return (unsigned int)ch;
	return (unsigned int)(ch & 0xff);
}

void convertq(void)
{
}

void PSformatq(void)
{
}

void read_configq(char *name, int mode)
{
	(void)name;
	(void)mode;
}

void zcursonq(void)
{
}

char * TYPE itoa(int value, char *str, int radix)
{
	if (str == NULL)
		return NULL;
	if (radix == 10)
		snprintf(str, 32, "%d", value);
	else if (radix == 16)
		snprintf(str, 32, "%x", value);
	else if (radix == 8)
		snprintf(str, 32, "%o", value);
	else
		snprintf(str, 32, "%d", value);
	return str;
}

#ifdef strupr
#undef strupr
#endif
char * TYPE strupr(char *str)
{
	char *p = str;
	if (!str) return NULL;
	while (*p) {
		*p = (char)toupper((unsigned char)*p);
		p++;
	}
	return str;
}

#ifdef strlwr
#undef strlwr
#endif
char * TYPE strlwr(char *str)
{
	char *p = str;
	if (!str) return NULL;
	while (*p) {
		*p = (char)tolower((unsigned char)*p);
		p++;
	}
	return str;
}

/* Portable implementations for crosshair cursor and digitizer routines */
void TYPE curson(int row, int col, float **fx, float **fy, int *find)
{
	(void)row; (void)col; (void)fx; (void)fy;
	if (find) *find = 0;
}

void TYPE curvefollow(int row, int col, float *x, float *y, int npts)
{
	(void)row; (void)col; (void)x; (void)y; (void)npts;
}

int TYPE GetDataPoint(float *x, float *y)
{
	if (x) *x = 0.0f;
	if (y) *y = 0.0f;
	return 0;
}


