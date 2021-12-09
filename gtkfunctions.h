/********************************************************
*	commands.h: Provide functions for every		*
*	command in Atsugami				*
*							*
*	Author:	Nate Morrison.				*
*							*
*	Purpose: Taggable command line image manager	*
*	with build-in commands.				*
*							*
*	Usage: Call functions as usual.			*
********************************************************/
#include <fcntl.h>
#include <libpq-fe.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <unistd.h>

extern void exit_nicely(PGconn *conn) {
	PQfinish(conn);
	exit(1);
}

extern int dbconnect(char *conninfo, PGconn **conn) {
	*conn = PQconnectdb(conninfo);
	if (PQstatus(*conn) != CONNECTION_OK) {
		fprintf(stderr, "%s", PQerrorMessage(*conn));
		return(1);
	}
	if (PQstatus(*conn) == CONNECTION_OK) {
		printf("Successfully connected to the database.\n");
	}
	return(0);
}

typedef struct __dynamicQuery {
	char *command;
	int nParams;
	const Oid *paramTypes;
	char *paramValues;
	const int *paramLengths;
	const int *paramFormats;
	int resultFormats;
} dynamicQuery;

/*extern int dynamicQueryParams(PGconn *conn,
			char *command,
			int nParams,
			const Oid *paramTypes,
			char *paramValues,
			const int *paramLengths,
			const int *paramFormats,
			int resultFormat); */

extern int query(PGresult *res, char *conninfo, PGconn **conn) {
	*conn = PQconnectdb(conninfo);
	PQprintOpt	options = {0};
	options.header		= 1;	/* Printoutput field headings and row count */
	options.align		= 1;	/* Fill align the fields */
	options.standard	= 0;	/* Old brain dead format */
	options.html3		= 0;	/* Output HTML tables */
	options.expanded	= 0;	/* Expand tables */
	options.pager		= 0;	/* Use pager for output if needed */
	options.fieldSep	= "|";	/* Field sparator */
	options.tableOpt	= 0;	/* Attributes for HTML table element */
	options.caption	= 0;	/* HTML table caption */
//	options.*fieldName	= 1;	/* Null-terminated array of replacement field names */

	if (PQresultStatus(res) != PGRES_TUPLES_OK) {
		fprintf(stderr, "%s", PQerrorMessage(*conn));
		PQclear(res);
		exit_nicely(*conn);
	}

	PQprint(stdout, res, &options);
	PQclear(res);
	return(0);
}

extern int AGabout() {
	printf("Atsugami 0.3\n"
		"Copyright (C) 2021 Nate Morrison\n"
		"License: ...\n"
		"This is software is FREE SOFTWARE WITH ABSOLUTELY NO WARRANTY.\n"
		);
	return(0);
}

extern int AGhelp() {
	printf("Usage: atsugami [command]...\n"
		"Command-line image tagger and manager.\n");
	printf("\nA tag is:\n"
		"	1. Artist\n"
		"	2. Copyright (i.e. a media franchise)\n"
		"	3. Character\n"
		"	4. Metadata (e.g. photograph, screenshot)\n\n");
	printf("Commands:\n"
		"	ls			List all files in the database.\n"
		"	lstag			List all tags in the database.\n"
		"	find		[tag]	Search for files which have the specified tags.\n"
		"	wiki		[tag]	Show information about a tag.\n"
		"	rm		[UUID]	Delete a file from the database. This DOES NOT delete it from your drive.\n"
		"	rm wiki		[tag]	Delete a wiki page.\n"
		"	rmtag		[tag]	Deletes the tag from the database and removes the tag from all files with it. Does not affect the image.\n"
		"	import		[file]	Import a file from your computer to the database. The image will be moved to ~/Pictures and renamed\n				to its UUID while preserving its extension.\n"
		"	mkwiki		[tag]	Create a wiki page.\n"
		"	mktag		[tag]	Create a tag.\n"
		"	edit		[UUID]	Edit the metadata of a file.\n"
		"	editwiki	[tag]	Edit a wiki page.\n"
		"	edittag		[tag]	Rename a tag; this will rename its associated wiki page (if one exists).\n"
		"	favourites	[UUID]	List your favourite/bookmarked files.\n"
		"	favourite	[UUID]	Add a file to your favourites list.\n"
		"	unfavourite	[UUID]	Remove a file from your favourites list.\n"
		"\n"
		"	version		Show the verionsion of the program.\n"
		"	help		Show this menu.\n");
	return(0);
}

extern int AGversion(float version) {
	printf("Atsugami version %f\n", version);
	return(0);
}