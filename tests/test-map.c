#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "structures/bmap.h"
#include "structures/bmap.c"        // i am not crazy
#include "structures/global_defs.c" // i am not crazy

static inline _Bool my_eq(void *first, void *second)
{
	return *((int*)first) == *((int*)second);
}

void test_mod(bmap *map)
{
	int *item;
	char key[64];

	for (int i = 0; i < 20; i++) {
		item = malloc(sizeof(int));
		*item = i;
		snprintf(key, 64, "key%d", i);
		bmap_insert(map, key, item);
	}
	if (map->key_count != 20 || map->used_count != 20)
		exit(1);

	item = bmap_remove(map, "key1");
	if (*item != 1 || map->key_count != 19 || map->used_count != 20)
		exit(1);
	free(item);

	item = bmap_remove(map, "asdfghjkl");
	if (item != NULL || errno != EINVAL)
		exit(1);

	item = malloc(sizeof(int));
	*item = 100;
	bmap_insert(map, "key1", item);
	if (map->key_count != 20 || map->used_count != 20)
		exit(1);

	bmap_clear(map); // Note: valgrind will complain
	if (map->key_count != 0 || map->used_count != 0)
		exit(1);
}

void test_get(bmap *map)
{
	int number = 12;
	int *item;
	char key[64];

	for (int i = 0; i < 20; i++) {
		item = malloc(sizeof(int));
		*item = i;
		snprintf(key, 64, "key%d", i);
		bmap_insert(map, key, item);
	}

	item = bmap_get(map, "key10");
	if (*item != 10)
		exit(1);

	if (bmap_get(map, "asdfff") != NULL || 
	    bmap_contains(map, "asdfff") ||
	    errno != EINVAL)
		exit(1);

	strcpy(key, bmap_find_key(map, &number, my_eq));
	if (strcmp(key, "key12") != 0)
		exit(1);
}

int main(void)
{
	bmap *map = bmap_create(0, 0);
	bmap_destroy(&map);

	map = bmap_create(13, 0.65);

	test_mod(map);
	test_get(map); // Note: valgrind will complain

	bmap_destroy(&map);
	return 0;
}
