/* lex.c
 * Copyright (c) 2021-2022 by Nate Morrison.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "atsugami.h"
#include <stdio.h>
#include <string.h>

char *input_sanitiser(char *str) {
	int iter;

	/* Remove illegal characters from str */
	iter = (strlen(str) - 1);
	while (iter > 0) {
		if (str[iter] == ('\n' | '\t' | '\v' | '*' | ','))
			for (int iter1 = iter; iter1 > 0; iter1--)
				str[iter1] = ' ';
		else
			break;
	}

	/* Remove spaces at the end of str */
	iter = (strlen(str) - 1);
	while (iter > 0) {
		if (str[iter] == ' ')
			for (int iter1 = iter; iter1 > 0; iter1--)
				str[iter1] = str[iter1 - 1];
		else
			break;
	}

	/* Remove spaces at the start of str */
	iter = 0;
	while (iter < strlen(str)) {
		if (str[iter] == ' ')
			for (int iter1 = iter; iter1 < strlen(str); iter1++)
				str[iter1] = str[iter1 + 1];
		else
			break;
	}

	return str;
}

char *space_remover(char *str) {
	/* RUN input_sanitiser() BEFORE CALLING THIS FUNCTION! *
	 * The first character in sanitised_str is guranteed to be non-whitespace */
	char *sanitised_str = input_sanitiser(str);

	return sanitised_str;
}

extern char *search_input_sanitiser(char *str) {
	char *query = input_sanitiser(str);
	
	return query;
}

extern char *tag_input_sanitiser(char *str) {
	/* A tag cannot contain any of the characters automatically removed by input_sanitiser(),
	 * nor can the following apply:
	 * Starts with '~' or '-'
	 * Consecutive "__"
	 */
	char *sanitised_str = input_sanitiser(str);

	return sanitised_str;
}

extern long word_count(const char *str) {
	long wc;
	wc = 0;

	return wc;
}
