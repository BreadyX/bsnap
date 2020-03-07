#include <stdio.h>
#include "snap-action.h"

int snap_eval(int argc, char **argv)
{
	printf("argc: %d\nargv: ", argc);
	for (int i = 0; i < argc; i++)
		printf("%s ", argv[i]);
	printf("\n");

	return 0;
}
