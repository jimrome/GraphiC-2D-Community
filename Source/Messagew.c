/********************************************************************
	(c) 1994 by Scientific Endeavors Corporation.
    			All rights reserved.
    	Windows version developed by BIT, Inc.

    	Version 7.0 10/06/93
	This file contains routines for managing messages.

        Modified for Windows version of GraphiC.  LB
********************************************************************/

/************************ includes/defines *************************/
#include <graphic.h>
#define ERRFILE "graphic.err"
#define MESGFILE "messages.dat"
#define TABFILE "messages.tab"

/************************ variable definitions *********************/
#ifndef BOOL
typedef int BOOL;
#endif
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
static FILE *TempFilePtr;
static char *szAppMess = "GraphiC/Win Message";
static char errflgq = 0;
static BOOL MessageFound;

/************************ function declarations ********************/
char *local_messages[] = {
"\nError opening " MESGFILE " or " TABFILE "\n\
unable to generate error message",
"\nFailure of environment search,\n\
could not open " MESGFILE " or " TABFILE "\n\
unable to generate error message"
};

void BuildMessageq(char mesgtype, int mesgnum, char *filename, int linno);

#if MSQ
#pragma optimize("", off)
#endif
/********************************************************************
    FUNCTION NAME: gmessageq
    ARGUMENTS: mesgtype -- type of message to be printed
                           == 'W' for warning (level 1)
                           == 'E' for errors (level 2)
                           == 'F' for fatal errors (level 3)
                           == 'M' for messages (level 0)
                           == 't' for TrueType warnings 
                           == 'T' for TrueType errors
                           == 'p' for Postscript warnings
                           == 'P' for Postscript errors
                           == 'N' for new messages (the message
                                  number is not yet assigned)
               mesgnum -- message number for the message to be printed
               filename -- file from which the error was generated
               linno -- line on which the error was generated
    PURPOSE: Put error messages into the error file and
             print the messages on the screen

3-3-93 Tim
Added the variable MessageFound so that we can display the line no and
file name of a call to messageq() with a message that could not be found.
********************************************************************/
void TYPE gmessageq(char mesgtype, int mesgnum, char *filename, int linno)
{
	int ch, play = g_iq[dev_typeq].in_play;
	int size;
	char *MesgText = NULL;
	FILE *errfileq = NULL;

	if (in_gmessageq)                           /* Prevent recursive calls */
		return;
	MessageFound = TRUE;
	in_gmessageq = 1;
	TempFilePtr = NULL;
	TempFilePtr = gpc_tmpfile();
	if (TempFilePtr != NULL)
		BuildMessageq(mesgtype, mesgnum, filename, linno);
	if ((mesgtype != MESGQ || !MessageFound) && !play) { /* Open error file */
		if (errflgq)
			errfileq = gfopen(ERRFILE, "at", 0);
		else
			errfileq = gfopen(ERRFILE, "wt", 0);
		if (errfileq != NULL) {
			errflgq = 1;
			if (TempFilePtr != NULL) {
				fseek(TempFilePtr, 0L, SEEK_SET);
				while ((ch = fgetc(TempFilePtr)) != EOF)
					fputc(ch, errfileq);
			}
			else {
				fprintf(errfileq, "\nMessage %c%03d was generated in file %s (line %d)",
					mesgtype, mesgnum, filename, linno);
				fprintf(errfileq, "and the message buffer could not be created.\n");
			}
			gfclose(errfileq);
		}
	}
	if (mesgtype == 'P' || mesgtype == 'T')
		mesgtype = ERRQ;
	if (mesgtype == 't' || mesgtype == 't')
		mesgtype = WARNQ;
	if (mesgtype != WARNQ) {
		if (TempFilePtr != NULL) {
			fseek(TempFilePtr, 0L, SEEK_END);
			size = (int)ftell(TempFilePtr);
			fseek(TempFilePtr, 0L, SEEK_SET);
			MesgText = (char *)gpcalloc(size + 1, sizeof(char));
			if (MesgText != NULL) {
				for (ch = 0; ch < size; ch++)
					MesgText[ch] = (char)fgetc(TempFilePtr);
				MesgText[size] = '\0';
#if GPC_WIN
				MessageBox(GetFocus(), (LPSTR)MesgText, szAppMess,
					MB_ICONINFORMATION | MB_OK);
#else
				fprintf(stderr, "[GraphiC] %s: %s\n", szAppMess, MesgText);
#endif
			}
		}
		else {
			MesgText = (char *)gpcalloc(128, sizeof(char));
			if (MesgText != NULL) {
				sprintfq(MesgText, "Message %c%03d was generated in file %s (line %d) ",
					mesgtype, mesgnum, filename, linno);
				strcat(MesgText, "and the message buffer could not be created.");
#if GPC_WIN
				MessageBox(GetFocus(), (LPSTR)MesgText, szAppMess,
					MB_ICONINFORMATION | MB_OK);
#else
				fprintf(stderr, "[GraphiC] %s: %s\n", szAppMess, MesgText);
#endif
			}
		}
		if (MesgText == NULL) {
			MesgText = "A message was generated and the message buffer could not be created.";
#if GPC_WIN
			MessageBox(GetFocus(), (LPSTR)MesgText, szAppMess,
				MB_ICONINFORMATION | MB_OK);
#else
			fprintf(stderr, "[GraphiC] %s: %s\n", szAppMess, MesgText);
#endif
		}
		else
			gpcfree((void **)&MesgText);
	}
	if (mesgptrq != NULL)
		mesgptrq = NULL;
	if (TempFilePtr != NULL)
		fclose(TempFilePtr);
	if ((mesgtype == FATALQ) || (mesgtype == ERRQ)) {
#if GPC_WIN
		if (mesgtype == FATALQ) {                   /* Exit if fatal error */
			MesgText = "Fatal Error: GraphiC/Win will exit.";
			MessageBox(GetFocus(), (LPSTR)MesgText, szAppMess,
				MB_ICONSTOP | MB_OK);
			ch = IDCANCEL;
		}
		else {                                /* Continue or exit if error */
			MesgText = "Error: Choose OK to continue or CANCEL to exit.";
			ch = MessageBox(GetFocus(), (LPSTR)MesgText, szAppMess,
				MB_ICONSTOP | MB_OKCANCEL);
		}
		if (ch == IDCANCEL){ /* Exit program */
			gfreeallq();
			if (tkf_pointerq)
				gfclose(tkf_pointerq);
			DestroyWindow(g_iq[VIDEO].hWnd);	/* Get the hell out of Dodge */
#ifndef MAKEDLL
			exit(0);
#endif
		}
#else
		if (mesgtype == FATALQ) {
			fprintf(stderr, "[GraphiC] Fatal Error: exiting.\n");
			gfreeallq();
			if (tkf_pointerq)
				gfclose(tkf_pointerq);
			exit(1);
		}
#endif
		plotadd();
	}
	in_gmessageq = 0;
}

