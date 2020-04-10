#include <stdlib.h>

#include "structures/global_defs.h"

_Bool default_equals(void *first, void *second)
{
	return first == second;
}

int default_compare(void *first, void *second)
{
	if (first == second)
		return 0;
	else if (first < second)
		return -1;
	else
		return 1;
}

void default_obj(void *obj)
{
	free(obj);
}
