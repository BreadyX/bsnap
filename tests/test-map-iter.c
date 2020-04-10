#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "structures/bmap.h"
#include "structures/bmap.c"        // I'm not crazy
#include "structures/global_defs.c" // I'm not crazy

void populate_map(bmap *map)
{
	int *item;
	char key[64];

	for (int i = 0; i < 20; i++) {
		item = malloc(sizeof(int));
		*item = i;
		snprintf(key, 64, "key%d", i);
		bmap_insert(map, key, item);
	}
}

void test(bmap_iter *iter)
{
	int *item;
	while(!bmap_iter_is_done(iter)) {
		item = bmap_iter_next(iter);
		if (!item ||
			iter->map->keys[iter->offset] == NULL ||
			errno == EINVAL ||
			errno == EPERM)
			exit(1);
	}

	bmap_iter_reset(iter);
	do {
		item = bmap_iter_next(iter);
		if (!item ||
			iter->map->keys[iter->offset] == NULL ||
			errno == EINVAL ||
			errno == EPERM)
			exit(1);
	} while(bmap_iter_has_next(iter));
	item = bmap_iter_next(iter);
	if (!bmap_iter_is_done(iter))
		exit(1);

	// invalidation test
	bmap_iter_reset(iter);
	item = malloc(sizeof(int));
	*item = 1234;
	bmap_insert(iter->map, "asdf1234", item);
	bmap_iter_next(iter);
	if (errno != EPERM)
		exit(1);
}

int main(void)
{
	bmap *map = bmap_create(0, 0);
	populate_map(map);

	bmap_iter *iter = bmap_iter_create(map);

	test(iter);

	bmap_iter_destroy(&iter); // Note: valgrind will complain
	bmap_destroy(&map);       // Note: valgrind will complain
}
