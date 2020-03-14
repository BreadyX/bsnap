#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define __CMD_INTERNAL
#include "cmd.h"
#include "config.h"

static int INDENT_SIZE = 2;
static int FIRST_COLUMN_LEN = 20;
static int WRAP_AT = 80;

static void print_entry(char *first, char *second);
static void new_line_second_col(void);

void print_help_usage(b_cmd_context *context)
{
	if (!(context->usage))
		printf("%s\n", context->usage);
	else
		printf("%s: [OPTION...] {COMMAND}\n", context->name);
}

void print_help_description(b_cmd_context *context)
{
	if (!(context->usage))
		printf("%s\n", context->usage);
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
	
	int second_i, cur_word_i;
	int second_size = WRAP_AT - (INDENT_SIZE + FIRST_COLUMN_LEN);
	int remaining = second_size;
	char word[STRLEN];
	
	// indent
	for (int i = 0; i < INDENT_SIZE; i++)
		putchar(' ');

	// first
	printf("%s", first);
	if (first_len > FIRST_COLUMN_LEN) {
		new_line_second_col();
	} else
		for (int i = 0; i < FIRST_COLUMN_LEN - first_len; i++)
			putchar(' ');

	// second
	if (second_len > remaining) {
		for(second_i = cur_word_i = 0; second_i <= second_len; second_i++) {
			if (isblank(second[second_i]) || second[second_i] == '\0') {
				word[cur_word_i] = '\0';
				cur_word_i = 0;
				if (second_i > remaining) {
					remaining += second_size;
					new_line_second_col();
				}
				printf("%s%c", word, second[second_i]);
			} else {
				if (cur_word_i < STRLEN)
					word[cur_word_i] = second[second_i];
				cur_word_i++;
			}
		}
		putchar('\n');
	} else
		printf("%s\n", second);
}

void new_line_second_col(void)
{
	putchar('\n');
	for (int i = 0; i < FIRST_COLUMN_LEN + INDENT_SIZE; i++)
		putchar(' ');
}
