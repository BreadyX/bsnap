#include <stdlib.h>
#include <errno.h>

#define __CMD_INTERNAL
#include "cmd.h"

b_cmd_context *new_context(char *name, _Bool print_errors)
{
	b_cmd_context *to_return;

	to_return = malloc(sizeof(b_cmd_context));
	if (!to_return)
		return to_return;

	errno = 0;
	to_return->commands = calloc(1, sizeof(b_command));
	if (errno != 0)
		goto err;

	errno = 0;
	to_return->options = calloc(1, sizeof(b_option));
	if (errno != 0)
		goto err;

	errno = 0;
	to_return->name = name ? strdup(name) : calloc(1, sizeof(char));
	if (errno != 0)
		goto err;

	to_return->print_errors = print_errors;
	return to_return;

err:
	free(to_return);
	return NULL;
}

void push_commands(b_cmd_context *context, const b_command *commands)
{
	b_command *new_commands;

	if (!context)
		return;

	new_commands = cat_commands(context->commands, commands);
	if (!new_commands)
		return;
	free(context->commands);
	context->commands = new_commands;
}

void clear_commands(b_cmd_context *context)
{
	b_command *new_commands;
	if (!context)
		return;

	new_commands = calloc(1, sizeof(b_command));
	if (!new_commands)
		return;

	free(context->commands);
	context->commands = new_commands;
}

void push_options(b_cmd_context *context, const b_option *options)
{
	b_option *new_options;

	if (!context)
		return;

	new_options = cat_options(context->options, options);
	if (!new_options)
		return;
	free(context->options);
	context->options = new_options;
}

void clear_options(b_cmd_context *context)
{
	b_option *new_options;
	if (!context)
		return;

	new_options = calloc(1, sizeof(b_command));
	if (!new_options)
		return;

	free(context->options);
	context->options = new_options;
}

void set_name(b_cmd_context *context, char *name)
{
	char *new_name = NULL;

	if (!name || !context)
		return;

	new_name = strdup(name);
	if (!new_name)
		return;

	free(context->name);
	context->name = new_name;
}

void set_print_errors(b_cmd_context *context, _Bool print_errors)
{
	if (!context)
		return;

	context->print_errors = print_errors;
}

void delete_context(b_cmd_context **context)
{
	if (!context || !(*context))
		return;

	free((*context)->name);
	free((*context)->options);
	free((*context)->commands);
	free(*context);

	*context = NULL;
}
