#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>

#define __CMD_INTERNAL
#include "cmd.h"

static void set_str(char **to_set, char *new);

bcmd_context *bcmd_context_create(char *name)
{
	bcmd_context *to_return;

	to_return = malloc(sizeof(bcmd_context));
	if (!to_return)
		return to_return;

	errno = 0;
	to_return->commands = calloc(1, sizeof(bcommand));
	if (errno != 0)
		goto err;

	errno = 0;
	to_return->options = calloc(1, sizeof(boption));
	if (errno != 0)
		goto err;

	errno = 0;
	to_return->name = name ? strdup(name) : calloc(1, sizeof(char));
	if (errno != 0)
		goto err;

	to_return->usage = NULL;
	to_return->description = NULL;
	to_return->epilog = NULL;

	to_return->print_errors = true;
	to_return->handle_help = true;
	return to_return;

err:
	free(to_return);
	return NULL;
}

void bcmd_context_pushc(bcmd_context *context, const bcommand *commands)
{
	bcommand *new_commands;

	if (!context)
		return;

	new_commands = bcommand_cat(context->commands, commands);
	if (!new_commands)
		return;
	free(context->commands);
	context->commands = new_commands;
}

void bcmd_context_clearc(bcmd_context *context)
{
	bcommand *new_commands;
	if (!context)
		return;

	new_commands = calloc(1, sizeof(bcommand));
	if (!new_commands)
		return;

	free(context->commands);
	context->commands = new_commands;
}

void bcmd_context_pusho(bcmd_context *context, const boption *options)
{
	boption *new_options;

	if (!context)
		return;

	new_options = boption_cat(context->options, options);
	if (!new_options)
		return;
	free(context->options);
	context->options = new_options;
}

void bcmd_context_clearo(bcmd_context *context)
{
	boption *new_options;
	if (!context)
		return;

	new_options = calloc(1, sizeof(bcommand));
	if (!new_options)
		return;

	free(context->options);
	context->options = new_options;
}

void bcmd_context_set_name(bcmd_context *context, char *name)
{
	if (!name || !context)
		return;
	set_str(&(context->name), name);
}

char *bcmd_context_get_name(bcmd_context *context)
{
	return context ? strdup(context->name) : NULL;
}

void bcmd_context_set_usage(bcmd_context *context, char *usage)
{
	if (!usage || !context)
		return;
	set_str(&(context->usage), usage);
}

char *bcmd_context_get_usage(bcmd_context *context)
{
	return context && context->usage ? strdup(context->usage) : NULL;
}

void bcmd_context_set_description(bcmd_context *context, char *description)
{
	if (!description || !context)
		return;
	set_str(&(context->description), description);
}

char *bcmd_context_get_description(bcmd_context *context)
{
	return context && context->description ? strdup(context->description) : NULL;
}

void bcmd_context_set_epilog(bcmd_context *context, char *epilog)
{
	if (!epilog || !context)
		return;
	set_str(&(context->epilog), epilog);
}

char *bcmd_context_get_epilog(bcmd_context *context)
{
	return context && context->epilog ? strdup(context->epilog) : NULL;
}

void set_str(char **str, char *new_str)
{
	char *new = NULL;

	new = strdup(new_str);
	if (!new)
		return;

	free(*str);
	*str = new;
}

void bcmd_context_set_print_errors(bcmd_context *context, _Bool print_errors)
{
	if (!context)
		return;
	context->print_errors = print_errors;
}

_Bool bcmd_context_get_print_errors(bcmd_context *context)
{
	return context ? context->print_errors : false;
}

void bcmd_context_set_handle_help(bcmd_context *context, _Bool handle_help)
{
	if (!context)
		return;
	context->handle_help = handle_help;
}

_Bool bcmd_context_get_handle_help(bcmd_context *context)
{
	return context ? context->handle_help : false;
}

void bcmd_context_destroy(bcmd_context **context)
{
	if (!context || !(*context))
		return;

	free((*context)->name);
	free((*context)->usage);
	free((*context)->description);
	free((*context)->epilog);
	free((*context)->options);
	free((*context)->commands);
	free(*context);

	*context = NULL;
}
