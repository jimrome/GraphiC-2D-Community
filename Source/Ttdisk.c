/* (c) 1984-1993 by Scientific Endeavors Corporation.
   All rights reserved.
Version 7.0 PostScript/Truetype extensions written by David Dillow.
**************************************************************/
#include <graphic.h>
#include <truetype.h>

#ifdef MULTITHREAD
struct TTDISK_DATA {
BYTE *tableq;
TTLONG positionq, tablelengthq;
} *disk_dataq = NULL;
#define dd_data disk_dataq[instctr]
#define tableq dd_data.tableq
#define positionq dd_data.positionq
#define tablelengthq dd_data.tablelengthq
#else
static BYTE *tableq;
static TTLONG positionq, tablelengthq;
#endif

BYTE Get8Bits(FILE *file)
{
	int c = fgetc(file);
	return (c == EOF) ? 0 : (BYTE)c;
}

USHORT Get16Bits(FILE *file)
{
	unsigned char b[2];
	if (fread(b, 1, 2, file) != 2)
		return 0;
	return (USHORT)((((USHORT)b[0]) << 8) | (USHORT)b[1]);
}

ULONG Get32Bits(FILE *file)
{
	unsigned char b[4];
	if (fread(b, 1, 4, file) != 4)
		return 0;
	return (ULONG)((((ULONG)b[0]) << 24) | (((ULONG)b[1]) << 16) | (((ULONG)b[2]) << 8) | (ULONG)b[3]);
}

int LoadTable(FILE *file, ULONG offset, ULONG length)
{
#ifdef MULTITHREAD
	G_PROCESS_ID

	disk_dataq = (struct TTDISK_DATA *)gpcrealloc(disk_dataq,
		(instctr + 1) * sizeof(struct TTDISK_DATA));
#endif

	if (fseek(file, offset, SEEK_SET) != 0)
		return FALSE;

	tableq = (BYTE *)gpcalloc((size_t)length + 4, 1);
	if (tableq == NULL)
		return FALSE;

	if (fread(tableq, 1, length, file) != length)
	{
		gpcfree((void **) &tableq);
		tableq = NULL;
		return FALSE;
	}

	positionq = 0;
	tablelengthq = length;

    return TRUE;
}

void FinishTable(void)
{
	G_PROCESS_ID

	if (tableq != NULL) {
		gpcfree((void **) &tableq);
		tableq = NULL;
	}
}

void Tableseek(TTLONG offset, USHORT direction)
{
	G_PROCESS_ID

	switch(direction)
	{
		case 0:
			positionq = offset;
			break;
		case 1:
			positionq += offset;
			break;
		case 2:
			positionq = tablelengthq - offset - 1;
			break;
	}
}

BYTE Get8Table(void)
{
	G_PROCESS_ID

	if (positionq >= tablelengthq) return 0;
	return tableq[(unsigned int) positionq++];
}

USHORT Get16Table(void)
{
	BYTE b0, b1;
	G_PROCESS_ID

	if (positionq + 1 >= tablelengthq) return 0;
	b0 = tableq[(unsigned int) positionq++];
	b1 = tableq[(unsigned int) positionq++];

	return (USHORT)((((USHORT)b0) << 8) | (USHORT)b1);
}

ULONG Get32Table(void)
{
	BYTE b0, b1, b2, b3;
	G_PROCESS_ID

	if (positionq + 3 >= tablelengthq) return 0;
	b0 = tableq[(unsigned int) positionq++];
	b1 = tableq[(unsigned int) positionq++];
	b2 = tableq[(unsigned int) positionq++];
	b3 = tableq[(unsigned int) positionq++];

	return (ULONG)((((ULONG)b0) << 24) | (((ULONG)b1) << 16) | (((ULONG)b2) << 8) | (ULONG)b3);
}

ULONG TableTell(void)
{
	G_PROCESS_ID

	return positionq;
}

void SetTable(BYTE *newtable)
{
	G_PROCESS_ID

	tableq = newtable;

	positionq = 0;
	tablelengthq = 0x7FFFFFFF;
}
