#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "structures/bmap.h"

struct bmap_s {
	double load_factor;
	int threshold;

	int size;
	int key_count;
	int used_count;
	int mod_count;

	char **keys;
	void **values;
};

struct bmap_iter_s {
	bmap *map;
	int mod_count;
	int offset;
};

static inline int P(int i) { return i++; } // NON NORMALIZED
static inline int normalize(int i, int size) { return i % size; }

static unsigned long hash_key(const char *s); // NON NORMALIZED
static int resize_map(bmap *map);

static int bmap_iter_find_next(bmap *map, int start);

static char *TOMBSTONE = "f";

/*
 * bmap
 */
int bmap_size(bmap *map) { return map->key_count; }

bmap *bmap_create(int start_size, double load_factor) {
	start_size = start_size < DEFAULT_START_SIZE ? DEFAULT_START_SIZE : start_size;
	if (load_factor <= 0 || load_factor >= 1)
		load_factor = DEFAULT_LOAD_FACTOR;

	bmap *to_ret = malloc(sizeof(bmap));
	if (!to_ret)
		goto first_error;

	to_ret->load_factor = load_factor;
	to_ret->size = start_size;
	to_ret->threshold = start_size * load_factor;
	to_ret->key_count = 0;
	to_ret->used_count = 0;
	to_ret->mod_count = 0;

	to_ret->keys = calloc(start_size, sizeof(char *));
	if (!to_ret->keys)
		goto second_error;

	to_ret->values = calloc(start_size, sizeof(void *));
	if (!to_ret->values)
		goto second_error;

	return to_ret;

second_error:
	free(to_ret);
	// fall-through
first_error:
	return NULL;
}

void *bmap_insert(bmap *map, const char *key, void *value)
{
	void *to_ret = NULL;
	char *new_key;
	int off;

	if (!map || !key) {
		errno = EINVAL;
		return to_ret;
	}

	if (map->used_count >= map->threshold) {
		if (resize_map(map) == EXIT_FAILURE) {
			errno = ENOMEM;
			return NULL;
		}
	}

	new_key = strdup(key);
	if (!new_key)
		return to_ret;

	off = normalize(hash_key(new_key), map->size);
	for (int i = off, j = -1, x = 1; ; i = normalize(off + P(x++), map->size)) {
		if (map->keys[i] == TOMBSTONE) {
			if (j == -1) // keep track of the first tombstone found
				j = i;
		} else if (map->keys[i] != NULL) { // UPDATE
			if (strcmp(map->keys[i], new_key) == 0) {
				to_ret = map->values[i];
				if (j == -1) { // no tombstone found
					map->values[i] = value;
				} else { // swap found with tombstone
					map->keys[j] = map->keys[i];
					map->keys[i] = TOMBSTONE;
					map->values[i] = NULL;
					map->values[j] = value;
					free(new_key);
				}
				map->mod_count++;
				return to_ret;
			}
		} else { // INSERT
			if (j == -1) { // no tombstone found
				map->keys[i] = new_key;
				map->values[i] = value;

				map->key_count++;
				map->used_count++;
			} else { // use tombstone
				map->keys[j] = new_key;
				map->values[j] = value;

				map->key_count++;
			}
			map->mod_count++;
			return to_ret;
		}
	}
}

int resize_map(bmap *map)
{
	char **old_keys = map->keys,
	     **new_keys = NULL;
	void **old_values = map->values,
		 **new_values = NULL;
	int old_size = map->size;
	int new_size = (map->size * 2) + 1;
	int new_threshold = new_size * map->load_factor;

	new_keys = calloc(new_size, sizeof(char*));
	if (!new_keys)
		return EXIT_FAILURE;

	new_values = calloc(new_size, sizeof(void*));
	if (!new_keys)
		return EXIT_FAILURE;

	// UPDATE VALUES
	map->keys = new_keys;
	map->values = new_values;
	map->key_count = 0;
	map->used_count = 0;
	map->size = new_size;
	map->threshold = new_threshold;

	// REINSERT VALUES
	for (int i = 0; i < old_size; i++) {
		if (old_keys[i] != NULL && old_keys[i] != TOMBSTONE) {
			bmap_insert(map, old_keys[i], old_values[i]);
			free(old_keys[i]);
		}
	}
	free(old_keys);
	free(old_values);

	return EXIT_SUCCESS;
}

unsigned long hash_key(const char *s)
{
	// implementation of the PJW hash algorithm
	const unsigned char *u_s = (unsigned char*) s;
    unsigned long h = 0,
				  high;
	size_t bits = sizeof(unsigned long);

	while (*u_s) {
		h = (h << (bits / 4)) + *u_s++;
		high = h & 0xF0000000;
		if (high != 0) {
			h ^= high >> ((bits * 3) / 4);
			h &= ~high;
		}
	}
    return h;
}

