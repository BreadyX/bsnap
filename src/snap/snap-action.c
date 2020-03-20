#include <stdio.h>
#include <stdbool.h>
#include <errno.h>

#include "config.h"
#include "cmd.h"
#include "snap.h"

bool opt_help;

static b_option snap_option[] = {
	{ "help", 'h', "Print this dialog", ARG_NONE, &opt_help, NULL },
	{ 0 }
};

static void do_snap();

int snap_callback(int argc, char **argv)
{
	b_cmd_context *snap_context;
	int status;

	snap_context = new_context(NAME" snap", true);
	if (!snap_context)
		return errno;

	push_options(snap_context, snap_option);

	printf("Snap action\n");
	status = parse_options(snap_context, &argc, argv);
	switch(status) {
		case OPT_SUCCESS:
			do_snap(snap_context);
		default:
			return status;
	}
}

void do_snap(b_cmd_context *context)
{
	if (opt_help)
		print_help_complete(context);
}
