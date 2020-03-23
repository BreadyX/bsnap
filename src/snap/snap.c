#include <stdio.h>
#include <stdbool.h>
#include <errno.h>

#include "config.h"
#include "cmd.h"
#include "main.h"
#include "commands/snap.h"

bool opt_help;

static boption snap_option[] = {
	{ 0 }
};

int snap_callback(int argc, char **argv)
{
	bcmd_context *snap_context;
	int status;

	snap_context = bcmd_context_new(NAME" snap");
	if (!snap_context)
		return errno;

	bcmd_context_pusho(snap_context, global_options);
	bcmd_context_pusho(snap_context, snap_option);

	printf("Snap action\n");
	status = bcmd_context_parseo(snap_context, &argc, argv);
	switch(status) {
		case OPT_SUCCESS:
		default:
			return status;
	}
}
