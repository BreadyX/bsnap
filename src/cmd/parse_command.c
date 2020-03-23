#include <stdio.h>
#include <string.h>

#define __CMD_INTERNAL
#include "cmd.h"

static cmd_result find_cmd(int argc, char **argv, char **cmd_str, int *i);
static cmd_result switch_commands(char *command_str, bcommand *cmds,
						          bcommand **found);
static void print_error(char *name, char *command_str, cmd_result status);

cmd_result bcmd_context_getc(bcmd_context *context, int *argc, char **argv,
							 bcommand *found)
{
	if (!context)
		return COMMAND_INVALID;

	char *command_str = NULL;
	int i = 0;
	bcommand *commands = context->commands,
			  *command;
	cmd_result status = COMMAND_SUCCESS;

	status = find_cmd(*argc, argv, &command_str, &i);
	if (status != COMMAND_SUCCESS)
		goto err;

	status = switch_commands(command_str, commands, &command);
	if (status != COMMAND_SUCCESS)
		goto err;

	move_to_last(i, *argc, argv);
	argv[--(*argc)] = NULL;
	memcpy(found, command, sizeof(bcommand));
	return COMMAND_SUCCESS;

err:
	if (context->print_errors)
		print_error(context->name, command_str, status);
	return status;
}

cmd_result find_cmd(int argc, char **argv, char **cmd_str, int *i)
{
	for (*i = 1; *i < argc; (*i)++) {
		if (argv[*i][0] != '-') {
			*cmd_str = argv[*i];
			return COMMAND_SUCCESS;
		}
	}
	return COMMAND_MISSING;
}


cmd_result switch_commands(char *command_str, bcommand *cmds, bcommand **found)
{
	int len = bcommand_len(cmds);
	for (int i = 0; i < len; i++) {
		if (strcmp(command_str, cmds[i].name) == 0) {
			*found = &cmds[i];
			return COMMAND_SUCCESS;
		}
	}
	return COMMAND_INVALID;
}

void print_error(char *name, char *command_str, cmd_result status)
{
	switch (status) {
		case COMMAND_INVALID:
			fprintf(stderr, "%s: Command %s is invalid\n", name, command_str);
			break;
		case COMMAND_MISSING: break;
		case COMMAND_SUCCESS: break;
	}
}
