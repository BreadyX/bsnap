#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

#include "config.h"
#include "cmd/cmd.h"

#include "snap/snap-action.h"
// TODO: other actions

static b_command commands[] = {
	{ "snap", "snap", snap_eval },
	{ "restore", "restore", NULL },
	{ "list", "list", NULL },
	{ "show", "show", NULL },
	{ 0 }
};

bool opt_help;
bool opt_version;
static b_option base_options[] = {
	{ "help", 'h', "Show help", ARG_NONE, &opt_help, NULL },
	{ "version", 'V', "Show version", ARG_NONE, &opt_version, NULL },
	{ 0 }
};

void do_base(void);

int main(int argc, char **argv)
{
	b_cmd_context *main_context;
	b_command found_command = {0};

	int status;

	errno = 0;
	main_context = new_context(NAME, true);
	if (!main_context)
		return errno;

	push_commands(main_context, commands);
	push_options(main_context, base_options);

	status = extract_command(main_context, &argc, argv, &found_command);
	switch (status) {
		case COMMAND_SUCCESS:
			status = (found_command.handler)(argc, argv);
			// check for status
			return EXIT_SUCCESS;
		case COMMAND_MISSING:
			status = parse_options(main_context, &argc, argv);
			if (status != OPT_SUCCESS) {
				printf("Print help\n");
				return EXIT_FAILURE;
			}
			do_base();
			return EXIT_SUCCESS;
		case COMMAND_INVALID:
			printf("Print help\n");
			return EXIT_FAILURE;
	}
}

void do_base(void)
{
	if (opt_help)
		printf("Print help\n");
	else if (opt_version)
		printf("Print version\n");
}
