#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define __CMD_INTERNAL
#include "cmd.h"
#include "config.h"
#include "print_utils.h"

static int INDENT_SIZE = 2;
static int FIRST_COLUMN_LEN = 20;
static int WRAP_AT = 80;

static void print_option(boption *option);
static void print_entry(char *first, char *second);
static void new_line_second_col(void);

static void print_help_usage(bcmd_context *context);
static _Bool print_help_description(bcmd_context *context);
static _Bool print_command_description(bcmd_context *context);
static _Bool print_option_description(bcmd_context *context);
static _Bool print_help_epilog(bcmd_context *context);

void bcmd_context_print_help(bcmd_context *context)
{
	if (!context)
		return;

	print_help_usage(context);
	putchar('\n');

	if (print_help_description(context))
		putchar('\n');
	putchar('\n');

	if (print_command_description(context))
		putchar('\n');

	if (print_option_description(context))
		putchar('\n');

	if (print_help_epilog(context))
		putchar('\n');
}

void print_help_usage(bcmd_context *context)
{
	if (context->usage)
		printf("Usage: %s", context->usage);
	else
		printf("Usage: %s [OPTION...] {COMMAND}", context->name);
}

_Bool print_help_description(bcmd_context *context)
{
	if (context->description) {
		print_wrapped(context->description, WRAP_AT, NULL);
		return true;
	}
	return false;
}

_Bool print_command_description(bcmd_context *context)
{
	int len = bcommand_len(context->commands);

	if (len == 0)
		return false;

	printf("Commands:\n");
	for (int i = 0; i < len; i++)
		print_entry(context->commands[i].name, context->commands[i].tip);
	return true;

}

_Bool print_option_description(bcmd_context *context)
{
	int len = boption_len(context->options);
	boption stock_help = { HELP_LONG, HELP_SHORT, "Show this help message",
		ARG_NONE, NULL, NULL };

	if (len == 0 && !context->handle_help)
		return false;

	printf("Options:\n");
	for (int i = 0; i < len; i++)
		print_option(&(context->options[i]));

	if (context->handle_help)
		print_option(&stock_help);

	return true;
}

void print_option(boption *option) {
	char opt[STRLEN];

	memset(opt, '\0', sizeof(opt));
	if (option->short_name != '\0')
		snprintf(opt, STRLEN, "-%c, --", option->short_name);
	else
		strcat(opt, "     --");

	if (option->long_name != NULL)
		strcat(opt, option->long_name);
	if (option->type != ARG_NONE) {
		strcat(opt, "=");
		if (option->arg_name != NULL)
			strcat(opt, option->arg_name);
		else
			strcat(opt, "ARG");
	}

	print_entry(opt, option->tip);
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

_Bool print_help_epilog(bcmd_context *context)
{
	if (context->epilog) {
		print_wrapped(context->epilog, WRAP_AT, NULL);
		return true;
	}
	return false;
}