void *bmap_get(bmap *map, const char *key)
{
	if (!map || !key) {
		errno = EINVAL;
		return NULL;
	}

	int off = normalize(hash_key(key), map->size);

	for (int i = off, j = -1, x = 1; ; i = normalize(off + P(x++), map->size)) {
		if (map->keys[i] == TOMBSTONE) {
			if (j == -1)
				j = i;
		} else if (map->keys[i] != NULL) {
			if (strcmp(map->keys[i], key) == 0) {
				if (j != -1) {
					map->keys[j] = map->keys[i];
					map->keys[i] = TOMBSTONE;
					map->values[j] = map->values[i];
					map->values[i] = NULL;
				} else
					return map->values[i];
			}
		} else {
			errno = EINVAL;
			return NULL;
		}
	}
}

_Bool bmap_contains(bmap *map, const char *key)
{
	bmap_get(map, key);
	return errno != EINVAL;
}

char *bmap_find_key(bmap *map, void *item, equals eq)
{
	if (!map) {
		errno = EINVAL;
		return NULL;
	}

	char *key = NULL;

	eq = eq ? eq : default_equals;
	for (int i = 0; i < map->size; i++) {
		if (map->keys[i] != NULL && map->keys[i] != TOMBSTONE) {
			if (eq(map->values[i], item)) {
				key = map->keys[i];
				break;
			}
		}
	}
	return key ? strdup(key) : NULL;
}

void *bmap_remove(bmap *map, const char *key)
{
	if (!map || !key) {
		errno = EINVAL;
		return NULL;
	}

	int off = normalize(hash_key(key), map->size);
	void *to_ret;

	for (int i = off, x = 1; ; i = normalize(off + P(x++), map->size)) {
		if (map->keys[i] == NULL) {
			errno = EINVAL;
			return NULL;
		}
		if (map->keys[i] == TOMBSTONE)
			continue;
		if (strcmp(map->keys[i], key) == 0) {
			map->key_count--;
			map->mod_count++;
			free(map->keys[i]);
			map->keys[i] = TOMBSTONE;
			to_ret = map->values[i];
			map->values[i] = NULL;
			return to_ret;
		}
	}
}

void bmap_clear(bmap *map)
{
	if (!map) {
		errno = EINVAL;
		return;
	}

	for (int i = 0; i < map->size; i++) {
		if (map->keys[i] != NULL) {
			if (map->keys[i] != TOMBSTONE)
				free(map->keys[i]);
			map->keys[i] = NULL;
		}
		map->values[i] = NULL;
	}
	map->key_count = 0;
	map->used_count = 0;
	map->mod_count++;
}

void bmap_destroy(bmap **map_ptr)
{
	if (!map_ptr || !(*map_ptr)) {
		errno = EINVAL;
		return;
	}

	bmap_clear(*map_ptr);
	free((*map_ptr)->keys);
	free((*map_ptr)->values);
	free(*map_ptr);
	*map_ptr = NULL;
}

/*
 * Iter
 */
bmap_iter *bmap_iter_create(bmap *map)
{
	if (!map) {
		errno = EINVAL;
		return NULL;
	}

	bmap_iter *to_ret = malloc(sizeof(bmap_iter));

	if (to_ret) {
		to_ret->map = map;
		to_ret->mod_count = map->mod_count;
		to_ret->offset = bmap_iter_find_next(map, 0);
	}
	return to_ret;
}

int bmap_iter_find_next(bmap *map, int start)
{
	for (int i = start + 1; i < map->size; i++) {
		if (map->keys[i] != NULL && map->keys[i] != TOMBSTONE) {
			return i;
		}
	}
	return -1;
}

void *bmap_iter_next(bmap_iter *iter)
{
	if (!iter) {
		errno = EINVAL;
		return NULL;
	}
	if (iter->mod_count != iter->map->mod_count) {
		errno = EPERM;
		return NULL;
	}

	void *to_ret = iter->map->values[iter->offset];
	iter->offset = bmap_iter_find_next(iter->map, iter->offset);

	return to_ret;
}

_Bool bmap_iter_has_next(bmap_iter *iter)
{
	if (!iter) {
		errno = EINVAL;
		return 0;
	}
	if (iter->mod_count != iter->map->mod_count) {
		errno = EPERM;
		return 0;
	}

	if (iter->offset == -1 || bmap_iter_find_next(iter->map, iter->offset) == -1)
		return 0;
	return 1;
}

_Bool bmap_iter_is_done(bmap_iter *iter)
{
	if (!iter) {
		errno = EINVAL;
		return 0;
	}
	if (iter->mod_count != iter->map->mod_count) {
		errno = EPERM;
		return 0;
	}
	return iter->offset == -1;
}

void bmap_iter_reset(bmap_iter *iter)
{
	if (!iter) {
		errno = EINVAL;
		return;
	}
	iter->mod_count = iter->map->mod_count;
	iter->offset = bmap_iter_find_next(iter->map, 0);
}

void bmap_iter_destroy(bmap_iter **iter_ptr)
{
	if (!iter_ptr || !(*iter_ptr))
		return;

	free(*iter_ptr);
	*iter_ptr = NULL;
}
