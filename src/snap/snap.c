#include <stdio.h>
#include <stdbool.h>
#include <errno.h>

#include "config.h"
#include "cmd.h"
#include "main.h"
#include "commands/snap.h"

bool opt_help;

static b_option snap_option[] = {
	{ 0 }
};

int snap_callback(int argc, char **argv)
{
	b_cmd_context *snap_context;
	int status;

	snap_context = new_context(NAME" snap");
	if (!snap_context)
		return errno;

	push_options(snap_context, global_options);
	push_options(snap_context, snap_option);

	printf("Snap action\n");
	status = parse_options(snap_context, &argc, argv);
	switch(status) {
		case OPT_SUCCESS:
		default:
			return status;
	}
}
