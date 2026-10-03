/********************************************************************
    FILE: memory_portable.c
    (c) 1984-2026 Scientific Endeavors Corporation / Modernized Port
    PURPOSE: Portable 64-bit memory management for GraphiC.
             Replaces 16-bit Windows GlobalAlloc/GlobalLock calls with
             standard C library memory allocation (calloc, free, realloc)
             while preserving full API compatibility and optional TESTMEM
             guard-byte / leak detection debugging.
********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <graphic.h>

#ifdef TESTMEM
struct ELEM {
	void *dataptr;
	char file[64];
	int line_num;
	size_t amount;
	unsigned long test_pattern;
	struct ELEM *next;
};
static struct ELEM *top = NULL;
#endif

void GCLASS gfreeallq(void)
{
#ifdef TESTMEM
	struct ELEM *elem = top;
	while (elem != NULL) {
		struct ELEM *next = elem->next;
		free(elem);
		elem = next;
	}
	top = NULL;
#endif
}

int GCLASS gchkmemq(void)
{
#ifdef TESTMEM
	return (top != NULL) ? 1 : 0;
#else
	return 0;
#endif
}

int GCLASS printer_memq(int nbuf, int row, int col)
{
	int i, j;
	int scratch = 0;

	if ((row * col) == 0)
		return 0;

	if (row == -1) {
		scratch = 1;
		row = 1;
	}

	for (i = 0; i < nbuf; i++) {
		g_clsq.tkb[i] = (unsigned char **)gpcalloc(row, sizeof(unsigned char *));
		if (g_clsq.tkb[i] == NULL) {
			bfreeq(scratch ? -row : row, i);
			return 0;
		}
		for (j = 0; j < row; j++) {
			g_clsq.tkb[i][j] = (unsigned char *)gpcalloc(col, sizeof(unsigned char));
			if (g_clsq.tkb[i][j] == NULL) {
				bfreeq(scratch ? -row : row, i + 1);
				return 0;
			}
		}
	}
	return 1;
}

void GCLASS bfreeq(int row, int nbuf)
{
	int i, j;
	G_PROCESS_ID

	if (nbuf <= 0)
		return;
	if (row == -1)
		row = 1;

	for (i = 0; i < nbuf; i++) {
		if (g_clsq.tkb[i] != NULL) {
			for (j = 0; j < row; j++) {
				gpcfree((void **)&g_clsq.tkb[i][j]);
			}
			gpcfree((void **)&g_clsq.tkb[i]);
		}
	}
	g_prntq.nbuff = 0;
}

void ** TYPE GCLASS dim2(int row, int col, unsigned int size)
{
	int i;
	void **prow = (void **)gpcalloc(row + 1, sizeof(void *));
	if (prow == NULL)
		return NULL;

	for (i = 0; i < row; i++) {
		prow[i] = (void *)gpcalloc(col, size);
		if (prow[i] == NULL) {
			while (i > 0) {
				i--;
				gpcfree((void **)&prow[i]);
			}
			gpcfree((void **)&prow);
			return NULL;
		}
	}
	prow[row] = NULL; /* Flag end of array for free2 */
	return prow;
}

void ** TYPE GCLASS free2(void ***pa)
{
	int i = 0;
	if (pa == NULL || *pa == NULL)
		return NULL;

	while ((*pa)[i] != NULL) {
		gpcfree((void **)&(*pa)[i]);
		i++;
	}
	gpcfree((void **)pa);
	*pa = NULL;
	return NULL;
}

#ifdef TESTMEM
void * TYPE GCLASS gpcallocq(size_t nobj, size_t size, char *file, int line_num)
{
	size_t nbytes = nobj * size;
	size_t total = sizeof(struct ELEM) + nbytes + 1; /* +1 guard byte */
	unsigned char *raw = (unsigned char *)calloc(1, total);
	struct ELEM *elem;
	unsigned char *user_data;

	if (raw == NULL)
		return NULL;

	elem = (struct ELEM *)raw;
	user_data = raw + sizeof(struct ELEM);
	elem->dataptr = user_data;

	if (file != NULL) {
		const char *base = strrchr(file, "/");
		if (!base) base = strrchr(file, "\\");
		strncpy(elem->file, base ? base + 1 : file, sizeof(elem->file) - 1);
		elem->file[sizeof(elem->file) - 1] = "\0";
	} else {
		strcpy(elem->file, "unknown");
	}

	elem->line_num = line_num;
	elem->amount = nbytes;
	elem->test_pattern = 0x7f7f7f7fUL;
	*(user_data + nbytes) = 0x7f; /* Guard byte at the end */

	elem->next = top;
	top = elem;

	return (void *)user_data;
}

