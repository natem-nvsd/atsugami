/********************************************************
*	Atsugami: Command-line image manager		*
*							*
*	Author:	Nate Morrison.				*
*							*
*	Purpose: Taggable command line image manager	*
*	with build-in commands.				*
*							*
*	Usage:						*
********************************************************/
#include <fcntl.h>
#include "functions.h"
#include <libpq-fe.h>
#include <malloc_np.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
	char conninfo[] = "dbname=atsugami"; /* Sets the database for dbconnect() */ /*------------> DO NOT DELETE!!! <------------*/
	PGconn *conn = NULL;
//	PGresult *res;

/*	PGnotify *notify;
	int nnotifies;
	int i, j; */

	/* Commands */
	int ls			= 0;
	int lstag		= 0;
	int find		= 0;
	int about		= 0;
	int help		= 0;
	int ver			= 0;
	int wiki		= 0;
	int rm			= 0;
	int del			= 0;
	int rmwiki		= 0;
	int rmtag		= 0;
	int import		= 0;
	int mkwiki		= 0;
	int mktag		= 0;
	int edit		= 0;
	int editwiki		= 0;
	int edittag		= 0;
	int view		= 0;
	int favs		= 0;
	int fav			= 0;
	int unfav		= 0;

	printf("argc = %d\n", argc);
	/* Check if enough arguments have been provided */
	if (argv[1] == NULL) {
		fprintf(stderr, "%s: Atsugami requires at least one argument. See '%s help' for more details.\n", argv[0], argv[0]);
		exit(1);
	}
	
	if	(strcmp(argv[1], "ls")		== 0) ls++;
	else if	(strcmp(argv[1], "lstag")	== 0) lstag++;
	else if	(strcmp(argv[1], "find")	== 0) find++;
	else if (strcmp(argv[1], "about")	== 0) about++;
	else if (strcmp(argv[1], "help")	== 0) help++;
	else if (strcmp(argv[1], "version")	== 0) ver++;
	else if (strcmp(argv[1], "wiki")	== 0) wiki++;
	else if (strcmp(argv[1], "rm")		== 0) rm++;
	else if (strcmp(argv[1], "delete")	== 0) del++;
	else if (strcmp(argv[1], "rmwiki")	== 0) rmwiki++;
	else if (strcmp(argv[1], "rmtag")	== 0) rmtag++;
	else if (strcmp(argv[1], "import")	== 0) import++;
	else if (strcmp(argv[1], "mkwiki")	== 0) mkwiki++;
	else if (strcmp(argv[1], "mktag")	== 0) mktag++;
	else if (strcmp(argv[1], "edit")	== 0) edit++;
	else if (strcmp(argv[1], "editwiki")	== 0) editwiki++;
	else if (strcmp(argv[1], "edittag")	== 0) edittag++;
	else if (strcmp(argv[1], "view")	== 0) view++;
	else if (strcmp(argv[1], "favourites")	== 0) favs++;
	else if (strcmp(argv[1], "favorites")	== 0) favs++;
	else if (strcmp(argv[1], "favourite")	== 0) fav++;
	else if (strcmp(argv[1], "favorite")	== 0) fav++;
	else if (strcmp(argv[1], "unfavourite")	== 0) unfav++;
	else if (strcmp(argv[1], "unfavorite")	== 0) unfav++;
	else {
		fprintf(stderr, "%s: %s: Invalid command. See '%s help' for more details.\n", argv[0], argv[1], argv[0]);
		return(1);
	}

	/* Commands */
	while (ls == 1) {
		while (argc > 2) {
			fprintf(stderr, "%s: %s: Too many arguments.\n", argv[0], argv[2]);
			return(1);
		}
		dbconnect(conninfo, &conn);
		query(PQexec(conn, "SELECT uuid, artist, copyrights, characters, tags FROM public.files;"), conninfo, &conn);
		return(0);
	}

	while (lstag == 1) {
		dbconnect(conninfo, &conn);
		query(PQexec(conn, "SELECT name FROM public.tags;"), conninfo, &conn);
		return(0);
	}

	while (find == 1) {
		dbconnect(conninfo, &conn);

		const char *search_query = argv[3];
		printf("%s\n", search_query); /* Causes a seg fault */

		return(0);
	}

	while (about == 1) {
		AGabout();
		return(0);
	}
	while (help == 1) {
		AGhelp();
	}

	while (ver == 1) {
		AGversion(0.2);
		return(0);
	}

	if (wiki) {
		fprintf(stderr, "This command has not been implemented.\n");
		return(1);
	}
	
	while (rm == 1) {
/*		dbconnect(conninfo, &conn);
		malloc(sizeof(argv[3]));
		char rmfile = argv[3]; 
		free(*rmfile); */

		fprintf(stderr, "This command has not been implemented.\n");
		return(1);
	}
	while (del == 1) {
		fprintf(stderr, "This command has not been implemented.\n");
		return(1);
	}
	
	while (rmwiki == 1) {
		fprintf(stderr, "This command has not been implemented.\n");
		return(1);
	}

	while (rmtag == 1) {
		fprintf(stderr, "This command has not been implemented.\n");
		return(1);
	}
	while (import == 1) {
		fprintf(stderr, "This command has not been implemented.\n");
		return(1);
/*		if (argc < 3) {
			fprintf(stderr, "%s: import: No arguments provided.\n", argv[0]);
			exit_nicely(conn);
		}
		dbconnect(conninfo, &conn); */

		/* allocate memory for the path to the file to import (infilename) */
/*		malloc(sizeof(argv[3]));
		char infilename;
		strcat(infilename, argv[3]);
//		[] = argv[3];
//		infilename = argv[3];
		printf("%s\n", infilename);
		free(infilename); */

		/* Allocate memory of the file to open (infile) */
/*		FILE *infile = infilename;
		malloc(sizeof(infile));
		open(infile, 0000);
		return(0); */
	}

	while (mkwiki == 1) {
		fprintf(stderr, "This command has not been implemented.\n");
		return(1);
	}

	while (mktag == 1) {
		fprintf(stderr, "This command has not been implemented.\n");
		return(1);
	}

	while (edit == 1) {
		fprintf(stderr, "This command has not been implemented.\n");
		return(1);
	}

	while (editwiki == 1) {
		fprintf(stderr, "This command has not been implemented.\n");
		return(1);
	}

	while (edittag == 1) {
		fprintf(stderr, "This command has not been implemented.\n");
		return(1);
	}

	while (view == 1) {
		fprintf(stderr, "This command has not been implemented.\n");
		return(1);
	}

	while (favs == 1) {
		fprintf(stderr, "This command has not been implemented.\n");
		return(1);
	}

	while (fav == 1) {
		fprintf(stderr, "This command has not been implemented.\n");
		return(1);
	}

	while (unfav == 1) {
		fprintf(stderr, "This command has not been implemented.\n");
		return(1);
	}

	return(0);
}
