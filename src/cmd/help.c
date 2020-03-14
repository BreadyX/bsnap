#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define __CMD_INTERNAL
#include "cmd.h"
#include "config.h"

static int INDENT_SIZE = 2;
static int FIRST_COLUMN_LEN = 20;
static int WRAP_AT = 80;

static void print_wrapped(char *text, int limit, void (indent)(void));

static void print_entry(char *first, char *second);
static void new_line_second_col(void);

void print_help_usage(b_cmd_context *context)
{
	if (context->usage)
		printf("%s\n", context->usage);
	else
		printf("%s: [OPTION...] {COMMAND}\n", context->name);
}

void print_help_description(b_cmd_context *context)
{
	if (context->description)
		print_wrapped(context->description, WRAP_AT, NULL);
}

void print_command_description(b_cmd_context *context)
{
	printf("Commands:\n");
	for (int i = 0; !command_is_null(context->commands[i]); i++)
		print_entry(context->commands[i].name, context->commands[i].tip);
}

void print_entry(char *first, char *second)
{
	int first_len = strlen(first);
	int second_len = strlen(second);
	int second_size = WRAP_AT - (INDENT_SIZE + FIRST_COLUMN_LEN);
	
	for (int i = 0; i < INDENT_SIZE; i++)
		putchar(' ');

	printf("%s", first);
	if (first_len > FIRST_COLUMN_LEN) {
		putchar('\n');
		new_line_second_col();
	} else
		for (int i = 0; i < FIRST_COLUMN_LEN - first_len; i++)
			putchar(' ');

	if (second_len > second_size)
		print_wrapped(second, second_size, new_line_second_col);
	else
		printf("%s\n", second);
}

void print_wrapped(char *text, int limit, void (indent)(void))
{
	int text_len = strlen(text);
	int text_i, cur_word_i;
	int remaining = limit;
	char word[STRLEN];

	for(text_i = cur_word_i = 0; text_i <= text_len; text_i++) {
		if (isblank(text[text_i]) || text[text_i] == '\0') {
			word[cur_word_i] = '\0';
			cur_word_i = 0;
			if (text_i > remaining) {
				remaining += limit;
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
	putchar('\n');
}

void new_line_second_col(void)
{
	for (int i = 0; i < FIRST_COLUMN_LEN + INDENT_SIZE; i++)
		putchar(' ');
}
