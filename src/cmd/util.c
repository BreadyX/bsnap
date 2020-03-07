#define __CMD_INTERNAL
#include "cmd.h"

void move_to_last(int i, int argc, char **argv)
{
	char *tmp;
	for (i = i + 1 ; i < argc; i++) {
		tmp = argv[i];
		argv[i] = argv[i - 1];
		argv[i - 1] = tmp;
	}
}
