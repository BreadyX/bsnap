#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define __CMD_INTERNAL
#include "cmd.h"
#include "config.h"
#include "print_utils.h"

static int INDENT_SIZE = 2;
static int FIRST_COLUMN_LEN = 20;
static int WRAP_AT = 80;

static void print_entry(char *first, char *second);
static void new_line_second_col(void);

void print_help_usage(b_cmd_context *context)
{
	if (context->usage)
		printf("%s", context->usage);
	else
		printf("%s: [OPTION...] {COMMAND}", context->name);
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

void print_option_description(b_cmd_context *context)
{
	char opt[STRLEN];
	b_option *cur_opt;

	printf("Options:\n");
	for (int i = 0; i < option_len(context->options); i++) {
		cur_opt = &(context->options[i]);

		memset(opt, '\0', sizeof(opt));
		if (cur_opt->short_name != '\0')
			snprintf(opt, STRLEN, "-%c, --", cur_opt->short_name);
		else
			strcat(opt, "     --");

		if (cur_opt->long_name != NULL)
			strcat(opt, cur_opt->long_name);
		if (cur_opt->type != ARG_NONE) {
			strcat(opt, "=");
			if (cur_opt->arg_name != NULL)
				strcat(opt, cur_opt->arg_name);
			else
				strcat(opt, "ARG");
		}

		print_entry(opt, context->options[i].tip);
	}
}

void print_help_epilog(b_cmd_context *context)
{
	if (context->epilog)
		print_wrapped(context->epilog, WRAP_AT, NULL);
}


void print_help_complete(b_cmd_context *context)
{
	print_help_usage(context);
	putchar('\n');

	print_help_description(context);
	putchar('\n');
	putchar('\n');

	print_command_description(context);
	putchar('\n');

	print_option_description(context);
	putchar('\n');

	print_help_epilog(context);
	putchar('\n');
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

	if (second_len > second_size) {
		print_wrapped(second, second_size, new_line_second_col);
		putchar('\n');
	} else
		printf("%s\n", second);
}

void new_line_second_col(void)
{
	for (int i = 0; i < FIRST_COLUMN_LEN + INDENT_SIZE; i++)
		putchar(' ');
}
