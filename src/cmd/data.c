#include <stdlib.h>
#include <string.h>

#define __CMD_INTERNAL
#include "cmd.h"

_Bool bcommand_null(bcommand command)
{
	bcommand command_empty = {0};
	return memcmp(&command, &command_empty, sizeof(bcommand)) == 0;
}

_Bool boption_null(boption option)
{
	boption option_empty = {0};
	return memcmp(&option, &option_empty, sizeof(boption)) == 0;
}

int bcommand_len(const bcommand *commands)
{
	int i;
	
	if (!commands)
		return 0;
	
	for (i = 0; !bcommand_null(commands[i]); i++)
		;
	return i;
}

bcommand *bcommand_cat(const bcommand *first, const bcommand *second)
{
	int old_len, new_len, total_len;
	bcommand *new_commands = NULL;
	
	if (!first || !second)
		return NULL;
	
	old_len = bcommand_len(first);
	new_len = bcommand_len(second);
	total_len = old_len + new_len + 1;
	
	new_commands = calloc(total_len, sizeof(bcommand));
	if (!new_commands)
		return NULL;

	memmove(new_commands, first, old_len * sizeof(bcommand));
	memmove(&new_commands[old_len], second, new_len * sizeof(bcommand));

	return new_commands;
}

int boption_len(const boption *options)
{
	int i;
	
	if (!options)
		return 0;
	
	for (i = 0; !boption_null(options[i]); i++)
		;
	return i;
}

boption *boption_cat(const boption *first, const boption *second)
{
	int old_len, new_len, total_len;
	boption *new_options = NULL;
	
	if (!first || !second)
		return NULL;
	
	old_len = boption_len(first);
	new_len = boption_len(second);
	total_len = old_len + new_len + 1;
	
	new_options = calloc(total_len, sizeof(boption));
	if (!new_options)
		return NULL;

	memmove(new_options, first, old_len * sizeof(boption));
	memmove(&new_options[old_len], second, new_len * sizeof(boption));

	return new_options;
}
