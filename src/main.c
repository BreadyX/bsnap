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

static bcommand commands[] = {
	{ "snap", "Create a new snapshot", snap_callback },
	/* { "restore", "restore", NULL }, */
	/* { "list", "list", NULL }, */
	/* { "show", "show", NULL }, */
	{ 0 }
};

bool opt_version;
static boption base_options[] = {
	{ "version", 'V', "Print info about version", ARG_NONE, &opt_version, NULL },
	{ 0 }
};

boption global_options[] = {
	{ "verbose", 'v', "Be verbose", ARG_NONE, &BE_VERBOSE, NULL },
	{ 0 }
};

static void do_base(bcmd_context *context);

int main(int argc, char **argv)
{
	bcmd_context *main_context;
	bcommand found_command = {0};

	int status;

	errno = 0;
	main_context = bcmd_context_new(NAME);

	if (!main_context)
		return errno;

	bcmd_context_pushc(main_context, commands);

	bcmd_context_pusho(main_context, global_options);
	bcmd_context_pusho(main_context, base_options);

	bcmd_context_set_description(main_context, PRG_DESCRIPTION);
	bcmd_context_set_epilog(main_context, PRG_EPILOG);

	status = bcmd_context_getc(main_context, &argc, argv, &found_command);
	switch (status) {
		case COMMAND_SUCCESS:
			status = (found_command.handler)(argc, argv);
			// check for status
			return EXIT_SUCCESS;
		case COMMAND_MISSING:
			status = bcmd_context_parseo(main_context, &argc, argv);
			if (status != OPT_SUCCESS) {
				bcmd_context_print_help(main_context);
				return EXIT_FAILURE;
			}
			do_base(main_context);
			return EXIT_SUCCESS;
		case COMMAND_INVALID:
			bcmd_context_print_help(main_context);
			return EXIT_FAILURE;
	}
}

void do_base(bcmd_context *context)
{
	if (opt_version) {
		printf("%s - version %s", NAME, VERSION);
		exit(EXIT_SUCCESS);
	}
}
