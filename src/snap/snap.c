#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "config.h"
#include "cmd.h"
#include "main.h"

#include "commands/snap.h"

static boption snap_option[] = {
	{ 0 }
};

int snap_callback(int argc, char **argv)
{
	bcmd_context *snap_context;
	int status;

	snap_context = bcmd_context_new(NAME" snap");
	if (!snap_context)
		goto error;

	bcmd_context_pusho(snap_context, global_options);
	bcmd_context_pusho(snap_context, snap_option);

	printf("Snap action\n");
	status = bcmd_context_parseo(snap_context, &argc, argv);
	if (status != OPT_SUCCESS)
		goto error;
	// read_config();
	// for name in arg
	//   matches += find_matching();
	// for match in matches
	//   create_snap()
	goto success;

error:
	status = EXIT_FAILURE;

success:
	return status;
}