void TYPE GCLASS gpcfreeq(void **ptr, char *file, int line_num)
{
	struct ELEM *prev = NULL, *curr;
	unsigned char *user_data;

	if (ptr == NULL || *ptr == NULL)
		return;

	curr = top;
	while (curr != NULL && curr->dataptr != *ptr) {
		prev = curr;
		curr = curr->next;
	}

	if (curr != NULL) {
		user_data = (unsigned char *)curr->dataptr;
		if (*(user_data + curr->amount) != 0x7f) {
			fprintf(stderr, "[GraphiC] MEMORY CORRUPTION: Guard byte trashed for block allocated in %s:%d, freed at %s:%d\n",
				curr->file, curr->line_num, file ? file : "unknown", line_num);
		}
		if (prev != NULL)
			prev->next = curr->next;
		else
			top = curr->next;
		free(curr);
	} else {
		free(*ptr);
	}
	*ptr = NULL;
}

void * TYPE GCLASS gpcreallocq(void *mem_ptr, size_t size, char *file, int line_num)
{
	void *new_ptr = gpcallocq(1, size, file, line_num);
	struct ELEM *curr;
	size_t old_size;

	if (new_ptr == NULL)
		return NULL;

	if (mem_ptr != NULL) {
		curr = top;
		while (curr != NULL && curr->dataptr != mem_ptr)
			curr = curr->next;
		old_size = (curr != NULL && curr->amount < size) ? curr->amount : size;
		memcpy(new_ptr, mem_ptr, old_size);
		gpcfreeq(&mem_ptr, file, line_num);
	}
	return new_ptr;
}

#else

void * TYPE GCLASS gpcalloc(size_t nobj, size_t size)
{
	return calloc(nobj ? nobj : 1, size ? size : 1);
}

void TYPE GCLASS gpcfree(void **ptr)
{
	if (ptr != NULL && *ptr != NULL) {
		free(*ptr);
		*ptr = NULL;
	}
}

void * TYPE GCLASS gpcrealloc(void *mem_ptr, size_t size)
{
	if (mem_ptr == NULL)
		return gpcalloc(1, size);
	return realloc(mem_ptr, size ? size : 1);
}
#endif

long TYPE GCLASS HowMuchMem(void)
{
	/* Modern 64-bit systems have ample address space */
	return 1024L * 1024L * 1024L; /* 1 GB */
}

/**************************************************************/
/* Panel line structures are full -- assign new ones.         */
/**************************************************************/
int GCLASS newbufsq(struct GXYPAIRSQ G_FAR * G_FAR *xy, int index)
{
	if (index % 64 == 63) {
		*xy = (struct GXYPAIRSQ G_FAR *)gpcrealloc((void *) *xy, (index + 65) *
			sizeof(struct GXYPAIRSQ));
		if (*xy == NULL)
			messageq(FATALQ, 9);			/* Out of Memory */
	}
	return(++index);
}

/**************************************************************/
/* Vector buffers are full -- assign new ones.                */
/* size is the number of bytes in the object to be allocated  */
/**************************************************************/
int GCLASS newvbufsq(void G_FAR * G_FAR *x,
	void G_FAR * G_FAR *y, int index, int size)
{
	if (index % 64 == 63) {
		*x = (void G_FAR *)gpcrealloc((void *) * x, (index + 65) * (size_t)size);
		*y = (void G_FAR *)gpcrealloc((void *) * y, (index + 65) * (size_t)size);
		if (*x == NULL || *y == NULL) {
			gpcfree((void **) x);
			gpcfree((void **) y);
			messageq(FATALQ, 9);			/* Out of Memory */
		}
	}
	return(++index);
}

/**************************************************************/
/* Vector buffer of any type is full -- assign new one.       */
/* size is the number of bytes in the object to be allocated  */
/**************************************************************/
int GCLASS newabufsq(void **x, int index, int size)
{
	if (index % 64 == 63) {
		*x = (void *)gpcrealloc((void *) * x, (index + 65) * (size_t)size);
		if (*x == NULL) {
			gpcfree((void **) x);
			messageq(FATALQ, 9);			/* Out of Memory */
		}
	}
	return(++index);
}

/********************************************************************
    FUNCTION NAME: strdupq
    ARGUMENTS: string - string to be duplicated
    PURPOSE: Copy a NULL-terminated string into a new array. This function
		allocates the new array. The calling function must free the space
		when it is no longer needed.
********************************************************************/
char * GCLASS strdupq(char *string)
{
	char *dup_string;
	size_t len;

	if (string == NULL)
		return(NULL);
	len = strlen(string) + 1;
	dup_string = (char *)gpcalloc(len, sizeof(char));
	if (dup_string != NULL)
		strcpy(dup_string, string);
	return(dup_string);
}

