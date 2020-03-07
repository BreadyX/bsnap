#include <stdlib.h>

#include "cmd.h"
extern _Bool command_is_null(b_command command);
extern _Bool option_is_null(b_option option);

int command_len(const b_command *commands)
{
	int i;
	
	if (!commands)
		return 0;
	
	for (i = 0; !command_is_null(commands[i]); i++)
		;
	return i;
}

b_command *cat_commands(const b_command *first, const b_command *second)
{
	int old_len, new_len, total_len;
	b_command *new_commands = NULL;
	
	if (!first || !second)
		return NULL;
	
	old_len = command_len(first);
	new_len = command_len(second);
	total_len = old_len + new_len + 1;
	
	new_commands = calloc(total_len, sizeof(b_command));
	if (!new_commands)
		return NULL;

	memmove(new_commands, first, old_len * sizeof(b_command));
	memmove(&new_commands[old_len], second, new_len * sizeof(b_command));

	return new_commands;
}

int option_len(const b_option *options)
{
	int i;
	
	if (!options)
		return 0;
	
	for (i = 0; !option_is_null(options[i]); i++)
		;
	return i;
}

b_option *cat_options(const b_option *first, const b_option *second)
{
	int old_len, new_len, total_len;
	b_option *new_options = NULL;
	
	if (!first || !second)
		return NULL;
	
	old_len = option_len(first);
	new_len = option_len(second);
	total_len = old_len + new_len + 1;
	
	new_options = calloc(total_len, sizeof(b_option));
	if (!new_options)
		return NULL;

	memmove(new_options, first, old_len * sizeof(b_option));
	memmove(&new_options[old_len], second, new_len * sizeof(b_option));

	return new_options;
}
