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

typedef struct __dynamicQuery {
	char *command;
	int nParams;
	const Oid *paramTypes;
	char *paramValues;
	const int *paramLengths;
	const int *paramFormats;
	int resultFormats;
} dynamicQuery;
