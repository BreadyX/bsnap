#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "config.h.in"
#include "print_utils.h"

void print_wrapped(char *text, int limit, void (indent)(void))
{
	int text_len = strlen(text);
	int text_i, cur_word_i;
	int remaining = limit;
	char word[STRLEN];

	for(text_i = cur_word_i = 0; text_i <= text_len; text_i++) {
		if (isspace(text[text_i]) || text[text_i] == '\0') {
			word[cur_word_i] = '\0';
			cur_word_i = 0;
			if (text_i > remaining || text[text_i] == '\n') {
				remaining += limit;
				if (text[text_i] != '\n')
					putchar('\n');
				if (indent)
					(*indent)();
			}
			printf("%s%c", word, text[text_i]);
		} else {
			if (cur_word_i < STRLEN)
				word[cur_word_i] = text[text_i];
			cur_word_i++;
		}
	}
}
