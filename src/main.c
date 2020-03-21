#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

#include "main.h"
#include "config.h"
#include "cmd.h"

#include "commands/snap.h"
// TODO: other actions

static b_command commands[] = {
	{ "snap", "Create a new snapshot", snap_callback },
	/* { "restore", "restore", NULL }, */
	/* { "list", "list", NULL }, */
	/* { "show", "show", NULL }, */
	{ 0 }
};

bool opt_version;
static b_option base_options[] = {
	{ "version", 'V', "Print info about version", ARG_NONE, &opt_version, NULL },
	{ 0 }
};

b_option global_options[] = {
	{ "verbose", 'v', "Be verbose", ARG_NONE, &BE_VERBOSE, NULL },
	{ 0 }
};

static void do_base(b_cmd_context *context);

int main(int argc, char **argv)
{
	b_cmd_context *main_context;
	b_command found_command = {0};

	int status;

	errno = 0;
	main_context = new_context(NAME);

	if (!main_context)
		return errno;

	push_commands(main_context, commands);

	push_options(main_context, global_options);
	push_options(main_context, base_options);

	set_description(main_context, PRG_DESCRIPTION);
	set_epilog(main_context, PRG_EPILOG);

	status = extract_command(main_context, &argc, argv, &found_command);
	switch (status) {
		case COMMAND_SUCCESS:
			status = (found_command.handler)(argc, argv);
			// check for status
			return EXIT_SUCCESS;
		case COMMAND_MISSING:
			status = parse_options(main_context, &argc, argv);
			if (status != OPT_SUCCESS) {
				print_help(main_context);
				return EXIT_FAILURE;
			}
			do_base(main_context);
			return EXIT_SUCCESS;
		case COMMAND_INVALID:
			print_help(main_context);
			return EXIT_FAILURE;
	}
}

void do_base(b_cmd_context *context)
{
	if (opt_version) {
		printf("%s - version %s", NAME, VERSION);
		exit(EXIT_SUCCESS);
	}
}
