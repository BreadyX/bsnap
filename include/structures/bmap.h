#ifndef __STRUCTURES_MAP_H__
#define __STRUCTURES_MAP_H__

#include "./global_defs.h"

#define DEFAULT_START_SIZE  13
#define DEFAULT_LOAD_FACTOR 0.65

typedef struct bmap_s bmap;
typedef struct bmap_iter_s bmap_iter;

bmap  *bmap_create(int start_size, double load_factor);
int    bmap_size(bmap *map);
void  *bmap_insert(bmap *map, const char *key, void *value);
void  *bmap_get(bmap *map, const char *key);
_Bool  bmap_contains(bmap *map, const char *key);
char  *bmap_find_key(bmap *map, void *item, equals eq);
void  *bmap_remove(bmap *map, const char *key);
void   bmap_clear(bmap *map);
void   bmap_destroy(bmap **map_ptr);

bmap_iter *bmap_iter_create(bmap *map);
void      *bmap_iter_next(bmap_iter *iter);
_Bool      bmap_iter_has_next(bmap_iter *iter);
_Bool      bmap_iter_is_done(bmap_iter *iter);
void       bmap_iter_reset(bmap_iter *iter);
void       bmap_iter_destroy(bmap_iter **iter_ptr);

#endif
