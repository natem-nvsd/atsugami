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
#include <errno.h>
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
	/* There are too many commands. Merge them into fewer commands, just with GNU-style flags (e.g. ls --wiki) */
	/* Listing */
	int ls			= 0;
	int lstag		= 0;
	int lsart		= 0;
	int lschar		= 0;
	int lscopy		= 0;
	int find		= 0;

	/* Program meta */
	int about		= 0;
	int help		= 0;
	int ver			= 0;
	int wiki		= 0;

	/* Deleting */
	int rm			= 0;
	int rmtag		= 0;
	int rmart		= 0;
	int rmchar		= 0;
	int rmcopy		= 0;
	int rmwiki		= 0;
	int del			= 0;

	/* Creating */
	int import		= 0;
	int mktag		= 0;
	int mkart		= 0;
	int mkchar		= 0;
	int mkcopy		= 0;
	int mkwiki		= 0;

	/* Editing */
	int ed			= 0;
	int edtag		= 0;
	int edart		= 0;
	int edchar		= 0;
	int edcopy		= 0;
	int edwiki		= 0;

	/* File meta */
	int view		= 0;
	int favs		= 0;
	int fav			= 0;
	int unfav		= 0;

	/* Check if enough arguments have been provided */
	if (argv[1] == NULL) {
		fprintf(stderr, "%s: Atsugami requires at least one argument. See '%s help' for more details.\n", argv[0], argv[0]);
		exit(1);
	}

	if	(strcmp(argv[1], "ls")		== 0) ls++;
	else if	(strcmp(argv[1], "lstag")	== 0) lstag++;
	else if (strcmp(argv[1], "lsartist")	== 0) lsart++;
	else if (strcmp(argv[1], "lscharacter") == 0) lschar++;
	else if (strcmp(argv[1], "lscopyright") == 0) lscopy++;
	else if	(strcmp(argv[1], "find")	== 0) find++;

	else if (strcmp(argv[1], "about")	== 0) about++;
	else if (strcmp(argv[1], "help")	== 0) help++;
	else if (strcmp(argv[1], "version")	== 0) ver++;
	else if (strcmp(argv[1], "wiki")	== 0) wiki++;

	else if (strcmp(argv[1], "rm")		== 0) rm++;
	else if (strcmp(argv[1], "delete")	== 0) del++;
	else if (strcmp(argv[1], "rmtag")	== 0) rmtag++;
	else if (strcmp(argv[1], "rmartist")	== 0) rmart++;
	else if (strcmp(argv[1], "rmcharacter")	== 0) rmchar++;
	else if (strcmp(argv[1], "rmcopyright")	== 0) rmcopy++;
	else if (strcmp(argv[1], "rmwiki")	== 0) rmwiki++;

	else if (strcmp(argv[1], "import")	== 0) import++;
	else if (strcmp(argv[1], "mktag")	== 0) mktag++;
	else if (strcmp(argv[1], "mkartist")	== 0) mkart++;
	else if (strcmp(argv[1], "mkcharacter")	== 0) mkchar++;
	else if (strcmp(argv[1], "mkcopyright")	== 0) mkcopy++;
	else if (strcmp(argv[1], "mkwiki")	== 0) mkwiki++;

	else if (strcmp(argv[1], "edit")	== 0) ed++;
	else if (strcmp(argv[1], "edittag")	== 0) edtag++;
	else if (strcmp(argv[1], "editartist")	== 0) edart++;
	else if (strcmp(argv[1], "editcharacter") == 0) edchar++;
	else if (strcmp(argv[1], "editcopyright") == 0) edcopy++;
	else if (strcmp(argv[1], "editwiki")	== 0) edwiki++;

	else if (strcmp(argv[1], "view")	== 0) view++;
	else if (strcmp(argv[1], "favourites")	== 0) favs++;
	else if (strcmp(argv[1], "favorites")	== 0) favs++;
	else if (strcmp(argv[1], "favourite")	== 0) fav++;
	else if (strcmp(argv[1], "favorite")	== 0) fav++;
	else if (strcmp(argv[1], "unfavourite")	== 0) unfav++;
	else if (strcmp(argv[1], "unfavorite")	== 0) unfav++;
	else {
		fprintf(stderr, "%s: %s: EINVAL: Invalid command. See '%s help' for more details.\n", argv[0], argv[1], argv[0]);
		return(EINVAL);
	}

	/* Commands */
	if (ls) {
		while (argc > 2) {
			fprintf(stderr, "%s: %s: E2BIG: Too many arguments.\n", argv[0], argv[1]);
			return(E2BIG);
		}
		dbconnect(conninfo, &conn);
		query(PQexec(conn, "SELECT uuid, artist, copyrights, characters, tags FROM public.files;"), conninfo, &conn);
		return(0);
	}

	if (lstag) {
		dbconnect(conninfo, &conn);
		query(PQexec(conn, "SELECT name FROM public.tags;"), conninfo, &conn);
		PQfinish(conn);
		return(0);
	}

	if (lsart) {
		dbconnect(conninfo, &conn);
		query(PQexec(conn, "SELECT name FROM public.artists;"), conninfo, &conn);
		PQfinish(conn);
		return(0);
	}

	if (lschar) {
		dbconnect(conninfo, &conn);
		query(PQexec(conn, "SELECT name FROM public.characters;"), conninfo, &conn);
		PQfinish(conn);
		return(0);
	}

	if (lscopy) {
		dbconnect(conninfo, &conn);
		query(PQexec(conn, "SELECT name FROM public.copyrights;"), conninfo, &conn);
		return(0);
	}

	if (find) {
		while (argv == NULL) {
			fprintf(stderr, "%s: %s: One argument required.\n", argv[0], argv[1]);
			return(1);
		}
		while (argc > 3) {
			fprintf(stderr, "%s: %s: Too many arguments.\n", argv[0], argv[1]);
			return(7);
		}

		/***********************************************\
		*************************************************
		!!!!!! THIS CODE IS PRONE TO SQL INJECTION !!!!!!
		*************************************************
		\***********************************************/
		const int LEN = sizeof(argv[2]);
		const char *paramValues[1];

		int rowId;
		int ret = sscanf(argv[2], "%d", &rowId);

		char str[LEN];
		snprintf(str, LEN, "%d", rowId);
		paramValues[0] = str;

		/* Connect to the database */
		dbconnect(conninfo, &conn);

		char *stm = "SELECT name FROM public.tags WHERE name LIKE '%$1%'";

//		PGresult *res;
//		res = PQexecParams(conn, stm[0], 1, NULL, &paramValues[0], NULL, NULL, 0);
		PGresult *res = PQexecParams(conn, stm, 1, NULL, paramValues, NULL, NULL, 0);

		PQprintOpt      options = {0};
		options.header          = 1;    /* Printoutput field headings and row count */
		options.align           = 1;    /* Fill align the fields */
		options.standard        = 0;    /* Old brain dead format */
		options.html3           = 0;    /* Output HTML tables */
		options.expanded        = 0;    /* Expand tables */
		options.pager           = 0;    /* Use pager for output if needed */
		options.fieldSep        = "|";  /* Field sparator */
		options.tableOpt        = 0;    /* Attributes for HTML table element */
		options.caption = 0;    /* HTML table caption */

		if (PQresultStatus(res) != PGRES_TUPLES_OK) {
			fprintf(stderr, "%s", PQerrorMessage(conn));
			PQclear(res);
			exit_cleanly(conn);
		}

		PQprint(stdout, res, &options);
		PQclear(res);
		PQfinish(conn);
		return(0);
	}

	if (about) {
		AGabout();
		return(0);
	}

	if (help) {
		AGhelp();
	}

	if (ver) {
		AGversion(0.3);
		return(0);
	}

	if (wiki) {
		fprintf(stderr, "This command has not been implemented.\n");
		return(1);
	}
	
	if (rm) {
		fprintf(stderr, "This command has not been implemented.\n");
		return(1);
	}

	if (del) {
		fprintf(stderr, "This command has not been implemented.\n");
		return(1);
	}
	
	if (rmwiki) {
		fprintf(stderr, "This command has not been implemented.\n");
		return(1);
	}

	if (rmtag) {
		fprintf(stderr, "This command has not been implemented.\n");
		return(1);
	}

	if (import) {
		fprintf(stderr, "This command has not been implemented.\n");
		return(1);
	}

	if (mkwiki) {
		fprintf(stderr, "This command has not been implemented.\n");
		return(1);
	}

	if (mktag) {
		fprintf(stderr, "This command has not been implemented.\n");
		return(1);
	}

	if (ed) {
		fprintf(stderr, "This command has not been implemented.\n");
		return(1);
	}

	if (edwiki) {
		fprintf(stderr, "This command has not been implemented.\n");
		return(1);
	}

	if (edtag) {
		fprintf(stderr, "This command has not been implemented.\n");
		return(1);
	}

	if (view) {
		fprintf(stderr, "This command has not been implemented.\n");
		return(1);
	}

	if (favs) {
		fprintf(stderr, "This command has not been implemented.\n");
		return(1);
	}

	if (fav) {
		fprintf(stderr, "This command has not been implemented.\n");
		return(1);
	}

	if (unfav) {
		fprintf(stderr, "This command has not been implemented.\n");
		return(1);
	}

	return(0);
}