/**************************************************************************/
/* Build message for gmessageq()                                          */
void BuildMessageq(char mesgtype, int mesgnum, char *filename, int linno)
{
	char type, level;
	FILE *mesgfile = NULL, *tabfile = NULL;
	int i, number, offset, size;
	char seps[]   = " ,\t\n";
	char *token;
    char cbuff[80];



	if (mesgptrq != NULL && mesgposq)
		fprintf(TempFilePtr, "\n%s", mesgptrq);
	if (mesgtype == NEWQ) {                                 /* New message */
		fprintf(TempFilePtr, "\nNew Message in file %s (line %d):",
			filename, linno);
		fprintf(TempFilePtr, "\nPlease report this message to ");
		fprintf(TempFilePtr, "Scientific Endeavors Corp\n");
	}
	else if ((mesgtype == WARNQ) && (mesgnum == 17)) {   /* GPC path error */
		fprintf(TempFilePtr, "\nWarning 017 in file %s (line %d):",
			filename, linno);
		fprintf(TempFilePtr, "\nFailure of environment search for ");
		fprintf(TempFilePtr, GRAPHIC_ENV);
		fprintf(TempFilePtr, "=<path>\n");
	}
	else {                                        /* Get message from file */
		if (mesgtype != MESGQ) {
			if (mesgtype == 'P' || mesgtype == 'T')
				level = ERRQ;
			if (mesgtype == 't' || mesgtype == 't')
				level = WARNQ;
			switch (level) {                        /* Get message type */
				case WARNQ:
					fprintf(TempFilePtr, "\nWarning ");
					break;
				case ERRQ:
					fprintf(TempFilePtr, "\nError ");
					break;
				case FATALQ:
					fprintf(TempFilePtr, "\nFatal Error ");
					break;
			}
			fprintf(TempFilePtr, "%03d in file %s (line %d):",
				mesgnum, filename, linno);
		}
		mesgfile = gfopen(MESGFILE, "rt", 0);        /* Open message files */
		tabfile = gfopen(TABFILE, "rt", 0);
		if (mesgfile == NULL || tabfile == NULL){
			fprintf(TempFilePtr, "\nNote: Unable to open %s or %s.\n", MESGFILE, TABFILE);
			if (mesgnum == 39) {
				fprintf(TempFilePtr, "GraphiC was unable to load at least one of the requested fonts.\n"
				                     "Please verify that font files (*.fnt, *.ttf) are accessible\n"
				                     "in the current directory, parent directories, or GRAPHIC path.\n");
			} else {
				fprintf(TempFilePtr, "(Message %c%03d in file %s, line %d)\n",
					mesgtype, mesgnum, filename, linno);
			}
		}
		else {
			while (!feof(tabfile)) {                 /* Search for message */
			
				fgets(cbuff, 80, tabfile);
				
				/* Use strtok to parse string */
   				token = strtok(cbuff, seps);
				type = cbuff[0];
				
   				token = strtok(NULL, seps);
   				number = atoi(token);
   				
   				token = strtok(NULL, seps);
   				offset = atoi(token);
   				
   				token = strtok(NULL, seps);
   				size = atoi(token);
				/*fscanf(tabfile, "%c %d %d %d\n", &type, &number, &offset,
					&size);*/
				if ((type == mesgtype) && (number == mesgnum))
					break;
			}
			gfclose(tabfile);
			if ((type != mesgtype) || (number != mesgnum)) {  /* Not found */
				MessageFound = FALSE;
				if (mesgtype == MESGQ) {
					fprintf(TempFilePtr,
						"\nMessage %03d in file %s (line %d):",
						mesgnum, filename, linno);
				}
				fprintf(TempFilePtr, "\nMessage not found in message file.");
				fprintf(TempFilePtr, "\nPlease report this message to ");
				fprintf(TempFilePtr, "Scientific Endeavors Corp.\n");
			}
			else {                                         /* Read message */
				fseek(mesgfile, (long)offset, SEEK_SET);
				for (i = 0; i < size; i++)
					fputc(fgetc(mesgfile), TempFilePtr);
			}
			gfclose(mesgfile);
		}
	}
	if (mesgptrq != NULL && !mesgposq)
		fprintf(TempFilePtr, "%s\n", mesgptrq);
}
#if MSQ
#pragma optimize("", on)
#endif
/********************************************************************
    FUNCTION NAME: gprintq
    ARGUMENTS: string -- string to be printed
               position -- position of string relative to message
                   1 == string printed before message
                   0 == string printed after message
    PURPOSE: Build a string to be printed on the next
		call to messageq().
********************************************************************/
void gprintq(char *string, char position)
{
	G_PROCESS_ID 
	
	if (string == NULL){
		mesgptrq = NULL;
		return;
	}
	if (*string == '\0'){
		mesgptrq = NULL;
		return;
	}
	mesgptrq = string;
	if (position)
		mesgposq = 1;
	else
		mesgposq = 0;
}

/***********************************************************/
/* Allows the user to re-enter a plot and make additions. */
void TYPE plotadd(void)
{
	int noteksav = notekq;
	G_PROCESS_ID

	if (notekq)
		return;
	adding_flgq = 1;
	g_zq.zoomit = 2;
	notekq = 1;
	if (g_scq.gmode != 'g')
		scrnpltq();
	if (!in_gmessageq) {
		tkf_incrementq = tkf_inc_saveq - tkf_offsetq;
		fseek(tkf_pointerq, tkf_inc_saveq, SEEK_SET);
		tkf_inc_saveq = 0;
	}
	g_zq.zoomit = 0;
	has_beepedq = 0;
	notekq = noteksav;
	adding_flgq = 0;
}
