#include <stdio.h>

#define __CMD_INTERNAL
#include "cmd.h"
#include "config.h"

static int INDENT_SIZE = 2;
static int FIRST_COLUMN_LEN = 20;
static int WRAP_AT = 80;

static void print_line(char *first, char *second);

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
		print_line(context->commands[i].name, context->commands[i].tip);
}

void print_line(char *first, char *second)
{
	int first_len = strlen(first);
	int second_len = strlen(second);
	int remaining = WRAP_AT - (INDENT_SIZE + FIRST_COLUMN_LEN);

	// indent
	for (int i = 0; i < INDENT_SIZE; i++)
		putchar(' ');

	// first
	printf("%s", first);
	if (first_len > FIRST_COLUMN_LEN) {
		putchar('\n');
		for (int i = 0; i < FIRST_COLUMN_LEN + INDENT_SIZE; i++)
			putchar(' ');
	} else
		for (int i = 0; i < FIRST_COLUMN_LEN - first_len; i++)
			putchar(' ');

	// second
	if (second_len > remaining) {
		for(int i = 0; i < remaining; i++)
			putchar(second[i]);
		putchar('\n');

		// TODO: pretty linebreak
		for (int base = remaining; base < second_len; base += remaining) {
			for (int i = 0; i < FIRST_COLUMN_LEN + INDENT_SIZE; i++)
				putchar(' ');
			for(int i = 0; i < remaining; i++) {
				if (base + i < second_len)
					putchar(second[base + i]);
			}
			putchar('\n');
		}
	} else
		printf("%s\n", second);
}
