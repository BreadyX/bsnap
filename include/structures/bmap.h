#ifndef __STRUCTURES_MAP_H__
#define __STRUCTURES_MAP_H__

#include "./global_defs.h"

#define BMAP_DEFAULT_START_SIZE  13
#define BMAP_DEFAULT_LOAD_FACTOR 0.65
#define BMAP_DEFAULT_HASH        default_hash_func

#define BMAP_DELETE_OBJS    0x01
#define BMAP_DEFAULT_FLAGS  0x01

typedef struct bmap_s bmap;
typedef struct bmap_iter_s bmap_iter;
typedef unsigned long (*hash_func)(const char*);

unsigned long default_hash_func(const char *key);

bmap  *bmap_create(int start_size, double load_factor, unsigned char flags,
		           hash_func hash, delete_obj deleter);
int    bmap_size(bmap *map);
void  *bmap_insert(bmap *map, const char *key, void *value);
void  *bmap_get(bmap *map, const char *key);
_Bool  bmap_contains(bmap *map, const char *key);
char  *bmap_find_key(bmap *map, void *item, equals eq);
void  *bmap_remove(bmap *map, const char *key);
void   bmap_clear(bmap *map);
void   bmap_destroy(bmap *map);

inline void bmap_destroyer(void *ptr) { bmap_destroy(ptr); }

bmap_iter *bmap_iter_create(bmap *map);
void      *bmap_iter_next(bmap_iter *iter);
_Bool      bmap_iter_has_next(bmap_iter *iter);
_Bool      bmap_iter_is_done(bmap_iter *iter);
void       bmap_iter_reset(bmap_iter *iter);
void       bmap_iter_destroy(bmap_iter *iter_ptr);

inline void bmap_iter_destroyer(void *ptr) { bmap_iter_destroy(ptr); }

#endif
